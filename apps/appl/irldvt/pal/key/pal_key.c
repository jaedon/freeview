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
//#include <octo_common.h>

#include <di_key.h>
#include <di_power.h>

#include <pal_key.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/* RCU key lookup table */

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

void PAL_KEY_Init(void)
{
	//	Nothing to do in DIRECTFB mode.
	HxLOG_Critical("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
//#if defined(CONFIG_OP_FREESAT)
//	DI_KEY_SetWakeupRcuKey(DI_KEY_WAKEUP_KEY_TYPE_NUMBER, DI_KEY_REMOCON_0);
//#endif
}

HERROR	PAL_KEY_GetCurrentCustomCode(HUINT16 *pusCustomCode)
{
	DI_ERR_CODE		err;
#ifdef	CONFIG_PROD_RCU_MULTI_CUSTOM_CODE
	HUINT16			usCustomCode;

	err = DI_KEY_GetCurrentCustomCode(&usCustomCode);
	*pusCustomCode = usCustomCode;
#else
	err = DI_ERR_OK;
#endif

	return ((err == DI_ERR_OK) ? ERR_OK : ERR_FAIL);
}

#if 0
KEY_NotifyCallback_t PAL_KEY_GetRegisterKeyCallback(void)
{
	HxLOG_Critical("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
	return (KEY_NotifyCallback_t)NULL;
}

HERROR PAL_KEY_RegisterKeyCallback(KEY_NotifyCallback_t pfnKeyNotifyCallback)
{
	HxLOG_Critical("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
	return ERR_OK;
}

void PAL_KEY_DisableKeyInput(void)
{
	HxLOG_Critical("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
}

void PAL_KEY_EnableKeyInput(void)
{
	HxLOG_Critical("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
}

HUINT8 *PAL_KEY_GetKeyString(HUINT32 ulKeyCode)
{
	HxLOG_Critical("We don't need function named (%s) in DirectFB mode\n", __FUNCTION__);
	return (HUINT8*)"undefined";
}
#endif

/*********************** End of File ******************************/
