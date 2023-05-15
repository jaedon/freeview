/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_smbc.c
	@brief

	Description:  									\n
	Module: UMMA Samba Client				\n

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
#include <vkernel.h>
#include <hlib.h>
#include <uapi.h>
#include <napi.h>
#include "se_uapi.h"

#include <umma_int.h>
#include <umma_fm.h>
#include "umma_smbc.h"
#include "umma_smbs.h"
#include <sys/mount.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SAMBAC_MAX_LIST				(50)
#define SAMBAC_CMD_ARGC_MAX_SIZE	(10)
#define SAMBAC_CMD_ARGV_MAX_SIZE	(100)

#define SAMBAC_CMD_SMBTREE			"/usr/bin/smbtree"			// auto-detect
#define SAMBAC_CMD_NMBLOOKUP		"/usr/bin/nmblookup"		//	resolve to ip
#define SAMBAC_CMD_MOUNT			"/usr/bin/mount.cifs"		// mount
#define SAMBAC_CMD_UNMOUNT			"/usr/bin/umount.cifs" 	// umount
//#define SAMBAC_CMD_UNMOUNT			"/bin/umount" 				// umount

#define SAMBAC_CONF_PATH			"/usr/samba/smb.conf"

#define SAMBAC_CMD_MOUNT_RESULT		"/tmp/.result_mount"		//"/usr/samba/.result_mount"
#define SAMBAC_CMD_UMOUNT_RESULT	"/tmp/.result_umount"		//"/usr/samba/.result_umount"
#define SAMBAC_CMD_CLEANUP_RESULT	"/tmp/.result_cleanup"
#define SAMBAC_CMD_MKDIR_RESULT		"/tmp/.result_mkdir"		//"/usr/samba/.result_mkdir"
#define SAMBAC_CMD_RMDIR_RESULT		"/tmp/.result_rmdir"		//"/usr/samba/.result_rmdir"

#define SAMBAC_SMBTREE_RESULT		"/tmp/.smbtree_parser"
#define SAMBAC_NMBLOOKUP_RESULT		"/tmp/.nmblookup_parser"


#define SAMBAC_MOUNT_ROOT			"/media/smb"//"/mnt/hd2/smb"
#define RESERVED					0x00

#define	UMMA_SMBC_MOUNT	 128

#define UMMA_SMBC_IP_LEN	 16
#define UMMA_SMBC_ID_LEN	 64
#define UMMA_SMBC_PATH_LEN	 512
#define UMMA_SMBC_PASSWORD_LEN	 64
#define UMMA_SMBC_LOG_LEN			128
#define UMMA_SMBC_FOLDER_LEN		128
#define UMMA_SMBC_SRVNAME_LEN		64
#define UMMA_SMBC_WORKGROUP_LEN		64

#define UMMA_SMBC_CMD_LEN			512
#define UMMA_SMBC_STDOUT_LEN		128

#define UMMA_SMBC_SCANTASK_NAME	"ummaSmbcTreeTask"

#define ENTRY	HxLOG_Debug(" +++++++++++++++++++++++++++++\n");
#define LEAVE	HxLOG_Debug(" -----------------------------\n");

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HCHAR	szName[UMMA_SMBC_WORKGROUP_LEN];
	HxList_t *pstServerList;
	HINT32 nServerCnt;
} UMMA_SMBC_WORKGROUP_t;

typedef struct
{
	HCHAR szPath[UMMA_SMBC_PATH_LEN];

} UMMA_SMBC_UNMOUNT_t;

typedef struct
{
	HINT32		nServerHandle;
	HINT32		nRpcHandle;
	HULONG 	ulMsgQueue;

	HUINT32 ulSemId;
	HUINT32 ulMainTaskId;
	HUINT32 ulScanTaskId;
	HUINT32 ulEvtTaskId;

	HxList_t *pstWorkgroupList;
	HxList_t *pstMountedList;

	HBOOL bInit;
	HBOOL bScanning;

	HBOOL bExistScanResult;

	HCHAR szIpAddr[16];
	HCHAR szSmbsName[32];

} UMMA_SMBC_t;

typedef struct
{
	UMMA_SMBC_CMD_e eCmd;
	void *pvData;
	//HINT32 nSize; // data Size
	HINT32 nReqId;
} UMMA_SMBC_MSG_t;

typedef enum
{
	eUMMA_SMBC_EVT_SCAN_START = 0,
	eUMMA_SMBC_EVT_SCAN_STOP,
	eUMMA_SMBC_EVT_SCAN_END,
	eUMMA_SMBC_EVT_SERVER_ADD,
	eUMMA_SMBC_EVT_SERVER_REMOVE,
	eUMMA_SMBC_EVT_MOUNTED,
	eUMMA_SMBC_EVT_UNMOUNTED,
} UMMA_SMBC_Event_e;

typedef HBOOL (*COMPARE_FUNC_t)(void *user_data,  void *listData);
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static UMMA_SMBC_t	*s_pstUmmaSmbc = NULL;
static HUINT32		s_ulListSemId = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static UMMA_SMBC_t *	umma_smbc_getInstance(void);
static UMMA_SMBC_t *	umma_smbc_getInstanceWO(void);
static void		umma_smbc_releaseInstance(UMMA_SMBC_t *pstUmma);
STATIC void	umma_smbc_print_mountInfo( UAPI_SMBC_MOUNT_t *pstMountInfo );

#define __________Internal_Function__________
STATIC void umma_smbc_notifyEventToClient( UMMA_SMBC_t *pstSmbc, UMMA_SMBC_Event_e eEvent, void *pvData, int flag)
{
	switch ( eEvent )
	{
		case eUMMA_SMBC_EVT_MOUNTED :
		case eUMMA_SMBC_EVT_UNMOUNTED :
			{
				UAPI_SMBC_MOUNT_t stMount;
				UAPI_SMBC_MOUNT_t *pstMount = (UAPI_SMBC_MOUNT_t *)pvData;
				HxSTD_MemSet(&stMount, 0, sizeof(UAPI_SMBC_MOUNT_t));
				if ( NULL == pstMount )
					pstMount = &stMount;
#ifdef CONFIG_DEBUG
				umma_smbc_print_mountInfo( pstMount );
#endif
				HLIB_RPC_Notify(pstSmbc->nRpcHandle, RPC_UMMA_SMBC_onMountResult, "iib", 0, eEvent, pstMount, sizeof(UAPI_SMBC_MOUNT_t) );
			}
			break;
		case eUMMA_SMBC_EVT_SCAN_START :
		case eUMMA_SMBC_EVT_SCAN_STOP :
		case eUMMA_SMBC_EVT_SCAN_END :
		case eUMMA_SMBC_EVT_SERVER_ADD :
		case eUMMA_SMBC_EVT_SERVER_REMOVE :
			{
				HINT32 listSize = 0;
				UAPI_SMBC_SERVER_t stServer;
				UAPI_SMBC_SERVER_t *pServerInfo = (UAPI_SMBC_SERVER_t *)pvData;
				UAPI_SMBC_SHARED_t stShared;
				UAPI_SMBC_SHARED_t *pSharedList = NULL;
				HxSTD_MemSet(&stServer, 0, sizeof(UAPI_SMBC_SERVER_t));
				HxSTD_MemSet(&stShared, 0, sizeof(UAPI_SMBC_SHARED_t));

				if ( NULL == pServerInfo )
					pServerInfo = &stServer;

				pSharedList = (UAPI_SMBC_SHARED_t *)pServerInfo->pstSharedList;
				if ( NULL == pSharedList )
				{
					pSharedList = &stShared;
					listSize = sizeof(UAPI_SMBC_SHARED_t);
				}
				else
					listSize = sizeof(UAPI_SMBC_SHARED_t) * pServerInfo->nSharedCnt;

				HLIB_RPC_Notify(pstSmbc->nRpcHandle, RPC_UMMA_SMBC_onScanResult, "iibb", 0, eEvent, pServerInfo, sizeof(UAPI_SMBC_SERVER_t), pSharedList, listSize);
			}
			break;
	}
}

