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
#define MAX_RINGBUF_INST_NUM					16

#define ENTER_FUNC							VK_SEM_Get (s_ulHxRingBuf_SemId)
#define LEAVE_FUNC							VK_SEM_Release (s_ulHxRingBuf_SemId)
#define ENTER_INST(pstInst)					VK_SEM_Get (pstInst->ulSemId)
#define LEAVE_INST(pstInst)					VK_SEM_Release (pstInst->ulSemId)


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
STATIC _hxRingBufInst_t *hxringbuf_getEmptyInstance (void)
{
	return HLIB_STD_MemCalloc(sizeof(_hxRingBufInst_t));
}

STATIC _hxRingBufInst_t *hxringbuf_getInstance (Handle_t hRingBuf)
{
	return (_hxRingBufInst_t *)hRingBuf;
}

STATIC HERROR hxringbuf_clearInstance (_hxRingBufInst_t *pstInst)
{
	HLIB_MEM_Free(pstInst);
	return ERR_OK;
}


#define _____FOR_API_FUNCTIONS_____
STATIC Handle_t hxringbuf_createRingBuffer (HxRingBuf_Setup_t *pstSetup)
{
	_hxRingBufInst_t		*pstInst;
	HERROR					 hErr;

	if ((NULL == pstSetup) || (0 == pstSetup->ulMaxSize))
	{
		HxLOG_Error ("Setup Value wrong\n");
		return HANDLE_NULL;
	}

	/* chunk 방식일 경우, max size를 배수가 n이 되도록 수정한다. */
	switch (pstSetup->eBufType)
	{
	case eHxRINGBUF_TYPE_CHUNK:
	case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
		if(pstSetup->ulChunkSize > 0)
			pstSetup->ulMaxSize = pstSetup->ulMaxSize - (pstSetup->ulMaxSize % pstSetup->ulChunkSize);
		break;

	default:
		break;
	}

	pstInst = hxringbuf_getEmptyInstance();
	if (NULL == pstInst)
	{
		HxLOG_Error ("No empty instance...\n");
		return HANDLE_NULL;
	}

	switch (pstSetup->eBufType)
	{
	case eHxRINGBUF_TYPE_CHUNK:
		hErr = HLIB_RINGBUF_CHUNK_SetupInstance (pstInst, pstSetup);
		break;

	case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
		hErr = HLIB_RINGBUF_VARCHUNK_SetupInstance (pstInst, pstSetup);
		break;

	case eHxRINGBUF_TYPE_LINEAR:
	default:
		hErr = HLIB_RINGBUF_LINEAR_SetupInstance (pstInst, pstSetup);
		break;
	}

	if (ERR_OK != hErr)
	{
		hErr=hxringbuf_clearInstance(pstInst);
		HxLOG_Error ("Setup failed, BufferType(%d)\n", pstSetup->eBufType);
		return HANDLE_NULL;
	}

	return (Handle_t)pstInst;

}

STATIC void hxringbuf_destroyRingBuffer (Handle_t hRingBuf)
{
	_hxRingBufInst_t			*pstInst = hxringbuf_getInstance (hRingBuf);

	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_LINEAR:
			HLIB_RINGBUF_LINEAR_DestroyInstance (pstInst);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			HLIB_RINGBUF_VARCHUNK_DestroyInstance (pstInst);
			break;

		case eHxRINGBUF_TYPE_CHUNK:
			HLIB_RINGBUF_CHUNK_DestroyInstance (pstInst);
			break;

		default:
			return;
		}

		hxringbuf_clearInstance (pstInst);
	}
}

#define ___EXTERNAL_FUNCTIONS___

Handle_t HLIB_RINGBUF_Create (HxRingBuf_Setup_t *pstSetup)
{
	Handle_t			 hResult;

	HxLOG_Trace();

	hResult = hxringbuf_createRingBuffer (pstSetup);

	return hResult;
}

