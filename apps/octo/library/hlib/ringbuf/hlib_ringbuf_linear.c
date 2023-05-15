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
STATIC INLINE HBOOL hlib_ringbuf_linear_isBufferEmpty (_palRingBuf_Linear_t *pstLinear, HBOOL bCurrPtr)
{
	HUINT32					 ulHead = pstLinear->ulHeadPtr;
	HUINT32					 ulTail = (TRUE == bCurrPtr) ? pstLinear->ulCurrPtr : pstLinear->ulTailPtr;

	return (ulHead == ulTail);
}

STATIC INLINE HBOOL hlib_ringbuf_linear_isBufferFull (_palRingBuf_Linear_t *pstLinear, HBOOL bCurrPtr)
{
	HUINT32					 ulHead = pstLinear->ulHeadPtr + 1;
	HUINT32					 ulTail = (TRUE == bCurrPtr) ? pstLinear->ulCurrPtr : pstLinear->ulTailPtr;

	if (ulHead >= pstLinear->ulMaxSize)
	{
		ulHead -= pstLinear->ulMaxSize;
	}

	return (ulHead == ulTail);
}

#define ___MEMBER_FUNCTIONS___
HERROR HLIB_RINGBUF_LINEAR_SetupInstance (_hxRingBufInst_t *pstInst, HxRingBuf_Setup_t *pstSetup)
{
	HUINT8					*pucBuf;
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	pucBuf = (HUINT8 *)HLIB_MEM_Malloc (pstSetup->ulMaxSize);
	if (NULL == pucBuf)
	{
		HxLOG_Error ("Memory allocation failed:\n");
		return ERR_FAIL;
	}

	pstInst->eBufType			= eHxRINGBUF_TYPE_LINEAR;
	pstInst->eRuleAfterRead		= pstSetup->eRuleAfterRead;
	pstInst->bBlockOverflow 	= pstSetup->bBlockOverflow;
	pstInst->ulMaxSize			= pstSetup->ulMaxSize;
	pstInst->cbSignal 			= pstSetup->cbSignal;

	pstInst->ullAccumulatedSize	= 0L;
	pstInst->ullAbsoluteStart	= 0L;
	pstInst->ullAbsoluteCurrent	= 0L;

	pstLinear->pucBuffer		= pucBuf;
	pstLinear->ulHeadPtr		= 0;
	pstLinear->ulTailPtr		= 0;
	pstLinear->ulCurrPtr		= 0;
	pstLinear->ulMaxSize		= pstSetup->ulMaxSize;

	return ERR_OK;
}

HERROR HLIB_RINGBUF_LINEAR_DestroyInstance (_hxRingBufInst_t *pstInst)
{
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	if (NULL != pstLinear->pucBuffer)
	{
		HLIB_MEM_Free (pstLinear->pucBuffer);
	}

	HxSTD_MemSet (pstLinear, 0, sizeof(_palRingBuf_Linear_t));
	return ERR_OK;
}

HERROR HLIB_RINGBUF_LINEAR_ReadData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize)
{
	HINT32					 nLeftSize = (HINT32)ulDataSize;
	HINT32					 nCopySize, nCopySize1;
	HUINT32					 ulReadSize = 0;
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	if ((0 == ulDataSize) || (NULL == pucData))
	{
		return ERR_FAIL;
	}

	while (nLeftSize > 0)
	{
		if (TRUE == hlib_ringbuf_linear_isBufferEmpty (pstLinear, TRUE))
		{
			goto END_FUNC;
		}

		if (pstLinear->ulHeadPtr < pstLinear->ulCurrPtr)
		{
			nCopySize1 = (pstLinear->ulMaxSize - pstLinear->ulCurrPtr);
		}
		else
		{
			nCopySize1 = (pstLinear->ulHeadPtr - pstLinear->ulCurrPtr);
		}

		nCopySize = (nCopySize1 <= nLeftSize) ? nCopySize1 : nLeftSize;
		HxSTD_MemCopy (pucData, pstLinear->pucBuffer + pstLinear->ulCurrPtr, nCopySize);

		pstLinear->ulCurrPtr += nCopySize;
		if (pstLinear->ulCurrPtr >= pstLinear->ulMaxSize)
		{
			pstLinear->ulCurrPtr -= pstLinear->ulMaxSize;
		}

		pucData += nCopySize;
		ulReadSize += nCopySize;
		nLeftSize -= nCopySize;
	}

END_FUNC:
	switch (pstInst->eRuleAfterRead)
	{
	case eHxRINGBUF_READRULE_REMOVE:
		pstLinear->ulTailPtr = pstLinear->ulCurrPtr;
		break;

	case eHxRINGBUF_READRULE_REMAIN:
	default:
		break;
	}

	if (NULL != pulReadSize)					{ *pulReadSize = ulReadSize; }
	return (ulReadSize > 0) ? ERR_OK : ERR_FAIL;
}

