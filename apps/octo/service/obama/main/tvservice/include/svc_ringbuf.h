/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 svc_ringbuf.h
	@brief	  MW의 common utility 파일 헤더

	Description: MW와 AP가 모두 사용하는 DRM관련  API의 헤더 파일이다.
	Module: MWC/UTIL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MW_RINGBUF_H__
#define	__MW_RINGBUF_H__

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
typedef enum
{
	eRingBuf_Type_Linear,			// Linear Type : 하나의 긴 Linear Buffer로 간주하는 Type
	eRingBuf_Type_Chunk,			// Chunk Type : 여러개의 Chunk Size로 읽는 Type
	eRingBuf_Type_VariableChunk,	// Variable Chunk Type : 여러개의 Chunk 이지만 각 Chunk Size는 각각 다를 수 있다. Write시 반드시 Variable Chunk 의 경우 그 Chunk의 End를 알려주도록 해야 한다.

	eRingBuf_Type_EndOfCase
} SvcRingBuf_Type_e;

typedef enum
{
	eRingBuf_Read_Remove,			// 읽은 내용은 곧바로 지우고 비운다.
	eRingBuf_Read_Remain,			// 별도로 지우는 명령을 내리지 않는 이상 읽은 버퍼를 지우지 않는다.

	eRingBuf_Read_EndOfCase
} SvcRingBuf_ReadRule_e;


typedef struct
{
	SvcRingBuf_Type_e		eBufType;

	HUINT32					 ulMaxSize;
	HUINT32					 ulChunkSize;

	HBOOL					 bBlockOverflow;				// Overflow될 경우, 이전 것을 지우지 않고 정지 상태로 놔두는가 에 대한 Flag
	SvcRingBuf_ReadRule_e	 eRuleAfterRead;				// Read한 Data를 어떻게 처리할 것인가에 대한 Type

	HxRingbuf_Signal		 cbSignal;
} SvcRingBuf_Setup_t;


typedef struct
{
	HUINT64					 ullAccumulatedSize;			// 이제까지 계속 Write 되어 온 전체 사이즈. 그동안 지워진 것 모두 다 포함.
	HUINT64					 ullAbsoluteStart;				// 현재 존재하는 Buffer의 Start Position. 그동안 받아온 사이즈로 계산했을 때의 위치임.
	HUINT64					 ullAbsoluteCurrent;			// 현재 존재하는 Buffer의 Current Read Position. 그동안 받아온 사이즈로 계산했을 때의 위치임.

	HUINT32					 ulBufferSize;					// 현재 Buffering 되어 있는 사이즈
	HUINT32					 ulCurrentPnt;					// Buffering된 Size에 비하여의 Current Read Position의 위치.
	HUINT32					 ulUnreadSize;					// Buffering된 것 중 아직 읽지 않은 사이즈

	// Chunk 관련 정보
	HUINT32 				 ulChunkNum;
	HUINT32 				 ulStartChunkId, ulEndChunkId;

	HBOOL					 bOverflowed;					// Ring Buffer 가 Overflow 되었음을 나타내는 Flag
} SvcRingBuf_Status_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern Handle_t SVC_RINGBUF_Create (SvcRingBuf_Setup_t *pstSetup);
extern void		SVC_RINGBUF_Destroy (Handle_t hRingBuf);
extern HERROR	SVC_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize);
extern HERROR	SVC_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize);
extern HERROR	SVC_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize);
extern HERROR	SVC_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	SVC_RINGBUF_FlushToChunk (Handle_t hRingBuf);
extern HERROR	SVC_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData);
extern HERROR	SVC_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId);
extern HERROR	SVC_RINGBUF_ClearBuffer (Handle_t hRingBuf);
extern HERROR	SVC_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, SvcRingBuf_Status_t *pstStatus);
extern HERROR	SVC_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf);
extern HERROR	SVC_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf);
extern HERROR	SVC_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf);

#endif /* !__MW_RINGBUF_H__ */

