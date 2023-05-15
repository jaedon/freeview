/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <string.h>

/* chip manufacture's  header files */

/* humax header files */
#include "di_modem.h"

#include "fvk.h"
#include "fdi_common.h"
#include "fdi_modem.h"

/*******************************************************************************
* Debug level
*******************************************************************************/


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FDI_MODEM_BUFFER_LEN	40
#define FDI_MODEM_MSG_QCOUNT	1000
#define FDI_MODEM_MSG_SIZE		16
#define FDI_MODEM_RX_BUFF_SIZE	1024
#define FDI_MODEM_TXRX_RETRY_CNT	200
#define	FDI_MODEM_ATZ_CMD_TIMEOUT	500
#define	FDI_MODEM_INIT_CMD_TIMEOUT	2000
#define FDI_MODEM_INIT_STRING	"AT+MS=V22B;\\N5;S7=45;S91=10;E0V0M0W1"
#define FDI_MODEM_DIAL_STRING	"AT+MS=V22B;\\N5;S7=45;S91=10;E0V0M0W1X0;S8=0"

/* modem line status */
typedef enum
{
	FDI_MODEM_LINE_NOT_IN_USE            	= 0,
	FDI_MODEM_LINE_IN_USE_NOT_CONNECTED  	= 1,
	FDI_MODEM_LINE_IN_USE_CONNECTED      	= 2,
	FDI_MODEM_LINE_IN_USE_OTHER          	= 3,
	FDI_MODEM_LINE_IN_USE_PPP_CONNECTED		= 4,
	FDI_MODEM_LINE_CAN_NOT_KNOW
} FDI_MODEM_LINE_STATUS_e;


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* Static variables and functions
*******************************************************************************/
static unsigned long s_ulFdiModemSema4 = 0;
static unsigned char s_ModemRxBuff[FDI_MODEM_RX_BUFF_SIZE] = {'\0',};
static unsigned int s_unRxBuffReadPtr = 0;
static unsigned int s_unRxBuffWritePtr = 0;
static unsigned char s_bInitModem = FALSE;
static FDI_MODEM_LINE_STATUS_e s_eModemStatus = 0;
static pfnFDI_MODEM_Notify	s_pfnFdiModemCallback = NULL;

static void INTRN_FDI_MODEM_Task(unsigned char *pucArg, unsigned long ulParam);
static void INTRN_FDI_MODEM_Callback(unsigned char eModemEvent, unsigned long ulParam);
static FDI_ERR_CODE INTRN_FDI_MODEM_InitModem(void);
static FDI_ERR_CODE INTRN_FDI_MODEM_RxResponse(unsigned char *pucBuf, unsigned int *unSize, unsigned int unTimeout);
static FDI_ERR_CODE INTRN_FDI_MODEM_FindString(char *pcSrcString, char *pcFindString);


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_Open(void)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	
	FDI_FUNC_ENTER;

	eFvkError = FVK_SEM_Create("Fdi_ModemSema4", &s_ulFdiModemSema4);
	if(eFvkError != FVK_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODEM_Open] Error, Creat Msg or Task, eFvkError: %d !!\n", eFvkError));
		return eFdiError = FDI_ERR;
	}
	
	/* modem init */
	DI_MODEM_Reset();
	FVK_TASK_Sleep(500);
	
	DI_MODEM_RegisterRxCallback((pfnDI_MODEM_Notify)INTRN_FDI_MODEM_Callback);

