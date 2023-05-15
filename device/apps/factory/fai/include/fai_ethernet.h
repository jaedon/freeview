/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_ETHERNET_H_
#define _FAI_ETHERNET_H_

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

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_ETHERNET_Init(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_ETHERNET_Test_UI(void);

FAI_ERR_CODE FAI_ETHERNET_ARPCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

FAI_ERR_CODE FAI_ETHERNET_Speed_Test_UI(void);

#if (ETHERNET_SPEED_ENABLE==YES)
FAI_ERR_CODE FAI_ETHERNET_Speed_Test(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
#endif

#endif

