/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_BT_H_
#define _FDI_BT_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"
#include "di_bt.h"


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */
FDI_ERR_CODE FDI_BT_Open(void);
FDI_ERR_CODE FDI_BT_StartScan(void);
FDI_ERR_CODE FDI_BT_RegisterCallback(pfnBT_EVT_CALLBACK pCallback);
FDI_ERR_CODE FDI_BT_Enable(void);

#endif

