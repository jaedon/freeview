/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  uapi_smbs.c
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
static UAPI_NotiSmbs_t s_pfnUapiSmbsListener = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static void		uapi_smbs_ipc_listener(void *arg);
#if defined(CONFIG_DEBUG)
static void		uapi_smbs_debug_InitCommandTools(void);
#endif

static HERROR	uapi_smbs_event_AddEventListener(UAPI_t *uapi, HINT32 nRequestId, void *pvUser, UAPI_Noti_t onEventOccurred)
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

static HERROR	uapi_smbs_checkPasswdCb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_MSG_t *pData = (UMMA_MSG_t *)pvUserData;
	HINT32 isValid = FALSE;
	if (nArgc != 1)
	{
		HxLOG_Error("nArgc is not valid [%d:1]\n", nArgc);
		return ERR_FAIL;
	}

	isValid = HxOBJECT_INT(apArgv[0]);

	pData->nParam1 = err;
	pData->nParam2 = isValid;

	return ERR_OK;
}

static HERROR	uapi_smbs_getSharedCb(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 nCnt = 0;
	HINT32 listSize = 0;
	UAPI_SMBS_SHARED_t *pSharedList = NULL;
	UAPI_SMBS_SHARED_t *pRetList = NULL;
	UMMA_MSG_t *pData = (UMMA_MSG_t *)pvUserData;
	if (nArgc != 1)
	{
		HxLOG_Error("nArgc is not valid [%d:1]\n", nArgc);
		return ERR_FAIL;
	}

	listSize = HxOBJECT_BIN_SIZE(apArgv[0]);
	if ( listSize < sizeof(UAPI_SMBS_SHARED_t) )
	{
		nCnt = 0;
		pSharedList = NULL;
	}
	else
	{
		nCnt = listSize / sizeof(UAPI_SMBS_SHARED_t);
		pSharedList = (UAPI_SMBS_SHARED_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	}

	pData->nParam1 = err;
	pData->nParam2 = nCnt;
	if ( nCnt )
	{
		pRetList = (UAPI_SMBS_SHARED_t *)HLIB_MEM_Calloc(listSize);
		HxSTD_MemCopy( pRetList, pSharedList, listSize );
	}
	pData->nParam3 = (HINT32)pRetList;
#ifdef CONFIG_DEBUG
	HxLOG_Debug("[UAPI_SMBS] Requset SHARED LIST Count(%d)\n", nCnt);
	{
		HINT32 i;
		for ( i=0; i < nCnt; i++ )
		{
			HxLOG_Debug("[UAPI_SMBS] Request Shared Path : %s\n", pSharedList[i].szPath);
		}
	}
#endif


	return ERR_OK;
}

static HERROR	uapi_smbs_getPublicAccess(HERROR err, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_MSG_t *pData = (UMMA_MSG_t *)pvUserData;
	HINT32 isAccess = FALSE;
	if (nArgc != 1)
	{
		HxLOG_Error("nArgc is not valid [%d:1]\n", nArgc);
		return ERR_FAIL;
	}

	isAccess = HxOBJECT_INT(apArgv[0]);

	pData->nParam1 = err;
	pData->nParam2 = isAccess;

	return ERR_OK;
}

static HERROR	uapi_smbs_onCallResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UAPI_t	*pstUapi = (UAPI_t*)pvUserData;
	UAPI_SMBS_INFO_t *pSmbsInfo = NULL;

	HINT32			eEvt = 0;
	HINT32			nRetErr = ERR_FAIL;

	pstUapi;
	if (nArgc != 4)
	{
		HxLOG_Error("nArgc is not valid [%d:5]\n", nArgc);
		return ERR_FAIL;
	}

	eEvt = HxOBJECT_INT(apArgv[0]);
	nRetErr	= (UAPI_SMBS_EVT_e)HxOBJECT_INT(apArgv[1]);
	HxLOG_Debug("[UAPI_FTPS] EVENT : %d \n", eEvt);

	pSmbsInfo = (UAPI_SMBS_INFO_t *)HxOBJECT_BIN_DATA(apArgv[2]);

	switch ( eEvt )
	{
		case eUAPI_SMBS_EVT_SERVER_START :
			break;
		case eUAPI_SMBS_EVT_SERVER_STOP :
			break;
		case eUAPI_SMBS_EVT_CONFIG_CHANGE :
			break;
		case eUAPI_SMBS_EVT_PASSWD_CHANGE :
			break;
		case eUAPI_SMBS_EVT_PERMISSION_CHANGE :
			break;
		case eUAPI_SMBS_EVT_RESPONE_SHAREDLIST :
			{
				HINT32 nCnt = 0;
				HINT32 i = 0;
				HINT32 listSize = 0;
				UAPI_SMBS_SHARED_t *pSharedList = NULL;

				listSize = HxOBJECT_BIN_SIZE(apArgv[3]);
				if ( listSize < sizeof(UAPI_SMBS_SHARED_t) )
				{
					nCnt = 0;
					pSharedList = NULL;
				}
				else
				{
					nCnt = listSize / sizeof(UAPI_SMBS_SHARED_t);
					pSharedList = (UAPI_SMBS_SHARED_t *)HxOBJECT_BIN_DATA(apArgv[3]);
				}

				HxLOG_Debug("[UAPI] SHARED LIST Count(%d)\n", nCnt);
				while ( i < nCnt )
				{
					HxLOG_Debug("[UAPI] Shared Path : %s\n", pSharedList[i].szPath);
					i++;
				}

			}
			break;
		case eUAPI_SMBS_EVT_RESPONE_SERVERINFO :
			break;
		case eUAPI_SMBS_EVT_RESPONE_CHKPASSWD :
			break;
		default :
			return ERR_OK;
	}

	if ( NULL == s_pfnUapiSmbsListener )
		return ERR_FAIL;

	HxLOG_Debug("Call registered function : [%d]\n", nRequestId);

	s_pfnUapiSmbsListener( eEvt, (void *)nRetErr, (void *)pSmbsInfo );
	return ERR_OK;
}

