/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   abr_mpdcontroller.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:
 * File Description:     Adaptive Bitrate Streaming MPD Controller
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif

#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "di_iptuner_config.h"
#include "abr_mpdcontroller.h"

#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "abr_mpdparser.h"
#include "util_url.h"
#include "di_uart.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
#define ABR_MPD_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef ABR_MPD_DEBUG
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

#define UNUSED_PARAM(x)  ((void)x)

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/

 /*
#define ABR_MPD_CONFIG_NOT_USE_VK 0

#if ABR_MPD_CONFIG_NOT_USE_VK
#define VK_MEM_Alloc malloc
#define VK_MEM_Memset memset
#define VK_strncpy VK_strncpy
#define VK_MEM_Free free
#define VK_StrDup strdup
#endif
*/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

typedef struct ABR_MPD_NODE_s
{
	HINT32 nId; /* Segment ID */
	HINT64 range;
	HINT64 offset;
	HINT64 buffer_offset;
	HUINT64 ullBitrate;
} ABR_MPD_NODE_t;

typedef struct ABR_MPD_QUEUE_s
{
	HINT32 nCur;
	HINT32 nFront;
	HINT32 nRear;
	HINT32 nSize;
	ABR_MPD_NODE_t *nodeSet;
} ABR_MPD_QUEUE_t;

typedef struct ABR_MPD_BUFFER_s
{
	HINT32 nBufSize;
	HINT8 *pBuffer;
	ABR_MPD_QUEUE_t queue;
} ABR_MPD_BUFFER_t;

typedef struct ABR_MPD_NETSTATUS_s
{
	HINT32 nRecv;
	HUINT32 ulNetRTD;
	HUINT32 ulProcRTD;
	TIMEVAL_t tvstart;
	TIMEVAL_t tvend;
	TIMEVAL_t tvlast;
} ABR_MPD_NETSTATUS_t;

typedef struct ABR_MPD_s
{
	/* Current Info */
	HINT32 nCurPeriod;
	HINT32 nCurRepresent;
	HINT32 nCurSegment;

	HULONG ulSema;

#if 0 /* DO NOT USE TASK by ykkim */
	/* for ABR Switching Control Task */
	HULONG ulTaskId;
	HBOOL bStopABRTask;
	HBOOL bABRTaskQuit;
	HUINT32 ulInterval;
#endif

	/* for estimating bandwidth(throughput) */
	ABR_MPD_NETSTATUS_t stNetStatus[ABR_MPD_CONFIG_ABR_AVGCOUNT];
	HUINT64 ullBandwidth;
	HUINT64 ullMinBandwidth;
	HINT32 nAvgCount;
	HBOOL bNetStatusFull;

	/* for MPD Control */
	HINT8 *pBaseUrl; /* Manifest Url */
	mpdElement_Segment_t *currentSegment;
	mpdParser_t *parser;
	HBOOL bTargetChanged;
	HUINT64 ullMaxBitrate;
	HUINT64 ullMinBitrate;
	ABR_MPD_BUFFER_t buffer;
	ABR_QUEUE_t* pQueue;
	/* for ABR Controller in CANVAS */

	HINT64  nCurrentPos;
	HINT64	nStreamLength; /* Is End of Stream */
	HUINT64 ullTimeForSeek;
//	HBOOL bWaitForDecision;

	/* for MPD IO Controller */
//	HUINT64 ullTimeForSeek; /* Seek */
//	HINT64 	nStreamLength; /* Is End of Stream */
} ABR_MPD_t;

/* End typedef */

#if _________static________
#endif

/*----------------------------------------------------------------------------------------------
 *	static functions
 *---------------------------------------------------------------------------------------------*/
static inline mpdData_t *__mpdData(void *mpdData) { return (mpdData_t*)mpdData; }
static inline ABR_MPD_t *__mpd(ABR_HANDLE *hMpd) { return (ABR_MPD_t*)hMpd; }

static void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
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
/*
static ABR_ERR P_ABR_MPD_CalcBounds(ABR_MPD_t *mpd)
{
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(mpd == NULL || mpd->parser == NULL
		|| mpd->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}

	mpdData = __mpdData(mpd->parser->mpdData);


}*/

#if __________p__________
#endif

/**
 * @brief       locking abr handle
 * @param       hMpd : abr handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see         P_ABR_MPD_Unlock
 **/
ABR_ERR P_ABR_MPD_Lock(ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	VK_SEM_Get(pMpd->ulSema);
	return ABR_OK;
}

/**
 * @brief       unlocking abr handle
 * @param       hMpd : abr handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see         P_ABR_MPD_Unlock
 **/
ABR_ERR P_ABR_MPD_Unlock(ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	VK_SEM_Release(pMpd->ulSema);
	return ABR_OK;
}

