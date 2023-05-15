/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_output_panel.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <svc_sys.h>


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
STATIC HINT32					s_nRpcHandle;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____

#define _____OAPI_COMMAND_FUNCTIONS_____

//	apArgv[0] : DxLedId_e
//	apArgv[1] : DxLedStatus_e
STATIC HERROR om_output_panel_SetLed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxLedId_e		eLedId;
	DxLedStatus_e	eLedStatus;

	HxLOG_Assert(nArgc == 2);

	eLedId = HxOBJECT_INT(apArgv[0]);
	eLedStatus = HxOBJECT_INT(apArgv[1]);

	if (eLedStatus == eDxLEDSTATUS_ON)
	{
		SVC_SYS_SetLedBlink(eLedId, 3);
		SVC_SYS_SetLedOnOffByCustom(eLedId, TRUE);
	} else
	if (eLedStatus == eDxLEDSTATUS_OFF)
	{
		SVC_SYS_SetLedOnOffByCustom(eLedId, FALSE);
	}

	return ERR_OK;
}

STATIC HERROR om_output_panel_SetAutoLed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 0);

	SVC_SYS_SetLedAuto();

	return ERR_OK;
}

//	apArgv[0] : DxPanelCategory_e
STATIC HERROR om_output_panel_SetCategoryLed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxPanelCategory_e	eCategory;

	HxLOG_Assert(nArgc == 1);

	eCategory = HxOBJECT_INT(apArgv[0]);
	SVC_SYS_SetCategoryInform(eCategory);

	return ERR_OK;
}

STATIC HERROR om_output_panel_ClearLed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 0);

	SVC_SYS_ClearLedByCustom();

	return ERR_OK;
}

//	apArgv[0] : HINT32
//	apArgv[1] : HCHAR*
//	apArgv[2] : DxPanelAlign_e
STATIC HERROR om_output_panel_SetPanel(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nPriority;
	HCHAR	*pszString;
	DxPanelAlign_e	eAlign;
	HxLOG_Assert(nArgc == 3);

	nPriority = HxOBJECT_INT(apArgv[0]);
	pszString = HxOBJECT_STR(apArgv[1]);
	eAlign    = HxOBJECT_INT(apArgv[2]);

	SVC_SYS_DisplayPanelString(nPriority, pszString, eAlign);

	return ERR_OK;
}

//	apArgv[0] : HINT32
STATIC HERROR om_output_panel_SetPanelAuto(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nPriority;

	HxLOG_Assert(nArgc == 1);

	nPriority = HxOBJECT_INT(apArgv[0]);

	SVC_SYS_ClearPanelString(nPriority);

	return ERR_OK;
}

STATIC HERROR om_output_panel_SetPanelDimLevel(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nPriority;
	DimmingLevel_e eDimLevel;

	HxLOG_Assert(nArgc == 2);

	nPriority = HxOBJECT_INT(apArgv[0]);
	eDimLevel = HxOBJECT_INT(apArgv[1]);
#if defined(CONFIG_MW_SYS_VFD)
	SVC_SYS_SetLedVfdDimmLevel(eDimLevel);
#else
	SVC_SYS_SetLedDimmLevel(eDimLevel);
#endif

	NOT_USED_PARAM(nPriority);

	return ERR_OK;
}

//	apArgv[0] : DxLedId_e
//	apArgv[1] : DxRgbLedColor_e
//	apArgv[2] : HBOOL
STATIC HERROR om_output_panel_SetRgbLed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxLedId_e		eLedId;
	DxRgbLedColor_e	eLedColor = eDxRGBLED_COLOR_NONE;
	HBOOL			bBlink = FALSE;

	HxLOG_Assert(nArgc == 3);

	eLedId = HxOBJECT_INT(apArgv[0]);
	eLedColor = HxOBJECT_INT(apArgv[1]);
	bBlink = HxOBJECT_INT(apArgv[2]);

	(void)SVC_SYS_SetRGBLedByColor(eLedId, eLedColor, bBlink);

	return ERR_OK;
}

//	apArgv[0] : DxLedId_e
STATIC HERROR om_output_panel_SetRgbLedOff(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxLedId_e		eLedId;

	HxLOG_Assert(nArgc == 1);

	eLedId = HxOBJECT_INT(apArgv[0]);

	(void)SVC_SYS_SetRGBLedOff(eLedId);

	return ERR_OK;
}

