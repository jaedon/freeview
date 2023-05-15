/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_buffermgr.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2012.02.28
 * File Description:	SSTR Buffer Manager
 * Module:
 * Remarks:
 */

/**
 * @defgroup SSTR_BUFFERMGR
 * @brief SSTR Buffer Manager 이며 Fragment를 저장할 Buffer의 운용을 위한 역할을 한다. <br> 현재 본 모듈은 HLS에서도 사용된다.
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
 */

/**
 * @brief SSTR Buffer Manager 이며 Fragment를 저장할 Buffer의 운용을 위한 역할을 한다. <br> 현재 본 모듈은 HLS에서도 사용된다.
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
 * @file sstr_buffermgr.c
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
#include "sstr_buffermgr.h"
#include "di_uart.h"

#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "util_url.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
//#define SSTR_BUFFERMGR_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef SSTR_BUFFERMGR_DEBUG
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

SSTR_BUFFERMGR_t*	SSTR_BUFFERMGR_Open (HUINT64 ullBufferSize, HINT32 nBufferType)
{
	SSTR_BUFFERMGR_t *pBufferMgr = NULL;
	HINT8 tmpString[256]; /* for naming each tasks */
	HINT32 nRet = 0;
	PrintEnter;
	UNUSED_PARAM(nBufferType); /* Avoid Compile Warning */
	if(ullBufferSize <= 0)
	{
		PrintExit;
		return NULL;
	}
	pBufferMgr = (SSTR_BUFFERMGR_t*)VK_MEM_Alloc(sizeof(SSTR_BUFFERMGR_t));
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> pBufferMgr is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pBufferMgr->ullBufferSize = ullBufferSize;
	pBufferMgr->pBuffer = (HINT8 *)DD_MEM_Alloc(ullBufferSize);
	if(pBufferMgr->pBuffer == NULL)
	{
 		if(pBufferMgr != NULL)
	 	{
			VK_MEM_Free(pBufferMgr);
		}
		PrintExit;
		return NULL;
	}
	pBufferMgr->ullReadPos = 0;
	pBufferMgr->ullWritePos = 0;
	pBufferMgr->bWriteLock = FALSE;
	pBufferMgr->bReadLock= FALSE;
	VK_memset (tmpString, 0, sizeof(tmpString));
	VK_snprintf ((char*)tmpString,VK_strlen("SSTR_BUFFER_SEMA_")+10,"SSTR_BUFFER_SEMA_%p",(void*)pBufferMgr); /* + 10 is AddressSize */
	nRet = VK_SEM_Create(&pBufferMgr->ulSema,(char*)tmpString, VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nRet )
	{
		PrintError ("%s(%d) Error> SSTR Controller Buffer Sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nRet );
		if(pBufferMgr->pBuffer != NULL)
		{
			VK_MEM_Free(pBufferMgr->pBuffer);
			if(pBufferMgr != NULL)
			{
				VK_MEM_Free(pBufferMgr);
			}
		}
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pBufferMgr;
}

SSTR_BUFF_ERR	SSTR_BUFFERMGR_Close (SSTR_BUFFERMGR_t *hBufferMgr)
{
	PrintEnter;
	if(hBufferMgr == NULL)
	{
		PrintExit;
		return SSTR_BUFF_FAIL;
	}
	if(hBufferMgr->pBuffer != NULL)
	{
		DD_MEM_Free(hBufferMgr->pBuffer);
	}
	hBufferMgr->pBuffer = NULL;
	hBufferMgr->ullBufferSize = 0;
	hBufferMgr->ullReadPos = 0;
	hBufferMgr->ullWritePos = 0;
	hBufferMgr->bWriteLock = FALSE;
	hBufferMgr->bReadLock= FALSE;
	VK_SEM_Destroy(hBufferMgr->ulSema);
/*
	if(hBufferMgr != NULL)
	{
		DD_MEM_Free(hBufferMgr);
	}
	hBufferMgr = NULL;
*/
	PrintExit;
	return SSTR_BUFF_OK;
}

SSTR_BUFF_ERR	SSTR_BUFFERMGR_Clear (SSTR_BUFFERMGR_t *hBufferMgr)
{
	PrintEnter;
	if(hBufferMgr == NULL)
	{
		PrintExit;
		return SSTR_BUFF_FAIL;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	hBufferMgr->ullReadPos = 0;
	hBufferMgr->ullWritePos = 0;
	hBufferMgr->bWriteLock = 0;
	VK_MEM_Memset(hBufferMgr->pBuffer,0x00,hBufferMgr->ullBufferSize);
	VK_SEM_Release(hBufferMgr->ulSema);
	PrintExit;
	return SSTR_BUFF_OK;
}

HUINT64	SSTR_BUFFERMGR_Write (SSTR_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize)
{
//	PrintEnter;
	HUINT64 ullSubSize = 0;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}

	VK_SEM_Get(hBufferMgr->ulSema);
	if(hBufferMgr->ullWritePos == hBufferMgr->ullReadPos && hBufferMgr->bWriteLock == TRUE)
	{
//		PrintDebug ("%s(%d) Debug> can't Write(close to ReadPos :%"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,hBufferMgr->ullReadPos,ullSize,hBufferMgr->ullWritePos);
		VK_SEM_Release(hBufferMgr->ulSema);
		return 0;
	}
	if(hBufferMgr->ullWritePos + ullSize > hBufferMgr->ullBufferSize)
	{
		ullSubSize = (hBufferMgr->ullWritePos + ullSize) - hBufferMgr->ullBufferSize;
		ullSize = hBufferMgr->ullBufferSize - hBufferMgr->ullWritePos;
	}
	if( hBufferMgr->ullWritePos < hBufferMgr->ullReadPos )
	{
		if( (hBufferMgr->ullWritePos + ullSize) >= hBufferMgr->ullReadPos)
		{
			ullSize = hBufferMgr->ullReadPos - hBufferMgr->ullWritePos;
			hBufferMgr->bWriteLock = TRUE;
			VK_memcpy(hBufferMgr->pBuffer + hBufferMgr->ullWritePos,pData,ullSize);
	//		PrintError("%s(%d) Debug> can't Write(close to ReadPos :%"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,pBufferMgr->ullReadPos,ullSize,pBufferMgr->ullWritePos);
			hBufferMgr->ullWritePos += ullSize;
			VK_SEM_Release(hBufferMgr->ulSema);
			return ullSize;
		}
	}
//	PrintError("%s(%d) Debug> Write %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,ullSize,pBufferMgr->ullWritePos);
	VK_memcpy(hBufferMgr->pBuffer + hBufferMgr->ullWritePos,pData,ullSize);
	if(ullSubSize > 0)
	{
		if( ullSubSize >= hBufferMgr->ullReadPos )
		{
			ullSubSize = hBufferMgr->ullReadPos;
			hBufferMgr->bWriteLock = TRUE;
		}
		// pBufferMgr->ullWritePos	= 0;
//		PrintError("%s(%d) Debug> Write(Sub) %"PRId64"(%"PRId64") from %"PRId64"(%"PRId64") \n",__FUNCTION__,__LINE__,ullSize,ullSubSize,hBufferMgr->ullWritePos,hBufferMgr->ullReadPos);
		VK_memcpy(hBufferMgr->pBuffer,pData + ullSize,ullSubSize);
		hBufferMgr->ullWritePos = ullSubSize;
		ullSize += ullSubSize;
	}
	else
	{
		hBufferMgr->ullWritePos += ullSize;
	}
//	PrintExit;
	VK_SEM_Release(hBufferMgr->ulSema);
	return ullSize;
}

HUINT64	SSTR_BUFFERMGR_Read (SSTR_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize)
{
	HUINT64 ullSubSize = 0;
//	PrintEnter;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);

	if(hBufferMgr->bReadLock == TRUE)
	{
//		PrintDebug ("%s(%d) Debug> bReadLock(close to WritePos : %"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,hBufferMgr->ullWritePos,ullSize,hBufferMgr->ullReadPos);
//		PrintExit;
		VK_SEM_Release(hBufferMgr->ulSema);
		return 0;
	}
	if(hBufferMgr->ullReadPos == hBufferMgr->ullWritePos && hBufferMgr->bWriteLock == FALSE)
	{
//		PrintDebug ("%s(%d) Debug> can't Read(close to WritePos : %"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,hBufferMgr->ullWritePos,ullSize,hBufferMgr->ullReadPos);
//		PrintExit;
		VK_SEM_Release(hBufferMgr->ulSema);
		return 0;
	}
	if(hBufferMgr->ullReadPos + ullSize > hBufferMgr->ullBufferSize)
	{
		ullSubSize = (hBufferMgr->ullReadPos + ullSize) - hBufferMgr->ullBufferSize;
		ullSize = hBufferMgr->ullBufferSize - hBufferMgr->ullReadPos;
	}

	if( hBufferMgr->ullReadPos < hBufferMgr->ullWritePos )
	{
		if( (hBufferMgr->ullReadPos + ullSize) >= hBufferMgr->ullWritePos)
		{
			ullSize = hBufferMgr->ullWritePos - hBufferMgr->ullReadPos;
			VK_memcpy(pData,hBufferMgr->pBuffer + hBufferMgr->ullReadPos,ullSize);
//			PrintError("%s(%d) Debug> can't Read(close to WritePos : %"PRId64") %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,pBufferMgr->ullWritePos,ullSize,pBufferMgr->ullReadPos);
			hBufferMgr->ullReadPos += ullSize;
			if(hBufferMgr->bWriteLock == TRUE)
			{
				hBufferMgr->bWriteLock = FALSE;
			}
			VK_SEM_Release(hBufferMgr->ulSema);
			PrintExit;
			return ullSize;
		}
	}
	VK_memcpy(pData,hBufferMgr->pBuffer + hBufferMgr->ullReadPos,ullSize);
//	PrintError("%s(%d) Debug> Read  %"PRId64" from %"PRId64" \n",__FUNCTION__,__LINE__,ullSize,pBufferMgr->ullReadPos);
	if(ullSubSize > 0)
	{
		if( ullSubSize > hBufferMgr->ullWritePos )
		{
			ullSubSize = hBufferMgr->ullWritePos;
//			return SSTR_ERR_FAIL_BUFFER_SIZE;
		}
		// pBufferMgr->ullWritePos	= 0;
		VK_memcpy(pData + ullSize,hBufferMgr->pBuffer,ullSubSize);
//		PrintError("%s(%d) Debug> Read(Sub) %"PRId64"(%"PRId64") from %"PRId64"(%"PRId64") \n",__FUNCTION__,__LINE__,ullSize,ullSubSize,hBufferMgr->ullReadPos,hBufferMgr->ullWritePos);
		hBufferMgr->ullReadPos = ullSubSize;
		ullSize += ullSubSize;
	}
	else
	{
		hBufferMgr->ullReadPos += ullSize;
	}
	if(hBufferMgr->bWriteLock == TRUE)
	{
		hBufferMgr->bWriteLock = FALSE;
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	PrintExit;
	return ullSize;
}

HUINT64	SSTR_BUFFERMGR_GetWritePos (SSTR_BUFFERMGR_t *hBufferMgr)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return hBufferMgr->ullWritePos;
}

SSTR_BUFF_ERR	SSTR_BUFFERMGR_SetWritePos (SSTR_BUFFERMGR_t *hBufferMgr,HUINT64 ullWritePos)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_BUFF_FAIL;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_BUFF_FAIL;
	}
	if(hBufferMgr->ullBufferSize <= ullWritePos)
	{
		PrintError("%s(%d) Err> INVALID Position\n",__FUNCTION__,__LINE__);
		return SSTR_BUFF_FAIL;
	}
	hBufferMgr->ullWritePos = ullWritePos;
	return SSTR_BUFF_OK;
}