/**
 * @brief       open buffer with play list queue
 * @param       mpd : structure of mpd
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/*
* used for playlist queue ("buffer" is not supported yet)
*/
static ABR_ERR P_ABR_MPD_BufferOpen(ABR_MPD_t *mpd)
{
	ABR_MPD_QUEUE_t *pQueue = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(mpd == NULL || mpd->parser == NULL
		|| mpd->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}

	mpdData = __mpdData(mpd->parser->mpdData);
	pQueue = (ABR_QUEUE_t *)VK_MEM_Alloc(sizeof(ABR_QUEUE_t));
	if(pQueue == NULL)
	{
		PrintError("%s(%d) Err> Queue is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(mpdData->ulMinBufferTime == 0)
	{
		//mpd->buffer.nBufSize = 0;
		//mpd->buffer.pBuffer = NULL;
		pQueue->nCur = 0;
		pQueue->nFront = 0;
		pQueue->nRear = 0;
		pQueue->nSize = ABR_MPD_CONST_DEFAULT_QUEUESIZE;
		pQueue->nodeSet = (ABR_MPD_NODE_t *)VK_MEM_Alloc(sizeof(ABR_MPD_NODE_t) * pQueue->nSize);
		VK_MEM_Memset(pQueue->nodeSet,0x00,sizeof(ABR_MPD_NODE_t) * pQueue->nSize);
	}
	else
	{
		//mpd->buffer.nBufSize = 0;
		//mpd->buffer.pBuffer = NULL;
		pQueue->nCur = 0;
		pQueue->nFront = 0;
		pQueue->nRear = 0;
		pQueue->nSize = ABR_MPD_CONST_DEFAULT_QUEUESIZE;
		pQueue->nodeSet = (ABR_MPD_NODE_t *)VK_MEM_Alloc(sizeof(ABR_MPD_NODE_t) * pQueue->nSize);
		VK_MEM_Memset(pQueue->nodeSet,0x00,sizeof(ABR_MPD_NODE_t) * pQueue->nSize);
	}
	mpd->pQueue = pQueue;
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       close buffer with playlist queue
 * @param       mpd : structure of mpd
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
static ABR_ERR P_ABR_MPD_BufferClose(ABR_MPD_t *mpd)
{
	ABR_MPD_QUEUE_t *pQueue = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(mpd == NULL || mpd->parser == NULL
		|| mpd->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}

	mpdData = __mpdData(mpd->parser->mpdData);
	pQueue = &mpd->buffer.queue;
	mpd->buffer.nBufSize = 0;
	mpd->buffer.pBuffer = NULL;
	pQueue->nCur = 0;
	pQueue->nFront = 0;
	pQueue->nRear = 0;
	pQueue->nSize = ABR_MPD_CONST_DEFAULT_QUEUESIZE;
	if(pQueue->nodeSet != NULL)
	{
		VK_MEM_Free(pQueue->nodeSet);
	}
	if(pQueue != NULL)
	{
		VK_MEM_Free(pQueue);
	}
	return ABR_OK;
}

/**
 * @brief       build url using template and base url
 * @param       mpd : structure of mpd
 * @param       szBuf : buffer for storing url
 * @param       nBufSize : size of the buffer
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
static ABR_ERR P_ABR_MPD_BuildTemplateUrl(ABR_MPD_t *mpd, HINT8 *szBuf, HINT32 nBufSize)
{
	HINT32 nPos = 0;
	HINT32 nLen = 0;
	HINT32 nRepresentId = 0;
	HINT32 nIndex = 0;
	HINT8 *pTemp = NULL;
	HINT8 szIndexBuff[ABR_MPD_CONST_ID_LENGTH] = {0};
	HINT8 szUrlTemp[ABR_MPD_CONST_URL_LENGTH] = {0};
	mpdData_t *mpdData = NULL;
	UNUSED_PARAM(nBufSize);
	PrintEnter;
	if(mpd == NULL || mpd->parser == NULL
		|| mpd->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	mpdData = __mpdData(mpd->parser->mpdData);
	if(mpdData == NULL || mpdData->periodSet+(mpd->nCurPeriod) == NULL
		|| ((mpdData->periodSet+mpd->nCurPeriod)->representSet+mpd->nCurRepresent) == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(((mpdData->periodSet+mpd->nCurPeriod)->representSet+mpd->nCurRepresent)->segInfo.segType == MPD_SEGMENT_TYPE_TEMPLATE)
	{
		PrintDebug("%s(%d) Segment Type is ABR_MPD_SEGMENT_TYPE_TEMPLATE\n",__FUNCTION__,__LINE__);
		if((mpdData->periodSet+mpd->nCurPeriod)->bExistSegInfoDef != TRUE)
		{
			PrintError("%s(%d) Err> SegmentInfoDefault is not available\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_FAIL;
		}
 		nRepresentId = ((mpdData->periodSet+mpd->nCurPeriod)->representSet+mpd->nCurRepresent)->nId;
 		nIndex = (mpdData->periodSet+mpd->nCurPeriod)->segInfoDef.startIndex + mpd->nCurSegment;
		pTemp = (HINT8*)(mpdData->periodSet+mpd->nCurPeriod)->segInfoDef.pSrcUrlTemplate;
		VK_MEM_Memset(szUrlTemp,0x00,ABR_MPD_CONST_URL_LENGTH);
		nPos = 0;
		while(pTemp != NULL)
		{
			HINT8 *pTempStart = pTemp;
			pTemp = VK_strstr((char*)pTempStart,(char*)ABR_MPD_TEMPLATE_REPRESENTID);
			if(pTemp != NULL)
			{
				VK_MEM_Memset(szIndexBuff,0x00,ABR_MPD_CONST_ID_LENGTH);
				itoa(nRepresentId,szIndexBuff,10);
				nLen = pTemp-pTempStart;
				if(ABR_MPD_CONST_URL_LENGTH <= nPos+nLen+VK_strlen(szIndexBuff))
				{
					PrintError("%s(%d) Err> Url Size Over\n",__FUNCTION__,__LINE__);
					PrintExit;
					return ABR_FAIL;
				}
				VK_strncpy(&szUrlTemp[nPos],(char*)pTempStart,nLen);
				VK_strcpy(&szUrlTemp[nPos+nLen],szIndexBuff);
				nPos+=nLen;
				nPos+=VK_strlen(szIndexBuff);
				pTemp+=VK_strlen(ABR_MPD_TEMPLATE_REPRESENTID);
			}
			else
			{
				VK_strcpy(&szUrlTemp[nPos],(char*)pTempStart);
			}
		}
		VK_strcpy((char*)szBuf,(char*)szUrlTemp);
		VK_MEM_Memset(szUrlTemp,0x00,ABR_MPD_CONST_URL_LENGTH);
		pTemp = szBuf;
		nPos = 0;
		while(pTemp != NULL)
		{
			HINT8 *pTempStart = pTemp;
			pTemp = VK_strstr((char*)pTempStart,ABR_MPD_TEMPLATE_INDEX);
			if(pTemp != NULL)
			{
				VK_MEM_Memset(szIndexBuff,0x00,ABR_MPD_CONST_ID_LENGTH);
				itoa(nIndex,szIndexBuff,10);
				nLen = pTemp-pTempStart;
				if(ABR_MPD_CONST_URL_LENGTH <= nPos+nLen+VK_strlen(szIndexBuff))
				{
					PrintError("%s(%d) Err> Url Size Over\n",__FUNCTION__,__LINE__);
					PrintExit;
					return ABR_FAIL;
				}
				VK_strncpy(&szUrlTemp[nPos],(char*)pTempStart,nLen);
				VK_strcpy(&szUrlTemp[nPos+nLen],szIndexBuff);
				nPos+=nLen;
				nPos+=VK_strlen(szIndexBuff);
				pTemp+=VK_strlen(ABR_MPD_TEMPLATE_INDEX);
			}
			else
			{
				VK_strcpy(&szUrlTemp[nPos],(char*)pTempStart);
			}
		}
		VK_strcpy((char*)szBuf,(char*)szUrlTemp);
		VK_MEM_Memset(szUrlTemp,0x00,ABR_MPD_CONST_URL_LENGTH);
		pTemp = szBuf;
		nPos = 0;
		while(pTemp != NULL)
		{
			HINT8 *pTempStart = pTemp;
			pTemp = VK_strstr((char*)pTempStart,ABR_MPD_TEMPLATE_D_DOLLAR);
			if(pTemp != NULL)
			{
				VK_MEM_Memset(szIndexBuff,0x00,ABR_MPD_CONST_ID_LENGTH);
				nLen = pTemp-pTempStart;
				if(ABR_MPD_CONST_URL_LENGTH <= nPos+nLen+VK_strlen(ABR_MPD_TEMPLATE_DOLLAR))
				{
					PrintError("%s(%d) Err> Url Size Over\n",__FUNCTION__,__LINE__);
					PrintExit;
					return ABR_FAIL;
				}
				VK_strncpy(&szUrlTemp[nPos],(char*)pTempStart,nLen);
				VK_strcpy(&szUrlTemp[nPos+nLen],ABR_MPD_TEMPLATE_DOLLAR);
				nPos+=nLen;
				nPos+=VK_strlen(ABR_MPD_TEMPLATE_DOLLAR);
				pTemp+=VK_strlen(ABR_MPD_TEMPLATE_D_DOLLAR);
			}
			else
			{
				VK_strcpy(&szUrlTemp[nPos],(char*)pTempStart);
			}
		}
		VK_strcpy((char*)szBuf,(char*)szUrlTemp);
		PrintExit;
		return ABR_OK;
		/* For Template Url */
	}
	PrintExit;
	return ABR_FAIL;
}

/**
 * @brief       build url using base url
 * @param       mpd : structure of mpd
 * @param       szBuf : buffer for storing url
 * @param       nBufSize : size of the buffer
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
static ABR_ERR P_ABR_MPD_BuildUrl(ABR_MPD_t *mpd, HINT8 *szBuf, HINT32 nBufSize)
{
	HINT32 nLen = 0;
	HINT8 *pBaseUrl = NULL;
	HINT8 *pSegInfoBaseUrl = NULL;
	HINT8 szUrlTemp[ABR_MPD_CONST_URL_LENGTH] = {0};
	mpdData_t *mpdData = NULL;
	PrintEnter;
	if(mpd == NULL || mpd->parser == NULL
		|| mpd->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	mpdData = __mpdData(mpd->parser->mpdData);
	if(mpdData == NULL || mpdData->periodSet+(mpd->nCurPeriod) == NULL
		|| ((mpdData->periodSet+mpd->nCurPeriod)->representSet+mpd->nCurRepresent) == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	else
	{
		PrintDebug("%s(%d) Segment Type is ABR_MPD_SEGMENT_TYPE_URL\n",__FUNCTION__,__LINE__);
		if(NULL != VK_strstr((char*)szBuf,"http:/"))
		{
			PrintExit;
			PrintDebug("%s(%d) URL : %s\n",__FUNCTION__,__LINE__,szBuf);
			return ABR_OK;
		}

		if(((mpdData->periodSet+mpd->nCurPeriod)->representSet+mpd->nCurRepresent)->segInfo.pBaseUrl != NULL)
		{
			pSegInfoBaseUrl = (HINT8 *)((mpdData->periodSet+mpd->nCurPeriod)->representSet+mpd->nCurRepresent)->segInfo.pBaseUrl;
		}
		else
		{

			if((mpdData->periodSet+mpd->nCurPeriod)->bExistSegInfoDef == TRUE)
			{
				pSegInfoBaseUrl = (HINT8 *)(mpdData->periodSet+mpd->nCurPeriod)->segInfoDef.pBaseUrl;
			}
		}
		if(pSegInfoBaseUrl != NULL)
		{

			if(NULL != VK_strstr((char*)pSegInfoBaseUrl,"http:/"))
			{
				nLen = 0;
				VK_MEM_Memset(szUrlTemp,0x00,ABR_MPD_CONST_URL_LENGTH);
				nLen = VK_strlen(pSegInfoBaseUrl);
				VK_strcpy(szUrlTemp, szBuf);
				if(nBufSize <= nLen + (HINT32)VK_strlen(szUrlTemp))
				{
					PrintError("%s(%d) Err> Buffer Size over : %d \n",__FUNCTION__,__LINE__,nBufSize);
					PrintExit;
					return ABR_FAIL;
				}
				VK_MEM_Memset(szBuf,0x00,nBufSize);
				VK_strncpy((char*)szBuf,(char*)pSegInfoBaseUrl,nLen);
				VK_strcat(szBuf,"/");
				VK_strcat(szBuf,szUrlTemp);
				/* Integration */
				PrintDebug("%s(%d) URL : %s\n",__FUNCTION__,__LINE__,szBuf);
				PrintExit;
				return ABR_OK;
			}
		}

		if(mpdData->pBaseUrl != NULL)
		{
			pBaseUrl = (HINT8*)mpdData->pBaseUrl;
		}
		else if(mpd->pBaseUrl != NULL)
		{
			pBaseUrl = (HINT8*)mpd->pBaseUrl;
		}

		if(NULL != VK_strstr((char*)pBaseUrl,"http:/"))
		{
			nLen = 0;
			VK_MEM_Memset(szUrlTemp,0x00,ABR_MPD_CONST_URL_LENGTH);
			nLen = VK_strlen(pBaseUrl);
			VK_strcpy(szUrlTemp, szBuf);
			if(nBufSize <= nLen + (HINT32)VK_strlen(szUrlTemp))
			{
				PrintError("%s(%d) Err> Buffer Size over : %d \n",__FUNCTION__,__LINE__,nBufSize);
				PrintExit;
				return ABR_FAIL;
			}
			VK_MEM_Memset(szBuf,0x00,nBufSize);
			VK_strncpy((char*)szBuf,(char*)pBaseUrl,nLen);
			if(pSegInfoBaseUrl == NULL)
			{
				VK_strcat(szBuf,"/");
				VK_strcat(szBuf,szUrlTemp);
				/* Integration */
				PrintDebug("%s(%d) URL : %s\n",__FUNCTION__,__LINE__,szBuf);
				PrintExit;
				return ABR_OK;
			}
			else
			{
				VK_strcat(szBuf,"/");
				VK_strcat(szBuf,pSegInfoBaseUrl);
				VK_strcat(szBuf,"/");
				VK_strcat(szBuf,szUrlTemp);
				/* Integration */
				PrintDebug("%s(%d) URL : %s\n",__FUNCTION__,__LINE__,szBuf);
				PrintExit;
				return ABR_OK;
			}
		}
		else
		{
			PrintError("%s(%d) Err> Build Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_FAIL;
		}
	}
	PrintError("%s(%d) Err> Build Fail\n",__FUNCTION__,__LINE__);
	PrintExit;
	return ABR_FAIL;
}

/*----------------------------------------------------------------------------------------------
 *	Task
 *---------------------------------------------------------------------------------------------*/

#if __________task__________
#endif

/**
 * @brief       abr control task
 * @param       pParam : mpd handle
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

#if 0 /* DO NOT USE TASK by ykkim */
static void IPTUNER_ABR_MPD_ABR_Task (void *pParam)
{
	HUINT64 ullBandwidth = 0;
	ABR_MPD_t *pMpd = NULL;
	ABR_HANDLE hMpd = (ABR_HANDLE) pParam;
	PrintEnter;
	pMpd = __mpd(hMpd);
	if (pParam == NULL)
	{
		PrintDebug ("%s(%d) : IPTUNER_ABR_ABR_MPD_Task unavailable (ABR Handle is null)!!!\n",__FUNCTION__,__LINE__);
		return;
	}
	while ( TRUE != pMpd->bStopABRTask )
	{
		P_ABR_MPD_MeasureBandwidth(hMpd, &ullBandwidth);
		// ABR_MPD_SetLowerBitrate(hMpd, ullBandwidth);
		// ullBandwidth = 0;
		usleep(ABR_MPD_CONST_DEFAULT_INTERVAL);
	}
	pMpd->bABRTaskQuit = TRUE;
	PrintExit;
	return;
}
#endif

