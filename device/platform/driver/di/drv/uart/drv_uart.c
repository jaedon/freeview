/**
 * drv_uart.c
*/

/**
 * @defgroup		DRV_UART DRV_UART : DRV_UART module
 * @author		Jin-hong Kim
 * @note			DRV_UART APIs
 * @brief			Define broadcom dependent DRV_UART
 * @file			drv_uart.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "linden_trace.h"

/* include  drv headers */
#include "drv_uart.h"
#if !defined(CONFIG_OS_UCOS)
#include <termio.h>
#include <unistd.h>
#endif

#if defined(CONFIG_BCM_UART)
/* chip manufacture's  header files */
#include "nexus_base_types.h"
#include "nexus_uart.h"
#include "berr.h"
#include "bkni.h"
#include "bkni_multi.h"

/* humax header files */
#include "taskdef.h"
#include "vkernel.h"
#include "drv_err.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_BCM_UART)
#define DEBUG_MODULE						TRACE_MODULE_DRV_UART

#define DRV_UART_MAX_PORTNUMBER				3
// semaphore
#define DRV_UART_SEMAPHORE_NAME				"UartSemaR0"
#define DRV_UART_SEMAPHORE_NAME_LENGTH		sizeof(DRV_UART_SEMAPHORE_NAME)

#define DRV_UART_QUEUE_COUNT	2048
#define DRV_UART_STACK_SIZE	1024
#endif


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
#if defined(CONFIG_BCM_UART)
typedef struct
{
	HULONG ulReadSemaphore;
	HULONG ulWriteSemaphore;
	HUINT32 rxOverrun;
	HUINT32 rxFrameError;
	HUINT32 rxParityError;
    HUINT32				ulRxBytes;
    HUINT32				ulTxBytes;
	NEXUS_UartHandle ptHandle;
	NEXUS_UartSettings tSetting;
	DRV_UART_CALLBACK 	pfUartCallback[DRV_UART_EVENT_END];
}DRV_UART_INFO_t;

typedef struct
{
	HUINT8	param;
	BKNI_EventHandle ptEvent;
}DRV_UART_TASK_MSG;

#endif


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
#if defined(CONFIG_BCM_UART)
static HINT8 s_aucString[DRV_UART_SEMAPHORE_NAME_LENGTH] = DRV_UART_SEMAPHORE_NAME;
static DRV_UART_INFO_t s_aUartInfo[DRV_UART_MAX_PORTNUMBER];
static HULONG	s_ulUartMsg = 0;
#endif


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_BCM_UART)
static void P_DRV_UART_Callback(void *context, int param);
#endif

