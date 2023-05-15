/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_sambaserver.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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
#include	<uapi.h>

#include	<apk.h>

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
static HxList_t		*s_pstFileMgrSmbsCbList = NULL;
static ApkFileMgr_SMBS_INFO_t *s_pstSmbServerInfo = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static inline ApkFileMgr_SMBS_INFO_t *apk_filemgr_sambaserver_getInstance( void )
{
	if ( NULL == s_pstSmbServerInfo )
		s_pstSmbServerInfo =  (ApkFileMgr_SMBS_INFO_t *)HLIB_MEM_Calloc(sizeof(ApkFileMgr_SMBS_INFO_t));
	return s_pstSmbServerInfo;
}
static void apk_filemgr_freeEventCb( HINT32 nArgc, void *apArgv[] )
{
}

static inline HINT32 apk_filemgr_sambaserver_convertApkFtpsInfo(
						const UAPI_SMBS_INFO_t *pstUapiInfo, ApkFileMgr_SMBS_INFO_t *pstApkInfo  )
{
	pstApkInfo->bStarted = pstUapiInfo->bStarted;
	pstApkInfo->nClientCnt = pstUapiInfo->nClientCnt;

	HxSTD_StrNCpy( pstApkInfo->stConf.szWorkgroup, pstUapiInfo->stConf.szWorkgroup,
					sizeof(pstApkInfo->stConf.szWorkgroup)-1 );
	HxSTD_StrNCpy( pstApkInfo->stConf.szServerName, pstUapiInfo->stConf.szServerName,
					sizeof(pstApkInfo->stConf.szServerName)-1 );
	HxSTD_StrNCpy( pstApkInfo->stConf.szServerDesc, pstUapiInfo->stConf.szServerDesc,
					sizeof(pstApkInfo->stConf.szServerDesc)-1 );
	return 0;
}

static HERROR	apk_filemgr_sambaserver_listener( HINT32 eEvent, void *pvData, void *pvUserData )
{
	HxList_t	*list;
	APK_FILEMGR_SAMBASERVER_Notifier_t	fnNotifier;
	HINT32 retErr = (HINT32)pvData;
	UAPI_SMBS_INFO_t *pServerInfo = (UAPI_SMBS_INFO_t *)pvUserData;
	ApkFileMgr_SMBS_INFO_t *pApkServerInfo = apk_filemgr_sambaserver_getInstance();
	APK_FILEMGR_SMBS_EVT_e apkEvent = eAPK_FILEMGR_SMBS_EVT_UNKNOWN;

	apk_filemgr_sambaserver_convertApkFtpsInfo( pServerInfo, pApkServerInfo );

	switch ( eEvent )
	{
		case eUAPI_SMBS_EVT_SERVER_START :
			apkEvent = eAPK_FILEMGR_SMBS_EVT_SERVER_START;
			break;
		case eUAPI_SMBS_EVT_SERVER_STOP :
			apkEvent = eAPK_FILEMGR_SMBS_EVT_SERVER_STOP;
			break;
		case eUAPI_SMBS_EVT_CONFIG_CHANGE :
			apkEvent = eAPK_FILEMGR_SMBS_EVT_CONFIG_CHANGE;
			break;
		case eUAPI_SMBS_EVT_PASSWD_CHANGE :
			apkEvent = eAPK_FILEMGR_SMBS_EVT_PASSWD_CHANGE;
			break;
		case eUAPI_SMBS_EVT_PERMISSION_CHANGE :
			apkEvent = eAPK_FILEMGR_SMBS_EVT_PERMISSION_CHANGE;
			break;
		case eUAPI_SMBS_EVT_STATUS_CHANGE :
			apkEvent = eAPK_FILEMGR_SMBS_EVT_STATUS_CHANGE;
			break;
		case eUAPI_SMBS_EVT_RESPONE_SERVERINFO :
		case eUAPI_SMBS_EVT_RESPONE_SHAREDLIST :
		case eUAPI_SMBS_EVT_RESPONE_CHKPASSWD :
		default :
			return ERR_OK;
	}

	list = s_pstFileMgrSmbsCbList;
	while ( list )
	{
		fnNotifier = (APK_FILEMGR_SAMBASERVER_Notifier_t)HLIB_LIST_Data(list);
		if (fnNotifier)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = apk_filemgr_freeEventCb;
			stApkEvtArgs.ulArgCount = 4;
			stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)eEvent;
			stApkEvtArgs.apArgV[2] = (void *)retErr;
			stApkEvtArgs.apArgV[3] = (void *)NULL;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
		list = list->next;
	}

	return ERR_OK;
}