/**
 * @brief       create abr control task
 * @param       hMpd : mpd handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

static ABR_ERR  P_ABR_MPD_CreateABRThread (ABR_HANDLE hMpd)
{
	ABR_ERR nRet = ABR_OK;
	ABR_MPD_t * pMpd = NULL;
	HINT32 nVKRet;
	HINT8 tmpString[256]; /* for naming each tasks */

	PrintEnter;

	VK_memset (tmpString, 0, sizeof(tmpString));

	/* validation checking */
	if (hMpd == NULL)
	{
		PrintError ("%s(%d) Error> Wrong MPD handle. The pointer of hMpd is NULL !\n",__FUNCTION__,__LINE__ );
		nRet = ABR_FAIL;
		PrintExit;
		return nRet;
	}
	pMpd = __mpd(hMpd);
#if 0 /* DO NOT USE TASK ykkim */
	/* Create Download Task */
	VK_memset (tmpString, 0, sizeof(tmpString));
	VK_sprintf (tmpString,"ABR_ABR_MPD_Task_%p",hMpd);
	nVKRet = VK_TASK_Create (
				IPTUNER_ABR_MPD_ABR_Task		/* Task Functon Pointer */
				, DOWNLOAD_TASK_PRIORITY	/* Download Task Priority : TODO */
				, ABR_MPD_CONST_DEFAULT_STACK_SIZE		/* Download Stack Size */
				, tmpString					/* Task Name */
				, (void*) hMpd				/* Param */
				, &pMpd->ulTaskId			/* Task ID */
				, TRUE);					/* Joinable Flag */
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s(%d) Error> ABR(MPD) Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nRet = ABR_FAIL;
		PrintExit;
		return nRet;
	}

	/* Start Download Task */
	VK_TASK_Start(pMpd->ulTaskId);
#endif
	PrintExit;
	return nRet;
}

#if __________di__________
#endif

/*----------------------------------------------------------------------------------------------
 *	External API
 *---------------------------------------------------------------------------------------------*/

/**
 * @brief       open mpd controller
 * @param       pMpdBuff : stored manifest in buffer
 * @param	nContentLength : size of manifest
 * @return      abr handle
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_HANDLE 	ABR_MPD_Open (	HINT8	*pMpdBuff, HINT32 nContentLength)
{
	HINT32  nRet;
	HINT8 tmpString[256]; /* for naming each tasks */
	ABR_HANDLE hMpd = NULL;
	ABR_MPD_t *pMpd = NULL;
	mpdData_t *mpdData = NULL;
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

	// nContentLength = P_ABR_MPD_GetManifest(pHTuner,pMpdBuff);
#if ABR_MPD_CONFIG_MEMORY_PARSE
	hMpd = ABR_MPD_MemOpen(pMpdBuff, nContentLength+ABR_MPD_CONST_PADDING);
#else
	hMpd = ABR_MPD_FileOpen(ABR_MPD_CONFIG_TEMP_CACHE);
#endif
	// P_ABR_MPD_CalcBounds(hMpd);
	if( hMpd == NULL )
	{
		PrintError("%s(%d) Error> MPD Open Failn",__FUNCTION__,__LINE__);
		nRet = ABR_FAIL;
		return NULL;
	}
	P_ABR_MPD_BufferOpen(hMpd);
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);

	VK_memset (tmpString, 0, sizeof(tmpString));
	VK_sprintf (tmpString,"ABR_ABR_MPD_SEMA_%p",hMpd);
	nRet = VK_SEM_Create(&pMpd->ulSema,  tmpString, VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nRet )
	{
		PrintError ("%s(%d) Error> ABR(MPD) Sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nRet );
		nRet = ABR_FAIL;
		return NULL;
	}
	/*
	if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.ulDuration > 0)
	{
		pMpd->ulInterval = ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.ulDuration;
	}
	else
	{
		if((mpdData->periodSet+pMpd->nCurPeriod)->bExistSegInfoDef == FALSE)
		{
			pMpd->ulInterval = 0;
		}
		else
		{
			if((mpdData->periodSet+pMpd->nCurPeriod)->segInfoDef.ulDuration > 0)
			{
				pMpd->ulInterval = (mpdData->periodSet+pMpd->nCurPeriod)->segInfoDef.ulDuration;
			}
			else
			{
				pMpd->ulInterval = 0;
			}
		}
	}
	pMpd->ulInterval /= 2;
	if(pMpd->ulInterval <= ABR_MPD_CONST_DEFAULT_INTERVAL)
	{
		pMpd->ulInterval = ABR_MPD_CONST_DEFAULT_INTERVAL;
	}*/
//	PrintData ("%s (%d) Task Interval : %d\n",__FUNCTION__,__LINE__, pMpd->ulInterval);
#if 0 /* DO NOT USE TASK by ykkim */
	nRet = P_ABR_MPD_CreateABRThread(hMpd);
	if(nRet != ABR_OK)
	{
		ABR_MPD_Close(hMpd);
		hMpd = NULL;
	}
#endif
	PrintExit;
	return hMpd;
}

/**
 * @brief	make the abr handle from memory
 * @param       szBuf : stored manifest in buffer
 * @param	nBufSize : size of manifest
 * @return      abr handle
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_HANDLE 	ABR_MPD_MemOpen (HINT8 *szBuf, HINT32 nBufSize)
{
	mpdErr_e err = 0;
//	mpdData_t *mpdData = NULL;
	mpdParser_t* parser = NULL;
	ABR_MPD_t *mpd = NULL;
	// ABR_HANDLE hMpd = NULL;
	PrintEnter;
	parser = (ABR_HANDLE)ABR_MPDParser_MemOpen((char*)szBuf,nBufSize);
	if(parser == NULL)
	{
		PrintError("%s(%d) Err> Parser is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	err = ABR_MPDParser_Parse(parser);
	if(err != MPD_ERR_OK)
	{
		PrintError("%s(%d) Err> Parse Fail (error code : %d)\n",__FUNCTION__,__LINE__,err);
		PrintExit;
		return NULL;
	}
	mpd = (ABR_MPD_t*)VK_MEM_Alloc(sizeof(ABR_MPD_t));
	if(mpd == NULL)
	{
		PrintError("%s(%d) Err> MPD Memory Alloc Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(mpd,0x00,sizeof(ABR_MPD_t));
	mpd->parser = parser;
	PrintExit;
	return (ABR_HANDLE)mpd;
}

/**
 * @brief       open the manifest file to make the abr handle
 * @param       szPath : path of manifest file
 * @return      abr handle
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_HANDLE 	ABR_MPD_FileOpen (HINT8 *szPath)
{
	mpdErr_e err = 0;
//	mpdData_t *mpdData = NULL;
	mpdParser_t* parser = NULL;
	ABR_MPD_t *mpd = NULL;
	// ABR_HANDLE hMpd = NULL;
	PrintEnter;
	parser = (ABR_HANDLE)ABR_MPDParser_FileOpen((char*)szPath);
	if(parser == NULL)
	{
		PrintError("%s(%d) Err> Parser is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	err = ABR_MPDParser_Parse(parser);
	if(err != ABR_OK)
	{
		PrintError("%s(%d) Err> Parse Fail (error code : %d)\n",__FUNCTION__,__LINE__,err);
		PrintExit;
		return NULL;
	}
	mpd = (ABR_MPD_t*)VK_MEM_Alloc(sizeof(ABR_MPD_t));
	if(mpd == NULL)
	{
		PrintError("%s(%d) Err> MPD Memory Alloc Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(mpd,0x00,sizeof(ABR_MPD_t));
	mpd->parser = parser;
	PrintExit;
	return (ABR_HANDLE)mpd;
}

/**
 * @brief       close abr handle
 * @param       hMpd : mpd handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR 	ABR_MPD_Close (ABR_HANDLE hMpd)
{
//	mpdData_t *mpdData = NULL;
//	mpdParser_t* parser = NULL;
	ABR_MPD_t *pMpd = NULL;
	PrintEnter;
	// ABR_HANDLE hMpd = NULL;
	if(hMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD Handle is NULL\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
#if 0 /* DO NOT USE TASK by ykkim */
	pMpd->bStopABRTask = TRUE;
	while(pMpd->bABRTaskQuit != TRUE)
	{
		usleep(100);
	}
#endif
	P_ABR_MPD_BufferClose(pMpd);
	VK_SEM_Destroy(pMpd->ulSema);
#if 0 /* DO NOT USE TASK by ykkim */
	VK_TASK_Destroy(pMpd->ulTaskId);
#endif
	ABR_MPDParser_Close(((ABR_MPD_t*)hMpd)->parser);
	VK_MEM_Free(pMpd);
	PrintExit;
	return ABR_OK;
}
/*
ABR_ERR 	ABR_MPD_SetThroughput(ABR_HANDLE hMpd, HINT32 nRecv, HUINT32 ulRTD)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	P_ABR_MPD_Lock(hMpd);
	pMpd->nRecv = nRecv;
	pMpd->ulRTD = ulRTD;
	P_ABR_MPD_Unlock(hMpd);
	return ABR_OK;
}*/

/**
 * @brief       setting throughput for measuring bandwidth
 * @param       hMpd : abr handle
 * @param	nRecv : data size
 * @param	tvstart : beginning time of receive
 * @param	tvstart : ending time of receive
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR 	ABR_MPD_SetThroughput(ABR_HANDLE hMpd, HINT32 nRecv, TIMEVAL_t tvstart, TIMEVAL_t tvend)
{
	ABR_MPD_t* pMpd = NULL;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	P_ABR_MPD_Lock(hMpd);
	if(pMpd->bNetStatusFull)
	{
		P_ABR_MPD_Unlock(hMpd);
		return ABR_OK;
	}
	pMpd->stNetStatus[pMpd->nAvgCount].nRecv = nRecv;
//	pMpd->ulRTD = ulRTD;
	if(pMpd->nAvgCount != 0)
	{
		pMpd->stNetStatus[pMpd->nAvgCount].tvlast = pMpd->stNetStatus[pMpd->nAvgCount-1].tvend;
	}
	else
	{
		pMpd->stNetStatus[pMpd->nAvgCount].tvlast = pMpd->stNetStatus[ABR_MPD_CONFIG_ABR_AVGCOUNT-1].tvend;
	}
	pMpd->stNetStatus[pMpd->nAvgCount].tvend = tvend;
	pMpd->stNetStatus[pMpd->nAvgCount].tvstart = tvstart;
	if(++pMpd->nAvgCount >= ABR_MPD_CONFIG_ABR_AVGCOUNT)
	{
		pMpd->nAvgCount = 0;
		pMpd->bNetStatusFull = TRUE;
	}
	P_ABR_MPD_Unlock(hMpd);
	return ABR_OK;
}

/**
 * @brief       setting base url
 * @param       hMpd : abr handle
 * @param	pBaseUrl : base url
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark	not supported yet
 * @see
 **/

