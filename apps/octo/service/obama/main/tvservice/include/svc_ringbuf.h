/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 svc_ringbuf.h
	@brief	  MW�� common utility ���� ���

	Description: MW�� AP�� ��� ����ϴ� DRM����  API�� ��� �����̴�.
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
	eRingBuf_Type_Linear,			// Linear Type : �ϳ��� �� Linear Buffer�� �����ϴ� Type
	eRingBuf_Type_Chunk,			// Chunk Type : �������� Chunk Size�� �д� Type
	eRingBuf_Type_VariableChunk,	// Variable Chunk Type : �������� Chunk ������ �� Chunk Size�� ���� �ٸ� �� �ִ�. Write�� �ݵ�� Variable Chunk �� ��� �� Chunk�� End�� �˷��ֵ��� �ؾ� �Ѵ�.

	eRingBuf_Type_EndOfCase
} SvcRingBuf_Type_e;

typedef enum
{
	eRingBuf_Read_Remove,			// ���� ������ ��ٷ� ����� ����.
	eRingBuf_Read_Remain,			// ������ ����� ����� ������ �ʴ� �̻� ���� ���۸� ������ �ʴ´�.

	eRingBuf_Read_EndOfCase
} SvcRingBuf_ReadRule_e;


typedef struct
{
	SvcRingBuf_Type_e		eBufType;

	HUINT32					 ulMaxSize;
	HUINT32					 ulChunkSize;

	HBOOL					 bBlockOverflow;				// Overflow�� ���, ���� ���� ������ �ʰ� ���� ���·� ���δ°� �� ���� Flag
	SvcRingBuf_ReadRule_e	 eRuleAfterRead;				// Read�� Data�� ��� ó���� ���ΰ��� ���� Type

	HxRingbuf_Signal		 cbSignal;
} SvcRingBuf_Setup_t;


typedef struct
{
	HUINT64					 ullAccumulatedSize;			// �������� ��� Write �Ǿ� �� ��ü ������. �׵��� ������ �� ��� �� ����.
	HUINT64					 ullAbsoluteStart;				// ���� �����ϴ� Buffer�� Start Position. �׵��� �޾ƿ� ������� ������� ���� ��ġ��.
	HUINT64					 ullAbsoluteCurrent;			// ���� �����ϴ� Buffer�� Current Read Position. �׵��� �޾ƿ� ������� ������� ���� ��ġ��.

	HUINT32					 ulBufferSize;					// ���� Buffering �Ǿ� �ִ� ������
	HUINT32					 ulCurrentPnt;					// Buffering�� Size�� ���Ͽ��� Current Read Position�� ��ġ.
	HUINT32					 ulUnreadSize;					// Buffering�� �� �� ���� ���� ���� ������

	// Chunk ���� ����
	HUINT32 				 ulChunkNum;
	HUINT32 				 ulStartChunkId, ulEndChunkId;

	HBOOL					 bOverflowed;					// Ring Buffer �� Overflow �Ǿ����� ��Ÿ���� Flag
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

