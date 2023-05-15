/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   abr_queue.c  $
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
#include "di_uart.h"
#include "abr_queue.h"

#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"
#include "abr_mpdparser.h"
#include "util_url.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define ABR_QUEUE_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef ABR_QUEUE_DEBUG
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

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

#if _________static________
#endif

/*----------------------------------------------------------------------------------------------
 *	static functions
 *---------------------------------------------------------------------------------------------*/

#if __________p__________
#endif

#if __________di__________
#endif

/*----------------------------------------------------------------------------------------------
 *	External API
 *---------------------------------------------------------------------------------------------*/

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

ABR_ERR ABR_QUEUE_GetNodeInfoByOffset(ABR_QUEUE_t* pQueue, const HINT64 sz_offset, ABR_QUEUE_NODE_t* pNode)
{
	ABR_QUEUE_NODE_t* rightNode = NULL;
	//HINT32 rightId = 0;
	//HUINT64 rightBitrate = 0;
	HINT32 nCur = 0;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	nCur = pQueue->nFront;
	if((pQueue->nodeSet+nCur)->offset + (pQueue->nodeSet+nCur)->header_size > sz_offset)
	{
		PrintDebug("%s(%d) Err> sz_offset is not EXIST\n",__FUNCTION__,__LINE__);
		PrintDebug("%s(%d) Debug> Front(%d:%d) is %"PRId64"\n",__FUNCTION__,__LINE__,(pQueue->nodeSet+nCur)->nId,(pQueue->nodeSet+nCur)->nType,(pQueue->nodeSet+nCur)->offset);
		PrintExit;
		VK_SEM_Release(pQueue->ulSema);
		return -1;
	}
	do
	{
		HINT64 left = 0;
		// HINT64 right = 0;

		left = (pQueue->nodeSet+nCur)->offset + (pQueue->nodeSet+nCur)->header_size;

		// PrintDebug("%s(%d) Debug> left sz_offset(%d) is %"PRId64"\n",__FUNCTION__,__LINE__,(pQueue->nodeSet+nCur)->nId,left);

		if((nCur + 1) == pQueue->nSize)
		{
			rightNode = (pQueue->nodeSet);
			//right = (pQueue->nodeSet)->offset;
			//rightId = (pQueue->nodeSet)->nId;
			//rightBitrate = (pQueue->nodeSet)->ullBitrate;
		}
		else
		{
			rightNode = (pQueue->nodeSet+nCur+1);
			//right = (pQueue->nodeSet+nCur+1)->offset;
			//rightId = (pQueue->nodeSet+nCur+1)->nId;
			//rightBitrate = (pQueue->nodeSet+nCur+1)->ullBitrate;
		}
//		PrintDebug("%s(%d) Debug> left(%"PRId64") right sz_offset(%d) is %"PRId64"\n",__FUNCTION__,__LINE__,left,rightNode->nId,rightNode->offset);
		if(left <= sz_offset && rightNode->offset + rightNode->header_size > sz_offset)
		{
			pNode->nId = rightNode->nId;
			pNode->nType = rightNode->nType;
			pNode->offset = rightNode->offset;
			pNode->range  = rightNode->range;
			pNode->ullBitrate = rightNode->ullBitrate;
			pNode->bufferOffset = rightNode->bufferOffset;
			pNode->bComplete = rightNode->bComplete;
			pNode->header_size = rightNode->header_size;
//			PrintDebug("%s(%d) Debug> sz_voffset: %"PRId64" , nId : %d , ullBitrate : %lld\n",__FUNCTION__,__LINE__,rightNode->offset,rightNode->nId,rightNode->ullBitrate);
			PrintExit;
			VK_SEM_Release(pQueue->ulSema);
			return ABR_OK;
		}
		nCur++;
		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
		if(nCur == pQueue->nRear && rightNode->offset == sz_offset)
		{
			pNode->nId = rightNode->nId + 1;
			pNode->offset = 0;
			pNode->range  = 0;
			pNode->ullBitrate = 0;
			pNode->bufferOffset = 0;
			PrintExit;
			VK_SEM_Release(pQueue->ulSema);
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
	/*
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
	*/
	pNode->nId = -1;
	pNode->offset = -1;
	pNode->range  = -1;
	pNode->ullBitrate = -1;
	pNode->bufferOffset = -1;
	PrintExit;
	PrintDebug("%s(%d) Err> sz_offset(%"PRId64") is not EXIST\n",__FUNCTION__,__LINE__,sz_offset);
	VK_SEM_Release(pQueue->ulSema);
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
ABR_ERR ABR_QUEUE_GetCurNodeInfo(ABR_QUEUE_t* pQueue, ABR_QUEUE_NODE_t* pNode)
{
	ABR_ERR nRet = ABR_FAIL;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> Queue is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(pNode == NULL)
	{
		PrintError("%s(%d) Err> Node is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	pNode->nId = (pQueue->nodeSet+pQueue->nCur)->nId;
	pNode->nType = (pQueue->nodeSet+pQueue->nCur)->nType;
	pNode->offset = (pQueue->nodeSet+pQueue->nCur)->offset;
	pNode->ullBitrate = (pQueue->nodeSet+pQueue->nCur)->ullBitrate;
	pNode->range = (pQueue->nodeSet+pQueue->nCur)->range;
	pNode->bufferOffset = (pQueue->nodeSet+pQueue->nCur)->bufferOffset;
	pNode->bComplete = (pQueue->nodeSet+pQueue->nCur)->bComplete;
	pNode->header_size = (pQueue->nodeSet+pQueue->nCur)->header_size;
	nRet = ABR_OK;
	PrintExit;
	VK_SEM_Release(pQueue->ulSema);
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
ABR_ERR ABR_QUEUE_GetNodeInfoBySegment(ABR_QUEUE_t* pQueue, HINT32 nId, HINT32 nType,  ABR_QUEUE_NODE_t* pNode)
{
	ABR_ERR nRet = ABR_FAIL;
	HINT32 nCur = 0;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> Queue is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	if(pNode == NULL)
	{
		PrintError("%s(%d) Err> Node is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	nCur = pQueue->nFront + 1;
	do
	{
		if((pQueue->nodeSet+nCur)->nId == nId && (pQueue->nodeSet+nCur)->nType == nType )
		{
			pNode->nId = (pQueue->nodeSet+pQueue->nCur)->nId;
			pNode->offset = (pQueue->nodeSet+pQueue->nCur)->offset;
			pNode->ullBitrate = (pQueue->nodeSet+pQueue->nCur)->ullBitrate;
			pNode->range = (pQueue->nodeSet+pQueue->nCur)->range;
			pNode->bufferOffset = (pQueue->nodeSet+pQueue->nCur)->bufferOffset;
			pNode->header_size = (pQueue->nodeSet+pQueue->nCur)->header_size;
			pNode->nType = (pQueue->nodeSet+pQueue->nCur)->nType;
			pNode->bComplete= (pQueue->nodeSet+pQueue->nCur)->bComplete;
			nRet = ABR_OK;
			break;
		}
		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
	}while(nCur++ != pQueue->nRear);
	VK_SEM_Release(pQueue->ulSema);
	PrintExit;
	return nRet;
}

HBOOL ABR_QUEUE_GetComplete(ABR_QUEUE_t* pQueue, HINT32 nId)
{
	ABR_ERR nRet = ABR_FAIL;
	HINT32 nCur = 0;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	nCur = pQueue->nFront + 1;
	do
	{
		if((pQueue->nodeSet+nCur)->nId == nId)
		{
			return (pQueue->nodeSet+pQueue->nCur)->bComplete;
		}
		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
	}while(nCur++ != pQueue->nRear);
	VK_SEM_Release(pQueue->ulSema);
	PrintExit;
	return nRet;
}

ABR_ERR ABR_QUEUE_SetComplete(ABR_QUEUE_t* pQueue, HINT32 nId, HBOOL bComplete)
{
	ABR_ERR nRet = ABR_FAIL;
	HINT32 nCur = 0;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	nCur = pQueue->nFront + 1;
	do
	{
		if((pQueue->nodeSet+nCur)->nId == nId)
		{
			(pQueue->nodeSet+pQueue->nCur)->bComplete = bComplete;
//			PrintDebug("%s(%d) Debug> Segment(%d) Complete\n",__FUNCTION__,__LINE__,nId);
			nRet = ABR_OK;
			break;
		}
		if(nCur == pQueue->nSize)
		{
			nCur = 0;
		}
	}while(nCur++ != pQueue->nRear);
	VK_SEM_Release(pQueue->ulSema);
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

ABR_ERR ABR_QUEUE_PushBack(ABR_QUEUE_t* pQueue, ABR_QUEUE_NODE_t tNode)
{
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> MPD is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);

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
#if DI_IPTUNER_MPD_CONFIG_BUFFER_TYPE
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
		(pQueue->nodeSet+(pQueue->nRear+1))->offset = (pQueue->nodeSet+pQueue->nRear)->offset + tNode.range;
		pQueue->nRear++;
	}
	else
	{
		pQueue->nodeSet->offset = (pQueue->nodeSet+pQueue->nRear)->offset + tNode.range;
		pQueue->nRear=0;
	}
	(pQueue->nodeSet+pQueue->nRear)->nId = tNode.nId;
	(pQueue->nodeSet+pQueue->nRear)->nType = tNode.nType;
	(pQueue->nodeSet+pQueue->nRear)->ullBitrate = tNode.ullBitrate;
	(pQueue->nodeSet+pQueue->nRear)->range = tNode.range;
	(pQueue->nodeSet+pQueue->nRear)->bufferOffset = tNode.bufferOffset;
	(pQueue->nodeSet+pQueue->nRear)->bComplete = tNode.bComplete;
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
	PrintDebug("%s(%d) Debug> Pushed(%d) ID: %d, Size : %lld, offset : %lld, Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nRear,tNode.nId,tNode.range,(pQueue->nodeSet+(pQueue->nRear))->offset,tNode.ullBitrate);
	PrintExit;
	VK_SEM_Release(pQueue->ulSema);
	return ABR_OK;
}

#if 0 /* ABR_QUEUE_PushFront is not used */
ABR_ERR ABR_QUEUE_PushFront(DI_ABR_HANDLE hMpd, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate)
{
	ABR_QUEUE_t *pQueue = NULL;
	DI_IPTUNER_MPD_t* pMpd = NULL;
	mpdData_t *mpdData = NULL;
	PrintEnter;

	if(hMpd == NULL || ((DI_IPTUNER_MPD_t*)hMpd)->parser == NULL
		|| ((DI_IPTUNER_MPD_t*)hMpd)->parser->mpdData == NULL)
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
ABR_ERR ABR_QUEUE_Replace(ABR_QUEUE_t* pQueue, ABR_QUEUE_NODE_t tNode)
{
	HINT32 nCur = 0;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> Queue is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	nCur = pQueue->nFront + 1;
	do
	{
		if(nCur >= pQueue->nSize)
		{
			nCur = 0;
		}
		if((pQueue->nodeSet+nCur)->nId == tNode.nId && (pQueue->nodeSet+nCur)->nType == tNode.nType )
		{
			(pQueue->nodeSet+nCur)->offset = tNode.offset;
			(pQueue->nodeSet+nCur)->range = tNode.range;
			(pQueue->nodeSet+nCur)->bufferOffset = tNode.bufferOffset;
			(pQueue->nodeSet+nCur)->bComplete = tNode.bComplete;
			(pQueue->nodeSet+nCur)->header_size = tNode.header_size;
			break;
		}
	}while(nCur++ != pQueue->nRear);
	PrintDebug("%s(%d) Debug> Replace(%d) ID: %d Size : %lld , Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nCur,tNode.nId,tNode.range,tNode.ullBitrate);
	VK_SEM_Release(pQueue->ulSema);
	PrintExit;
	return ABR_OK;
}

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
ABR_ERR ABR_QUEUE_Realign(ABR_QUEUE_t* pQueue, HINT64 sz_Base, ABR_QUEUE_NODE_t tNode)
{
	HINT32 nCur = 0;
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> Queue is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);

	nCur = pQueue->nFront + 1;
	do
	{
		if(nCur >= pQueue->nSize)
		{
			nCur = 0;
		}
		if((pQueue->nodeSet+nCur)->nId == tNode.nId && (pQueue->nodeSet+nCur)->nType == tNode.nType )
		{
			(pQueue->nodeSet+nCur)->offset = sz_Base + tNode.range;
			(pQueue->nodeSet+nCur)->range = tNode.range;
			(pQueue->nodeSet+nCur)->bufferOffset = tNode.bufferOffset;
			(pQueue->nodeSet+nCur)->bComplete = tNode.bComplete;
			(pQueue->nodeSet+nCur)->header_size = tNode.header_size;
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
			if((pQueue->nodeSet+nCur)->range >= (pQueue->nodeSet+nCur)->offset)
			{
				(pQueue->nodeSet+nCur-1)->offset = 0;
			}
			else
			{
			(pQueue->nodeSet+nCur-1)->offset = (pQueue->nodeSet+nCur)->offset - (pQueue->nodeSet+nCur)->range;
		}

		}
		else
		{
			if((pQueue->nodeSet+nCur)->range >= (pQueue->nodeSet+nCur)->offset)
			{
				(pQueue->nodeSet)->offset = 0;
			}
		else
		{
			(pQueue->nodeSet)->offset = (pQueue->nodeSet+nCur)->offset - (pQueue->nodeSet+nCur)->range;
		}
		}
		nCur--;
		if(nCur < 0)
		{
			nCur = pQueue->nSize;
		}
	}

#if DI_IPTUNER_MPD_CONFIG_BUFFER_TYPE
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

	PrintDebug("%s(%d) Debug> Replace(%d) ID: %d Size : %lld , Bitrate : %lld\n",__FUNCTION__,__LINE__,pQueue->nCur,tNode.nId,tNode.range,tNode.ullBitrate);
	VK_SEM_Release(pQueue->ulSema);
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
ABR_ERR ABR_QUEUE_Reset(ABR_QUEUE_t* pQueue, HINT64 nBase)
{
	PrintEnter;
	if(pQueue == NULL || pQueue->nodeSet == NULL)
	{
		PrintError("%s(%d) Err> Queue is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return ABR_FAIL;
	}
	VK_SEM_Get(pQueue->ulSema);
	pQueue->nCur = 0;
	pQueue->nFront = 0;
	pQueue->nRear = 0;
	VK_MEM_Memset(pQueue->nodeSet,0x00,sizeof(ABR_QUEUE_NODE_t) * pQueue->nSize);
	(pQueue->nodeSet+pQueue->nFront)->offset = nBase;
	(pQueue->nodeSet+pQueue->nFront)->nId = -1;
	VK_SEM_Release(pQueue->ulSema);
	PrintExit;
	return ABR_OK;
}