ABR_ERR 	ABR_MPD_SetBaseUrl (ABR_HANDLE hMpd, HINT8* pBaseUrl)
{
	ABR_MPD_t* pMpd = NULL;
//	mpdData_t *mpdData = NULL;
	PrintEnter;
#if 0
	if(pBaseUrl == NULL)
	{
		PrintError("%s(%d) Err> Base Url is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_INVALID_PARAM;
	}
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	pMpd->pBaseUrl= pBaseUrl;
#endif
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       setting period
 * @param       hMpd : abr handle
 * @param	nPeriod : period ID
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR 	ABR_MPD_SetPeriod (ABR_HANDLE hMpd, HINT32 nPeriod)
{
	ABR_MPD_t* pMpd = NULL;
//	mpdData_t *mpdData = NULL;
	PrintEnter;
	if(nPeriod < 0)
	{
		PrintError("%s(%d) Err> Period(%d) is INVALID\n",__FUNCTION__,__LINE__,nPeriod);
		PrintExit;
		return ABR_INVALID_PARAM;
	}
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(ABR_MPD_GetTotalPeriod(hMpd) <= nPeriod)
	{
		PrintError("%s(%d) Err> Period(%d) is INVALID\n",__FUNCTION__,__LINE__,nPeriod);
		PrintExit;
		return ABR_NO_RESOURCE;
	}
	pMpd = __mpd(hMpd);
	pMpd->nCurPeriod = nPeriod;
	pMpd->bTargetChanged=TRUE;
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       setting bitrate
 * @param       hMpd : abr handle
 * @param	ullBitrate : bitrate for setting
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR 	ABR_MPD_SetBitrate (ABR_HANDLE hMpd, HUINT64 ullBitrate)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->ullBandwidth == ullBitrate)
	{
		PrintExit;
		return ABR_OK;
	}
	for(int i = 0; i < (mpdData->periodSet+pMpd->nCurPeriod)->nTotalRepresent ; i++)
	{
		if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth == ullBitrate)
		{
			pMpd->nCurRepresent = i;
			PrintDebug("%s(%d) Debug> Set Bitrate(%lld)\n",__FUNCTION__,__LINE__,ullBitrate);
			PrintExit;
			return ABR_OK;
		}
	}
	PrintError("%s(%d) Err> Bitrate(%lld) is INVALID\n",__FUNCTION__,__LINE__,ullBitrate);
	PrintExit;
	return ABR_INVALID_PARAM;
}

