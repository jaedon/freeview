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
#include <svc_ringbuf.h>


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
STATIC HxRingBuf_Type_e svc_ringbuf_convertMwBufType2PalBufType (SvcRingBuf_Type_e eMwType)
{
	switch (eMwType)
	{
	case eRingBuf_Type_Linear:
		return eHxRINGBUF_TYPE_LINEAR;

	case eRingBuf_Type_Chunk:
		return eHxRINGBUF_TYPE_CHUNK;

	case eRingBuf_Type_VariableChunk:
		return eHxRINGBUF_TYPE_VARIABLE_CHUNK;

	default:
		break;
	}

	return eHxRINGBUF_TYPE_LINEAR;
}

STATIC HxRingBuf_ReadRule_e svc_ringbuf_convertMwRuleAfterRead2PalRuleAfterRead (SvcRingBuf_ReadRule_e eMwRule)
{
	switch (eMwRule)
	{
	case eRingBuf_Read_Remove:
		return eHxRINGBUF_READRULE_REMOVE;

	case eRingBuf_Read_Remain:
		return eHxRINGBUF_READRULE_REMAIN;

	default:
		break;
	}

	return eHxRINGBUF_READRULE_REMOVE;
}

STATIC void svc_ringbuf_convertMwSetup2PalSetup (SvcRingBuf_Setup_t *pstMwSetup, HxRingBuf_Setup_t *pstPalSetup)
{
	pstPalSetup->eBufType				= svc_ringbuf_convertMwBufType2PalBufType (pstMwSetup->eBufType);
	pstPalSetup->ulMaxSize				= pstMwSetup->ulMaxSize;
	pstPalSetup->ulChunkSize			= pstMwSetup->ulChunkSize;
	pstPalSetup->bBlockOverflow			= pstMwSetup->bBlockOverflow;
	pstPalSetup->eRuleAfterRead			= svc_ringbuf_convertMwRuleAfterRead2PalRuleAfterRead (pstMwSetup->eRuleAfterRead);
	pstPalSetup->cbSignal				= pstMwSetup->cbSignal;
}

STATIC void svc_ringbuf_convertPalStatus2MwStatus (HxRingBuf_Status_t *pstPalStatus, SvcRingBuf_Status_t *pstMwStatus)
{
	pstMwStatus->ullAccumulatedSize		= pstPalStatus->ullAccumulatedSize;
	pstMwStatus->ullAbsoluteStart		= pstPalStatus->ullAbsoluteStart;
	pstMwStatus->ullAbsoluteCurrent		= pstPalStatus->ullAbsoluteCurrent;
	pstMwStatus->ulBufferSize			= pstPalStatus->ulBufferSize;
	pstMwStatus->ulCurrentPnt			= pstPalStatus->ulCurrentPnt;
	pstMwStatus->ulUnreadSize			= pstPalStatus->ulUnreadSize;
	pstMwStatus->ulChunkNum				= pstPalStatus->ulChunkNum;
	pstMwStatus->ulStartChunkId			= pstPalStatus->ulStartChunkId;
	pstMwStatus->ulEndChunkId			= pstPalStatus->ulEndChunkId;
	pstMwStatus->bOverflowed			= pstPalStatus->bOverflowed;
}

#define ___EXTERNAL_FUNCTIONS___
Handle_t SVC_RINGBUF_Create (SvcRingBuf_Setup_t *pstSetup)
{
	HxRingBuf_Setup_t	 stPalSetup;

	if (NULL == pstSetup)					{ return ERR_FAIL; }

	svc_ringbuf_convertMwSetup2PalSetup (pstSetup, &stPalSetup);
	return PAL_RINGBUF_Create (&stPalSetup);
}

void SVC_RINGBUF_Destroy (Handle_t hRingBuf)
{
	PAL_RINGBUF_Destroy (hRingBuf);
}

HERROR SVC_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize)
{
	return PAL_RINGBUF_ReadData (hRingBuf, ulDataSize, pucData, pulReadSize);
}

HERROR SVC_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize)
{
	return PAL_RINGBUF_ReadChunk (hRingBuf, ulChunkId, pucData, pulChunkSize);
}

HERROR SVC_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize)
{
	return PAL_RINGBUF_GetChunkSize (hRingBuf, ulChunkId, pulChunkSize);
}

HERROR SVC_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize)
{
	return PAL_RINGBUF_WriteData (hRingBuf, ulDataSize, pucData, pulWrittenSize);
}

HERROR SVC_RINGBUF_FlushToChunk (Handle_t hRingBuf)
{
	return PAL_RINGBUF_FlushToChunk (hRingBuf);
}

HERROR SVC_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData)
{
	return PAL_RINGBUF_EraseData (hRingBuf, ulEraseData);
}

HERROR SVC_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId)
{
	return PAL_RINGBUF_EraseChunk (hRingBuf, ulChunkId);
}

HERROR SVC_RINGBUF_ClearBuffer (Handle_t hRingBuf)
{
	return PAL_RINGBUF_ClearBuffer (hRingBuf);
}

HERROR SVC_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, SvcRingBuf_Status_t *pstStatus)
{
	HxRingBuf_Status_t		 stPalStatus;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pstStatus)					{ return ERR_FAIL; }

	hErr = PAL_RINGBUF_GetBufferingStatus (hRingBuf, &stPalStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_RINGBUF_GetBufferingStatus err:\n");
		return ERR_FAIL;
	}

	svc_ringbuf_convertPalStatus2MwStatus (&stPalStatus, pstStatus);
	return ERR_OK;
}

HERROR SVC_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf)
{
	return PAL_RINGBUF_ResetOverflowFlag (hRingBuf);
}

HERROR SVC_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf)
{
	return PAL_RINGBUF_SetWriteBlockingLock (hRingBuf);
}

HERROR SVC_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf)
{
	return PAL_RINGBUF_ResetWriteBlockingLock (hRingBuf);
}

/* end of file */