static UAPI_t*	uapi_smbs_getInstance(void)
{
	static UAPI_t	*s_uapi = NULL;

	if (s_uapi == NULL)
	{
		HERROR	hErr;

		s_uapi = (UAPI_t*)UAPI_Calloc(sizeof(UAPI_t));

		HxSEMT_Create(&s_uapi->ulSemId, "uapismbssem", 0);

		HxSEMT_Get(s_uapi->ulSemId);
		hErr = UAPI_RpcProtocol_GetRpcHandle(&s_uapi->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error !! (GetRpcHandle)\n");
			HxLOG_Assert(s_uapi->nRpcHandle);
		}

		s_uapi->pstCallbackTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, UAPI_Free_CB);

		//	Register UMMA Event Notifier
		HLIB_RPC_RegisterNotifyCallback(s_uapi->nRpcHandle, RPC_UMMA_SMBS_onRequestResult,   "iibb", (void*)s_uapi, uapi_smbs_onCallResult);

	}
	else
	{
		HxSEMT_Get(s_uapi->ulSemId);
	}

	return s_uapi;
}

static void		uapi_smbs_releaseInstance(UAPI_t *uapi)
{
	HxSEMT_Release(uapi->ulSemId);
}

void	UAPI_SMBS_Init(void)
{
	uapi_smbs_releaseInstance(uapi_smbs_getInstance());
#if defined(CONFIG_DEBUG)
	uapi_smbs_debug_InitCommandTools();
#endif
}

HERROR	UAPI_SMBS_SetNotifier( UAPI_NotiSmbs_t pfnApkListener )
{
	HxLOG_Trace();
	s_pfnUapiSmbsListener = pfnApkListener;
	return ERR_OK;
}

