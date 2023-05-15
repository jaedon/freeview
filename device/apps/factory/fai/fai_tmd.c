/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
/* global header files */
#include <time.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_tmd.h"
#include "fai_util.h"
#include "fai_testparams.h"
#include "fdi_tmd.h"

/* model specific configuration header */
#include "factorysw_config.h"

#define TRACE_TMD 0

#define FAI_TMD_CMD_START_CODE 			0xFD
#define FAI_TMD_REQ_ENTER_TEST_CODE		0xFA
#define FAI_TMD_ACK_START_CODE 			0xF9
#define FAI_TMD_NAK_START_CODE 			0xEA
#define FAI_TMD_RET_START_CODE 			0xFD
#define FAI_TMD_ENTER_TEST_MODE_CODE 	0xFB

#define FAI_TMD_PROTOCOL_VERSION			TMD_PROTOCOL_VERSION

#define FAI_TMD_MIN_PACKET_LENGTH		6		/* msg id + category + module + action + num of param */
#define FAI_TMD_MAX_PACKET_LENGTH		255		/* msg id + category + module + action + num of param */

#define FAI_TMD_START_PACKET_LENGTH 	9
#define FAI_TMD_ACK_LENGTH				6
#define FAI_TMD_NAK_LENGTH				7

#define FAI_TMD_RETRY_TIME				1000
#define FAI_TMD_MAX_WAIT				(0xFFFFFFFF) /* no timeout */

typedef enum
{
	FAI_TMD_CMD_IDLE = 0,
	FAI_TMD_CMD_RECEIVED_HEADER,
	FAI_TMD_CMD_RECEIVED_PACKET_LENGTH,
	FAI_TMD_CMD_RECEIVED_PAYLOAD,
	FAI_TMD_CMD_RECEIVED_CRC16_ONEBYTE,
	FAI_TMD_CMD_RECEIVED_CRC16_TWOBYTE,
	FAI_TMD_CMD_DISCONNECT
} FAI_TMD_CMD_STEP;

typedef enum
{
	FAI_TMD_NAK_INVALID_CMD = 0,
	FAI_TMD_NAK_CRC_ERR,
	FAI_TMD_NAK_SYNC_FIELD_ERR
} FAI_TMD_NAK;

typedef struct
{
	unsigned short usMsgId;
	unsigned char ucCategory;
	unsigned char ucModule;
	unsigned char ucAction;
	unsigned char ucNumOfParams;
	unsigned char aucParams[FAI_TMD_MAX_PARAM_LENGTH];
} FAI_TMD_PACKET_T; 

/* TMD Test Result Parameter */
#if 1
typedef struct tmdtestinfo
{
	unsigned short usMsgId[MAX_TMD_TEST_MULTI_INDEX];
	BOOL bExecuting[MAX_TMD_TEST_MULTI_INDEX];
	long lStartTime[MAX_TMD_TEST_MULTI_INDEX];
	BOOL bArray;
} FAI_TMD_TEST_INFO_T;
#else
typedef struct tmdtestinfo
{
	unsigned short usMsgId;
	BOOL bExecuting;
	long lStartTime;
} FAI_TMD_TEST_INFO_T;
#endif
unsigned long g_ulTmdSemID, g_ulCmdMsgID, g_ulResultMsgID;
static unsigned char g_ucTestIndex;
static unsigned long g_ulTimerId;
FAI_TMD_TEST_INFO_T g_stTmdTestInfo[MAX_TMD_TEST_RESULT];

static void FAI_TMD_RecvCmdTask(void *data);
static void FAI_TMD_CmdProcTask(void *data);
static void FAI_TMD_ResultTask(void *data);

FAI_ERR_CODE INTRN_FAI_TMD_Start();
FAI_ERR_CODE INTRN_FAI_TMD_StartTest(FAI_TMD_PACKET_T *ptTmdPacket);
BOOL INTRN_FAI_TMD_IsValidCmd(unsigned char ucCategory, unsigned char ucModule, unsigned char ucAction);
int INTRN_FAI_TMD_GetTableIndex(unsigned char ucCategory, unsigned char ucModule, unsigned char ucAction);
BOOL INTRN_FAI_TMD_IsMultiCmd(int nIndexId);
int INTRN_FAI_TMD_GetMutiIndex(int nIndex,unsigned short usMsgId);
FAI_ERR_CODE INTRN_FAI_TMD_ProcCmd(FAI_TMD_PACKET_T *ptTmdPacket);
FAI_ERR_CODE INTRN_FAI_TMD_SendACK(unsigned char* pucMsgId);
FAI_ERR_CODE INTRN_FAI_TMD_SendNAK(unsigned char* pucMsgId, FAI_TMD_NAK eErrFlag);
FAI_ERR_CODE INTRN_FAI_TMD_SendBusy(unsigned char* pucMsgId);
BOOL INTRN_FAI_TMD_CheckReqestEnterTestPacket(unsigned char* pucCmdBuf);
void INTRN_FAI_TMD_SetTestIndex(unsigned char ucIndex);
int INTRN_FAI_TMD_GetTestIndex(void);
unsigned char INTRN_FAI_TMD_GetParamsLen(unsigned char *pucParams, int numOfParams);
FAI_ERR_CODE INTRN_FAI_TMD_SendParameterSizeError(unsigned char* pucMsgId);