//	apArgv[0] : DxLedId_e
//	apArgv[1] : DxRgbLedDimmLevel_e
STATIC HERROR om_output_panel_SetRgbLedDimm(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxLedId_e			eLedId;
	DxRgbLedDimmLevel_e	eDimmLevel = eDxRGBLED_DIMMLEVEL_SOLID;

	HxLOG_Assert(nArgc == 2);

	eLedId = HxOBJECT_INT(apArgv[0]);
	eDimmLevel = HxOBJECT_INT(apArgv[1]);

	(void)SVC_SYS_SetRGBLedDimmingLevel(eLedId, eDimmLevel);

	return ERR_OK;
}

//	apArgv[0] : DxLedId_e
//	apArgv[1] : HBOOL
STATIC HERROR om_output_panel_SetRgbLedBlink(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxLedId_e		eLedId;
	HBOOL			bBlink = FALSE;

	HxLOG_Assert(nArgc == 2);

	eLedId = HxOBJECT_INT(apArgv[0]);
	bBlink = HxOBJECT_INT(apArgv[1]);

	if(bBlink == TRUE)
	{
		(void)SVC_SYS_SetRGBLedBlinkOn(eLedId);
	}
	else
	{
		(void)SVC_SYS_SetRGBLedBlinkOff(eLedId);
	}

	return ERR_OK;
}

//	apArgv[0] : DxPowerLedStatus_e
STATIC HERROR om_output_panel_SetPowerLedState(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxPowerLedStatus_e eLedStatus;

	HxLOG_Assert(nArgc == 1);

	eLedStatus = HxOBJECT_INT(apArgv[0]);

	(void)SVC_SYS_SetPowerLedState(eLedStatus);

	return ERR_OK;
}

#define _____PROCEDURE_____

#define ___GLOBAL_FUNCTIONS___
HERROR OM_OUTPUT_PANEL_Init(void)
{
	HERROR					err;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	err = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RpcProtocol_GetRpcHandle() ERROR !!! ERROR !!! ERROR !!! ERROR !!!\n");
	}

	//	-------   RPC Methods   -------	//
	err = ERR_OK;
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetLed, "ii", om_output_panel_SetLed, NULL,
							"Set Led : (DxLedId_e eLedId, DxLedStatus_e eStatus)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetAuto, NULL, om_output_panel_SetAutoLed, NULL,
							"Set LED Auto : (void)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetCategory, "i", om_output_panel_SetCategoryLed, NULL,
							"Set LED Category : (DxPanelCategory_e eCategory)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_Clear, NULL, om_output_panel_ClearLed, NULL,
							"Clear LED State : (void)");

	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanel, "isi", om_output_panel_SetPanel, NULL,
							"Set Panel String : (HINT32 priority, HCHAR *panelString, DxPanelAlign_e panelAlign)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanelAuto, "i", om_output_panel_SetPanelAuto, NULL,
							"Set Panel Auto : (HINT32 priority)");

	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanelDimLevel, "ii", om_output_panel_SetPanelDimLevel, NULL,
							"Set Panel Dim Level : (HINT32 priority, DimmingLevel_e eDimLevel)");

	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLed, "iii", om_output_panel_SetRgbLed, NULL,
							"Set RGB Led : (DxLedId_e eLedId, DxRgbLedColor_e eColor, HBOOL bBlink)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLedOff, "i", om_output_panel_SetRgbLedOff, NULL,
							"Set RGB Led Off : (DxLedId_e eLedId)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLedDimm, "ii", om_output_panel_SetRgbLedDimm, NULL,
							"Set RGB Led DimmLevel : (DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLedBlink, "ii", om_output_panel_SetRgbLedBlink, NULL,
							"Set RGB Led Blink : (DxLedId_e eLedId, HBOOL bBlink)");
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPowerLedState, "i", om_output_panel_SetPowerLedState, NULL,
							"Set Power Led Status: (DxPowerLedStatus_e eLedStatus)");


	if (err != ERR_OK)
	{
		HxLOG_Error("error : obama fail to register method!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}