STATIC HBOOL umma_smbc_cb_compareBySharedName( void *user_data, void *list_data )
{
	UAPI_SMBC_SHARED_t *pShared = (UAPI_SMBC_SHARED_t*)list_data;
	if ( !HxSTD_StrCmp( (HCHAR *)user_data, pShared->szName ) )
		return TRUE;
	return FALSE;
}

STATIC HBOOL umma_smbc_cb_compareByServereName( void *user_data, void *list_data )
{
	UAPI_SMBC_SERVER_t *pServer = (UAPI_SMBC_SERVER_t*)list_data;
	if ( !HxSTD_StrCmp( (HCHAR *)user_data, pServer->szName ) )
		return TRUE;
	return FALSE;
}

STATIC HBOOL umma_smbc_cb_compareByWorkgroupName( void *user_data, void *list_data )
{
	UMMA_SMBC_WORKGROUP_t *pWorkgroup = (UMMA_SMBC_WORKGROUP_t*)list_data;
	if ( !HxSTD_StrCmp( (HCHAR *)user_data, pWorkgroup->szName ) )
		return TRUE;
	return FALSE;
}

STATIC HBOOL umma_smbc_cb_compareByServerIp( void *user_data, void *list_data )
{
	UAPI_SMBC_SERVER_t *pServer = (UAPI_SMBC_SERVER_t*)list_data;
	if ( !HxSTD_StrCmp( (HCHAR *)user_data, pServer->szIp ) )
		return TRUE;
	return FALSE;
}

STATIC HBOOL umma_smbc_cb_compareByPath( void *user_data, void *list_data )
{
	UAPI_SMBC_MOUNT_t *pMount = (UAPI_SMBC_MOUNT_t*)list_data;
	if ( !HxSTD_StrCmp( (HCHAR *)user_data, pMount->szPath ) )
		return TRUE;
	return FALSE;
}

static void umma_smbc_cb_freeShared( void *data )
{
	HLIB_STD_MemFree(data);
}

static void umma_smbc_cb_freeServer( void *data )
{
	UAPI_SMBC_SERVER_t *pstServer = (UAPI_SMBC_SERVER_t *)data;
	if ( pstServer->pstSharedList )
	{
		umma_smbc_cb_freeShared( pstServer->pstSharedList );
	}
	HLIB_STD_MemFree(pstServer);
}

static void umma_smbc_cb_freeWorkgroup( void *data )
{
	UMMA_SMBC_WORKGROUP_t *pstWorkgroup = (UMMA_SMBC_WORKGROUP_t *)data;
	pstWorkgroup->pstServerList = HLIB_LIST_RemoveAllFunc( pstWorkgroup->pstServerList, umma_smbc_cb_freeServer );
	HLIB_STD_MemFree(pstWorkgroup);
}

static void umma_smbc_netconf_callback(HINT32 event, void* data)
{
	HUINT32 param1 = 0 ; // param2 = 0, param3 = 0;

	if(data)
		param1 = ((NAPINetConfInfo_t *)data)->id;

	switch( event )
	{
		case NAPI_EVENT_NETCONF_CONN_SUCCESS:
			break;
		case NAPI_EVENT_NETCONF_CONN_FAIL:
			break;
		case NAPI_EVENT_NETCONF_AUTOIP_SUCCESS:
			break;
		case NAPI_EVENT_NETCONF_AUTOIP_FAIL:
			break;
		case NAPI_EVENT_NETCONF_LINK_CONNECTED:
			break;
		case NAPI_EVENT_NETCONF_LINK_DISCONNECTED:
			break;
		case NAPI_EVENT_NETCONF_STATUS_UPDATED:
			break;
		default :
			return ;
	}

	return;
}

static	UMMA_SMBC_WORKGROUP_t *umma_smbc_findWorkgroup( UMMA_SMBC_t *pSmbc, HCHAR *pszStr, COMPARE_FUNC_t pCbCmp )
{
	HxList_t *pWorkgroupItem = NULL;

	pWorkgroupItem = HLIB_LIST_FindEx( pSmbc->pstWorkgroupList, (void *)pszStr, pCbCmp );
	if ( pWorkgroupItem )
		return (UMMA_SMBC_WORKGROUP_t *)HLIB_LIST_Data(pWorkgroupItem);
	return  NULL;
}

static  UAPI_SMBC_SERVER_t *umma_smbc_findServer( UMMA_SMBC_t *pSmbc, HCHAR *pszStr, COMPARE_FUNC_t pCbCmp )
{
	HxList_t *pWorkgroupItem = NULL;
	HxList_t *pServerItem = NULL;
	UMMA_SMBC_WORKGROUP_t *pWork = NULL;
	UAPI_SMBC_SERVER_t *pServer = NULL;

	pWorkgroupItem = HLIB_LIST_First(pSmbc->pstWorkgroupList);
	while ( pWorkgroupItem )
	{
		pWork = (UMMA_SMBC_WORKGROUP_t *) HLIB_LIST_Data(pWorkgroupItem);

		pServerItem = HLIB_LIST_FindEx( pWork->pstServerList, (void *)pszStr, pCbCmp );
		if ( pServerItem )
		{
			pServer = (UAPI_SMBC_SERVER_t *)HLIB_LIST_Data(pServerItem);
			break;
		}
		pWorkgroupItem = HLIB_LIST_Next(pWorkgroupItem);
	}

	return pServer;
}

static  UAPI_SMBC_SHARED_t *umma_smbc_findShared( UAPI_SMBC_SERVER_t *pstServer, HCHAR *pszSharedName )
{
	HINT32 i = 0;
	if ( NULL == pstServer )
	{
		return NULL;
	}

	while( i < pstServer->nSharedCnt )
	{
		if ( !HxSTD_StrCmp( pszSharedName, pstServer->pstSharedList[i].szName ) )
		{
			return &pstServer->pstSharedList[i];
		}
		i++;
	}
	return NULL;
}

STATIC HBOOL umma_smbc_isMount(const HCHAR *path)
{
	HBOOL ret = FALSE;

	HCHAR line[256];
	HCHAR devname[80];
	HCHAR mountdir[80];
	HCHAR fstype[12];

	FILE *fp = fopen("/proc/mounts", "r");
	if (NULL == fp) {
		HxLOG_Warning("cannot read \"/proc/mounts\"\n");
		ret = FALSE;
	}
	else
	{
		while (fgets(line, 255, fp))
		{
			sscanf(line, "%80s%80s%12s", devname, mountdir, fstype);
			if (0 == HxSTD_StrCmp(mountdir, path)) // || TRUE == HLIB_STD_StrStartWith(mountdir, path))
			{
				ret = TRUE;
				break;
			}
		}
		fclose(fp);
		fp = NULL;
	}
	return ret;
}

