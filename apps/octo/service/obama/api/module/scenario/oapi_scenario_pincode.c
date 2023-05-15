/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_scenario_pincode.c
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
#define	ENTER_SCENARIO		HxSEMT_Get(s_ulScenarioPinSemId)
#define	LEAVE_SCENARIO		HxSEMT_Release(s_ulScenarioPinSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32		s_ulScenarioPinSemId;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____NOTIFICATION_CALLBACKS_____
STATIC void oapi_scenario_pincode_onChecked(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbChecked = (HBOOL*)userdata;

	if (hErr == ERR_OK)
		*pbChecked = TRUE;
	else
		*pbChecked = FALSE;
}

STATIC void oapi_scenario_pincode_onVerified(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbVerified = (HBOOL*)userdata;


	if (hErr == ERR_OK)
		*pbVerified = TRUE;
	else
		*pbVerified = FALSE;
}

STATIC void oapi_scenario_pincode_onSet(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HERROR	*peErr = (HERROR*)userdata;

	*peErr = hErr;
}

STATIC void oapi_scenario_pincode_onSetBlock(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HERROR	*peErr = (HERROR*)userdata;

	*peErr = hErr;
}

STATIC void oapi_scenario_pincode_onGetBlock(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HBOOL	*pbBlock = (HBOOL*)userdata;

	if (hErr == ERR_OK)
		*pbBlock = TRUE;
	else
		*pbBlock = FALSE;
}


#define ___GLOBAL_FUNCTIONS___

HERROR	OAPI_SCENARIO_PINCODE_Init(void)
{
	HxSEMT_Create(&s_ulScenarioPinSemId, "scen_pin", HxSEMT_FIFO);

	return ERR_OK;
}

HERROR	OAPI_SCENARIO_PINCODE_DeInit(void)
{
	return ERR_OK;
}

HERROR	OAPI_SCENARIO_PINCODE_Check(const HCHAR *pszPinStr, HBOOL *pbChecked)
{
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_scenario_pincode_onChecked, (void*)pbChecked, RPC_OAPI_SCENARIO_PINCODE_CHECK, "s", pszPinStr);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");
		*pbChecked = FALSE;

		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	OAPI_SCENARIO_PINCODE_Verify(const HCHAR *pszPinStr, HBOOL *pbVerified)
{
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_scenario_pincode_onVerified, (void*)pbVerified, RPC_OAPI_SCENARIO_PINCODE_VERIFY, "s", pszPinStr);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");
		*pbVerified = FALSE;


		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	OAPI_SCENARIO_PINCODE_Reset(const HCHAR *pszPinStr)
{
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if(hErr == ERR_OK)
	{
		nReqId = HLIB_RPC_Call(nRpcHandle, RPC_OAPI_SCENARIO_PINCODE_RESET, "s", pszPinStr);
	}

	return ERR_OK;
}

HERROR	OAPI_SCENARIO_PINCODE_Set(const HCHAR *pszOldPinStr, const HCHAR *pszNewPinStr)
{
	HINT32	nReqId;
	HERROR	err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_scenario_pincode_onSet, (void*)&err, RPC_OAPI_SCENARIO_PINCODE_SET, "ss", pszOldPinStr, pszNewPinStr);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		return ERR_FAIL;
	}

	return err;
}

HERROR	OAPI_SCENARIO_PINCODE_SetTemporaryBlock(const HCHAR *pszPinStr, HBOOL bBlock)
{
	HINT32	nReqId;
	HERROR	err;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_scenario_pincode_onSetBlock, (void*)&err, RPC_OAPI_SCENARIO_PINCODE_SETBLOCK, "si", pszPinStr, (bBlock ? 1 : 0));
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");

		return ERR_FAIL;
	}

	return err;
}

HERROR	OAPI_SCENARIO_PINCODE_GetTemporaryBlock(HBOOL *pbBlock)
{
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_scenario_pincode_onGetBlock, (void*)pbBlock, RPC_OAPI_SCENARIO_PINCODE_GETBLOCK, NULL);
	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("error to join - \n");
		*pbBlock = FALSE;
		return ERR_FAIL;
	}

	return ERR_OK;
}