/**
 * @brief       setting bitrate automatically
 * @param       hMpd : abr handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HUINT64 	ABR_MPD_SetAutoBitrate (ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT64 ullBitrate = 0;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ullBitrate;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	switch(ABR_MPD_CONFIG_AUTO_BITRATE_TYPE)
	{
		case ABR_MPD_CONST_HIGHEST_BITRATE :
			ullBitrate = ABR_MPD_GetMaxBitrate(hMpd,ABR_MPD_GetCurrentPeriodId(hMpd));
			ABR_MPD_SetBitrate(hMpd,ullBitrate);
			break;
		case ABR_MPD_CONST_LOWEST_BITRATE :
			ullBitrate = ABR_MPD_GetMinBitrate(hMpd,ABR_MPD_GetCurrentPeriodId(hMpd));
			ABR_MPD_SetBitrate(hMpd,ullBitrate);
			break;
		case ABR_MPD_CONST_HISTORICAL_BITRATE :
			PrintError("%s(%d) Err> Historical Auto Setting is not support\n",__FUNCTION__,__LINE__);
			ullBitrate = ABR_MPD_GetMaxBitrate(hMpd,ABR_MPD_GetCurrentPeriodId(hMpd));
			ABR_MPD_SetBitrate(hMpd,ullBitrate);
			break;
		case ABR_MPD_CONST_PRECALC_BITRATE :
			ABR_MPD_SetLowerBitrate (hMpd, ABR_MPD_GetCurrentBandwidth(hMpd));
			ullBitrate = ABR_MPD_GetCurrentBitrate(hMpd);
			break;
		default :
			ullBitrate = ABR_MPD_GetMaxBitrate(hMpd,ABR_MPD_GetCurrentPeriodId(hMpd));
			ABR_MPD_SetBitrate(hMpd,ullBitrate);
			break;
	}
//	PrintError("%s(%d) Err> Bitrate(%lld) is INVALID\n",__FUNCTION__,__LINE__,ullBitrate);
	pMpd->ullBandwidth = ullBitrate + 1;
	PrintExit;
	return ullBitrate;
}

/**
 * @brief       setting bitrate that lower than specific bitrate value
 * @param       hMpd : abr handle
 * @param       ullBitrate : specific bitrate
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR ABR_MPD_SetLowerBitrate (ABR_HANDLE hMpd, HUINT64 ullBitrate)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT64 ullLowerBitrate = 0;
//	(void)hMpd;
//	(void)ulBitrate;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	for(int i = 0; i < (mpdData->periodSet+pMpd->nCurPeriod)->nTotalRepresent ; i++)
	{
		if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth < ullBitrate
			&& ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth > ullLowerBitrate)
		{
			ullLowerBitrate = ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth;
		}
	}
	if(ullLowerBitrate > 0)
	{
		ABR_MPD_SetBitrate (hMpd, ullLowerBitrate);
	}
	else
	{
		ABR_MPD_SetBitrate (hMpd, ABR_MPD_GetMinBitrate(hMpd,pMpd->nCurPeriod));
	}
	// PrintExit;
	return ABR_OK;
}

/**
 * @brief       setting representation
 * @param       hMpd : abr handle
 * @param       nRePresent : representation ID
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR 	ABR_MPD_SetRepresent (ABR_HANDLE hMpd, HINT32 nRepresent)
{
	ABR_MPD_t* pMpd = NULL;
//	mpdData_t *mpdData = NULL;
	PrintEnter;
	if(nRepresent < 0)
	{
		PrintError("%s(%d) Err> Representation(%d) is INVALID\n",__FUNCTION__,__LINE__,nRepresent);
		PrintExit;
		return ABR_INVALID_PARAM;
	}
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	if(ABR_MPD_GetTotalRepresent(hMpd,pMpd->nCurPeriod) <= nRepresent)
	{
		PrintError("%s(%d) Err> Representation(%d) is INVALID\n",__FUNCTION__,__LINE__,nRepresent);
		PrintExit;
		return ABR_NO_RESOURCE;
	}
	pMpd->nCurRepresent = nRepresent;
	pMpd->bTargetChanged = TRUE;
	PrintExit;
	return ABR_OK;
}
#if 0 /* ABR_MPD_SetSegmentByTime is not use */
ABR_ERR		ABR_MPD_SetSegmentByTime(ABR_HANDLE hMpd, HUINT64 *pullTime)
{
	ABR_ERR nRet = ABR_OK;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT32 ulDuration = 0;
	HINT32 nSegment = 0;
	PrintEnter;
	if(*pullTime < 0)
	{
		PrintError("%s(%d) Err> Time(%d) is INVALID\n",__FUNCTION__,__LINE__,*pullTime);
		PrintExit;
		return ABR_INVALID_PARAM;
	}
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(ABR_MPD_GetMediaDuration(hMpd) < *pullTime)
	{
		PrintError("%s(%d) Err> pullTime(%lld) : Duration(%lld) \n",__FUNCTION__,__LINE__,*pullTime,ABR_MPD_GetMediaDuration(hMpd));
		PrintExit;
		return ABR_INVALID_PARAM;
	}
	ulDuration = ABR_MPD_GetSegmentDuration(hMpd);
	if(ulDuration <= 0)
	{
		PrintError("%s(%d) Err> Duration is Zero\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	// ulDuration /= 1000; /* millseconds -> seconds */
	nSegment = *pullTime / ulDuration;
	nRet = ABR_MPD_SetSegment(hMpd,nSegment);
	if(nRet != ABR_OK)
	{
		PrintError("%s(%d) Err> ABR_MPD_SetSegment Fail errcode(%d)\n",__FUNCTION__,__LINE__,nRet);
		PrintExit;
		return nRet;
	}
	*pullTime -= (nSegment * ulDuration);
	PrintExit;
	return ABR_OK;
}
#endif

/**
 * @brief       get number of the segment by specific time
 * @param       hMpd : abr handle
 * @param       ullTime : specific time
 * @return      number of the segment
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32		ABR_MPD_GetSegmentByTime(ABR_HANDLE hMpd, HUINT64 ullTime)
{
	HINT32 nRet = -1;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT32 ulDuration = 0;
	HINT32 nSegment = 0;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return nRet;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(ABR_MPD_GetMediaDuration(hMpd) < ullTime)
	{
		PrintError("%s(%d) Err> pullTime(%lld) : Duration(%d) \n",__FUNCTION__,__LINE__,ullTime,ABR_MPD_GetMediaDuration(hMpd));
		PrintExit;
		return nRet;
	}
	ulDuration = ABR_MPD_GetSegmentDuration(hMpd);
	if(ulDuration <= 0)
	{
		PrintError("%s(%d) Err> Duration is Zero\n",__FUNCTION__,__LINE__);
		PrintExit;
		return nRet;
	}
	// ulDuration /= 1000; /* millseconds -> seconds */
	nSegment = ullTime / ulDuration;
	PrintExit;
	return nSegment;
}

/**
 * @brief       setting segment
 * @param       hMpd : abr handle
 * @param       nSegment : segment ID
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR 	ABR_MPD_SetSegment (ABR_HANDLE hMpd, HINT32 nSegment)
{
	ABR_MPD_t* pMpd = NULL;
//	mpdData_t *mpdData = NULL;
	PrintEnter;
	if(nSegment < 0)
	{
		PrintError("%s(%d) Err> Segment(%d) is INVALID\n",__FUNCTION__,__LINE__,nSegment);
		PrintExit;
		return ABR_INVALID_PARAM;
	}
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	if(ABR_MPD_GetTotalSegment(hMpd,pMpd->nCurPeriod,pMpd->nCurRepresent) <= nSegment)
	{
		PrintError("%s(%d) Err> Segment(%d) is INVALID\n",__FUNCTION__,__LINE__,nSegment);
		PrintExit;
		return ABR_NO_RESOURCE;
	}
	pMpd->nCurSegment = nSegment;
	pMpd->bTargetChanged=TRUE;
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       setting next segment
 * @param       hMpd : abr handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR 	ABR_MPD_NextSegment (ABR_HANDLE hMpd)
{
	ABR_ERR err = ABR_OK;
	mpdData_t *mpdData = NULL;
	ABR_MPD_t *pMpd = NULL;
	mpdElement_Segment_t *segment = NULL;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(mpdData == NULL || (mpdData->periodSet+pMpd->nCurPeriod) == NULL
		|| ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent) == NULL)

	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.segType == MPD_SEGMENT_TYPE_TEMPLATE)
	{
		err = ABR_MPD_SetSegment(hMpd,pMpd->nCurSegment+1);
		if(err != ABR_OK)
		{
			PrintError("%s(%d) Err> SetSegment Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
			PrintExit;
			return err;
		}
	}
	else
	{
		if(pMpd->currentSegment == NULL)
		{
			PrintError("%s(%d) Err> Current Segment is NULL\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_FAIL;
		}
		segment = ABR_MPDParser_NextSegment(pMpd->currentSegment);
		if(segment == NULL)
		{
			PrintError("%s(%d) Err> Next Segment is NULL\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_NO_SEGMENT;
		}
		err = ABR_MPD_SetSegment(hMpd,pMpd->nCurSegment+1);
		if(err != ABR_OK)
		{
			PrintError("%s(%d) Err> SetSegment Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
			PrintExit;
			return err;
		}
		pMpd->currentSegment = segment;
	}
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       setting previous segment
 * @param       hMpd : abr handle
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR 	ABR_MPD_PrevSegment (ABR_HANDLE hMpd)
{
	ABR_ERR err = ABR_OK;
	mpdData_t *mpdData = NULL;
	ABR_MPD_t *pMpd = NULL;
	mpdElement_Segment_t *segment = NULL;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(mpdData == NULL || mpdData->periodSet+(pMpd->nCurPeriod) == NULL
		|| ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent) == NULL)

	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.segType == MPD_SEGMENT_TYPE_TEMPLATE)
	{
		if((pMpd->nCurSegment-1) >= 0)
		{
			err = ABR_MPD_SetSegment(hMpd,pMpd->nCurSegment-1);
			if(err != ABR_OK)
			{
				PrintError("%s(%d) Err> SetSegment Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
				PrintExit;
				return err;
			}
		}
	}
	else
	{
		if(pMpd->currentSegment == NULL)
		{
			PrintError("%s(%d) Err> Current Segment is NULL\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_FAIL;
		}
		segment = ABR_MPDParser_PrevSegment(pMpd->currentSegment);
		if(segment == NULL)
		{
			PrintError("%s(%d) Err> Prev Segment is NULL\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_NO_SEGMENT;
		}
		err = ABR_MPD_SetSegment(hMpd,pMpd->nCurSegment-1);
		if(err != ABR_OK)
		{
			PrintError("%s(%d) Err> SetSegment Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
			PrintExit;
			return err;
		}
		pMpd->currentSegment = segment;
	}
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       get initializeSegment Url
 * @param       hMpd : abr handle
 * @param 	szBuf : buffer for storing url
 * @param	nBufSize : size of the buffer
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR 	ABR_MPD_GetInitSegUrl (ABR_HANDLE hMpd, HINT8 *szBuf, int nBufSize)
{
	ABR_ERR err = ABR_OK;
	mpdData_t *mpdData = NULL;
	HINT8 *pInitSegUrl = NULL;
	ABR_MPD_t *pMpd = NULL;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = (ABR_MPD_t*)hMpd;
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(mpdData == NULL || mpdData->periodSet+(pMpd->nCurPeriod) == NULL
		|| ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent) == NULL)

	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	else
	{
		pInitSegUrl = ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.initSegUrl.pSrcUrl;
		if(pInitSegUrl == NULL)
		{
			PrintDebug("%s(%d) Warn> InitialisationSegmentURL(%d,%d) is NULL\n",__FUNCTION__,__LINE__,pMpd->nCurPeriod,pMpd->nCurRepresent);
			err = ABR_MPD_GetSegmentUrl(hMpd,szBuf,nBufSize);
			if(err != ABR_OK)
			{
				PrintError("%s(%d) Err> errcode : %d\n",__FUNCTION__,__LINE__,err);
				PrintExit;
				return ABR_FAIL;
			}
		}
		else
		{
			VK_strcpy((char*)szBuf,(char*)pInitSegUrl);
			err = P_ABR_MPD_BuildUrl(pMpd,szBuf,nBufSize);
			if(err != ABR_OK)
			{
				PrintError("%s(%d) Err> Build Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
				PrintExit;
				return err;
			}
		}
	}
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       get url of the current segment
 * @param       hMpd : abr handle
 * @param 	szBuf : buffer for storing url
 * @param	nBufSize : size of the buffer
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR 	ABR_MPD_GetSegmentUrl (ABR_HANDLE hMpd, HINT8 *szBuf, int nBufSize)
{
	ABR_ERR err = ABR_OK;
	mpdData_t *mpdData = NULL;
	ABR_MPD_t *pMpd = NULL;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = (ABR_MPD_t*)hMpd;
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(mpdData == NULL || mpdData->periodSet+(pMpd->nCurPeriod) == NULL
		|| ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent) == NULL)

	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.segType == MPD_SEGMENT_TYPE_TEMPLATE)
	{
		/* For Template Url */
		err = P_ABR_MPD_BuildTemplateUrl(pMpd,szBuf,nBufSize);
		if(err != ABR_OK)
		{
			PrintError("%s(%d) Err> Build Template Url Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
			PrintExit;
			return err;
		}
		err = P_ABR_MPD_BuildUrl(pMpd,szBuf,nBufSize);
		if(err != ABR_OK)
		{
			PrintError("%s(%d) Err> Build Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
			PrintExit;
			return err;
		}
		PrintExit;
		return ABR_OK;
	}
	else
	{
		if(pMpd->currentSegment == NULL || pMpd->bTargetChanged == TRUE)
		{
			if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.firstSegment == NULL)
			{
				PrintError("%s(%d) Err> First Segment(%d,%d) is NULL\n",__FUNCTION__,__LINE__,pMpd->nCurPeriod,pMpd->nCurRepresent);
				PrintExit;
				return ABR_FAIL;
			}
			pMpd->currentSegment = ABR_MPDParser_n_Segment(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.firstSegment,pMpd->nCurSegment);
			if(pMpd->currentSegment == NULL)
			{
				PrintExit;
				return ABR_FAIL;
			}
			pMpd->bTargetChanged = FALSE;
		}
		if(ABR_MPDParser_GetSegmentUrl(pMpd->currentSegment,(char*)szBuf,nBufSize) != ABR_OK)
		{
			PrintError("%s(%d) Err> Get Segment Url Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return ABR_FAIL;
		}
		err = P_ABR_MPD_BuildUrl(pMpd,szBuf,nBufSize);
		if(err != ABR_OK)
		{
			PrintError("%s(%d) Err> Build Fail (errcode : %d)\n",__FUNCTION__,__LINE__,err);
			PrintExit;
			return err;
		}
	}
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       get duration of the current segment
 * @param       hMpd : abr handle
 * @param 	szBuf : buffer for storing url
 * @param	nBufSize : size of the buffer
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HUINT32		ABR_MPD_GetSegmentDuration(ABR_HANDLE hMpd)
{
	HINT32 nRet = 0;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT32 ulDuration = 0;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return nRet;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.ulDuration > 0)
	{
		ulDuration = ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.ulDuration;
	}
	else
	{
		if((mpdData->periodSet+pMpd->nCurPeriod)->bExistSegInfoDef == FALSE)
		{
			PrintError("%s(%d) Err> SegmentInfoDefault is not exist \n",__FUNCTION__,__LINE__);
			PrintExit;
			return nRet;
		}
		else
		{
			if((mpdData->periodSet+pMpd->nCurPeriod)->segInfoDef.ulDuration > 0)
			{
				ulDuration = (mpdData->periodSet+pMpd->nCurPeriod)->segInfoDef.ulDuration;
			}
			else
			{
				PrintError("%s(%d) Err> Segment Duration is not exist \n",__FUNCTION__,__LINE__);
				PrintExit;
				return nRet;
			}
		}
	}
	return ulDuration;
}

/**
 * @brief       get range of the current segment
 * @param       hMpd : abr handle
 * @return      range
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark	not suppored yet
 * @see
 **/

HUINT32	ABR_MPD_GetSegmentRange (ABR_HANDLE hMpd)
{
	PrintEnter;
	(void)hMpd;
	PrintExit;
	return 0;
}

/**
 * @brief       get number of periods
 * @param       hMpd : abr handle
 * @return      number of periods
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetTotalPeriod (ABR_HANDLE hMpd)
{
	mpdData_t *mpdData = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	mpdData = __mpdData(((ABR_MPD_t*)hMpd)->parser->mpdData);
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return mpdData->nTotalPeriod;
}

/**
 * @brief       get number of represetation
 * @param       hMpd : abr handle
 * @param       nPeriod : number of target period
 * @return      number of representation
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetTotalRepresent (ABR_HANDLE hMpd, HINT32 nPeriod)
{
	mpdData_t *mpdData = NULL;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	mpdData = __mpdData(((ABR_MPD_t*)hMpd)->parser->mpdData);
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	if(mpdData->nTotalPeriod <= nPeriod || mpdData->periodSet == NULL || (mpdData->periodSet+nPeriod) == NULL)
	{
		PrintError("%s(%d) Err> MPD PeriodSet is INVALID\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return (mpdData->periodSet+nPeriod)->nTotalRepresent;
}

/**
 * @brief       get number of segement
 * @param       hMpd : abr handle
 * @param       nPeriod : target period ID
 * @param       nPeriod : target representation ID
 * @return      number of segment
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetTotalSegment (ABR_HANDLE hMpd, HINT32 nPeriod, HINT32 nRepresent)
{
	mpdData_t *mpdData = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	mpdData = __mpdData(((ABR_MPD_t*)hMpd)->parser->mpdData);
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	if(mpdData->nTotalPeriod <= nPeriod || mpdData->periodSet == NULL || (mpdData->periodSet+nPeriod) == NULL)
	{
		PrintError("%s(%d) Err> MPD PeriodSet is INVALID\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	if((mpdData->periodSet+nPeriod)->nTotalRepresent <= nRepresent
		|| (mpdData->periodSet+nPeriod)->representSet == NULL
		|| (mpdData->periodSet+nPeriod)->representSet+nRepresent == NULL)
	{
		PrintError("%s(%d) Err> MPD RepresentationSet is INVALID\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return ((mpdData->periodSet+nPeriod)->representSet+nRepresent)->segInfo.nTotalSegment;
}

/**
 * @brief       get number of segement from current period/representation
 * @param       hMpd : abr handle
 * @return      number of segment
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetCurrentTotalSegment (ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	// PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(((ABR_MPD_t*)hMpd)->parser->mpdData);
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	if(mpdData->nTotalPeriod <= pMpd->nCurPeriod || mpdData->periodSet == NULL || (mpdData->periodSet+pMpd->nCurPeriod) == NULL)
	{
		PrintError("%s(%d) Err> MPD PeriodSet is INVALID\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	if((mpdData->periodSet+pMpd->nCurPeriod )->nTotalRepresent <= pMpd->nCurRepresent
		|| (mpdData->periodSet+pMpd->nCurPeriod )->representSet == NULL
		|| (mpdData->periodSet+pMpd->nCurPeriod )->representSet+pMpd->nCurRepresent == NULL)
	{
		PrintError("%s(%d) Err> MPD RepresentationSet is INVALID\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	// PrintExit;
	return ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.nTotalSegment;
}

/**
 * @brief       get highest bitrate
 * @param       hMpd : abr handle
 * @param	nPeriod : target period ID
 * @return      highest bitrate value
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HUINT64 	ABR_MPD_GetMaxBitrate (ABR_HANDLE hMpd, HINT32 nPeriod)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT64 ullMaxBitrate = 0;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(pMpd->nCurPeriod == nPeriod && pMpd->ullMaxBitrate > 0)
	{
		return pMpd->ullMaxBitrate;
	}
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	for(int i = 0; i < (mpdData->periodSet+pMpd->nCurPeriod)->nTotalRepresent ; i++)
	{
		if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth > ullMaxBitrate)
		{
			ullMaxBitrate = ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth;
			PrintDebug("%s(%d) Debug> Bitrate(%d) is (%lld)\n",__FUNCTION__,__LINE__,nPeriod, ullMaxBitrate);
		}
	}
	PrintDebug("%s(%d) Debug> MaxBitrate(%d) is (%lld)\n",__FUNCTION__,__LINE__,nPeriod, ullMaxBitrate);
	pMpd->ullMaxBitrate = ullMaxBitrate;
//	PrintExit;
	return ullMaxBitrate;
}

/**
 * @brief       get lowest bitrate
 * @param       hMpd : abr handle
 * @param	nPeriod : target period ID
 * @return      lowest bitrate value
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HUINT64 	ABR_MPD_GetMinBitrate (ABR_HANDLE hMpd, HINT32 nPeriod)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HUINT64 ullMinBitrate = 0;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(pMpd->nCurPeriod == nPeriod && pMpd->ullMinBitrate > 0)
	{
		return pMpd->ullMinBitrate;
	}
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	ullMinBitrate = ABR_MPD_GetMaxBitrate (hMpd,nPeriod);
	for(int i = 0; i < (mpdData->periodSet+pMpd->nCurPeriod)->nTotalRepresent ; i++)
	{
		if(((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth < ullMinBitrate)
		{
			ullMinBitrate = ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+i)->ullBandwidth;
			PrintDebug("%s(%d) Debug> Bitrate(%d) is (%lld)\n",__FUNCTION__,__LINE__,nPeriod, ullMinBitrate);
		}
	}
	PrintDebug("%s(%d) Debug> MinBitrate(%d) is (%lld)\n",__FUNCTION__,__LINE__,nPeriod, ullMinBitrate);
	pMpd->ullMinBitrate = ullMinBitrate;
//	PrintExit;
	return ullMinBitrate;
}

/**
 * @brief       get duration of media content
 * @param       hMpd : abr handle
 * @return      duration of media content
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT32 	ABR_MPD_GetMediaDuration (ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(mpdData->ulDuration > 0)
	{
//		PrintExit;
		return mpdData->ulDuration;
	}
	else
	{
		return ABR_MPD_GetTotalSegment(hMpd, pMpd->nCurPeriod, pMpd->nCurRepresent) * ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->segInfo.ulDuration;
	}
	/* if the duration is unknown, shall i calculate duration? */
//	PrintExit;
	return 0;
}

