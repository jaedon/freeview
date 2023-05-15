/**
	@file     pal_key.c
	@brief    RCU, Front DI를 제어하고 MW/AP에 API를 제공한다.

	Description: 		\n
	Module: PAL/RCU			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <di_key.h>
#include <di_power.h>

#include <pal_key.h>

/*******************************************************************/
/** Defines */
/*******************************************************************/


/*******************************************************************/
/** Typedef */
/*******************************************************************/


/*******************************************************************/
/** Global Variables */
/*******************************************************************/


/*******************************************************************/
/** Static Variables */
/*******************************************************************/
STATIC KEY_NotifyCallback_t scbKeyPreProcessing;


/*******************************************************************/
/** Static Function Prototypes */
/*******************************************************************/


/*******************************************************************/
/** Functions */
/*******************************************************************/
STATIC HINT32 P_KEY_PreProcessingCallback(DI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, DI_KEY_TYPE eKeyType)
{
	KEY_PressMode_e ePressMode;
	KEY_Device_t tDevice;
	HINT32 rc = -1;

	switch(eDevice) {
	case DI_FRONT_KEY:
		tDevice = eKeyDevice_FRONT;
		break;
	case DI_RCU_KEY:
		tDevice = eKeyDevice_RCU;
		break;
	default:
		goto done;
	}

	switch(eKeyType) {
	case DI_KEY_PRESSED:
		ePressMode = eKeyPress_DOWN;
		break;
	case DI_KEY_REPEATED:
		ePressMode = eKeyPress_REPEAT;
		break;
	case DI_KEY_RELEASED:
		ePressMode = eKeyPress_UP;
		break;
	case DI_KEY_TOGGLED:
	default:
		goto done;
	}

	if (scbKeyPreProcessing)
	{
		scbKeyPreProcessing(ulKeyCode, ePressMode, tDevice);
	}

	rc = 0;
done:
	return rc;	
}

void PAL_KEY_Init(void)
{
	//	Nothing to do in DIRECTFB mode.
#if !defined(CONFIG_PACKAGE_DIRECTFB)
	HxLOG_Warning("You could implement this function\n");
#else
	HxLOG_Print("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
#endif

	DI_KEY_RegisterKeyPreProcessingCallback(P_KEY_PreProcessingCallback);
}

HERROR	PAL_KEY_GetCurrentCustomCode(HUINT16 *pusCustomCode)
{
	DI_ERR_CODE		err;

#if defined(CONFIG_MW_HDD_FORMAT_JIGSW)
	err = DI_ERR_OK;
	return err;
#endif

#ifdef	CONFIG_PROD_RCU_MULTI_CUSTOM_CODE
	HUINT16			usCustomCode;

	err = DI_KEY_GetCurrentCustomCode(&usCustomCode);
	*pusCustomCode = usCustomCode;
#else
	err = DI_ERR_OK;
#endif

	return ((err == DI_ERR_OK) ? ERR_OK : ERR_FAIL);
}

HERROR PAL_KEY_RegisterKeyPreProcessingCallback(KEY_NotifyCallback_t pfnNotify)
{
	scbKeyPreProcessing = pfnNotify;
	return ERR_OK;
}

/*********************** End of File ******************************/
