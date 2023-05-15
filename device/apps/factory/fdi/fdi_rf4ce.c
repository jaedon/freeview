




/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2012 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_rf4ce.h"

#include "di_rf4ce.h"
#include "drv_rf4ce.h"

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

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
pfnFDI_RF4CE_NotifyFunc	g_pfnRf4ceEventCallback = NULL;


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_RF4CE_Open()
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_RF4CE_Pairing(BOOL bOnOff)
{
	DI_RF4CE_SetPairing(bOnOff, 0);
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_RF4CE_ClearPairingTable( void )
{
	DRV_Error eDrvErr = DRV_OK;

	eDrvErr = DRV_RF4CE_ClearPairingTable();
	if(eDrvErr != DRV_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_RF4CE_Pairing] ClearPairingTable Error\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_RF4CE_RegisterCallback(pfnFDI_RF4CE_NotifyFunc pfnCallback)
{
	if(pfnCallback == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_RF4CE_RegisterCallback] Invalid Parameter. pfnCallback==NULL\n"));
		return FDI_ERR;
	}

	g_pfnRf4ceEventCallback = pfnCallback;

	DI_RF4CE_RegisterEventCallback(DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS, INTRN_FDI_RF4CE_NotifyFunc);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FDI_RF4CE_NotifyFunc(DI_RF4CE_CALLBACK_DATA_t *pEventData)
{
	FDI_RF4CE_EVENT_e eEvent = FDI_RF4CE_EVENT_MAX;

	switch(pEventData->eEvent)
	{
	case DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS:
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_RF4CE_NotifyFunc] Pairing Success\n"));
		eEvent = FDI_RF4CE_EVENT_PAIRING_SUCCESS; break;
	case DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED:
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_RF4CE_NotifyFunc] Pairing Failed\n"));
		eEvent = FDI_RF4CE_EVENT_PAIRING_FAILED; break;
	default:
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_RF4CE_NotifyFunc] Unknown Event\n"));
		eEvent = FDI_RF4CE_EVENT_MAX; break;
	}
	
	g_pfnRf4ceEventCallback(eEvent);
	
}