/*******************************************************************************
* function : FAI_TMD_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_Init()
{
	FVK_ERR_CODE eFvkErr;
	FDI_ERR_CODE eFdiErr;
	unsigned long ulTmdCmdRecvTaskID, ulTmdCmdProcTaskID, ulTmdResultTaskID;

	FVK_MEM_Set(&g_stTmdTestInfo, 0x00, sizeof(g_stTmdTestInfo));

	eFdiErr = FDI_TMD_Init();
	if(eFdiErr!= FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FDI_TMD_Init \n"));
		return FAI_ERR;
	}	
	eFvkErr = FVK_SEM_Create("TmdSem",&g_ulTmdSemID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}
	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_TMD_PACKET_T), (char*)"CmdMsg", &g_ulCmdMsgID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}
	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_TMD_RESULT_T), (char*)"ResultMsg", &g_ulResultMsgID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}	
	eFvkErr = FVK_TASK_Create(FAI_TMD_RecvCmdTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "RecvCmdTask", NULL, &ulTmdCmdRecvTaskID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
	eFvkErr = FVK_TASK_Create(FAI_TMD_CmdProcTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "CmdProcTask", NULL, &ulTmdCmdProcTaskID);
	if(eFvkErr!= FVK_NO_ERR)	
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
	eFvkErr = FVK_TASK_Create(FAI_TMD_ResultTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "ResultTask", NULL, &ulTmdResultTaskID);
	if(eFvkErr!= FVK_NO_ERR)	
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}	
	eFvkErr = FVK_TASK_Start(ulTmdCmdRecvTaskID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}
	eFvkErr = FVK_TASK_Start(ulTmdCmdProcTaskID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}
	eFvkErr = FVK_TASK_Start(ulTmdResultTaskID);
	if(eFvkErr!= FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FAI_TMD_GetParam
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned char FAI_TMD_GetParam(unsigned char *pucParamBuf, unsigned char *pucArgList, int numOfParams)
{
	int i, len, ofs=0;

	for(i=0; i<numOfParams; i++)
	{
		len = pucArgList[ofs++];
		ofs += len;
	}
	len = pucArgList[ofs++];
	FVK_MEM_Copy(pucParamBuf, &pucArgList[ofs], len);

	return len;
}

/*******************************************************************************
* function : FAI_TMD_AddResultParam
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_AddResultParam(FAI_TMD_RESULT_T *ptResult, void *pvParam, unsigned char ucLen)
{
	int i, len, ofs=0;
	
	for(i=0; i<ptResult->ucNumOfParam; i++)
	{
		len = ptResult->aucParams[ofs++];
		ofs += len;
	}
	ptResult->aucParams[ofs++] = ucLen;
	FVK_MEM_Copy(&ptResult->aucParams[ofs], pvParam, ucLen);
	ptResult->ucNumOfParam++;

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FAI_TMD_SetResult
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_SetResult(FAI_TMD_RESULT_T *ptResult)
{
	FVK_ERR_CODE eFvkErr;
	int i;

	/* category(0), module(0), action(0) indicate current test item */
	if(!ptResult->ucCategory && !ptResult->ucModule && !ptResult->ucAction)
	{
		i = INTRN_FAI_TMD_GetTestIndex();
		ptResult->ucCategory = g_stTmdTestItemTable[i].ucCategory;
		ptResult->ucModule = g_stTmdTestItemTable[i].ucModule;
		ptResult->ucAction = g_stTmdTestItemTable[i].ucAction;
	}
	
	/* send msg */
	eFvkErr = FVK_MSG_Send(g_ulResultMsgID, ptResult, sizeof(FAI_TMD_RESULT_T));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_SetResult] : FVK_MSG_Send Error!!\n"));
		return FAI_ERR;
	}	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FAI_TMD_ArgToValue
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_ArgToValue(unsigned long *pulValue, unsigned char *pucArg, int nArgSize)
{
	int i;

	*pulValue = (unsigned long)pucArg[0];
	for(i=1; i<nArgSize; i++)
	{
		*pulValue = (unsigned long)(*pulValue<<8)|pucArg[i];
	}
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FAI_TMD_RecvCmdTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_TMD_CMD_STEP g_eStep = FAI_TMD_CMD_DISCONNECT;

static void FAI_TMD_RecvCmdTask(void *data)
{
	FVK_ERR_CODE eFvkErr;
	FDI_ERR_CODE eFdiErr;
	FAI_ERR_CODE eFaiErr;
	unsigned char ucRcvByte, ucTotalPacketLength, ucRecvPacketLength;
	unsigned char aucCmdBuf[FAI_TMD_MAX_PACKET_LENGTH];
	unsigned short usCrc16;
	FAI_TMD_PACKET_T tTmdPacket;
	int i;
	int len, ofs=0;
	
	while(1)
	{
		g_eStep = FAI_TMD_CMD_DISCONNECT;
		eFdiErr = FDI_TMD_Open();
		if( eFdiErr != FDI_NO_ERR )
		{
			//FAI_PRINT(FAI_PRT_WARNING, ("[FAI_TMD_RecvCmdTask] : FDI_TMD_Open Error!!\n"));
			FVK_TASK_Sleep(FAI_TMD_RETRY_TIME);
			continue;
		}

		FAI_PRINT(TRACE_TMD, ("==== TMD Connection OK ================\n\n"));

#if 0
		FVK_MEM_Set(&g_stTmdTestInfo, 0x00, sizeof(g_stTmdTestInfo));
#endif
		eFaiErr = INTRN_FAI_TMD_Start();
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : INTRN_FAI_TMD_Start Error!!\n"));
			FDI_TMD_Close();			
			continue;
		}
		
		FAI_PRINT(TRACE_TMD, ("==== Send Enter Test Mode Packet!! ====\n\n"));

		g_eStep = FAI_TMD_CMD_IDLE;
		while(1)
		{
			eFdiErr = FDI_TMD_ReceiveData(&ucRcvByte, sizeof(char), FAI_TMD_MAX_WAIT);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : FDI_TMD_ReceiveData Error!!\n"));
				break;
			}

			switch(g_eStep)
			{
				case FAI_TMD_CMD_IDLE:
					if( ucRcvByte != FAI_TMD_CMD_START_CODE && ucRcvByte != FAI_TMD_REQ_ENTER_TEST_CODE )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : Receive Start Code Error!! 0x%02x\n", ucRcvByte));
						break;
					}
					ucRecvPacketLength = 1;
					aucCmdBuf[0] = ucRcvByte;					
					g_eStep = FAI_TMD_CMD_RECEIVED_HEADER;
					break;
				case FAI_TMD_CMD_RECEIVED_HEADER:
					ucTotalPacketLength = ucRcvByte;
					ucRecvPacketLength++;
					aucCmdBuf[1] = ucTotalPacketLength;
					if(ucTotalPacketLength > 0)
					{
						g_eStep = FAI_TMD_CMD_RECEIVED_PACKET_LENGTH;
					}
					else
					{
						g_eStep = FAI_TMD_CMD_RECEIVED_PAYLOAD;
					}
					break;
				case FAI_TMD_CMD_RECEIVED_PACKET_LENGTH:
					aucCmdBuf[ucRecvPacketLength++] = ucRcvByte;
					if( ucTotalPacketLength == ucRecvPacketLength-2 )
					{
						g_eStep = FAI_TMD_CMD_RECEIVED_PAYLOAD;
						break;
					}
					break;
				case FAI_TMD_CMD_RECEIVED_PAYLOAD:
					usCrc16 = ucRcvByte << 8;
					g_eStep = FAI_TMD_CMD_RECEIVED_CRC16_ONEBYTE;
					break;
				case FAI_TMD_CMD_RECEIVED_CRC16_ONEBYTE:
					usCrc16 |= ucRcvByte;
					
					/*
					 *                                                     Command Packet
					 * |---8 bit----|---8 bit----|---16 bit---|---8 bit----|---8 bit----|---8 bit----|---8 bit----|---N bit----|---16 bit---|
					 * |header(0xFD)|   length   |	 Msg ID   |  Category  |   Module   |Action Item |num of param|   params   |  CRC 16    |
					 * |------------|------------|------------|------------|------------|------------|------------|------------|------------|
					 *
					 *         Param descriptor
					 * |---8 bit----|---len of param bytes----|
					 * |len of param|         param           |
					 * |------------|-------------------------|
					 *
					 */
					
					FAI_PRINT(TRACE_TMD, ("\n==== Receive Packet ===================\n"));
					FAI_PRINT(TRACE_TMD, ("Msg ID        : 0x%02X%02X\n", aucCmdBuf[2], aucCmdBuf[3]));
					FAI_PRINT(TRACE_TMD, ("Command       : 0x%02X 0x%02X 0x%02X\n", aucCmdBuf[4], aucCmdBuf[5], aucCmdBuf[6]));
					FAI_PRINT(TRACE_TMD, ("Num of Params : 0x%02X\n", aucCmdBuf[7]));
					FAI_PRINT(TRACE_TMD, ("Params        : "));
					for(i=0; i<ucTotalPacketLength-6; i++)
					{
						FAI_PRINT(TRACE_TMD, ("0x%02X ", aucCmdBuf[8+i]));
					}
					FAI_PRINT(TRACE_TMD, ("\n"));
					FAI_PRINT(TRACE_TMD, ("CRC16         : 0x%04X\n", usCrc16));
					
					ofs = 0;
					len = 0;
					for(i=0; i< aucCmdBuf[7]; i++)
					{
						len = aucCmdBuf[8+(ofs++)];
						ofs += len;
					}
										
					/* Check CRC */
					if( usCrc16 != FAI_UTIL_GetCRC16(0, aucCmdBuf, ucTotalPacketLength+2) )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : CRC Error!!\n"));
						eFaiErr = INTRN_FAI_TMD_SendNAK(&aucCmdBuf[2], FAI_TMD_NAK_CRC_ERR);
						if( eFaiErr != FAI_NO_ERR )
						{
							FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : INTRN_FAI_TMD_SendNak Error!!\n"));
						}
						g_eStep = FAI_TMD_CMD_IDLE;
						break;
					}

					/* Check request test mode packet */
					if( INTRN_FAI_TMD_CheckReqestEnterTestPacket(aucCmdBuf) )
					{
						eFaiErr = INTRN_FAI_TMD_Start();
						if( eFaiErr != FAI_NO_ERR )
						{
							FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : INTRN_FAI_TMD_Start Error!!\n"));
							break;
						}
						FAI_PRINT(TRACE_TMD, ("==== Send Enter Test Mode Packet!! ====\n\n"));						
						g_eStep = FAI_TMD_CMD_IDLE;
						break;
					}
					
					/* Check command validation */
					if( !INTRN_FAI_TMD_IsValidCmd(aucCmdBuf[4], aucCmdBuf[5], aucCmdBuf[6]) )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : Invalid Command!!\n"));
						eFaiErr = INTRN_FAI_TMD_SendNAK(&aucCmdBuf[2], FAI_TMD_NAK_INVALID_CMD);
						if( eFaiErr != FAI_NO_ERR )
						{
							g_eStep = FAI_TMD_CMD_IDLE;							
							FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : INTRN_FAI_TMD_SendNak Error!!\n"));
						}
						g_eStep = FAI_TMD_CMD_IDLE;
						break;
					}

					/* Everything is OK, Send ACK */
					eFaiErr = INTRN_FAI_TMD_SendACK(&aucCmdBuf[2]);
					if( eFaiErr != FAI_NO_ERR )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : INTRN_FAI_TMD_SendNak Error!!\n"));
						g_eStep = FAI_TMD_CMD_IDLE;
						break;
					}
					
					FAI_PRINT(TRACE_TMD, ("==== Send ACK =========================\n"));
					tTmdPacket.usMsgId = (aucCmdBuf[2]<<8)|aucCmdBuf[3];
					tTmdPacket.ucCategory = aucCmdBuf[4];
					tTmdPacket.ucModule = aucCmdBuf[5];
					tTmdPacket.ucAction= aucCmdBuf[6];
					tTmdPacket.ucNumOfParams = aucCmdBuf[7];
					FVK_MEM_Copy(tTmdPacket.aucParams, &aucCmdBuf[8], ucTotalPacketLength-6);

					/* Set start flag */
					eFaiErr = INTRN_FAI_TMD_StartTest(&tTmdPacket);
					if( eFaiErr != FAI_NO_ERR )
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : INTRN_FAI_TMD_StartTest Error!!\n"));
						g_eStep = FAI_TMD_CMD_IDLE;
						INTRN_FAI_TMD_SendBusy(&aucCmdBuf[2]);
						break;
					}

					if(ofs != (ucTotalPacketLength-6))
					{
						FAI_TMD_RESULT_T tTMDResult;
						unsigned char ucTmdErrCode[2];
						
						FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

						tTMDResult.ucCategory = aucCmdBuf[4];
						tTMDResult.ucModule = aucCmdBuf[5];
						tTMDResult.ucAction = aucCmdBuf[6];
						tTMDResult.eResult = FAI_TMD_ERR_FAIL;

						ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
						ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
						FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

						FAI_TMD_SetResult(&tTMDResult);
						g_eStep = FAI_TMD_CMD_IDLE;
						break;						
					}

					FAI_PRINT(TRACE_TMD, ("==== Start Test =======================\n"));

					/* Send msg */
					eFvkErr = FVK_MSG_Send(g_ulCmdMsgID, &tTmdPacket, sizeof(FAI_TMD_PACKET_T));
					if (eFvkErr != FVK_NO_ERR)
					{
						FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_RecvCmdTask] : FVK_MSG_Send Error!!\n"));
						g_eStep = FAI_TMD_CMD_IDLE;
						break;
					}
					g_eStep = FAI_TMD_CMD_IDLE;
					break;
				default:
					break;
			}
		}
		FDI_TMD_Close();
	}
}

