#include <string.h>
#include "octo_common.h"
#include "satsa_sci.h"
#include "satsa_sci_protocol_t1.h"

/****************************************************************/
/*          LOCAL TYPES                                         */
/****************************************************************/

#define ERROR_CNT_RESYNCH   3   /* an errorCnt threshold */
#define ERROR_CNT_FAILED    6   /* an errorCnt threshold */  // leeyh 2003.11.02  0xFF -> 6

/*
** ISO 7816-3 defines bit positioning as b8-b1, while C convention as b7-b0
** hence (-1).
*/
#define M_BIT			(HUINT8)(0x20)  /* More data bit M, position within the PCB byte of an I-block*/
#define NS_BIT			(HUINT8)(0x40)  /* Send Sequence number bit N(S), position within the PCB byte of an I-block */

#define NR_BIT			(HUINT8)(0x10)  /* Sequence number bit N(R), position within the PCB byte of an R-block */

#define RESPONSE_BIT	(HUINT8)(0x20)  /* Response bit, position within the PCB byte of an S-Block */

#define ID_BIT_7			(HUINT8)(0x80)  /* Block ID bit, position within the PCB byte of any block */
#define ID_BIT_6			(HUINT8)(0x40)  /* Block ID bit, position within the PCB byte of any block */

/* see ISO 7816-3, 9.6.2.4.3 */
typedef enum
{
	S_RESYNCH_REQ	= 0x00,
	S_RESYNCH_RES	= 0x20,
	S_IFS_REQ       	= 0x01,
	S_IFS_RES		= 0x21,
	S_ABORT_REQ     = 0x02,
	S_ABORT_RES	= 0x22,
	S_WTX_REQ		= 0x03,
	S_WTX_RES		= 0x23,
	S_VPP_ERR_RES	= 0x24
} S_BLOCK;

typedef enum
{
	BLOCK_ID_I = 0,
	BLOCK_ID_R,
	BLOCK_ID_S,
	BLOCK_ID_R_ERROR
} BLOCK_ID;

typedef enum
{
	PROLOGUE_NAD = 0,
	PROLOGUE_PCB,
	PROLOGUE_LEN
} PROLOGUE_FIELD_INDEX;

typedef SATSA_SCARD_RESULT_CODE (*LEVEL_1_ERROR_CALLBACK)(SATSA_SCARD_DEV_NUM devNum,
                                                             HBOOL *endSequence);

typedef enum
{
	BLOCK_DIR_IN = 0,
	BLOCK_DIR_OUT
} BLOCK_DIR;

static BLOCK_ID lastSendBlockType;

/**************************************************************************************************/
#define ________SATSA_SCI_ProtocolT1_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________SATSA_SCI_ProtocolT1_Private_Define_______________________________________________________
/**************************************************************************************************/



static SATSA_SCARD_RESULT_CODE local_ErrorCallback_ResendBlock(SATSA_SCARD_DEV_NUM devNum,
                                                            HBOOL *endSequence);
static SATSA_SCARD_RESULT_CODE local_ErrorCallback_Send_R_Block(   SATSA_SCARD_DEV_NUM devNum,
                                                                HBOOL *endSequence);
static SATSA_SCARD_RESULT_CODE local_ErrorCallback_Resend_I_Block( SATSA_SCARD_DEV_NUM devNum,
                                                                HBOOL *endSequence);
static SATSA_SCARD_RESULT_CODE local_ErrorCallback_ReceiveFailed(  SATSA_SCARD_DEV_NUM devNum,
                                                                HBOOL *endSequence);

//void SATSA_SCARD_T1_Compose_I_Block(SATSA_SCARD_DEV_NUM devNum);
static void local_Compose_R_Block(SATSA_SCARD_DEV_NUM devNum);

static void local_Compose_S_Block(SATSA_SCARD_DEV_NUM devNum,S_BLOCK type,HUINT8 infoByte);

static SATSA_SCARD_RESULT_CODE local_ErrorDetection(SATSA_SCARD_DEV_NUM devNum);

static SATSA_SCARD_RESULT_CODE local_ErrorSequence(SATSA_SCARD_DEV_NUM devNum,
                                                LEVEL_1_ERROR_CALLBACK errCallback,
                                                HBOOL *endSequence);


/*****************************************************************
*
* Function Name
*   GenerateLRC
*
* Inputs
*   lrcData            Buffer containing data
*   length          Number of bytes in the buffer
*
* Outputs
*   None
*
* Return Codes
*   LRC value
*
* Description
*   Generates Error Detection Code - LRC.
*
*****************************************************************/

static HUINT8 local_GenerateLRC(HUINT8 lrcData[], HUINT16 lrcLength)
{
	HUINT8 edc;
	HUINT16 i;

	HxLOG_Print("GenerateLRC++\r\n");

	edc = lrcData[0];
	HxLOG_Print("\t %x",edc);

	for (i = 1; i < lrcLength; ++i)
	{
		edc ^= lrcData[i];
		HxLOG_Print(" %2x ",edc);
	}
	HxLOG_Print("\r\n");
	HxLOG_Print("GenerateLRC--\r\n");

	return (edc);
}


/*****************************************************************
*
* Function Name
*   GenerateCRC
*
* Inputs
*   data            Buffer containing data
*   length          Number of bytes in the buffer
*
* Outputs
*   None
*
* Return Codes
*   CRC value
*
* Description
*   Generates Error Detection Code - CRC.
*
*****************************************************************/

static HUINT16 local_GenerateCRC(HUINT8 crcData[], HUINT16 crcLength)
{
	HUINT16 edc = 0;

	/* TO BE COMPLETED */

	return (edc);
}