SSTR_BUFF_ERR	SSTR_BUFFERMGR_SetReadLock (SSTR_BUFFERMGR_t *hBufferMgr,HBOOL bLock)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_BUFF_FAIL;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return SSTR_BUFF_FAIL;
	}
	hBufferMgr->bReadLock = bLock;
	return SSTR_BUFF_OK;
}

HUINT64	SSTR_BUFFERMGR_GetReadPos (SSTR_BUFFERMGR_t *hBufferMgr)
{
//	PrintEnter;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return hBufferMgr->ullReadPos;
}

HUINT64 SSTR_BUFFERMGR_GetWritableSize (SSTR_BUFFERMGR_t *hBufferMgr)
{
	HUINT64 ullSize = 0;
//	PrintEnter;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->ullReadPos == hBufferMgr->ullWritePos && hBufferMgr->bWriteLock == TRUE)
	{
		return 0;
	}
	if(hBufferMgr->ullReadPos > hBufferMgr->ullWritePos)
	{
		ullSize = hBufferMgr->ullReadPos - hBufferMgr->ullWritePos;
	}
	else
	{
		ullSize = (hBufferMgr->ullBufferSize - hBufferMgr->ullWritePos) + hBufferMgr->ullReadPos;
	}
	return ullSize;
}

HUINT64 SSTR_BUFFERMGR_GetReadableSize (SSTR_BUFFERMGR_t *hBufferMgr)
{
	HUINT64 ullSize = 0;
//	PrintEnter;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->ullReadPos == hBufferMgr->ullWritePos && hBufferMgr->bWriteLock == FALSE)
	{
		return 0;
	}
	if(hBufferMgr->ullReadPos < hBufferMgr->ullWritePos)
	{
		ullSize = hBufferMgr->ullWritePos - hBufferMgr->ullReadPos;
	}
	else
	{
		ullSize = (hBufferMgr->ullBufferSize - hBufferMgr->ullReadPos) + hBufferMgr->ullWritePos;
	}
	return ullSize;
}

HUINT64	SSTR_BUFFERMGR_GetBufferSize (SSTR_BUFFERMGR_t *hBufferMgr)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) Err> Buffer is NULL\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return hBufferMgr->ullBufferSize;
}

/*@}*/
