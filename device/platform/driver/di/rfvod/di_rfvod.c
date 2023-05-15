/*******************************************************************************
* File name : di_rfvod.c
* Author : gthan
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2013.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"
#include "di_err.h"
#include "vkernel.h"
#include "di_uart.h"
#include "drv_err.h"


/*******************************************************************************
* Debug level
*******************************************************************************/
#ifdef DI_RFVOD_DEBUG
#define PrintInfo		DI_UART_Print
#define PrintDebug	DI_UART_Print
#define PrintError	DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	DI_UART_Print
#define PrintInfo		DI_UART_Print
#endif


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static HBOOL s_bIsRfvodMode = FALSE;


/*******************************************************************************
* Static function prototypes
*******************************************************************************/



/*******************************************************************************
* function : DRV_RFVOD_Init
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_RFVOD_Init(void)
{
	DRV_Error eDrvError = DRV_OK;

	s_bIsRfvodMode = FALSE;

	return eDrvError;
}

/*******************************************************************************
* function : DRV_RFVOD_GetRfvodMode
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_RFVOD_GetRfvodMode(HBOOL *bIsRfvodMode)
{
	DRV_Error eDrvError = DRV_OK;

	*bIsRfvodMode = s_bIsRfvodMode;

	return eDrvError;
}

/*******************************************************************************
* function : DI_RFVOD_SetRfvodMode
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RFVOD_SetRfvodMode(void)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	s_bIsRfvodMode = TRUE;

	return eDiErr;
}

/*******************************************************************************
* function : DI_RFVOD_UnsetRfvodMode
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RFVOD_UnsetRfvodMode(void)
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	s_bIsRfvodMode = FALSE;

	return eDiErr;
}

/* end of file */
