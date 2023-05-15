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
#include "../int_streaming.h"




/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_RINGBUF_INST_NUM					16

#define ENTER_FUNC							HxSEMT_Get (s_ulPLRingBuf_SemId)
#define LEAVE_FUNC							HxSEMT_Release (s_ulPLRingBuf_SemId)
#define ENTER_INST(pstInst)					HxSEMT_Get (pstInst->ulSemId)
#define LEAVE_INST(pstInst)					HxSEMT_Release (pstInst->ulSemId)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct _plRingBufInst  _plRingBufInst_t;

struct _plRingBufInst
{
	unsigned long				 ulSemId;
	HBOOL						 bExecuting;

	Handle_t					 hInstHandle;
	Handle_t					 hHxRingbufHandle;

	_plRingBufInst_t			*owner;
	_plRingBufInst_t			*next;		// list of no-owner's _plRingBufInst
};

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32			 s_ulPLRingBuf_SemId;
STATIC _plRingBufInst_t	 s_astPLRingBuf_Instance[MAX_RINGBUF_INST_NUM];

#define ___INTERNAL_FUNCTIONS___
STATIC _plRingBufInst_t *_pl_ringbufgetEmptyInstance (void)
{
	HUINT32				 ulIndex;
	HERROR				 hErr;

	for (ulIndex = 0; ulIndex < MAX_RINGBUF_INST_NUM; ulIndex++)
	{
		if (TRUE != s_astPLRingBuf_Instance[ulIndex].bExecuting)
		{
			if (0 == s_astPLRingBuf_Instance[ulIndex].ulSemId)
			{
				HUINT32			ulSemId;
				HCHAR			szSemName[16];

				HxSTD_PrintToStrN (szSemName, 16, "RingBuf%02dSem", ulIndex);
				hErr = HxSEMT_Create (&ulSemId, szSemName, HxSEMT_FIFO);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("Semaphore failed!!!\n");
					return NULL;
				}

				s_astPLRingBuf_Instance[ulIndex].ulSemId = ulSemId;
			}

			return &(s_astPLRingBuf_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC _plRingBufInst_t *_pl_ringbufgetInstance (Handle_t hRingBuf)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < MAX_RINGBUF_INST_NUM; ulIndex++)
	{
		if ((TRUE == s_astPLRingBuf_Instance[ulIndex].bExecuting) && (hRingBuf == s_astPLRingBuf_Instance[ulIndex].hInstHandle))
		{
			return &(s_astPLRingBuf_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC HERROR _pl_ringbufclearInstance (_plRingBufInst_t *pstInst)
{
	unsigned long			 ulSemId = pstInst->ulSemId;

	// 반드시 지우면 안되는 Semaphore ID 외에는 다 지운다.

	HxSTD_MemSet (pstInst, 0, sizeof(_plRingBufInst_t));
	pstInst->ulSemId = ulSemId;
	return ERR_OK;
}


#define _____FOR_API_FUNCTIONS_____
STATIC Handle_t _pl_ringbufcreateRingBuffer (HxRingBuf_Setup_t *pstSetup)
{
	STATIC Handle_t			 s_hLastRingBuf = 0x0001;
	Handle_t				 hRingBuf = (s_hLastRingBuf < 0xFFFFFFF0) ? s_hLastRingBuf + 1 : 0x00000001;
	_plRingBufInst_t		*pstInst;

	if ((NULL == pstSetup) || (0 == pstSetup->ulMaxSize))
	{
		HxLOG_Error ("Setup Value wrong\n");
		return HANDLE_NULL;
	}

	while (1)
	{
		if (NULL == _pl_ringbufgetInstance (hRingBuf))
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

	pstInst = _pl_ringbufgetEmptyInstance();
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

STATIC void _pl_ringbufdestroyRingBuffer (Handle_t hRingBuf)
{
	_plRingBufInst_t			*pstInst = _pl_ringbufgetInstance (hRingBuf);

	if (NULL != pstInst)
	{
		HLIB_RINGBUF_Destroy(pstInst->hHxRingbufHandle);
		_pl_ringbufclearInstance (pstInst);
	}
}

#define ___EXTERNAL_FUNCTIONS___
HERROR PL_RINGBUF_Init (void)
{
	HERROR		 hErr;

	HxLOG_Trace();

	hErr = HxSEMT_Create (&s_ulPLRingBuf_SemId, "PlRingBufSem", HxSEMT_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Semaphore failed!!!\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet (s_astPLRingBuf_Instance, 0, sizeof(_plRingBufInst_t) * MAX_RINGBUF_INST_NUM);
	return ERR_OK;
}

void PL_RINGBUF_DeInit (void)
{
	HxLOG_Trace();

	HxSEMT_Destroy(s_ulPLRingBuf_SemId);
	HxSTD_MemSet (s_astPLRingBuf_Instance, 0, sizeof(_plRingBufInst_t) * MAX_RINGBUF_INST_NUM);
}


Handle_t PL_RINGBUF_Create (HxRingBuf_Setup_t *pstSetup)
{
	Handle_t			 hResult;

	HxLOG_Trace();

	ENTER_FUNC;
	hResult = _pl_ringbufcreateRingBuffer (pstSetup);
	LEAVE_FUNC;

	return hResult;
}

void PL_RINGBUF_Destroy (Handle_t hRingBuf)
{
	_plRingBufInst_t			*pstInst;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		_pl_ringbufdestroyRingBuffer (hRingBuf);
		LEAVE_INST (pstInst);
	}
	LEAVE_FUNC;
}

//2014/6/25 mhkang: [SAT>IP Owner/No-owner] append no-owner's hRinBuf to owner's.
HERROR PL_RINGBUF_AppendRingBuf(Handle_t hRingBufNoOwner, Handle_t hRingBufOwner)
{
	_plRingBufInst_t		*pstInstNoOwner;
	_plRingBufInst_t		*pstInstOwner;
	_plRingBufInst_t		*temp;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInstNoOwner = _pl_ringbufgetInstance (hRingBufNoOwner);
    if (pstInstNoOwner == NULL)
    {
        HxLOG_Info("pstInstNoOwner is null(hRingBufNoOwner:%d)\n", hRingBufNoOwner);
        return hErr;
    }

	pstInstOwner   = _pl_ringbufgetInstance (hRingBufOwner);
	LEAVE_FUNC;

	if (pstInstOwner == NULL)
	{
		HxLOG_Info("pstInstOwner is null(hRingBufNoOwner:%d)\n", hRingBufNoOwner);
		return hErr;
	}

	ENTER_INST (pstInstOwner);

	temp = pstInstOwner;
	while(temp != NULL)
	{
		if (temp->next == NULL)
		{
			temp->next = pstInstNoOwner;
			pstInstNoOwner->owner = temp;
			hErr = ERR_OK;

			HxLOG_Info("Append hRingBufSrc:%d hRingBufOwner:%d\n", hRingBufNoOwner, hRingBufOwner);

			break;
		}

		temp = temp->next;
	}
	LEAVE_INST (pstInstOwner);

	return hErr;
}

//2014/6/25 mhkang: [SAT>IP Owner/No-owner] remove no-owner's hRingBuf from owner's.
HERROR PL_RINGBUF_RemoveRingBuf(Handle_t hRingBufNoOwner)
{
	_plRingBufInst_t		*pstInstNoOwner;
	_plRingBufInst_t		*pstInstOwner;
	_plRingBufInst_t		*temp, *next, *current;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInstNoOwner = _pl_ringbufgetInstance (hRingBufNoOwner);
    if (pstInstNoOwner == NULL)
    {
        HxLOG_Info("pstInstNoOwner is null(hRingBufNoOwner:%d)\n", hRingBufNoOwner);
        return hErr;
    }

	pstInstOwner   = pstInstNoOwner->owner;
	LEAVE_FUNC;

	if (pstInstOwner == NULL)
	{
		HxLOG_Info("pstInstOwner is null(hRingBufNoOwner:%d)\n", hRingBufNoOwner);
		return hErr;
	}

	ENTER_INST (pstInstOwner);

	current = pstInstOwner;
	temp = pstInstOwner->next;

	while(temp != NULL)
	{
		if (temp->hInstHandle ==  pstInstNoOwner->hInstHandle)
		{
			next = temp->next;
			current->next = next;
			hErr = ERR_OK;

			HxLOG_Info("Remove hRingBufSrc:%d hRingBufOwner:%d\n", hRingBufNoOwner, pstInstOwner->hInstHandle);

			break;
		}

		current = temp;
		temp = temp->next;
	}

	LEAVE_INST (pstInstOwner);

	return hErr;
}

HERROR PL_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize)
{
	_plRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize)
{
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize)
{
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	HUINT32					 ulWrittenSize = 0;
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);	
	LEAVE_FUNC;

	if (NULL != pstInst)
	{	
		ENTER_INST (pstInst);

		hErr = HLIB_RINGBUF_WriteData (pstInst->hHxRingbufHandle, ulDataSize, pucData, &ulWrittenSize);

		if (hErr == ERR_OK)
		{
			if (pstInst->next != NULL) // if no-owner exists.
			{
				// recuresive call.
				hErr = PL_RINGBUF_WriteData(pstInst->next->hInstHandle,ulDataSize, pucData,  pulWrittenSize);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("hRingBuf(0x%x) hErr(%d) or pstInst(%d) error\n", pstInst->next->hInstHandle, hErr, pstInst);
				}
			}
		}

		LEAVE_INST (pstInst);

	}

	if( ERR_OK != hErr || NULL == pstInst)
	{
		HxLOG_Error("hRingBuf(0x%x) hErr(%d) or pstInst(%d) error\r\n", hRingBuf, hErr, pstInst);
	}

	if (NULL != pulWrittenSize)					{ *pulWrittenSize = ulWrittenSize; }
	return hErr;
}

HERROR PL_RINGBUF_FlushToChunk (Handle_t hRingBuf)
{
	_plRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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


HERROR PL_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData)
{
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId)
{
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_ClearBuffer (Handle_t hRingBuf)
{
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus)
{
	_plRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstStatus)					{ return ERR_FAIL; }

	HxSTD_MemSet (pstStatus, 0, sizeof(HxRingBuf_Status_t));

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf)
{
	_plRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf)
{
	_plRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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

HERROR PL_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf)
{
	_plRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
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


void PL_RINGBUF_Dump (Handle_t hRingBuf)
{
#if defined(CONFIG_DEBUG)
	_plRingBufInst_t		*pstInst = NULL;

	ENTER_FUNC;
	pstInst = _pl_ringbufgetInstance (hRingBuf);
	LEAVE_FUNC;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
 		HxLOG_Print("============================================================\r\n");
		HxLOG_Print("hRingBuf(0x%x)\n", (HUINT32)hRingBuf);
		HxLOG_Print("pstInst(0x%x)\n", 	(HUINT32)pstInst);
		HxLOG_Print("  + ulSemId           : 0x%x\r\n", pstInst->ulSemId);
		HxLOG_Print("  + bExecuting        : %s\r\n", ((pstInst->bExecuting == TRUE) ? "TRUE" : "FALSE"));

		HLIB_RINGBUF_DEBUG_Dump(pstInst->hHxRingbufHandle);
		LEAVE_INST (pstInst);
	}
#endif
}


/* end of file */