/*****************************************************************
*
* Function Name
*   GetBlockID
*
* Inputs
*   pcbByte         Protocol control byte
*
* Outputs
*   None
*
* Return Codes
*   block type
*
* Description
*   Gets block ID, see ISO 7816-3 9.6.2.4.
*
*****************************************************************/

static BLOCK_ID local_GetBlockID(HUINT8 pcbByte)
{
	if ((pcbByte & ID_BIT_7) == 0)
	{
		return (BLOCK_ID_I);
	}
	else if ((pcbByte & ID_BIT_6) == 0)
	{
		return (BLOCK_ID_R);
	}
	else
	{
		return (BLOCK_ID_S);
	}
}


/*****************************************************************
*
* Function Name
*   SetEpilogueField
*
* Inputs
*   devNum          Device number
*
* Outputs
*   None
*
* Return Codes
*   None
*
* Description
*   Sets up EPILOGUE field & blockOutLen.
*
*****************************************************************/

static void local_SetEpilogueField(SATSA_SCARD_DEV_NUM devNum)
{
	HUINT16 crc;
	HUINT8 lrc;

	HUINT16 i;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("SetEpilogueField++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	i = (HUINT16)(SATSA_SCARD_T1_PROLOGUE_LEN + t1Block->infFieldOutLen);

	if (scb->EDC == SATSA_SCARD_LRC_EDC)
	{
		lrc = local_GenerateLRC(t1Block->blockOut, i);
		t1Block->blockOut[i] = lrc;

		t1Block->blockOutLen = (HUINT16)(i + 1);
	}
	else
	{
		/*
		** The order of the LOW/HIGH bytes is not known at the moment.
		** Assumed a[i] = LOW, a[i+1] = HIGH.
		*/
		crc = local_GenerateCRC(t1Block->blockOut, i);
		t1Block->blockOut[i] = (HUINT8)crc;
		t1Block->blockOut[i+1] = (HUINT8)(crc >> 8);

		t1Block->blockOutLen = (HUINT16)(i + 2);
	}

	{
		int i;

		for(i=0;i<t1Block->blockOutLen;i++)
			HxLOG_Print(" %d = [%x] \r\n",i,t1Block->blockOut[i]);
	}

	HxLOG_Print("SetEpilogueField--\r\n");
}

/*****************************************************************
*
* Function Name
*   SATSA_SCARD_RESULT_CODE Process_I_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag.
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Processes an I-Block.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_Process_I_Block(  SATSA_SCARD_DEV_NUM devNum,
                                                    HBOOL *endSequence)
{
	HUINT8 pcbByte;
	HUINT8 lenByte;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("Process_I_Block++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	pcbByte = t1Block->blockIn[PROLOGUE_PCB];
	lenByte = t1Block->blockIn[PROLOGUE_LEN];

	HxLOG_Print(" lenByte  = %d \r\n",lenByte);
	HxLOG_Print(" ifscValue  = %d \r\n",t1Block->ifsc);

	if( lastSendBlockType == BLOCK_ID_R_ERROR )
	{
		scb->error |= SATSA_SCARD_OTHER_ERROR;
		t1Block->errorCode = SATSA_SCARD_T1_ERROR_OTHER;
		t1Block->errorCnt  = 2;

		HxLOG_Print("SATSA SCARD T1: Last Block is Error Sequence\n\r");

		return (local_ErrorSequence(devNum,local_ErrorCallback_ReceiveFailed,endSequence));
	}

	if (lenByte > t1Block->ifsc)
	{
		/*
		** ERROR - INF field contains more bytes than set by the IFD
		*/
		scb->error |= SATSA_SCARD_OTHER_ERROR;
		t1Block->errorCode = SATSA_SCARD_T1_ERROR_OTHER;

		HxLOG_Print("SATSA SCARD T1: INF field greater than IFD\n\r");

		return (local_ErrorSequence(devNum,local_ErrorCallback_ReceiveFailed,endSequence));
	}

	if ((scb->numRAPDUBytesReceived + lenByte) > scb->transInfo.lengthMaxIn)
	{
		/*
		** FATAL ERROR - Receive application buffer overflow
		*/
		HxLOG_Print("SATSA SCARD T1: Receive overflow error!\r\n");

		scb->error |= SATSA_SCARD_OVERRUN_ERROR;

		*endSequence = SATSA_TRUE;
		return (SATSA_SCARD_FAIL);
	}

	t1Block->errorCode = SATSA_SCARD_T1_ERROR_FREE;
	t1Block->errorCnt = 0;
///////////////////////////////////////////////////////////////////////////////
	/* data in okay - extract the INF field to the dataIn buffer */
	(void) SATSA_memcpy(t1Block->dataInPtr,&t1Block->blockIn[PROLOGUE_LEN+1],lenByte);
