
#include "htype.h"
#include "pal_sc.h"
#define SATSA_FALSE	0
#define SATSA_TRUE	!(SATSA_FALSE)


typedef enum
{
	SATSA_SCARD_0 = 0,
	SATSA_SCARD_1,
	SATSA_SCARD_MAX_DEVICES
}SATSA_SCARD_DEV_NUM;

typedef enum
{
	SATSA_SCARD_INSERTED,
	SATSA_SCARD_REMOVED,
	SATSA_SCARD_CARD_BUSY
}SATSA_SCARD_STATE;

typedef enum
{
	SATSA_SCARD_OK = 0,
	SATSA_SCARD_FAIL,
	SATSA_SCARD_INVALID_PARAM,
	SATSA_SCARD_INVALID_DEVICE,
	SATSA_SCARD_NOT_OPEN,
	SATSA_SCARD_ALREADY_OPEN,
	SATSA_SCARD_TIMEOUT,
	SATSA_SCARD_NOT_INITIALISED,
	SATSA_SCARD_ALREADY_INITIALISED,
	SATSA_SCARD_CNTRL_INTERFACE_FAIL,
	SATSA_SCARD_FAILED_TO_ACTIVATE_CARD,
	SATSA_SCARD_BUSY,
	SATSA_SCARD_NO_RESET,
	SATSA_SCARD_NO_OPERATION_TO_ABORT,
	SATSA_SCARD_ABORT_IN_PROGRESS,
	//SATSA_SCARD_OPERATION_STOP,    // JHK 03/09/25
	SATSA_SCARD_NO_CARD,
	SATSA_SCARD_NOT_IN_NEGOTIABLE_MODE,
       SATSA_SCARD_NOT_POWERED_UP,
    	SATSA_SCARD_IFS_COMPLETE // LeeYH 2003.11.03
}SATSA_SCARD_RESULT_CODE;

typedef enum
{
	SATSA_SCARD_EVENT_REMOVED = 0,
	SATSA_SCARD_EVENT_INSERTED,
	SATSA_SCARD_EVENT_RESET_COMPLETED,
	SATSA_SCARD_EVENT_PTS_COMPLETED,
	SATSA_SCARD_EVENT_TRANSACTION_COMPLETED,
	SATSA_SCARD_EVENT_ABORT_COMPLETED,
	SATSA_SCARD_EVENT_POWERDOWN_COMPLETED,
	SATSA_SCARD_EVENT_ERROR,
	SATSA_SCARD_EVENT_TIMEDOUT,
    	SATSA_SCARD_EVENT_CRITICAL_FAILURE,
    	SATSA_SCARD_EVENT_IFS_COMPLETED // LeeYH 2003.11.03
}SATSA_SCARD_EVENT;

#if 1
typedef enum
{
	SATSA_SCARD_NO_ERROR				= 0x00000000,
	SATSA_SCARD_HW_ERROR			= 0x00000001,
	SATSA_SCARD_PARITY_ERROR		= 0x00000002,
	SATSA_SCARD_BUFFER_OVERFLOW	= 0x00000004,
	SATSA_SCARD_FRAME_ERROR			= 0x00000008,
	SATSA_SCARD_EDC_ERROR			= 0x00000010,
	SATSA_SCARD_TIMEOUT_ERROR		= 0x00000020,
	SATSA_SCARD_OVERRUN_ERROR		= 0x00000040,
	SATSA_SCARD_ATR_ERROR			= 0x00000080,
	SATSA_SCARD_INVALID_CARD_ERROR	= 0x00000100,
	SATSA_SCARD_OTHER_ERROR			= 0x00000200,
	SATSA_SCARD_TRANSMIT_ERROR		= 0x00000400,
	SATSA_SCARD_NO_CHAR_ERROR		= 0x00000800,
	SATSA_SCARD_NO_BLOCK_ERROR		= 0x00001000
}SATSA_SCARD_ERROR;

#else
typedef enum
{
	SATSA_SCARD_NO_ERROR		= 0,
	SATSA_SCARD_HW_ERROR,
	SATSA_SCARD_PARITY_ERROR,
	SATSA_SCARD_BUFFER_OVERFLOW,
	SATSA_SCARD_FRAME_ERROR,
	SATSA_SCARD_EDC_ERROR,
	SATSA_SCARD_TIMEOUT_ERROR,
	SATSA_SCARD_OVERRUN_ERROR,
	SATSA_SCARD_ATR_ERROR,
	SATSA_SCARD_INVALID_CARD_ERROR,
	SATSA_SCARD_OTHER_ERROR,
	SATSA_SCARD_TRANSMIT_ERROR,
	SATSA_SCARD_NO_CHAR_ERROR,
	SATSA_SCARD_NO_BLOCK_ERROR
}SATSA_SCARD_ERROR;
#endif
typedef struct
{
	SATSA_SCARD_DEV_NUM 	deviceNumber;
//	PalSc_Handle_t hSci;
	HUINT8 *pucWriteBuf;
	HINT32 numWrite;
	HUINT8 *pucReadBuf;
	HUINT32 pucNumRead;
}SATSA_SCARD_TRANS_IOPB;