static UAPI_SMBC_SHARED_t * umma_smbc_add_shared( UAPI_SMBC_SERVER_t *pstServer, UAPI_SMBC_SHARED_t *pstShared  )
{
	UAPI_SMBC_SHARED_t *pNewSharedList = NULL;
	UAPI_SMBC_SHARED_t *pFindShared = NULL;
	if ( NULL == pstServer || NULL== pstShared )
		return NULL;

	pFindShared = umma_smbc_findShared(pstServer, pstShared->szName);
	if ( pFindShared )
	{
		HLIB_STD_MemFree( pstShared );
		return pFindShared;
	}

	HxSTD_PrintToStrN(pstShared->szMountPath, sizeof(pstShared->szMountPath), "%s/%s/%s", SAMBAC_MOUNT_ROOT, pstServer->szName, pstShared->szName);

	if ( 0 == (pstServer->nSharedCnt % 4) )
	{
		HINT32 nSize = pstServer->nSharedCnt + 4;
		pNewSharedList = (UAPI_SMBC_SHARED_t *)HLIB_STD_MemAlloc( sizeof(UAPI_SMBC_SHARED_t) * nSize );
		if ( pstServer->pstSharedList )
		{
			HxSTD_MemCopy( pNewSharedList, pstServer->pstSharedList, sizeof(UAPI_SMBC_SHARED_t) * pstServer->nSharedCnt);
			HLIB_STD_MemFree(pstServer->pstSharedList);
			pstServer->pstSharedList = NULL;
		}
		pstServer->pstSharedList = pNewSharedList;
	}

	HxSTD_MemCopy( &pstServer->pstSharedList[pstServer->nSharedCnt], pstShared, sizeof(UAPI_SMBC_SHARED_t) );
	HLIB_STD_MemFree( pstShared );

	pstShared = &pstServer->pstSharedList[pstServer->nSharedCnt];
	pstServer->nSharedCnt++;
	return pstShared;
}

static HERROR umma_smbc_remove_shared( UAPI_SMBC_SERVER_t *pstServer, UAPI_SMBC_SHARED_t *pstShared  )
{
	if ( NULL == pstServer || NULL== pstShared )
		return ERR_FAIL;

//	pstServer->pstSharedList = HLIB_LIST_Remove( pstServer->pstSharedList, pstShared );
	pstServer->nSharedCnt--;
	return ERR_OK;
}

static UAPI_SMBC_SERVER_t * umma_smbc_add_server( UMMA_SMBC_WORKGROUP_t *pstWorkgroup, UAPI_SMBC_SERVER_t *pstServer )
{
	HxList_t *pItem = NULL;
	if ( NULL == pstWorkgroup || NULL== pstServer )
		return NULL;

	HxSEMT_Get(s_ulListSemId);

	pItem = HLIB_LIST_FindEx( pstWorkgroup->pstServerList, pstServer->szName, umma_smbc_cb_compareByServereName);
	if (pItem )
	{
		HLIB_STD_MemFree( pstServer );
		HxSEMT_Release(s_ulListSemId);
		return (UAPI_SMBC_SERVER_t *)HLIB_LIST_Data(pItem);
	}

	HxSTD_StrNCpy(pstServer->szWorkGroup, pstWorkgroup->szName, sizeof(pstServer->szWorkGroup)-1);
	pstWorkgroup->pstServerList = HLIB_LIST_Append( pstWorkgroup->pstServerList, pstServer );
	pstWorkgroup->nServerCnt++;

	HxSEMT_Release(s_ulListSemId);
	return pstServer;
}

static HERROR umma_smbc_remove_server( UMMA_SMBC_WORKGROUP_t *pstWorkgroup, UAPI_SMBC_SERVER_t *pstServer )
{
	if ( NULL == pstWorkgroup || NULL== pstServer )
		return ERR_FAIL;

	HxSEMT_Get(s_ulListSemId);

	pstWorkgroup->pstServerList = HLIB_LIST_Remove( pstWorkgroup->pstServerList, pstServer );
	umma_smbc_cb_freeServer( pstServer );
	pstWorkgroup->nServerCnt--;

	HxSEMT_Release(s_ulListSemId);

	return ERR_OK;
}

static UMMA_SMBC_WORKGROUP_t * umma_smbc_add_workgroup( UMMA_SMBC_t *pstSmbc, UMMA_SMBC_WORKGROUP_t *pstWorkgroup )
{
	HxList_t *pItem = NULL;
	if ( NULL == pstSmbc || NULL == pstWorkgroup )
		return NULL;

	pItem = HLIB_LIST_FindEx( pstSmbc->pstWorkgroupList, pstWorkgroup->szName, umma_smbc_cb_compareByWorkgroupName);
	if ( pItem )
	{
		HLIB_STD_MemFree( pstWorkgroup );
		return (UMMA_SMBC_WORKGROUP_t *)HLIB_LIST_Data(pItem);
	}
	pstSmbc->pstWorkgroupList = HLIB_LIST_Append( pstSmbc->pstWorkgroupList, pstWorkgroup );
	return pstWorkgroup;
}

static HERROR umma_smbc_remove_workgroup( UMMA_SMBC_t *pstSmbc, UMMA_SMBC_WORKGROUP_t *pstWorkgroup )
{
	if ( NULL == pstSmbc || NULL == pstWorkgroup )
		return ERR_FAIL;

	pstSmbc->pstWorkgroupList = HLIB_LIST_Remove( pstSmbc->pstWorkgroupList, pstWorkgroup );
	umma_smbc_cb_freeWorkgroup(pstWorkgroup);
	return ERR_OK;
}

static HERROR umma_smbc_add_mountedInfo( UMMA_SMBC_t *pstSmbc, UAPI_SMBC_MOUNT_t *pstMountInfo )
{
	if ( NULL == pstSmbc || NULL == pstMountInfo )
		return ERR_FAIL;

	pstSmbc->pstMountedList = HLIB_LIST_Append( pstSmbc->pstMountedList, pstMountInfo );
	return ERR_OK;
}

STATIC HERROR umma_smbc_remove_mountedInfo( UMMA_SMBC_t *pstSmbc, UAPI_SMBC_MOUNT_t *pstMountInfo )
{
	if ( NULL == pstSmbc || NULL == pstMountInfo )
		return ERR_FAIL;

	pstSmbc->pstMountedList = HLIB_LIST_Remove( pstSmbc->pstMountedList, pstMountInfo );
	HLIB_STD_MemFree(pstMountInfo);
	return ERR_OK;
}

static HERROR umma_smbc_parser_for_shared( const HCHAR *pszBuf, UAPI_SMBC_SHARED_t **ppstShared )
{
	HCHAR 	szSharedFolderName[UMMA_SMBC_PATH_LEN] = { 0 };
	HINT32  carrIdx = 0;
	UAPI_SMBC_SHARED_t *pNewShared = NULL;

	if ( NULL == pszBuf || NULL == ppstShared )
	{
		return ERR_FAIL;
	}

	if( NULL != HxSTD_StrStr(pszBuf, "$"))  // $ë¥??¬í•¨?˜ëŠ” ?´ë”???œìŠ¤???´ë”?´ë?ë¡??œì™¸
	{ // ex) \c$, \sharedfolder
		return ERR_FAIL;
	}

	HxSTD_StrNCpy(szSharedFolderName, pszBuf + HxSTD_StrLen("SHARED:\\"), sizeof(szSharedFolderName)-1);
	carrIdx = HxSTD_StrLen(szSharedFolderName)-1;
	/* ê°œí–‰ ë¬¸ìž ?œê±° */
	if ( carrIdx > -1 && '\n'==szSharedFolderName[carrIdx] )
		szSharedFolderName[carrIdx] = '\0';

	pNewShared = (UAPI_SMBC_SHARED_t *)HLIB_STD_MemAlloc(sizeof(UAPI_SMBC_SHARED_t));
	HxSTD_MemSet( pNewShared, 0x00, sizeof(UAPI_SMBC_SHARED_t) );

	HxSTD_StrNCpy(pNewShared->szName, szSharedFolderName, sizeof(pNewShared->szName)-1);

	*ppstShared = pNewShared;

	return ERR_OK;
}