///////////////////////////////////////////////////////////////////////////////
	t1Block->dataInPtr += lenByte;
	scb->numRAPDUBytesReceived += lenByte;
	t1Block->inSeqNum = (HUINT8)((pcbByte & (NS_BIT)) ? 1:0);

	if ((local_GetBlockID(t1Block->blockOut[PROLOGUE_PCB]) == BLOCK_ID_I) &&
	(scb->numCAPDUBytesSent < scb->transInfo.lengthOut))
	{
		/*
		** The last block sent successfully, was an I-block.
		** update the data out fields as that block is now confirmed
		*/
		t1Block->dataOutPtr += t1Block->infFieldOutLen;
		scb->numCAPDUBytesSent += t1Block->infFieldOutLen;
	}

	/*
	** TempCode
	** TO BE TESTED with the appropriate ICC
	** See ISO 7816-3 9.6.2.2.2
	*/

	if (lenByte == 0)
	{
		/*
		** Received an I-block with empty information field
		** Advance N(S) and send the same block back as ACK.
		*/
		(void) SATSA_memcpy(  t1Block->blockOut,
								t1Block->blockIn,
								t1Block->blockInLen);

		if (t1Block->outSeqNum == 0)
		{
			t1Block->outSeqNum = 1;
			t1Block->blockOut[PROLOGUE_PCB] |= NS_BIT;
		}
		else
		{
			t1Block->outSeqNum = 0;
			t1Block->blockOut[PROLOGUE_PCB] &= ((HUINT8)(~NS_BIT));
		}

		t1Block->infFieldOutLen = 0;
		t1Block->blockOutLen = t1Block->blockInLen;

		t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
		*endSequence = SATSA_FALSE;

		return (SATSA_SCARD_OK);
	}

	if ((pcbByte & M_BIT) == 0)
	{
		/*
		** transaction COMPLETED
		*/
		*endSequence = SATSA_TRUE;
		return (SATSA_SCARD_OK);
	}

	/*
	** there is more data to follow from the ICC
	*/
	local_Compose_R_Block(devNum);

	/*
	** switch to transmit mode
	*/
	t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
	*endSequence = SATSA_FALSE;

	HxLOG_Print("Process_I_Block--\r\n");

	return (SATSA_SCARD_OK);
}


/*****************************************************************
*
* Function Name
*   SATSA_SCARD_RESULT_CODE Process_R_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag.
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Processes an R-Block.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_Process_R_Block(  SATSA_SCARD_DEV_NUM devNum,
                                                    HBOOL *endSequence)
{
	HUINT8 inSequenceNum;
	HUINT8 outSequenceNum;
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;
	int								errCode;

	HxLOG_Print("Process_R_Block++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;


	if ((local_GetBlockID(t1Block->blockOut[PROLOGUE_PCB]) == BLOCK_ID_S) &&
				((t1Block->blockOut[PROLOGUE_PCB] & RESPONSE_BIT) == 0))
	{
		/*
		** ERROR, the last block sent was an S(REQUEST)
		** We should not get an R-block in response to an S(REQUEST) block.
		** See ISO 7816-3, 9.6.2.3.2, Rule 7.3
		*/
		scb->error |= SATSA_SCARD_OTHER_ERROR;

		return (local_ErrorSequence( devNum,local_ErrorCallback_ResendBlock,endSequence));
	}

	inSequenceNum = (HUINT8)((t1Block->blockIn[PROLOGUE_PCB] & NR_BIT)?1:0);
	outSequenceNum = (HUINT8)((t1Block->outSeqNum)?0:1);
	// outSequenceNum = (HUINT8)((t1Block->outSeqNum)?1:0);	// JHK 03/08/22 catch the Bug!

	HxLOG_Print("[NEC] R Block, inSequenceNum:%d, outSequenceNum:%d, numCAPDU:%d, lengthOut:%d\r\n",
				inSequenceNum, outSequenceNum, scb->numCAPDUBytesSent, scb->transInfo.lengthOut));

	if (inSequenceNum != outSequenceNum)
	{
		/*
		** OK - N(R) received is different from the N(S) sent.
		*/
		t1Block->errorCode = SATSA_SCARD_T1_ERROR_FREE;
		//t1Block->errorCnt = 0; // LeeYH 2003.11.04

		if ((local_GetBlockID(t1Block->blockOut[PROLOGUE_PCB]) == BLOCK_ID_I) &&
						(scb->numCAPDUBytesSent < scb->transInfo.lengthOut))
		{
			/*
			** The last block sent successfully, was an I-block.
			*/
			t1Block->dataOutPtr += t1Block->infFieldOutLen;
			scb->numCAPDUBytesSent += t1Block->infFieldOutLen;
		}

		if (scb->numCAPDUBytesSent < scb->transInfo.lengthOut && t1Block->errorCnt == 0) //LeeYH, 2003.11.04
		{
			/*
			** there is more data to send to the ICC
			*/
			if (t1Block->outSeqNum == 0)
			{
				t1Block->outSeqNum = 1;
			}
			else
			{
				t1Block->outSeqNum = 0;
			}

			t1Block->errorCnt = 0; // LeeYH 2003.11.04
			MW_SATSA_SCARD_T1_Compose_I_Block(devNum);

			t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
			*endSequence = SATSA_FALSE;

			return (SATSA_SCARD_OK);
		}

		/*
		** ERROR, there is no more data to send to the ICC and the ICC
		** has requested additional data.
		*/
		scb->error |= SATSA_SCARD_OVERRUN_ERROR;

		/* force R(0) sequence number */
		t1Block->inSeqNum = SATSA_SCARD_T1_DEFAULT_IN_SEQ_NUM;
		t1Block->errorCode = SATSA_SCARD_T1_ERROR_OTHER;

		return (local_ErrorSequence(devNum,local_ErrorCallback_Send_R_Block,endSequence));
	}

	/*
	** ERROR, N(R) == N(S)
	*/
	scb->error |= SATSA_SCARD_OTHER_ERROR;

	t1Block->outSeqNum = outSequenceNum; // LeeYH 2003.11.04
	errCode = (int)local_ErrorSequence(devNum,local_ErrorCallback_Resend_I_Block,endSequence);

	HxLOG_Print("Process_R_Block--\r\n");

	return (errCode);
}