void HLIB_RINGBUF_Destroy (Handle_t hRingBuf)
{
	_hxRingBufInst_t			*pstInst;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		hxringbuf_destroyRingBuffer (hRingBuf);
	}
}

HERROR HLIB_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize)
{
	_hxRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);

	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_LINEAR:
			hErr = HLIB_RINGBUF_LINEAR_ReadData (pstInst, ulDataSize, pucData, pulReadSize);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(NULL != pstInst->cbSignal && ERR_OK == hErr)
		{
			HxRingBuf_Status_t		stStatus;

			HxSTD_MemSet (&stStatus, 0, sizeof(HxRingBuf_Status_t));

			switch (pstInst->eBufType)
			{
			case eHxRINGBUF_TYPE_LINEAR:
				hErr = HLIB_RINGBUF_LINEAR_GetBufferingStatus (pstInst, &stStatus);
				break;

			case eHxRINGBUF_TYPE_CHUNK:
				hErr = HLIB_RINGBUF_CHUNK_GetBufferingStatus (pstInst, &stStatus);
				break;

			case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
				hErr = HLIB_RINGBUF_VARCHUNK_GetBufferingStatus (pstInst, &stStatus);
				break;

			default:
				hErr = ERR_FAIL;
				break;
			}

			if(ERR_OK != hErr)
			{
				// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
				pstInst->cbSignal(eHxRINGBUF_SIG_Read, hRingBuf, &stStatus);
			}
			hErr = ERR_OK;
		}

	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize)
{
	_hxRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_CHUNK:
			hErr = HLIB_RINGBUF_CHUNK_ReadChunk (pstInst, ulChunkId, pucData, pulChunkSize);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			hErr = HLIB_RINGBUF_VARCHUNK_ReadChunk (pstInst, ulChunkId, pucData, pulChunkSize);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(NULL != pstInst->cbSignal && ERR_OK == hErr)
		{
			HxRingBuf_Status_t		stStatus;

			HxSTD_MemSet (&stStatus, 0, sizeof(HxRingBuf_Status_t));

			switch (pstInst->eBufType)
			{
			case eHxRINGBUF_TYPE_LINEAR:
				hErr = HLIB_RINGBUF_LINEAR_GetBufferingStatus (pstInst, &stStatus);
				break;

			case eHxRINGBUF_TYPE_CHUNK:
				hErr = HLIB_RINGBUF_CHUNK_GetBufferingStatus (pstInst, &stStatus);
				break;

			case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
				hErr = HLIB_RINGBUF_VARCHUNK_GetBufferingStatus (pstInst, &stStatus);
				break;

			default:
				hErr = ERR_FAIL;
				break;
			}

			if(ERR_OK != hErr)
			{
				// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
				pstInst->cbSignal(eHxRINGBUF_SIG_Read, hRingBuf, &stStatus);
			}
			hErr = ERR_OK;
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize)
{
	_hxRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_CHUNK:
			hErr = HLIB_RINGBUF_CHUNK_GetChunkSize (pstInst, ulChunkId, pulChunkSize);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			hErr = HLIB_RINGBUF_VARCHUNK_GetChunkSize (pstInst, ulChunkId, pulChunkSize);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	HUINT32					ulWrittenSize = 0;
	_hxRingBufInst_t		*pstInst;
	HERROR					hErr = ERR_FAIL;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		if (TRUE != pstInst->bBlockingWrite)
		{
			switch (pstInst->eBufType)
			{
			case eHxRINGBUF_TYPE_LINEAR:
				hErr = HLIB_RINGBUF_LINEAR_WriteData (pstInst, ulDataSize, pucData, &ulWrittenSize);
				break;

			case eHxRINGBUF_TYPE_CHUNK:
				hErr = HLIB_RINGBUF_CHUNK_WriteData (pstInst, ulDataSize, pucData,  &ulWrittenSize);
				break;

			case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
				hErr = HLIB_RINGBUF_VARCHUNK_WriteData (pstInst, ulDataSize, pucData, &ulWrittenSize);
				break;

			default:
				hErr = ERR_FAIL;
				break;
			}

			if(NULL != pstInst->cbSignal && ERR_OK == hErr)
			{
				HxRingBuf_Status_t		stStatus;

				HxSTD_MemSet (&stStatus, 0, sizeof(HxRingBuf_Status_t));

				switch (pstInst->eBufType)
				{
				case eHxRINGBUF_TYPE_LINEAR:
					hErr = HLIB_RINGBUF_LINEAR_GetBufferingStatus (pstInst, &stStatus);
					break;

				case eHxRINGBUF_TYPE_CHUNK:
					hErr = HLIB_RINGBUF_CHUNK_GetBufferingStatus (pstInst, &stStatus);
					break;

				case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
					hErr = HLIB_RINGBUF_VARCHUNK_GetBufferingStatus (pstInst, &stStatus);
					break;

				default:
					hErr = ERR_FAIL;
					break;
				}

				if(ERR_OK == hErr)
				{
					// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
					pstInst->cbSignal(eHxRINGBUF_SIG_Write, hRingBuf, &stStatus);
				}
				hErr = ERR_OK;
			}
		}
		else
		{
			HxLOG_Debug ("Ring Buffer (0x%08x) Write Lock Enabled:\n", hRingBuf);
		}
	}

	if( ERR_OK != hErr || NULL == pstInst)
	{
		HxLOG_Error("hRingBuf(%d) hErr(%d) or pstInst(%d) error\r\n", hRingBuf, hErr, pstInst);
	}

	if (NULL != pulWrittenSize)
	{
		*pulWrittenSize = ulWrittenSize;
	}
	return hErr;
}

