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

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_tmd.h"

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
extern BOOL g_bMesMode;

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
BOOL g_bTmdWait;

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_TMD_Open(void)
{
	FDI_ERR_CODE errCode;
	
	errCode = FDI_USB_SerialOpen();
	if (errCode == FDI_ERR)
	{
		FDI_PRINT(FDI_PRT_WARNING, ("[FDI_TMD_Open] : FDI_USB_SerialOpen error \n"));
		
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
FDI_ERR_CODE FDI_TMD_Close(void)
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
FDI_ERR_CODE FDI_TMD_SendData(unsigned char *pucData, unsigned long ulSize)
{
	/* MES mode에 진입한 경우 TMD 통신은 멈춘다. */
	if (g_bMesMode == TRUE)
	{
		return FDI_ERR;
	}

	return FDI_USB_SerialSend(pucData, ulSize);	
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_TMD_ReceiveData(unsigned char *pucData, unsigned long ulSize, unsigned long ulTimeoutMs)
{
	FDI_ERR_CODE errCode;
	unsigned long ulReadSize = 0;
	
	/* MES mode에 진입한 경우 TMD 통신은 멈춘다. */
	if (g_bMesMode == TRUE)
	{
		return FDI_ERR;
	}

	while (ulReadSize != ulSize)
	{
		errCode = FDI_USB_SerialReceive(&pucData[ulReadSize], 1, ulTimeoutMs);
		if (errCode != FDI_NO_ERR)
		{
			FVK_TASK_Sleep(10);
		}
		else
		{
			ulReadSize++;
		}
		
		if (g_bMesMode == TRUE)
		{
			return FDI_ERR;
		}
	}

	return FDI_NO_ERR;	
}
