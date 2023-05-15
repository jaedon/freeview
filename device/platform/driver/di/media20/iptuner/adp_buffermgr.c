/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   adp_buffermgr.c  $
 * Version:			$Revision:   0.2  $
 * Original Author:		YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2012.08.31
 * File Description:	Adaptive Buffer Manager
 * Module:
 * Remarks:
 */

/**
 * @defgroup ADP_BUFFERMGR
 * @brief Adaptive Buffer Manager 이며 Buffer를 Block 단위로 관리 가능 하다.
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 31 Aug 2012
 */

/**
 * @brief Adaptive Buffer Manager 이며 Buffer를 Block 단위로 관리 가능 하다.
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
 * @file adp_buffermgr.c
 */

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
#include "di_uart.h"
#include "adp_buffermgr.h"

#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
//#define ADP_BUFFERMGR_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef ADP_BUFFERMGR_DEBUG
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

/*----------------------------------------------------------------------------------------------
 *	PRIVATE Function
 *---------------------------------------------------------------------------------------------*/
static ADP_BUFFER_ERR P_ADP_BUFFERMGR_DestroyFreeList (ADP_BUFFERMGR_t*	pBufferMgr)
{
	UTIL_DLLNode_t *pNode = NULL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	pNode = UTIL_DLLIST_GetHeadNode(&pBufferMgr->freeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t*)pNode->pData;
		if(pBlock != NULL)
		{
			VK_MEM_Free(pBlock);
			pNode->pData = NULL;
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	if(UTIL_DLLIST_Reset(&pBufferMgr->freeList) != DLL_ERR_OK)
	{
		PrintError("%s(%d) Reset List Fail \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_LIST;
	}
	return ADP_BUFFER_OK;
}

static ADP_BUFFER_ERR P_ADP_BUFFERMGR_FlushActiveList (ADP_BUFFERMGR_t*	pBufferMgr)
{
	UTIL_DLLNode_t *pNode = NULL;
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	pNode = UTIL_DLLIST_Extract_toEnd(&pBufferMgr->activeList,0);
	if(pNode != NULL)
	{
		if(UTIL_DLLIST_AppendNode(&pBufferMgr->freeList, pNode) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Append Block to FreeList Fail \n\r",__FUNCTION__,__LINE__);
			return ADP_BUFFER_LIST;
		}
	}
	pBufferMgr->readBlockNode = NULL;
	pBufferMgr->writeBlockNode = NULL;
	pBufferMgr->ullReadOffset = 0;
	pBufferMgr->ullWriteOffset = 0;
	pBufferMgr->tStatus.ullUsedSize=0;
	pBufferMgr->tStatus.ullFreedSize=pBufferMgr->tStatus.ullTotalSize;
	pBufferMgr->tStatus.ulUsedBlocks=0;
	pBufferMgr->tStatus.ulFreedBlocks=pBufferMgr->tStatus.ulTotalBlocks;
	pBufferMgr->tStatus.ullReadableSize=0;
	pBufferMgr->tStatus.unSegments=0;
	return ADP_BUFFER_OK;
}

static UTIL_DLLNode_t* P_ADP_BUFFERMGR_BlockAllocation (ADP_BUFFERMGR_t*	pBufferMgr)
{
	UTIL_DLLNode_t *pNode = NULL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return NULL;
	}
	pNode = UTIL_DLLIST_ExtractHead(&pBufferMgr->freeList);
	if(pNode != NULL)
	{
		if(UTIL_DLLIST_AppendNode(&pBufferMgr->activeList, pNode) != DLL_ERR_OK)
		{
			PrintError("%s(%d) <Critical> Append Block to Active Fail \n\r",__FUNCTION__,__LINE__);
			if(UTIL_DLLIST_PreppendNode(&pBufferMgr->freeList, pNode) != DLL_ERR_OK)
			{
				PrintError("%s(%d) <Critical> Preppend Block to FreeList Fail \n\r",__FUNCTION__,__LINE__);
			}
			/* PrependNode */
			return NULL;
		}
		else
			pBlock = (ADP_BUFFER_Block_t*)pNode->pData;
	}

	if(pBlock != NULL)
	{
		pBlock->ulFreed = pBlock->ulBlockSize;
		pBlock->ulCarved = 0;
		pBlock->bStart = FALSE;
		pBlock->bEnd = FALSE;
		pBlock->unRefCnt = 0;
		pBlock->ullStartOffset = 0;
		pBlock->unInterest = 0;
		VK_MEM_Memset(&pBlock->tUserData,0x00,sizeof(ADP_BUFFER_UserData_t));
		pBufferMgr->tStatus.ulUsedBlocks+=1;
		pBufferMgr->tStatus.ulFreedBlocks-=1;
	}

	return pNode;
}

static ADP_BUFFER_ERR P_ADP_BUFFERMGR_BlockDeallocation (ADP_BUFFERMGR_t*	pBufferMgr, ADP_BUFFER_Block_t *pBlock)
{
	UTIL_DLLNode_t *pNode = NULL;
	HBOOL bEndBlock = FALSE;
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pBlock->unRefCnt != 0)
	{
		PrintError("%s(%d) Block is referenced \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_REFBLOCK;
	}
	if(pBlock->bEnd == TRUE)
	{
		bEndBlock = TRUE;
	}
	pNode = UTIL_DLLIST_ExtractNodeByDataAddr(&pBufferMgr->activeList,pBlock);
	if(pNode != NULL)
	{
		if(UTIL_DLLIST_AppendNode(&pBufferMgr->freeList, pNode) != DLL_ERR_OK)
		{
			PrintError("%s(%d) <Critical> Append Block to Free Fail \n\r",__FUNCTION__,__LINE__);
			/* PrependNode */
			return ADP_BUFFER_CRITICAL;
		}
	}
	pBufferMgr->tStatus.ulUsedBlocks-=1;
	pBufferMgr->tStatus.ulFreedBlocks+=1;
	pBufferMgr->tStatus.ullUsedSize-=pBufferMgr->tStatus.ulBlockSize;
	pBufferMgr->tStatus.ullFreedSize+=pBufferMgr->tStatus.ulBlockSize;
	if(bEndBlock == TRUE)
	{
		pBufferMgr->tStatus.unSegments-=1;
	}
	return ADP_BUFFER_OK;
}


/*----------------------------------------------------------------------------------------------
*  GLOBAL Function
*---------------------------------------------------------------------------------------------*/

 /*!
  *  \brief Buffer Manager 생성
  *  \param pConfig : Configuration
  *  \return Buffer Manager Handle
  *  \see
  */
ADP_BUFFERMGR_t* ADP_BUFFERMGR_Create (ADP_BUFFERMGR_CONFIG_t *pConfig)
{
	ADP_BUFFERMGR_t* pBufferMgr = NULL;
	HINT8 sem_name[128]={0};
	HINT32 nRet = 0;
	if(pConfig == NULL)
	{
		PrintError("%s(%d) Config is NULL",__FUNCTION__,__LINE__);
		return NULL;
	}
	if(pConfig->ullBufferSize == 0 || pConfig->ullBufferSize == 0)
	{
		PrintError("%s(%d) Configuration has INVALID VALUE",__FUNCTION__,__LINE__);
		return NULL;
	}
	pBufferMgr = VK_MEM_Alloc(sizeof(ADP_BUFFERMGR_t));
	if(pBufferMgr == NULL)
	{
		PrintError("%s(%d) Memory Alloc Fail",__FUNCTION__,__LINE__);
		return NULL;
	}
	else
	{
		VK_MEM_Memset(pBufferMgr,0x00,sizeof(ADP_BUFFERMGR_t));
	}
	if(pConfig->eType == ADP_BUFFERTYPE_MEMORY)
	{
		HUINT32 ulBlock = pConfig->ullBufferSize / pConfig->ulBlockSize;
		if(ulBlock * pConfig->ulBlockSize < pConfig->ullBufferSize)
		{
			ulBlock++;
		}
		pBufferMgr->tStatus.eType = pConfig->eType;
		pBufferMgr->tStatus.eMode = pConfig->eMode;
		pBufferMgr->tStatus.ulBlockSize = pConfig->ulBlockSize;
		pBufferMgr->tStatus.ullTotalSize = ulBlock * pConfig->ulBlockSize;
		pBufferMgr->tStatus.ullFreedSize = pBufferMgr->tStatus.ullTotalSize;
		pBufferMgr->tStatus.ullUsedSize = 0;
		pBufferMgr->tStatus.ulTotalBlocks = ulBlock;
		pBufferMgr->tStatus.ulFreedBlocks = pBufferMgr->tStatus.ulTotalBlocks;
		pBufferMgr->tStatus.ulUsedBlocks = 0;
		pBufferMgr->pBuffer = VK_MEM_Alloc(pBufferMgr->tStatus.ullTotalSize);
		if(pBufferMgr->pBuffer == NULL)
		{
			PrintError("%s(%d) Memory Alloc Fail",__FUNCTION__,__LINE__);
			goto create_fail;
		}
		for(HUINT32 i = 0 ; i < ulBlock ; i++)
		{
			ADP_BUFFER_Block_t *pBlock = NULL;
			pBlock = VK_MEM_Alloc(sizeof(ADP_BUFFER_Block_t));
			if(pBlock == NULL)
			{
				PrintError("%s(%d) Memory Alloc Fail",__FUNCTION__,__LINE__);
				goto create_fail;
			}
			else
			{
				VK_MEM_Memset(pBlock,0x00,sizeof(ADP_BUFFER_Block_t));
			}
			pBlock->id = i;
			pBlock->pBuffer = pBufferMgr->pBuffer + (i * pBufferMgr->tStatus.ulBlockSize);
			pBlock->ulBlockSize = pBufferMgr->tStatus.ulBlockSize;
			if(UTIL_DLLIST_Append(&pBufferMgr->freeList, pBlock) != DLL_ERR_OK)
			{
				if(pBlock != NULL)
				{
					VK_MEM_Free(pBlock);
				}
				PrintError("%s(%d) <Critical> Append Block to FreeList Fail",__FUNCTION__,__LINE__);
				goto create_fail;
			}
		}
	}
	else
	{
		PrintError("%s(%d) Not Supported Yet",__FUNCTION__,__LINE__);
		VK_MEM_Free(pBufferMgr);
		return NULL;
	}
	VK_snprintf((char*)sem_name,VK_strlen("ADP_BUFFERMGR_Sema_")+10, "ADP_BUFFERMGR_Sema_%p", (void*)pBufferMgr);
	nRet = VK_SEM_Create((unsigned long*)&pBufferMgr->ulSema, (const char*)sem_name, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Create Semaphore Fail\n\r",__FUNCTION__,__LINE__, nRet);
		goto create_fail;
	}
	return pBufferMgr;

	create_fail:
	ADP_BUFFERMGR_Destroy(pBufferMgr);
	return NULL;
}

 /*!
  *  \brief Buffer Manager 제거
  *  \param hBufferMgr : Buffer Manager Handle
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_Destroy (ADP_BUFFERMGR_t *hBufferMgr)
{
	HULONG ulSema = 0;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	ulSema = hBufferMgr->ulSema;
	VK_SEM_Get(ulSema);
	if(P_ADP_BUFFERMGR_FlushActiveList(hBufferMgr) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) [Critical] Flush ActiveList Fail\n\r",__FUNCTION__,__LINE__);
	}
	if(P_ADP_BUFFERMGR_DestroyFreeList(hBufferMgr) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) [Critical] Destroy FreeList Fail\n\r",__FUNCTION__,__LINE__);
	}
	if(hBufferMgr->pBuffer != NULL)
	{
		VK_MEM_Free(hBufferMgr->pBuffer);
		hBufferMgr->pBuffer = NULL;
	}
	if(hBufferMgr != NULL)
	{
		VK_MEM_Free(hBufferMgr);
		hBufferMgr = NULL;
	}
	VK_SEM_Release(ulSema);
	VK_SEM_Destroy(ulSema);
	return ADP_BUFFER_OK;
}

 /*!
  *  \brief Buffer Manager
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param pUserData : UserData
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_Open (ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFER_UserData_t *pUserData)
{
	UTIL_DLLNode_t *pNode = NULL;
	ADP_BUFFER_ERR errCode = ADP_BUFFER_OK;
	ADP_BUFFER_Block_t *pBlock = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	if(hBufferMgr->writeBlockNode != NULL)
	{
		PrintError("%s(%d) [WARN] Current Write Block is not closed \n\r",__FUNCTION__,__LINE__);
		errCode = ADP_BUFFER_NOTCLOSED;
		goto open_error;
	}
	if(pUserData == NULL)
	{
		PrintError("%s(%d) [WARN] User Data is NULL \n\r",__FUNCTION__,__LINE__);
		errCode = ADP_BUFFER_INVALIDPARAM;
		goto open_error;
	}
	pNode = P_ADP_BUFFERMGR_BlockAllocation(hBufferMgr);
	if(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t*)pNode->pData;
		if(pBlock == NULL)
		{
			PrintError("%s(%d) [Critical] Block is NULL \n\r",__FUNCTION__,__LINE__);
			errCode = ADP_BUFFER_CRITICAL;
			goto open_error;
		}
		pBlock->bStart = TRUE;
		pBlock->ullStartOffset = pUserData->ullStartOffset;
		pBlock->unRefCnt = 1;
		VK_MEM_Memcpy(&pBlock->tUserData,pUserData,sizeof(ADP_BUFFER_UserData_t));
		hBufferMgr->ullWriteOffset = 0;
		hBufferMgr->writeBlockNode = pNode;
		if(hBufferMgr->readBlockNode == NULL)
		{
			pBlock->unRefCnt++;
			hBufferMgr->readBlockNode = pNode;
			hBufferMgr->ullReadOffset = 0;
		}
	}
	else
	{
		errCode = ADP_BUFFER_NOFREEBLOCK;
		goto open_error;
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;

open_error:
#if 0 /* deadcode */
	if(pBlock != NULL)
	{
		if(P_ADP_BUFFERMGR_BlockDeallocation(hBufferMgr,pBlock) != ADP_BUFFER_OK)
		{
			errCode = ADP_BUFFER_CRITICAL;
		}
	}
#endif
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

 /*!
  *  \brief Buffer Manager 제거
  *  \param hBufferMgr : Buffer Manager Handle
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_Close (ADP_BUFFERMGR_t *hBufferMgr)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_OK;
	ADP_BUFFER_Block_t *pCurBlock = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	if(hBufferMgr->writeBlockNode == NULL)
	{
		PrintError("%s(%d) [WARN] Current Write Block is already closed \n\r",__FUNCTION__,__LINE__);
		errCode = ADP_BUFFER_ISCLOSED;
		goto close_error;
	}
	pCurBlock = (ADP_BUFFER_Block_t *)hBufferMgr->writeBlockNode->pData;
	if(pCurBlock != NULL)
	{
		if(pCurBlock->bEnd == TRUE)
		{
			PrintError("%s(%d) [WARN] Current Write Block is already closed \n\r",__FUNCTION__,__LINE__);
			errCode = ADP_BUFFER_ISCLOSED;
			goto close_error;
		}
		pCurBlock->bEnd = TRUE;
		hBufferMgr->writeBlockNode = NULL;
		if(pCurBlock->unRefCnt != 0)
		{
			pCurBlock->unRefCnt--;
		}
		hBufferMgr->tStatus.unSegments+=1;
	}
	else
	{
		PrintError("%s(%d) [WARN] Current Write Block is already closed \n\r",__FUNCTION__,__LINE__);
		errCode = ADP_BUFFER_ISCLOSED;
		goto close_error;
	}
close_error:
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}


 /*!
  *  \brief Buffer Manager 비움
  *  \param hBufferMgr : Buffer Manager Handle
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_Flush (ADP_BUFFERMGR_t *hBufferMgr)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_OK;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_FAIL;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	errCode = P_ADP_BUFFERMGR_FlushActiveList(hBufferMgr);
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

 /*!
  *  \brief Buffer에 Write
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param pData : Write 할 Data
  *  \param ullSize : Data 크기
  *  \return Write 된 크기
  *  \see
  */
HUINT64	ADP_BUFFERMGR_Write (ADP_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize)
{
	HUINT64 ullResult = 0;
	HUINT64 ullWriteSize = 0;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		ullResult = 0;
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	if(hBufferMgr->bWriteLock == TRUE)
	{
		ullResult = 0;
		goto write_error;
	}
	if(hBufferMgr->writeBlockNode == NULL)
	{
		PrintError("%s(%d) [WARN] Current Write Block is NULL \n\r",__FUNCTION__,__LINE__);
		ullResult = 0;
		goto write_error;
	}
	pCurBlock = (ADP_BUFFER_Block_t *)hBufferMgr->writeBlockNode->pData;
	if(pCurBlock != NULL)
	{
		while(ullSize > 0)
		{
			if((pCurBlock->ulFreed == 0 || pCurBlock->bEnd == TRUE) && ullSize > 0)
			{
				pNode = P_ADP_BUFFERMGR_BlockAllocation(hBufferMgr);
				if(pNode == NULL)
				{
					PrintError("%s(%d) pNode is null. \n\n", __FUNCTION__, __LINE__);
					break;
				}

				pBlock = (ADP_BUFFER_Block_t*)pNode->pData;
				if(pBlock != NULL)
				{
					pBlock->ullStartOffset = pCurBlock->ullStartOffset + pCurBlock->ulCarved;
					pBlock->unRefCnt++;
					VK_MEM_Memcpy(&pBlock->tUserData,&pCurBlock->tUserData,sizeof(ADP_BUFFER_UserData_t));
					pCurBlock->unRefCnt--;

					pCurBlock = pBlock;
					hBufferMgr->writeBlockNode = pNode;
					hBufferMgr->ullWriteOffset = 0;
				}
				else
				{
					PrintError("%s(%d) [Critical] Current Write Block Data is NULL \n\r",__FUNCTION__,__LINE__);
					ullResult = 0;
					goto write_error;
				}
			}

			if(ullSize > pCurBlock->ulFreed)
			{
				ullWriteSize = pCurBlock->ulFreed;
			}
			else
			{
				ullWriteSize = ullSize;
			}

			if((pCurBlock->pBuffer - hBufferMgr->pBuffer) + (hBufferMgr->ullWriteOffset + ullWriteSize)
			> hBufferMgr->tStatus.ullTotalSize)
			{
				PrintError("%s(%d) [Critical] Buffer Overflow \n\r",__FUNCTION__,__LINE__);
				goto write_error;
			}

			if(hBufferMgr->ullWriteOffset != pCurBlock->ulCarved)
			{
				PrintError("%s(%d) [Critical] Current Write Offset is INVALID\n\r",__FUNCTION__,__LINE__);
				goto write_error;
			}

			VK_MEM_Memcpy(pCurBlock->pBuffer + hBufferMgr->ullWriteOffset,pData+ullResult,ullWriteSize);

			pCurBlock->ulCarved += ullWriteSize;
			hBufferMgr->ullWriteOffset += ullWriteSize;
			pCurBlock->ulFreed -= ullWriteSize;
			hBufferMgr->tStatus.ullFreedSize -= ullWriteSize;
			hBufferMgr->tStatus.ullUsedSize += ullWriteSize;
			hBufferMgr->tStatus.ullReadableSize += ullWriteSize;
			if((pCurBlock->ulCarved + pCurBlock->ulFreed) != pCurBlock->ulBlockSize)
			{
				PrintError("%s(%d) [Critical] Current Write Block Size is INVALID\n\r",__FUNCTION__,__LINE__);
				goto write_error;
			}

			ullSize -= ullWriteSize;
			ullResult += ullWriteSize;
		}
	}
	else
	{
		PrintError("%s(%d) [Critical] Current Write Block Data is NULL \n\r",__FUNCTION__,__LINE__);
		ullResult = 0;
		goto write_error;
	}
write_error:
	VK_SEM_Release(hBufferMgr->ulSema);
	return ullResult;
}

 /*!
  *  \brief Buffer로 부터 Read
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param pData : Read 할 Data Buffer
  *  \param ullSize : Read 할 크기
  *  \return Read 된 크기
  *  \see
  */
HUINT64 ADP_BUFFERMGR_Read (ADP_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize)
{
	HUINT64 ullResult = 0;
	HUINT64 ullReadSize = 0;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		ullResult = 0;
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	if(hBufferMgr->bReadLock == TRUE)
	{
		ullResult = 0;
		goto read_error;
	}
	if(hBufferMgr->readBlockNode == NULL)
	{
		PrintError("%s(%d) [WARN] Current Read Block is NULL \n\r",__FUNCTION__,__LINE__);
		ullResult = 0;
		goto read_error;
	}
	pNode = hBufferMgr->readBlockNode;
	pCurBlock = (ADP_BUFFER_Block_t *)pNode->pData;
	if(pCurBlock != NULL)
	{
		if(pCurBlock->ulCarved == 0)
		{
			PrintDebug("%s(%d) Current Read Block is Empty \n\r",__FUNCTION__,__LINE__);
			ullResult = 0;
			goto read_error;
		}
		while(ullSize > 0)
		{
			if(ullSize > pCurBlock->ulCarved - hBufferMgr->ullReadOffset)
			{
				ullReadSize = pCurBlock->ulCarved - hBufferMgr->ullReadOffset;
			}
			else
			{
				ullReadSize = ullSize;
			}
			if(hBufferMgr->ullReadOffset > pCurBlock->ulCarved)
			{
				PrintError("%s(%d) [Critical] Current Read Offset is INVALID\n\r",__FUNCTION__,__LINE__);
				goto read_error;
			}
			if((pCurBlock->pBuffer - hBufferMgr->pBuffer) + (hBufferMgr->ullReadOffset + ullReadSize)
			> hBufferMgr->tStatus.ullTotalSize)
			{
				PrintError("%s(%d) [Critical] Buffer Overflow \n\r",__FUNCTION__,__LINE__);
				goto read_error;
			}
			if( ullReadSize > 0 )
			{
				VK_MEM_Memcpy(pData+ullResult,pCurBlock->pBuffer + hBufferMgr->ullReadOffset,ullReadSize);
				hBufferMgr->ullReadOffset += ullReadSize;
				hBufferMgr->tStatus.ullReadableSize -= ullReadSize;
				ullSize -= ullReadSize;
				ullResult += ullReadSize;
			}
			if(ullSize > 0)
			{
				if(((pCurBlock->ulBlockSize == hBufferMgr->ullReadOffset) || (hBufferMgr->ullReadOffset == pCurBlock->ulCarved))
				&& ((pCurBlock->bEnd != TRUE) || hBufferMgr->tStatus.eMode == ADP_BUFFERMODE_SEQUENCIAL))
				{
					pNode = UTIL_DLLIST_NextNode(pNode);
					if(pNode == NULL)
					{
						//ullResult = 0;
						goto read_error;
					}
					pBlock = (ADP_BUFFER_Block_t*)pNode->pData;
					if(pBlock != NULL)
					{
						pBlock->unRefCnt++;
						pBlock->unInterest++;
						pCurBlock->unRefCnt--;
						pCurBlock = pBlock;
						hBufferMgr->readBlockNode = pNode;
						hBufferMgr->ullReadOffset = 0;
					}
					else
					{
						PrintError("%s(%d) [Critical] Current Read Block Data is NULL \n\r",__FUNCTION__,__LINE__);
						//ullResult = 0;
						goto read_error;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	else
	{
		PrintError("%s(%d) [Critical] Current Read Block Data is NULL \n\r",__FUNCTION__,__LINE__);
		ullResult = 0;
		goto read_error;
	}
read_error:
	VK_SEM_Release(hBufferMgr->ulSema);
	return ullResult;
}

/*!
 *  \brief Block의 Userdata를 얻음
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param unCategory : Type or Adaption Id(Stream Id) 등으로 사용
 *  \param unSegId : Segment Id
 *  \param[out] pUserData : 찾은 Segment의 UserData
 *  \return ErrorCode
 *  \see
*/
ADP_BUFFER_ERR   ADP_BUFFERMGR_GetUserData (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, ADP_BUFFER_UserData_t *pUserData)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_FAIL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pUserData == NULL)
	{
		PrintError("%s(%d) [WARN] Userdata is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_INVALIDPARAM;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pBlock != NULL)
		{
			if(pBlock->tUserData.ulCategory == unCategory
			&& pBlock->tUserData.ulSegId == unSegId)
			{
				VK_MEM_Memcpy(pUserData,&pBlock->tUserData,sizeof(ADP_BUFFER_UserData_t));
				errCode = ADP_BUFFER_OK;
				break;
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

/*!
 *  \brief Segment Id와 Offset으로 Block Id와 Offset 찾음
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param unCategory : Type or Adaption Id(Stream Id) 등으로 사용
 *  \param unSegId : Segment Id
 *  \param ullOffset : Segment Offset
 *  \param[out] punBlockId : 찾은 Block Id
 *  \param[out] pullBlockOffset : 찾은 Block 내 Offset
 *  \return ErrorCode
 *  \see
*/
ADP_BUFFER_ERR 	 ADP_BUFFERMGR_FindBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullOffset, HUINT32 *punBlockId, HUINT64 *pullBlockOffset)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_FAIL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}

	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pBlock != NULL)
		{
			if(pBlock->tUserData.ulCategory == unCategory
			&& pBlock->tUserData.ulSegId == unSegId)
			{
				if(pBlock->ullStartOffset + pBlock->ulCarved > ullOffset
				&& pBlock->ullStartOffset <= ullOffset )
				{
					*punBlockId = pBlock->id;
					*pullBlockOffset = ullOffset - pBlock->ullStartOffset;
					errCode = ADP_BUFFER_OK;
					break;
				}
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

/*!
 *	\brief Offset으로 Block Id와 Offset 찾음 (for None UserData)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param ullOffset : Segment Offset
 *	\param[out] punBlockId : 찾은 Block Id
 *	\param[out] pullBlockOffset : 찾은 Block 내 Offset
 *	\return ErrorCode
 *	\see
 */
ADP_BUFFER_ERR 	 ADP_BUFFERMGR_FindBlockByOffset (ADP_BUFFERMGR_t *hBufferMgr, HUINT64 ullOffset, HUINT32 *punBlockId, HUINT64 *pullBlockOffset)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_FAIL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pBlock != NULL)
		{
			if(pBlock->ullStartOffset + pBlock->ulCarved > ullOffset
			&& pBlock->ullStartOffset <= ullOffset )
			{
				*punBlockId = pBlock->id;
				*pullBlockOffset = ullOffset - pBlock->ullStartOffset;
				errCode = ADP_BUFFER_OK;
				break;
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

/*!
 *	\brief Segment Id와 Time으로 Block Id와 Offset 찾음
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param unCategory : Type or Adaption Id(Stream Id) 등으로 사용
 *	\param unSegId : Segment Id
 *	\param ullTime : Time
 *	\param[out] punBlockId : 찾은 Block Id
 *	\param[out] pullBlockOffset : 찾은 Block 내 Offset
 *	\return ErrorCode
 *	\see
 */
ADP_BUFFER_ERR ADP_BUFFERMGR_FindBlockByTime (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullTime, HUINT32 *punBlockId, HUINT64 *pullBlockOffset)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_FAIL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;

	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pBlock != NULL)
		{
			if(pBlock->tUserData.ulCategory == unCategory
			&& pBlock->tUserData.ulSegId == unSegId)
			{
				if(pBlock->tUserData.ullStartTime + pBlock->tUserData.ullDuration > ullTime
				&& pBlock->tUserData.ullStartTime <= ullTime )
				{
					*punBlockId = pBlock->id;
					*pullBlockOffset = 0;
					errCode = ADP_BUFFER_OK;
					break;
				}
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

 /*!
  *  \brief Buffer Read 방지 Lock Setting
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param bLock : Lock 여부
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_SetReadLock (ADP_BUFFERMGR_t *hBufferMgr, HBOOL bLock)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	hBufferMgr->bReadLock = bLock;
	VK_SEM_Release(hBufferMgr->ulSema);
	return ADP_BUFFER_OK;
}

 /*!
  *  \brief Buffer Write 방지 Lock Setting
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param bLock : Lock 여부
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_SetWriteLock (ADP_BUFFERMGR_t *hBufferMgr, HBOOL bLock)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	hBufferMgr->bWriteLock = bLock;
	VK_SEM_Release(hBufferMgr->ulSema);
	return ADP_BUFFER_OK;
}

 /*!
  *  \brief Read Offset 설정
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param ulReadBlock : 설정할 Read Block
  *  \param ullReadPos : 설정할 Read Offset
  *  \return ADP_BUFFER_ERR
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_SetReadPos (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 ulReadBlockId, HUINT64 ullReadPos)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_FAIL;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	UTIL_DLLNode_t *pCurNode = NULL;
	HUINT32 ulCarved = 0;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pBlock != NULL)
		{
			if(pBlock->id == ulReadBlockId)
			{
				if(pBlock->ulCarved < ullReadPos)
				{
					PrintError("%s(%d) [WARN] ReadOffset is INVALID \n\r",__FUNCTION__,__LINE__);
					errCode = ADP_BUFFER_INVALIDPARAM;
					break;
				}
				else
				{
					pCurNode = hBufferMgr->readBlockNode;
					if(pCurNode != NULL)
					{
						pCurBlock = (ADP_BUFFER_Block_t *)pCurNode->pData;
						if(pCurBlock != NULL)
						{
							pCurBlock->unRefCnt--;
						}
					}
					pBlock->unRefCnt++;
					pBlock->unInterest++;
					hBufferMgr->ullReadOffset = ullReadPos;
					hBufferMgr->readBlockNode = pNode;
					hBufferMgr->tStatus.ullReadableSize = hBufferMgr->tStatus.ullUsedSize - (ulCarved + ullReadPos);
					errCode = ADP_BUFFER_OK;
					break;
				}
			}
			else
				ulCarved += pBlock->ulCarved;
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

 /*!
  *  \brief Head Block 부터 refcnt가 없는 Block에 한하여 FreeList로 이동 (Write 를 위한 Block 확보)
  *  \param hBufferMgr : Buffer Manager Handle
  *  \return 확보한 Block 갯수
  *  \see
  */
HUINT32 ADP_BUFFERMGR_SimpleRetainBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unMaxBlocks)
{
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	HUINT32 unResult = 0;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	while(unMaxBlocks > unResult)
	{
		pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
		if(pNode != NULL)
		{
			pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
			if(pBlock != NULL)
			{
				if(pBlock->unRefCnt == 0)
				{
					if(P_ADP_BUFFERMGR_BlockDeallocation (hBufferMgr, pBlock) != ADP_BUFFER_OK)
					{
						PrintError("%s(%d) <Critical> Block to Free Fail \n\r",__FUNCTION__,__LINE__);
						break;
					}
					unResult++;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return unResult;
}

 /*!
  *  \brief Head Block 부터 refcnt가 없는 Block에 한하여 FreeList로 이동 (Write 를 위한 Block 확보)
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param unCategory :  제거 대상 카테고리
  *  \param unSegId :  제거 대상 Segment
  *  \param ullBefore :  제거 제한 Offset  (해당 Offset 이전 Block 만 제거)
  *  \param unMaxBlocks : 최대 확보 Block
  *  \return 확보한 Block 갯수
  *  \note 확보 대상은 기준 Offset이 없는 경우 오래된 것 부터 제거, Write된 데이터가 없는 경우 제거, 기준 Offset 이전 Block 제거
  *  \see
  */
HUINT32 ADP_BUFFERMGR_RetainBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullBefore, HUINT32 unMaxBlocks)
{
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	HUINT32 unResult = 0;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(unMaxBlocks > unResult)
	{
		if(pNode != NULL && pNode != hBufferMgr->writeBlockNode)
		{
			if(pNode != hBufferMgr->readBlockNode)
			{
				pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
				if(pBlock != NULL)
				{
					if((pBlock->tUserData.ulSegId == unSegId && pBlock->tUserData.ulCategory == unCategory)
					&& (ullBefore == 0 || pBlock->ulCarved == 0 || pBlock->tUserData.ullStartOffset + pBlock->ulCarved < ullBefore))
					{
						if(pBlock->unRefCnt == 0)
						{
							pNode = UTIL_DLLIST_NextNode(pNode);
							// PrintError("%s(%d) <Debug> Retain Block (%llu ~ %llu) Before %llu \n\r",__FUNCTION__,__LINE__,pBlock->tUserData.ullStartOffset,pBlock->tUserData.ullStartOffset + pBlock->ulCarved,ullBefore);
							if(P_ADP_BUFFERMGR_BlockDeallocation (hBufferMgr, pBlock) != ADP_BUFFER_OK)
							{
								PrintError("%s(%d) <Critical> Block to Free Fail \n\r",__FUNCTION__,__LINE__);
								break;
							}
							unResult++;
							continue;
						}
						else
						{
							PrintError("%s(%d) <Critical> Bad Block \n\r",__FUNCTION__,__LINE__);
							break;
						}
					}
				}
			}
		}
		else
		{
			break;
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return unResult;
}

 /*!
 *	\brief Head Block 부터 refcnt가 없는 Block에 한하여 사용 빈도가 적은 Block 을 FreeList로 이동 (Write 를 위한 Block 확보)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param unCategory :  제거 대상 카테고리
 *	\param unSegId :  제거 대상 Segment
 *	\param ullBefore : 제거 제한 Offset  (해당 Offset 이전 Block 만 제거)
 *	\param unMaxBlocks : 최대 확보 Block
 *	\return 확보한 Block 갯수
 * 	\note 제한 Offset 설정이 중요하다.  제한 Offset 기준으로 한번 이상은 사용이 되었어야 할 Block 들을 대상으로 사용해야 한다.
 *	\see
 */
HUINT32 ADP_BUFFERMGR_RetainUninterestedBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullBefore)
{
	ADP_BUFFER_Block_t *pBlock = NULL;
	ADP_BUFFER_Block_t *pUnintrestedBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	HUINT32 unResult = 0;
	HUINT32 unMinInterested = 0;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL && pNode != hBufferMgr->writeBlockNode)
	{
		if(pNode != hBufferMgr->readBlockNode)
		{
			pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
			if(pBlock != NULL)
			{
				if((pBlock->tUserData.ulSegId == unSegId && pBlock->tUserData.ulCategory == unCategory)
				&& ( ullBefore == 0 || pBlock->tUserData.ullStartOffset + pBlock->ulCarved < ullBefore))
				{
					if(pBlock->unRefCnt == 0)
					{
						if(pBlock->unInterest == 0)
						{
							pUnintrestedBlock = pBlock;
							break;
						}
						else
						{
							if( unMinInterested == 0 || unMinInterested > pBlock->unInterest)
							{
								unMinInterested = pBlock->unInterest;
								pUnintrestedBlock = pBlock;
							}
						}
					}
					else
					{
						PrintError("%s(%d) <Critical> Bad Block \n\r",__FUNCTION__,__LINE__);
						break;
					}
				}
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	if(pUnintrestedBlock != NULL)
	{
		if(P_ADP_BUFFERMGR_BlockDeallocation (hBufferMgr, pUnintrestedBlock) != ADP_BUFFER_OK)
		{
			PrintError("%s(%d) <Critical> Block to Free Fail \n\r",__FUNCTION__,__LINE__);
		}
		else
		{
			unResult++;
		}
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return unResult;
}

/*!
  *  \brief Buffer 의 크기를 재조정 한다. (현재 크기 보다 작게 변경 불가능)
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param ullSize : 재조정할 크기
  *  \return 최종 Buffer 크기
  *  \see
  */
HUINT64		ADP_BUFFERMGR_Resize (ADP_BUFFERMGR_t *hBufferMgr, HUINT64 ullSize)
{
	HBOOL bReadBlock = FALSE;
	HBOOL bWriteBlock = FALSE;
	HUINT32 unReadBlockId = 0;
	HUINT32 unWriteBlockId = 0;
	UTIL_DLLNode_t *pNode = NULL;
	if(hBufferMgr == NULL || hBufferMgr->pBuffer == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	if(ullSize <= hBufferMgr->tStatus.ullTotalSize)
	{
		PrintError("%s(%d) [WARN] Resizing value must greater than current total size\n\r",__FUNCTION__,__LINE__);
		goto resize_end;
	}
	if(hBufferMgr->tStatus.eType == ADP_BUFFERTYPE_MEMORY)
	{
		HUINT32 ulBlock = ullSize / hBufferMgr->tStatus.ulBlockSize;
		HUINT8 *pNewBuffer = NULL;
		HUINT32 ulCurBlocks = hBufferMgr->tStatus.ulTotalBlocks;
		ADP_BUFFER_Block_t *pBlock = NULL;
		if(ulBlock * (HUINT64)hBufferMgr->tStatus.ulBlockSize < ullSize)
		{
			ulBlock++;
		}
		if(ulBlock <= ulCurBlocks)
		{
			goto resize_end;
		}
		pNewBuffer = VK_MEM_Alloc(ulBlock * hBufferMgr->tStatus.ulBlockSize);
		if(pNewBuffer == NULL)
		{
			PrintError("%s(%d) Memory Alloc Fail",__FUNCTION__,__LINE__);
			goto resize_end;
		}
		else
		{
			VK_MEM_Memset(pNewBuffer,0x00,ulBlock * hBufferMgr->tStatus.ulBlockSize);
			VK_MEM_Memcpy(pNewBuffer,hBufferMgr->pBuffer,hBufferMgr->tStatus.ullTotalSize);
			if(hBufferMgr->readBlockNode != NULL)
			{
				pBlock = (ADP_BUFFER_Block_t *)hBufferMgr->readBlockNode->pData;
				if(pBlock != NULL)
				{
					bReadBlock = TRUE;
					unReadBlockId = pBlock->id;
				}
				else
				{
					PrintError("%s(%d) <Critical> Block is NULL",__FUNCTION__,__LINE__);
					VK_MEM_Free(pNewBuffer);
					goto resize_end;
				}
			}
			if(hBufferMgr->writeBlockNode != NULL)
			{
				pBlock = (ADP_BUFFER_Block_t *)hBufferMgr->writeBlockNode->pData;
				if(pBlock != NULL)
				{
					bWriteBlock = TRUE;
					unWriteBlockId = pBlock->id;
				}
				else
				{
					PrintError("%s(%d) <Critical> Block is NULL",__FUNCTION__,__LINE__);
					VK_MEM_Free(pNewBuffer);
					goto resize_end;
				}
			}
			pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
			while(pNode != NULL)
			{
				pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
				if(pBlock != NULL)
				{
					pBlock->pBuffer = pNewBuffer + pBlock->id * hBufferMgr->tStatus.ulBlockSize;
					if(bWriteBlock == TRUE && pBlock->id == unWriteBlockId)
					{
						hBufferMgr->writeBlockNode = pNode;
						bWriteBlock = FALSE;
					}
					if(bReadBlock == TRUE && pBlock->id == unReadBlockId)
					{
						hBufferMgr->readBlockNode = pNode;
						bReadBlock = FALSE;
					}
				}
				else
				{
					PrintError("%s(%d) <Critical> Block is NULL",__FUNCTION__,__LINE__);
				}
				pNode = UTIL_DLLIST_NextNode(pNode);
			}
			pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->freeList);
			while(pNode != NULL)
			{
				pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
				if(pBlock != NULL)
				{
					pBlock->pBuffer = pNewBuffer + pBlock->id * hBufferMgr->tStatus.ulBlockSize;
					if(bWriteBlock == TRUE && pBlock->id == unWriteBlockId)
					{
						hBufferMgr->writeBlockNode = pNode;
						bWriteBlock = FALSE;
					}
					if(bReadBlock == TRUE && pBlock->id == unReadBlockId)
					{
						hBufferMgr->readBlockNode = pNode;
						bReadBlock = FALSE;
					}
				}
				else
				{
					PrintError("%s(%d) <Critical> Block is NULL",__FUNCTION__,__LINE__);
				}
				pNode = UTIL_DLLIST_NextNode(pNode);
			}
			VK_MEM_Free(hBufferMgr->pBuffer);
			hBufferMgr->pBuffer = pNewBuffer;
		}
		ulBlock -= ulCurBlocks;
		for(HUINT32 i = 0 ; i < ulBlock ; i++)
		{
			pBlock = VK_MEM_Alloc(sizeof(ADP_BUFFER_Block_t));
			if(pBlock == NULL)
			{
				PrintError("%s(%d) Memory Alloc Fail",__FUNCTION__,__LINE__);
				goto resize_end;
			}
			else
			{
				VK_MEM_Memset(pBlock,0x00,sizeof(ADP_BUFFER_Block_t));
			}
			pBlock->id = ulCurBlocks + i;
			pBlock->pBuffer = hBufferMgr->pBuffer + ((ulCurBlocks + i) * hBufferMgr->tStatus.ulBlockSize);
			pBlock->ulBlockSize = hBufferMgr->tStatus.ulBlockSize;
			if(UTIL_DLLIST_Append(&hBufferMgr->freeList, pBlock) != DLL_ERR_OK)
			{
				if(pBlock != NULL)
				{
					VK_MEM_Free(pBlock);
				}
				PrintError("%s(%d) <Critical> Append Block to FreeList Fail",__FUNCTION__,__LINE__);
				goto resize_end;
			}
			hBufferMgr->tStatus.ulTotalBlocks++;
			hBufferMgr->tStatus.ulFreedBlocks++;
			hBufferMgr->tStatus.ullTotalSize += hBufferMgr->tStatus.ulBlockSize;
			hBufferMgr->tStatus.ullFreedSize += hBufferMgr->tStatus.ulBlockSize;
		}
	}
	else
	{
		PrintError("%s(%d) Not Supported Yet",__FUNCTION__,__LINE__);
		goto resize_end;
	}
	resize_end:
	VK_SEM_Release(hBufferMgr->ulSema);
	return hBufferMgr->tStatus.ullTotalSize;
}

 /*!
  *  \brief 현재 Write 중인 Block을 Flush 시킨다.
  *  \param hBufferMgr : Buffer Manager Handle
  *  \see
  */
ADP_BUFFER_ERR ADP_BUFFERMGR_FlushWriteBlock (ADP_BUFFERMGR_t *hBufferMgr)
{
	ADP_BUFFER_ERR errCode = ADP_BUFFER_FAIL;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	UTIL_DLLNode_t *pCurNode = NULL;
	UTIL_DLLNode_t *pDestNode = NULL;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return 0;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pCurNode = pNode = hBufferMgr->writeBlockNode;
	if(pCurNode != NULL && pCurNode->pData != NULL)
	{
		pCurBlock = (ADP_BUFFER_Block_t *)pCurNode->pData;
		while(pNode != NULL)
		{
			if(pNode == hBufferMgr->readBlockNode)
			{
				break;
			}
			pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
			if(pBlock != NULL)
			{
				if(pBlock->tUserData.ulSegId == pCurBlock->tUserData.ulSegId
				|| pBlock->tUserData.ulCategory == pCurBlock->tUserData.ulCategory)
				{
					pDestNode = pNode;
				}
				else
				{
					break;
				}
			}
			else
			{
				pDestNode = NULL;
				PrintError("%s(%d) <Critical> Block is NULL \n\r",__FUNCTION__,__LINE__);
				errCode = ADP_BUFFER_FAIL;
				break;
			}
			if(pBlock->bStart == TRUE)
			{
				break;
			}
			pNode = UTIL_DLLIST_PrevNode(pNode);
		}
		if(pDestNode != NULL)
		{
			HBOOL bFlushEnd = FALSE;
			UTIL_DLLNode_t *pPrevNode = NULL;
			errCode = ADP_BUFFER_OK;
			pNode = hBufferMgr->writeBlockNode;
			hBufferMgr->writeBlockNode = NULL;
			hBufferMgr->ullWriteOffset = 0;
			while(pNode != NULL)
			{
				pPrevNode = UTIL_DLLIST_PrevNode(pNode);
				if(pDestNode == pNode || pPrevNode == NULL)
				{
					bFlushEnd = TRUE;
				}
				pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
				if(pBlock != NULL)
				{
					HUINT32 ulCarved = pBlock->ulCarved;
					pBlock->unRefCnt = 0;
					if(P_ADP_BUFFERMGR_BlockDeallocation (hBufferMgr, pBlock) != ADP_BUFFER_OK)
					{
						PrintError("%s(%d) <Critical> Block to Free Fail \n\r",__FUNCTION__,__LINE__);
						errCode = ADP_BUFFER_FAIL;
						break;
					}
					else
						hBufferMgr->tStatus.ullReadableSize-=ulCarved;
				}
				if(bFlushEnd == TRUE)
				{
					break;
				}
				pNode = pPrevNode;
			}
		}
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return errCode;
}

/*!
 *  \brief Block의 크기를 얻는다.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pSize : Block 크기
 *  \return ADP_BUFFER_ERR
 *  \see
*/
ADP_BUFFER_ERR ADP_BUFFERMGR_GetBlockSize (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 *pSize)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pSize == NULL)
	{
		PrintError("%s(%d) [WARN] INVALID PARAM \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_INVALIDPARAM;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	*pSize = hBufferMgr->tStatus.ulBlockSize;
	VK_SEM_Release(hBufferMgr->ulSema);
	return ADP_BUFFER_OK;
}

/*!
 *  \brief Status 를 얻는다.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pStatus : Status
 *  \return ADP_BUFFER_ERR
 *  \see
*/
ADP_BUFFER_ERR ADP_BUFFERMGR_GetStatus (ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_STATUS_t *pStatus)
{
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pStatus == NULL)
	{
		PrintError("%s(%d) [WARN] INVALID PARAM \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_INVALIDPARAM;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	VK_MEM_Memcpy(pStatus,&hBufferMgr->tStatus,sizeof(ADP_BUFFERMGR_STATUS_t));
	VK_SEM_Release(hBufferMgr->ulSema);
	return ADP_BUFFER_OK;
}

/*!
 *  \brief Read Block의 Snapshot 를 얻는다.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pSnapshot : Block Snapshot
 *  \return ADP_BUFFER_ERR
 *  \see
*/
ADP_BUFFER_ERR ADP_BUFFERMGR_GetReaderSnapshot(ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_BlockSnapshot_t *pSnapshot)
{
	ADP_BUFFER_ERR nRet = ADP_BUFFER_OK;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pSnapshot == NULL)
	{
		PrintError("%s(%d) [WARN] Status Pointer is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_INVALIDPARAM;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = hBufferMgr->readBlockNode;
	if(pNode == NULL)
	{
		PrintError("%s(%d) [WARN] Read Node is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_FAIL;
	}
	pCurBlock = (ADP_BUFFER_Block_t *)pNode->pData;
	if(pCurBlock != NULL)
	{
		if(hBufferMgr->ullReadOffset > pCurBlock->ulCarved)
		{
			PrintError("%s(%d) <Critical> Offset is Invalid \n\r",__FUNCTION__,__LINE__);
			nRet = ADP_BUFFER_CRITICAL;
		}
		else
		{
			pSnapshot->hdl = pNode;
			pSnapshot->unBlockId = pCurBlock->id;
			pSnapshot->unInterest = pCurBlock->unInterest;
			pSnapshot->ulBlockSize = pCurBlock->ulBlockSize;
			pSnapshot->ulFreedSize = pCurBlock->ulFreed;
			pSnapshot->ulCarvedSize = pCurBlock->ulCarved;
			pSnapshot->ullStartOffset = pCurBlock->ullStartOffset;
			pSnapshot->unRefCnt = pCurBlock->unRefCnt;
			pSnapshot->ulReadableSize = pCurBlock->ulCarved - hBufferMgr->ullReadOffset;
			pSnapshot->pBuffer = pCurBlock->pBuffer + hBufferMgr->ullReadOffset;
			if(pCurBlock->bEnd == TRUE)
			{
				pSnapshot->ulWritableSize = 0;
			}
			else
				pSnapshot->ulWritableSize = pCurBlock->ulFreed;
		}
	}
	else
	{
		PrintError("%s(%d) <Critical> Block Data is NULL\n\r",__FUNCTION__,__LINE__);
		nRet = ADP_BUFFER_CRITICAL;
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return nRet;
}

/*!
 *  \brief Write Block의 Snapshot 를 얻는다.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pSnapthot : Write Block Snapshot
 *  \return ADP_BUFFER_ERR
 *  \see
*/
ADP_BUFFER_ERR ADP_BUFFERMGR_GetWriterSnapshot(ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_BlockSnapshot_t *pSnapshot)
{
	ADP_BUFFER_ERR nRet = ADP_BUFFER_OK;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pSnapshot == NULL)
	{
		PrintError("%s(%d) [WARN] Status Pointer is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_INVALIDPARAM;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	pNode = hBufferMgr->writeBlockNode;
	pCurBlock = (ADP_BUFFER_Block_t *)pNode->pData;
	if(pCurBlock != NULL)
	{
		if(hBufferMgr->ullWriteOffset > pCurBlock->ulCarved)
		{
			PrintError("%s(%d) <Critical> Offset is Invalid \n\r",__FUNCTION__,__LINE__);
			nRet = ADP_BUFFER_CRITICAL;
		}
		else
		{
			pSnapshot->hdl = pNode;
			pSnapshot->unBlockId = pCurBlock->id;
			pSnapshot->unInterest = pCurBlock->unInterest;
			pSnapshot->ulBlockSize = pCurBlock->ulBlockSize;
			pSnapshot->ulFreedSize = pCurBlock->ulFreed;
			pSnapshot->ulCarvedSize = pCurBlock->ulCarved;
			pSnapshot->ullStartOffset = pCurBlock->ullStartOffset;
			pSnapshot->unRefCnt = pCurBlock->unRefCnt;
			pSnapshot->ulReadableSize = pCurBlock->ulCarved;
			pSnapshot->pBuffer = pCurBlock->pBuffer;
			if(pCurBlock->bEnd == TRUE)
			{
				pSnapshot->ulWritableSize = 0;
			}
			else
				pSnapshot->ulWritableSize = pCurBlock->ulFreed;
		}
	}
	else
	{
		PrintError("%s(%d) <Critical> Block Data is NULL\n\r",__FUNCTION__,__LINE__);
		nRet = ADP_BUFFER_CRITICAL;
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return nRet;
}

/*!
 *  \brief 다음 Block의 Snapshot 를 얻는다. Snapshot의 hdl이 NULL 이면 첫 Block의 Snapshot을 얻는다.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pSnapthot : Next Block Snapshot
 *  \return ADP_BUFFER_ERR
 *  \see
*/
ADP_BUFFER_ERR 	 ADP_BUFFERMGR_NextSnapshot(ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_BlockSnapshot_t *pSnapshot)
{
	ADP_BUFFER_ERR nRet = ADP_BUFFER_OK;
	ADP_BUFFER_Block_t *pCurBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_NOHANDLE;
	}
	if(pSnapshot == NULL)
	{
		PrintError("%s(%d) [WARN] Status Pointer is NULL \n\r",__FUNCTION__,__LINE__);
		return ADP_BUFFER_INVALIDPARAM;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	if(pSnapshot->hdl == NULL)
	{
		pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	}
	else
	{
		pNode = UTIL_DLLIST_NextNode((UTIL_DLLNode_t *)pSnapshot->hdl);
	}
	if(pNode == NULL)
	{
		nRet = ADP_BUFFER_NOMOREDATA;
	}
	else
	{
		pCurBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pCurBlock != NULL)
		{
			pSnapshot->hdl = pNode;
			pSnapshot->unInterest = pCurBlock->unInterest;
			pSnapshot->ulBlockSize = pCurBlock->ulBlockSize;
			pSnapshot->ulFreedSize = pCurBlock->ulFreed;
			pSnapshot->ulCarvedSize = pCurBlock->ulCarved;
			pSnapshot->ullStartOffset = pCurBlock->ullStartOffset;
			pSnapshot->unRefCnt = pCurBlock->unRefCnt;
			pSnapshot->ulReadableSize = pCurBlock->ulCarved;
			pSnapshot->pBuffer = pCurBlock->pBuffer;
			if(pCurBlock->bEnd == TRUE)
			{
				pSnapshot->ulWritableSize = 0;
			}
			else
				pSnapshot->ulWritableSize = pCurBlock->ulFreed;
		}
		else
		{
			PrintError("%s(%d) <Critical> Block Data is NULL\n\r",__FUNCTION__,__LINE__);
			nRet = ADP_BUFFER_CRITICAL;
		}
	}
	VK_SEM_Release(hBufferMgr->ulSema);
	return nRet;
}

void ADP_BUFFERMGR_PrintStatus (ADP_BUFFERMGR_t *hBufferMgr)
{
	ADP_BUFFER_Block_t *pBlock = NULL;
	UTIL_DLLNode_t *pNode = NULL;
	if(hBufferMgr == NULL)
	{
		PrintError("%s(%d) [WARN] Buffer Manager is NULL \n\r",__FUNCTION__,__LINE__);
		return;
	}
	VK_SEM_Get(hBufferMgr->ulSema);
	PrintDebug("-----------------------------------------------------------\n");
	PrintDebug("+  Buffer Manager Status\n");
	PrintDebug("-----------------------------------------------------------\n");
	PrintDebug("|  TYPE : %s \n",hBufferMgr->tStatus.eType ? "FILE" : "MEMORY");
	PrintDebug("|  Block Size : %u \n",hBufferMgr->tStatus.ulBlockSize);
	PrintDebug("|  Total Buffer Size : %u \n",hBufferMgr->tStatus.ullTotalSize);
	PrintDebug("|  Used Buffer Size : %u \n",hBufferMgr->tStatus.ullUsedSize);
	PrintDebug("|  Freed Buffer Size : %u \n",hBufferMgr->tStatus.ullFreedSize);
	PrintDebug("|  Total Blocks : %u \n",hBufferMgr->tStatus.ulTotalBlocks);
	PrintDebug("|  Used Blocks : %u \n",hBufferMgr->tStatus.ulUsedBlocks);
	PrintDebug("|  Freed Blocks : %u \n",hBufferMgr->tStatus.ulFreedBlocks);
	PrintDebug("-----------------------------------------------------------\n");
	PrintDebug("+  Actice Block Status\n");
	PrintDebug("-----------------------------------------------------------\n");
	pNode = UTIL_DLLIST_GetHeadNode(&hBufferMgr->activeList);
	while(pNode != NULL)
	{
		pBlock = (ADP_BUFFER_Block_t *)pNode->pData;
		if(pBlock != NULL)
		{
			PrintDebug("+ Block %u (%s) ",pBlock->id,pBlock->bStart ? "Opened" : (pBlock->bEnd ? "Closed" : "-----"));
			if(pNode == hBufferMgr->readBlockNode)
			{
				PrintDebug("<-- READ BLOCK(%llu)",hBufferMgr->ullReadOffset);
			}
			if(pNode == hBufferMgr->writeBlockNode)
			{
				PrintDebug("<-- WRITE BLOCK(%llu)",hBufferMgr->ullWriteOffset);
			}
			PrintDebug("\n-----------------------------------------------------------\n");
			PrintDebug("| Offset %llu\n",pBlock->ullStartOffset);
			PrintDebug("| Carved %u\n",pBlock->ulCarved);
			PrintDebug("| Freed %u\n",pBlock->ulFreed);
			PrintDebug("| Ref %u\n",pBlock->unRefCnt);
		}
		else
		{
			PrintDebug("[!] Missing Block\n");
		}
		PrintDebug("-----------------------------------------------------------\n");
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_SEM_Release(hBufferMgr->ulSema);
}