HERROR HLIB_RINGBUF_FlushToChunk (Handle_t hRingBuf)
{
	_hxRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			hErr = HLIB_RINGBUF_VARCHUNK_FlushToChunk (pstInst);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(NULL != pstInst->cbSignal && ERR_OK == hErr)
		{
			// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
			pstInst->cbSignal(eHxRINGBUF_SIG_FlushToChunk, hRingBuf, NULL);
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}


HERROR HLIB_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData)
{
	_hxRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_LINEAR:
			hErr = HLIB_RINGBUF_LINEAR_EraseData (pstInst, ulEraseData);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(NULL != pstInst->cbSignal && ERR_OK == hErr)
		{
			// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
			pstInst->cbSignal(eHxRINGBUF_SIG_EraseData, hRingBuf, NULL);
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId)
{
	_hxRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();


	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_CHUNK:
			hErr = HLIB_RINGBUF_CHUNK_DeleteChunk (pstInst, ulChunkId);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			hErr = HLIB_RINGBUF_VARCHUNK_DeleteChunk (pstInst, ulChunkId);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(NULL != pstInst->cbSignal && ERR_OK == hErr)
		{
			// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
			pstInst->cbSignal(eHxRINGBUF_SIG_EraseChunk, hRingBuf, NULL);
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_ClearBuffer (Handle_t hRingBuf)
{
	_hxRingBufInst_t			*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();


	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_LINEAR:
			hErr = HLIB_RINGBUF_LINEAR_ClearBuffer (pstInst);
			break;

		case eHxRINGBUF_TYPE_CHUNK:
			hErr = HLIB_RINGBUF_CHUNK_ClearBuffer (pstInst);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			hErr = HLIB_RINGBUF_VARCHUNK_ClearBuffer (pstInst);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}

		if(NULL != pstInst->cbSignal && ERR_OK == hErr)
		{
			// callback 내부에서 HLIB_RINGBUF 사용하지 말 것!!!
			pstInst->cbSignal(eHxRINGBUF_SIG_ClearBuffer, hRingBuf, NULL);
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus)
{
	_hxRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstStatus)					{ return ERR_FAIL; }

	HxSTD_MemSet (pstStatus, 0, sizeof(HxRingBuf_Status_t));

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_LINEAR:
			hErr = HLIB_RINGBUF_LINEAR_GetBufferingStatus (pstInst, pstStatus);
			break;

		case eHxRINGBUF_TYPE_CHUNK:
			hErr = HLIB_RINGBUF_CHUNK_GetBufferingStatus (pstInst, pstStatus);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			hErr = HLIB_RINGBUF_VARCHUNK_GetBufferingStatus (pstInst, pstStatus);
			break;

		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf)
{
	_hxRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		pstInst->bBlockOverflow = FALSE;
		hErr = ERR_OK;
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf)
{
	_hxRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		pstInst->bBlockingWrite = TRUE;
		hErr = ERR_OK;
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}

HERROR HLIB_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf)
{
	_hxRingBufInst_t		*pstInst;
	HERROR					 hErr = ERR_FAIL;

	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		pstInst->bBlockingWrite = FALSE;
		hErr = ERR_OK;
	}

	if( ERR_OK != hErr|| NULL == pstInst)
	{
		HxLOG_Error("hErr(%d) or pstInst(%d) error\r\n", hErr, pstInst);
	}

	return hErr;
}



