/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_scenario_pincode.c
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
* MS949 encoding \uc744 \uc0ac\uc6a9\ud558\ub294 source file \uc785\ub2c8\ub2e4.
* MS949 encoding\u3092\u4f7f\u7528\u3057\u3066 source file\u3067\u3059\u3002
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
#include <dlib.h>
#include <hapi.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <db_param.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_pg.h>
#include <mgr_live.h>


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

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
STATIC const HCHAR*	om_scenario_pincode_GetSystemPincode(void)
{
	static HCHAR	s_szPincodeStr[16];

	if (DB_PARAM_GetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32 *)s_szPincodeStr, 16) != ERR_OK)
	{
		DB_PARAM_SetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32)"0000", 0);
		HLIB_STD_StrNCpySafe(s_szPincodeStr, "0000", 16);
	}

	return s_szPincodeStr;
}
STATIC HERROR	om_scenario_pincode_Check(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HUINT32 ulActionId = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	ulActionId = MGR_ACTION_GetMainActionId();

	if (HxSTD_StrCmp(om_scenario_pincode_GetSystemPincode(), HxOBJECT_STR(apArgv[0])) == 0)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
		BUS_PostMessage(NULL, eMEVT_PG_PIN_CHECK_OK, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);
	} else
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
		BUS_PostMessage(NULL, eMEVT_PG_PIN_CHECK_DENIED, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);
	}

	return ERR_OK;
}

STATIC HERROR	om_scenario_pincode_Verify(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{

	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	if (HxSTD_StrCmp(om_scenario_pincode_GetSystemPincode(), HxOBJECT_STR(apArgv[0])) == 0)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	} else
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
	}

	return ERR_OK;
}

STATIC HERROR	om_scenario_pincode_Set(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	if (HxSTD_StrCmp(om_scenario_pincode_GetSystemPincode(), HxOBJECT_STR(apArgv[0])) == 0)
	{
		DB_PARAM_SetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32)HxOBJECT_STR(apArgv[1]), 0);
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	} else
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
	}

	return ERR_OK;
}

STATIC HERROR	om_scenario_pincode_Reset(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Warning("PINCODE RESET function is called!!! <--- something is wrong... \n");
	DB_PARAM_SetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32)HxOBJECT_STR(apArgv[0]), 0);

	return ERR_OK;
}

STATIC HERROR	om_scenario_pincode_SetBlock(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HUINT32 ulActionId = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	ulActionId = MGR_ACTION_GetMainActionId();

	if (HxSTD_StrCmp(om_scenario_pincode_GetSystemPincode(), HxOBJECT_STR(apArgv[0])) == 0)
	{
		if (HxOBJECT_INT(apArgv[1]))
		{
		}
		else
		{
			BUS_PostMessage(NULL, eMEVT_PG_PIN_CHECK_OK, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);
		}
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	}
	else
	{
		BUS_PostMessage(NULL, eMEVT_PG_PIN_CHECK_DENIED, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);
	}

	return ERR_OK;
}

STATIC HERROR	om_scenario_pincode_GetBlock(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL		bPassed;
	HUINT32		ulActionId;
	Handle_t	hSvcHandle;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	ulActionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetActualSvcHandle(ulActionId, &hSvcHandle) == ERR_OK)
	{
		if (hSvcHandle != HANDLE_NULL)
		{
			bPassed = MGR_LIVE_SvcLockIsPassed(ulActionId, hSvcHandle);
			if (bPassed)
			{
				HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
				return ERR_OK;
			}
		}
	}

	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL, NULL);

	return ERR_OK;
}

HERROR	OM_SCENARIO_PINCODE_Init(void)
{
	HERROR	err = ERR_OK;
	HINT32	nRpcHandle = 0;

	err |= OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(err == ERR_OK)
	{

		err  = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_CHECK, "s", (HxRPC_Func_t)om_scenario_pincode_Check, NULL,
								"Check PINCODE (const HCHAR *pszPinCode)");
		err |= HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_VERIFY, "s", (HxRPC_Func_t)om_scenario_pincode_Verify, NULL,
								"Verify PINCODE (const HCHAR *pszPinCode)");
		err |= HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_SET, "ss", (HxRPC_Func_t)om_scenario_pincode_Set, NULL,
								"Set Pincode (const HCHAR *pszOldPin, const HCHAR *pszNewPin)");
		err |= HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_RESET, "s", (HxRPC_Func_t)om_scenario_pincode_Reset, NULL,
								"Reset PINCODE (const HCHAR *pszPincode)");
		err |= HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_SETBLOCK, "si", (HxRPC_Func_t)om_scenario_pincode_SetBlock, NULL,
								"Set TemporaryBlock (HBOOL bBlocked)");
		err |= HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_GETBLOCK, NULL, (HxRPC_Func_t)om_scenario_pincode_GetBlock, NULL,
								"Get TemporaryBlock ");
	}

	return err;
}