#if !defined(CONFIG_OS_UCOS)
DRV_Error DRV_UART_ReadByteNoWaitForViaccessGetc(HUINT8 *data)
{
	char ch=0, read_num=0;

	struct termios buf;
	struct termios save;

	tcgetattr(STDIN_FILENO, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 0;
	buf.c_cc[VTIME] = 0;

	//tcsetattr(0, TCSAFLUSH, &buf);
	tcsetattr(STDIN_FILENO, TCSANOW, &buf);

	//ch = VK_getchar();
	read_num = read(STDIN_FILENO, &ch, 1);

	//tcsetattr(0, TCSAFLUSH, &save);
	tcsetattr(STDIN_FILENO, TCSANOW, &save);

	if(read_num == 1){
		*data = (unsigned char)ch;

		return DRV_OK;
	}

	*data = 0;

	return DRV_ERR;
}
#endif

#if defined(CONFIG_BCM_UART)
/*******************************************************************************
* function : P_DRV_UART_Callback
* description : Uart Interrupt 발생시 호출되는 Callback Function.
* input : context(이벤트 전달용 context), param(사용하지 않음)
* output : None
* return : None
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_DRV_UART_Callback(void *pContext, int param)
{
	HINT32	nVkError = VK_OK;
	DRV_UART_TASK_MSG	sMsg;

	PrintEnter();

	sMsg.param 		= param;
	sMsg.ptEvent 	= pContext;
	nVkError = VK_MSG_SendTimeout(s_ulUartMsg, &sMsg, sizeof(DRV_UART_TASK_MSG),0);
	if(nVkError != VK_OK)
	{
		PrintError(" P_DRV_UART_Callback Send Message Error; %d\n", nVkError);
	}

	PrintExit();
}
/*******************************************************************************
* function : DRV_UART_P_ProcessTask
* description : uart write/read 시 callback을 부른다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_UART_P_ProcessTask(void *pvArg)
{
	HINT32	nVkError = VK_OK;
	DRV_UART_TASK_MSG	sMsg;
	HUINT32	ulReadNum = 0;
	HUINT32 ulPortNumber = 0;
	HUINT32 ulUartEventTmp	= 0;
	HUINT32 ulUartEvent	= 0;
	NEXUS_UartStatus	tUartStatus;

	BSTD_UNUSED(pvArg);

	while(1)
	{
		nVkError = VK_MSG_Receive(s_ulUartMsg, &sMsg, sizeof(DRV_UART_TASK_MSG));
		if(nVkError != VK_OK)
		{
			PrintError(" ReceiveTimeout: nVkError: 0x%x !!\n", nVkError);
			continue;
		}

		BSTD_UNUSED(sMsg.ptEvent);
		BKNI_SetEvent(sMsg.ptEvent);

		ulPortNumber = sMsg.param/10;
		ulUartEventTmp = sMsg.param - (ulPortNumber*10);
		if(ulUartEventTmp == DRV_UART_EVENT_TX)
		{
			ulUartEvent = DRV_UART_EVENT_TX;
		}
		else if(ulUartEventTmp == DRV_UART_EVENT_RX)
		{
			ulUartEvent = DRV_UART_EVENT_RX;
		}
		else
		{
			PrintError(" Err]Check UartEvent 0x%x \n",sMsg.param);
			continue;
		}

		PrintDebug("DRV_UART_P_ProcessTask PortNumber : %d  UartEvent %d\n",ulPortNumber,ulUartEvent);

		NEXUS_Uart_GetStatus(s_aUartInfo[ulPortNumber].ptHandle, &tUartStatus);

		PrintDebug("ulParam: %d, rx: %d	tx: %d\n", sMsg.param, tUartStatus.rxBytes, tUartStatus.txBytes);

		if(ulUartEvent == DRV_UART_EVENT_RX)
		{
			if(s_aUartInfo[ulPortNumber].rxFrameError != tUartStatus.rxFrameError)
			{
				PrintError("rxFrameError- previous[%d] current[%d]\n", s_aUartInfo[ulPortNumber].rxFrameError, tUartStatus.rxFrameError);
				s_aUartInfo[ulPortNumber].rxFrameError = tUartStatus.rxFrameError;
				if(s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX_ERROR] != NULL)
				{
					s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX](DRV_UART_EVENT_RX_ERROR, 0);
				}
			}
			if(s_aUartInfo[ulPortNumber].rxOverrun != tUartStatus.rxOverrun)
			{
				PrintError("rxOverrun- previous[%d] current[%d]\n", s_aUartInfo[ulPortNumber].rxOverrun, tUartStatus.rxOverrun);
				s_aUartInfo[ulPortNumber].rxOverrun = tUartStatus.rxOverrun;
				if(s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX_ERROR] != NULL)
				{
					s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX](DRV_UART_EVENT_RX_ERROR, 0);
				}
			}
			if(s_aUartInfo[ulPortNumber].rxParityError != tUartStatus.rxParityError)
			{
				PrintError("rxParityError- previous[%d] current[%d]\n", s_aUartInfo[ulPortNumber].rxParityError, tUartStatus.rxParityError);
				s_aUartInfo[ulPortNumber].rxParityError = tUartStatus.rxParityError;
				if(s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX_ERROR] != NULL)
				{
					s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX](DRV_UART_EVENT_RX_ERROR, 0);
				}
			}

			if(tUartStatus.rxBytes >= s_aUartInfo[ulPortNumber].ulRxBytes)
			{
			ulReadNum = tUartStatus.rxBytes - s_aUartInfo[ulPortNumber].ulRxBytes;
			}
			else
			{
				ulReadNum = 0xFFFFFFFF - s_aUartInfo[ulPortNumber].ulRxBytes + 1 + tUartStatus.rxBytes;
			}

			if(ulReadNum > 0)
			{
				if(s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX] != NULL)
				{
					s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_RX](DRV_UART_EVENT_RX, ulReadNum);
				}
			}
			s_aUartInfo[ulPortNumber].ulRxBytes = tUartStatus.rxBytes;
		}
		else if(ulUartEvent == DRV_UART_EVENT_TX)
		{
			ulReadNum = tUartStatus.txBytes - s_aUartInfo[ulPortNumber].ulTxBytes;
			if(tUartStatus.txBytes >= s_aUartInfo[ulPortNumber].ulTxBytes)
			{
				ulReadNum = tUartStatus.txBytes - s_aUartInfo[ulPortNumber].ulTxBytes;
			}
			else
			{
				ulReadNum = 0xFFFFFFFF - s_aUartInfo[ulPortNumber].ulTxBytes + 1 + tUartStatus.txBytes;
			}

			if(ulReadNum > 0)
			{
				if(s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_TX] != NULL)
				{
					s_aUartInfo[ulPortNumber].pfUartCallback[DRV_UART_EVENT_TX](DRV_UART_EVENT_TX, ulReadNum);
				}
			}

			s_aUartInfo[ulPortNumber].ulTxBytes = tUartStatus.txBytes;
		}
	}

}

/*******************************************************************************
* function : DRV_UART_Init
* description : Uart 구조체 초기화하는 함수
* input : None
* output : None
* return : DRV_Error Value
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_UART_Init(void)
{
	HUINT32 ulIndex;
	HINT32	nVkError = 0;
	HULONG	ulUartTaskID = 0;
	HUINT32	unTaskID = 0;

	PrintEnter();

	nVkError = VK_MSG_Create(DRV_UART_QUEUE_COUNT, sizeof(DRV_UART_TASK_MSG), "UartCallbackMsg", &s_ulUartMsg, VK_SUSPENDTYPE_FIFO);
	if(nVkError != VK_OK)
	{
		PrintError(" UartCallbackMsg Create Error !!\n");
		return DRV_ERR;
	}
	nVkError = VK_TASK_Create(DRV_UART_P_ProcessTask, USER_PRIORITY6, DRV_UART_STACK_SIZE, "DiUARTTask", (void *)unTaskID, &ulUartTaskID, FALSE);
	if(nVkError != VK_OK)
	{
		PrintError(" Task Create Error !!\n");
		return DRV_ERR;
	}
	VK_TASK_Start(ulUartTaskID);

	for(ulIndex = 0; ulIndex < DRV_UART_MAX_PORTNUMBER; ulIndex++)
	{
		s_aUartInfo[ulIndex].ptHandle = NULL;
		NEXUS_Uart_GetDefaultSettings(&(s_aUartInfo[ulIndex].tSetting));
	}

	PrintExit();
	return DRV_OK;
}


/*******************************************************************************
* function : DRV_UART_Open
* description : Uart Port를 사용가능하도록 Open하는 함수
* input : ulPortNumber(사용하고자하는 Port Number)
* output : None
* return : DRV_Error Value
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_UART_Open(HUINT32 ulPortNumber)
{
	HINT32 lVKError;
	DRV_Error eDRVError;
	BERR_Code unBERR;
	BKNI_EventHandle ptEvent;

	PrintEnter();
	eDRVError = DRV_OK;
	if(ulPortNumber >= DRV_UART_MAX_PORTNUMBER)
	{
		PrintError("Uart Port Number is invalid, Max Port Number = %d, ulPortNumber = %d\n", DRV_UART_MAX_PORTNUMBER, ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(s_aUartInfo[ulPortNumber].ptHandle != NULL)
	{
		PrintError("Uart Port[%d] is opened already\n", ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	s_aucString[DRV_UART_SEMAPHORE_NAME_LENGTH - 2] = '0' + ulPortNumber;
	s_aucString[DRV_UART_SEMAPHORE_NAME_LENGTH - 3] = 'R';
	lVKError = VK_SEM_Create(&(s_aUartInfo[ulPortNumber].ulReadSemaphore), (char *)s_aucString, VK_SUSPENDTYPE_FIFO);
	if(lVKError != VK_OK)
	{
		PrintError("Uart Read Semaphore[%d] Create Error\n", ulPortNumber);
		eDRVError = DRV_ERR;
		goto done;
	}
	s_aucString[DRV_UART_SEMAPHORE_NAME_LENGTH - 2] = '0' + ulPortNumber;
	s_aucString[DRV_UART_SEMAPHORE_NAME_LENGTH - 3] = 'W';
	lVKError = VK_SEM_Create(&(s_aUartInfo[ulPortNumber].ulWriteSemaphore), (char *)s_aucString, VK_SUSPENDTYPE_FIFO);
	if(lVKError != VK_OK)
	{
		PrintError("Uart Write Semaphore[%d] Create Error\n", ulPortNumber);
		eDRVError = DRV_ERR;
		goto done;
	}

    unBERR = BKNI_CreateEvent(&ptEvent);
	if(unBERR != BERR_SUCCESS)
	{
		PrintError("BKNI_CreateEvent Error(RX) %d\n", unBERR);
		eDRVError = DRV_ERR;
		goto done;
	}
	s_aUartInfo[ulPortNumber].tSetting.rxReady.callback = P_DRV_UART_Callback;
	s_aUartInfo[ulPortNumber].tSetting.rxReady.context = ptEvent;
	s_aUartInfo[ulPortNumber].tSetting.rxReady.param = (ulPortNumber*10)+(DRV_UART_EVENT_RX);
    unBERR = BKNI_CreateEvent(&ptEvent);
	if(unBERR != BERR_SUCCESS)
	{
		PrintError("BKNI_CreateEvent Error(TX) %d\n", unBERR);
		eDRVError = DRV_ERR;
		goto done;
	}
	s_aUartInfo[ulPortNumber].tSetting.txDone.callback = P_DRV_UART_Callback;
	s_aUartInfo[ulPortNumber].tSetting.txDone.context = ptEvent;
	s_aUartInfo[ulPortNumber].tSetting.txDone.param = (ulPortNumber*10)+(DRV_UART_EVENT_TX);

	s_aUartInfo[ulPortNumber].ptHandle = NEXUS_Uart_Open(ulPortNumber, &(s_aUartInfo[ulPortNumber].tSetting));
	if(s_aUartInfo[ulPortNumber].ptHandle == NULL)
	{
		PrintError("Uart Open Error\n");
		eDRVError = DRV_ERR;
		goto done;
	}

done:
	PrintExit();
	return eDRVError;
}


/*******************************************************************************
* function : DRV_UART_GetStatus
* description : Uart Port의 상태(Opened or Closed)를 얻어오는 함수
* input : ulPortNumber(상태를 알고자 하는 Port Number)
* output : peStatus(상태를 저장할 포인터)
* return : DRV_Error Value
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_UART_GetStatus(HUINT32 ulPortNumber, DRV_UARTSTATUS_e *peStatus)
{
	DRV_Error eDRVError;

	PrintEnter();
	eDRVError = DRV_OK;
	if(ulPortNumber >= DRV_UART_MAX_PORTNUMBER)
	{
		PrintError("Uart GetStatus : Port Number is invalid, Max Port Number = %d, ulPortNumber = %d\n", DRV_UART_MAX_PORTNUMBER, ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(s_aUartInfo[ulPortNumber].ptHandle == NULL)
	{
		*peStatus = DRV_UARTSTATUS_CLOSED;
	}
	else
	{
		*peStatus = DRV_UARTSTATUS_OPENED;
	}

done:
	PrintExit();
	return eDRVError;
}


/*******************************************************************************
* function : DRV_UART_SetParam
* description : Uart Port를 설정(buffer size, baud rate, parity, data bits, stop bits)
* input : ulPortNumber(사용하고자하는 Port Number), ptParam(설정값의 구조체 포인터)
* output : None
* return : DRV_Error Value
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_UART_SetParam(HUINT32 ulPortNumber, DRV_UARTPARAM_t *ptParam)
{
	DRV_Error eDRVError;

	PrintEnter();
	eDRVError = DRV_OK;
	if(ulPortNumber >= DRV_UART_MAX_PORTNUMBER)
	{
		PrintError("Uart SetParam : Port Number is invalid, Max Port Number = %d, ulPortNumber = %d\n", DRV_UART_MAX_PORTNUMBER, ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}
	if(ptParam->eParity >= DRV_UARTPARITY_MAX)
	{
		PrintError("Uart SetParam : Parity is invalid, Max Parity = %d, eParity = %d\n", DRV_UARTPARITY_MAX, ptParam->eParity);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}
	if(ptParam->eDataBits >= DRV_UARTDATABITS_MAX)
	{
		PrintError("Uart SetParam : DataBits is invalid, Max DataBits = %d, eDataBits = %d\n", DRV_UARTDATABITS_MAX, ptParam->eDataBits);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}
	if(ptParam->eStopBits >= DRV_UARTSTOPBITS_MAX)
	{
		PrintError("Uart SetParam : StopBits is invalid, Max StopBits = %d, eStopBits = %d\n", DRV_UARTSTOPBITS_MAX, ptParam->eStopBits);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	s_aUartInfo[ulPortNumber].tSetting.txBufferSize = ptParam->ulTXBufferSize;
	s_aUartInfo[ulPortNumber].tSetting.rxBufferSize = ptParam->ulRXBufferSize;
	s_aUartInfo[ulPortNumber].tSetting.baudRate = ptParam->ulBaudRate;
	switch(ptParam->eParity)
	{
		case DRV_UARTPARITY_NONE :
			s_aUartInfo[ulPortNumber].tSetting.parity = NEXUS_UartParity_eNone;
			break;

		case DRV_UARTPARITY_ODD :
			s_aUartInfo[ulPortNumber].tSetting.parity = NEXUS_UartParity_eOdd;
			break;

		case DRV_UARTPARITY_EVEN :
		default :
			s_aUartInfo[ulPortNumber].tSetting.parity = NEXUS_UartParity_eEven;
			break;
	}
	switch(ptParam->eDataBits)
	{
		case DRV_UARTDATABITS_7 :
			s_aUartInfo[ulPortNumber].tSetting.dataBits = NEXUS_UartDataBits_e7;
			break;

		case DRV_UARTDATABITS_8 :
		default :
			s_aUartInfo[ulPortNumber].tSetting.dataBits = NEXUS_UartDataBits_e8;
			break;
	}
	switch(ptParam->eStopBits)
	{
		case DRV_UARTSTOPBITS_0 :
			s_aUartInfo[ulPortNumber].tSetting.stopBits = NEXUS_UartStopBits_e0;
			break;

		case DRV_UARTSTOPBITS_1 :
			s_aUartInfo[ulPortNumber].tSetting.stopBits = NEXUS_UartStopBits_e1;
			break;

		case DRV_UARTSTOPBITS_2 :
		default :
			s_aUartInfo[ulPortNumber].tSetting.stopBits = NEXUS_UartStopBits_e2;
			break;
	}

	if(s_aUartInfo[ulPortNumber].ptHandle)
	{
		NEXUS_Uart_SetSettings(s_aUartInfo[ulPortNumber].ptHandle, &(s_aUartInfo[ulPortNumber].tSetting));
	}

done:
	PrintExit();
	return eDRVError;
}


/*******************************************************************************
* function : DRV_UART_Read
* description : 지정된 Uart 포트에서 데이터를 읽음
* input : ulPortNumber(사용할 Uart Port Number), pucData(읽은 데이터를 저장할 pointer), ulDataSize(읽을 데이터 크기),
* output : None
* return : DRV_Error Value
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_UART_Read(HUINT32 ulPortNumber, HUINT8 *pucData, HUINT32 ulDataSize)
{
	HUINT32 ulReadSize;
	DRV_Error eDRVError;
	BERR_Code unBERR;
	NEXUS_Error	eNexusError;

	PrintEnter();
	eDRVError = DRV_OK;
	if(ulPortNumber >= DRV_UART_MAX_PORTNUMBER)
	{
		PrintError("Uart Read : Port Number is invalid, Max Port Number = %d, ulPortNumber = %d\n", DRV_UART_MAX_PORTNUMBER, ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(s_aUartInfo[ulPortNumber].ptHandle == NULL)
	{
		PrintError("Uart Read : Port[%d] is not opened\n", ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(pucData == NULL)
	{
		PrintError("Uart Read : pucData is invalid\n");
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(ulDataSize == 0)
	{
		PrintDebug("Uart Read : ulDataSize is zero\n");
		goto done;
	}

	VK_SEM_Get(s_aUartInfo[ulPortNumber].ulReadSemaphore);
	while(ulDataSize > 0)
	{
		eNexusError = NEXUS_Uart_Read(s_aUartInfo[ulPortNumber].ptHandle, pucData, ulDataSize, &ulReadSize);
		if(eNexusError != NEXUS_SUCCESS)
		{
			PrintError("Uart Read : NEXUS_Uart_Read Error %d\n", eNexusError);
			eDRVError = DRV_ERR;
			VK_SEM_Release(s_aUartInfo[ulPortNumber].ulReadSemaphore);
			goto done;
		}

		if(ulDataSize > ulReadSize)
		{
			ulDataSize -= ulReadSize;
			pucData += ulReadSize;
			unBERR = BKNI_WaitForEvent(s_aUartInfo[ulPortNumber].tSetting.rxReady.context, BKNI_INFINITE);
			if(unBERR != BERR_SUCCESS)
			{
				PrintError("Uart Read : BKNI_WaitForEvent Error %d\n", unBERR);
				eDRVError = DRV_ERR;
				VK_SEM_Release(s_aUartInfo[ulPortNumber].ulReadSemaphore);
				goto done;
			}
		}
		else
		{
			ulDataSize = 0;
		}

	}
	VK_SEM_Release(s_aUartInfo[ulPortNumber].ulReadSemaphore);

done:
	PrintExit();
	return eDRVError;
}


/*******************************************************************************
* function : DRV_UART_Write
* description : 지정된 Uart 포트로 데이터를 출력
* input : ulPortNumber(사용할 Uart Port Number), pucData(출력할 데이터의 pointer), ulDataSize(출력할 데이터 크기),
* output : None
* return : DRV_Error Value
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_UART_Write(HUINT32 ulPortNumber, HUINT8 *pucData, HUINT32 ulDataSize)
{
	HUINT32 ulWriteSize;
	DRV_Error eDRVError;
	BERR_Code unBERR;
	NEXUS_Error	eNexusError;

	PrintEnter();
	eDRVError = DRV_OK;
	if(ulPortNumber >= DRV_UART_MAX_PORTNUMBER)
	{
		PrintError("Uart Write : Uart Port Number is invalid, Max Port Number = %d, ulPortNumber = %d\n", DRV_UART_MAX_PORTNUMBER, ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(s_aUartInfo[ulPortNumber].ptHandle == NULL)
	{
		PrintError("Uart Write : Uart Port[%d] is not opened\n", ulPortNumber);
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(pucData == NULL)
	{
		PrintError("Uart Write : pucData is invalid\n");
		eDRVError = DRV_ERR_INVALID_PARAMETER;
		goto done;
	}

	if(ulDataSize == 0)
	{
		PrintDebug("Uart Write : ulDataSize is zero\n");
		goto done;
	}
	VK_SEM_Get(s_aUartInfo[ulPortNumber].ulWriteSemaphore);
	while(ulDataSize > 0)
	{
		eNexusError = NEXUS_Uart_Write(s_aUartInfo[ulPortNumber].ptHandle, pucData, ulDataSize, &ulWriteSize);

		if(eNexusError != NEXUS_SUCCESS)
		{

			PrintError("Uart Write : NEXUS_Uart_Write Error %d\n", eNexusError);
			eDRVError = DRV_ERR;

			VK_SEM_Release(s_aUartInfo[ulPortNumber].ulWriteSemaphore);
			goto done;
		}

		if(ulDataSize > ulWriteSize)
		{
			ulDataSize -= ulWriteSize;
			pucData += ulWriteSize;
			unBERR = BKNI_WaitForEvent(s_aUartInfo[ulPortNumber].tSetting.txDone.context, BKNI_INFINITE);
			if(unBERR != BERR_SUCCESS)
			{
				PrintError("Uart Write : BKNI_WaitForEvent Error %d\n", unBERR);
				eDRVError = DRV_ERR;
				VK_SEM_Release(s_aUartInfo[ulPortNumber].ulWriteSemaphore);
				goto done;
			}
		}
		else
		{
			ulDataSize = 0;
		}

	}
	VK_SEM_Release(s_aUartInfo[ulPortNumber].ulWriteSemaphore);

done:
	PrintExit();
	return eDRVError;
}

void DRV_UART_RegisterEventNotify(HUINT32 ulPortNumber,DRV_UART_EVENT eUartEvent, DRV_UART_CALLBACK pfUARTEventCallback)
{
	if(eUartEvent < DRV_UART_EVENT_END )
	{
		s_aUartInfo[ulPortNumber].pfUartCallback[eUartEvent]= pfUARTEventCallback;
	}
	return;
}

#endif