/*****************************************************************
*
* Function Name
*   SATSA_SCARD_RESULT_CODE Process_S_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag.
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Processes an S-Block.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_Process_S_Block(  SATSA_SCARD_DEV_NUM devNum,
                                                    HBOOL *endSequence)
{
	HUINT8         pcbByte;
	HUINT8         sCode;
	S_BLOCK     sBlock;
	SATSA_SCARD_RESULT_CODE  result = SATSA_SCARD_OK;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("Process_S_Block++\r\n");

	lastSendBlockType = BLOCK_ID_S;

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	pcbByte = t1Block->blockIn[PROLOGUE_PCB];

	t1Block->errorCode = SATSA_SCARD_T1_ERROR_FREE;
	t1Block->errorCnt = 0;

	sCode = (HUINT8)(pcbByte & ((HUINT8)0x3F));   /* S-block type */

	switch (sCode)
	{
		case 0x00:
			sBlock = S_RESYNCH_REQ;
			break;
		case 0x20:
			sBlock = S_RESYNCH_RES;
			break;
		case 0x01:
			sBlock = S_IFS_REQ;
			break;
		case 0x02:
			sBlock = S_ABORT_REQ;
			break;
		case 0x21:
			sBlock = S_IFS_RES;
			break;
		case 0x22:
			sBlock = S_ABORT_RES;
			break;
		case 0x03:
			sBlock = S_WTX_REQ;
			break;
		case 0x24:
			sBlock = S_VPP_ERR_RES;
			break;
		default:
			return SATSA_SCARD_FAIL;
	}


	if ((pcbByte & RESPONSE_BIT) == 0)
	{
		/*
		** S(REQUEST) block has been received
		*/
		if ((local_GetBlockID(t1Block->blockOut[PROLOGUE_PCB]) == BLOCK_ID_I) &&
						(scb->numCAPDUBytesSent < scb->transInfo.lengthOut))
		{
			/*
			** The last block sent successfully, was an I-block.
			*/
			t1Block->dataOutPtr += t1Block->infFieldOutLen;
			scb->numCAPDUBytesSent += t1Block->infFieldOutLen;
		}

		switch (sBlock)
		{
			case S_RESYNCH_REQ:
				/*
				** Invalid case
				*/
				scb->error |= SATSA_SCARD_OTHER_ERROR;
				*endSequence = SATSA_TRUE;
				result = SATSA_SCARD_FAIL;
				break;

			case S_IFS_REQ:
				t1Block->ifsc = t1Block->blockIn[PROLOGUE_LEN+1];

				local_Compose_S_Block(devNum, S_IFS_RES, (HUINT8)t1Block->ifsc);

				t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
				*endSequence = SATSA_FALSE;
				break;

			case S_ABORT_REQ:
				do
				{
					scb->flags.abortRequested = SATSA_TRUE;
				} while (scb->flags.abortRequested != SATSA_TRUE);

				t1Block->cardAbortRequested = SATSA_TRUE;

				t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
				*endSequence = SATSA_FALSE;
				break;


			case S_WTX_REQ:
				local_Compose_S_Block(devNum, S_WTX_RES, t1Block->blockIn[PROLOGUE_LEN+1]);

				t1Block->wtx = t1Block->blockIn[PROLOGUE_LEN+1];

				t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
				*endSequence = SATSA_FALSE;
				break;

			default:
				result = SATSA_SCARD_FAIL;

		}
	}
	else
	{
		/*
		** S(RESPONSE) block has been received
		*/
		switch (sBlock)
		{
			case S_RESYNCH_RES:
				if ((++scb->retryCnt) <= scb->retry)
				{
					/*
					** Resynchronised, restart transaction
					*/
#if 0 //????????????????????????????????????????????????????????????????????????????????????????????
					if (SATSA_SCARD_FlushReadBuffer(devNum) != SATSA_SCARD_OK)
					{
						scb->error |= SATSA_SCARD_OTHER_ERROR;

						*endSequence = SATSA_TRUE;
						return (SATSA_SCARD_FAIL);
					}
#endif
					t1Block->dataOutPtr = scb->transInfo.dataOut;
					t1Block->outSeqNum = 0;
					t1Block->dataInPtr = scb->transInfo.dataIn;
					t1Block->inSeqNum = SATSA_SCARD_T1_DEFAULT_IN_SEQ_NUM;

					t1Block->cardAbortRequested = SATSA_FALSE;

					scb->numCAPDUBytesSent = 0;
					scb->numRAPDUBytesReceived = 0;

					MW_SATSA_SCARD_T1_Compose_I_Block(devNum);

					t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
					*endSequence = SATSA_FALSE;

				}
				else
				{
					/*
					** Transaction failed after 'retry' number of attempts.
					*/
					*endSequence = SATSA_TRUE;
					scb->error |= SATSA_SCARD_OTHER_ERROR;
					result = SATSA_SCARD_FAIL;
				}
				break;

			case S_IFS_RES:
				/*
				** IFD needs to send S(IFS request) block in order to get
				** the response.  In order to this, application needs to
				** pass block id as a parameter (currently not implemented).
				*/
				t1Block->ifsd = t1Block->blockIn[PROLOGUE_LEN+1];

				*endSequence = SATSA_TRUE;
				result = SATSA_SCARD_IFS_COMPLETE;
				break;

			case S_ABORT_RES:
				*endSequence = SATSA_TRUE;
				break;


			case S_WTX_RES:
			case S_VPP_ERR_RES:
				/*
				** Invalid case
				*/
				scb->error |= SATSA_SCARD_OTHER_ERROR;
				*endSequence = SATSA_TRUE;
				result = SATSA_SCARD_FAIL;
				break;

			default:
				result = SATSA_SCARD_FAIL;

		}
	}
	HxLOG_Print("Process_S_Block, endSequence:(%d) sCode:(0x%02X) pcbByte:(0x%02X) sBlock:(0x%02X)\r\n", *endSequence, sCode, pcbByte, sBlock);

	HxLOG_Print("Process_S_Block--\r\n");

	return result;
}


