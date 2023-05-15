/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_index_parser.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     to support YouView indexFile.
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2010 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


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
#include <inttypes.h>    /** for 64bit MACRO ex>PRId64 */
#include <arpa/inet.h>


#include "htype.h"
#include "di_uart.h"
#include "util_index_parser.h"
#include "di_iptuner_config.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#define UTIL_INDEX_PARSER_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef UTIL_INDEX_PARSER_DEBUG
//#define INDEX_DEBUG
#define PrintData       DI_UART_Print
#define PrintDebug      DI_UART_Print
#define PrintError      DI_UART_Print
#define PrintEnter      DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit       DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData       while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug      while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError      DI_UART_Print
#endif
#else
#define PrintData       while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug      while (0) ((int (*)(char *, ...)) 0)
#define PrintError      while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif

#define UNUSED_PARAM(x)  ((void)x)
/* End if define */
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
HINT32 UTIL_Index_GetOffsetByPosition (HUINT8 *pIndex, HUINT32 nPos, HUINT64 *pOffset)
{
	HUINT8 *p=0;
	HUINT32 nNumOfSamples=0;
	HUINT64 ulSampleStartBytes=0;
	HUINT32 nSampleStartTime=0;
	HUINT64 ulTotalBytes=0;
	HUINT32 nDuration=0;
	/*int i=0;*/

	PrintEnter;

	if(pIndex==NULL || pOffset==NULL) return -1;

	/* Get NumOfSamples */
	nNumOfSamples = ntohl(*(HUINT32*)pIndex);

	if(nNumOfSamples==0) return -1;

	p = pIndex+sizeof(HUINT32);

	/* Get Duration & Totoal Bytes */
	ulTotalBytes = (HUINT64) ntohl ((HUINT64)(*(HUINT32*)(p+(nNumOfSamples*12)+4)) << 32)
	               + (HUINT64) ntohl ((HUINT64)(*(HUINT32*)(p+(nNumOfSamples*12))));

	nDuration = ntohl (*(HUINT32*)(p+((nNumOfSamples)*12)+8));

#ifdef INDEX_DEBUG
	PrintData("%s(%d) : Number of Samples  : %u samples\n",__FUNCTION__,__LINE__, nNumOfSamples);
	PrintData("%s(%d) : Total Content Size : %lld bytes\n",__FUNCTION__,__LINE__, ulTotalBytes);
	PrintData("%s(%d) : Content Duration   : %u msec\n",__FUNCTION__,__LINE__, nDuration);
#endif

	/* check nPos exists whithin index time range */
	if(nPos > nDuration)
	{
		PrintError ("%s(%d) Error> invalid position \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/* Search nearest smaple by nPos */
	for ( p=pIndex+sizeof(HUINT32) ; p < pIndex+(nNumOfSamples*(12)); p+=12 )
	{
		ulSampleStartBytes = (HUINT64)ntohl((HUINT64)(*(HUINT32*)(p+4)) << 32) + (HUINT64)ntohl(*(HUINT32*)p);
		nSampleStartTime = ntohl(*(HUINT32*)(p+8));

		/* find nearest offset by position */
		if ( nPos < nSampleStartTime)
		{
			*pOffset = ulSampleStartBytes;
			break;
		}
	}

#ifdef INDEX_DEBUG
	PrintDebug ("%s(%d) Info> Searched sample : %lld bytes %u msecs\n"
	,__FUNCTION__
	,__LINE__
	,ulSampleStartBytes
	,nSampleStartTime);
#endif

	PrintExit;
	return 0;
}

HINT32 UTIL_Index_GetDuration (HUINT8 *pIndex , HUINT32 *pPos)
{
	UNUSED_PARAM(pIndex);
	UNUSED_PARAM(pPos);
	return 0;
}

HINT32 UTIL_Index_GetNumOfSample (HUINT8 *pIndex)
{
	UNUSED_PARAM(pIndex);
	return 0;
}

HINT32 UTIL_Index_GetFileSize (HUINT8 *pIndex, HUINT64 *pOffset)
{
	UNUSED_PARAM(pIndex);
	UNUSED_PARAM(pOffset);
	return 0;
}