static HERROR umma_smbc_parser_for_server( const HCHAR *pszBuf, UAPI_SMBC_SERVER_t **ppstServer )
{
	HCHAR 	szServerName[UMMA_SMBC_SRVNAME_LEN] = { 0 };
	UAPI_SMBC_SERVER_t *pNewServer = NULL;
	HINT32 serverLen = 0;

	if ( NULL == pszBuf || NULL == ppstServer )
	{
		return ERR_FAIL;
	}

	HxSTD_StrNCpy(szServerName, pszBuf + HxSTD_StrLen("SERVER:\\\\"), sizeof(szServerName)-1);
	serverLen = HxSTD_StrLen(szServerName);
	/* ê°œí–‰ ë¬¸ìž ?œê±° */
	if ( serverLen > 0 && '\n'==szServerName[serverLen-1] )
		szServerName[serverLen-1] = '\0';

	pNewServer = (UAPI_SMBC_SERVER_t *)HLIB_STD_MemAlloc(sizeof(UAPI_SMBC_SERVER_t));
	HxSTD_MemSet( pNewServer, 0x00, sizeof(UAPI_SMBC_SERVER_t) );

	HxSTD_StrNCpy(pNewServer->szName, szServerName, sizeof(pNewServer->szName)-1);

	*ppstServer =  pNewServer;
	return ERR_OK;
}

static HERROR umma_smbc_parser_for_workgroup( const HCHAR *pszBuf, UMMA_SMBC_WORKGROUP_t **ppstWorkGroup )
{
	HCHAR 	szWorkGroup[UMMA_SMBC_WORKGROUP_LEN] = { 0 };
	UMMA_SMBC_WORKGROUP_t *pNewWorkgroup = NULL;
	HINT32 workLen = 0;

	if ( NULL == pszBuf || NULL == ppstWorkGroup )
	{
		return ERR_FAIL;
	}

	HxSTD_StrNCpy( szWorkGroup, pszBuf + HxSTD_StrLen("WORKGRUOP:"), sizeof(szWorkGroup)-1 );
	workLen = HxSTD_StrLen(szWorkGroup);
	/* ê°œí–‰ ë¬¸ìž ?œê±° */
	if ( workLen > 0 && '\n' == szWorkGroup[ workLen -1 ])
		szWorkGroup[workLen -1] = '\0';

	pNewWorkgroup = (UMMA_SMBC_WORKGROUP_t *)HLIB_STD_MemAlloc(sizeof(UMMA_SMBC_WORKGROUP_t));
	HxSTD_MemSet( pNewWorkgroup, 0x00, sizeof(UMMA_SMBC_WORKGROUP_t) );

	HxSTD_StrNCpy(pNewWorkgroup->szName, szWorkGroup, sizeof(pNewWorkgroup->szName)-1);

	*ppstWorkGroup = pNewWorkgroup;
	return ERR_OK;
}

static inline void umma_smbc_parser_for_failedConnection( const HCHAR *pszBuf, HCHAR *pszName, HINT32 nNameSize  )
{
	HCHAR *pTmp = NULL;
	if ( NULL == pszBuf || NULL == pszName )
		return ;

	pTmp = (HCHAR*)(pszBuf + HxSTD_StrLen("cli_start_connection: failed to connect to "));
	HxSTD_StrNCpy( pszName , pTmp, nNameSize-1 );
	pTmp = HxSTD_StrRChar( pszName, '<');
	if (pTmp )
		*pTmp = '\0';
}

STATIC HERROR umma_smbc_resolve_serverIp( UAPI_SMBC_SERVER_t *pstServer )
{
	FILE	*pfile = NULL;
	HCHAR 	szBuf[UMMA_SMBC_STDOUT_LEN] = { 0 } ;
	HCHAR 	szCmd[256] = { 0 };
	HCHAR	szServerIp[UMMA_SMBC_IP_LEN];
	HCHAR	*pTmp = NULL;
	HINT32  carrIdx = 0;
	HBOOL	bFind = FALSE;
	HERROR ret = ERR_FAIL;

	HxSTD_PrintToStrN(szCmd, sizeof(szCmd), "%s -N \"%s\" -s %s", SAMBAC_CMD_NMBLOOKUP, pstServer->szName, SAMBAC_CONF_PATH);

	ret = SEUAPI_PopenCmd_Smbc(szCmd, SAMBAC_NMBLOOKUP_RESULT);
	if (ret == ERR_FAIL)
	{
		HxLOG_Error("PopenCmd failed.\n");
		return ERR_FAIL;
	}

	pfile = fopen(SAMBAC_NMBLOOKUP_RESULT, "r");
	if( NULL == pfile )
	{
		HxLOG_Error("Cannot open %s.\n", SAMBAC_NMBLOOKUP_RESULT);
		return ERR_FAIL;
	}

	while (fgets( szBuf, sizeof(szBuf), pfile ) )
	{
		if(NULL != (pTmp = HxSTD_StrStr(szBuf, "IP:")))
		{
			HxSTD_StrNCpy( szServerIp, pTmp + HxSTD_StrLen("IP:"), sizeof(szServerIp)-1 );
			carrIdx = HxSTD_StrLen(szServerIp)-1;
			if ( carrIdx > -1 && '\n'==szServerIp[carrIdx] )
				szServerIp[carrIdx] = '\0';

			HxSTD_StrNCpy( pstServer->szIp, szServerIp, sizeof(pstServer->szIp)-1 );
			bFind = TRUE;
		}

	}

	fclose(pfile);
	pfile = NULL;

	if ( bFind )
		return ERR_OK;

	return ERR_FAIL;
}

STATIC void	umma_smbc_print_sharedInfo( UAPI_SMBC_SHARED_t* pSharedInfo )
{
	if ( NULL == pSharedInfo )
		return ;
	HxLOG_Info("  [Shared] Name : %s\n", pSharedInfo->szName);
	HxLOG_Info("   =>MountPath  : %s\n", pSharedInfo->szMountPath);
//	HxLOG_Info("   =>log        : %s\n", pSharedInfo->szLogMsg);
//	HxLOG_Info("   =>ID         : %s\n", pSharedInfo->szId);
//	HxLOG_Info("   =>Password   : %s\n", pSharedInfo->szPassword);
//	HxLOG_Info("   =>state      : %d\n", pSharedInfo->nState);
//	HxLOG_Info("   =>isMounted  : %d\n", pSharedInfo->bMounted);
}

STATIC void	umma_smbc_print_serverInfo( UAPI_SMBC_SERVER_t *pstServer )
{
	if ( NULL == pstServer )
		return ;
	HxLOG_Info("[ Server ] Name : %s\n",  pstServer->szName );
	HxLOG_Info(" => Workgroup   : %s\n",  pstServer->szWorkGroup );
	HxLOG_Info(" => IP Address  : %s\n",  pstServer->szIp );
	HxLOG_Info(" => Shared Cnt  : %d\n",  pstServer->nSharedCnt );
	{
		HINT32 i = 0;
		UAPI_SMBC_SHARED_t *pShared = NULL;
		while ( i < pstServer->nSharedCnt )
		{
		    pShared = &pstServer->pstSharedList[i];
			umma_smbc_print_sharedInfo( pShared );
			i++;
		}
	}
}

STATIC void	umma_smbc_print_workgroupInfo( UMMA_SMBC_WORKGROUP_t *pstWorkgroup )
{
	if ( NULL == pstWorkgroup )
		return ;
	HxLOG_Info("[ WORKGROUP ] Name : %s\n",  pstWorkgroup->szName );
}

STATIC void	umma_smbc_print_mountInfo( UAPI_SMBC_MOUNT_t *pstMountInfo )
{
	if ( NULL == pstMountInfo )
		return ;
	HxLOG_Info("  [Mount] Server Name : %s, Shared Name : %s\n", pstMountInfo->szServerName, pstMountInfo->szSharedName);
	HxLOG_Info("   =>IP  : %s\n", pstMountInfo->szIp);
	HxLOG_Info("   =>Path  : %s\n", pstMountInfo->szPath);
	HxLOG_Info("   =>ID         : %s\n", pstMountInfo->szId);
	HxLOG_Info("   =>Password   : %s\n", pstMountInfo->szPassword);
	HxLOG_Info("   =>log        : %s\n", pstMountInfo->szLogMsg);
	HxLOG_Info("   =>nError  : %d\n", pstMountInfo->nError);
}

