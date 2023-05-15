/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_RF_H_
#define _FDI_RF_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_RF_BYPASS_ON = 0, 
	FDI_RF_BYPASS_OFF
} FDI_RF_BYPASS;

typedef enum
{
	FDI_RF_TV_STD_PAL_I = 0,
	FDI_RF_TV_STD_PAL_G,
	FDI_RF_TV_STD_PAL_B,
	FDI_RF_TV_STD_PAL_D,
	FDI_RF_TV_STD_PAL_K,
	FDI_RF_TV_STD_PAL_M,
	FDI_RF_TV_STD_NTSC,
	FDI_RF_TV_STD_END
} FDI_RF_TV_STANDARD_e;

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
FDI_ERR_CODE FDI_RF_Open();

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_RF_SetBypass(FDI_RF_BYPASS eOnOff);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_RF_SetModulation(FDI_RF_TV_STANDARD_e eTvStd, unsigned long unChannel, unsigned long nOffset);

#endif