HERROR	APK_FILEMGR_SAMBASERVER_Init(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	ApkFileMgr_SMBS_INFO_t *pServerInfo = apk_filemgr_sambaserver_getInstance();
	UAPI_SMBS_Init();
	UAPI_SMBS_SetNotifier(apk_filemgr_sambaserver_listener);
	UAPI_SMBS_GetServerInfo( (UAPI_SMBS_INFO_t*)pServerInfo );
#endif
	return ERR_OK;
}

HERROR	APK_FILEMGR_SAMBASERVER_RegisterCallback(APK_FILEMGR_SAMBASERVER_Notifier_t fnNotifier)
{
	s_pstFileMgrSmbsCbList = HLIB_LIST_Append( s_pstFileMgrSmbsCbList, (void *)fnNotifier );
	return ERR_OK;
}

HERROR	APK_FILEMGR_SAMBASERVER_UnregisterCallback(APK_FILEMGR_SAMBASERVER_Notifier_t fnNotifier)
{
	s_pstFileMgrSmbsCbList = HLIB_LIST_Remove( s_pstFileMgrSmbsCbList, (void *)fnNotifier );
	return ERR_OK;
}

HINT32	APK_FILEMGR_SAMBASERVER_Start(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_StartServer();
#else
	return HANDLE_NULL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_Stop(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_StopServer();
#else
	return HANDLE_NULL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_SetConfig( const ApkFileMgr_SMBS_CONF_t *pstConfig )
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_SetConfig((UAPI_SMBS_CONF_t*)pstConfig);
#else
	return HANDLE_NULL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_GetServerInfo( ApkFileMgr_SMBS_INFO_t *pstInfo )
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	ApkFileMgr_SMBS_INFO_t *pServerInfo = NULL;
	if ( NULL==pstInfo )
		return HANDLE_NULL;
	pServerInfo = apk_filemgr_sambaserver_getInstance();
	HxSTD_MemCopy( pstInfo, pServerInfo, sizeof(ApkFileMgr_SMBS_INFO_t) );
	return 0;
#else
	return HANDLE_NULL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_SetPassword( const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd )
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_SetPassword( pszUserName, pszCurPasswd, pszNewPasswd );
#else
	return HANDLE_NULL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_CheckPassword( pszUserName, pszPasswd, pbIsValid );
#else
	return HANDLE_NULL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_SetPermission(const HCHAR *pszUserName, ApkFileMgr_SMBS_SHARED_t *pstShared )
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_SetPermission( pszUserName, (UAPI_SMBS_SHARED_t*)pstShared );
#else
	return HANDLE_NULL;
#endif
}

HERROR	APK_FILEMGR_SAMBASERVER_GetSharedList(const HCHAR *pszUserName, HINT32 *pnCount, ApkFileMgr_SMBS_SHARED_t **ppstList)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_GetSharedList( pszUserName, pnCount, (UAPI_SMBS_SHARED_t**)ppstList );
#else
	return ERR_FAIL;
#endif
}

HINT32	APK_FILEMGR_SAMBASERVER_SetPublicAccess(const HCHAR *pszUserName, HBOOL bAccess)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_SetPublicAccess( pszUserName, bAccess );
#else
	return HANDLE_NULL;
#endif
}

HERROR	APK_FILEMGR_SAMBASERVER_GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbAccess)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
	return UAPI_SMBS_GetPublicAccess( pszUserName, pbAccess );
#else
	return ERR_FAIL;
#endif
}