/*****************************************************************
*
* Function Name
*   MMAC_SCARD_T1_Initialise
*
* Inputs
*   devNum          Device number
*
* Outputs
*   None
*
* Return Codes
*   MMAC_SCARD_OK
*   MMAC_SCARD_FAIL
*
* Description
*   Sets up internal data transfer arguments.
*
*****************************************************************/

SATSA_SCARD_RESULT_CODE MW_SATSA_SCARD_T1_Initialise(SATSA_SCARD_DEV_NUM devNum)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("SATSA_SCARD_T1_Initialise++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	scb->numCAPDUBytesSent = 0;
	scb->numRAPDUBytesReceived = 0;
	t1Block->dataOutPtr = scb->transInfo.dataOut;
	t1Block->dataInPtr = scb->transInfo.dataIn;
	t1Block->errorCnt = 0;
	t1Block->errorCode = SATSA_SCARD_T1_ERROR_FREE;

//	scb->t1CntrlBlock.ifsc      = SATSA_SCARD_T1_DEFAULT_IFSC;
	scb->t1CntrlBlock.ifsd      = SATSA_SCARD_T1_DEFAULT_IFSD;
	scb->t1CntrlBlock.outSeqNum	= SATSA_SCARD_T1_DEFAULT_OUT_SEQ_NUM;
	scb->t1CntrlBlock.inSeqNum		= SATSA_SCARD_T1_DEFAULT_IN_SEQ_NUM;
	scb->retry		= SATSA_SCARD_DEFAULT_RETRY;
	scb->EDC		= SATSA_SCARD_LRC_EDC;

	/*
	** Node addressing is not used, hence set to 0.
	** If used, needs to be passed as a parameter by the application.
	** See ISO 7816-3, 9.4.1.1
	*/
	t1Block->nodeAddr = 0x00;

	/*
	** This flag is set if the ICC requests ABORT operation.
	*/
	t1Block->cardAbortRequested = SATSA_FALSE;
//	memset(&(scb->satsa_param_buf),0,sizeof(PalSc_Param_t));

	scb->retryCnt = 0;
	t1Block->wtx = SATSA_SCARD_T1_INVALID_WTX;


	HxLOG_Print("SATSA_SCARD_T1_Initialise--\r\n");

	return (SATSA_SCARD_OK);
}


/*****************************************************************
*
* Function Name
*   SATSA_SCARD_T1_Compose_I_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   None
*
* Return Codes
*   None
*
* Description
*   Composes an I-block to send.
*
*****************************************************************/

void MW_SATSA_SCARD_T1_Compose_I_Block(SATSA_SCARD_DEV_NUM devNum)
{
	HUINT16 infoLength;
	HBOOL moreData;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("SATSA_SCARD_T1_Compose_I_Block++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	lastSendBlockType = BLOCK_ID_I;

	/*
	** set up information field
	*/
	moreData = SATSA_FALSE;
	/* large blocks of data will be split into ifsc size info blocks  */
	infoLength = (HUINT16)(scb->transInfo.lengthOut - scb->numCAPDUBytesSent);

	if (infoLength > t1Block->ifsc)
	{
		infoLength = t1Block->ifsc;
		moreData = SATSA_TRUE; /* there is more data to send following this block */
	}

	(void) SATSA_memcpy(&t1Block->blockOut[PROLOGUE_LEN+1],t1Block->dataOutPtr,infoLength);
	/*
	** set up prologue field
	*/
	t1Block->blockOut[PROLOGUE_NAD] = t1Block->nodeAddr;
	t1Block->blockOut[PROLOGUE_PCB] = 0x00;

	if (moreData == SATSA_TRUE)
	{
		/* set M_BIT */
		t1Block->blockOut[PROLOGUE_PCB] |= M_BIT;
	}

	if (t1Block->outSeqNum == 1)
	{
		/* set NS_BIT */
		t1Block->blockOut[PROLOGUE_PCB] |= NS_BIT;
		t1Block->outSeqNum = 0;
	}
	else
	{
		/* increment outSeqNum */
		t1Block->outSeqNum = 1;
	}

	t1Block->blockOut[PROLOGUE_LEN] = (HUINT8)infoLength;

	t1Block->infFieldOutLen = infoLength;

	/*
	** set up epilogue field
	*/
	local_SetEpilogueField(devNum);

	{
		int i;

		for(i=0;i<3;i++)
			HxLOG_Print(" %d = [%2x] \r\n",i,t1Block->blockOut[i]);
	}

	HxLOG_Print("SATSA_SCARD_T1_Compose_I_Block--\r\n");

}

void MW_SATSA_SCARD_T1_Compose_S_Block(SATSA_SCARD_DEV_NUM devNum)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	//HUINT8 ifscValue;
	HUINT16 ifscValue = 264;

	lastSendBlockType = BLOCK_ID_S;

	//SCI_GetParamIFSC(devNum, &(ifscValue));

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	t1Block->ifsc = (HUINT16)ifscValue;

	HxLOG_Print(" ifscValue  = %d \r\n",ifscValue);
	HxLOG_Print(" ifscValue  = %d \r\n",t1Block->ifsc);

	local_Compose_S_Block( devNum, S_IFS_REQ, (HUINT8)ifscValue );
}

/*****************************************************************
*
* Function Name
*   SATSA_SCARD_T1_SendBlock
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag.
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Sends a block.
*
*****************************************************************/

