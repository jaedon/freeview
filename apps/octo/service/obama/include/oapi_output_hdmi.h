/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_hdmi.h
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

#ifndef __OAPI_OUTPUT_HDMI_H__
#define __OAPI_OUTPUT_HDMI_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_OUTPUT_HDMI_Init					"oapi.output.hdmi.Init"
#define RPC_OAPI_OUTPUT_HDMI_Set3DSelect			"oapi.output.hdmi.Set3DSelect"
#define RPC_OAPI_OUTPUT_HDMI_SetCecEnable			"oapi.output.hdmi.SetCecEnable"
#define RPC_OAPI_OUTPUT_HDMI_CEC_SetVolumeEnable	"oapi.output.hdmi.SetCecVolumeEnable"
#define RPC_OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed	"oapi.output.hdmi.CecUserCtrllpressed"
#define RPC_OAPI_OUTPUT_HDMI_SetNflixDrmStatus      "oapi.output.hdmi.SetNflixDrmStatus"

#define RPC_OBAMA_OUTPUT_HDMI_onStatusChanged		"obama.output.hdmi.onStatusChanged"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL					bSupportCec;
	HBOOL					bCecEnable;
    HBOOL                   bCecVolumeCtrlEnable;
	HBOOL					bPlugIn;
	DxHdcpVersion_e			eHdcpVer;
	DxHdcpStatus_e			eHdcpStatus;
    DxNflixDrmStatus_e      eNflixDrmStatus;
	DxHdmi3DSelect_e		eHdmi3DSelect;
	DxVideo3DFormat_b		ul3DFormatCapability;
	DxVideo3DFormat_e		eHdmi3DStatus;
	DxResolutionStatus_b	ulResolutionCapability;
	DxHdmiEdidInfo_t		stHdmiEdidInfo;
} OxOutputHdmi_Cache_t;

typedef void (*OxOutputHdmi_Notifier_t) (void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR OAPI_OUTPUT_HDMI_Init(OxOutputHdmi_Notifier_t fnNotifier);
HERROR OAPI_OUTPUT_HDMI_DeInit(void);
HERROR OAPI_OUTPUT_HDMI_Set3DSelect(DxHdmi3DSelect_e eHdmi3DSelect);
HERROR OAPI_OUTPUT_HDMI_SetCecEnable(HBOOL bEnable);
HERROR OAPI_OUTPUT_HDMI_GetStatus(OxOutputHdmi_Cache_t *pstStatus);
HERROR OAPI_OUTPUT_HDMI_SetCecVolumeCtrlEnable(HBOOL bEnable);
HERROR OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed(DxHdmiUserCtrlPressed_e eParam);
HERROR OAPI_OUTPUT_HDMI_GetNetflixDrmStatus(DxNflixDrmStatus_e *peNflixDrmStatus);
HERROR OAPI_OUTPUT_HDMI_SetNetflixDrmStatus(DxNflixDrmStatus_e eNflixDrmStatus);

#endif // __OAPI_OUTPUT_HDMI_H__