HINT32	UAPI_SMBS_StartServer(void)
{
	UAPI_t	*uapi;
	HINT32	nReqId = -1;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_Start, NULL );
	}
	uapi_smbs_releaseInstance(uapi);

	return nReqId;
}

HINT32	UAPI_SMBS_StopServer(void)
{
	UAPI_t	*uapi;
	HINT32	nReqId = -1;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		nReqId = HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_Stop, NULL);
	}

	uapi_smbs_releaseInstance(uapi);

	return nReqId;
}

HERROR	UAPI_SMBS_SetConfig( const UAPI_SMBS_CONF_t *pstConfig )
{
	UAPI_t	*uapi;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_SetConfig, "b", pstConfig, sizeof(UAPI_SMBS_CONF_t));
	}

	uapi_smbs_releaseInstance(uapi);
	return ERR_OK;
}

HERROR	UAPI_SMBS_GetServerInfo( UAPI_SMBS_INFO_t *pstConfig )
{
	UAPI_t	*uapi;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_GetServerInfo, NULL);
	}

	uapi_smbs_releaseInstance(uapi);
	return ERR_OK;
}

HERROR	UAPI_SMBS_SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd )
{
	UAPI_t	*uapi;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_SetPasswd, "sss", pszUserName, pszCurPasswd, pszNewPasswd);
	}
	uapi_smbs_releaseInstance(uapi);

	return ERR_OK;
}

HERROR	UAPI_SMBS_CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid )
{
	UAPI_t	*uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	UMMA_MSG_t *pUserData = NULL;

	pUserData =  (UMMA_MSG_t *)HLIB_MEM_Calloc(sizeof(UMMA_MSG_t));
	if(pUserData == NULL)
	{
		HxLOG_Error("Error, Allocation Fail !!!\n");
		return ERR_FAIL;
	}

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		uapi_smbs_releaseInstance(uapi);
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, uapi_smbs_checkPasswdCb, pUserData,
					RPC_UAPI_SMBS_CheckPasswd, "ss", pszUserName, pszPasswd);
		if(nReqId == -1)
		{
			HxLOG_Error("Error, RPC Request Fail !!!\n");
			HLIB_MEM_Free(pUserData);
			return ERR_FAIL;
		}
		if(HLIB_RPC_Join (uapi->nRpcHandle, nReqId, 1000) == ERR_FAIL)
		{
			HxLOG_Error("Error, RPC Timeout !!!\n");
			HLIB_MEM_Free(pUserData);
			return ERR_FAIL;
		}
	}

	hErr = pUserData->nParam1;
	*pbIsValid = pUserData->nParam2 ? TRUE : FALSE;
	HLIB_MEM_Free(pUserData);

	return hErr;
}

HERROR	UAPI_SMBS_SetPermission(const HCHAR *pszUserName, UAPI_SMBS_SHARED_t *pstShared )
{
	UAPI_t	*uapi;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_SetPermission, "sb", pszUserName, pstShared, sizeof(UAPI_SMBS_SHARED_t));
	}
	uapi_smbs_releaseInstance(uapi);

	return ERR_OK;
}

HERROR	UAPI_SMBS_GetSharedList(const HCHAR *pszUserName, HINT32 *pnCount, UAPI_SMBS_SHARED_t **ppstList)
{
	UAPI_t	*uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	UMMA_MSG_t *pUserData = NULL;

	pUserData =  (UMMA_MSG_t *)HLIB_MEM_Calloc(sizeof(UMMA_MSG_t));
	if(pUserData == NULL)
	{
		HxLOG_Error("Error, Allocation Fail !!!\n");
		return ERR_FAIL;
	}

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		uapi_smbs_releaseInstance(uapi);
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, uapi_smbs_getSharedCb, pUserData,
								RPC_UAPI_SMBS_GetSharedList, "s", pszUserName);
		if(nReqId == -1)
		{
			HxLOG_Error("Error, RPC Request Fail !!!\n");
			HLIB_MEM_Free(pUserData);
			return ERR_FAIL;
		}
		if(HLIB_RPC_Join (uapi->nRpcHandle, nReqId, 1000) == ERR_FAIL)
		{
			HxLOG_Error("Error, RPC Timeout !!!\n");
			HLIB_MEM_Free(pUserData);
			return ERR_FAIL;
		}
	}

	hErr = pUserData->nParam1;
	*pnCount = pUserData->nParam2;
	*ppstList = (UAPI_SMBS_SHARED_t *)pUserData->nParam3;
	HLIB_MEM_Free(pUserData);

	return hErr;
}