SATSA_SCARD_RESULT_CODE MW_SATSA_SCARD_T1_SendBlock( SATSA_SCARD_DEV_NUM devNum,HBOOL *endSequence)
{
//	SATSA_SCARD_RESULT_CODE result;
//	SATSA_SCARD_WRITE_IOPB writeIOPB;
	BLOCK_ID block;
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;
//	int	i;

	HxLOG_Print("SATSA_SCARD_T1_SendBlock++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	block = local_GetBlockID(t1Block->blockOut[PROLOGUE_PCB]);
	if ((block != BLOCK_ID_S) && (scb->flags.abortRequested == SATSA_TRUE))
	{
		if (t1Block->cardAbortRequested == SATSA_TRUE)
		{
			/* Overwrite any block pending & compose an ABORT RESPONSE block */
			local_Compose_S_Block(devNum, S_ABORT_RES, 0);
		}
		else
		{
			/* Overwrite any block pending & compose an ABORT REQUEST block */
			local_Compose_S_Block(devNum, S_ABORT_REQ, 0);
		}
	}

	/*
	** Send a block
	*/
	*endSequence = SATSA_FALSE;

	HxLOG_Print("SATSA_SCARD_T1_SendBlock--\r\n");

	return (SATSA_SCARD_OK);
}


/*****************************************************************
*
* Function Name
*   SATSA_SCARD_T1_ReceiveBlock
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag.
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Receives a block.
*
*****************************************************************/

SATSA_SCARD_RESULT_CODE MW_SATSA_SCARD_T1_ReceiveBlock(  SATSA_SCARD_DEV_NUM devNum,
                                                            HBOOL *endSequence)
{
	SATSA_SCARD_RESULT_CODE  result;
//	SATSA_SCARD_READ_IOPB    readIOPB;
//	HUINT16                    epiLen;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;
	int								errCode;
//	int								i;
//	static int nCount=1;

	HxLOG_Print("SATSA_SCARD_T1_ReceiveBlock++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;
#if 0
	/* set number of bytes expected */
	if(scb->transInfo.lengthIn > t1Block->ifsd)
	{
		/* data in for this transaction is greater than ifsd so must be broken into smaller blocks */
		t1Block->blockInLen = (HUINT16)(scb->transInfo.lengthIn - scb->numRAPDUBytesReceived);
		if(t1Block->blockInLen > t1Block->ifsd)
		{
			t1Block->blockInLen = t1Block->ifsd;
		}
	}
	else
	{
		/* in some cases the ICC may split the RAPDU into separate data and status blocks even
		if the total is less than IFSD */
		t1Block->blockInLen = (scb->transInfo.lengthIn - scb->numRAPDUBytesReceived);
	}
	epiLen = (HUINT16)((scb->params.EDC == SATSA_SCARD_LRC_EDC)?1:2);
	t1Block->blockInLen += (HUINT16)(SATSA_SCARD_T1_PROLOGUE_LEN + epiLen);
#endif
	/*
	** Receive a block
	*/
#if 0
	readIOPB.deviceNumber = devNum;
	readIOPB.data = t1Block->blockIn;
	readIOPB.length = t1Block->blockInLen;
	result = SATSA_SCARD_Read(&readIOPB);
	if((result != SATSA_SCARD_OK) && (result != SATSA_SCARD_TIMEOUT))
	{
		HxLOG_Info("SATSA_SCARD_Read>Fails errCode:(%x)\r\n", result);
	}
	else
	{
		result = SATSA_SCARD_OK;
	}
	t1Block->blockInLen = readIOPB.length;

	NEC_Print("[NEC] @@@ SATSA_SCARD_T1_ReceiveBlock, Length: %d ",readIOPB.length);
	for(i=0;i<readIOPB.length; i++)
	{
		if( i%16 == 0 ) NEC_Print("\r\n");
		NEC_Print(" %02X", readIOPB.data[i]);
	}
	NEC_Print("\r\n");

	if (result != SATSA_SCARD_OK)
	{
#if (SATSA_SCARD_DEBUG_SERIAL == SATSA_TRUE)
		if(((UI32)scb->error) & ((UI32)SATSA_SCARD_NO_CHAR_ERROR))
		{
			DSI_printf(DSI_PORT_1,"SATSA SCARD T1: Character Waiting Timout \r\n");
		}
		if(((UI32)scb->error) & ((UI32)SATSA_SCARD_NO_BLOCK_ERROR))
		{
			DSI_printf(DSI_PORT_1,"SATSA SCARD T1: Block Waiting Timout \r\n");
		}
#endif
		scb->error |= SATSA_SCARD_OTHER_ERROR;
		t1Block->errorCode = SATSA_SCARD_T1_ERROR_OTHER;
		DSI_printf(1,"SATSA_SCARD_Read>Fails errCode:(0x%X)\r\n", result);
	}

	if( NEC_GetDebugMode() == 2 )
		PrintT1Info();
#endif

	HxLOG_Print("[SATSA_SCARD_T1_ReceiveBlock] Read Length: %d ",t1Block->blockInLen);

	if(t1Block->blockInLen >= SATSA_SCARD_T1_MIN_BLOCK_LEN)
	{
		/*
		**  read has been successful
		**  check data returned
		*/
		result = local_ErrorDetection(devNum);

		if (result == SATSA_SCARD_OK)
		{
			switch (local_GetBlockID(t1Block->blockIn[PROLOGUE_PCB]))
			{
				case BLOCK_ID_I:
					return (local_Process_I_Block(devNum, endSequence));

				case BLOCK_ID_R:
					return (local_Process_R_Block(devNum, endSequence));

				case BLOCK_ID_S:
					return (local_Process_S_Block(devNum, endSequence));

				default:
					return SATSA_SCARD_FAIL;
			}
		}
		else
		{
			scb->error |= SATSA_SCARD_EDC_ERROR;
			t1Block->errorCode = SATSA_SCARD_T1_ERROR_EDC;
			HxLOG_Info("SATSA_SCARD_T1_ReceiveBlock>ErrorDetection Fails errCode:(%x)\r\n", result);
		}
	}

	errCode = local_ErrorSequence(devNum, local_ErrorCallback_ReceiveFailed, endSequence);
	if(errCode != 0)
	{
		HxLOG_Info("MMAX_SCARD_T1_REC> Fails errCode:(%x)\r\n", errCode);
	}

	HxLOG_Print("SATSA_SCARD_T1_ReceiveBlock--\r\n");

	return errCode;
}


/*****************************************************************
*
* Function Name
*   Compose_R_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   None
*
* Return Codes
*   None
*
* Description
*   Composes an R-block.
*
*****************************************************************/

static void local_Compose_R_Block(SATSA_SCARD_DEV_NUM devNum)
{
	HUINT8 inSequenceNum;
	HUINT8	i;
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;


	HxLOG_Print("Compose_R_Block++\r\n");

	lastSendBlockType = BLOCK_ID_R;

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	if (t1Block->inSeqNum == 0)
	{
		inSequenceNum = 1;
	}
	else
	{
		inSequenceNum = 0;
	}

	/*
	** set up prologue field
	*/
	t1Block->blockOut[PROLOGUE_NAD] = t1Block->nodeAddr;
	t1Block->blockOut[PROLOGUE_PCB] = 0x80;

	if (inSequenceNum == 1)
	{
		t1Block->blockOut[PROLOGUE_PCB] |= NR_BIT;
	}
	//t1Block->blockOut[PROLOGUE_PCB] |= (UI8)t1Block->errorCode; // LeeYH, 2003.11.04 for DM_CA
	t1Block->blockOut[PROLOGUE_LEN] = 0;

	t1Block->infFieldOutLen = 0;

	/*
	** set up epilogue field
	*/
	local_SetEpilogueField(devNum);


	HxLOG_Print("[Compose_R_Block]  %d\r\n", t1Block->blockOutLen);

	for( i=0; i<t1Block->blockOutLen; i++ ) {
		HxLOG_Print("\r\n");
		HxLOG_Print(" %02X", t1Block->blockOut[i]);
	}
	HxLOG_Print("\r\n");


	HxLOG_Print("Compose_R_Block--\r\n");
}


/*****************************************************************
*
* Function Name
*   Compose_S_Block
*
* Inputs
*   devNum          Device number
*   type            S-block type
*   infoByte        Content of the only info byte
*
* Outputs
*   None
*
* Return Codes
*   None
*
* Description
*   Composes an S-block.
*
*****************************************************************/

static void local_Compose_S_Block(SATSA_SCARD_DEV_NUM devNum, S_BLOCK type, HUINT8 infoByte)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;
//	HUINT8 i;

	HxLOG_Print("Compose_S_Block++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	lastSendBlockType = BLOCK_ID_S;

	/*
	** set up prologue field
	*/
	t1Block->blockOut[PROLOGUE_NAD] = t1Block->nodeAddr;
	t1Block->blockOut[PROLOGUE_PCB] = 0xC0;
	t1Block->blockOut[PROLOGUE_PCB] |= (HUINT8)type;

	switch (type)
	{
		case S_RESYNCH_REQ:
		case S_RESYNCH_RES:
		case S_ABORT_REQ:
		case S_ABORT_RES:
		case S_VPP_ERR_RES:
			/* there is no INF field - ignore infoByte */
			t1Block->blockOut[PROLOGUE_LEN] = 0;
			t1Block->infFieldOutLen = 0;
			break;

		case S_IFS_REQ:
		case S_IFS_RES:
		case S_WTX_REQ:
		case S_WTX_RES:
			t1Block->blockOut[PROLOGUE_LEN+1] = infoByte;
			t1Block->blockOut[PROLOGUE_LEN] = 1;
			t1Block->infFieldOutLen = 1;
			break;

		default:
			break;
	}

	/*
	** set up epilogue field
	*/
	local_SetEpilogueField(devNum);

	HxLOG_Print("Compose_S_Block--\r\n");

}


/*****************************************************************
*
* Function Name
*   ErrorDetection
*
* Inputs
*   devNum          Device number
*
* Outputs
*   None
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Performs error detection.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_ErrorDetection(SATSA_SCARD_DEV_NUM devNum)
{
	HUINT16 	crc;
	HUINT8 		lrc;
	HUINT8		generatedLRC;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("ErrorDetection++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	if (scb->EDC == SATSA_SCARD_LRC_EDC)
	{
		lrc = t1Block->blockIn[t1Block->blockInLen-1];
		generatedLRC = local_GenerateLRC(t1Block->blockIn, (HUINT16)(t1Block->blockInLen-1));
		//DSI_printf(1,"lrc:0x%x generatedLRC:0x%x EDC:0x%x InLen:0x%x\n\r", lrc, generatedLRC, scb->params.EDC, t1Block->blockInLen);

		if (lrc == generatedLRC)
		{
			return (SATSA_SCARD_OK);
		}
	}
	else
	{
		/*
		** The order of the LOW/HIGH bytes is not known at the moment.
		** Assumed a[i] = LOW, a[i+1] = HIGH.
		*/
		crc = (HUINT16)(((HUINT16)t1Block->blockIn[t1Block->blockInLen-1]) << ((HUINT16)8));
		crc += (HUINT16)t1Block->blockIn[t1Block->blockInLen-2];

		if (crc == local_GenerateCRC(t1Block->blockIn, (HUINT16)(t1Block->blockInLen-2)))
		{
			return (SATSA_SCARD_OK);
		}
	}

	HxLOG_Print("ErrorDetection--\r\n");

	return (SATSA_SCARD_FAIL);
}


