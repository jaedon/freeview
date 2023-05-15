/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_SC_H_
#define _FDI_SC_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_SC_EXTRACTED = 0,
	FDI_SC_INSERTED,
	FDI_SC_CLEAR
} FDI_SC_EVENT;

typedef enum
{
	FDI_T_0  = 1,
	FDI_T_1  = 2,
	FDI_T_14 = 4
} FDI_SC_PROTOCOL;

typedef enum 
{
	FDI_SC_NOT_PRESENT = 0,
	FDI_SC_PRESENT
} FDI_SC_STATUS;

typedef FDI_ERR_CODE (*pfnFDI_SC_EVT_CALLBACK)(unsigned long ulDeviceId, FDI_SC_EVENT eEvent);

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_GetCapability(unsigned long *pulNumofDevice);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_Open(unsigned long ulDeviceId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_Reset(unsigned long ulDeviceId, unsigned char *pucAtr, unsigned char *pucLen);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_SetParams(unsigned long ulDeviceId, FDI_SC_PROTOCOL eProtocol);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_GetCardStatus(unsigned long ulDeviceId, FDI_SC_STATUS *peStatus);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_RegisterCallback(unsigned long ulDeviceId, pfnFDI_SC_EVT_CALLBACK pfnCallback);

#endif


