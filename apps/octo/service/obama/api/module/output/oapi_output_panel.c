/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_panel.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

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
STATIC HBOOL					s_bOutputPanelInit;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____NOTIFICATION_CALLBACKS_____

#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_OUTPUT_PANEL_Init(void)
{
	HERROR		err;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bOutputPanelInit != TRUE)
	{
		err = OAPI_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
		if(err != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has OAPI_RpcProtocol_GetRpcHandle() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		s_bOutputPanelInit = TRUE;
	}

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetLed(DxLedId_e eLedId, DxLedStatus_e eStatus)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetLed, "ii", eLedId, eStatus);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetLedAuto(void)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetAuto, NULL);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetLedCategory(DxPanelCategory_e eCategory)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetCategory, "i", eCategory);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_ClearLed(void)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_Clear, NULL);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetPanel(HINT32 nPriority, const HCHAR *pszString, DxPanelAlign_e ePanelAlign)
{
	if (pszString == NULL)
		HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanel, "isi", nPriority, "", ePanelAlign);
	else
		HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanel, "isi", nPriority, pszString, ePanelAlign);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetPanelAuto(HINT32 nPriority)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanelAuto, "i", nPriority);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetPanelDimLevel(HINT32 nPriority, HINT32 nDimLevel)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPanelDimLevel, "ii", nPriority, nDimLevel);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetPanelAnimation(HINT32 nPriority)
{
	HxLOG_Warning("not implemented!\n");
	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetRgbLed(DxLedId_e eLedId, DxRgbLedColor_e eLedColor, HBOOL bBlink)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLed, "iii", eLedId, eLedColor, bBlink);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetRgbLedOff(DxLedId_e eLedId)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLedOff, "i", eLedId);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetRgbLedDimm(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLedDimm, "ii", eLedId, eDimmLevel);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetRgbLedBlink(DxLedId_e eLedId, HBOOL bBlink)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetRgbLedBlink, "ii", eLedId, bBlink);

	return ERR_OK;
}

HERROR	OAPI_OUTPUT_PANEL_SetPowerLedState(DxPowerLedStatus_e eLedStatus)
{
	HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_PANEL_SetPowerLedState, "i", eLedStatus);

	return ERR_OK;
}