/*****************************************************************
*
* Function Name
*   ErrorSequence
*
* Inputs
*   devNum          Device number
*   callback        Function to be executed for LEVEL 1 error procedure
*
* Outputs
*   endSequence     End of transaction flag
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Performs error correction sequence.
*   'callback' calls a level 1 error sequence routine which
*   exact functionality depends on circumstances.
*
*   Always return from the calling function after executing ErrorSequence().
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_ErrorSequence(SATSA_SCARD_DEV_NUM devNum,
                                                LEVEL_1_ERROR_CALLBACK errCallback,
                                                HBOOL *endSequence)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("ErrorSequence++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	t1Block->errorCnt++;

	if (t1Block->errorCnt < ERROR_CNT_RESYNCH)
	{
		return ((*errCallback)(devNum, endSequence));
	}
	else if (t1Block->errorCnt < ERROR_CNT_FAILED)
	{
		local_Compose_S_Block(devNum, S_RESYNCH_REQ, 0);
		t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
		*endSequence = SATSA_FALSE;
		return (SATSA_SCARD_OK);
	}
	else
	{
		*endSequence = SATSA_TRUE;
		return (SATSA_SCARD_FAIL);
	}

	HxLOG_Print("ErrorSequence--\r\n");
}


/*****************************************************************
*
* Function Name
*   ErrorCallback_ResendBlock
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Level 1 error sequence callback
*   Resends the last block sent.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_ErrorCallback_ResendBlock(SATSA_SCARD_DEV_NUM devNum,
                                                            HBOOL *endSequence)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("ErrorCallback_ResendBlock++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
	*endSequence = SATSA_FALSE;

	HxLOG_Print("ErrorCallback_ResendBlock--\r\n");

	return (SATSA_SCARD_OK);
}


/*****************************************************************
*
* Function Name
*   ErrorCallback_Send_R_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Level 1 error sequence callback
*   Sends an R-block.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_ErrorCallback_Send_R_Block(   SATSA_SCARD_DEV_NUM devNum,
                                                                HBOOL *endSequence)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("ErrorCallback_Send_R_Block++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	/* send resynch request block */
	/* 현재 scenarios로 통신을 하게 되면 error가 나오다가 resync이후에 제대로 transfer가 이루어 진다. */
	/* 무조건 먼저 resync request를 하도록 수정......20070916 */

