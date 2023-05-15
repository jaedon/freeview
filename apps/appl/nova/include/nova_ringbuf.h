/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  nova_ringbuf.h
	@brief	  homma/pl_ringbuf_context.c 소스코드를 가져왔음.
	@attention Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

#ifndef	__NOVA_RINGBUF_H__
#define __NOVA_RINGBUF_H__

/* *************************************************************************************************** */
#define ___LOCAL_INCLUDE________________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */






/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif

extern HERROR	PL_RINGBUF_Init (void);
extern void 	PL_RINGBUF_DeInit (void);
extern Handle_t PL_RINGBUF_Create (HxRingBuf_Setup_t *pstSetup);
extern void		PL_RINGBUF_Destroy (Handle_t hRingBuf);
extern HERROR	PL_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize);
extern HERROR	PL_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize);
extern HERROR	PL_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize);
extern HERROR	PL_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	PL_RINGBUF_FlushToChunk (Handle_t hRingBuf);
extern HERROR	PL_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData);
extern HERROR	PL_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId);
extern HERROR	PL_RINGBUF_ClearBuffer (Handle_t hRingBuf);
extern HERROR	PL_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus);
extern HERROR	PL_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf);
extern HERROR	PL_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf);
extern HERROR	PL_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf);

/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

extern void PL_RINGBUF_Dump (Handle_t hRingBuf);

#ifdef __cplusplus
}
#endif

#endif /* !__NOVA_RINGBUF_H__ */