/**
 * @brief       get current bitrate
 * @param       hMpd : abr handle
 * @return      bitrate
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HUINT64 ABR_MPD_GetCurrentBitrate (ABR_HANDLE hMpd)
{
	ABR_MPD_t *pMpd = NULL;
	mpdData_t *mpdData = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	pMpd = __mpd(hMpd);
	if(pMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	mpdData = __mpdData(pMpd->parser->mpdData);
	if(mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD Data is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	if(mpdData->nTotalPeriod <= pMpd->nCurPeriod)
	{
		PrintError("%s(%d) Err> MPD Current Period Number is INVALID(%d in %d) \n",__FUNCTION__,__LINE__,pMpd->nCurPeriod,mpdData->nTotalPeriod);
//		PrintExit;
		return 0;
	}
	if(mpdData->periodSet == NULL)
	{
		PrintError("%s(%d) Err> MPD PeriodSet is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}

	if((mpdData->periodSet+pMpd->nCurPeriod) == NULL)
	{
		PrintError("%s(%d) Err> MPD Current Period(%d) is NULL\n",__FUNCTION__,__LINE__,pMpd->nCurPeriod);
//		PrintExit;
		return 0;
	}
	if((mpdData->periodSet+pMpd->nCurPeriod )->nTotalRepresent <= pMpd->nCurRepresent
		|| (mpdData->periodSet+pMpd->nCurPeriod )->representSet == NULL
		|| (mpdData->periodSet+pMpd->nCurPeriod )->representSet+pMpd->nCurRepresent == NULL)
	{
		PrintError("%s(%d) Err> MPD RepresentationSet is INVALID\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return ((mpdData->periodSet+pMpd->nCurPeriod)->representSet+pMpd->nCurRepresent)->ullBandwidth;
}

/**
 * @brief       get current period ID
 * @param       hMpd : abr handle
 * @return      period ID
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetCurrentPeriodId (ABR_HANDLE hMpd)
{
	ABR_MPD_t *pMpd = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	pMpd = __mpd(hMpd);
	if(pMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return pMpd->nCurPeriod;
}

/**
 * @brief       get current representation ID
 * @param       hMpd : abr handle
 * @return      representation ID
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetCurrentRepresentId (ABR_HANDLE hMpd)
{
	ABR_MPD_t *pMpd = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	pMpd = __mpd(hMpd);
	if(pMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return pMpd->nCurRepresent;
}

/**
 * @brief       get current segment ID
 * @param       hMpd : abr handle
 * @return      segment ID
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32 	ABR_MPD_GetCurrentSegmentId (ABR_HANDLE hMpd)
{
	ABR_MPD_t *pMpd = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	pMpd = __mpd(hMpd);
	if(pMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
//	PrintExit;
	return pMpd->nCurSegment;
}

/**
 * @brief       get current bandwidth
 * @param       hMpd : abr handle
 * @return      bandwidth
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HUINT64 ABR_MPD_GetCurrentBandwidth (ABR_HANDLE hMpd)
{
	HUINT64 ullBandwidth = 0;
	ABR_MPD_t *pMpd = NULL;
//	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	pMpd = __mpd(hMpd);
	if(pMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	ullBandwidth = pMpd->ullBandwidth;
	return ullBandwidth;
}

/**
 * @brief       measuring bandwidth
 * @param       hMpd : mpd handle
 * @param       pullBandwidth : for storing measured bandwidth
 * @return      abr error code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR 	ABR_MPD_MeasureBandwidth(ABR_HANDLE hMpd, HUINT64 *pullBandwidth)
{
	HFLOAT64 dRTD = 0;
	HFLOAT64 dRecv = 0;
	HUINT64 ullProcBitrate = 0;
	HUINT64 ullMaxBitrate = 0;
	HUINT64 ullMinBitrate = 0;
	HINT32 nCount = 0;
	HUINT64 ullBandwidth = 0;
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	P_ABR_MPD_Lock(hMpd);
	/*if(pMpd->ulRTD<= 0 || pMpd->nRecv <= 0)
	{
		PrintExit;
		P_ABR_MPD_Unlock(hMpd);
		return ABR_OK;
	}*/
	/*if(pMpd->nAvgCount <= 0 || pMpd->nRecv <= 0)
	{
		PrintExit;
		P_ABR_MPD_Unlock(hMpd);
		return ABR_OK;
	}*/
	if(!pMpd->bNetStatusFull)
	{
//		PrintExit;
		P_ABR_MPD_Unlock(hMpd);
		return ABR_OK;
	}
	for(nCount = 0 ; nCount < ABR_MPD_CONFIG_ABR_AVGCOUNT ; nCount++)
	{
		/*if(pMpd->stNetStatus[nCount].tvstart.tv_sec <=0 && pMpd->stNetStatus[nCount].tvstart.tv_usec <=0)
		{
			break;
		}*/
		pMpd->stNetStatus[nCount].ulNetRTD = (pMpd->stNetStatus[nCount].tvend.tv_sec-pMpd->stNetStatus[nCount].tvstart.tv_sec)*ABR_MPD_CONST_MICROSEC + (pMpd->stNetStatus[nCount].tvend.tv_usec - pMpd->stNetStatus[nCount].tvstart.tv_usec);
		if(pMpd->stNetStatus[nCount].tvlast.tv_usec || pMpd->stNetStatus[nCount].tvlast.tv_sec)
		{
			pMpd->stNetStatus[nCount].ulProcRTD = (pMpd->stNetStatus[nCount].tvend.tv_sec - pMpd->stNetStatus[nCount].tvlast.tv_sec)*ABR_MPD_CONST_MICROSEC + (pMpd->stNetStatus[nCount].tvend.tv_usec - pMpd->stNetStatus[nCount].tvlast.tv_usec);
		}
		else
		{
			pMpd->stNetStatus[nCount].ulProcRTD = pMpd->stNetStatus[nCount].ulNetRTD;
		}
		dRecv = pMpd->stNetStatus[nCount].nRecv;
		dRTD = pMpd->stNetStatus[nCount].ulProcRTD;
		ullBandwidth = ((dRecv / (dRTD / ABR_MPD_CONST_MICROSEC) ) * 8);
		// PrintDebug("%s(%d) Debug> Bandwidth[%d] = %lld , Recv = %lf , Round Trip = %lf, Proc Round Trip = %ld\n",__FUNCTION__,__LINE__,nCount,ullBandwidth,dRecv,dRTD,pMpd->stNetStatus[nCount].ulProcRTD);
		if(ullMaxBitrate == 0 && ullMinBitrate == 0)
		{
			ullMaxBitrate = ullBandwidth;
			ullMinBitrate = ullBandwidth;
		}
		else if(ullBandwidth < ullMinBitrate)
		{
			ullMinBitrate = ullBandwidth;
		}
		else if(ullBandwidth > ullMaxBitrate)
		{
			ullMaxBitrate = ullBandwidth;
		}
		ullProcBitrate += ullBandwidth;
	}
	if(nCount <= 0 || nCount < ABR_MPD_CONFIG_ABR_AVGCOUNT)
	{
//		PrintExit;
		P_ABR_MPD_Unlock(hMpd);
		return ABR_OK;
	}
	switch(ABR_MPD_CONFIG_SWITCHING_ALGO_TYPE)
	{
		case ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG:
			*pullBandwidth = ullProcBitrate / nCount;
			break;
		case ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_LOW:
			ullProcBitrate -= ullMinBitrate;
			*pullBandwidth = ullProcBitrate / (nCount-1); /* except min bitrate */
			break;
		case ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_HL:
			ullProcBitrate -= (ullMinBitrate + ullMaxBitrate);
			*pullBandwidth = ullProcBitrate / (nCount-2); /* except max & min bitrate */
			break;
		case ABR_MPD_CONST_ALGO_PROC_BITRATE_HIGHEST:
			*pullBandwidth = ullMaxBitrate;
			break;
		case ABR_MPD_CONST_ALGO_PROC_BITRATE_LOWEST:
			*pullBandwidth = ullMinBitrate;
			break;
	}
	pMpd->ullBandwidth = *pullBandwidth;
