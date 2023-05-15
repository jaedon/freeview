/*******************************************************************************
* File name : di_modem.c
* Author : 
* description :
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */
#include "bstd.h"
#include "nexus_base_types.h"
#include "nexus_platform.h"
#include "nexus_uart.h"

/* humax header files */
#include "linden_trace.h"
#include "taskdef.h"
#include "vkernel.h"
#include "drv_err.h"
#include "drv_gpio.h"
#include "di_err.h"
#include "di_modem.h"


/*******************************************************************************
* Debug level
*******************************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DRV_MISC


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define DRV_MODEM_RX_PARAM	0
#define DRV_MODEM_TX_PARAM	1
#define DRV_MODEM_TRNS_BUFF_SIZE	(3072 + 1)
#define DRV_MODEM_TX_TIMEOUT	(5*1000)
#define DRV_MODEM_QUEUE_COUNT	1024
#define DRV_MODEM_STACK_SIZE	1024

typedef struct
{
	HUINT32	ulTxBufferSize;
	HUINT32	ulUsedSize;
}DRV_MODEM_TXBUFF_t;


/*******************************************************************************
* Global variables and structures
*******************************************************************************/
HUINT8	g_ucRxStatus = 1;

/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static HULONG	s_ulModemMsg = 0;
static HULONG	s_ulModemSema4 = 0;
static NEXUS_UartHandle	s_pstUartHandle = NULL;
static NEXUS_UartStatus	s_stUartStatus;
static DRV_MODEM_TXBUFF_t	s_stTxBuffStatus;
static pfnDI_MODEM_Notify	s_pfnModemCallback = NULL;

/*******************************************************************************
* Static function prototypes
*******************************************************************************/
static void DRV_MODEM_P_ProcessTask(void *pvArg);
static void DRV_MODEM_P_Callback(void *pContext, HUINT32 ulParam);
static void DRV_MODEM_P_GetDCD_Callback(void *context, int param);


/*******************************************************************************
* function : DI_MODEM_Read
* description : read received data from modem
* input : pucData(data buffer), ulDataSize
* output : pucActualReadSize
* return : DI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_MODEM_Read(HUINT8 *pucData, HUINT32 ulDataSize, HUINT32 *pulActualReadSize)
{
	DI_ERR_CODE	eDiError = DRV_OK;
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;
	HUINT32	ulReadNum = 0;
	
	PrintEnter();

	if((pucData == NULL) || (ulDataSize == 0))
	{
    	PrintError(" Invalid Parameter, pucData: 0x%x ulDataSize: %d !!\n", pucData, ulDataSize);
		eDiError = DI_ERR_INVALID_PARAM;
		goto done;
	}
    *pulActualReadSize = 0;

    eNexusError = NEXUS_Uart_Read(s_pstUartHandle, pucData, ulDataSize, &ulReadNum);
    if(eNexusError != NEXUS_SUCCESS)
    {
    	PrintError(" NEXUS_Uart_Read error, eNexusError: %d\n", eNexusError);
		eDiError = DI_ERR_ERROR;
        goto done;
    }
    *pulActualReadSize = ulReadNum;

done:
	PrintExit();
	return eDiError;
}


/*******************************************************************************
* function : DI_MODEM_Write
* description : 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_MODEM_Write(HUINT8 *pucData, HUINT32 ulDataSize, HUINT32 *pulActualWriteSize)
{
	DI_ERR_CODE	eDiError = DRV_OK;
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;
	BERR_Code	eBcmError = BERR_SUCCESS;
	HUINT32	ulWriteNum = 0;
	
	PrintEnter();

	*pulActualWriteSize = 0;
	if((pucData == NULL) || (ulDataSize == 0))
	{
    	PrintError(" Invalid Parameter, pucData: 0x%x ulDataSize: %d !!\n", pucData, ulDataSize);
		eDiError = DI_ERR_INVALID_PARAM;
		goto done;
	}

	VK_SEM_Get(s_ulModemSema4);
	if(s_stTxBuffStatus.ulUsedSize >= s_stTxBuffStatus.ulTxBufferSize)
	{
    	PrintError(" Full TX Buffer !!\n");
		goto done;
	}
	
    eNexusError = NEXUS_Uart_Write(s_pstUartHandle, (void *)pucData, ulDataSize, &ulWriteNum);
    if(eNexusError != NEXUS_SUCCESS)
	{
    	PrintError(" eNexusError: 0x%x !!\n", eNexusError);
		eDiError = DI_ERR_ERROR;
        goto done;
	}
    *pulActualWriteSize = ulWriteNum;
	s_stTxBuffStatus.ulUsedSize = s_stTxBuffStatus.ulUsedSize + ulWriteNum;
	
done:
	VK_SEM_Release(s_ulModemSema4);
	PrintExit();
	return eDiError;
}


/*******************************************************************************
* function : DI_MODEM_Reset
* description : reset modem 
* input : 
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_MODEM_Reset(void)
{
	DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_LOW);
	VK_TASK_Sleep(100);
	DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_HI);
	VK_TASK_Sleep(50);
}

/*******************************************************************************
* function : DI_Modem_Disconnect
* description : make disconnection 
* input : 
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_MODEM_Disconnect(void)
{
	DRV_GPIO_Write(GPIO_ID_MODEM_DTR, GPIO_HI);
	VK_TASK_Sleep(300);
	DRV_GPIO_Write(GPIO_ID_MODEM_DTR, GPIO_LOW);
}

/*******************************************************************************
* function : DI_MODEM_CheckDCD
* description : check dcd from mw
* input : none
* output : pucDcdState
* return : DI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_MODEM_CheckDCD(HUINT8 *pucDcdState)
{
	DRV_Error	eDrvError = DRV_OK;
	GPIO_STATE_t eGpioState = GPIO_LOW;

	eDrvError = DRV_GPIO_Read(GPIO_ID_MODEM_DCD, &eGpioState);
	if(eDrvError != DRV_OK)
	{
    	PrintError(" GPIO(MODEM_DCD) read error!!!, eDrvError: 0x%x !!\n", eDrvError);
		return;
	}

	*pucDcdState = (HUINT8)eGpioState;
}

/********************************************************************
Function   : DI_MODEM_RegisterRxCallback
Description : callback function for Receive data from Modem or DCD status
Input      : none
Output     : none
Return	 : DDI_MICOM_OK

********************************************************************/
void DI_MODEM_RegisterRxCallback(pfnDI_MODEM_Notify pfnNotify)
{
	s_pfnModemCallback = (pfnDI_MODEM_Notify)pfnNotify;
}