HERROR	UAPI_SMBS_SetPublicAccess(const HCHAR *pszUserName, HBOOL bIsAccess)
{
	UAPI_t	*uapi;

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		HLIB_RPC_Call(uapi->nRpcHandle, RPC_UAPI_SMBS_SetPublicAccess, "si", pszUserName, bIsAccess);
	}
	uapi_smbs_releaseInstance(uapi);

	return ERR_OK;
}

HERROR	UAPI_SMBS_GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbIsAccess)
{
	UAPI_t	*uapi;
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	UMMA_MSG_t *pUserData = NULL;

	pUserData =  (UMMA_MSG_t *)HLIB_MEM_Calloc(sizeof(UMMA_MSG_t));
	if(pUserData == NULL)
	{
		HxLOG_Error("Error, Allocation Fail !!!\n");
		return ERR_FAIL;
	}

	uapi = uapi_smbs_getInstance();
	if (uapi)
	{
		uapi_smbs_releaseInstance(uapi);
		nReqId = HLIB_RPC_Request(uapi->nRpcHandle, uapi_smbs_getPublicAccess, pUserData,
								RPC_UAPI_SMBS_GetPublicAccess, "s", pszUserName);
		if(nReqId == -1)
		{
			HxLOG_Error("Error, RPC Request Fail !!!\n");
			HLIB_MEM_Free(pUserData);
			return ERR_FAIL;
		}
		if(HLIB_RPC_Join (uapi->nRpcHandle, nReqId, 1000) == ERR_FAIL)
		{
			HxLOG_Error("Error, RPC Timeout !!!\n");
			HLIB_MEM_Free(pUserData);
			return ERR_FAIL;
		}
	}

	hErr = pUserData->nParam1;
	*pbIsAccess = pUserData->nParam2 ? TRUE : FALSE;
	HLIB_MEM_Free(pUserData);

	return hErr;
}

#if defined(CONFIG_DEBUG)
static int	uapi_smbs_debug_cmdActionTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[UAPI_SMBS_CMD] Server %s\n", aucStr);
	if ( !HxSTD_StrCmp( "START", aucStr ) )
	{
		UAPI_SMBS_StartServer();
	}
	else if ( !HxSTD_StrCmp( "STOP", aucStr ) )
	{
		UAPI_SMBS_StopServer();
	}
	else if ( !HxSTD_StrCmp( "RESTART", aucStr ) )
	{
		UAPI_SMBS_StopServer();
		UAPI_SMBS_StartServer();
	}
	else
	{
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] %s is not supported.\n", aucStr);
	}

	return HxCMD_OK;
}

static int	uapi_smbs_debug_cmdConfgTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	UAPI_SMBS_CONF_t stConf;

	HxSTD_MemSet( &stConf, 0x00, sizeof(UAPI_SMBS_CONF_t) );
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[UAPI_SMBS_CMD] Server Conf %s\n", aucStr);
	if ( !HxSTD_StrCmp( "SET", aucStr ) )
	{
		HCHAR szName[128] = { 0 };

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( NULL ==  aucArg )
			return HxCMD_ERR;

		sscanf(aucArg, "%s", (char *)szName);
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] ServerName %s\n", szName);
		HxSTD_StrNCpy( stConf.szServerName, szName, sizeof(stConf.szServerName)-1 );

		UAPI_SMBS_SetConfig( &stConf);
	}
	else if ( !HxSTD_StrCmp( "GET", aucStr ) )
	{
		UAPI_SMBS_INFO_t stInfo;
		UAPI_SMBS_GetServerInfo( &stInfo);
	}
	else
	{
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] %s is not supported.\n", aucStr);
	}
	return HxCMD_OK;
}

