/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_panel.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_port.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HERROR	NX_PANEL_SetAppPriority(HINT32 lPriority)
{
	return	NX_PORT_PANEL_SetAppPriority(lPriority);
}


#define ________________RELATED_SYSTEM_FRONT_PANEL_LED_________________

HERROR	NX_PANEL_SetLedOnOff(DxLedId_e eLedId, HBOOL bOnOff, HBOOL bAuto, HBOOL bAllClear)
{
	HERROR		hError = ERR_FAIL;

	NX_APP_Debug("==NX_PANEL_SetLedOnOffClear== eLedId:(%u), bOnOff:(%d), bAuto:(%d), bAllClear:(%d) \n", eLedId, bOnOff, bAuto, bAllClear);

	if (bAllClear)
	{
		hError = NX_PORT_PANEL_ClearLed();
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_PORT_PANEL_ClearLed() Fail!! \n");
		}
	}

	if (bAuto)
	{
		hError = NX_PORT_PANEL_SetLedAuto();
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_PORT_PANEL_SetLedAuto() Fail!! \n");
		}
	}
	else
	{
		hError = NX_PORT_PANEL_SetLed(eLedId, (bOnOff ? eDxLEDSTATUS_ON : eDxLEDSTATUS_OFF));
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_PORT_PANEL_SetLed() Fail!! \n");
		}
	}

	return	hError;
}


HERROR	NX_PANEL_SetLedAuto(void)
{
	return	NX_PORT_PANEL_SetLedAuto();
}


HERROR	NX_PANEL_ClearLed(void)
{
	return	NX_PORT_PANEL_ClearLed();
}

#define ________________RELATED_SYSTEM_FRONT_PANEL__________________

HERROR	NX_PANEL_SetString(HUINT8 *pszPanelString, DxPanelAlign_e eAlign, HBOOL bAuto)
{
	HERROR		hError = ERR_FAIL;

#if defined (CONFIG_MW_SYS_VFD) || defined (CONFIG_MW_SYS_LCD) || defined (CONFIG_MW_SYS_7SEG)
	if (bAuto)
	{
		hError = NX_PORT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
	}
	else
	{
		hError = NX_PORT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, (HCHAR*)pszPanelString, eAlign);
	}
#else
	hError = ERR_OK;
#endif

	return hError;
}


HERROR	NX_PANEL_ClearString(void)
{
	HERROR		hError = ERR_FAIL;

#if defined (CONFIG_MW_SYS_VFD) || defined (CONFIG_MW_SYS_LCD) || defined (CONFIG_MW_SYS_7SEG)
	hError = NX_PORT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, NULL, eDxPANEL_ALIGN_CENTER);
#else
	hError = ERR_OK;
#endif

	return hError;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