/*******************************************************************************
* function : FAI_TMD_CmdProcTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void FAI_TMD_CmdProcTask(void *data)
{
	FAI_ERR_CODE eFaiErr;
	FVK_ERR_CODE eVkRet;
	FAI_TMD_PACKET_T tTmdPacket;

	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulCmdMsgID, &tTmdPacket, sizeof(FAI_TMD_PACKET_T));
		if(eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_CmdProcTask] : FVK_MSG_Receive Error!!\n"));
			continue;
		}

		FAI_PRINT(TRACE_TMD, ("==== Execute Test Fuction =============\n"));
		
		/* process command */
		eFaiErr = INTRN_FAI_TMD_ProcCmd(&tTmdPacket);
		if(eFaiErr != FAI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_CmdProcTask] : INTRN_FAI_TMD_ProcessCommand Error!!\n"));
			continue;
		}		
	}
}
static FVK_ERR_CODE INTRN_FAI_TMD_TimerCallback(unsigned long ulTimerId, void *pvParams)
{
	FVK_ERR_CODE eFvkErr;
	FAI_TMD_RESULT_T tResult=*(FAI_TMD_RESULT_T *)pvParams;
		
	/* send msg */
	eFvkErr = FVK_MSG_Send(g_ulResultMsgID, &tResult, sizeof(FAI_TMD_RESULT_T));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_SetResult] : FVK_MSG_Send Error!!\n"));
	}	
}

