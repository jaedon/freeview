/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_filemgr_sambaclient.c
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
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
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

#include	"_apk_int.h"
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
static HxList_t		*s_pstFileMgrSmbCbList = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
static ApkFileMgr_SMBSharedFolderInfo_t * apK_filemgr_sambaclinet_creteShared(UAPI_SMBC_SERVER_t *pstServerInfo, UAPI_SMBC_SHARED_t *pstShared)
{
	ApkFileMgr_SMBSharedFolderInfo_t *pApkShared = NULL;
	pApkShared = (ApkFileMgr_SMBSharedFolderInfo_t *)HLIB_STD_MemAlloc(sizeof(ApkFileMgr_SMBSharedFolderInfo_t));
	HxSTD_MemSet( pApkShared, 0x00, sizeof(ApkFileMgr_SMBSharedFolderInfo_t) );
	HxSTD_StrNCpy( pApkShared->szServerName, pstServerInfo->szName, sizeof(pApkShared->szServerName) );
	HxSTD_StrNCpy( pApkShared->szSharedName, pstShared->szName, sizeof(pApkShared->szSharedName) );
	HxSTD_StrNCpy( pApkShared->szIp, pstServerInfo->szIp, sizeof(pApkShared->szIp) );
	HxSTD_StrNCpy( pApkShared->szId, pstShared->szId, sizeof(pApkShared->szId) );
	HxSTD_StrNCpy( pApkShared->szPassword, pstShared->szPassword, sizeof(pApkShared->szPassword) );
	HxSTD_StrNCpy( pApkShared->szMountPath, pstShared->szMountPath, sizeof(pApkShared->szMountPath) );
	HxSTD_StrNCpy( pApkShared->szLogMsg, pstShared->szLogMsg, sizeof(pApkShared->szLogMsg) );

	return pApkShared;
}
static ApkFileMgr_SMBSharedFolderInfo_t * apK_filemgr_sambaclinet_creteSharedByMountInfo(UAPI_SMBC_MOUNT_t *pstMountInfo)
{
	ApkFileMgr_SMBSharedFolderInfo_t *pApkShared = NULL;
	pApkShared = (ApkFileMgr_SMBSharedFolderInfo_t *)HLIB_STD_MemAlloc(sizeof(ApkFileMgr_SMBSharedFolderInfo_t));
	HxSTD_MemSet( pApkShared, 0x00, sizeof(ApkFileMgr_SMBSharedFolderInfo_t) );
	HxSTD_StrNCpy( pApkShared->szServerName, pstMountInfo->szServerName, sizeof(pApkShared->szServerName)-1 );
	HxSTD_StrNCpy( pApkShared->szSharedName, pstMountInfo->szSharedName, sizeof(pApkShared->szSharedName)-1 );
	HxSTD_StrNCpy( pApkShared->szIp, pstMountInfo->szIp, sizeof(pApkShared->szIp)-1 );
	HxSTD_StrNCpy( pApkShared->szId, pstMountInfo->szId, sizeof(pApkShared->szId)-1 );
	HxSTD_StrNCpy( pApkShared->szPassword, pstMountInfo->szPassword, sizeof(pApkShared->szPassword)-1 );
	HxSTD_StrNCpy( pApkShared->szMountPath, pstMountInfo->szPath, sizeof(pApkShared->szMountPath)-1 );
	HxSTD_StrNCpy( pApkShared->szLogMsg, pstMountInfo->szLogMsg, sizeof(pApkShared->szLogMsg)-1 );
	return pApkShared;
}

static void		apk_filemgr_sambaclient_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	HINT32	eEvent = 0;

	if ( 5 != nArgc )
		return;

	eEvent = (HINT32)apArgV[4];
	switch( eEvent ) 
	{
		case 3:
			{
				ApkFileMgr_SMBSharedFolderInfo_t *pSharedInfo = (ApkFileMgr_SMBSharedFolderInfo_t *)apArgV[2];
				if (NULL != pSharedInfo) 
				{
					HLIB_STD_MemFree(pSharedInfo);
				}
			}
			break;
		case 5:
			{
				ApkFileMgr_SMBSharedFolderInfo_t *pSharedInfo = (ApkFileMgr_SMBSharedFolderInfo_t *)apArgV[2];
				if (NULL !=pSharedInfo) 
				{
					HLIB_STD_MemFree(pSharedInfo);
				}

			}
			break;
		case 6:
			{
				ApkFileMgr_SMBSharedFolderInfo_t *pSharedInfo = (ApkFileMgr_SMBSharedFolderInfo_t *)apArgV[2];
				if (NULL !=pSharedInfo) 
				{
					HLIB_STD_MemFree(pSharedInfo);
				}
			}
			break;
		default :
			break;

	}
}

