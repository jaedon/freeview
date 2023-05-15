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

HERROR	NX_PORT_PANEL_SetAppPriority(HINT32 lPriority)
{
	return	APK_OUTPUT_PANEL_SetAppPriority(lPriority);
}

HERROR	NX_PORT_PANEL_SetLed(DxLedId_e eLedId, DxLedStatus_e eStatus)
{
	return	APK_OUTPUT_PANEL_SetLed(eLedId, eStatus);
}

HERROR	NX_PORT_PANEL_SetLedAuto(void)
{
	return	APK_OUTPUT_PANEL_SetLedAuto();
}

HERROR	NX_PORT_PANEL_SetLedCategory(DxPanelCategory_e eCategory)
{
	return	APK_OUTPUT_PANEL_SetLedCategory(eCategory);
}

HERROR	NX_PORT_PANEL_ClearLed(void)
{
	return	APK_OUTPUT_PANEL_ClearLed();
}

HERROR	NX_PORT_PANEL_SetPanel(HINT32 lPriority, HCHAR *pszString, DxPanelAlign_e ePanelAlign)
{
	return	APK_OUTPUT_PANEL_SetPanel(lPriority, pszString, ePanelAlign);
}

HERROR	NX_PORT_PANEL_SetPanelAuto(HINT32 lPriority)
{
	return	APK_OUTPUT_PANEL_SetPanelAuto(lPriority);
}

HERROR	NX_PORT_PANEL_SetPanelDimLevel(HINT32 lPriority, HINT32 nDimLevel)
{
	return	APK_OUTPUT_PANEL_SetPanelDimLevel(lPriority, nDimLevel);
}

HERROR	NX_PORT_PANEL_SetPanelAnimation(HINT32 lPriority)
{
	return	APK_OUTPUT_PANEL_SetPanelAnimation(lPriority);
}

HERROR	NX_PORT_PANEL_SetRgbLed(DxLedId_e eLedId, DxRgbLedColor_e eLedColor, HBOOL bBlink)
{
	return	APK_OUTPUT_PANEL_SetRgbLed(eLedId, eLedColor, bBlink);
}

HERROR	NX_PORT_PANEL_SetRgbLedOff(DxLedId_e eLedId)
{
	return	APK_OUTPUT_PANEL_SetRgbLedOff(eLedId);
}

HERROR	NX_PORT_PANEL_SetRgbLedDimm(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel)
{
	return	APK_OUTPUT_PANEL_SetRgbLedDimm(eLedId, eDimmLevel);
}

HERROR	NX_PORT_PANEL_SetRgbLedBlink(DxLedId_e eLedId, HBOOL bBlink)
{
	return	APK_OUTPUT_PANEL_SetRgbLedBlink(eLedId, bBlink);
}

HERROR	NX_PORT_PANEL_SetPowerLedState(DxPowerLedStatus_e eLedStatus)
{
	return	APK_OUTPUT_PANEL_SetPowerLedState(eLedStatus);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
