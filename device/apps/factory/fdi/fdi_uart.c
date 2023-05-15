/*******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Uart Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#if !defined(CONFIG_OS_RTOS)
#include <termio.h>
#include <termios.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fdi_err.h"
#include "fdi_common.h"
#include "fdi_uart.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_UART_Open(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_UART_Write(unsigned char *pucData, unsigned long ulSize)
{
	int retVal = 0;
	int nWriteCount = 0;
	int nRetry = 200;
	unsigned long orgSize = ulSize;

	while((orgSize > 0) && ( nRetry > 0))
	{
		retVal = write(1, &pucData[nWriteCount], orgSize);
		if(retVal != orgSize)
		{
			orgSize = orgSize - retVal;
			nWriteCount = nWriteCount + retVal;
			FVK_TASK_Sleep(10);
			nRetry--;
		}
		else
		{
			orgSize = 0;
			break;
		}
	};

	if(orgSize > 0)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_UART_WriteByte(unsigned char ucData)
{
	write(1, &ucData, 1);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_UART_Read(unsigned char *pucData, unsigned long ulSize, unsigned long *ulRealSize)
{
	int retVal = 0;
	int nReadCount = 0;
	unsigned long orgSize = ulSize;
	int nRetry = 200;

	while((orgSize > 0) && ( nRetry > 0))
	{
		retVal = read(0, &pucData[nReadCount], orgSize);
		if(retVal != orgSize)
		{
			orgSize = orgSize - retVal;
			nReadCount = nReadCount + retVal;
			FVK_TASK_Sleep(10);
			nRetry--;
		}
		else
		{
			orgSize = 0;
			break;
		}
	};

	*ulRealSize = ulSize - orgSize;
	return FDI_NO_ERR;

}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_UART_ReadByte(unsigned char *pucData)
{
	int bytes_read;

	if(NULL == pucData)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : parameter NULL !!\n", __FUNCTION__));
		return FDI_ERR;
	}
	bytes_read = read(0, pucData, 1);
	if (bytes_read < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : Read data failure !!\n", __FUNCTION__));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/* end of file */