/*
** T=1 protocol
*/
/* T1 block structure length */
#define SATSA_SCARD_T1_PROLOGUE_LEN  3
#define SATSA_SCARD_T1_INFO_LEN      255 /* 0-254, 255 is reserved for future use */
#define SATSA_SCARD_T1_EPILOGUE_LEN  2
#define SATSA_SCARD_T1_MIN_BLOCK_LEN 4

/*
** Maximum number of bytes that can be transferred either way
** in any single transaction.
*/
#define SATSA_SCARD_T1_MAX_LEN   ((SATSA_SCARD_T1_PROLOGUE_LEN + SATSA_SCARD_T1_INFO_LEN) + SATSA_SCARD_T1_EPILOGUE_LEN)


/*
** Max number of retry attempts before failing
*/
#define SATSA_SCARD_DEFAULT_RETRY            (1)

/*
** Maximum block size is IFSC plus 4 or 5 bytes
** (depending upon the length of the epilogue field).
** Different values for IFSC & IFSD can be negotiated.
*/
#define SATSA_SCARD_T1_DEFAULT_IFSC  32  /* default max information field size for the card */
#define SATSA_SCARD_T1_DEFAULT_IFSD  32  /* default max information field size for the interface device */
#define SATSA_SCARD_T1_DEFAULT_IN_SEQ_NUM  0xFF    /* no I-blocks have been successfully received */
#define SATSA_SCARD_T1_DEFAULT_OUT_SEQ_NUM  0x0
#define SATSA_SCARD_T1_INVALID_WTX   (0) /* integer multiplier of the BWT value */

/* applies to both the T=0 & T=1 transactions */
#define SATSA_SCARD_MIN_C_APDU_LEN   (4)     /* at least 'CLA, INS, P1 & P2' bytes */
#define SATSA_SCARD_MIN_R_APDU_LEN   (2)     /* at least 'SW1 SW2' bytes */

/* Smart Card Transaction States */
typedef enum
{
	SATSA_SCARD_IDLE = 0,
	SATSA_SCARD_RESET_TRANSACTION,
	SATSA_SCARD_PTS_TRANSACTION,
	SATSA_SCARD_T0_TRANSACTION,
	SATSA_SCARD_T1_TRANSACTION
} SATSA_SCARD_TRANSACTION_STATES;

typedef enum
{
	SATSA_SCARD_LRC_EDC = 0,
	SATSA_SCARD_CRC_EDC,
	SATSA_SCARD_INVALID_EDC
} SATSA_SCARD_EDC;

/* error codes as defined in ISO 7816-3, 9.6.2.4.2 */
typedef enum
{
	SATSA_SCARD_T1_ERROR_FREE = 0,
	SATSA_SCARD_T1_ERROR_EDC,
	SATSA_SCARD_T1_ERROR_OTHER
} SATSA_SCARD_T1_ERROR;


/* T=1 PROTOCOL TRANSACTION STATES */
typedef enum
{
	SATSA_SCARD_T1_IDLE = 0,
	SATSA_SCARD_T1_SENDING_BLOCK,
	SATSA_SCARD_T1_RECEIVING_BLOCK
} SATSA_SCARD_T1_STATES;

typedef struct
{
	SATSA_SCARD_T1_STATES		state;
	HUINT16		ifsc;       /* max information field size for the ICC */
	HUINT16		ifsd;       /* max information field size for the IFD */
	HUINT8		wtx;        /*
	                        ** ICC requests additional Wait Time. Valid only
	                        ** after S(WTX request) has been received and until
	                        ** S(WTX, response) has been sent.
	                        */
	HUINT8		nodeAddr;   /* addressing is not used, set to 0 */
	HUINT8		blockOut[SATSA_SCARD_T1_MAX_LEN];
	HUINT8		blockIn[SATSA_SCARD_T1_MAX_LEN];
	HUINT16		blockOutLen;
	HUINT16		blockInLen;
	HUINT16		infFieldOutLen; /* information field length of the block sent */
	HUINT8		outSeqNum;      /* sequence number of the last I-block sent */
	HUINT8		inSeqNum;       /* sequence number of the last I-block received */

	HUINT8		*dataOutPtr;/* pointer to data within C_APDU */
	HUINT8		*dataInPtr; /* pointer to data within R_APDU */

	SATSA_SCARD_EDC	EDC;
	HUINT8		errorCnt;   /*
	                        ** 1-2: repeat operation or R(N) block, 3-5: S(RESYNCH, request)
	                        ** block, 6+: ERROR condition
	                        */
	SATSA_SCARD_T1_ERROR	errorCode;
	HBOOL		cardAbortRequested; /* ICC initiated ABORT operation */
} SATSA_SCARD_T1_CONTROL_BLOCK;

