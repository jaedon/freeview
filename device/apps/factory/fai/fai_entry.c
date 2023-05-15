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
#include <stdio.h>

/* chip manufacture's  header files */

/* humax header files */
#include "cmdtool.h"
#include "di_init.h"
#include "di_uart.h"
#include "vkernel.h"

/* model specific configuration header */

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
extern int FAI_Main();

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
int Application_Main(void)
{
	CMD_Init();
	CMD_DI_Init();

	printf("\n****************************\n"
			 "** Factory Software Start **\n"
			 "** New Build System		**\n"
			 "****************************\n");
	FAI_Main();

	CMD_KeyTask(NULL);

	return 0;
}

