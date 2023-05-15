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
STATIC HBOOL hlib_ringbuf_chunk_isBufferWritable (_palRingBuf_Chunk_t *pstChunk)
{
	HUINT32				 ulNextHead;

	if (CHUNK_ID_CURRENT == pstChunk->pnChunkIdArray[pstChunk->ulHeadChunk])
	{
		return TRUE;
	}

	ulNextHead =  pstChunk->ulHeadChunk + 1;
	if (ulNextHead >= pstChunk->ulChunkNum)
	{
		ulNextHead = 0;
	}

	if (pstChunk->ulTailChunk == ulNextHead)
	{
		return FALSE;
	}

	pstChunk->pnChunkIdArray[ulNextHead] = CHUNK_ID_CURRENT;
	pstChunk->ulHeadChunk = ulNextHead;

	return TRUE;
}

#define _____CHUNK_INDEX_CACHE_____
STATIC HERROR hlib_ringbuf_chunk_rearrangeChunkIdAndIndexCache (_palRingBuf_Chunk_t *pstChunk)
{
	HUINT32				 ulSrcCnt, ulDstCnt;

	for (ulSrcCnt = 0, ulDstCnt = 0; ulSrcCnt < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulSrcCnt++)
	{
		if (pstChunk->astChunkIndexCache[ulSrcCnt].nChunkId < 0)					{ continue; }

		pstChunk->astChunkIndexCache[ulDstCnt].nChunkId		= pstChunk->astChunkIndexCache[ulSrcCnt].nChunkId;
		pstChunk->astChunkIndexCache[ulDstCnt].nChunkIndex	= pstChunk->astChunkIndexCache[ulSrcCnt].nChunkIndex;

		ulDstCnt ++;
	}

	for (; ulDstCnt < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulDstCnt++)
	{
		pstChunk->astChunkIndexCache[ulDstCnt].nChunkId		= -1;
		pstChunk->astChunkIndexCache[ulDstCnt].nChunkIndex	= -1;
	}

	return ERR_OK;
}

STATIC HERROR hlib_ringbuf_chunk_addChunkIdAndIndexToCache (_palRingBuf_Chunk_t *pstChunk, HUINT32 ulChunkId, HUINT32 ulChunkIndex)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		if (pstChunk->astChunkIndexCache[ulCount].nChunkId < 0)
		{
			pstChunk->astChunkIndexCache[ulCount].nChunkId		= (HINT32)ulChunkId;
			pstChunk->astChunkIndexCache[ulCount].nChunkIndex	= (HINT32)ulChunkIndex;

			return ERR_OK;
		}
	}

	// Cache가 가득찼을 경우:
	pstChunk->astChunkIndexCache[0].nChunkId		= -1;
	pstChunk->astChunkIndexCache[0].nChunkIndex		= -1;

	hlib_ringbuf_chunk_rearrangeChunkIdAndIndexCache (pstChunk);

	pstChunk->astChunkIndexCache[HLIB_RINGBUF_CHUNKID_CACHE_SIZE - 1].nChunkId		= (HINT32)ulChunkId;
	pstChunk->astChunkIndexCache[HLIB_RINGBUF_CHUNKID_CACHE_SIZE - 1].nChunkIndex	= (HINT32)ulChunkIndex;

	return ERR_OK;

}