/*******************************************************************************
* function : FAI_TMD_ResultTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void FAI_TMD_ResultTask(void *data)
{
	FDI_ERR_CODE eFdiErr;	
	FVK_ERR_CODE eVkRet;
	FAI_TMD_RESULT_T tResult;
	unsigned short usCrc16;
	unsigned char aucResult[FAI_TMD_MAX_PACKET_LENGTH];
	unsigned char ucResultSize;
	unsigned char ucParamsLen;
	int i,j,nMultiIndex;
	
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulResultMsgID, &tResult, sizeof(FAI_TMD_RESULT_T));
		if(eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_ResultTask] : FVK_MSG_Receive Error!!\n"));
			continue;
		}

		FVK_SEM_Get(g_ulTmdSemID);
		
		i = INTRN_FAI_TMD_GetTableIndex(tResult.ucCategory, tResult.ucModule, tResult.ucAction); 
#if 1
		if(INTRN_FAI_TMD_IsMultiCmd(i)==TRUE)
		{
			nMultiIndex = INTRN_FAI_TMD_GetMutiIndex(i, tResult.usMsgId);
			if(nMultiIndex == 0xFF)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_GetMutiIndex] :MultiIndex Error !!\n")); 	
				FVK_SEM_Release(g_ulTmdSemID);
				return;
			}
		}
		else
		{
			nMultiIndex = 0;
		}
#endif

		if( !g_stTmdTestInfo[i].bExecuting[nMultiIndex] )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_ResultTask] : Test was not executed!!\n"));
			FVK_SEM_Release(g_ulTmdSemID);
			continue;
		}
#if 0		
		g_stTmdTestInfo[i].bExecuting[nMultiIndex] = FALSE;
#endif		

		/*
		 *                                               Result Packet
		 * |---8 bit----|---8 bit----|---16 bit---|---8 bit----|---8 bit----|---8 bit----|---N bit----|---16 bit---|
		 * |header(0xFD)|   length   |   Msg ID   | Result bit | spent time |num of param|   params   |   CRC 16   |
		 * |------------|------------|------------|------------|------------|------------|------------|------------|
		 * 
		 *         Param descriptor
		 * |---8 bit----|---len of param bytes----|
		 * |len of param|         param           |
		 * |------------|-------------------------|
		 * 
		 */
		ucParamsLen = INTRN_FAI_TMD_GetParamsLen(tResult.aucParams, tResult.ucNumOfParam);
		aucResult[0] = FAI_TMD_RET_START_CODE;
		aucResult[1] = 5+ucParamsLen;
		aucResult[2] = (unsigned char)(g_stTmdTestInfo[i].usMsgId[nMultiIndex] >> 8);		
		aucResult[3] = (unsigned char)((g_stTmdTestInfo[i].usMsgId[nMultiIndex] << 8) >> 8);		
		aucResult[4] = tResult.eResult;
		aucResult[5] = (unsigned char)(time(NULL)-g_stTmdTestInfo[i].lStartTime[nMultiIndex]);
		aucResult[6] = tResult.ucNumOfParam;
		FVK_MEM_Copy(&aucResult[7], tResult.aucParams, ucParamsLen);
		
		usCrc16 = FAI_UTIL_GetCRC16(0, &aucResult[0], aucResult[1]+2);
		aucResult[7+ucParamsLen] = (unsigned char)(usCrc16 >> 8);
		aucResult[8+ucParamsLen] = (unsigned char)((usCrc16 << 8) >> 8);
		
		ucResultSize = 9+ucParamsLen;

		FAI_PRINT(TRACE_TMD, ("==== Send Result ======================\n"));
		FAI_PRINT(TRACE_TMD, ("Msg ID        : 0x%02X%02X\n", aucResult[2], aucResult[3]));
		FAI_PRINT(TRACE_TMD, ("Result        : 0x%02X\n", aucResult[4]));
		FAI_PRINT(TRACE_TMD, ("Spent Time    : 0x%02X\n", aucResult[5]));
		FAI_PRINT(TRACE_TMD, ("Num of Params : 0x%02X\n", aucResult[6]));
		FAI_PRINT(TRACE_TMD, ("Params        : "));
		for(j=0; j<ucParamsLen; j++)
		{
			FAI_PRINT(TRACE_TMD, ("0x%02X ", aucResult[7+j]));
		}
		FAI_PRINT(TRACE_TMD, ("\n"));
		FAI_PRINT(TRACE_TMD, ("CRC16         : 0x%04X\n", usCrc16));

		FVK_SEM_Release(g_ulTmdSemID);
		
		/* send result packet */
		if(g_eStep == FAI_TMD_CMD_DISCONNECT)
		{
			printf("Ethernet]Current: Disconnected,,, Wait..\n");
			FVK_TIMER_CreateStart(1000, INTRN_FAI_TMD_TimerCallback, &tResult, sizeof(FAI_TMD_RESULT_T), FVK_TIMER_ONCE, &g_ulTimerId);
		}
		else
		{
			eFdiErr = FDI_TMD_SendData(aucResult, ucResultSize);
			if( eFdiErr != FDI_NO_ERR )
			{
			
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_ResultTask] : FDI_TMD_SendData Error!!\n"));
				if(g_eStep == FAI_TMD_CMD_DISCONNECT)
				{
					printf("Ethernet]Current: Disconnected,,, Wait..\n");
					FVK_TIMER_CreateStart(1000, INTRN_FAI_TMD_TimerCallback, &tResult, sizeof(FAI_TMD_RESULT_T), FVK_TIMER_ONCE, &g_ulTimerId);
				}
			}
			else
			{
				g_stTmdTestInfo[i].bExecuting[nMultiIndex] = FALSE;
			}
		}
	}
}

