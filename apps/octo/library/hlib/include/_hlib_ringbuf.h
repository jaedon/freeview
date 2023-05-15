
#ifndef	__HLIB_RINGBUF_H___
#define	__HLIB_RINGBUF_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


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
	eHxRINGBUF_TYPE_LINEAR,				// Linear Type : �ϳ��� �� Linear Buffer�� �����ϴ� Type
	eHxRINGBUF_TYPE_CHUNK,					// Chunk Type : �������� Chunk Size�� �д� Type
	eHxRINGBUF_TYPE_VARIABLE_CHUNK,			// Variable Chunk Type : �������� Chunk ������ �� Chunk Size�� ���� �ٸ� �� �ִ�. Write�� �ݵ�� Variable Chunk �� ��� �� Chunk�� End�� �˷��ֵ��� �ؾ� �Ѵ�.

	eHxRINGBUF_TYPE_MAX
} HxRingBuf_Type_e;

typedef enum
{
	eHxRINGBUF_READRULE_REMOVE,				// ���� ������ ��ٷ� ����� ����.
	eHxRINGBUF_READRULE_REMAIN,				// ������ ����� ����� ������ �ʴ� �̻� ���� ���۸� ������ �ʴ´�.

	eHxRINGBUF_READRULE_MAX
} HxRingBuf_ReadRule_e;

typedef enum
{
	eHxRINGBUF_SIG_Write,
	eHxRINGBUF_SIG_Read,
	eHxRINGBUF_SIG_EraseChunk,
	eHxRINGBUF_SIG_EraseData,
	eHxRINGBUF_SIG_ClearBuffer,
	eHxRINGBUF_SIG_FlushToChunk,

	eHxRINGBUF_SIG_End
} HxRingBuf_SignalType_e;

typedef struct
{
	HUINT64					 ullAccumulatedSize;			// �������� ��� Write �Ǿ� �� ��ü ������. �׵��� ������ �� ��� �� ����.
	HUINT64					 ullAbsoluteStart;				// ���� �����ϴ� Buffer�� Start Position. �׵��� �޾ƿ� ������� ������� ���� ��ġ��.
	HUINT64					 ullAbsoluteCurrent;			// ���� �����ϴ� Buffer�� Current Read Position. �׵��� �޾ƿ� ������� ������� ���� ��ġ��.

	HUINT32					 ulBufferSize;					// ���� Buffering �Ǿ� �ִ� ������
	HUINT32					 ulCurrentPnt;					// Buffering�� Size�� ���Ͽ��� Current Read Position�� ��ġ.
	HUINT32					 ulUnreadSize;					// Buffering�� �� �� ���� ���� ���� ������

	// Chunk ���� ����
	HUINT32					 ulChunkNum;
	HUINT32					 ulStartChunkId, ulEndChunkId;

	HBOOL					 bOverflowed;					// Ring Buffer �� Overflow �Ǿ����� ��Ÿ���� Flag
} HxRingBuf_Status_t;


typedef void	(* HxRingbuf_Signal)(HxRingBuf_SignalType_e eType, Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus);

typedef struct
{
	HxRingBuf_Type_e			 eBufType;					// Ring Buffer Type
	HUINT32						 ulMaxSize;
	HUINT32						 ulChunkSize;				// Ring Buffer�� ChunkType�� ��� ���������� Chunk Size

	HBOOL						 bBlockOverflow;			// Overflow�� ���, ���� ���� ������ �ʰ� ���� ���·� ���δ°� �� ���� Flag
	HxRingBuf_ReadRule_e		 eRuleAfterRead;			// Read�� Data�� ��� ó���� ���ΰ��� ���� Type

	HxRingbuf_Signal			 cbSignal;					// Read�� Write�� �Ͼ�� signal�� �߻��Ѵ�. callback ���ο��� Ringbuf ���� �Լ� �θ��� ����!!!
} HxRingBuf_Setup_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	HLIB_RINGBUF_Init (void);
extern Handle_t HLIB_RINGBUF_Create (HxRingBuf_Setup_t *pstSetup);
extern void		HLIB_RINGBUF_Destroy (Handle_t hRingBuf);
extern HERROR	HLIB_RINGBUF_ReadData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulReadSize);
extern HERROR	HLIB_RINGBUF_ReadChunk (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT8 *pucData, HUINT32 *pulChunkSize);
extern HERROR	HLIB_RINGBUF_GetChunkSize (Handle_t hRingBuf, HUINT32 ulChunkId, HUINT32 *pulChunkSize);
extern HERROR	HLIB_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);
extern HERROR	HLIB_RINGBUF_WriteData (Handle_t hRingBuf, HUINT32 ulDataSize, HUINT8 *pucData, HUINT32 *pulWrittenSize);

extern HERROR	HLIB_RINGBUF_FlushToChunk (Handle_t hRingBuf);
extern HERROR	HLIB_RINGBUF_EraseData (Handle_t hRingBuf, HUINT32 ulEraseData);
extern HERROR	HLIB_RINGBUF_EraseChunk (Handle_t hRingBuf, HUINT32 ulChunkId);
extern HERROR	HLIB_RINGBUF_ClearBuffer (Handle_t hRingBuf);
extern HERROR	HLIB_RINGBUF_GetBufferingStatus (Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus);
extern HERROR	HLIB_RINGBUF_ResetOverflowFlag (Handle_t hRingBuf);
extern HERROR	HLIB_RINGBUF_SetWriteBlockingLock (Handle_t hRingBuf);
extern HERROR	HLIB_RINGBUF_ResetWriteBlockingLock (Handle_t hRingBuf);

extern void 	HLIB_RINGBUF_DEBUG_Dump (Handle_t hRingBuf);


#endif /* !__HLIB_RINGBUF_H___ */

