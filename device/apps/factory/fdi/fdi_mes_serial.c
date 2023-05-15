/*******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_err.h"
#include "fdi_common.h"

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
BOOL g_bMesMode;

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MES_Open(void)
{
	FDI_ERR_CODE errCode;
	
	g_bMesMode = TRUE;

	errCode = FDI_USB_SerialOpen();
	if (errCode == FDI_ERR)
	{
		g_bMesMode = FALSE;
		
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MES_Open] : FDI_USB_SerialOpen failed!!\n"));
		
		return FDI_ERR;
	}
	
	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_MES_Open] : open success!!\n"));

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
FDI_ERR_CODE FDI_MES_SendData(unsigned char *pucData, unsigned long ulSize)
{
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
FDI_ERR_CODE FDI_MES_ReceiveData(unsigned char *pucData, unsigned long ulSize, unsigned long ulTimeoutMs)
{
	FDI_ERR_CODE errCode;
	unsigned long ulReadSize = 0;
	
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
FDI_ERR_CODE FDI_MES_Close(void)
{
	g_bMesMode = FALSE;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_MES_Close] : close success!!\n"));

	return FDI_NO_ERR;
}

