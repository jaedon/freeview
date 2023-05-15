/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_ringbuf.h
	@brief	  MW의 common utility 파일 헤더

	Description: MW와 AP가 모두 사용하는 DRM관련  API의 헤더 파일이다.
	Module: MWC/UTIL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_RINGBUF_H___
#define	__PAL_RINGBUF_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	PAL_RINGBUF_Init (void);
extern Handle_t PAL_RINGBUF_Create (HxRingBuf_Setup_t *pstSetup);
extern void		PAL_RINGBUF_Destroy (Handle_t hRingBuf);
extern HERROR	PAL_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize);
extern HERROR	PAL_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize);
extern HERROR	PAL_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize);
extern HERROR	PAL_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	PAL_RINGBUF_FlushToChunk (Handle_t hRingBuf);
extern HERROR	PAL_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData);
extern HERROR	PAL_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId);
extern HERROR	PAL_RINGBUF_ClearBuffer (Handle_t hRingBuf);
extern HERROR	PAL_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus);
extern HERROR	PAL_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf);
extern HERROR	PAL_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf);
extern HERROR	PAL_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf);

#if defined(CONFIG_DEBUG)
extern void PAL_RINGBUF_Dump (Handle_t hRingBuf);
#endif


#endif /* !__PAL_RINGBUF_H___ */