STATIC void umma_smbc_initResource(void)
{
	HxList_t *pSharedList = NULL;
	int nSharedListCount = HLIB_LIST_Length(pSharedList);
	if(0 < nSharedListCount)
	{

		HLIB_LIST_RemoveAllFunc(pSharedList, HLIB_STD_MemFree_CB);

		pSharedList = NULL;
	}
}

static void umma_smbc_sendMsgToMainTask( UMMA_SMBC_t *pstSmbCtx, UMMA_SMBC_CMD_e eCmd, void *data, HINT32 nReqId )
{
	UMMA_SMBC_MSG_t	stMsg;
	stMsg.eCmd = eCmd;
	stMsg.pvData = data;
	stMsg.nReqId = nReqId;
	HLIB_MSG_Send(pstSmbCtx->ulMsgQueue, &stMsg, sizeof(UMMA_SMBC_MSG_t));
}

static HERROR umma_smbc_init( UMMA_SMBC_t *pstSmbc )
{
	pstSmbc->bInit = TRUE;
	pstSmbc->bScanning = FALSE;
	return ERR_OK;
}

static void umma_smbc_cb_sendServerInfo ( void *data )
{
	UAPI_SMBC_SERVER_t *pServerInfo = (UAPI_SMBC_SERVER_t *)data;
	UMMA_SMBC_t *pSmbc = (UMMA_SMBC_t *)umma_smbc_getInstanceWO();

	if ( !pServerInfo )
		return ;
	if ( ERR_OK == umma_smbc_resolve_serverIp(pServerInfo) )
		umma_smbc_notifyEventToClient( pSmbc, eUMMA_SMBC_EVT_SERVER_ADD, pServerInfo, TRUE );
	else
	{
		//remove
	}
}

static HERROR umma_smbc_scenario_preScanReuslt( HxList_t *pWorkgroupList )
{
	HxList_t *pList = pWorkgroupList;
	UMMA_SMBC_WORKGROUP_t *pWorkgroup = NULL;
	while ( pList )
	{
		pWorkgroup = HLIB_LIST_Data(pList);
		HLIB_LIST_Foreach(pWorkgroup->pstServerList, umma_smbc_cb_sendServerInfo );
		pList = HLIB_LIST_Next(pList);
	}
	return ERR_OK;
}

STATIC void	umma_smbc_doScan( void *pParam )
{
	UMMA_SMBC_t *pSmbcCtx = (UMMA_SMBC_t *)pParam;

	UMMA_SMBC_WORKGROUP_t	*pNewWorkgroup = NULL;
	UMMA_SMBC_WORKGROUP_t	*pCurrentWorkgroup = NULL;
	UAPI_SMBC_SERVER_t		*pNewServer = NULL;
	UAPI_SMBC_SERVER_t		*pCurrentServer = NULL;
	UAPI_SMBC_SHARED_t		*pNewShared = NULL;

	FILE	*pfile = NULL;
	HCHAR	*pTmp = NULL;
	HCHAR 	szBuf[UMMA_SMBC_STDOUT_LEN] = { 0 } ;
	HCHAR	szCmd[512] = {0,};
	HERROR	ret = ERR_FAIL;

	{
		NAPINetConfInfo_t network_info;
		HINT32 devIdx = NAPI_NetConf_GetDefaultIndex();

		HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));
		NAPI_NetConf_GetInfo(devIdx, &network_info);

		HxSTD_StrNCpy(pSmbcCtx->szIpAddr, network_info.ip_info.ipv4.ipAddr, sizeof(pSmbcCtx->szIpAddr));

#if defined(CONFIG_SUPPORT_UMMA_SAMBA_SERVER)
		if ( TRUE == UMMA_SMBS_IsStarted() )
		{
			UMMA_SMBS_GetName( pSmbcCtx->szSmbsName, sizeof(pSmbcCtx->szSmbsName) );
		}
		else
		{
			HxSTD_MemSet( pSmbcCtx->szSmbsName, 0x00, sizeof(pSmbcCtx->szSmbsName) );
		}
#endif
	}

	if ( pSmbcCtx->bExistScanResult )
	{
		umma_smbc_scenario_preScanReuslt( pSmbcCtx->pstWorkgroupList );
	}

	/* 1. run by [smbtree] and make server-list */
	HxLOG_Print("\n************ SMB TREE (+) *************\n");
	HxSTD_snprintf(szCmd, sizeof(szCmd), "%s -N -s %s 2>&1", SAMBAC_CMD_SMBTREE, SAMBAC_CONF_PATH);

	ret = SEUAPI_PopenCmd_Smbc(szCmd, SAMBAC_SMBTREE_RESULT);
	if (ret == ERR_FAIL)
	{
		HxLOG_Error("[SMBC] smbtree failed.\n");
		return;
	}

	pfile = fopen(SAMBAC_SMBTREE_RESULT, "r");
	if (NULL == pfile)
	{
		HxLOG_Error("[SMBC] Cannot open %s.\n", SAMBAC_SMBTREE_RESULT);
		return ;
	}
	/* 2. parsing output data from pipe  */
	while (fgets( szBuf, sizeof(szBuf), pfile ) )
	{
		if(NULL != (pTmp = HxSTD_StrStr(szBuf, "SHARED:\\")))
		{
			if ( ERR_OK != umma_smbc_parser_for_shared( pTmp, &pNewShared ) )
			{
				continue;
			}
			pNewShared = umma_smbc_add_shared( pCurrentServer, pNewShared );
		}
		else if (NULL != (pTmp = HxSTD_StrStr(szBuf, "SERVER:\\\\")))
		{
			if ( ERR_OK != umma_smbc_parser_for_server( pTmp, &pNewServer ) )
			{
				continue;
			}

			pNewServer = umma_smbc_add_server( pCurrentWorkgroup, pNewServer );
			/* Server ?¨ìœ„ë¡?Notify */
			if ( pCurrentServer != pNewServer )
			{
				HxLOG_Print("[UMMA_SMBC] Searched server : %s\n", pNewServer->szName);
				/* szIpë¥?ì²´í¬?˜ëŠ” ?´ìœ ???´ì „??ê²€?‰ëœ Serverë¥??„ë‹¬?˜ì? ?Šê¸° ?„í•¨??*/
				if ( NULL != pCurrentServer  && !HxSTD_StrLen(pCurrentServer->szIp) )
				{
					umma_smbc_sendMsgToMainTask( pSmbcCtx, eUMMA_SMBC_CMD_RESOLV_IP, pCurrentServer, 0 );
				}
				pCurrentServer = pNewServer;
			}
		}
		else if (NULL != (pTmp = HxSTD_StrStr(szBuf, "WORKGRUOP:")))
		{
			if ( ERR_OK != umma_smbc_parser_for_workgroup( pTmp, &pNewWorkgroup ) )
			{
				continue;
			}

			pNewWorkgroup = umma_smbc_add_workgroup( pSmbcCtx, pNewWorkgroup );
			pCurrentWorkgroup = pNewWorkgroup;
		}
		else if (NULL !=(pTmp = HxSTD_StrStr(szBuf, "cli_start_connection: failed to connect to ")) )
		{
			HCHAR szName[UMMA_SMBC_FOLDER_LEN] = { 0 };
			UAPI_SMBC_SERVER_t *pFindServer = NULL;

			umma_smbc_parser_for_failedConnection( pTmp, szName, sizeof(szName) );
			HxLOG_Error("failed to connect : %s\n", szName);
			pFindServer = umma_smbc_findServer( pSmbcCtx, szName, umma_smbc_cb_compareByServereName );
			if ( pFindServer )
			{
				umma_smbc_notifyEventToClient( pSmbcCtx, eUMMA_SMBC_EVT_SERVER_REMOVE, pFindServer, TRUE );
				umma_smbc_remove_server( pCurrentWorkgroup, pFindServer );
			}
		}
	}

	/* Server ?¨ìœ„ë¡?Notify */
	if( pCurrentServer  && !HxSTD_StrLen(pCurrentServer->szIp))
	{
		umma_smbc_sendMsgToMainTask( pSmbcCtx, eUMMA_SMBC_CMD_RESOLV_IP, pCurrentServer, 0 );
	}

	HxLOG_Print("\n************ SMB TREE(-) *************\n");
	pSmbcCtx->bScanning = FALSE;
	umma_smbc_sendMsgToMainTask( pSmbcCtx, eUMMA_SMBC_CMD_SCAN_END, NULL, 0 );

	fclose(pfile);
	pfile = NULL;
	VK_TASK_Sleep(100);
}

