/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_SCART_H_
#define _FDI_SCART_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_SCART_OUTPUT_TV, 
	FDI_SCART_OUTPUT_VCR,
	FDI_SCART_OUTPUT_TV_VCR
} FDI_SCART_OUTPUT_e;

typedef enum
{
	FDI_SCART_AR_NONE,
	FDI_SCART_AR_4_3,
	FDI_SCART_AR_16_9
} FDI_SCART_AR_e;

typedef enum
{
	FDI_SCART_VFORMAT_CVBS,
	FDI_SCART_VFORMAT_RGB,
	FDI_SCART_VFORMAT_SVHS
} FDI_SCART_VFORMAT_e;

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
typedef void (*FDI_SCART_CALLBACK)(void*);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SCART_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SCART_GetCapability(unsigned long *pulNumOfScart);

/******************************************************************************
* function : PDI_SCART_SetBypass
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_SetHwBypass(BOOL bEnable);

/******************************************************************************
* function : PDI_SCART_SetBypass
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_SetBypass(BOOL bEnable);

/******************************************************************************
* function : FDI_SCART_SetAspectRatio
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_e eOutput, FDI_SCART_AR_e eAspectRatio);

/******************************************************************************
* function : FDI_SCART_SetVideoFormat
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_e eOutput, FDI_SCART_VFORMAT_e eVForamt);

/******************************************************************************
* function : FDI_SCART_RegisterCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_SCART_RegisterVCRCallback(FDI_SCART_CALLBACK pfnCallback);

#endif