/*
	dRTD = pMpd->ulNetRTD;
	dRecv = pMpd->nRecv;
	ullBandwidth = (dRecv / (dRTD / ABR_MPD_CONST_MICROSEC) ) * 8;
	PrintDebug("%s(%d) Debug> Network Bandwidth = %lld\n",__FUNCTION__,__LINE__,ullBandwidth);
	dRTD = pMpd->ulProcRTD;
	ullProcBitrate = (dRecv / (dRTD / ABR_MPD_CONST_MICROSEC) ) * 8;
	PrintDebug("%s(%d) Debug> Process Bitrate = %lld\n",__FUNCTION__,__LINE__,ullProcBitrate);
#if 1
	*pullBandwidth = ullProcBitrate;
#else
	*pullBandwidth = ullBandwidth;
#endif
*/

/*	if( pMpd->nAvgCount > 1)
	{
		pMpd->ullBandwidth = *pullBandwidth + pMpd->ullMinBandwidth;
		pMpd->ullBandwidth /= 2;
		if(*pullBandwidth < pMpd->ullMinBandwidth)
		{
			pMpd->ullMinBandwidth = *pullBandwidth;
		}
	}
	else
	{
		pMpd->ullMinBandwidth = *pullBandwidth;
		pMpd->ullBandwidth = *pullBandwidth;
	}
*/
	pMpd->bNetStatusFull = FALSE;
	P_ABR_MPD_Unlock(hMpd);
	// PrintDebug("%s(%d) Debug> ulRecvDuration (pMpd->ulRecvDuration) = %ld\n",__FUNCTION__,__LINE__,(pMpd->ulRecvDuration));
	// PrintDebug("%s(%d) Debug> ullReceived (pMpd->ullReceived) = %ld\n",__FUNCTION__,__LINE__,(pMpd->nReceived));

//	pMpd->ullReceived = 0;
	// PrintDebug("%s(%d) Debug> Min Bandwidth = %lld\n",__FUNCTION__,__LINE__,ullMinBitrate);
	// PrintDebug("%s(%d) Debug> Max Bandwidth = %lld\n",__FUNCTION__,__LINE__,ullMaxBitrate);
	// PrintDebug("%s(%d) Debug> Estimated Bandwidth = %lld\n",__FUNCTION__,__LINE__,pMpd->ullBandwidth);
	//PrintExit;
	return ABR_OK;
}


ABR_QUEUE_t*	ABR_MPD_GetQueue(ABR_HANDLE hMpd)
{
	mpdData_t *mpdData = NULL;
//	PrintEnter;
	if(hMpd == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
//		PrintExit;
		return 0;
	}
	return ((ABR_MPD_t*)hMpd)->pQueue;
}

#if 0 /* YouView by ykkim */
/**
 * @brief       get information of node in the playlist queue by offset
 * @param       hMpd : abr handle
 * @param	sz_offset : target offset
 * @param	nId : the segment ID corresponds to the target offset
 * @param	sz_voffset : virtual offset
 * @param	ullBitrate : bitrate of segment
 * @return      abr err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR ABR_MPD_Queue_GetNodeInfo(ABR_HANDLE hMpd, const HINT64 sz_offset, HINT32 *nId, HINT64 *sz_voffset, HUINT64 *ullBitrate)
{
	HINT32 rightId = 0;
	HUINT64 rightBitrate = 0;
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	HINT32 nCur = 0;
	PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;
	// PrintDebug("%s(%d) Debug> sz_offset is %"PRId64"\n",__FUNCTION__,__LINE__,sz_offset);
	nCur = pQueue->nFront;
	if((pQueue->nodeSet+nCur)->offset > sz_offset)
	{
		PrintExit;
		return -1;
	}
	do
	{
		HINT64 left = 0;
		HINT64 right = 0;

		left = (pQueue->nodeSet+nCur)->offset;

		// PrintDebug("%s(%d) Debug> left sz_offset(%d) is %"PRId64"\n",__FUNCTION__,__LINE__,(pQueue->nodeSet+nCur)->nId,left);

		if((nCur + 1) == pQueue->nSize)
		{
			right = (pQueue->nodeSet)->offset;
			rightId = (pQueue->nodeSet)->nId;
			rightBitrate = (pQueue->nodeSet)->ullBitrate;
		}
		else
		{
			right = (pQueue->nodeSet+nCur+1)->offset;
			rightId = (pQueue->nodeSet+nCur+1)->nId;
			rightBitrate = (pQueue->nodeSet+nCur+1)->ullBitrate;
		}
		// PrintDebug("%s(%d) Debug> right sz_offset(%d) is %"PRId64"\n",__FUNCTION__,__LINE__,rightId,right);
		if(left <= sz_offset && right > sz_offset)
		{
			*sz_voffset = left;
			*nId = rightId;
			*ullBitrate = rightBitrate;
			PrintExit;
			// PrintDebug("%s(%d) Debug> sz_voffset: %"PRId64" , nId : %d , ullBitrate : %lld\n",__FUNCTION__,__LINE__,*sz_voffset,*nId,*ullBitrate);
			return ABR_OK;
		}
		nCur++;
		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
		if(nCur == pQueue->nRear && right == sz_offset)
		{
			*sz_voffset = right;
			*nId = rightId+1;
			*ullBitrate = rightBitrate;
			PrintExit;
			return ABR_OK;
		}
	}while(nCur != pQueue->nRear);

	/*
	for(int i = 0 ; i == pQueue->nSize; i++)
	{
		HINT64 left = 0;
		HINT64 right = 0;
		if((pQueue->nFront + i) == pQueue->nSize)
		{
			left = (pQueue->nodeSet+(pQueue->nFront + i - pQueue->nSize))->offset;
		}
		else
		{
			left = (pQueue->nodeSet+(pQueue->nFront + i))->offset;
		}
		PrintDebug("%s(%d) Debug> left sz_offset is %"PRId64"\n",__FUNCTION__,__LINE__,left);
		if((pQueue->nFront + i + 1) == pQueue->nSize)
		{
			right = (pQueue->nodeSet+((pQueue->nFront + i + 1)- pQueue->nSize))->offset;
			rightId = (pQueue->nodeSet+((pQueue->nFront + i + 1)- pQueue->nSize))->nId;
			rightBitrate = (pQueue->nodeSet+((pQueue->nFront + i + 1)- pQueue->nSize))->ullBitrate;
		}
		else
		{
			right = (pQueue->nodeSet+(pQueue->nFront + i + 1))->offset;
			rightId = (pQueue->nodeSet+(pQueue->nFront + i + 1))->nId;
			rightBitrate = (pQueue->nodeSet+(pQueue->nFront + i + 1))->ullBitrate;
		}
		PrintDebug("%s(%d) Debug> right sz_offset is %"PRId64"\n",__FUNCTION__,__LINE__,right);
		if(left <= sz_offset && right > sz_offset)
		{
			*sz_voffset = left;
			*nId = rightId;
			*ullBitrate = rightBitrate;
			PrintDebug("%s(%d) Debug> sz_voffset: %"PRId64" , nId : %d , ullBitrate : %lld\n",__FUNCTION__,__LINE__,*sz_voffset,*nId,*ullBitrate);
			return ABR_OK;
		}
	}
	*/
	if((pQueue->nodeSet+pQueue->nFront)->offset > sz_offset)
	{
		*nId = (pQueue->nodeSet+pQueue->nFront)->nId - 1;
		if(*nId < 0)
		{
			*nId = 0;
		}
	}
	else
	{
		*nId = (pQueue->nodeSet+pQueue->nRear)->nId + 1;
	}
	PrintExit;
	PrintDebug("%s(%d) Err> sz_offset is not EXIST\n",__FUNCTION__,__LINE__);
	return ABR_NO_RESOURCE;
}

/**
 * @brief       get information of current node in the playlist queue
 * @param       hMpd : abr handle
 * @param	nNodeId : node ID
 * @param	sz_voffset : virtual offset
 * @param	ullBitrate : bitrate of segment
 * @return      abr err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR ABR_MPD_Queue_GetCurNodeInfo(ABR_HANDLE hMpd, HINT32 *nNodeId, HINT64 *sz_voffset, HUINT64 *ullBitrate)
{
	ABR_ERR nRet = ABR_FAIL;
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;
	if(nNodeId != NULL)
	{
		*nNodeId = pQueue->nCur;
	}
	if(sz_voffset != NULL)
	{
		*sz_voffset = (pQueue->nodeSet+pQueue->nCur)->offset;
	}
	if(ullBitrate != NULL)
	{
		*ullBitrate = (pQueue->nodeSet+pQueue->nCur)->ullBitrate;
	}
	nRet = ABR_OK;

	PrintExit;
	return nRet;
}

/**
 * @brief       get information of node in the playlist queue by segment
 * @param       hMpd : abr handle
 * @param	nId : target segment ID
 * @param	nNodeId : the node ID corresponds to the target segment ID
 * @param	sz_voffset : virtual offset
 * @param	ullBitrate : bitrate of segment
 * @return      abr err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR ABR_MPD_Queue_GetNodeInfoBySegment(ABR_HANDLE hMpd, HINT32 nId, HINT32 *nNodeId, HINT64 *sz_voffset, HUINT64 *ullBitrate)
{
	ABR_ERR nRet = ABR_FAIL;
	HINT32 nCur = 0;
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;

	nCur = pQueue->nFront + 1;
	do
	{
		if((pQueue->nodeSet+nCur)->nId == nId)
		{
			*nNodeId = nCur;
			*sz_voffset = (pQueue->nodeSet+nCur)->offset;
			*ullBitrate = (pQueue->nodeSet+nCur)->ullBitrate;
			nRet = ABR_OK;
			break;
		}

		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
	}while(nCur++ != pQueue->nRear);
	PrintExit;
	return nRet;
}

/**
 * @brief       push back information of segment to the playlist queue
 * @param       hMpd : abr handle
 * @param	nId : segment ID
 * @param	sz_file : size of segment
 * @param	ullBitrate : bitrate of segment
 * @return      abr err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

ABR_ERR ABR_MPD_Queue_PushBack(ABR_HANDLE hMpd, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate)
{
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
//	HINT32 nCur = 0;
	PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;

//	nCur = pQueue->nFront + 1;
	/*do
	{
		if((pQueue->nodeSet+nCur)->nId == nId)
		{
			PrintExit;
			PrintDebug("%s(%d) Debug> Already Pushed(%d) ID: %d Size : %lld , Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nRear,nId,sz_file,ullBitrate);
			return ABR_OK;
		}
		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
	} while(nCur++ != pQueue->nRear);
	*/