STATIC HERROR hlib_ringbuf_chunk_deleteChunkIdToCache (_palRingBuf_Chunk_t *pstChunk, HUINT32 ulChunkId)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		if ((pstChunk->astChunkIndexCache[ulCount].nChunkId > 0) &&
			(pstChunk->astChunkIndexCache[ulCount].nChunkId == (HINT32)ulChunkId))
		{
			pstChunk->astChunkIndexCache[ulCount].nChunkId		= -1;
			pstChunk->astChunkIndexCache[ulCount].nChunkIndex	= -1;

			hlib_ringbuf_chunk_rearrangeChunkIdAndIndexCache (pstChunk);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR hlib_ringbuf_chunk_findChunkIndexFromCache (_palRingBuf_Chunk_t *pstChunk, HUINT32 ulChunkId, HUINT32 *pulChunkIndex)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		if (pstChunk->astChunkIndexCache[ulCount].nChunkId == (HINT32)ulChunkId)
		{
			*pulChunkIndex = (HUINT32)pstChunk->astChunkIndexCache[ulCount].nChunkIndex;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#define _____CHUNK_INDEX_____
STATIC HERROR hlib_ringbuf_chunk_getChunkIndexFromChunkId (_palRingBuf_Chunk_t *pstChunk, HUINT32 ulChunkId, HUINT32 *pulChunkIndex)
{
	HUINT32				 ulChunkIndex;
	HERROR				 hErr;

	hErr = hlib_ringbuf_chunk_findChunkIndexFromCache (pstChunk, ulChunkId, pulChunkIndex);
	if (ERR_OK == hErr)
	{
		return ERR_OK;
	}

	ulChunkIndex = pstChunk->ulTailChunk;
	while (1)
	{
		if ((pstChunk->pnChunkIdArray[ulChunkIndex] >= 0) && (pstChunk->pnChunkIdArray[ulChunkIndex] == (HINT32)ulChunkId))
		{
			*pulChunkIndex = ulChunkIndex;
			hlib_ringbuf_chunk_addChunkIdAndIndexToCache (pstChunk, ulChunkId, ulChunkIndex);
			return ERR_OK;
		}

		ulChunkIndex ++;
		if (ulChunkIndex >= pstChunk->ulChunkNum)
		{
			ulChunkIndex = 0;
		}

		if (ulChunkIndex ==pstChunk->ulTailChunk)
		{
			break;
		}
	}

	return ERR_FAIL;
}

#define _____CHUNK_ACTION_____

STATIC HERROR hlib_ringbuf_chunk_removeOldestChunk (_palRingBuf_Chunk_t *pstChunk)
{
	// Remove the Oldest Chunk: Remove the cache first
	hlib_ringbuf_chunk_deleteChunkIdToCache (pstChunk, pstChunk->pnChunkIdArray[pstChunk->ulTailChunk]);

	pstChunk->pnChunkIdArray[pstChunk->ulTailChunk] = CHUNK_ID_CURRENT;

	pstChunk->ulTailChunk ++;
	if (pstChunk->ulTailChunk >= pstChunk->ulChunkNum)
	{
		pstChunk->ulTailChunk = 0;
	}

	return ERR_OK;
}





#define ___MEMBER_FUNCTIONS___
HERROR HLIB_RINGBUF_CHUNK_SetupInstance (_hxRingBufInst_t *pstInst, HxRingBuf_Setup_t *pstSetup)
{
	HUINT32					 ulCount, ulMaxSize, ulChunkNum = 0;
	HUINT8					*pucBuf = NULL;
	HINT32					*pnChunkIdArray = NULL;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);

	if ((0 == pstSetup->ulMaxSize) || (2 > pstSetup->ulChunkSize) || (pstSetup->ulChunkSize >= pstSetup->ulMaxSize))
	{
		HxLOG_Error ("Max Size or Chunk Size is wrong: Max(%d), Chunk(%d)\n", pstSetup->ulMaxSize, pstSetup->ulChunkSize);
		goto FUNC_ERROR;
	}

	if(pstSetup->ulChunkSize != 0)
		ulChunkNum = ((pstSetup->ulMaxSize - 1) / pstSetup->ulChunkSize) + 1;
	if (ulChunkNum < 2)
	{
		HxLOG_Error ("Max Size or Chunk Size is wrong: Max(%d), Chunk(%d)\n", pstSetup->ulMaxSize, pstSetup->ulChunkSize);
		goto FUNC_ERROR;
	}

	ulMaxSize = ulChunkNum * pstSetup->ulChunkSize;

	pucBuf = (HUINT8 *)HLIB_MEM_Malloc (pstSetup->ulMaxSize);
	if (NULL == pucBuf)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto FUNC_ERROR;
	}

	pnChunkIdArray = (HINT32 *)HLIB_MEM_Malloc (sizeof(HINT32) * ulChunkNum);
	if (NULL == pnChunkIdArray)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		goto FUNC_ERROR;
	}

	for (ulCount = 0; ulCount < ulChunkNum; ulCount++)
	{
		pnChunkIdArray[ulCount] = CHUNK_ID_NULL;
	}

	pstInst->eBufType				= eHxRINGBUF_TYPE_CHUNK;
	pstInst->eRuleAfterRead			= pstSetup->eRuleAfterRead;
	pstInst->bBlockOverflow 		= pstSetup->bBlockOverflow;
	pstInst->ulMaxSize				= pstSetup->ulMaxSize;

	pstInst->ullAccumulatedSize		= 0L;
	pstInst->ullAbsoluteStart		= 0L;
	pstInst->ullAbsoluteCurrent		= 0L;
	pstInst->cbSignal 				= pstSetup->cbSignal;

	pstChunk->ulMaxSize				= ulMaxSize;
	pstChunk->pucBuffer				= pucBuf;
	pstChunk->ulChunkNum			= ulChunkNum;
	pstChunk->ulChunkSize			= pstSetup->ulChunkSize;
	pstChunk->pnChunkIdArray		= pnChunkIdArray;
	pstChunk->ulHeadChunk			= 0;
	pstChunk->ulTailChunk			= 0;
	pstChunk->ulWritePtr			= 0;
	pstChunk->nNextChunkId			= 0;
	pstChunk->pnChunkIdArray[0]		= CHUNK_ID_CURRENT;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		pstChunk->astChunkIndexCache[ulCount].nChunkId		= -1;
		pstChunk->astChunkIndexCache[ulCount].nChunkIndex	= -1;
	}

	return ERR_OK;
