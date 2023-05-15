/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_thermal_protection.c
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

/*
	Make this like audio, video, hdmi.
*/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// #define OUTPUT_THERMAL_INIT_ASYNC

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC HULONG						s_ulOapiOutputThermalSemId = 0L;
STATIC OxOutputThermal_Cache_t		s_stOapiOutputThermalCache;

#define	ENTER_OUTPUT_THERMAL_CACHE	VK_SEM_Get(s_ulOapiOutputThermalSemId)
#define	LEAVE_OUTPUT_THERMAL_CACHE	VK_SEM_Release(s_ulOapiOutputThermalSemId)

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL					s_bOutputThermalCacheInited = FALSE;
STATIC HBOOL					s_bOutputThermalInited = FALSE;

STATIC OxOutputThermal_Notifier_t	s_fnOutputThermalApkNotifier;
STATIC HINT32						s_nRpcHandle;


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____NOTIFICATION_CALLBACKS_____
STATIC HERROR oapi_output_thermal_onInited(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Error("%s()\n", __FUNCTION__);

	HxLog_ExprCheckReturnValue(nArgc != 1, ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputThermal_Cache_t)), ERR_FAIL);

	ENTER_OUTPUT_THERMAL_CACHE;
	s_stOapiOutputThermalCache = *(OxOutputThermal_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	s_bOutputThermalCacheInited = TRUE;
	LEAVE_OUTPUT_THERMAL_CACHE;

	return ERR_OK;
}

STATIC HERROR oapi_output_thermal_onStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Error("%s()\n", __FUNCTION__);

	HxLog_ExprCheckReturnValue(nArgc != 1, ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputThermal_Cache_t)), ERR_FAIL);

	ENTER_OUTPUT_THERMAL_CACHE;
	s_stOapiOutputThermalCache = *(OxOutputThermal_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	s_bOutputThermalCacheInited = TRUE;
	LEAVE_OUTPUT_THERMAL_CACHE;

	if(s_fnOutputThermalApkNotifier != NULL)
	{
		(s_fnOutputThermalApkNotifier)();
	}
	return ERR_OK;
}

#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_OUTPUT_THERMAL_Init(OxOutputThermal_Notifier_t fnNotifier)
{
	HERROR	hErr;

	HxLOG_Error("%s()\n", __FUNCTION__);

	if (s_bOutputThermalInited != TRUE)
	{
		if(VK_SEM_Create(&s_ulOapiOutputThermalSemId, "OapiOutputThermalSema", VK_SUSPENDTYPE_PRIORITY)  != VK_OK)
		{
			HxLOG_Error("[%s] Error in VK_SEM_Create()",  __FUNCTION__);
			return ERR_FAIL;
		}

		hErr = OAPI_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has OAPI_RpcProtocol_GetRpcHandle() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		s_fnOutputThermalApkNotifier = fnNotifier;

		hErr = HLIB_RPC_RegisterNotifyCallback(s_nRpcHandle, RPC_OBAMA_OUTPUT_THERMAL_onStatusChanged, "b", NULL, oapi_output_thermal_onStatusChanged);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has HLIB_RPC_RegisterNotifyCallback() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		(void) HLIB_RPC_Request(s_nRpcHandle, (HxRPC_Response_t)oapi_output_thermal_onInited, NULL, RPC_OAPI_OUTPUT_THERMAL_Init, NULL);
		s_bOutputThermalInited = TRUE;
	}

	return ERR_OK;
}