STATIC HERROR umma_smbc_scanStart( UMMA_SMBC_t *pstSmbc )
{
	HULONG tid = 0;

	if ( pstSmbc->bScanning )
		return ERR_FAIL;

	pstSmbc->bScanning = TRUE;

	HxLOG_Print("\n************ SCAN START(*) *************\n");
	VK_TASK_Create( umma_smbc_doScan, 70, SIZE_64K, UMMA_SMBC_SCANTASK_NAME, (void*)pstSmbc, (unsigned long*)&tid, FALSE);
	VK_TASK_Start(tid);
	pstSmbc->ulScanTaskId = tid;

	return ERR_OK;
}

STATIC HERROR umma_smbc_scanStop( UMMA_SMBC_t *pstSmbc )
{

	HxLOG_Print("\n************ SCAN STOP(*) *************\n");
	pstSmbc->bScanning = FALSE;

	return ERR_OK;
}

STATIC HERROR umma_smbc_mount( UMMA_SMBC_t *pstSmbc, UAPI_SMBC_MOUNT_t *pstMountReq )
{
	HCHAR szBuf[UMMA_SMBC_STDOUT_LEN] = { 0 };
	FILE	*pipeFd = NULL;
	HERROR err = ERR_OK;

	if ( NULL == pstMountReq )
		return ERR_FAIL;
	if( TRUE  == umma_smbc_isMount(pstMountReq->szPath) )
	{
		HxLOG_Debug("[%s] Already mounted [%s] \n", __FUNCTION__, pstMountReq->szPath);
		return ERR_OK;
	}

	if ( 0 != HxSTD_StrNCmp(SAMBAC_MOUNT_ROOT, pstMountReq->szPath, HxSTD_StrLen(SAMBAC_MOUNT_ROOT) ) )
	{
		return ERR_FAIL;
	}

	HxLOG_Print("\n************ MOUNT (+) *************\n");

	err = SEUAPI_MountPartition_Smbc(pstMountReq->szIp, pstMountReq->szSharedName, pstMountReq->szPath, pstMountReq->szId, pstMountReq->szPassword);

	pipeFd = fopen( SAMBAC_CMD_MOUNT_RESULT, "r" );
	if ( NULL == pipeFd )
		return ERR_FAIL;

	while( fgets(szBuf, sizeof(szBuf), pipeFd))
	{
		if(NULL != HxSTD_StrStr(szBuf, "error"))
		{
			HINT32 carrIdx = 0;
			HxLOG_Error("%s\n", szBuf);
			/* ê°œí–‰ ë¬¸ìž ?œê±° */
			carrIdx = HxSTD_StrLen(szBuf)-1;
			if ( carrIdx > -1 && '\n' == szBuf[carrIdx] )
				szBuf[carrIdx] = '\0';
			HxSTD_StrNCpy( pstMountReq->szLogMsg, szBuf, sizeof(pstMountReq->szLogMsg)-1 );
			err = ERR_FAIL;
		}
	}
	HxLOG_Print("\n************ MOUNT (-) Error(%d) *************\n", err);
	if ( ERR_OK == err )
	{
		pstMountReq->nError = ERR_OK;
		UMMA_FM_AddStorage( pstMountReq->szPath, "smb", eDxINTERFACE_SAMBA, 0);
	}
	else
		pstMountReq->nError = ERR_FAIL;

	fclose(pipeFd);


	return err;
}

STATIC HERROR umma_smbc_unmount( UMMA_SMBC_t *pstSmbc, const HCHAR *pszPath  )
{
	HERROR ret = ERR_FAIL;
	int nRetry = 0;
	int nError = 0;

	if ( NULL == pszPath )
		return ERR_FAIL;
	if( TRUE != umma_smbc_isMount(pszPath) )
	{
		HxLOG_Debug("[%s] Already unmounted [%s] \n", __FUNCTION__, pszPath);
		return ERR_OK;
	}
	UMMA_FM_RemoveStorage((HCHAR *)pszPath);
RETRY:
	HxLOG_Debug("Do Umount [%s] \n", pszPath);

	nError = SEUAPI_UmountPartition_Smbc((HUINT8*)pszPath);
	if(0 == nError) // success
	{
		HxLOG_Debug("Success to umount [%s] \n", pszPath);
	}
	else   			// fail
	{
		HxLOG_Error("Umount return Error : %d (%s) \n", errno, strerror(errno));
		if(5 > nRetry++) goto RETRY;
		ret = ERR_FAIL;
	}
	/* ê²°êµ­ ?¤íŒ¨?˜ë”?¼ë„ umount ?œê²ƒ?¼ë¡œ ê°„ì£¼?´ì•¼??*/
	return ret;
}