done:
	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_InitModem(void)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	DI_ERR_CODE		eDiError = DI_ERR_OK;
	unsigned char	ucEnd = 0xD;
	HUINT32			unActualWrittenSize = 0;
	unsigned char	ucDataBuff[FDI_MODEM_BUFFER_LEN];
	unsigned int	unDataSize = 0;

	if(s_bInitModem == TRUE)
	{
		return FDI_NO_ERR;
	}
	
	/* 1. send ATZ and receive OK within 500ms */
	eDiError = DI_MODEM_Write("ATZ", strlen("ATZ"), &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eDiError |= DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eFdiError |= INTRN_FDI_MODEM_RxResponse(ucDataBuff, &unDataSize, FDI_MODEM_ATZ_CMD_TIMEOUT);
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_RxResponse: %d \n", eFdiError));
		goto done;
	}
	eFdiError |= INTRN_FDI_MODEM_FindString(ucDataBuff, "OK");
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODEM_Open] Error, send ATZ !!\n", eFdiError));
		eFdiError = FDI_ERR;
		goto done;
	}
	FVK_TASK_Sleep(1000);

	/* 2. send Init command and receive 0 within 2000ms */
	eDiError |= DI_MODEM_Write("AT+GCI=61", strlen("AT+GCI=61"), &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eDiError |= DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eFdiError |= INTRN_FDI_MODEM_RxResponse(ucDataBuff, &unDataSize, FDI_MODEM_INIT_CMD_TIMEOUT);	
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_RxResponse: %d \n", eFdiError));
		goto done;
	}
	eFdiError |= INTRN_FDI_MODEM_FindString(ucDataBuff, "OK");
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODEM_Open] Error, send AT+GCI=61 !!\n", eFdiError));
		eFdiError = FDI_ERR;
		goto done;
	}

	eDiError |= DI_MODEM_Write(FDI_MODEM_INIT_STRING, strlen(FDI_MODEM_INIT_STRING), &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eDiError |= DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	FVK_TASK_Sleep(1000);
	
	eDiError |= DI_MODEM_Write("AT+GCI?", strlen("AT+GCI?"), &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eDiError |= DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eFdiError |= INTRN_FDI_MODEM_RxResponse(ucDataBuff, &unDataSize, FDI_MODEM_INIT_CMD_TIMEOUT);	
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_RxResponse: %d \n", eFdiError));
		goto done;
	}
	eFdiError |= INTRN_FDI_MODEM_FindString(ucDataBuff, "+GCI: 61");
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MODEM_Open] Error, send Init string !!\n", eFdiError));
		eFdiError = FDI_ERR;
		goto done;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("=====>>>>> Modem Init OK \n"));
	}

	s_unRxBuffWritePtr = s_unRxBuffReadPtr = 0;
	
	s_bInitModem = TRUE;
	
done:
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void FDI_MODEM_CheckDCDState(unsigned char *pucState)
{
	DI_MODEM_CheckDCD(pucState);
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_Connect(unsigned char *pucDialNum)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	DI_ERR_CODE		eDiError = DI_ERR_OK;
	unsigned char	ucEnd = 0xD;
	HUINT32     	unActualWrittenSize = 0;
	unsigned long	ulMessage[4];
	unsigned char	ucDataBuff[FDI_MODEM_BUFFER_LEN] = {'\0',};
	unsigned int	unDataSize = 0;
	unsigned int	i = 0;
	
	FDI_FUNC_ENTER;
	FDI_PRINT(FDI_PRT_DBGINFO, ("Dial Number : %s\n", pucDialNum));

	eDiError = DI_MODEM_Write("ATZ", strlen("ATZ"), &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eDiError = DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}
	eFdiError = INTRN_FDI_MODEM_RxResponse(ucDataBuff, &unDataSize, FDI_MODEM_ATZ_CMD_TIMEOUT);
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_RxResponse: %d \n", eFdiError));
		goto done;
	}
	eFdiError = INTRN_FDI_MODEM_FindString(ucDataBuff, "OK");
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_FindString: %d \n", eFdiError));
		goto done;
	}
	
	/* 2. send Init command and receive 0 within 1000ms */
	memset(ucDataBuff, '\0', sizeof(ucDataBuff));
	sprintf(ucDataBuff, "%sX0;S8=0", FDI_MODEM_INIT_STRING);
	eDiError = DI_MODEM_Write(ucDataBuff, strlen(ucDataBuff), &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_FindString: %d \n", eDiError));
		goto done;
	}
	eDiError = DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_FindString: %d \n", eDiError));
		goto done;
	}
	eFdiError = INTRN_FDI_MODEM_RxResponse(ucDataBuff, &unDataSize, FDI_MODEM_INIT_CMD_TIMEOUT/2);
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_RxResponse: %d \n", eFdiError));
		goto done;
	}
	eFdiError = INTRN_FDI_MODEM_FindString(ucDataBuff, "0");
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ INTRN_FDI_MODEM_FindString: %d \n", eFdiError));
		goto done;
	}
	
	/* 3. send dial number and wait connection during 90s */
	memset(ucDataBuff, '\0', sizeof(ucDataBuff));
	sprintf(ucDataBuff, "ATDT%s", pucDialNum);

	eDiError = DI_MODEM_Write(ucDataBuff, strlen(ucDataBuff), &unActualWrittenSize);
	if(eFdiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eFdiError));
		goto done;
	}
	eDiError = DI_MODEM_Write(&ucEnd, 1, &unActualWrittenSize);
	if(eDiError != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("@@ERROR@@ DI_MODEM_Write: %d \n", eDiError));
		goto done;
	}

	s_eModemStatus = FDI_MODEM_LINE_IN_USE_CONNECTED;

