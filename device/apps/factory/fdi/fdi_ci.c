/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "ftype.h"
#include "fdi_common.h"
#include "fdi_ci.h"

#include "di_err.h"
#include "di_ci.h"
#include "pdriver.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define DRV_DEV		"/dev/ci"					  /* Driver Device Name		*/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
FDI_CI_CallbackFunc s_fnCiCallback;

static void INTRN_FDI_CI_NotifyCamStatus(unsigned short usSlotId, unsigned short usState);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CI_Open(unsigned long ulSlotId)
{
	int slot;

	slot = DRV_CI_Open((unsigned char *)DRV_DEV);
	if (slot < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CI_Open] : DRV_CI_Open Error!!\n"));
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
FDI_CI_STATE FDI_CI_CheckModule(unsigned long ulSlotId)
{
	DI_CI_STATE eCiState;

	eCiState = DI_CI_CheckModule(ulSlotId);
	if( eCiState == DI_CI_INSERTED )
	{
		return FDI_CI_INSERTED;
	}
	else
	{
		return FDI_CI_EXTRACTED;
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
FDI_ERR_CODE FDI_CI_ReadMem(unsigned long ulSlotId, 
							unsigned long ulAddr, 
							unsigned long ulLen, 
							unsigned char *pucBuf)
{
	DRV_stMem tCiMem;
	int iRet;
	
	tCiMem.addr   = ulAddr;
	tCiMem.len    = ulLen;
	tCiMem.pbytes = pucBuf;
	iRet = DRV_CI_Ioctl(ulSlotId, DRV_READMEM, &tCiMem);
	if (iRet < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CI_ReadMem] : DRV_CI_Ioctl(READMEM) Error!!\n"));
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
FDI_ERR_CODE FDI_CI_WriteMem(unsigned long ulSlotId, 
								unsigned long ulAddr, 
								unsigned long ulLen, 
								unsigned char *pucBuf)
{
	DRV_stMem tCiMem;
	int iRet;
	
	tCiMem.addr   = ulAddr;
	tCiMem.len    = ulLen;
	tCiMem.pbytes = pucBuf;
	iRet = DRV_CI_Ioctl(ulSlotId, DRV_WRITEMEM, &tCiMem);
	if (iRet < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CI_ReadMem] : DRV_CI_Ioctl(WRITEMEM) Error!!\n"));
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
FDI_ERR_CODE FDI_CI_ReadIo(unsigned long ulSlotId, unsigned long ulReg, unsigned char *pucValue)
{
	DRV_stIO reg;	/* Ioctl parameter */

	reg.registr = ulReg;
	reg.pvalue  = pucValue;
	if (DRV_CI_Ioctl(ulSlotId, DRV_READIO, &reg) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("PHYS_ReadByte : DRV_CI_Ioctl error !!!\n"));
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
FDI_ERR_CODE FDI_CI_WriteIo(unsigned long ulSlotId, unsigned long ulReg, unsigned char *pucValue)
{
	DRV_stIO reg;	/* Ioctl parameter */

	reg.registr = ulReg;
	reg.pvalue  = pucValue;
	if (DRV_CI_Ioctl(ulSlotId, DRV_WRITEIO, &reg) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("PHYS_WriteByte : DRV_CI_Ioctl error !!!\n"));
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
FDI_ERR_CODE FDI_CI_Close(unsigned long ulSlotId)
{
	DRV_CI_Close(ulSlotId);
	
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
FDI_ERR_CODE FDI_CI_RegisterCallback(FDI_CI_CallbackFunc pfnCallback)
{
	s_fnCiCallback = pfnCallback;
	
	DI_CI_RegisterCallback(INTRN_FDI_CI_NotifyCamStatus);

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
static void INTRN_FDI_CI_NotifyCamStatus(unsigned short usSlotId, unsigned short usState)
{
	if( s_fnCiCallback != NULL )
	{
		s_fnCiCallback(usSlotId, usState);
	}
}

/* end of file */