STATIC HERROR umma_smbc_unmount_all( UMMA_SMBC_t *pstSmbc )
{
	HERROR eRet = ERR_FAIL;
	HxList_t *pItem = NULL;
	UAPI_SMBC_MOUNT_t *pMountInfo = NULL;

	pstSmbc->pstMountedList= HLIB_LIST_First( pstSmbc->pstMountedList );
	while ( pItem )
	{
		pMountInfo = HLIB_LIST_Data(pItem);
		pItem = HLIB_LIST_Next(pItem);

		umma_smbc_unmount( pstSmbc, pMountInfo->szPath );
		umma_smbc_remove_mountedInfo( pstSmbc, pMountInfo );
	}
	pstSmbc->pstMountedList = NULL;
	return eRet;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
#define __________Public_Function__________

static void		umma_smbc_mainLoop(void *arg);
static void		umma_smbc_eventListener(void *arg);

static UMMA_SMBC_t *	umma_smbc_getInstance(void)
{
	if (NULL == s_pstUmmaSmbc)
	{
		HULONG tid = 0;
		HERROR hErr;

		s_pstUmmaSmbc = (UMMA_SMBC_t *)UMMA_Calloc(sizeof(UMMA_SMBC_t));

		HxSEMT_Create(&s_pstUmmaSmbc->ulSemId, "ummasmbcsem", 0);

		HxSEMT_Get(s_pstUmmaSmbc->ulSemId);
		hErr = UMMA_RpcProtocol_GetRpcHandle(&s_pstUmmaSmbc->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error!! (Get RPC Handle)\n");
			HxLOG_Assert(s_pstUmmaSmbc->nRpcHandle);
		}

		HLIB_MSG_Create( 128, sizeof(UMMA_EVT_t), "ummaSmbcEvt",  &s_pstUmmaSmbc->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO );

		VK_TASK_Create( umma_smbc_mainLoop, 70, SIZE_64K, "ummaSmbcMainHandler", (void *)s_pstUmmaSmbc, &tid, FALSE );
		VK_TASK_Start(tid);

		s_pstUmmaSmbc->ulMainTaskId = tid;

		HxSEMT_Create(&s_ulListSemId, "ummasmbcListsem", 0);
	}
	else
	{
		HxSEMT_Get(s_pstUmmaSmbc->ulSemId);
	}

	return s_pstUmmaSmbc;
}
static UMMA_SMBC_t *	umma_smbc_getInstanceWO(void)
{
	return s_pstUmmaSmbc;
}

static void		umma_smbc_releaseInstance(UMMA_SMBC_t *pstSmbc)
{
	HxSEMT_Release(pstSmbc->ulSemId);
}

static void		umma_smbc_mainLoop(void *arg)
{
	UMMA_SMBC_t *pstSmbc = (UMMA_SMBC_t *)arg;
	UMMA_SMBC_MSG_t stMsg = { 0 };
	HERROR ret = ERR_FAIL;
	while (1)
	{
		if ( VK_OK != HLIB_MSG_Receive(pstSmbc->ulMsgQueue, &stMsg, sizeof(UMMA_SMBC_MSG_t)) )
		{
			VK_TASK_Sleep(100);
			continue;
		}

		switch ( stMsg.eCmd )
		{
			case eUMMA_SMBC_CMD_SCAN_START :
				umma_smbc_notifyEventToClient( pstSmbc, eUMMA_SMBC_EVT_SCAN_START, NULL, TRUE);
				{
					umma_smbc_scanStart( pstSmbc );
				}
				break;
			case eUMMA_SMBC_CMD_SCAN_STOP :
				{
					umma_smbc_scanStop( pstSmbc );
				}
				umma_smbc_notifyEventToClient( pstSmbc, eUMMA_SMBC_EVT_SCAN_STOP, NULL, TRUE);
				break;
			case eUMMA_SMBC_CMD_SCAN_END :
				{
					if ( pstSmbc->pstWorkgroupList )
						pstSmbc->bExistScanResult = TRUE;

					umma_smbc_notifyEventToClient( pstSmbc, eUMMA_SMBC_EVT_SCAN_END, NULL, TRUE);
				}
				break;

			case eUMMA_SMBC_CMD_MOUNT :
				{
					UAPI_SMBC_MOUNT_t *pMountReq = (UAPI_SMBC_MOUNT_t *)stMsg.pvData;
					UAPI_SMBC_SERVER_t *pServer = NULL;
					UAPI_SMBC_SHARED_t *pShared = NULL;

					ret = umma_smbc_mount( pstSmbc, pMountReq);
					if ( ERR_OK == ret )
						umma_smbc_add_mountedInfo( pstSmbc, pMountReq );

					pServer = umma_smbc_findServer( pstSmbc, pMountReq->szIp, umma_smbc_cb_compareByServerIp );
					if (pServer)
					{
						HxSTD_StrNCpy( pMountReq->szServerName, pServer->szName, sizeof(pMountReq->szServerName)-1 );
					}

					pShared = umma_smbc_findShared( pServer, pMountReq->szSharedName );
					if (pShared && ERR_OK == ret )
					{
						HxSTD_StrNCpy( pMountReq->szSharedName, pShared->szName, sizeof(pMountReq->szSharedName)-1 );
						pShared->bMounted = TRUE;
					}

					umma_smbc_notifyEventToClient( pstSmbc, eUMMA_SMBC_EVT_MOUNTED, pMountReq, TRUE);
					if ( pMountReq )
					{
						HLIB_STD_MemFree( pMountReq );
						pMountReq = NULL;
					}
				}
				break;
			case eUMMA_SMBC_CMD_UNMOUNT :
				{
					HxList_t *pItem = NULL;
					UAPI_SMBC_MOUNT_t *pMountInfo = NULL;
					UMMA_SMBC_UNMOUNT_t *pUnmountReq = (UMMA_SMBC_UNMOUNT_t *)stMsg.pvData;
					UAPI_SMBC_SHARED_t *pShared = NULL;
					UAPI_SMBC_SERVER_t *pServer = NULL;

					ret = umma_smbc_unmount( pstSmbc, pUnmountReq->szPath );

					pItem = HLIB_LIST_FindEx( pstSmbc->pstMountedList, (void *)pUnmountReq->szPath, umma_smbc_cb_compareByPath );
					pMountInfo = HLIB_LIST_Data( pItem );
					if ( NULL == pMountInfo )
					{

					}

					pServer = umma_smbc_findServer( pstSmbc, pMountInfo->szIp, umma_smbc_cb_compareByServerIp );
					if (pServer)
					{
						HxSTD_StrNCpy( pMountInfo->szServerName, pServer->szName, sizeof(pMountInfo->szServerName)-1 );
					}

					pShared = umma_smbc_findShared( pServer, pMountInfo->szSharedName );
					if (pShared && ERR_OK == ret)
					{
						HxSTD_StrNCpy( pMountInfo->szSharedName, pShared->szName, sizeof(pMountInfo->szSharedName)-1 );
						pShared->bMounted = FALSE;
					}

					umma_smbc_notifyEventToClient( pstSmbc, eUMMA_SMBC_EVT_UNMOUNTED, pMountInfo, TRUE);
					if ( pMountInfo && ERR_OK == ret )
					{
						umma_smbc_remove_mountedInfo( pstSmbc, pMountInfo );
						pMountInfo = NULL;
					}
					if ( pUnmountReq )
					{
						HLIB_STD_MemFree( pUnmountReq );
					}
				}
				break;
			case eUMMA_SMBC_CMD_UNMOUNT_ALL :
				{
					(void)umma_smbc_unmount_all( pstSmbc );
				}
				break;
			case eUMMA_SMBC_CMD_RESOLV_IP :
				{
					UAPI_SMBC_SERVER_t *pServerInfo = (UAPI_SMBC_SERVER_t *)stMsg.pvData;
					ret = umma_smbc_resolve_serverIp(pServerInfo);
					if ( ERR_OK == ret )
					{
						if ( HxSTD_StrCmp(pServerInfo->szIp, pstSmbc->szIpAddr) && HxSTD_StrCmp(pServerInfo->szName, pstSmbc->szSmbsName))
						{
							umma_smbc_notifyEventToClient( pstSmbc, eUMMA_SMBC_EVT_SERVER_ADD, pServerInfo, TRUE );
							HxLOG_Print("[UMMA_SMBC] Notify Server: %s\n", pServerInfo->szName,  pServerInfo->szIp);
						}
						else
						{
							UMMA_SMBC_WORKGROUP_t *pWorkgroup = NULL;
							pWorkgroup = umma_smbc_findWorkgroup( pstSmbc, pServerInfo->szWorkGroup, umma_smbc_cb_compareByWorkgroupName );
							umma_smbc_remove_server( pWorkgroup, pServerInfo );
							HxLOG_Debug("[UMMA_SMBC] Remove Server: %s - %s, STB : %s\n", pServerInfo->szName, pServerInfo->szIp, pstSmbc->szIpAddr);
						}
					}
					else
					{
						UMMA_SMBC_WORKGROUP_t *pWorkgroup = NULL;
						pWorkgroup = umma_smbc_findWorkgroup( pstSmbc, pServerInfo->szWorkGroup, umma_smbc_cb_compareByWorkgroupName );
						/* Crash ??? */
						umma_smbc_remove_server( pWorkgroup, pServerInfo );
						HxLOG_Debug("[UMMA_SMBC] server ip is not resolved. Remove Server: %s [%s], STB : %s\n", pServerInfo->szIp, pServerInfo->szName, pstSmbc->szIpAddr);
					}
				}
				break;
			case eUMMA_SMBC_CMD_GET_MOUNTED :
				{

				}
				break;
			default :
				break;
		}
	}
}

#define	______________RPC_API____________________

static HERROR	umma_smbc_rpc_scanStart(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBC_t	*pSmbc;

	UMMA_ASSERT(nArgc == 0);

	pSmbc = umma_smbc_getInstance();
	umma_smbc_sendMsgToMainTask( pSmbc, eUMMA_SMBC_CMD_SCAN_START, NULL, nRequestId );
	umma_smbc_releaseInstance(pSmbc);

	return ERR_OK;
}

static HERROR	umma_smbc_rpc_scanStop(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBC_t	*pSmbc;

	UMMA_ASSERT(nArgc == 0);

	pSmbc = umma_smbc_getInstance();
	umma_smbc_sendMsgToMainTask( pSmbc, eUMMA_SMBC_CMD_SCAN_STOP, NULL, nRequestId );
	umma_smbc_releaseInstance(pSmbc);

	return ERR_OK;
}

static HERROR	umma_smbc_rpc_mountedFolder(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBC_t	*pSmbc;

	UMMA_ASSERT(nArgc == 0);

	pSmbc = umma_smbc_getInstance();
	umma_smbc_sendMsgToMainTask( pSmbc, eUMMA_SMBC_CMD_GET_MOUNTED, NULL, nRequestId );
	umma_smbc_releaseInstance(pSmbc);

	return ERR_OK;
}

static HERROR	umma_smbc_rpc_mount(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBC_t	*pSmbc;
	UAPI_SMBC_MOUNT_t *pMountReq = NULL;

	UMMA_ASSERT(nArgc == 5);

	pMountReq = (UAPI_SMBC_MOUNT_t *)HLIB_STD_MemAlloc(sizeof(UAPI_SMBC_MOUNT_t));
	HxSTD_MemSet( pMountReq, 0x00, sizeof(UAPI_SMBC_MOUNT_t) );

	HxSTD_StrNCpy( pMountReq->szIp, 		HxOBJECT_STR(apArgv[0]), sizeof(pMountReq->szIp)-1 );
	HxSTD_StrNCpy( pMountReq->szSharedName, HxOBJECT_STR(apArgv[1]), sizeof(pMountReq->szSharedName)-1 );
	HxSTD_StrNCpy( pMountReq->szPath, 		HxOBJECT_STR(apArgv[2]), sizeof(pMountReq->szPath)-1 );
	HxSTD_StrNCpy( pMountReq->szId, 		HxOBJECT_STR(apArgv[3]), sizeof(pMountReq->szId)-1 );
	HxSTD_StrNCpy( pMountReq->szPassword, 	HxOBJECT_STR(apArgv[4]), sizeof(pMountReq->szPassword)-1 );

	pSmbc = umma_smbc_getInstance();
	umma_smbc_sendMsgToMainTask( pSmbc, eUMMA_SMBC_CMD_MOUNT, pMountReq, nRequestId );
	umma_smbc_releaseInstance(pSmbc);

	return ERR_OK;
}

static HERROR	umma_smbc_rpc_unmount(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBC_t	*pSmbc;
	UMMA_SMBC_UNMOUNT_t *pUnmountReq = NULL;

	UMMA_ASSERT(nArgc == 2);

	pUnmountReq = (UMMA_SMBC_UNMOUNT_t *)HLIB_STD_MemAlloc(sizeof(UMMA_SMBC_UNMOUNT_t));
	HxSTD_MemSet( pUnmountReq, 0x00, sizeof(UMMA_SMBC_UNMOUNT_t) );

	HxSTD_StrNCpy( pUnmountReq->szPath, 	HxOBJECT_STR(apArgv[1]), sizeof(pUnmountReq->szPath)-1 );

	pSmbc = umma_smbc_getInstance();
	umma_smbc_sendMsgToMainTask( pSmbc, eUMMA_SMBC_CMD_UNMOUNT, pUnmountReq, nRequestId );
	umma_smbc_releaseInstance(pSmbc);

	return ERR_OK;
}

static HERROR	umma_smbc_rpc_unmountAll(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBC_t	*pSmbc;

	UMMA_ASSERT(nArgc == 0);

	pSmbc = umma_smbc_getInstance();
	umma_smbc_sendMsgToMainTask( pSmbc, eUMMA_SMBC_CMD_UNMOUNT_ALL, NULL, nRequestId );
	umma_smbc_releaseInstance(pSmbc);
	return ERR_OK;
}
#define	______________API____________________
static void		umma_smbc_debug_InitCommandTools(void);

HERROR	UMMA_SMBC_Init(void)
{
	UMMA_SMBC_t *pstSmbc = umma_smbc_getInstance();

	HxLOG_Trace();

	HLIB_RPC_RegisterCall(pstSmbc->nRpcHandle, RPC_UAPI_SMBC_ScanStart, NULL, umma_smbc_rpc_scanStart,
		NULL, "SambaTree Scan Start: (void)");
	HLIB_RPC_RegisterCall(pstSmbc->nRpcHandle, RPC_UAPI_SMBC_ScanStop, NULL, umma_smbc_rpc_scanStop,
		NULL, "SambaTree Scan Stop: (void)");
	HLIB_RPC_RegisterCall(pstSmbc->nRpcHandle, RPC_UAPI_SMBC_MountedFolder, NULL, umma_smbc_rpc_mountedFolder,
		NULL, "Samba Get Mounted Folder: (void)");
	HLIB_RPC_RegisterCall(pstSmbc->nRpcHandle, RPC_UAPI_SMBC_Mount, "sssss", umma_smbc_rpc_mount,
		NULL, "Samba Mount : (HCHAR *pszIpAddr, HCHAR *pszShared, HCHAR *pMountPath, HCHAR *pszId, HCHAR *pszPasswd)");
	HLIB_RPC_RegisterCall(pstSmbc->nRpcHandle, RPC_UAPI_SMBC_Unmount, "sss", umma_smbc_rpc_unmount,
		NULL, "Samba Unmount: (HCHAR *pszIpAddr, HCHAR *pszShared, HCHAR *pszMountPath)");
	HLIB_RPC_RegisterCall(pstSmbc->nRpcHandle, RPC_UAPI_SMBC_UnmountAll, NULL, umma_smbc_rpc_unmountAll,
		NULL, "Samba Unmount All: (void)");

	HLIB_RPC_CreateNotification(pstSmbc->nRpcHandle, RPC_UMMA_SMBC_onScanResult, "iibb", "notify for Scanning (HINT32 nRequestId, UAPI_STATUS_t *pstStatus)");
	HLIB_RPC_CreateNotification(pstSmbc->nRpcHandle, RPC_UMMA_SMBC_onMountResult, "iib", "notify for Scanning (HINT32 nRequestId, UAPI_STATUS_t *pstStatus)");

	umma_smbc_init( pstSmbc );
	umma_smbc_releaseInstance(pstSmbc);

	NAPI_Init();
	NAPI_NetConf_RegisterCallback(umma_smbc_netconf_callback);
#ifdef	CONFIG_DEBUG
	umma_smbc_debug_InitCommandTools();
#endif

	return 0;
}

static int	umma_smbc_debug_cmdScanStart(void *szArgs)
{
	UMMA_SMBC_t	*pstSmbc;

	pstSmbc = umma_smbc_getInstance();
	umma_smbc_scanStart( pstSmbc );
	umma_smbc_releaseInstance(pstSmbc);

	return 0;
}
static void		umma_smbc_debug_InitCommandTools(void)
{

	HLIB_CMD_RegisterWord("smbc", umma_smbc_debug_cmdScanStart,
						"scanStart",
						"scan start by smbtree",
						"scanStart");
}