#if ABR_MPD_CONFIG_BUFFER_TYPE
	if(pQueue->nCur != pQueue->nRear)
	{
		if(pQueue->nCur+1 < pQueue->nSize)
		{
			pQueue->nCur = 0;
		}
		else
		{
			pQueue->nCur++;
		}
	}
#endif

	if((pQueue->nRear + 1) < pQueue->nSize)
	{
		(pQueue->nodeSet+(pQueue->nRear+1))->offset = (pQueue->nodeSet+pQueue->nRear)->offset + sz_file;
		pQueue->nRear++;
	}
	else
	{
		pQueue->nodeSet->offset = (pQueue->nodeSet+pQueue->nRear)->offset + sz_file;
		pQueue->nRear=0;
	}
	(pQueue->nodeSet+pQueue->nRear)->nId = nId;
	(pQueue->nodeSet+pQueue->nRear)->ullBitrate = ullBitrate;
	(pQueue->nodeSet+pQueue->nRear)->range = sz_file;
	if(pQueue->nRear == pQueue->nFront)
	{
		if(pQueue->nFront + 1 >= pQueue->nSize)
		{
			pQueue->nFront = 0;
		}
		else
		{
			pQueue->nFront++;
		}
	}

	pQueue->nCur = pQueue->nRear;
	PrintDebug("%s(%d) Debug> Pushed(%d) ID: %d, Size : %lld, offset : %lld, Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nRear,nId,sz_file,(pQueue->nodeSet+(pQueue->nRear))->offset,ullBitrate);
	PrintExit;
	return ABR_OK;
}

#if 0 /* ABR_MPD_Queue_PushFront is not used */
ABR_ERR ABR_MPD_Queue_PushFront(ABR_HANDLE hMpd, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate)
{
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;

	if((pQueue->nFront - 1) >= 0)
	{
		(pQueue->nodeSet+(pQueue->nFront-1))->offset = (pQueue->nodeSet+pQueue->nFront)->offset - sz_file;
		pQueue->nFront--;
	}
	else
	{
		(pQueue->nodeSet+(pQueue->nSize-1))->offset = (pQueue->nodeSet+pQueue->nFront)->offset - sz_file;
		pQueue->nFront=pQueue->nSize-1;
	}
	(pQueue->nodeSet+pQueue->nFront)->nId = nId;
	(pQueue->nodeSet+pQueue->nFront)->ullBitrate = ullBitrate;
	if(pQueue->nRear == pQueue->nFront)
	{
		if(pQueue->nRear - 1 < 0)
		{
			pQueue->nRear = pQueue->nSize-1;
		}
		else
		{
			pQueue->nRear--;
		}
	}
	pQueue->nCur = pQueue->nFront;
	PrintDebug("%s(%d) Debug> Pushed(%d) ID: %d Size : %lld , Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nRear,nId,sz_file,ullBitrate);
	PrintExit;
	return ABR_OK;
}
#endif

/**
 * @brief       replace information of segment in the playlist queue
 * @param       hMpd : abr handle
 * @param	sz_Base : base size of the replacing information
 * @param	nId : target node ID
 * @param	sz_file : size of segment
 * @param	ullBitrate : bitrate of segment
 * @return      abr err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR ABR_MPD_Queue_Replace(ABR_HANDLE hMpd, HINT64 sz_Base, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate)
{
	HINT32 nCur = 0;
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;

	nCur = pQueue->nFront + 1;
	do
	{
		if(nCur >= pQueue->nSize)
		{
			nCur = 0;
		}
		if((pQueue->nodeSet+nCur)->nId == nId)
		{
			(pQueue->nodeSet+nCur)->offset = sz_Base + sz_file;
			(pQueue->nodeSet+nCur)->range = sz_file;
			if((nCur-1) >= 0)
			{
				(pQueue->nodeSet+(nCur-1))->offset = sz_Base;
				pQueue->nCur = nCur;
				//PrintDebug("%s(%d) Debug> Replace Cursor : %d , Queue Size : %d\n",__FUNCTION__,__LINE__,pQueue->nCur,pQueue->nSize);
			}
			else
			{
				(pQueue->nodeSet)->offset = sz_Base;
				pQueue->nCur = nCur;
				//PrintDebug("%s(%d) Debug> Replace Cursor : %d , Queue Size : %d\n",__FUNCTION__,__LINE__,pQueue->nCur,pQueue->nSize);

			}
			break;
		}
	}while(nCur++ != pQueue->nRear);

	nCur = pQueue->nCur - 1;
	if(nCur < 0)
	{
		nCur = pQueue->nSize;
	}
	while(nCur != pQueue->nFront)
	{
		if((nCur-1) >= 0)
		{
			(pQueue->nodeSet+nCur-1)->offset = (pQueue->nodeSet+nCur)->offset - (pQueue->nodeSet+nCur)->range;
		}
		else
		{
			(pQueue->nodeSet)->offset = (pQueue->nodeSet+nCur)->offset - (pQueue->nodeSet+nCur)->range;
		}
		nCur--;
		if(nCur < 0)
		{
			nCur = pQueue->nSize;
		}
	}

#if ABR_MPD_CONFIG_BUFFER_TYPE
	nCur = pQueue->nCur;
	while(nCur != pQueue->nRear)
	{
		if((nCur+1) < pQueue->nSize)
		{
			(pQueue->nodeSet+nCur+1)->offset = (pQueue->nodeSet+nCur)->offset + (pQueue->nodeSet+(nCur+1))->range;
		}
		else
		{
			(pQueue->nodeSet)->offset = (pQueue->nodeSet+nCur)->offset + (pQueue->nodeSet)->range;
		}
		nCur++;
		if(nCur >= pQueue->nSize)
		{
			nCur = 0;
		}
	}
#else
	pQueue->nRear = pQueue->nCur;
#endif

	PrintDebug("%s(%d) Debug> Replace(%d) ID: %d Size : %lld , Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nCur,nId,sz_file,ullBitrate);
	PrintExit;
	return ABR_OK;
}

/**
 * @brief       reset the playlist queue with base size
 * @param       hMpd : abr handle
 * @param	sz_Base : based size
 * @return      abr err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
ABR_ERR ABR_MPD_Queue_Reset(ABR_HANDLE hMpd, HINT64 nBase)
{
	ABR_MPD_QUEUE_t *pQueue = NULL;
	ABR_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	mpdData = __mpdData(pMpd->parser->mpdData);
	pQueue = &pMpd->buffer.queue;
	pQueue->nCur = 0;
	pQueue->nFront = 0;
	pQueue->nRear = 0;
	VK_MEM_Memset(pQueue->nodeSet,0x00,sizeof(ABR_MPD_NODE_t) * pQueue->nSize);
	(pQueue->nodeSet+pQueue->nFront)->offset = nBase;
	(pQueue->nodeSet+pQueue->nFront)->nId = -1;
	PrintExit;
	return ABR_OK;
}
#endif

ABR_ERR ABR_MPD_SetTimeForSeek(ABR_HANDLE hMpd, HUINT64 ullTime)
{
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	pMpd->ullTimeForSeek = ullTime;
	//PrintExit;
	return ABR_OK;
}

HUINT64 ABR_MPD_GetTimeForSeek(ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	//PrintExit;
	return pMpd->ullTimeForSeek;
}

ABR_ERR ABR_MPD_SetStreamLength(ABR_HANDLE hMpd, HINT64 nLength)
{
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	pMpd->nStreamLength = nLength;
	//PrintExit;
	return ABR_OK;
}

HINT64 ABR_MPD_GetStreamLength(ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return TRUE;
	}
	pMpd = __mpd(hMpd);
	//PrintExit;
	return pMpd->nStreamLength;
}

ABR_ERR ABR_MPD_SetCurrentPosition(ABR_HANDLE hMpd, HINT64 nPosition)
{
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;

	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	pMpd = __mpd(hMpd);
	pMpd->nCurrentPos = nPosition;
	//PrintExit;
	return ABR_OK;
}

HINT64 ABR_MPD_GetCurrentPosition(ABR_HANDLE hMpd)
{
	ABR_MPD_t* pMpd = NULL;
	//PrintEnter;
	if(hMpd == NULL || ((ABR_MPD_t*)hMpd)->parser == NULL
		|| ((ABR_MPD_t*)hMpd)->parser->mpdData == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return TRUE;
	}
	pMpd = __mpd(hMpd);
	//PrintExit;
	return pMpd->nCurrentPos;
}



/*
HINT32 	ABR_MPD_GetTotalPeriod (ABR_HANDLE hMpd)
{
	return __mpdData(((DI_IPTUNER_ABR_ABR_MPD_t*)hMpd)->parser->mpdData)->nTotalPeriod;
}
HINT32 	ABR_MPD_GetTotalRepresent (ABR_HANDLE hMpd, HINT32 nPeriod)
{
	return (__mpdData(((DI_IPTUNER_ABR_ABR_MPD_t*)hMpd)->parser->mpdData)->periodSet+nPeriod)->nTotalRepresent;
}

HINT32 	ABR_MPD_GetTotalSegment (ABR_HANDLE hMpd, HINT32 nPeriod, HINT32 nRepresent)
{
	return ((__mpdData(((DI_IPTUNER_ABR_ABR_MPD_t*)hMpd)->parser->mpdData)->periodSet+nPeriod)->representSet+nRepresent)->segInfo.nTotalSegment;
}
*/
