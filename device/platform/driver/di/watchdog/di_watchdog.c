/**
 * di_watchdog.c
*/

/**
 * @defgroup		di_watchdog : watchdog device interface
 * @author			Kyu-Young Cho
 * @note			watchdog API file
 * @brief			watchdog apis
 * @file 			di_watchdog.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#endif
#include "drv_err.h"
#include "drv_watchdog.h"
#include "di_watchdog.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_LED

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/


/********************************************************************
Function   : DI_WATCHDOG_OnOff
Description :
Input	   : HBOOL onOff
Output	   : none
Return	   : DI_ERR_CODE
********************************************************************/

DI_ERR_CODE DI_WATCHDOG_OnOff (HBOOL onOff)
{
	DRV_Error		err;

	err = DRV_DOG_Enable((int)onOff);
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/********************************************************************
Function   : DI_WATCHDOG_GetUseStatus
Description :
Input	   : none
Output	   : HBOOL *useStatus
Return	   : DI_ERR_CODE
********************************************************************/
DI_ERR_CODE DI_WATCHDOG_GetUseStatus (HBOOL *useStatus)
{
	DRV_Error		err;

	err = DRV_DOG_IsEnabled((unsigned char*)useStatus);
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;

}

/********************************************************************
Function   : DI_WATCHDOG_SetResetPeriod
Description :
Input	   : HUINT32 sec
Output	   :
Return	   : DI_ERR_CODE
********************************************************************/
DI_ERR_CODE DI_WATCHDOG_SetResetPeriod (HUINT32 sec)
{
	DRV_Error		err;

	err = DRV_DOG_SetKickPeriod((unsigned int)sec);
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}


/********************************************************************
Function   : DI_WATCHDOG_SetTimeOut
Description :
Input	   : HUINT16 sec
Output	   : none
Return	   : DI_ERR_CODE
********************************************************************/
DI_ERR_CODE DI_WATCHDOG_SetTimeOut (HUINT16 sec)
{
	DRV_Error		err;

	err = DRV_DOG_SetTimeout((unsigned int)sec);
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/********************************************************************
Function   : DI_WATCHDOG_Reset
Description :
Input	   : none
Output	   : none
Return	   : DI_ERR_CODE
********************************************************************/
DI_ERR_CODE DI_WATCHDOG_Reset (void)
{
	DRV_Error		err;

	err = DRV_DOG_Kick();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

/********************************************************************
Function   : DI_WATCHDOG_Init
Description :
Input	   : none
Output	   : none
Return	   : DI_ERR_CODE
********************************************************************/
DRV_Error DI_WATCHDOG_Init (void)
{
	DRV_Error		err;

	err = DRV_DOG_Init();
	if (err != DRV_OK) {
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
