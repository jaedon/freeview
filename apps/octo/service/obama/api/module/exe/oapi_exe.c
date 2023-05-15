/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_exe.c
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
#include <oapi_exe.h>

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

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____NOTIFICATION_CALLBACKS_____

#define ___GLOBAL_FUNCTIONS___



static HCHAR * oapi_exe_MakeFunctionName(const HCHAR *szCmdName, const HCHAR *szProcName)
{
	HUINT32 ulNameLength = 0;
	HCHAR * szMethodName = NULL;
	ulNameLength = HxSTD_StrLen(szCmdName) + HxSTD_StrLen(szProcName) + 2;
	if(ulNameLength > OxEXE_PROCNAME_LENGTH)
		ulNameLength = OxEXE_PROCNAME_LENGTH;

	if(ulNameLength) szMethodName = (HCHAR *)OxMEM_Calloc(ulNameLength);
	if(szMethodName) snprintf(szMethodName, ulNameLength, "%s.%s", szCmdName, szProcName);
	return szMethodName;
}

HERROR	OAPI_EXE_Start(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 ulParam1, HUINT32 ulParam2,HUINT32 ulParam3)
{
	HCHAR * szMethodName = oapi_exe_MakeFunctionName(RPC_OAPI_EXE_START, szProcName);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();


	if(szMethodName)
	{
		HxLOG_Debug("[%s] szMethodName[%s]\n", __FUNCTION__, szMethodName);
		HLIB_RPC_Call (nRpcHandle, szMethodName, szSignature, ulParam1, ulParam2, ulParam3);
		OxMEM_Free(szMethodName);
	}
	return ERR_OK;
}

HERROR	OAPI_EXE_Set(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 ulParam1, HUINT32 ulParam2,HUINT32 ulParam3)
{
	HCHAR * szMethodName = oapi_exe_MakeFunctionName(RPC_OAPI_EXE_SET, szProcName);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("[%s] szMethodName[%s]\n", __FUNCTION__, szMethodName);

	if(szMethodName)
	{
		HLIB_RPC_Call (nRpcHandle, szMethodName, szSignature, ulParam1, ulParam2, ulParam3);
		OxMEM_Free(szMethodName);
	}
	return ERR_OK;
}

HERROR	OAPI_EXE_SetByBin(const HCHAR *szProcName, void *pvBin, HUINT32 ulSize)
{
	HCHAR * szMethodName = oapi_exe_MakeFunctionName(RPC_OAPI_EXE_SETBIN, szProcName);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("[%s] szMethodName[%s]\n", __FUNCTION__, szMethodName);

	if(szMethodName)
	{
		HLIB_RPC_Call (nRpcHandle, szMethodName, "b", pvBin, ulSize);
		OxMEM_Free(szMethodName);
	}
	return ERR_OK;
}


HERROR	OAPI_EXE_Stop(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 ulParam1, HUINT32 ulParam2,HUINT32 ulParam3)
{
	HCHAR * szMethodName = oapi_exe_MakeFunctionName(RPC_OAPI_EXE_STOP, szProcName);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Trace();
	HxLOG_Debug("[%s] szMethodName[%s]\n", __FUNCTION__, szMethodName);

	if(szMethodName)
	{
		HLIB_RPC_Call (nRpcHandle, szMethodName, szSignature, ulParam1, ulParam2, ulParam3);
		OxMEM_Free(szMethodName);
	}
	return ERR_OK;
}

HERROR	OAPI_EXE_Notify(const HCHAR *szProcName, HxRPC_Noti_t pfnCallback)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HCHAR * szMethodName = oapi_exe_MakeFunctionName(RPC_OAPI_EXE_NOTI, szProcName);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();
	HxLOG_Debug("[%s] szMethodName[%s]\n", __FUNCTION__, szMethodName);

	if(szMethodName)
	{
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, szMethodName, NULL, NULL, (HxRPC_Noti_t)pfnCallback);
		OxMEM_Free(szMethodName);
	}
	return ERR_OK;
}