/*******************************************************************************
* function : INTRN_FAI_TMD_Start
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_TMD_Start()
{
	FAI_ERR_CODE eFaiErr;
	unsigned char aucTestModePacket[FAI_TMD_START_PACKET_LENGTH];
	unsigned short usCrc16;

	/*                      Enter Test Mode Packet
	 * |---8 bit----|---8 bit----|---24 bit---|---16 bit---|---16 bit---|
	 * |header(0xFB)|	length	 |	 ID(HMX)  |   Version  |   CRC 16   |
	 * |------------|------------|------------|------------|------------|
	 */
	
	aucTestModePacket[0] = FAI_TMD_ENTER_TEST_MODE_CODE; /* start header */
	aucTestModePacket[1] = 0x05; 						 /* packet length */
	aucTestModePacket[2] = 'H';							 /* ID : HMX */
	aucTestModePacket[3] = 'M';
	aucTestModePacket[4] = 'X';
	aucTestModePacket[5] = FAI_TMD_PROTOCOL_VERSION >> 8; /* Protocol Version */
	aucTestModePacket[6] = ((FAI_TMD_PROTOCOL_VERSION<<8) >> 8);

	usCrc16 = FAI_UTIL_GetCRC16(0, aucTestModePacket, 7);
	aucTestModePacket[7] = usCrc16 >> 8;
	aucTestModePacket[8] = ((usCrc16<<8) >> 8);

	eFaiErr = FDI_TMD_SendData(aucTestModePacket, FAI_TMD_START_PACKET_LENGTH);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_Start] : FDI_TMD_SendData Error!!\n"));
		return FAI_ERR;
	}	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_StartTest
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_TMD_StartTest(FAI_TMD_PACKET_T *ptTmdPacket)
{
	int i,nMultiIndex;

	FVK_SEM_Get(g_ulTmdSemID);

	i = INTRN_FAI_TMD_GetTableIndex(ptTmdPacket->ucCategory, ptTmdPacket->ucModule, ptTmdPacket->ucAction);

	if(INTRN_FAI_TMD_IsMultiCmd(i)==TRUE)
	{
		nMultiIndex = INTRN_FAI_TMD_GetMutiIndex(i, ptTmdPacket->usMsgId);
		if(nMultiIndex == 0xFF)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_StartTest] :MultiIndex Error !!\n"));		
			FVK_SEM_Release(g_ulTmdSemID);
			return FAI_ERR;
		}
	}
	else
	{
		nMultiIndex = 0;
	}
	
	if( g_stTmdTestInfo[i].bExecuting[nMultiIndex] )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_StartTest] : Test is Already executing!!\n"));		
		FVK_SEM_Release(g_ulTmdSemID);
		return FAI_ERR;
	}
	g_stTmdTestInfo[i].usMsgId[nMultiIndex] = ptTmdPacket->usMsgId;
	g_stTmdTestInfo[i].bExecuting[nMultiIndex] = TRUE;
	g_stTmdTestInfo[i].lStartTime[nMultiIndex] = time(NULL);

	FVK_SEM_Release(g_ulTmdSemID);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_IsValidCmd
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
BOOL INTRN_FAI_TMD_IsValidCmd(unsigned char ucCategory, unsigned char ucModule, unsigned char ucAction)
{
	int i;
	
	for(i=0; i<MAX_TMD_TEST_ITEM; i++)
	{
		if( g_stTmdTestItemTable[i].ucCategory == ucCategory &&
			g_stTmdTestItemTable[i].ucModule == ucModule &&
			g_stTmdTestItemTable[i].ucAction == ucAction )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_GetTableIndex
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
int INTRN_FAI_TMD_GetTableIndex(unsigned char ucCategory, unsigned char ucModule, unsigned char ucAction)
{
	int i;
	
	for(i=0; i<MAX_TMD_TEST_ITEM; i++)
	{
		if( g_stTmdTestItemTable[i].ucCategory == ucCategory &&
			g_stTmdTestItemTable[i].ucModule == ucModule &&
			g_stTmdTestItemTable[i].ucAction == ucAction )
		{
			return i;
		}
	}
	return 0xFF; /* Invalid index */
}

/*******************************************************************************
* function : INTRN_FAI_TMD_IsMultiCmd
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
BOOL INTRN_FAI_TMD_IsMultiCmd(int nIndexId)
{

	return g_stTmdTestItemTable[nIndexId].bMulti;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_GetMutiIndex
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
int INTRN_FAI_TMD_GetMutiIndex(int nIndex,unsigned short usMsgId)
{
	int i;
	
	for(i=0; i<MAX_TMD_TEST_MULTI_INDEX; i++)
	{
		if( (g_stTmdTestInfo[nIndex].usMsgId[i] == usMsgId) &&
			(g_stTmdTestInfo[nIndex].bExecuting[i] == TRUE) )
		{
			return i;
		}
	}

	if(i == MAX_TMD_TEST_MULTI_INDEX)
	{
		for(i=0; i<MAX_TMD_TEST_MULTI_INDEX; i++)
		{
			if(g_stTmdTestInfo[nIndex].bExecuting[i] == FALSE)
			{
				return i;
			}
		}

	}
	return 0xFF; /* Invalid index */
}

/*******************************************************************************
* function : INTRN_FAI_TMD_ProcCmd
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_TMD_ProcCmd(FAI_TMD_PACKET_T *ptTmdPacket)
{
	int i;

	/* process command */
	for(i=0; i<MAX_TMD_TEST_ITEM; i++)
	{
		if( g_stTmdTestItemTable[i].ucCategory == ptTmdPacket->ucCategory &&
			g_stTmdTestItemTable[i].ucModule == ptTmdPacket->ucModule &&
			g_stTmdTestItemTable[i].ucAction == ptTmdPacket->ucAction &&
			g_stTmdTestItemTable[i].pfnTestFunc != NULL )
		{
			INTRN_FAI_TMD_SetTestIndex(i);
			g_stTmdTestItemTable[i].pfnTestFunc(ptTmdPacket->usMsgId,ptTmdPacket->ucNumOfParams, ptTmdPacket->aucParams);
		}
	}
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_SendAck
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_TMD_SendACK(unsigned char* pucMsgId)
{
	FAI_ERR_CODE eFaiErr;	
	unsigned char aucAckBuf[FAI_TMD_ACK_LENGTH];
	unsigned short usCrc16;

	/*                      ACK Packet
	 * |---8 bit----|---8 bit----|---16 bit---|---16 bit---|
	 * |header(0xF9)|	length	 |	 Msg ID   |   CRC 16   |
	 * |------------|------------|------------|------------|
	 */

	aucAckBuf[0] = FAI_TMD_ACK_START_CODE;  			/* ack start code */
	aucAckBuf[1] = 2;									/* ack packet length */
	aucAckBuf[2] = pucMsgId[0];							/* msg id */
	aucAckBuf[3] = pucMsgId[1];
	usCrc16 = FAI_UTIL_GetCRC16(0, aucAckBuf, 4);
	aucAckBuf[4] = (unsigned char)(usCrc16 >> 8);		/* crc16 */
	aucAckBuf[5] = (unsigned char)((usCrc16 << 8) >> 8);
	eFaiErr = FDI_TMD_SendData(aucAckBuf, FAI_TMD_ACK_LENGTH);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_SendAck] : FDI_TMD_SendData Error!!\n"));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_SendNak
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_TMD_SendNAK(unsigned char* pucMsgId, FAI_TMD_NAK eErrFlag)
{
	FAI_ERR_CODE eFaiErr;
	unsigned char aucNakBuf[FAI_TMD_NAK_LENGTH];
	unsigned short usCrc16;

	/*                      NAK Packet
	 * |---8 bit----|---8 bit----|---16 bit---|---8 bit----|---16 bit---|
	 * |header(0xEA)|	length	 |	 Msg ID   | Error Flag |   CRC 16   |
	 * |------------|------------|------------|------------|------------|
	 */

	aucNakBuf[0] = FAI_TMD_NAK_START_CODE;  			/* nak start code */
	aucNakBuf[1] = 3;									/* nak packet length */
	aucNakBuf[2] = pucMsgId[0];							/* msg id */
	aucNakBuf[3] = pucMsgId[1];
	aucNakBuf[4] = eErrFlag;
	usCrc16 = FAI_UTIL_GetCRC16(0, aucNakBuf, 5);
	aucNakBuf[5] = (unsigned char)(usCrc16 >> 8);		/* crc16 */
	aucNakBuf[6] = (unsigned char)((usCrc16 << 8) >> 8);
	eFaiErr = FDI_TMD_SendData(aucNakBuf, FAI_TMD_NAK_LENGTH);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_SendAck] : FDI_TMD_SendData Error!!\n"));
		return FAI_ERR;
	}	
	return FAI_NO_ERR;
}