/*******************************************************************************
* function : DRV_MODEM_Init
* description : create task for data processing and set default value
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_MODEM_Init(void)
{
	DRV_Error	eDrvError = DRV_OK;
	HINT32	nVkError = 0;
	NEXUS_UartSettings	tUartSetting;
	HULONG	ulModemTaskID = 0;
	HUINT32	unTaskID = 0;

	
	PrintEnter();
	
	nVkError = VK_SEM_Create(&s_ulModemSema4, "ModemTxMsg", VK_SUSPENDTYPE_FIFO);
	if(nVkError != VK_OK)
	{
		PrintError(" ModemMsg Create Error !!\n");
		return DRV_ERR;
	}
	nVkError = VK_MSG_Create(DRV_MODEM_QUEUE_COUNT, 4, "ModemTxMsg", &s_ulModemMsg, VK_SUSPENDTYPE_FIFO);
	if(nVkError != VK_OK)
	{
		PrintError(" ModemMsg Create Error !!\n");
		return DRV_ERR;
	}
	nVkError = VK_TASK_Create(DRV_MODEM_P_ProcessTask, USER_PRIORITY6, DRV_MODEM_STACK_SIZE, "DiModemTask", (void *)unTaskID, &ulModemTaskID, FALSE);
	if(nVkError != VK_OK)
	{
		PrintError(" Task Create Error !!\n");
		return DRV_ERR;
	}
	VK_TASK_Start(ulModemTaskID);
	
	/* set DTR low */
	eDrvError = DRV_GPIO_Write(GPIO_ID_MODEM_DTR, GPIO_LOW);
	if(eDrvError != DRV_OK)
	{
    	PrintError(" DRV_GPIO_Write error!!!, eDrvError: 0x%x !!\n", eDrvError);
	}
	
	/* register DCD line status interrupt */
	eDrvError = DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_MODEM_DCD, DRV_MODEM_P_GetDCD_Callback, NULL, 0);
	if(eDrvError != DRV_OK)
	{
		eDrvError = DRV_ERR;
		goto done;
	}

	eDrvError = DRV_GPIO_INTR_Enable(GPIO_ID_MODEM_DCD);
	if(eDrvError != DRV_OK)
	{
		eDrvError = DRV_ERR;
		goto done;
	}

	NEXUS_Uart_GetDefaultSettings(&tUartSetting);
	tUartSetting.txBufferSize = DRV_MODEM_TRNS_BUFF_SIZE;
	tUartSetting.rxBufferSize = DRV_MODEM_TRNS_BUFF_SIZE;
	tUartSetting.baudRate = 9600;
	tUartSetting.dataBits = NEXUS_UartDataBits_e8;
	tUartSetting.parity = NEXUS_UartParity_eNone;
	tUartSetting.stopBits = NEXUS_UartStopBits_e1;
	tUartSetting.rxReady.callback = DRV_MODEM_P_Callback;
	tUartSetting.rxReady.context = NULL;
	tUartSetting.rxReady.param = DRV_MODEM_RX_PARAM;
	tUartSetting.txDone.callback = DRV_MODEM_P_Callback;
	tUartSetting.txDone.context = NULL;
	tUartSetting.txDone.param = DRV_MODEM_TX_PARAM;
	s_pstUartHandle = NEXUS_Uart_Open(1, &tUartSetting);
	if(s_pstUartHandle == NULL)
	{
		eDrvError = DRV_ERR;
		goto done;
	}
	VK_memset(&s_stUartStatus, 0x00, sizeof(NEXUS_UartStatus));
	s_stTxBuffStatus.ulTxBufferSize = DRV_MODEM_TRNS_BUFF_SIZE;
	s_stTxBuffStatus.ulUsedSize = 0;
	