FUNC_ERROR:
	if (NULL != pucBuf)
	{
		HLIB_MEM_Free (pucBuf);
	}

	if (NULL != pnChunkIdArray)
	{
		HLIB_MEM_Free (pnChunkIdArray);
	}

	return ERR_FAIL;
}

HERROR HLIB_RINGBUF_CHUNK_DestroyInstance (_hxRingBufInst_t *pstInst)
{
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);

	HxLOG_Trace();

	if (NULL != pstChunk->pucBuffer)
	{
		HLIB_MEM_Free (pstChunk->pucBuffer);
	}

	if (NULL != pstChunk->pnChunkIdArray)
	{
		HLIB_MEM_Free (pstChunk->pnChunkIdArray);
	}

	HxSTD_MemSet (pstChunk, 0, sizeof(_palRingBuf_Chunk_t));
	return ERR_OK;
}

HERROR HLIB_RINGBUF_CHUNK_ReadChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HCHAR *pucData, HUINT32 *pulChunkSize)
{
	HUINT32					 ulChunkIndex;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);
	HERROR					 hErr;

	hErr = hlib_ringbuf_chunk_getChunkIndexFromChunkId (pstChunk, ulChunkId, &ulChunkIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("hlib_ringbuf_chunk_getChunkIndexFromChunkId err\n");
		return ERR_FAIL;
	}

	if (NULL != pucData)
	{
		HxSTD_MemCopy (pucData, pstChunk->pucBuffer + (pstChunk->ulChunkSize * ulChunkIndex), pstChunk->ulChunkSize);
	}

	if (NULL != pulChunkSize)
	{
		*pulChunkSize = pstChunk->ulChunkSize;
	}

	if (eHxRINGBUF_READRULE_REMOVE == pstInst->eRuleAfterRead)
	{
		HUINT32				 ulIndex;

		ulIndex = pstChunk->ulTailChunk;

		while (1)
		{
			hlib_ringbuf_chunk_deleteChunkIdToCache (pstChunk, pstChunk->pnChunkIdArray[ulIndex]);
			pstChunk->pnChunkIdArray[ulIndex] = CHUNK_ID_NULL;

			if ((ulIndex == ulChunkIndex) || (ulIndex == pstChunk->ulHeadChunk))
			{
				break;
			}

			ulIndex ++;
			if (ulIndex >= pstChunk->ulChunkNum)
			{
				ulIndex = 0;
			}

			if (ulIndex == pstChunk->ulTailChunk)
			{
				break;
			}
		}

		pstChunk->ulTailChunk = ulIndex;
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_CHUNK_GetChunkSize (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HUINT32 *pulChunkSize)
{
	HUINT32					 ulChunkIndex;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);
	HERROR					 hErr;

	hErr = hlib_ringbuf_chunk_getChunkIndexFromChunkId (pstChunk, ulChunkId, &ulChunkIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("hlib_ringbuf_chunk_getChunkIndexFromChunkId err\n");
		return ERR_FAIL;
	}

	if (NULL != pulChunkSize)
	{
		*pulChunkSize = pstChunk->ulChunkSize;
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_CHUNK_WriteData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	HBOOL					 bChunkComplete;
	HINT32					 nCopySize, nLeftSize = (HINT32)ulDataSize;
	HUINT8					*pucChunkBuf;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);

	if ((0 == ulDataSize) || (NULL == pucData))
	{
		return ERR_FAIL;
	}

	*pulWrittenSize = 0;

	while (nLeftSize > 0)
	{
		if (TRUE != hlib_ringbuf_chunk_isBufferWritable (pstChunk))
		{
			pstInst->bOverflowed = TRUE;
			break;
		}

		pucChunkBuf = pstChunk->pucBuffer + (pstChunk->ulHeadChunk * pstChunk->ulChunkSize);

		if (nLeftSize > (pstChunk->ulChunkSize - pstChunk->ulWritePtr))
		{
			bChunkComplete = TRUE;
			nCopySize = (pstChunk->ulChunkSize - pstChunk->ulWritePtr);
		}
		else
		{
			bChunkComplete = FALSE;
			nCopySize = nLeftSize;
		}

		HxSTD_MemCopy (pucChunkBuf + pstChunk->ulWritePtr, pucData, nCopySize);
		pstChunk->ulWritePtr += nCopySize;
		pucData += nCopySize;
		nLeftSize -= nCopySize;

		if (TRUE == bChunkComplete)
		{
			pstChunk->pnChunkIdArray[pstChunk->ulHeadChunk] = pstChunk->nNextChunkId;
			pstChunk->nNextChunkId ++;
			pstChunk->ulWritePtr = 0;
		}
	}

	// nLeftSize > 0 이면서 Full 상태에 돌입
	if ((TRUE == pstInst->bBlockOverflow) && (nLeftSize > 0))
	{
		*pulWrittenSize = (ulDataSize - nLeftSize);
		return (*pulWrittenSize > 0) ? ERR_OK : ERR_FAIL;
	}

	while (nLeftSize > 0)
	{
		hlib_ringbuf_chunk_removeOldestChunk (pstChunk);
		if (TRUE != hlib_ringbuf_chunk_isBufferWritable (pstChunk))
		{
			return ERR_FAIL;
		}

		pucChunkBuf = pstChunk->pucBuffer + (pstChunk->ulHeadChunk * pstChunk->ulChunkSize);

		if (nLeftSize > (pstChunk->ulChunkSize - pstChunk->ulWritePtr))
		{
			bChunkComplete = TRUE;
			nCopySize = (pstChunk->ulChunkSize - pstChunk->ulWritePtr);
		}
		else
		{
			bChunkComplete = FALSE;
			nCopySize = nLeftSize;
		}

		HxSTD_MemCopy (pucChunkBuf + pstChunk->ulWritePtr, pucData, nCopySize);
		pstChunk->ulWritePtr += nCopySize;
		pucData += nCopySize;
		nLeftSize -= nCopySize;

		if (TRUE == bChunkComplete)
		{
			pstChunk->pnChunkIdArray[pstChunk->ulHeadChunk] = pstChunk->nNextChunkId;
			pstChunk->nNextChunkId ++;
			pstChunk->ulWritePtr = 0;
		}
	}

	*pulWrittenSize = ulDataSize;
	return ERR_OK;
}