//for debug
static HCHAR *hxringbuf_DEBUG_BufTypeToStr(HxRingBuf_Type_e type)
{

	switch(type)
	{
	ENUM_TO_STR(eHxRINGBUF_TYPE_LINEAR);
	ENUM_TO_STR(eHxRINGBUF_TYPE_CHUNK);
	ENUM_TO_STR(eHxRINGBUF_TYPE_VARIABLE_CHUNK);
	ENUM_TO_STR(eHxRINGBUF_TYPE_MAX);

	default:
		break;
	}

	return "Error";
}

void HLIB_RINGBUF_DEBUG_Dump (Handle_t hRingBuf)
{
	_hxRingBufInst_t		*pstInst = NULL;


	pstInst = hxringbuf_getInstance (hRingBuf);
	if (NULL != pstInst)
	{
		HxLOG_Print("-----------------------------------\r\n");
		HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
		HxLOG_Print("hRingBuf(0x%x)\n", (HUINT32)hRingBuf);
		HxLOG_Print("pstInst(0x%x)\n", (HUINT32)pstInst);
		HxLOG_Print("  + bBlockOverflow    : %d\r\n", pstInst->bBlockOverflow);
		HxLOG_Print("  + eRuleAfterRead    : %d\r\n", pstInst->eRuleAfterRead);
		HxLOG_Print("  + ulMaxSize         : %9d(%dkb)\r\n", pstInst->ulMaxSize, pstInst->ulMaxSize/1024);
		HxLOG_Print("  + ullAccumulatedSize: %9lld(%lldkb)\r\n", pstInst->ullAccumulatedSize, 	pstInst->ullAccumulatedSize/1024);
		HxLOG_Print("  + ullAbsoluteStart  : %9lld(%lldkb)\r\n", pstInst->ullAbsoluteStart, 	pstInst->ullAbsoluteStart/1024);
		HxLOG_Print("  + ullAbsoluteCurrent: %9lld(%lldkb)\r\n", pstInst->ullAbsoluteCurrent, 	pstInst->ullAbsoluteCurrent/1024);
		HxLOG_Print("  + eBufType          : %s\r\n", hxringbuf_DEBUG_BufTypeToStr(pstInst->eBufType));
		switch (pstInst->eBufType)
		{
		case eHxRINGBUF_TYPE_LINEAR:
			HLIB_RINGBUF_LINEAR_Dump (pstInst);
			break;

		case eHxRINGBUF_TYPE_CHUNK:
			HLIB_RINGBUF_CHUNK_Dump (pstInst);
			break;

		case eHxRINGBUF_TYPE_VARIABLE_CHUNK:
			HLIB_RINGBUF_VARCHUNK_Dump(pstInst);
			break;

		default:
			break;
		}
		HxLOG_Print("-----------------------------------\r\n");
 	}
}

/* end of file */

