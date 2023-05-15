/**
	@file     mwc_ramdisk.c

	Description: MW와 AP가 공통으로 사용할 OCTO RAM Disk
	Module: MWC/RAMDISK			 	\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <hlib.h>

#include "int_ringbuf.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define CHUNK_ID_NULL					(-1)
#define CHUNK_ID_CURRENT				(-2)
#define CHUNK_ARRAY_APPEND_NUM			16

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___INTERNAL_FUNCTIONS___
STATIC HBOOL hlib_ringbuf_varchunk_isBufferWritable (_palRingBuf_VarChunk_t *pstVarChunk)
{
	HUINT32					 ulHead = pstVarChunk->stWritingChunkInfo.ulHeadPtr + 1;

	if (ulHead >= pstVarChunk->ulMaxSize)
	{
		ulHead -= pstVarChunk->ulMaxSize;
	}

	return (ulHead == pstVarChunk->stWritingChunkInfo.ulTailPtr);
}

#define _____CHUNK_INDEX_CACHE_____
STATIC HERROR hlib_ringbuf_varchunk_rearrangeChunkIdAndIndexCache (_palRingBuf_VarChunk_t *pstVarChunk)
{
	HUINT32				 ulSrcCnt, ulDstCnt;

	for (ulSrcCnt = 0, ulDstCnt = 0; ulSrcCnt < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulSrcCnt++)
	{
		if (pstVarChunk->astChunkIndexCache[ulSrcCnt].nChunkId < 0)					{ continue; }

		pstVarChunk->astChunkIndexCache[ulDstCnt].nChunkId		= pstVarChunk->astChunkIndexCache[ulSrcCnt].nChunkId;
		pstVarChunk->astChunkIndexCache[ulDstCnt].nChunkIndex	= pstVarChunk->astChunkIndexCache[ulSrcCnt].nChunkIndex;

		ulDstCnt ++;
	}

	for (; ulDstCnt < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulDstCnt++)
	{
		pstVarChunk->astChunkIndexCache[ulDstCnt].nChunkId		= CHUNK_ID_NULL;
		pstVarChunk->astChunkIndexCache[ulDstCnt].nChunkIndex	= CHUNK_ID_NULL;
	}

	return ERR_OK;
}

STATIC HERROR hlib_ringbuf_varchunk_addChunkIdAndIndexToCache (_palRingBuf_VarChunk_t *pstVarChunk, HUINT32 ulChunkId, HUINT32 ulChunkIndex)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		if (pstVarChunk->astChunkIndexCache[ulCount].nChunkId < 0)
		{
			pstVarChunk->astChunkIndexCache[ulCount].nChunkId		= (HINT32)ulChunkId;
			pstVarChunk->astChunkIndexCache[ulCount].nChunkIndex	= (HINT32)ulChunkIndex;

			return ERR_OK;
		}
	}

	// Cache가 가득찼을 경우:
	pstVarChunk->astChunkIndexCache[0].nChunkId			= CHUNK_ID_NULL;
	pstVarChunk->astChunkIndexCache[0].nChunkIndex		= CHUNK_ID_NULL;

	hlib_ringbuf_varchunk_rearrangeChunkIdAndIndexCache (pstVarChunk);

	pstVarChunk->astChunkIndexCache[HLIB_RINGBUF_CHUNKID_CACHE_SIZE - 1].nChunkId	= (HINT32)ulChunkId;
	pstVarChunk->astChunkIndexCache[HLIB_RINGBUF_CHUNKID_CACHE_SIZE - 1].nChunkIndex	= (HINT32)ulChunkIndex;

	return ERR_OK;

}

STATIC HERROR hlib_ringbuf_varchunk_deleteChunkIdToCache (_palRingBuf_VarChunk_t *pstVarChunk, HUINT32 ulChunkId)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		if ((pstVarChunk->astChunkIndexCache[ulCount].nChunkId > 0) &&
			(pstVarChunk->astChunkIndexCache[ulCount].nChunkId == (HINT32)ulChunkId))
		{
			pstVarChunk->astChunkIndexCache[ulCount].nChunkId		= CHUNK_ID_NULL;
			pstVarChunk->astChunkIndexCache[ulCount].nChunkIndex	= CHUNK_ID_NULL;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR hlib_ringbuf_varchunk_findChunkIndexFromCache (_palRingBuf_VarChunk_t *pstVarChunk, HUINT32 ulChunkId, HUINT32 *pulChunkIndex)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		if (pstVarChunk->astChunkIndexCache[ulCount].nChunkId == (HINT32)ulChunkId)
		{
			*pulChunkIndex = (HUINT32)pstVarChunk->astChunkIndexCache[ulCount].nChunkIndex;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#define _____CHUNK_INDEX_____
STATIC HERROR hlib_ringbuf_varchunk_getChunkIndexFromChunkId (_palRingBuf_VarChunk_t *pstVarChunk, HUINT32 ulChunkId, HUINT32 *pulChunkIndex)
{
	HUINT32				 ulCount;
	HERROR				 hErr;

	hErr = hlib_ringbuf_varchunk_findChunkIndexFromCache (pstVarChunk, ulChunkId, pulChunkIndex);
	if (ERR_OK == hErr)
	{
		return ERR_OK;
	}

	if (NULL != pstVarChunk->pstChunkInfoArray)
	{
		for (ulCount = 0; ulCount < pstVarChunk->ulChunkNum; ulCount++)
		{
			_palRingBuf_VarChunkInfo_t		*pstVarChunkInfo = &(pstVarChunk->pstChunkInfoArray[ulCount]);

			if (pstVarChunkInfo->nChunkId == (HINT32)ulChunkId)
			{
				*pulChunkIndex = ulCount;
				hlib_ringbuf_varchunk_addChunkIdAndIndexToCache (pstVarChunk, ulChunkId, ulCount);
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}


#define _____CHUNK_ITEM_INFO_____
STATIC HERROR hlib_ringbuf_varchunk_rearrangeChunk (_palRingBuf_VarChunk_t *pstVarChunk)
{
	HUINT32				 ulTailPtr = 0;

	if ((pstVarChunk->ulChunkNum > 0) && (NULL != pstVarChunk->pstChunkInfoArray))
	{
		HUINT32						 ulChunkNum = 0;
		HUINT32						 ulSrcIdx, ulDstIdx;
		_palRingBuf_VarChunkInfo_t	*pstSrc, *pstDst;

		for (ulSrcIdx = 0, ulDstIdx = 0; ulSrcIdx < pstVarChunk->ulChunkNum; ulSrcIdx++)
		{
			pstSrc = &(pstVarChunk->pstChunkInfoArray[ulSrcIdx]);
			pstDst = &(pstVarChunk->pstChunkInfoArray[ulDstIdx]);

			if (pstSrc->nChunkId != CHUNK_ID_NULL)					// 유효하면 남겨 놓는다.
			{
				if (pstDst != pstSrc)
				{
					HxSTD_MemCopy (pstDst, pstSrc, sizeof(_palRingBuf_VarChunkInfo_t));
				}

				ulChunkNum ++;
				ulDstIdx ++;
			}
			else
			{
				// 삭제 될 경우, 그만큼의 ring buffer에서 Writing Tail이 늘어난다.
				ulTailPtr = pstDst->ulEndPtr + 1;
				if (ulTailPtr >= pstVarChunk->ulMaxSize)
				{
					ulTailPtr -= pstVarChunk->ulMaxSize;
				}
			}
		}

		for (; ulSrcIdx < pstVarChunk->ulChunkArrayLength; ulSrcIdx++)
		{
			pstSrc = &(pstVarChunk->pstChunkInfoArray[ulSrcIdx]);

			HxSTD_MemSet (pstSrc, 0, sizeof(_palRingBuf_VarChunkInfo_t));
			pstSrc->nChunkId = CHUNK_ID_NULL;
		}

		pstVarChunk->ulChunkNum = ulChunkNum;
	}
	else
	{
		pstVarChunk->ulChunkNum = 0;
		if (NULL != pstVarChunk->pstChunkInfoArray)
		{
			HLIB_MEM_Free (pstVarChunk->pstChunkInfoArray);
			pstVarChunk->pstChunkInfoArray = NULL;
		}
	}

	if (pstVarChunk->ulChunkNum > 0)
	{
		ulTailPtr = pstVarChunk->pstChunkInfoArray[0].ulStartPtr;
	}

	hlib_ringbuf_varchunk_rearrangeChunkIdAndIndexCache (pstVarChunk);
	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR HLIB_RINGBUF_VARCHUNK_SetupInstance (_hxRingBufInst_t *pstInst, HxRingBuf_Setup_t *pstSetup)
{
	HUINT32					 ulCount;
	HUINT8					*pucBuf = NULL;
	_palRingBuf_VarChunk_t	*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);

	if (0 == pstSetup->ulMaxSize)
	{
		HxLOG_Error ("Max Size is wrong\n");
		goto FUNC_ERROR;
	}

	pucBuf = (HUINT8 *)HLIB_MEM_Malloc (pstSetup->ulMaxSize);
	if (NULL == pucBuf)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto FUNC_ERROR;
	}

	pstInst->eBufType				= eHxRINGBUF_TYPE_VARIABLE_CHUNK;
	pstInst->eRuleAfterRead			= pstSetup->eRuleAfterRead;
	pstInst->bBlockOverflow 		= pstSetup->bBlockOverflow;
	pstInst->ulMaxSize				= pstSetup->ulMaxSize;
	pstInst->cbSignal 				= pstSetup->cbSignal;

	pstInst->ullAccumulatedSize		= 0L;
	pstInst->ullAbsoluteStart		= 0L;
	pstInst->ullAbsoluteCurrent		= 0L;

	pstVarChunk->ulMaxSize			= pstSetup->ulMaxSize;
	pstVarChunk->pucBuffer			= pucBuf;
	pstVarChunk->ulChunkNum			= 0;
	pstVarChunk->ulChunkArrayLength	= 0;
//	pstVarChunk->stWritingChunkInfo.nChunkId	= -1;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		pstVarChunk->astChunkIndexCache[ulCount].nChunkId		= CHUNK_ID_NULL;
		pstVarChunk->astChunkIndexCache[ulCount].nChunkIndex	= CHUNK_ID_NULL;
	}

	return ERR_OK;
FUNC_ERROR:
	if (NULL != pucBuf)
	{
		HLIB_MEM_Free (pucBuf);
	}

	return ERR_FAIL;
}

HERROR HLIB_RINGBUF_VARCHUNK_DestroyInstance (_hxRingBufInst_t *pstInst)
{
	_palRingBuf_VarChunk_t	*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);

	if (NULL != pstVarChunk->pucBuffer)
	{
		HLIB_MEM_Free (pstVarChunk->pucBuffer);
	}

	if (NULL != pstVarChunk->pstChunkInfoArray)
	{
		HLIB_MEM_Free (pstVarChunk->pstChunkInfoArray);
	}

	HxSTD_MemSet (pstVarChunk, 0, sizeof(_palRingBuf_VarChunk_t));
	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_ReadChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HCHAR *pucData, HUINT32 *pulChunkSize)
{
	HUINT32						 ulChunkIndex, ulChunkSize, ulCopySize;
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);
	_palRingBuf_VarChunkInfo_t	*pstInfo;
	HERROR						 hErr;

	hErr = hlib_ringbuf_varchunk_getChunkIndexFromChunkId (pstVarChunk, ulChunkId, &ulChunkIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Chunk ID found failed: chunkID(%d)\n", ulChunkId);
		return ERR_FAIL;
	}

	if ((NULL == pstVarChunk->pstChunkInfoArray) || (ulChunkIndex >= pstVarChunk->ulChunkNum))
	{
		HxLOG_Error ("Wrong Value returned in chunkIndex(%d)\n", ulChunkIndex);
		return ERR_FAIL;
	}

	pstInfo = &(pstVarChunk->pstChunkInfoArray[ulChunkIndex]);

	if (pstInfo->ulStartPtr < pstInfo->ulEndPtr)
	{
		// Chunk 가 Ring Buffer에서 전체 버퍼 넘어서서 앞 쪽으로 가지 않은 경우
		ulChunkSize = pstInfo->ulEndPtr - pstInfo->ulStartPtr + 1;

		if (NULL != pucData)
		{
			HxSTD_MemCopy (pucData, pstVarChunk->pucBuffer + pstInfo->ulStartPtr, ulChunkSize);
		}
	}
	else
	{
		// Chunk 가 Ring Buffer에서 전체 버퍼 넘어서서 앞 쪽으로 넘어 간 경우
		ulChunkSize = 0;

		ulCopySize = pstVarChunk->ulMaxSize - pstInfo->ulStartPtr;
		if (NULL != pucData)
		{
			HxSTD_MemCopy (pucData, pstVarChunk->pucBuffer + pstInfo->ulStartPtr, ulCopySize);
		}
		ulChunkSize += ulCopySize;

		ulCopySize = pstInfo->ulEndPtr + 1;
		if (NULL != pucData)
		{
			HxSTD_MemCopy (pucData + ulChunkSize, pstVarChunk->pucBuffer, ulCopySize);
		}
		ulChunkSize += ulCopySize;
	}

	if (NULL != pulChunkSize)
	{
		*pulChunkSize = pstVarChunk->pstChunkInfoArray[ulChunkIndex].ulChunkSize;
	}

	// 읽은 Bluck을 지우는 eHxRINGBUF_READRULE_REMOVE 인가 ?
	if (eHxRINGBUF_READRULE_REMOVE == pstInst->eRuleAfterRead)
	{
		HUINT32				 ulCount;

		for (ulCount = 0; ulCount < ulChunkIndex; ulCount++)
		{
			pstInfo = &(pstVarChunk->pstChunkInfoArray[ulCount]);

			hlib_ringbuf_varchunk_deleteChunkIdToCache (pstVarChunk, pstInfo->nChunkId);
			pstInfo->nChunkId = CHUNK_ID_NULL;
		}

		hlib_ringbuf_varchunk_rearrangeChunk (pstVarChunk);
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_GetChunkSize (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HUINT32 *pulChunkSize)
{
	HUINT32						 ulChunkIndex;
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);
	HERROR						 hErr;

	hErr = hlib_ringbuf_varchunk_getChunkIndexFromChunkId (pstVarChunk, ulChunkId, &ulChunkIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Chunk ID found failed: chunkID(%d)\n", ulChunkId);
		return ERR_FAIL;
	}

	if ((NULL == pstVarChunk->pstChunkInfoArray) || (ulChunkIndex >= pstVarChunk->ulChunkNum))
	{
		HxLOG_Error ("Wrong Value returned in chunkIndex(%d)\n", ulChunkIndex);
		return ERR_FAIL;
	}

	if (NULL != pulChunkSize)
	{
		*pulChunkSize = pstVarChunk->pstChunkInfoArray[ulChunkIndex].ulChunkSize;
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_WriteData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	HINT32						 nLeftSize = (HINT32)ulDataSize;
	HINT32						 nCopySize, nCopySize1;
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);
	_palRingBuf_VarChunkInfo_t	*pstInfo;

	if ((0 == ulDataSize) || (NULL == pucData))
	{
		return ERR_FAIL;
	}

	*pulWrittenSize = 0;

	while (nLeftSize > 0)
	{
		if (TRUE != hlib_ringbuf_varchunk_isBufferWritable (pstVarChunk))
		{
			pstInst->bOverflowed = TRUE;
			break;
		}

		if (pstVarChunk->stWritingChunkInfo.ulHeadPtr < pstVarChunk->stWritingChunkInfo.ulTailPtr)
		{
			nCopySize1 = (pstVarChunk->stWritingChunkInfo.ulTailPtr - pstVarChunk->stWritingChunkInfo.ulHeadPtr);
		}
		else
		{
			nCopySize1 = (pstVarChunk->ulMaxSize - pstVarChunk->stWritingChunkInfo.ulHeadPtr);
		}

		nCopySize = (nCopySize1 <= nLeftSize) ? nCopySize1 : nLeftSize;
		HxSTD_MemCopy (pstVarChunk->pucBuffer + pstVarChunk->stWritingChunkInfo.ulHeadPtr, pucData, nCopySize);

		pstVarChunk->stWritingChunkInfo.ulHeadPtr += nCopySize;
		if (pstVarChunk->stWritingChunkInfo.ulHeadPtr >= pstVarChunk->ulMaxSize)
		{
			pstVarChunk->stWritingChunkInfo.ulHeadPtr -= pstVarChunk->ulMaxSize;
		}

		pucData += nCopySize;
		pstVarChunk->stWritingChunkInfo.ulChunkSize += nCopySize;
		nLeftSize -= nCopySize;
	}

	// nLeftSize > 0 이면서 Full 상태에 돌입
	if ((TRUE == pstInst->bBlockOverflow) && (nLeftSize > 0))
	{
		*pulWrittenSize = (ulDataSize - nLeftSize);
		return (*pulWrittenSize > 0) ? ERR_OK : ERR_FAIL;
	}
	else if (NULL == pstVarChunk->pstChunkInfoArray)
	{
		return ERR_FAIL;
	}

	while (nLeftSize > 0)
	{
		// 앞 줄의 Chunk를 제거해야 한다.
		if (TRUE != hlib_ringbuf_varchunk_isBufferWritable (pstVarChunk))
		{
			if (pstVarChunk->ulChunkNum == 0)				// 더 이상 지울 Chunk도 없다....
			{
				return ERR_FAIL;
			}

			pstInfo = &(pstVarChunk->pstChunkInfoArray[0]);

			hlib_ringbuf_varchunk_deleteChunkIdToCache (pstVarChunk, pstInfo->nChunkId);
			pstInfo->nChunkId = CHUNK_ID_NULL;

			hlib_ringbuf_varchunk_rearrangeChunk (pstVarChunk);
		}

		if (pstVarChunk->stWritingChunkInfo.ulHeadPtr < pstVarChunk->stWritingChunkInfo.ulTailPtr)
		{
			nCopySize1 = (pstVarChunk->stWritingChunkInfo.ulTailPtr - pstVarChunk->stWritingChunkInfo.ulHeadPtr);
		}
		else
		{
			nCopySize1 = (pstVarChunk->ulMaxSize - pstVarChunk->stWritingChunkInfo.ulHeadPtr);
		}

		nCopySize = (nCopySize1 <= nLeftSize) ? nCopySize1 : nLeftSize;
		HxSTD_MemCopy (pstVarChunk->pucBuffer + pstVarChunk->stWritingChunkInfo.ulHeadPtr, pucData, nCopySize);

		pstVarChunk->stWritingChunkInfo.ulHeadPtr += nCopySize;
		if (pstVarChunk->stWritingChunkInfo.ulHeadPtr >= pstVarChunk->ulMaxSize)
		{
			pstVarChunk->stWritingChunkInfo.ulHeadPtr -= pstVarChunk->ulMaxSize;
		}

		pucData += nCopySize;
		pstVarChunk->stWritingChunkInfo.ulChunkSize += nCopySize;
		nLeftSize -= nCopySize;
	}

	*pulWrittenSize = ulDataSize;
	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_FlushToChunk (_hxRingBufInst_t *pstInst)
{
	HUINT32						 ulCount;
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);
	_palRingBuf_VarChunkInfo_t	*pstNewInfo;
	_palRingBuf_VarChunkInfo_t	*pstWriting = &(pstVarChunk->stWritingChunkInfo);
#if defined(CONFIG_DEBUG)
	HLIB_RINGBUF_VARCHUNK_Dump(pstInst);
#endif
	if (pstWriting->ulChunkSize == 0)
	{
		HxLOG_Error ("Nothing Written\n");
		return ERR_FAIL;
	}

	if (pstVarChunk->ulChunkNum >= pstVarChunk->ulChunkArrayLength)
	{
		HUINT32						 ulNewArrayLength = pstVarChunk->ulChunkArrayLength + CHUNK_ARRAY_APPEND_NUM;
		_palRingBuf_VarChunkInfo_t	*pstChunkInfoArray;

		pstChunkInfoArray = (_palRingBuf_VarChunkInfo_t *)HLIB_STD_MemCalloc (sizeof(_palRingBuf_VarChunkInfo_t) * ulNewArrayLength);
		if (NULL == pstChunkInfoArray)
		{
			HxLOG_Error ("Memory allocation failed\n");
			return ERR_FAIL;
		}

		if ((NULL != pstVarChunk->pstChunkInfoArray) && (pstVarChunk->ulChunkNum > 0))
		{
			HxSTD_MemCopy (pstChunkInfoArray, pstVarChunk->pstChunkInfoArray, sizeof(_palRingBuf_VarChunkInfo_t) * pstVarChunk->ulChunkNum);
		}

		for (ulCount = pstVarChunk->ulChunkNum; ulCount < ulNewArrayLength; ulCount++)
		{
			pstChunkInfoArray[ulCount].nChunkId		= CHUNK_ID_NULL;
			pstChunkInfoArray[ulCount].nChunkId		= CHUNK_ID_NULL;
		}

		if (NULL != pstVarChunk->pstChunkInfoArray)
		{
			HLIB_MEM_Free (pstVarChunk->pstChunkInfoArray);
		}

		pstVarChunk->ulChunkArrayLength	= ulNewArrayLength;
		pstVarChunk->pstChunkInfoArray	= pstChunkInfoArray;
	}

	pstNewInfo = &(pstVarChunk->pstChunkInfoArray[pstVarChunk->ulChunkNum]);

	pstNewInfo->nChunkId		= pstWriting->nChunkId;
	pstNewInfo->ulChunkSize		= pstWriting->ulChunkSize;
	pstNewInfo->ulStartPtr		= pstWriting->ulTailPtr;
	if (0 == pstWriting->ulHeadPtr)
	{
		pstNewInfo->ulEndPtr	= pstVarChunk->ulMaxSize - 1;
	}
	else
	{
		pstNewInfo->ulEndPtr	= pstWriting->ulHeadPtr - 1;
	}

	pstWriting->nChunkId ++;
	pstWriting->ulChunkSize	= 0;

	pstVarChunk->ulChunkNum ++;

	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_DeleteChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId)
{
	HUINT32						 ulIndex, ulChunkIndex;
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);
	_palRingBuf_VarChunkInfo_t	*pstInfo;
	HERROR						 hErr;

	hErr = hlib_ringbuf_varchunk_getChunkIndexFromChunkId (pstVarChunk, ulChunkId, &ulChunkIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("hlib_ringbuf_varchunk_getChunkIndexFromChunkId err\n");
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex <= ulChunkIndex; ulIndex++)
	{
		pstInfo = &(pstVarChunk->pstChunkInfoArray[ulIndex]);

		hlib_ringbuf_varchunk_deleteChunkIdToCache (pstVarChunk, pstInfo->nChunkId);
		pstInfo->nChunkId = CHUNK_ID_NULL;
	}

	hlib_ringbuf_varchunk_rearrangeChunk (pstVarChunk);
	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_ClearBuffer (_hxRingBufInst_t *pstInst)
{
	HUINT32						 ulCount;
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);

	// Clear all the data
	if (NULL != pstVarChunk->pstChunkInfoArray)
	{
		HxSTD_MemSet (pstVarChunk->pstChunkInfoArray, 0, sizeof(_palRingBuf_VarChunkInfo_t) * pstVarChunk->ulChunkArrayLength);

		for (ulCount = 0; ulCount < pstVarChunk->ulChunkArrayLength; ulCount++)
		{
			pstVarChunk->pstChunkInfoArray[ulCount].nChunkId = CHUNK_ID_NULL;
		}
	}

	pstInst->ullAccumulatedSize		= 0L;
	pstInst->ullAbsoluteStart		= 0L;
	pstInst->ullAbsoluteCurrent		= 0L;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		pstVarChunk->astChunkIndexCache[ulCount].nChunkId		= CHUNK_ID_NULL;
		pstVarChunk->astChunkIndexCache[ulCount].nChunkIndex	= CHUNK_ID_NULL;
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_VARCHUNK_GetBufferingStatus (_hxRingBufInst_t *pstInst, HxRingBuf_Status_t *pstStatus)
{
	_palRingBuf_VarChunk_t		*pstVarChunk = &(pstInst->unBuffer.stVarChunkBuf);

	pstStatus->ullAccumulatedSize	= pstInst->ullAccumulatedSize;
	pstStatus->ullAbsoluteStart		= pstInst->ullAbsoluteStart;
	pstStatus->ullAbsoluteCurrent	= pstInst->ullAbsoluteCurrent;
	pstStatus->bOverflowed			= pstInst->bOverflowed;

	if ((NULL != pstVarChunk->pstChunkInfoArray) && (pstVarChunk->ulChunkNum > 0))
	{
		pstStatus->ulChunkNum			= pstVarChunk->ulChunkNum;
		pstStatus->ulStartChunkId		= pstVarChunk->pstChunkInfoArray[0].nChunkId;
		pstStatus->ulEndChunkId			= pstVarChunk->pstChunkInfoArray[pstVarChunk->ulChunkNum - 1].nChunkId;
	}
	else
	{
		pstStatus->ulChunkNum			= 0;
		pstStatus->ulStartChunkId		= 0;
		pstStatus->ulEndChunkId			= 0;
	}

	return ERR_OK;
}


void HLIB_RINGBUF_VARCHUNK_Dump(_hxRingBufInst_t *pstInst)
{
	HINT32 i = 0;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);

	HxLOG_Print("  + pstChunk(0x%x)\r\n", pstChunk);
	if(NULL == pstChunk)
	{
		return;
	}

	HxLOG_Print("	   + ulMaxSize	   : %9d(%dkb)\r\n", pstChunk->ulMaxSize, pstChunk->ulMaxSize/1024);
	HxLOG_Print("	   + pucBuffer	   : 0x%x\r\n", (HINT32)pstChunk->pucBuffer);
	HxLOG_Print("	   + ulChunkSize   : %9d(%dkb)\r\n", pstChunk->ulChunkSize, pstChunk->ulChunkSize/1024);
	HxLOG_Print("	   + ulChunkNum	   : %d\r\n", pstChunk->ulChunkNum);
	HxLOG_Print("	   + pnChunkIdArray: 0x%x\r\n", (HINT32)pstChunk->pnChunkIdArray);
	HxLOG_Print("	   + ulHeadChunk   : %d\r\n", pstChunk->ulHeadChunk);
	HxLOG_Print("	   + ulTailChunk   : %d\r\n", pstChunk->ulTailChunk);
	HxLOG_Print("	   + ulWritePtr    : %d\r\n", pstChunk->ulWritePtr);
	HxLOG_Print("	   + nNextChunkId  : %d\r\n", pstChunk->nNextChunkId);
	HxLOG_Print("	   + astChunkIndexCache(0x%x)\r\n", pstChunk->astChunkIndexCache);

	for(i=0; i < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; i++)
	{
	HxLOG_Print("	       + %d        : %d\r\n", i, pstChunk->astChunkIndexCache[i]);
	}
}

/* end of file */

