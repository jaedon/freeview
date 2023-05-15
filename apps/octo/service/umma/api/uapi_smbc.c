/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  uapi_smbc.c
	@brief

	Description:  									\n
	Module: Umma Samba Client API Module			\n

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
#include	<hlib.h>
#include	<vkernel.h>

#include	<uapi.h>
#include	<umma_int.h>

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
static UAPI_NotiSmbc_t s_pfnUapiSmbcListener = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

static HERROR	uapi_smbc_event_AddEventListener(UAPI_t *uapi, HINT32 nRequestId, void *pvUser, UAPI_Noti_t onEventOccurred)
{
	UAPI_CBINFO_t	*pstCbInfo;

	pstCbInfo = (UAPI_CBINFO_t*)HLIB_HASH_Lookup(uapi->pstCallbackTable, (void*)nRequestId);
	if (pstCbInfo)
	{
		HxLOG_Error("Callback is already registered! [%d]\n", nRequestId);
		return ERR_FAIL;
	}

	pstCbInfo = (UAPI_CBINFO_t*)UAPI_Calloc(sizeof(UAPI_CBINFO_t));
	pstCbInfo->nRequestId = nRequestId;
	pstCbInfo->cb.pfnEventCallback = onEventOccurred;
	pstCbInfo->pvUser = pvUser;
	HLIB_HASH_Insert(uapi->pstCallbackTable, (void*)nRequestId, (void*)pstCbInfo);

	return ERR_OK;
}

static HERROR	uapi_smbc_onMountResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
//	UAPI_t	*pstUapi = (UAPI_t*)pvUserData;

	UAPI_SMBC_MOUNT_t stMountInfo;
	HINT32	_nRequestId = 0;
	HINT32	eEvt = 0;

	if (nArgc != 3)
	{
		HxLOG_Error("nArgc is not valid [%d:4]\n", nArgc);
		return ERR_FAIL;
	}

	_nRequestId = HxOBJECT_INT(apArgv[0]);
	eEvt = HxOBJECT_INT(apArgv[1]);

	if ( NULL == s_pfnUapiSmbcListener)
		return ERR_FAIL;

	HxSTD_MemCopy(&stMountInfo, HxOBJECT_BIN_DATA(apArgv[2]), HxOBJECT_BIN_SIZE(apArgv[2]));

	s_pfnUapiSmbcListener( eEvt, &stMountInfo, NULL );
	return ERR_OK;
}

static HERROR	uapi_smbc_onScanResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
//	UAPI_t	*pstUapi = (UAPI_t*)pvUserData;

	UAPI_SMBC_SERVER_t stServerInfo;
	HINT32			_nRequestId;
	HINT32			eEvt = 0;
//	HCHAR *pEvent = NULL;

	if (nArgc != 4)
	{
		HxLOG_Error("nArgc is not valid [%d:4]\n", nArgc);
		return ERR_FAIL;
	}

	_nRequestId = HxOBJECT_INT(apArgv[0]);
	eEvt = HxOBJECT_INT(apArgv[1]);

	if ( NULL == s_pfnUapiSmbcListener )
		return ERR_FAIL;

	HxSTD_MemCopy(&stServerInfo, HxOBJECT_BIN_DATA(apArgv[2]), HxOBJECT_BIN_SIZE(apArgv[2]));
	stServerInfo.pstSharedList = (UAPI_SMBC_SHARED_t *)HxOBJECT_BIN_DATA(apArgv[3]);
	HxLOG_Debug("Call registered function : [%d]\n", nRequestId);

	s_pfnUapiSmbcListener( eEvt, &stServerInfo, NULL );
	return ERR_OK;
}

static UAPI_t*	uapi_smbc_getInstance(void)
{
	static UAPI_t	*s_uapi = NULL;

	if (s_uapi == NULL)
	{
		HERROR	hErr;

		s_uapi = (UAPI_t*)UAPI_Calloc(sizeof(UAPI_t));

		HxSEMT_Create(&s_uapi->ulSemId, "uapismbcsem", 0);

		HxSEMT_Get(s_uapi->ulSemId);
		hErr = UAPI_RpcProtocol_GetRpcHandle(&s_uapi->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error !! (GetRpcHandle)\n");
			HxLOG_Assert(s_uapi->nRpcHandle);
		}

		s_uapi->pstCallbackTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UAPI_Free_CB);

		//	Register UMMA Event Notifier
		HLIB_RPC_RegisterNotifyCallback(s_uapi->nRpcHandle, RPC_UMMA_SMBC_onScanResult,   "iibb", (void*)s_uapi, uapi_smbc_onScanResult);
		HLIB_RPC_RegisterNotifyCallback(s_uapi->nRpcHandle, RPC_UMMA_SMBC_onMountResult,   "iib", (void*)s_uapi, uapi_smbc_onMountResult);

	}
	else
	{
		HxSEMT_Get(s_uapi->ulSemId);
	}

	return s_uapi;
}

static void		uapi_smbc_releaseInstance(UAPI_t *uapi)
{
	HxSEMT_Release(uapi->ulSemId);
}

void	UAPI_SMBC_Init(void)
{
	uapi_smbc_releaseInstance(uapi_smbc_getInstance());
}

HERROR	UAPI_SMBC_SetNotifier( UAPI_NotiSmbc_t pfnApkListener )
{
	HxLOG_Trace();
	s_pfnUapiSmbcListener = pfnApkListener;
	return ERR_OK;
}

HINT32	UAPI_SMBC_StartScan(void)
{
	UAPI_t	*uapi;
	HINT32	nReqId = -1;

	uapi = uapi_smbc_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBC_ScanStart, NULL );
	}
	uapi_smbc_releaseInstance(uapi);

	return nReqId;
}

HINT32	UAPI_SMBC_StopScan(void)
{
	UAPI_t	*uapi;
	HINT32	nReqId = -1;

	uapi = uapi_smbc_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBC_ScanStop, NULL);
	}

	uapi_smbc_releaseInstance(uapi);

	return nReqId;
}

HERROR	UAPI_SMBC_Mount(const HCHAR *pszIp, const HCHAR *pszSharedName, const HCHAR *pszId, const HCHAR *pszPasswd, const HCHAR *pszPath )
{
	UAPI_t	*uapi;
	HINT32	nReqId;

	uapi = uapi_smbc_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBC_Mount, "sssss", pszIp, pszSharedName, pszPath, pszId, pszPasswd);
	}
	uapi_smbc_releaseInstance(uapi);

	return ERR_OK;
}

HERROR	UAPI_SMBC_Unmount(const HCHAR *pszIp, const HCHAR *pszSharedName, const HCHAR *pszPath)
{
	UAPI_t	*uapi;
	HINT32	nReqId;

	uapi = uapi_smbc_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBC_Unmount, "sss", pszIp, pszSharedName, pszPath);
	}
	uapi_smbc_releaseInstance(uapi);

	return ERR_OK;
}

HERROR	UAPI_SMBC_UnmountAll()
{
	UAPI_t	*uapi;
	HINT32	nReqId;

	uapi = uapi_smbc_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBC_UnmountAll, NULL);
	}
	uapi_smbc_releaseInstance(uapi);

	return ERR_OK;
}

