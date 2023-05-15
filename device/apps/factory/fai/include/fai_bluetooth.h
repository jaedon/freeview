/* $Header: $ */
/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_BLUETOOTH_H_
#define _FAI_BLUETOOTH_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/


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

FAI_ERR_CODE FAI_BLUETOOTH_Init(void);
FAI_ERR_CODE FAI_BLUETOOTH_TEST_TMD(unsigned short usMsgId, unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_BLUETOOTH_GetResult_TMD(unsigned char ucArgc, unsigned char *pucArgList);

FAI_ERR_CODE FAI_BLUETOOTH_Test_UI(void);

#endif