done:	
	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_Disconnect()
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	
	FDI_FUNC_ENTER;
	
	DI_MODEM_Disconnect();
	
	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_TxData(unsigned char *pucData, unsigned int unSize)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	HUINT32     	unActualWrittenSize = 0;
	unsigned char	ucRetryCount = FDI_MODEM_TXRX_RETRY_CNT;
	
	FDI_FUNC_ENTER;
	
	while((ucRetryCount > 0) && (unSize > 0))
	{
		DI_MODEM_Write(pucData, unSize, &unActualWrittenSize);
		if(unSize == unActualWrittenSize)
		{
			unSize = 0;
			break;
		}
		
		FVK_TASK_Sleep(10);
		unSize = unSize - unActualWrittenSize;
		ucRetryCount--;
	};

	if(unSize != 0) 
	{
		FDI_PRINT(FDI_PRT_ERROR, ("Error Write Data, remainSize: %d \n", unSize));
		eFdiError = FDI_ERR;
	}

	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_RxData(unsigned char *pucData, unsigned int unSize)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	unsigned int	unCurReadPtr = 0;
	unsigned int	unCurWritePtr = 0;
	unsigned int	unDataSize = 0;
	unsigned int	unRemainSize = 0;
	unsigned int	unBuffPtr = 0;
	unsigned char	ucRetryCount = FDI_MODEM_TXRX_RETRY_CNT;
	
	FDI_FUNC_ENTER;

	FVK_SEM_Get(s_ulFdiModemSema4);
	unCurReadPtr = s_unRxBuffReadPtr;
	unCurWritePtr = s_unRxBuffWritePtr;
	FVK_SEM_Release(s_ulFdiModemSema4);

	if(unCurReadPtr < unCurWritePtr)
	{
		unRemainSize = unCurWritePtr - unCurReadPtr;
	}
	else
	{
		unRemainSize = FDI_MODEM_RX_BUFF_SIZE - unCurReadPtr + unCurWritePtr;
	}
	if(unRemainSize < unSize)
	{
		eFdiError = FDI_ERR;
		goto done;
	}
	
	while(unSize > 0)
	{
		pucData[unBuffPtr] = s_ModemRxBuff[unCurReadPtr];
		unCurReadPtr++;
		unBuffPtr++;
		unSize--;
		if(unCurReadPtr == FDI_MODEM_RX_BUFF_SIZE)
		{
			unSize = 0;
		}
	}

	FVK_SEM_Get(s_ulFdiModemSema4);
	s_unRxBuffReadPtr = unCurReadPtr;
	FVK_SEM_Release(s_ulFdiModemSema4);

