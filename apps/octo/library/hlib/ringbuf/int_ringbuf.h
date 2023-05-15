/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mwc_drm.h
	@brief	  MW�� common utility ���� ���

	Description: MW�� AP�� ��� ����ϴ� DRM����  API�� ��� �����̴�.
	Module: MWC/UTIL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__INT_RINGBUF_H___
#define	__INT_RINGBUF_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#define HLIB_RINGBUF_CHUNKID_CACHE_SIZE			8

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
typedef struct
{
	HUINT32						 ulMaxSize;
	HUINT8						*pucBuffer;
	HUINT32						 ulHeadPtr, ulTailPtr, ulCurrPtr;
} _palRingBuf_Linear_t;

typedef struct
{
	HINT32						 nChunkId;				// ID: ���������� �������� �þ�� ID
	HINT32						 nChunkIndex;			// Index:���� Buffer���� ��ġ
} _palRingBuf_ChunkIndex_t;

typedef struct
{
	HUINT32						 ulMaxSize;
	HUINT8						*pucBuffer;

	HUINT32						 ulChunkNum, ulChunkSize;
	HINT32						*pnChunkIdArray;

	HUINT32						 ulHeadChunk, ulTailChunk;
	HUINT32						 ulWritePtr;
	HINT32						 nNextChunkId;

	_palRingBuf_ChunkIndex_t	 astChunkIndexCache[HLIB_RINGBUF_CHUNKID_CACHE_SIZE];
} _palRingBuf_Chunk_t;

typedef struct
{
	HINT32						 nChunkId;				// ID: ���������� �������� �þ�� ID
	HUINT32						 ulChunkSize;			// Variable Sized Chunk �̱� ������ Chunk Size�� ���� �ʿ��ϴ�

	// �ϼ��� Chunk �� ��츸 ����ϴ� ����:
	HUINT32						 ulStartPtr, ulEndPtr;	// Chunk �� Start/End Point. (CHUNK�� �ϼ��� �ñ⿡ �д´� )

	// Chunk Data �� �ƴ϶� Writing �߿� ���̴� variables
	HUINT32						 ulHeadPtr, ulTailPtr;	// Chunk Writing ����  ����

} _palRingBuf_VarChunkInfo_t;

typedef struct
{
	HUINT32						 ulMaxSize;
	HUINT8						*pucBuffer;

	HUINT32						 ulChunkNum, ulChunkArrayLength;
	_palRingBuf_VarChunkInfo_t	*pstChunkInfoArray;
	_palRingBuf_VarChunkInfo_t	 stWritingChunkInfo;

	_palRingBuf_ChunkIndex_t	 astChunkIndexCache[HLIB_RINGBUF_CHUNKID_CACHE_SIZE];
} _palRingBuf_VarChunk_t;

typedef struct
{
	HxRingBuf_Type_e			 eBufType;
	HBOOL						 bBlockOverflow;
	HBOOL						 bOverflowed;
	HBOOL						 bBlockingWrite;				// Write�� ���ϵ��� �ϴ� Lock: �Ϻ� �������� �̷��� ����� �ʿ��ϴ�.
	HxRingBuf_ReadRule_e		 eRuleAfterRead;
	HUINT32						 ulMaxSize;

	union
	{
		_palRingBuf_Linear_t	 stLinearBuf;
		_palRingBuf_Chunk_t		 stChunkBuf;
		_palRingBuf_VarChunk_t	 stVarChunkBuf;
	} unBuffer;

	HUINT64						 ullAccumulatedSize;			// �������� ��� Write �Ǿ� �� ��ü ������. �׵��� ������ �� ��� �� ����.
	HUINT64						 ullAbsoluteStart;				// ���� �����ϴ� Buffer�� Start Position. �׵��� �޾ƿ� ������� ������� ���� ��ġ��.
	HUINT64						 ullAbsoluteCurrent;			// ���� �����ϴ� Buffer�� Current Read Position. �׵��� �޾ƿ� ������� ������� ���� ��ġ��.
	HxRingbuf_Signal			 cbSignal;
} _hxRingBufInst_t;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

// pal_ringbuf_linear.c
extern HERROR	HLIB_RINGBUF_LINEAR_SetupInstance (_hxRingBufInst_t *pstInst, HxRingBuf_Setup_t *pstSetup);
extern HERROR	HLIB_RINGBUF_LINEAR_DestroyInstance (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_LINEAR_ReadData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize);
extern HERROR	HLIB_RINGBUF_LINEAR_WriteData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	HLIB_RINGBUF_LINEAR_EraseData (_hxRingBufInst_t *pstInst, HUINT32 ulEraseSize);
extern HERROR	HLIB_RINGBUF_LINEAR_ClearBuffer (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_LINEAR_GetBufferingStatus (_hxRingBufInst_t *pstInst, HxRingBuf_Status_t *pstStatus);

// pal_ringbuf_chunk.c
extern HERROR	HLIB_RINGBUF_CHUNK_SetupInstance (_hxRingBufInst_t *pstInst, HxRingBuf_Setup_t *pstSetup);
extern HERROR	HLIB_RINGBUF_CHUNK_DestroyInstance (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_CHUNK_ReadChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HCHAR *pucData, HUINT32 *pulChunkSize);
extern HERROR	HLIB_RINGBUF_CHUNK_GetChunkSize (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HUINT32 *pulChunkSize);
extern HERROR	HLIB_RINGBUF_CHUNK_WriteData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	HLIB_RINGBUF_CHUNK_DeleteChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId);
extern HERROR	HLIB_RINGBUF_CHUNK_ClearBuffer (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_CHUNK_GetBufferingStatus (_hxRingBufInst_t *pstInst, HxRingBuf_Status_t *pstStatus);

// pal_ringbuf_varchunk.c
extern HERROR	HLIB_RINGBUF_VARCHUNK_SetupInstance (_hxRingBufInst_t *pstInst, HxRingBuf_Setup_t *pstSetup);
extern HERROR	HLIB_RINGBUF_VARCHUNK_DestroyInstance (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_VARCHUNK_ReadChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HCHAR *pucData, HUINT32 *pulChunkSize);
extern HERROR	HLIB_RINGBUF_VARCHUNK_GetChunkSize (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId, HUINT32 *pulChunkSize);
extern HERROR	HLIB_RINGBUF_VARCHUNK_WriteData (_hxRingBufInst_t *pstInst, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	HLIB_RINGBUF_VARCHUNK_FlushToChunk (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_VARCHUNK_DeleteChunk (_hxRingBufInst_t *pstInst, HUINT32 ulChunkId);
extern HERROR	HLIB_RINGBUF_VARCHUNK_ClearBuffer (_hxRingBufInst_t *pstInst);
extern HERROR	HLIB_RINGBUF_VARCHUNK_GetBufferingStatus (_hxRingBufInst_t *pstInst, HxRingBuf_Status_t *pstStatus);

extern void HLIB_RINGBUF_LINEAR_Dump(_hxRingBufInst_t *pstInst);
extern void HLIB_RINGBUF_CHUNK_Dump(_hxRingBufInst_t *pstInst);
extern void HLIB_RINGBUF_VARCHUNK_Dump(_hxRingBufInst_t *pstInst);

#endif /* !__INT_RINGBUF_H___ */

