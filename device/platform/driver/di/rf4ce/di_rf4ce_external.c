/*******************************************************************************
* File name : di_rf4ce_external.c
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
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
#include "drv_err.h"
#include "di_uart.h"
#include "di_rf4ce.h"
#include "di_rf4ce_priv.h"
#include "drv_rf4ce.h"

/*******************************************************************************
* Debug level
*******************************************************************************/
#ifdef DI_RF4CE_DEBUG
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
extern RF4CE_INSTANCE_t g_tRF4CEInstance;
//#if defined(CONFIG_RF4CE_EXTERNAL_TI)
extern DRV_Error DRV_RF4CE_TI_Init(void);
extern DRV_Error DRV_RF4CE_TI_REQ_AllowPairReq(void);
//#endif

/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/

/*******************************************************************************
* function : DRV_RF4CE_EXTERNAL_Init
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_RF4CE_EXTERNAL_Init(void)
{
	DRV_Error nDrvErr = DRV_OK;

/*	#if defined(CONFIG_RF4CE_EXTERNAL_TI)	*/
	nDrvErr = DRV_RF4CE_TI_Init();
/*	#endif	*/
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] ERR DRV_RF4CE_TI_Init %d!!\n", __func__, nDrvErr);
		return DRV_ERR;
	}
	
	return DRV_OK;
}

/*******************************************************************************
* function : DI_RF4CE_SetPairing
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_SetPairing(HBOOL bPairing, HUINT32 ulTimeOut)
{
	DRV_Error 	eDrvError = DRV_ERR;

	eDrvError = DRV_RF4CE_SetPairing(bPairing, ulTimeOut);
	if(eDrvError != DRV_OK)
	{
		PrintError("[%s] DI_RF4CE_SetPairing() Error\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	
	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_RF4CE_GetPairedDevCount
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_GetPairedDevCount(HUINT32 *pulCnt)
{
	(void*)pulCnt;

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_RF4CE_GetIEEEAddr
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_GetIEEEAddr(HUINT32 ulIndex, HUINT8 *pucValue)
{
	ulIndex = ulIndex;
	(void*)pucValue;

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DRV_RF4CE_EXTERNAL_Task
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_RF4CE_EXTERNAL_Task(void *pvParam)
{
	pvParam = pvParam;

	return;
}

/* end of file */
