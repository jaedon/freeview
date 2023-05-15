/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_output_panel.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

#include <oapi.h>
#include <apk.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
static HINT32	s_nApplPriority = 0;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR	APK_OUTPUT_PANEL_Init(void)
{
	OAPI_OUTPUT_PANEL_Init();
	return ERR_OK;
}

HERROR	APK_OUTPUT_PANEL_DeInit(void)
{
	return ERR_OK;
}

HERROR	APK_OUTPUT_PANEL_SetAppPriority(HINT32 nPriority)
{
	s_nApplPriority = nPriority;

	return ERR_OK;
}

HERROR	APK_OUTPUT_PANEL_SetLed(DxLedId_e eLedId, DxLedStatus_e eStatus)
{
	return OAPI_OUTPUT_PANEL_SetLed(eLedId, eStatus);
}

HERROR	APK_OUTPUT_PANEL_SetLedAuto(void)
{
	return OAPI_OUTPUT_PANEL_SetLedAuto();
}

HERROR	APK_OUTPUT_PANEL_SetLedCategory(DxPanelCategory_e eCategory)
{
	return OAPI_OUTPUT_PANEL_SetLedCategory(eCategory);
}

HERROR	APK_OUTPUT_PANEL_ClearLed(void)
{
	return OAPI_OUTPUT_PANEL_ClearLed();
}

HERROR	APK_OUTPUT_PANEL_SetPanel(HINT32 nPriority, const HCHAR *pszString, DxPanelAlign_e ePanelAlign)
{
	return OAPI_OUTPUT_PANEL_SetPanel(s_nApplPriority + nPriority, pszString, ePanelAlign);
}

HERROR	APK_OUTPUT_PANEL_SetPanelAuto(HINT32 nPriority)
{
	return OAPI_OUTPUT_PANEL_SetPanelAuto(s_nApplPriority + nPriority);
}

HERROR	APK_OUTPUT_PANEL_SetPanelDimLevel(HINT32 nPriority, HINT32 nDimLevel)
{
	return OAPI_OUTPUT_PANEL_SetPanelDimLevel(nPriority, nDimLevel);
}

HERROR	APK_OUTPUT_PANEL_SetPanelAnimation(HINT32 nPriority)
{
	return OAPI_OUTPUT_PANEL_SetPanelAnimation(nPriority);
}

HERROR	APK_OUTPUT_PANEL_SetRgbLed(DxLedId_e eLedId, DxRgbLedColor_e eLedColor, HBOOL bBlink)
{
	return OAPI_OUTPUT_PANEL_SetRgbLed(eLedId, eLedColor, bBlink);
}

HERROR	APK_OUTPUT_PANEL_SetRgbLedOff(DxLedId_e eLedId)
{
	return OAPI_OUTPUT_PANEL_SetRgbLedOff(eLedId);
}

HERROR	APK_OUTPUT_PANEL_SetRgbLedDimm(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel)
{
	return OAPI_OUTPUT_PANEL_SetRgbLedDimm(eLedId, eDimmLevel);
}

HERROR	APK_OUTPUT_PANEL_SetRgbLedBlink(DxLedId_e eLedId, HBOOL bBlink)
{
	return OAPI_OUTPUT_PANEL_SetRgbLedBlink(eLedId, bBlink);
}

HERROR	APK_OUTPUT_PANEL_SetPowerLedState(DxPowerLedStatus_e eLedStatus)
{
	return OAPI_OUTPUT_PANEL_SetPowerLedState(eLedStatus);
}

/* end of file */