FAI_ERR_CODE INTRN_FAI_TMD_SendBusy(unsigned char* pucMsgId)
{
	FAI_ERR_CODE eFaiErr;
	unsigned char aucNakBuf[9];
	unsigned short usCrc16;

	/*                      Result Packet
	 * |---8 bit-----|---8 bit----|---16 bit---|---8 bit----|---8 bit----|---8 bit-----|---16 bit---|
	 * |header(0xFD) |   length	    |	    Msg ID   |   Result bit   |     Time      |Num of param|   CRC 16   |
	 * |------------|----------|------- ---|-----------|----------|------------|----------|
	 */

	aucNakBuf[0] = FAI_TMD_RET_START_CODE;  			/* nak start code */
	aucNakBuf[1] = 5;									/* nak packet length */
	aucNakBuf[2] = pucMsgId[0];							/* msg id */
	aucNakBuf[3] = pucMsgId[1];
	aucNakBuf[4] = 2;									/*Busy*/
	aucNakBuf[5] = 0;									/*TIME*/
	aucNakBuf[6] = 0;									/*Num of param*/	
	
	usCrc16 = FAI_UTIL_GetCRC16(0, aucNakBuf, 7);
	aucNakBuf[7] = (unsigned char)(usCrc16 >> 8);		/* crc16 */
	aucNakBuf[8] = (unsigned char)((usCrc16 << 8) >> 8);
	eFaiErr = FDI_TMD_SendData(aucNakBuf, 9);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_SendAck] : FDI_TMD_SendData Error!!\n"));
		return FAI_ERR;
	}	
	return FAI_NO_ERR;
}