static void		apk_filemgr_sambaclient_Listener( HINT32 eEvent, void *pvData, void *pvUserData)
{
	HxList_t	*list;
	APK_FILEMGR_SAMBACLIENT_Notifier_t	fnNotifier;
	HCHAR *pszEvent = NULL;
	switch ( eEvent )
	{
		case 0 :
			pszEvent = "started";
			break;
		case 1 :
			pszEvent = "stopped";
			break;
		case 2 :
			pszEvent = "";
			break;
		case 3 :
			pszEvent = "addserver";
			{
				HINT32 i = 0;
				ApkFileMgr_SMBSharedFolderInfo_t *pSharedInfo = NULL;
				UAPI_SMBC_SERVER_t *pServerInfo = (UAPI_SMBC_SERVER_t *)pvData;
				UAPI_SMBC_SHARED_t *pShared = NULL;
				for ( i = 0; i < pServerInfo->nSharedCnt; i++ )
				{
					list = s_pstFileMgrSmbCbList;
					pShared = &pServerInfo->pstSharedList[i];
					pSharedInfo = apK_filemgr_sambaclinet_creteShared( pServerInfo, pShared );
					while (list)
					{
						fnNotifier = (APK_FILEMGR_SAMBACLIENT_Notifier_t)HLIB_LIST_Data(list);
						if (fnNotifier)
						{
							APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

							stApkEvtArgs.pfnFreeFunc = apk_filemgr_sambaclient_freeEventCb;
							stApkEvtArgs.ulArgCount = 5;
							stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
							stApkEvtArgs.apArgV[1] = (void *)"addshared";
							stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pSharedInfo, sizeof(ApkFileMgr_SMBSharedFolderInfo_t));
							stApkEvtArgs.apArgV[3] = (void *)pvUserData;
							stApkEvtArgs.apArgV[4] = (void *)eEvent;

							APK_EVENT_Emit(&stApkEvtArgs);
						}
						list = list->next;
					}
					if (pSharedInfo)
					{
						HLIB_STD_MemFree(pSharedInfo);
						pSharedInfo = NULL;
					}
				}

			}
			return ;
		case 4 :
			pszEvent = "removeserver";
			break;
		case 5 :
			pszEvent= "mounted";
			{
				UAPI_SMBC_MOUNT_t *pMount = (UAPI_SMBC_MOUNT_t *)pvData;
				ApkFileMgr_SMBSharedFolderInfo_t *pSharedInfo = NULL;
				pSharedInfo = apK_filemgr_sambaclinet_creteSharedByMountInfo( pMount );
				if (pSharedInfo)
				{
					if ( ERR_OK == pMount->nError )
						pSharedInfo->bMounted = TRUE;
					else
						pSharedInfo->bMounted = FALSE;

					list = s_pstFileMgrSmbCbList;
					while (list)
					{
						fnNotifier = (APK_FILEMGR_SAMBACLIENT_Notifier_t)HLIB_LIST_Data(list);
						if (fnNotifier)
						{
							APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

							stApkEvtArgs.pfnFreeFunc = apk_filemgr_sambaclient_freeEventCb;
							stApkEvtArgs.ulArgCount = 5;
							stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
							stApkEvtArgs.apArgV[1] = (void *)pszEvent;
							stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pSharedInfo, sizeof(ApkFileMgr_SMBSharedFolderInfo_t));
							stApkEvtArgs.apArgV[3] = (void *)pvUserData;
							stApkEvtArgs.apArgV[4] = (void *)eEvent;

							APK_EVENT_Emit(&stApkEvtArgs);
						}
						list = list->next;
					}
					HLIB_STD_MemFree(pSharedInfo);
					pSharedInfo = NULL;
				}

			}
			return;
		case 6 :
			pszEvent = "unmounted";
			{
				UAPI_SMBC_MOUNT_t *pMount = (UAPI_SMBC_MOUNT_t *)pvData;
				ApkFileMgr_SMBSharedFolderInfo_t *pSharedInfo = NULL;
				pSharedInfo = apK_filemgr_sambaclinet_creteSharedByMountInfo( pMount );
				if (pSharedInfo)
				{
					if ( ERR_OK == pMount->nError )
						pSharedInfo->bMounted = FALSE;
					else
						pSharedInfo->bMounted = TRUE;

					list = s_pstFileMgrSmbCbList;
					while (list)
					{
						fnNotifier = (APK_FILEMGR_SAMBACLIENT_Notifier_t)HLIB_LIST_Data(list);
						if (fnNotifier)
						{
							APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

							stApkEvtArgs.pfnFreeFunc = apk_filemgr_sambaclient_freeEventCb;
							stApkEvtArgs.ulArgCount = 5;
							stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
							stApkEvtArgs.apArgV[1] = (void *)pszEvent;
							stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pSharedInfo, sizeof(ApkFileMgr_SMBSharedFolderInfo_t));
							stApkEvtArgs.apArgV[3] = (void *)pvUserData;
							stApkEvtArgs.apArgV[4] = (void *)eEvent;

							APK_EVENT_Emit(&stApkEvtArgs);
						}
						list = list->next;
					}
					HLIB_STD_MemFree(pSharedInfo);
					pSharedInfo = NULL;
				}
			}
			return;
		default :
			pszEvent = "";
			break;
	}

	list = s_pstFileMgrSmbCbList;
	while (list)
	{
		fnNotifier = (APK_FILEMGR_SAMBACLIENT_Notifier_t)HLIB_LIST_Data(list);
		if (fnNotifier)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = NULL;
			stApkEvtArgs.ulArgCount = 5;
			stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)pszEvent;
			stApkEvtArgs.apArgV[2] = (void *)pvData;
			stApkEvtArgs.apArgV[3] = (void *)pvUserData;
			stApkEvtArgs.apArgV[4] = (void *)eEvent;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
		list = list->next;
	}
}
#else
static void     apk_filemgr_sambaclient_Listener(const HCHAR *pszEvent, void *pvData, void *pvUserData)
{
	HxList_t    *list;
	APK_FILEMGR_SAMBACLIENT_Notifier_t  fnNotifier;

	list = s_pstFileMgrSmbCbList;
	while (list)
	{
		fnNotifier = (APK_FILEMGR_SAMBACLIENT_Notifier_t)HLIB_LIST_Data(list);
		if (fnNotifier)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			stApkEvtArgs.pfnFreeFunc = NULL;
			stApkEvtArgs.ulArgCount = 4;
			stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)pszEvent;
			stApkEvtArgs.apArgV[2] = (void *)pvData;
			stApkEvtArgs.apArgV[3] = (void *)pvUserData;

			APK_EVENT_Emit(&stApkEvtArgs);
		}
		list = list->next;
	}
}
#endif

