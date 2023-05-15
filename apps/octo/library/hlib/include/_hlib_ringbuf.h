
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
	eHxRINGBUF_TYPE_LINEAR,				// Linear Type : 하나의 긴 Linear Buffer로 간주하는 Type
	eHxRINGBUF_TYPE_CHUNK,					// Chunk Type : 여러개의 Chunk Size로 읽는 Type
	eHxRINGBUF_TYPE_VARIABLE_CHUNK,			// Variable Chunk Type : 여러개의 Chunk 이지만 각 Chunk Size는 각각 다를 수 있다. Write시 반드시 Variable Chunk 의 경우 그 Chunk의 End를 알려주도록 해야 한다.

	eHxRINGBUF_TYPE_MAX
} HxRingBuf_Type_e;

typedef enum
{
	eHxRINGBUF_READRULE_REMOVE,				// 읽은 내용은 곧바로 지우고 비운다.
	eHxRINGBUF_READRULE_REMAIN,				// 별도로 지우는 명령을 내리지 않는 이상 읽은 버퍼를 지우지 않는다.

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
	HUINT64					 ullAccumulatedSize;			// 이제까지 계속 Write 되어 온 전체 사이즈. 그동안 지워진 것 모두 다 포함.
	HUINT64					 ullAbsoluteStart;				// 현재 존재하는 Buffer의 Start Position. 그동안 받아온 사이즈로 계산했을 때의 위치임.
	HUINT64					 ullAbsoluteCurrent;			// 현재 존재하는 Buffer의 Current Read Position. 그동안 받아온 사이즈로 계산했을 때의 위치임.

	HUINT32					 ulBufferSize;					// 현재 Buffering 되어 있는 사이즈
	HUINT32					 ulCurrentPnt;					// Buffering된 Size에 비하여의 Current Read Position의 위치.
	HUINT32					 ulUnreadSize;					// Buffering된 것 중 아직 읽지 않은 사이즈

	// Chunk 관련 정보
	HUINT32					 ulChunkNum;
	HUINT32					 ulStartChunkId, ulEndChunkId;

	HBOOL					 bOverflowed;					// Ring Buffer 가 Overflow 되었음을 나타내는 Flag
} HxRingBuf_Status_t;


typedef void	(* HxRingbuf_Signal)(HxRingBuf_SignalType_e eType, Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus);

typedef struct
{
	HxRingBuf_Type_e			 eBufType;					// Ring Buffer Type
	HUINT32						 ulMaxSize;
	HUINT32						 ulChunkSize;				// Ring Buffer가 ChunkType일 경우 나누어지는 Chunk Size

	HBOOL						 bBlockOverflow;			// Overflow될 경우, 이전 것을 지우지 않고 정지 상태로 놔두는가 에 대한 Flag
	HxRingBuf_ReadRule_e		 eRuleAfterRead;			// Read한 Data를 어떻게 처리할 것인가에 대한 Type

	HxRingbuf_Signal			 cbSignal;					// Read나 Write가 일어나면 signal이 발생한다. callback 내부에서 Ringbuf 관련 함수 부르지 말것!!!
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

