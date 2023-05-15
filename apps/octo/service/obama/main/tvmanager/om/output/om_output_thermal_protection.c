/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_output_thermal_protection.c
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

#include <db_common.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_output.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>

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
STATIC HINT32						s_nRpcHandle;
STATIC OxOutputThermal_Cache_t		s_stOmOutputThermalCache;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_output_thermal_CheckStatusChange(void)
{
	HBOOL					bChanged = FALSE;
	HBOOL					bOverTemp = FALSE;
	HBOOL					bFanLock = FALSE;

	HxLOG_Error("%s()\n", __FUNCTION__);

	(void)SVC_OUT_GetOverTemp(&bOverTemp);
	if(bOverTemp != s_stOmOutputThermalCache.bOverTemp)
	{
		bChanged = TRUE;
		s_stOmOutputThermalCache.bOverTemp = bOverTemp;
	}

	(void)SVC_OUT_GetFanLock(&bFanLock);
	if(bFanLock != s_stOmOutputThermalCache.bFanLock)
	{
		bChanged = TRUE;
		s_stOmOutputThermalCache.bFanLock = bFanLock;
	}

	if(bChanged == TRUE)
	{
		HxLOG_Error("\n\n\n\t\t\t %s(Notify with reqID:0x%x)\n\n\n", __FUNCTION__, s_nRpcHandle);
		HLIB_RPC_NotifyWithRequestId(s_nRpcHandle, RPC_OBAMA_OUTPUT_THERMAL_onStatusChanged, s_nRpcHandle, "b", &s_stOmOutputThermalCache, sizeof(OxOutputThermal_Cache_t));
	}
}

#define _____OAPI_COMMAND_FUNCTIONS_____
#if 0
STATIC HERROR om_output_thermal_Shutdown(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Error("%s()\n", __FUNCTION__);

	HxLOG_Assert(nArgc == 0);

	//SVC_SYS_SetLedAuto();

	return ERR_OK;
}
#endif

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_output_thermal(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Error("%s(eMEVT_BUS_CREATE)\n", __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
		case eSEVT_OUT_OVERTEMPERATURE:
			HxLOG_Error("%s(eSEVT_OUT_OVERTEMPERATURE)\n", __FUNCTION__);
			om_output_thermal_CheckStatusChange();
			break;
		case eSEVT_OUT_FANLOCK:
			HxLOG_Error("%s(eSEVT_OUT_FANLOCK)\n", __FUNCTION__);
			om_output_thermal_CheckStatusChange();
			break;
#endif

		default:
			break;
	}

	return MESSAGE_PASS;
}


STATIC HERROR om_output_thermal_Init(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Error("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stOmOutputThermalCache, sizeof(OxOutputThermal_Cache_t));

	return ERR_OK;
}

#define ___GLOBAL_FUNCTIONS___
HERROR OM_OUTPUT_THERMAL_Init(void)
{
	HERROR					err;

	HxLOG_Error("%s()\n", __FUNCTION__);

	err = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RpcProtocol_GetRpcHandle() ERROR !!! ERROR !!! ERROR !!! ERROR !!!\n");
	}

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_output_thermal, 0, 0, 0, 0);

	//	-------   RPC Methods	------- //
	err = ERR_OK;
	err = HLIB_RPC_RegisterRequest (s_nRpcHandle, RPC_OAPI_OUTPUT_THERMAL_Init, NULL, om_output_thermal_Init, NULL,
							"Activate the output.thermal module: (void)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_OUTPUT_THERMAL_Init, err);
	}

#if 0
	//	-------   RPC Methods   -------	//
	err = ERR_OK;
	err |= HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_THERMAL_Shutdown, NULL, om_output_thermal_Shutdown, NULL,
							"Set LED Auto : (void)");

	if (err != ERR_OK)
	{
		HxLOG_Error("error : obama fail to register method!\n");
		return ERR_FAIL;
	}
#endif

	//	-------   Notifier  Callbakcs------- //
	err = HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OBAMA_OUTPUT_THERMAL_onStatusChanged, "b",
							"Thermal protection output parameters and status changed.\n"
							"\t   - (OxOutputThermal_Cache_t *pstOutputThermalStatus)\n"
							);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterCallback(%s) returned error(%d)\n", RPC_OBAMA_OUTPUT_THERMAL_onStatusChanged, err);
	}

	return ERR_OK;
}