#if 0
	Compose_R_Block(devNum);
#else
	local_Compose_S_Block(devNum, S_RESYNCH_REQ, 0);
#endif
	t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
	*endSequence = SATSA_FALSE;
	lastSendBlockType = BLOCK_ID_R_ERROR;
	HxLOG_Print("ErrorCallback_Send_R_Block--\r\n");

	return (SATSA_SCARD_OK);
}


/*****************************************************************
*
* Function Name
*   ErrorCallback_Resend_I_Block
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Level 1 error sequence callback
*   Resends previous I-block.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_ErrorCallback_Resend_I_Block( SATSA_SCARD_DEV_NUM devNum,
                                                                HBOOL *endSequence)
{
	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("ErrorCallback_Resend_I_Block++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	MW_SATSA_SCARD_T1_Compose_I_Block(devNum);

	HxLOG_Print("SATSA SCARD T1: resending I Block \r\n");

	t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
	*endSequence = SATSA_FALSE;

	HxLOG_Print("ErrorCallback_Resend_I_Block--\r\n");

	return (SATSA_SCARD_OK);
}


/*****************************************************************
*
* Function Name
*   ErrorCallback_ReceiveFailed
*
* Inputs
*   devNum          Device number
*
* Outputs
*   endSequence     End of transaction flag
*
* Return Codes
*   SATSA_SCARD_OK
*   SATSA_SCARD_FAIL
*
* Description
*   Level 1 error sequence callback
*   EDC failure or API read error.
*
*****************************************************************/

static SATSA_SCARD_RESULT_CODE local_ErrorCallback_ReceiveFailed(  SATSA_SCARD_DEV_NUM devNum,
                                                                HBOOL *endSequence)
{
	HUINT8 pcbByte;

	SATSA_SCARD_CONTROL_BLOCK *scb;
	SATSA_SCARD_T1_CONTROL_BLOCK *t1Block;

	HxLOG_Print("ErrorCallback_ReceiveFailed++\r\n");

	scb = &scardControlBlock[devNum];
	t1Block = &scb->t1CntrlBlock;

	pcbByte = t1Block->blockOut[PROLOGUE_PCB];

	if ((local_GetBlockID(pcbByte) == BLOCK_ID_S) &&((pcbByte & RESPONSE_BIT) == 0))
	{
		/*
		** The last block sent was an S(REQUEST) block.
		** See ISO 7816-3, 9.6.2.3.2, Rule 7.3
		*/

		/*
		** Resend already composed S(REQUEST) block.
		** There is nothing to do here.
		*/
	}
	else
	{
		/*
		** See ISO 7816-3, 9.6.2.3.2, Rules 7.3, 7.6
		*/
		local_Compose_R_Block(devNum);
	}

	t1Block->state = SATSA_SCARD_T1_SENDING_BLOCK;
	*endSequence = SATSA_FALSE;

	HxLOG_Print("ErrorCallback_ReceiveFailed--\r\n");

	return (SATSA_SCARD_OK);
}