static int	uapi_smbs_debug_cmdPasswdTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[UAPI_SMBS_CMD] Server Conf %s\n", aucStr);
	if ( !HxSTD_StrCmp( "SET", aucStr ) )
	{
		HCHAR szPasswd[128] = { 0 };
		HCHAR szNewPasswd[128] = { 0 };

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( NULL ==  aucArg )
			return HxCMD_ERR;

		sscanf(aucArg, "%s", (char *)szPasswd);

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( NULL ==  aucArg )
			return HxCMD_ERR;
		sscanf(aucArg, "%s", (char *)szNewPasswd);

		HLIB_CMD_Printf("[UAPI_SMBS_CMD] Change Passwd : %s, NewPasswd : %s\n", szPasswd, szNewPasswd);

		UAPI_SMBS_SetPassword( "admin", szPasswd, szNewPasswd);
	}
	else if ( !HxSTD_StrCmp( "CHK", aucStr ) )
	{
		HCHAR szPasswd[128] = { 0 };
		HBOOL bIsValid = FALSE;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( NULL ==  aucArg )
			return HxCMD_ERR;
		sscanf(aucArg, "%s", (char *)szPasswd);
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] Check Passwd : %s\n", szPasswd);
		UAPI_SMBS_CheckPassword( "admin", szPasswd, &bIsValid );
	}
	else
	{
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] %s is not supported.\n", aucStr);
	}
	return HxCMD_OK;
}

static int	uapi_smbs_debug_cmdSharedTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[UAPI_SMBS_CMD] SHARED %s\n", aucStr);
	if ( !HxSTD_StrCmp( "GET", aucStr ) )
	{
		HINT32 nCount = 0;
		UAPI_SMBS_SHARED_t *pSharedList = NULL;
		UAPI_SMBS_GetSharedList("admin", &nCount, &pSharedList );
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] list count %d.\n", nCount);

		if ( pSharedList )
			HLIB_MEM_Free(pSharedList);
	}
	else if ( !HxSTD_StrCmp( "RW", aucStr ) )
	{
		UAPI_SMBS_SHARED_t stShared;
		HxSTD_StrNCpy( stShared.szPath, "/mnt/hd2/Media/Video", sizeof(stShared.szPath) );
		HxSTD_StrNCpy( stShared.szName, "Video", sizeof(stShared.szName) );
		stShared.bWritable = FALSE;
		UAPI_SMBS_SetPermission( "admin", &stShared );
	}
	else
	{
		HLIB_CMD_Printf("[UAPI_SMBS_CMD] %s is not supported.\n", aucStr);
	}
	return HxCMD_OK;
}


static void		uapi_smbs_debug_InitCommandTools(void)
{
	HLIB_CMD_RegisterWord("smbs", uapi_smbs_debug_cmdActionTest,
						"act",
						"samba server action",
						"act [start|stop|restart]");
	HLIB_CMD_RegisterWord("smbs", uapi_smbs_debug_cmdConfgTest,
						"conf",
						"conf samba server",
						"conf [set serverName/get]");
	HLIB_CMD_RegisterWord("smbs", uapi_smbs_debug_cmdPasswdTest,
						"passwd",
						"password",
						"passwd [set xxx newXXX/ chk username xxx]");
	HLIB_CMD_RegisterWord("smbs", uapi_smbs_debug_cmdSharedTest,
						"shared",
						"shared list",
						"shared [get username / rw username]");
}
#endif