done:
	PrintExit();
	return eDrvError;
}


/*******************************************************************************
* function : DRV_MODEM_P_ProcessTask
* description : data 수신 시 callback을 부른다. 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_MODEM_P_ProcessTask(void *pvArg)
{
	HINT32	nVkError = VK_OK;
	HUINT8	ucMsg[2];
	HUINT8	ucIndex = 0;
	HUINT8	ucBuff = 0;
	HUINT32	ucDataNum = 0;
	HUINT32	ulReadNum = 0;
	NEXUS_UartStatus	tUartStatus;

	BSTD_UNUSED(pvArg);

	while(1)
	{
		nVkError = VK_MSG_Receive(s_ulModemMsg, ucMsg, sizeof(ucMsg));
		if(nVkError != VK_OK)
		{
	    	PrintError(" ReceiveTimeout: nVkError: 0x%x !!\n", nVkError);
	    	continue;
		}
		
	    NEXUS_Uart_GetStatus(s_pstUartHandle, &tUartStatus);

		PrintDebug("ulParam: %d, rx: %d  tx: %d\n", ucMsg[0], tUartStatus.rxBytes, tUartStatus.txBytes);

		if(ucMsg[0] == DRV_MODEM_RX_PARAM)
		{	
			if(s_stUartStatus.rxFrameError != tUartStatus.rxFrameError)
			{
				PrintError("rxFrameError- previous[%d] current[%d]\n", s_stUartStatus.rxFrameError, tUartStatus.rxFrameError);
				s_stUartStatus.rxFrameError = tUartStatus.rxFrameError;
			}
			if(s_stUartStatus.rxOverrun != tUartStatus.rxOverrun)
			{
				PrintError("rxOverrun- previous[%d] current[%d]\n", s_stUartStatus.rxOverrun, tUartStatus.rxOverrun);
				s_stUartStatus.rxOverrun = tUartStatus.rxOverrun;
			}
			if(s_stUartStatus.rxParityError != tUartStatus.rxParityError)
			{
				PrintError("rxParityError- previous[%d] current[%d]\n", s_stUartStatus.rxParityError, tUartStatus.rxParityError);
				s_stUartStatus.rxParityError = tUartStatus.rxParityError;
			}
			if(tUartStatus.rxBytes >= s_stUartStatus.rxBytes)
			{
				ulReadNum = tUartStatus.rxBytes - s_stUartStatus.rxBytes;
			}
			else
			{
				ulReadNum = 0xFFFFFFFF - s_stUartStatus.rxBytes + 1 + tUartStatus.rxBytes;
			}

			if(ulReadNum > 0)
			{
				if(s_pfnModemCallback != NULL)
				{
					s_pfnModemCallback(DI_MODEM_RX_MSG, ulReadNum);
				}
				else
				{
					for(ucIndex=0; ucIndex<ulReadNum; ucIndex++)
					{
				    	NEXUS_Uart_Read(s_pstUartHandle, &ucBuff, 1, &ucDataNum);
				    }
				}
			}
			s_stUartStatus.rxBytes = tUartStatus.rxBytes;
		}
		else if(ucMsg[0] == DRV_MODEM_TX_PARAM)
		{
			ulReadNum = tUartStatus.txBytes - s_stUartStatus.txBytes;
			s_stUartStatus.txBytes = tUartStatus.txBytes;

			VK_SEM_Get(s_ulModemSema4);
			s_stTxBuffStatus.ulUsedSize = s_stTxBuffStatus.ulUsedSize - ulReadNum;
			VK_SEM_Release(s_ulModemSema4);
		}
	}
}

/*******************************************************************************
* function : DRV_MODEM_P_RxCallback_ISR
* description : data 수신 시 callback을 부른다. 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_MODEM_P_Callback(void *pContext, HUINT32 ulParam)
{
	HINT32	nVkError = VK_OK;
	HUINT8	ucMsg[2];
	
	BSTD_UNUSED(pContext);
	
	PrintEnter();
	ucMsg[0] = ulParam;
	nVkError = VK_MSG_Send(s_ulModemMsg, ucMsg, 2);
	if(nVkError != VK_OK)
	{
		PrintError("Send Message Error; %d\n", nVkError);
	}
	PrintExit();
}

/*******************************************************************************
* function : DRV_MODEM_P_GetDCDCallback_ISR
* description : check DCD status 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_MODEM_P_GetDCD_Callback(void *context, int param)
{
	DRV_Error	eDrvError = DRV_OK;
	GPIO_STATE_t	eGpioState = GPIO_LOW;
	
	BSTD_UNUSED(context);
	BSTD_UNUSED(param);

	eDrvError = DRV_GPIO_Read(GPIO_ID_MODEM_DCD, &eGpioState);
	if(eDrvError != DRV_OK)
	{
    	PrintError(" DRV_GPIO_Read error, eDrvError: 0x%x !!\n", eDrvError);
		return;
	}	
	
	if(s_pfnModemCallback != NULL)
	{
		s_pfnModemCallback(DI_MODEM_GET_DCD, (HUINT32)eGpioState);
	}
}

/* end of file */