FAI_ERR_CODE INTRN_FAI_TMD_SendParameterSizeError(unsigned char* pucMsgId)
{
	FAI_ERR_CODE eFaiErr;
	unsigned char aucNakBuf[12];
	unsigned short usCrc16;

	/*                      Result Packet
	 * |---8 bit-----|---8 bit----|---16 bit---|---8 bit----|---8 bit----|---8 bit-----|---16 bit---|
	 * |header(0xFD) |   length	    |	    Msg ID   |   Result bit   |     Time      |Num of param|   CRC 16   |
	 * |------------|----------|------- ---|-----------|----------|------------|----------|
	 */

	aucNakBuf[0] = FAI_TMD_RET_START_CODE;  			/* nak start code */
	aucNakBuf[1] = 8;									/* nak packet length */
	aucNakBuf[2] = pucMsgId[0];							/* msg id */
	aucNakBuf[3] = pucMsgId[1];
	aucNakBuf[4] = 1;									/*FAIL*/
	aucNakBuf[5] = 0;									/*TIME*/
	aucNakBuf[6] = 1;									/*Num of param*/	
	aucNakBuf[7] = 2;									/*length param*/	
	aucNakBuf[8] = 0;									/*param*/	
	aucNakBuf[9] = 1;									/*param*/	
	
	usCrc16 = FAI_UTIL_GetCRC16(0, aucNakBuf, 10);
	aucNakBuf[10] = (unsigned char)(usCrc16 >> 8);		/* crc16 */
	aucNakBuf[11] = (unsigned char)((usCrc16 << 8) >> 8);
	eFaiErr = FDI_TMD_SendData(aucNakBuf, 12);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_TMD_SendAck] : FDI_TMD_SendData Error!!\n"));
		return FAI_ERR;
	}	
	return FAI_NO_ERR;
}