HERROR HLIB_RINGBUF_CHUNK_DeleteChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId)
{
	HUINT32					 ulIndex, ulChunkIndex;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);
	HERROR					 hErr;

	hErr = hlib_ringbuf_chunk_getChunkIndexFromChunkId (pstChunk, ulChunkId, &ulChunkIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("hlib_ringbuf_chunk_getChunkIndexFromChunkId err\n");
		return ERR_FAIL;
	}

	ulIndex = pstChunk->ulTailChunk;

	while (1)
	{
		hlib_ringbuf_chunk_deleteChunkIdToCache (pstChunk, pstChunk->pnChunkIdArray[ulIndex]);
		pstChunk->pnChunkIdArray[ulIndex] = CHUNK_ID_NULL;

		if ((ulIndex == ulChunkIndex) || (ulIndex == pstChunk->ulHeadChunk))
		{
			break;
		}

		ulIndex ++;
		if (ulIndex >= pstChunk->ulChunkNum)
		{
			ulIndex = 0;
		}

		if (ulIndex == pstChunk->ulTailChunk)
		{
			break;
		}
	}

	pstChunk->ulTailChunk = ulIndex;

	return ERR_OK;
}

HERROR HLIB_RINGBUF_CHUNK_ClearBuffer (_hxRingBufInst_t *pstInst)
{
	HUINT32					 ulCount;
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);

	// Clear all the data
	for (ulCount = 0; ulCount < pstChunk->ulChunkNum; ulCount++)
	{
		pstChunk->pnChunkIdArray[ulCount] = CHUNK_ID_NULL;
	}

	pstInst->ullAccumulatedSize		= 0L;
	pstInst->ullAbsoluteStart		= 0L;
	pstInst->ullAbsoluteCurrent		= 0L;

	pstChunk->ulHeadChunk			= 0;
	pstChunk->ulTailChunk			= 0;
	pstChunk->ulWritePtr			= 0;
	//	pstChunk->nNextChunkId			= 0;				// 여기를 0으로 하면 Clear 후 ChunkID를 reset 하게 됨
	pstChunk->pnChunkIdArray[0]		= CHUNK_ID_CURRENT;

	for (ulCount = 0; ulCount < HLIB_RINGBUF_CHUNKID_CACHE_SIZE; ulCount++)
	{
		pstChunk->astChunkIndexCache[ulCount].nChunkId		= -1;
		pstChunk->astChunkIndexCache[ulCount].nChunkIndex	= -1;
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_CHUNK_GetBufferingStatus (_hxRingBufInst_t *pstInst, HxRingBuf_Status_t *pstStatus)
{
	_palRingBuf_Chunk_t		*pstChunk = &(pstInst->unBuffer.stChunkBuf);

	pstStatus->ullAccumulatedSize	= pstInst->ullAccumulatedSize;
	pstStatus->ullAbsoluteStart		= pstInst->ullAbsoluteStart;
	pstStatus->ullAbsoluteCurrent	= pstInst->ullAbsoluteCurrent;
	pstStatus->bOverflowed			= pstInst->bOverflowed;

	if (pstChunk->ulHeadChunk == pstChunk->ulTailChunk)
	{
		pstStatus->ulChunkNum		= 0;
		pstStatus->ulStartChunkId	= (HUINT32)pstChunk->nNextChunkId;
		pstStatus->ulEndChunkId		= (HUINT32)pstChunk->nNextChunkId;
	}
	else
	{
		if (pstChunk->pnChunkIdArray[pstChunk->ulHeadChunk] < 0)		// Head가 다 차지 않은 Chunk : Count에 넣으면 안된다
		{
			HUINT32					 ulHeadChunk = (0 == pstChunk->ulHeadChunk) ? pstChunk->ulChunkNum - 1 : pstChunk->ulHeadChunk - 1;

			pstStatus->ulStartChunkId		= pstChunk->pnChunkIdArray[pstChunk->ulTailChunk];
			pstStatus->ulEndChunkId			= pstChunk->pnChunkIdArray[ulHeadChunk];
		}
		else
		{
			pstStatus->ulStartChunkId		= pstChunk->pnChunkIdArray[pstChunk->ulTailChunk];
			pstStatus->ulEndChunkId			= pstChunk->pnChunkIdArray[pstChunk->ulHeadChunk];
		}

		pstStatus->ulChunkNum		= pstStatus->ulEndChunkId - pstStatus->ulStartChunkId + 1;
	}

	return ERR_OK;
}


void HLIB_RINGBUF_CHUNK_Dump(_hxRingBufInst_t *pstInst)
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

