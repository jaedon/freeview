/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_CABLEMODEM_H_
#define _FAI_CABLEMODEM_H_

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
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
void INTRN_FAI_CM_TestTask(void *data);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_CABLEMODEM_Init();

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FAI_ERR_CODE FAI_CABLEMODEM_Check(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

#endif