/*******************************************************************************
* function : INTRN_FAI_TMD_CheckReqestEnterTestPacket
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
BOOL INTRN_FAI_TMD_CheckReqestEnterTestPacket(unsigned char* pucCmdBuf)
{
	if(pucCmdBuf[0] == FAI_TMD_REQ_ENTER_TEST_CODE)
		return TRUE;
	else
		return FALSE;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_SetTestIndex
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_TMD_SetTestIndex(unsigned char ucIndex)
{
	g_ucTestIndex = ucIndex;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_GetTestIndex
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
int INTRN_FAI_TMD_GetTestIndex(void)
{
	return g_ucTestIndex;
}

/*******************************************************************************
* function : INTRN_FAI_TMD_GetParamsLen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned char INTRN_FAI_TMD_GetParamsLen(unsigned char *pucParams, int numOfParams)
{
	int i, len, total=0;
	
	for(i=0; i<numOfParams; i++)
	{
		len = pucParams[total++];
		total += len;
	}
	return total;
}

/***********************************************************************************************
 * below functions will be remove 
 ***********************************************************************************************/
FAI_ERR_CODE FAI_TMD_StartTest(unsigned char ucParamID) /* this fuction will be remove */
{
	return FAI_ERR;
}

FAI_ERR_CODE FAI_TMD_SetTestResult(unsigned char ucCategory, /* this fuction will be remove */
									unsigned char ucModule,
									unsigned char ucAction,
									unsigned char ucParamID,
									unsigned char ucState,
									unsigned char ucNumberOfParam,
									unsigned char *pucParam,
									unsigned char ucLength)
{
	FAI_ERR_CODE eFaiErr;
	FAI_TMD_RESULT_T tResult;
	unsigned char ucParamsLen;
	
	tResult.ucCategory = ucCategory;
	tResult.ucModule = ucModule;
	tResult.ucAction = ucAction;
	tResult.eResult = ucState;
	tResult.ucNumOfParam = ucNumberOfParam;
	ucParamsLen = INTRN_FAI_TMD_GetParamsLen(pucParam, ucNumberOfParam);
	FVK_MEM_Copy(tResult.aucParams, pucParam, ucParamsLen);
		
	eFaiErr = FAI_TMD_SetResult(&tResult);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_SetTestResult] : FAI_TMD_SetResult Error!!\n"));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}

FAI_ERR_CODE FAI_TMD_SetCurrentResult(unsigned char ucParamID, /* this fuction will be remove */
										unsigned char ucState,
										unsigned char ucNumberOfParam,
										unsigned char *pucParam,
										unsigned char ucLength)
{
	int i;
 
	i = INTRN_FAI_TMD_GetTestIndex();
	FAI_TMD_SetTestResult(g_stTmdTestItemTable[i].ucCategory, g_stTmdTestItemTable[i].ucModule, g_stTmdTestItemTable[i].ucAction,
								ucParamID, ucState, ucNumberOfParam, pucParam, ucLength);
	

	return FAI_NO_ERR;
}