#define SATSA_SCARD_RX_BUFF_LEN                  (1024)

typedef struct
{
	HUINT8	data[ SATSA_SCARD_RX_BUFF_LEN ];
	HUINT8	*head;
	HUINT8	*tail;
	HUINT8	*top;
	HUINT16	count;
}SATSA_SCARD_RX_BUFFER;

typedef struct
{
	HBOOL	initialised;
	HBOOL	ATRdetected;
	HBOOL	ATRerror;
	HBOOL	guardClear;
	HBOOL	resettingCard;
	HBOOL	resetDone;
	HBOOL	abortRequested;
	HBOOL	poweredUp;
}SATSA_SCARD_FLAGS;

typedef struct
{
	HUINT8     *data;
	HUINT16    length;
}SATSA_SCARD_TX_BUFFER;

typedef struct
{
	HUINT8	*dataOut;
	HUINT16	lengthOut;
	HUINT8	*dataIn;
	HUINT8	*lengthIn;
	HUINT16	lengthMaxIn;
} SATSA_SCARD_TRANSACTION_INFO;

typedef struct	//temp
{
	HUINT32 dataLength;
	HUINT8  *data;
} SATSA_SCARD_EVENT_DATA;

typedef struct
{
	HUINT16						requestedRxCount;
//	SATSA_SCARD_RX_BUFFER		rxBuff;
//	SATSA_SCARD_TX_BUFFER		txBuff;
//	SATSA_SCARD_SEMAPHORE		semaphores;
	SATSA_SCARD_ERROR			error;
//	SATSA_SCARD_PARAMS		params;
	SATSA_SCARD_EDC			EDC;
	HUINT8						retry;			/* number of times to retry transaction */
	SATSA_SCARD_STATE			cardStatus;
//	 SATSA_SCARD_PROTOCOL         primaryProtocol;
//	SATSA_SCARD_PROTOCOL         secondaryProtocol;
	SATSA_SCARD_FLAGS			flags;
//	SATSA_SCARD_GUARD_PARAMS	guardParams;
	HUINT8						retryCnt;
//	HUINT8						atrBuffer[SATSA_SCARD_MAX_ATR_LEN];
//	HUINT8						atrLength;  /* length of ATR received */
	PalSc_Param_t				satsa_param_buf;

	HUINT32						numCAPDUBytesSent;
	HUINT32						numRAPDUBytesReceived;
	SATSA_SCARD_TRANSACTION_INFO		transInfo;
//	SATSA_SCARD_DEVICE_RESOURCE		systemRsrc;
	SATSA_SCARD_TRANSACTION_STATES	transactionStatus;
//	SATSA_SCARD_T0_CONTROL_BLOCK		t0CntrlBlock;
	SATSA_SCARD_T1_CONTROL_BLOCK		t1CntrlBlock;
//	SATSA_SCARD_PTS_CONTROL_BLOCK	ptsCntrlBlock;
	SATSA_SCARD_EVENT_DATA			eventData;		//temp
}SATSA_SCARD_CONTROL_BLOCK;


extern SATSA_SCARD_CONTROL_BLOCK scardControlBlock[SATSA_SCARD_MAX_DEVICES];
void MW_SATSA_SCARD_T1_Compose_S_Block(SATSA_SCARD_DEV_NUM devNum);
SATSA_SCARD_RESULT_CODE MW_SATSA_SCARD_T1_SendBlock( SATSA_SCARD_DEV_NUM devNum,HBOOL *endSequence);
SATSA_SCARD_RESULT_CODE MW_SATSA_SCARD_T1_Initialise(SATSA_SCARD_DEV_NUM devNum);
void MW_SATSA_SCARD_T1_Compose_I_Block(SATSA_SCARD_DEV_NUM devNum);
SATSA_SCARD_RESULT_CODE MW_SATSA_SCARD_T1_ReceiveBlock(  SATSA_SCARD_DEV_NUM devNum, HBOOL *endSequence);