HERROR HLIB_RINGBUF_LINEAR_WriteData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	HINT32					 nLeftSize = (HINT32)ulDataSize;
	HINT32					 nCopySize, nCopySize1;
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	if ((0 == ulDataSize) || (NULL == pucData))
	{
		return ERR_FAIL;
	}

	*pulWrittenSize = 0;

	while (nLeftSize > 0)
	{
		if (TRUE == hlib_ringbuf_linear_isBufferFull (pstLinear, FALSE))
		{
			pstInst->bOverflowed = TRUE;
			break;
		}

		if (pstLinear->ulHeadPtr < pstLinear->ulTailPtr)
		{
			nCopySize1 = (pstLinear->ulTailPtr - pstLinear->ulHeadPtr);
		}
		else
		{
			nCopySize1 = (pstLinear->ulMaxSize - pstLinear->ulHeadPtr);
		}

		nCopySize = (nCopySize1 <= nLeftSize) ? nCopySize1 : nLeftSize;
		HxSTD_MemCopy (pstLinear->pucBuffer + pstLinear->ulHeadPtr, pucData, nCopySize);

		pstLinear->ulHeadPtr += nCopySize;
		if (pstLinear->ulHeadPtr >= pstLinear->ulMaxSize)
		{
			pstLinear->ulHeadPtr -= pstLinear->ulMaxSize;
		}

		pucData += nCopySize;
		nLeftSize -= nCopySize;
	}

#if 0
	if (nLeftSize > 0)				// Buffr Full 상태
	{
		HxLOG_Error ("Buffer Full! remained Size = %d\n", nLeftSize);
	}
#endif

	// nLeftSize > 0 이면서 Full 상태에 돌입
	if ((TRUE == pstInst->bBlockOverflow) && (nLeftSize > 0))
	{
		*pulWrittenSize = (ulDataSize - nLeftSize);
		return (*pulWrittenSize > 0) ? ERR_OK : ERR_FAIL;
	}

	while (nLeftSize > 0)
	{
		nCopySize1 = (pstLinear->ulMaxSize - pstLinear->ulHeadPtr);
		nCopySize = (nCopySize1 <= nLeftSize) ? nCopySize1 : nLeftSize;

		HxSTD_MemCopy (pstLinear->pucBuffer + pstLinear->ulHeadPtr, pucData, nCopySize);

		pstLinear->ulHeadPtr += nCopySize;
		if (pstLinear->ulHeadPtr >= pstLinear->ulMaxSize)
		{
			pstLinear->ulHeadPtr -= pstLinear->ulMaxSize;
		}

		pstLinear->ulTailPtr = pstLinear->ulHeadPtr;
		pucData += nCopySize;
		nLeftSize -= nCopySize;
	}

	*pulWrittenSize = ulDataSize;
	return ERR_OK;
}

