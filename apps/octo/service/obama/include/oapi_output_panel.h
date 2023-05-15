/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_panel.h
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

#ifndef __OAPI_OUTPUT_PANEL_H__
#define __OAPI_OUTPUT_PANEL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	RPC_OAPI_OUTPUT_PANEL_SetLed				"oapi.output.panel.SetLed"
#define	RPC_OAPI_OUTPUT_PANEL_SetAuto				"oapi.output.panel.SetLedAuto"
#define	RPC_OAPI_OUTPUT_PANEL_SetCategory			"oapi.output.panel.SetCategory"
#define	RPC_OAPI_OUTPUT_PANEL_Clear					"oapi.output.panel.ClearLed"
#define	RPC_OAPI_OUTPUT_PANEL_SetPanel				"oapi.output.panel.SetPanel"
#define	RPC_OAPI_OUTPUT_PANEL_SetPanelAuto			"oapi.output.panel.SetPanelAuto"
#define	RPC_OAPI_OUTPUT_PANEL_SetPanelDimLevel		"oapi.output.panel.SetPanelDimLevel"
#define	RPC_OAPI_OUTPUT_PANEL_SetRgbLed				"oapi.output.panel.SetRgbLed"
#define	RPC_OAPI_OUTPUT_PANEL_SetRgbLedOff			"oapi.output.panel.SetRgbLedOff"
#define	RPC_OAPI_OUTPUT_PANEL_SetRgbLedDimm			"oapi.output.panel.SetRgbLedDimm"
#define	RPC_OAPI_OUTPUT_PANEL_SetRgbLedBlink		"oapi.output.panel.SetRgbLedBlink"
#define	RPC_OAPI_OUTPUT_PANEL_SetPowerLedState		"oapi.output.panel.SetPowerLedState"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern HERROR 	OAPI_OUTPUT_PANEL_Init(void);
extern HERROR	OAPI_OUTPUT_PANEL_SetLed(DxLedId_e eLedId, DxLedStatus_e eStatus);
extern HERROR	OAPI_OUTPUT_PANEL_SetLedAuto(void);
extern HERROR	OAPI_OUTPUT_PANEL_SetLedCategory(DxPanelCategory_e eCategory);
extern HERROR	OAPI_OUTPUT_PANEL_ClearLed(void);
extern HERROR	OAPI_OUTPUT_PANEL_SetPanel(HINT32 nPriority, const HCHAR *pszString, DxPanelAlign_e ePanelAlign);
extern HERROR	OAPI_OUTPUT_PANEL_SetPanelAuto(HINT32 nPriority);
extern HERROR	OAPI_OUTPUT_PANEL_SetPanelDimLevel(HINT32 nPriority, HINT32 nDimLevel);
extern HERROR	OAPI_OUTPUT_PANEL_SetPanelAnimation(HINT32 nPriority);
extern HERROR	OAPI_OUTPUT_PANEL_SetRgbLed(DxLedId_e eLedId, DxRgbLedColor_e eLedColor, HBOOL bBlink);
extern HERROR	OAPI_OUTPUT_PANEL_SetRgbLedOff(DxLedId_e eLedId);
extern HERROR	OAPI_OUTPUT_PANEL_SetRgbLedDimm(DxLedId_e eLedId, DxRgbLedDimmLevel_e eDimmLevel);
extern HERROR	OAPI_OUTPUT_PANEL_SetRgbLedBlink(DxLedId_e eLedId, HBOOL bBlink);
extern HERROR	OAPI_OUTPUT_PANEL_SetPowerLedState(DxPowerLedStatus_e eLedStatus);
#endif // __OAPI_OUTPUT_HDMI_H__

