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
#include <octo_common.h>
#include <isosvc.h>

#include <pal_ringbuf.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_RINGBUF_INST_NUM					16

#define ENTER_FUNC							VK_SEM_Get (s_ulPalRingBuf_SemId)
#define LEAVE_FUNC							VK_SEM_Release (s_ulPalRingBuf_SemId)
#define ENTER_INST(pstInst)					VK_SEM_Get (pstInst->ulSemId)
#define LEAVE_INST(pstInst)					VK_SEM_Release (pstInst->ulSemId)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	unsigned long				 ulSemId;
	HBOOL						 bExecuting;

	Handle_t					 hInstHandle;
	Handle_t					 hHxRingbufHandle;
} _palRingBufInst_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC unsigned long		 s_ulPalRingBuf_SemId;
STATIC _palRingBufInst_t	 s_astRingBuf_Instance[MAX_RINGBUF_INST_NUM];

#define ___INTERNAL_FUNCTIONS___
STATIC _palRingBufInst_t *pal_ringbuf_getEmptyInstance (void)
{
	HUINT32				 ulIndex;
	HERROR				 hErr;

	for (ulIndex = 0; ulIndex < MAX_RINGBUF_INST_NUM; ulIndex++)
	{
		if (TRUE != s_astRingBuf_Instance[ulIndex].bExecuting)
		{
			if (0 == s_astRingBuf_Instance[ulIndex].ulSemId)
			{
				unsigned long			 ulSemId;
				HCHAR					 szSemName[16];

				HxSTD_PrintToStrN (szSemName, 16, "RingBuf%02dSem", ulIndex);
				hErr = VK_SEM_Create (&ulSemId, szSemName, VK_SUSPENDTYPE_PRIORITY);
				if (VK_OK != hErr)
				{
					HxLOG_Error ("Semaphore failed!!!\n");
					return NULL;
				}

				s_astRingBuf_Instance[ulIndex].ulSemId = ulSemId;
			}

			return &(s_astRingBuf_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC _palRingBufInst_t *pal_ringbuf_getInstance (Handle_t hRingBuf)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < MAX_RINGBUF_INST_NUM; ulIndex++)
	{
		if ((TRUE == s_astRingBuf_Instance[ulIndex].bExecuting) && (hRingBuf == s_astRingBuf_Instance[ulIndex].hInstHandle))
		{
			return &(s_astRingBuf_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC HERROR pal_ringbuf_clearInstance (_palRingBufInst_t *pstInst)
{
	unsigned long			 ulSemId = pstInst->ulSemId;

	// 반드시 지우면 안되는 Semaphore ID 외에는 다 지운다.

	HxSTD_memset (pstInst, 0, sizeof(_palRingBufInst_t));
	pstInst->ulSemId = ulSemId;
	return ERR_OK;
}


#define _____FOR_API_FUNCTIONS_____
STATIC Handle_t pal_ringbuf_createRingBuffer (HxRingBuf_Setup_t *pstSetup)
{
	STATIC Handle_t			 s_hLastRingBuf = 0x0001;
	Handle_t				 hRingBuf = (s_hLastRingBuf < 0xFFFFFFF0) ? s_hLastRingBuf + 1 : 0x00000001;
	_palRingBufInst_t		*pstInst;

	if ((NULL == pstSetup) || (0 == pstSetup->ulMaxSize))
	{
		HxLOG_Error ("Setup Value wrong\n");
		return HANDLE_NULL;
	}

	while (1)
	{
		if (NULL == pal_ringbuf_getInstance (hRingBuf))
		{
			break;
		}

		hRingBuf = (hRingBuf < 0xFFFFFFF0) ? hRingBuf + 1 : 0x00000001;
		if (s_hLastRingBuf == hRingBuf)
		{
			HxLOG_Error ("Something Wrong !!!!!!\n");
			return HANDLE_NULL;
		}
	}

	pstInst = pal_ringbuf_getEmptyInstance();
	if (NULL == pstInst)
	{
		HxLOG_Error ("No empty instance...\n");
		return HANDLE_NULL;
	}

	pstInst->hHxRingbufHandle = HLIB_RINGBUF_Create(pstSetup);
	if (HANDLE_NULL == pstInst->hHxRingbufHandle || 0 == pstInst->hHxRingbufHandle)
	{
		HxLOG_Error ("Setup failed, BufferType(%d)\n", pstSetup->eBufType);
		return HANDLE_NULL;
	}

	pstInst->bExecuting		= TRUE;
	pstInst->hInstHandle	= hRingBuf;

	s_hLastRingBuf = hRingBuf;
	return hRingBuf;

}

STATIC void pal_ringbuf_destroyRingBuffer (Handle_t hRingBuf)
{
	_palRingBufInst_t			*pstInst = pal_ringbuf_getInstance (hRingBuf);

	if (NULL != pstInst)
	{
		HLIB_RINGBUF_Destroy(pstInst->hHxRingbufHandle);
		pal_ringbuf_clearInstance (pstInst);
	}
}

#define ___EXTERNAL_FUNCTIONS___
HERROR PAL_RINGBUF_Init (void)
{
	HERROR		 hErr;

	HxLOG_Trace();

	hErr = VK_SEM_Create (&s_ulPalRingBuf_SemId, "MwcRingBufSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("Semaphore failed!!!\n");
		return ERR_FAIL;
	}

	HxSTD_memset (s_astRingBuf_Instance, 0, sizeof(_palRingBufInst_t) * MAX_RINGBUF_INST_NUM);
	return ERR_OK;
}

Handle_t PAL_RINGBUF_Create (HxRingBuf_Setup_t *pstSetup)
{
	Handle_t			 hResult;

	HxLOG_Trace();

	ENTER_FUNC;
	hResult = pal_ringbuf_createRingBuffer (pstSetup);
	LEAVE_FUNC;

	return hResult;
}

void PAL_RINGBUF_Destroy (Handle_t hRingBuf)
{
	_palRingBufInst_t			*pstInst;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		pal_ringbuf_destroyRingBuffer (hRingBuf);
		LEAVE_INST (pstInst);
	}
	LEAVE_FUNC;
}

HERROR PAL_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize)
{
	_palRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_ReadData(pstInst->hHxRingbufHandle, ulDataSize, pucData, pulReadSize);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize)
{
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_ReadChunk (pstInst->hHxRingbufHandle, ulChunkId, pucData, pulChunkSize);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize)
{
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_GetChunkSize (pstInst->hHxRingbufHandle, ulChunkId, pulChunkSize);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	HUINT32					 ulWrittenSize = 0;
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_WriteData (pstInst->hHxRingbufHandle, ulDataSize, pucData, &ulWrittenSize);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr || NULL == pstInst)
	{
		HxLOG_Error("hRingBuf(%d) hErr(%d) or pstInst(%d) error\r\n", hRingBuf, hErr, pstInst);
	}

	if (NULL != pulWrittenSize)					{ *pulWrittenSize = ulWrittenSize; }
	return hErr;
}

HERROR PAL_RINGBUF_FlushToChunk (Handle_t hRingBuf)
{
	_palRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_FlushToChunk (pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}


HERROR PAL_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData)
{
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_EraseData (pstInst->hHxRingbufHandle, ulEraseData);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId)
{
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_EraseChunk (pstInst->hHxRingbufHandle, ulChunkId);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_ClearBuffer (Handle_t hRingBuf)
{
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_ClearBuffer (pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus)
{
	_palRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstStatus)					{ return ERR_FAIL; }

	HxSTD_memset (pstStatus, 0, sizeof(HxRingBuf_Status_t));

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_GetBufferingStatus (pstInst->hHxRingbufHandle, pstStatus);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf)
{
	_palRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_ResetOverflowFlag (pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf)
{
	_palRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_SetWriteBlockingLock (pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR PAL_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf)
{
	_palRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		hErr = HLIB_RINGBUF_ResetWriteBlockingLock (pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}


#if defined(CONFIG_DEBUG)
void PAL_RINGBUF_Dump (Handle_t hRingBuf)
{
	_palRingBufInst_t		*pstInst = NULL;

	ENTER_FUNC;
	pstInst = pal_ringbuf_getInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
 		HxLOG_Print("============================================================\r\n");
		HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
		HxLOG_Print("hRingBuf(0x%x)\n", (HUINT32)hRingBuf);
		HxLOG_Print("pstInst(0x%x)\n", 	(HUINT32)pstInst);
		HxLOG_Print("  + ulSemId           : 0x%x\r\n", pstInst->ulSemId);
		HxLOG_Print("  + bExecuting        : %s\r\n", ((pstInst->bExecuting == TRUE) ? "TRUE" : "FALSE"));

		HLIB_RINGBUF_DEBUG_Dump(pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}
}

#endif

/* end of file */