HERROR HLIB_RINGBUF_LINEAR_EraseData (_hxRingBufInst_t *pstInst, HUINT32 ulEraseSize)
{
	HBOOL					 bMoveCurrPtr;
	HINT32					 nMaxTailMove, nTailMoveSize;
	HINT32					 nLeftSize = (HINT32)ulEraseSize;
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	while (nLeftSize > 0)
	{
		bMoveCurrPtr = FALSE;

		if (TRUE == hlib_ringbuf_linear_isBufferEmpty (pstLinear, FALSE))
		{
			break;
		}

		if (pstLinear->ulHeadPtr < pstLinear->ulTailPtr)
		{
			nMaxTailMove = (pstLinear->ulMaxSize - pstLinear->ulTailPtr);
		}
		else
		{
			nMaxTailMove = (pstLinear->ulHeadPtr - pstLinear->ulTailPtr);
		}

		nTailMoveSize = (nMaxTailMove <= nLeftSize) ? nMaxTailMove : nLeftSize;

		if ((pstLinear->ulCurrPtr >= pstLinear->ulTailPtr) && (pstLinear->ulCurrPtr <= (pstLinear->ulTailPtr + nTailMoveSize)))
		{
			bMoveCurrPtr = TRUE;
		}

		pstLinear->ulTailPtr += nTailMoveSize;
		if (pstLinear->ulTailPtr >= pstLinear->ulMaxSize)
		{
			pstLinear->ulTailPtr -= pstLinear->ulMaxSize;
		}

		nLeftSize -= nTailMoveSize;
	}

	return ERR_OK;
}

HERROR HLIB_RINGBUF_LINEAR_ClearBuffer (_hxRingBufInst_t *pstInst)
{
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	pstInst->ullAccumulatedSize		= 0L;
	pstInst->ullAbsoluteStart		= 0L;
	pstInst->ullAbsoluteCurrent		= 0L;

	pstLinear->ulHeadPtr			= 0;
	pstLinear->ulTailPtr			= 0;
	pstLinear->ulCurrPtr			= 0;

	return ERR_OK;
}

HERROR HLIB_RINGBUF_LINEAR_GetBufferingStatus (_hxRingBufInst_t *pstInst, HxRingBuf_Status_t *pstStatus)
{
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	pstStatus->ullAccumulatedSize	= pstInst->ullAccumulatedSize;
	pstStatus->ullAbsoluteStart		= pstInst->ullAbsoluteStart;
	pstStatus->ullAbsoluteCurrent	= pstInst->ullAbsoluteCurrent;
	pstStatus->bOverflowed			= pstInst->bOverflowed;

	if (pstLinear->ulHeadPtr >= pstLinear->ulTailPtr)
	{
		pstStatus->ulBufferSize = pstLinear->ulHeadPtr - pstLinear->ulTailPtr;
	}
	else
	{
		pstStatus->ulBufferSize = (pstLinear->ulMaxSize - pstLinear->ulTailPtr) + pstLinear->ulHeadPtr;
	}

	if (pstLinear->ulCurrPtr >= pstLinear->ulTailPtr)
	{
		pstStatus->ulCurrentPnt = pstLinear->ulCurrPtr - pstLinear->ulTailPtr;
	}
	else
	{
		pstStatus->ulCurrentPnt = (pstLinear->ulMaxSize - pstLinear->ulTailPtr) + pstLinear->ulCurrPtr;
	}

	pstStatus->ulUnreadSize = (pstStatus->ulBufferSize > pstStatus->ulCurrentPnt) ? (pstStatus->ulBufferSize - pstStatus->ulCurrentPnt) : 0;
	return ERR_OK;
}

void HLIB_RINGBUF_LINEAR_Dump(_hxRingBufInst_t *pstInst)
{
	_palRingBuf_Linear_t	*pstLinear = &(pstInst->unBuffer.stLinearBuf);

	HxLOG_Print("  + pstLinear(0x%x)\r\n", pstLinear);
	if(NULL == pstLinear)
	{
		return;
	}

	HxLOG_Print("      + ulMaxSize     : %9d(%dkb)\r\n", pstLinear->ulMaxSize, pstLinear->ulMaxSize/1024);
	HxLOG_Print("      + pucBuffer     : 0x%x\r\n", pstLinear->pucBuffer);
	HxLOG_Print("      + ulHeadPtr     : 0x%x\r\n", pstLinear->ulHeadPtr);
	HxLOG_Print("      + ulTailPtr     : 0x%x\r\n", pstLinear->ulTailPtr);
	HxLOG_Print("      + ulCurrPtr     : 0x%x\r\n", pstLinear->ulCurrPtr);
}

/* end of file */