HERROR	APK_FILEMGR_SAMBACLIENT_Init(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	UAPI_SMBC_Init();
	UAPI_SMBC_SetNotifier(apk_filemgr_sambaclient_Listener);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR	APK_FILEMGR_SAMBACLIENT_RegisterCallback(APK_FILEMGR_SAMBACLIENT_Notifier_t fnNotifier)
{
	s_pstFileMgrSmbCbList = HLIB_LIST_Append(s_pstFileMgrSmbCbList, (void*)fnNotifier);

	return ERR_OK;
}

HERROR	APK_FILEMGR_SAMBACLIENT_UnregisterCallback(APK_FILEMGR_SAMBACLIENT_Notifier_t fnNotifier)
{
	s_pstFileMgrSmbCbList = HLIB_LIST_Remove(s_pstFileMgrSmbCbList, (void*)fnNotifier);

	return ERR_OK;
}

HERROR	APK_FILEMGR_SAMBACLIENT_StartScan(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	return UAPI_SMBC_StartScan();
#else
	return ERR_FAIL;
#endif
}

HERROR	APK_FILEMGR_SAMBACLIENT_StopScan(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	return UAPI_SMBC_StopScan();
#else
	return ERR_FAIL;
#endif
}

HERROR	APK_FILEMGR_SAMBACLIENT_Mount(ApkFileMgr_SMBSharedFolderInfo_t *pstInfo)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	return UAPI_SMBC_Mount( pstInfo->szIp, pstInfo->szSharedName, pstInfo->szId, pstInfo->szPassword,
		pstInfo->szMountPath );
#else
	return ERR_FAIL;
#endif
}

HERROR	APK_FILEMGR_SAMBACLIENT_Unmount(ApkFileMgr_SMBSharedFolderInfo_t *pstInfo)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	return UAPI_SMBC_Unmount( pstInfo->szIp, pstInfo->szSharedName, pstInfo->szMountPath );
#else
	return ERR_FAIL;
#endif
}

HERROR	APK_FILEMGR_SAMBACLIENT_UnmountAll(void)
{
#if defined(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT)
	return UAPI_SMBC_UnmountAll();
#else
	return ERR_FAIL;
#endif
}