done:	
	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_TxCommand(unsigned char *pucCmd)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	HUINT32	        unActualWrittenSize = 0;
	unsigned long	ulSize = 0;
	unsigned char	ucRetryCount = 200;
	unsigned char	ucEndCh = 0xD;
	
	FDI_FUNC_ENTER;
	FDI_PRINT(FDI_PRT_ERROR, ("Cmd : %s, length: %d \n", pucCmd, strlen(pucCmd)));

	ulSize = strlen(pucCmd);
	while((ucRetryCount > 0) && (ulSize > 0))
	{
		DI_MODEM_Write(pucCmd, ulSize, &unActualWrittenSize);
		if(ulSize == unActualWrittenSize)
		{
			ulSize = 0;
			break;
		}
		
		FVK_TASK_Sleep(10);
		ulSize = ulSize - unActualWrittenSize;
		ucRetryCount--;
	};

	if(ulSize == 0)
	{
		DI_MODEM_Write(&ucEndCh, 1, &unActualWrittenSize);
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("Error Write Command, remainSize: %d \n", ulSize));
		eFdiError = FDI_ERR;
	}
	
	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MODEM_RxResponse(unsigned char *pucBuf, unsigned int *punSize, unsigned int unTimeout)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;

	eFdiError = INTRN_FDI_MODEM_RxResponse(pucBuf, punSize, unTimeout);

	return eFdiError;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void FDI_MODEM_RegisterCallback(pfnFDI_MODEM_Notify pfnFdiCallback)
{
	s_pfnFdiModemCallback = pfnFdiCallback;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void FDI_MODEM_FlushRxBuffer(void)
{
	s_unRxBuffReadPtr = s_unRxBuffWritePtr = 0;
}

/*******************************************************************************
* function : INTRN_FDI_MODEM_Callback
* description : DI/DDI로부터 response를 수신하거나, change state의 결과를 얻기위해 등록.
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_FDI_MODEM_Callback(unsigned char eModemEvent, unsigned long ulParam)
{
	unsigned int	unReadPtr = 0;
	unsigned int	unRequestSize = 0;
	unsigned int	unActualReadSize = 0;
	unsigned char	ucBuffer[100];
	unsigned int	unCurWritePtr = 0;
	unsigned char	unIndex =0;
	
	if(eModemEvent == DI_MODEM_RX_MSG)
	{
		DI_MODEM_Read(ucBuffer, ulParam, &unActualReadSize);	
		
		FVK_SEM_Get(s_ulFdiModemSema4);
		for(unIndex=0; unIndex<unActualReadSize; unIndex++)
		{
			s_ModemRxBuff[s_unRxBuffWritePtr] = ucBuffer[unIndex];
			s_unRxBuffWritePtr++;
			if(s_unRxBuffWritePtr == FDI_MODEM_RX_BUFF_SIZE)
			{
				s_unRxBuffWritePtr = 0;
			}
		}
		FVK_SEM_Release(s_ulFdiModemSema4);
		
		if(s_pfnFdiModemCallback != NULL)
		{
			s_pfnFdiModemCallback(unActualReadSize);
		}
	}
}
/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FDI_ERR_CODE INTRN_FDI_MODEM_RxResponse(unsigned char *pucBuf, unsigned int *unSize, unsigned int unTimeout)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	unsigned int	unCurReadPtr = 0;
	unsigned int	unCurWritePtr = 0;
	unsigned int	unEndReadPtr = 0;
	unsigned int	i = 0;
	
	FDI_FUNC_ENTER;

	*unSize = 0;
	FVK_TASK_Sleep(unTimeout);

	FVK_SEM_Get(s_ulFdiModemSema4);
	unEndReadPtr = unCurReadPtr = s_unRxBuffReadPtr;
	unCurWritePtr = s_unRxBuffWritePtr;
	FVK_SEM_Release(s_ulFdiModemSema4);
	
	while((unEndReadPtr < unCurWritePtr) && (s_ModemRxBuff[unEndReadPtr] != '\0'))
	{
		pucBuf[i] = s_ModemRxBuff[unEndReadPtr];
		unEndReadPtr++;
		if(FDI_MODEM_RX_BUFF_SIZE <= unEndReadPtr)
		{
			unEndReadPtr = 0;
		}
		i++;
	}
	pucBuf[i] = '\0';
	*unSize = i;
	
	FDI_PRINT(FDI_PRT_DBGINFO, ("\n[INTRN_FDI_MODEM_RxResponse] : string: %s length :%d\n", pucBuf, *unSize));

	FVK_SEM_Get(s_ulFdiModemSema4);
	s_unRxBuffReadPtr = unEndReadPtr; 
	FVK_SEM_Release(s_ulFdiModemSema4);
	
done:
	FDI_FUNC_EXIT;
	return eFdiError;
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FDI_ERR_CODE INTRN_FDI_MODEM_FindString(char *pcSrcString, char *pcFindString)
{
	int	nSrcLength = 0;
	int	nFindStrLength = 0;
	int	nIndexI = 0;
	int	nIndexJ = 0;
	
	if( (pcSrcString == NULL) || (pcFindString == NULL) )
	{
		return FDI_ERR_INVALID_PARAMETER;
	}
	
	nSrcLength = strlen(pcSrcString);
	nFindStrLength = strlen(pcFindString);
	for(nIndexI = 0; nIndexI <= (nSrcLength - nFindStrLength); nIndexI++)
	{
		for(nIndexJ = 0; nIndexJ < nFindStrLength; nIndexJ++)
		{
			if (pcFindString[nIndexJ] != pcFindString[nIndexI + nIndexJ])
			{
				break;
			}
		}
		
		if(nIndexJ == nFindStrLength)
		{
			return FDI_NO_ERR;
		}
	}
	return FDI_ERR;;
}


