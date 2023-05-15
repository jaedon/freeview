/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_smbs.c
	@brief

	Description:  									\n
	Module: UMMA Samba Server				\n

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
#include <uapi.h>
#include <napi.h>
#include <hapi.h>
#include <oapi.h>

#include <umma_int.h>
#include <umma_fm.h>
#include "umma_smbs.h"

#include <sys/mount.h>
#include "umma_fs_int.h"
#include "se_uapi.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define SMBS_CMD_SMBD			"/usr/bin/smbd"			// samba daemon
#define SMBS_CMD_NMBD			"/usr/bin/nmbd"			// net-bios daemon

#define SMBS_LOCALSTATE_PATH	"/tmp/samba"
#define SMBS_RW_LINK_PATH		"/var/lib/humaxtv_user/samba"
#define SMBS_CONF_PATH		"/etc/samba/smb.conf"
#define SMBS_PRIVATE_PATH		"/etc/samba/private"
#define SMBS_PASSWD_PATH		"/etc/samba/smbpasswd"
#define SMBS_USERS_PATH		"/etc/samba/smbusers"
#define SMBS_SHARECONF_PATH		"/etc/samba/smbshared"
#define SMBS_SHARECONF_BACKUP_PATH		"/etc/samba/smbshared.backup"

#define SMBS_HOME_PATH		"/media/smbs"

#define UMMA_SMBS_WORKGROUP_LEN		64
#define UMMA_SMBS_SRVNAME_LEN		64
#define UMMA_SMBS_SVRDESC_LEN		128
#define UMMA_SMBS_SHARED_MAX		8

/* Default value */
#ifdef CONFIG_PRODUCT_NAME
#define SMBS_DEFAULT_NAME CONFIG_PRODUCT_NAME
#else
#define SMBS_DEFAULT_NAME "Humax_Media"
#endif
#define SAMBA_SERVER_NAME_BY_MODEL

#define SMBS_DEFAULT_DESC "Samba Server"
#define SMBS_DEFAULT_WORKGROUP "WORKGROUP"
#define SMBS_ROOT_USER	"admin"

#define SMBS_MOUNT_PATH_LEN		128

#if defined(CONFIG_PROD_HGS1000S)
#define SMBS_SHARED_MEDIA		"/Media"
#define SMBS_SHARED_VIDEO		"/Video"
#define SMBS_SHARED_MUSIC		"/Music"
#define SMBS_SHARED_PHOTO		"/Photo"
#define SMBS_SHARED_DOWNLOAD	"/Download"
#define SMBS_SHARED_RECORDING	"/Recordings"

#else
#define SMBS_SHARED_PATHS "/mnt/hd2/Media/Video;/mnt/hd2/Media/Music;/mnt/hd2/Media/Photo;/mnt/hd2/Media/Download"

#define SMBS_SHARED_MEDIA		"/mnt/hd2/Media"
#define SMBS_SHARED_VIDEO		"/mnt/hd2/Media/Video"
#define SMBS_SHARED_MUSIC		"/mnt/hd2/Media/Music"
#define SMBS_SHARED_PHOTO		"/mnt/hd2/Media/Photo"
#define SMBS_SHARED_DOWNLOAD	"/mnt/hd2/Media/Download"
#define SMBS_SHARED_RECORDING	"/mnt/hd2/Recordings"
#endif

#if defined(CONFIG_ENHANCED_SECURITY)
#define SMBS_SYSTEM_USER_NAME		"umma"
#define SMBS_SYSTEM_USER_VALUE		"1004"
#else
#define SMBS_SYSTEM_USER_NAME		"settop"
#define SMBS_SYSTEM_USER_VALUE		"1001"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 0
typedef struct
{
	HCHAR	szWorkgroup[UMMA_SMBS_WORKGROUP_LEN];
	HCHAR	szServerName[UMMA_SMBS_SRVNAME_LEN];
	HCHAR	szServerDesc[UMMA_SMBS_SVRDESC_LEN];

} UMMA_SMBS_CONFIG_t;
#else
typedef UAPI_SMBS_CONF_t UMMA_SMBS_CONFIG_t;
#endif

typedef struct
{
	HCHAR szName[64];
	HCHAR szHomePath[128];
	HxList_t *pstSharedList;
	HxList_t *pstBindList; //target path for mount. ex) /media/smbs/admin/inter
	HBOOL bPublic;
} UMMA_SMBS_USER_t;


typedef struct
{
	HINT32	nServerHandle;
	HINT32	nRpcHandle;
	HULONG 	ulMsgQueue;

	HUINT32 ulSemId;
	HUINT32 ulMainTaskId;
	HUINT32 ulScanTaskId;
	HUINT32 ulEvtTaskId;

	HxList_t *pstUserList;
	UMMA_SMBS_CONFIG_t stConfig;
	HBOOL bInit;
	HBOOL bIsStarted;
	HBOOL bPublic;
	HBOOL bIntHddMounted;
	HINT32 nRetryCnt;
	HCHAR szNetbiosName[32];
} UMMA_SMBS_t;

typedef enum
{
	eUMMA_SMBS_CMD_SERVER_START = 0,
	eUMMA_SMBS_CMD_SERVER_STOP,
	eUMMA_SMBS_CMD_SET_CONFIG,
	eUMMA_SMBS_CMD_GET_SERVERINFO,
	eUMMA_SMBS_CMD_SET_PASSWD,
	eUMMA_SMBS_CMD_CHK_PASSWD,
	eUMMA_SMBS_CMD_SET_PERMISSION,
	eUMMA_SMBS_CMD_GET_SHAREDLIST,
	eUMMA_SMBS_CMD_SET_PUBLICACCESS,
	eUMMA_SMBS_CMD_RETRY_MOUNT,

	eUMMA_SMBS_CMD_MAX
} UMMA_SMBS_CMD_e;

typedef UMMA_MSG_t UMMA_SMBS_MSG_t;
typedef UMMA_MSG_t UMMA_SMBS_DATA_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static UMMA_SMBS_t	*s_pstUmmaSmbs = NULL;
#if defined(CONFIG_PROD_HGS1000S)
static HCHAR s_szsmbsMountPath[SMBS_MOUNT_PATH_LEN] = "/mnt/hd2";
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static void umma_smbs_sendMsgToMainTask( UMMA_SMBS_t *pstSmbs, UMMA_SMBS_CMD_e eCmd, void *data, HINT32 nReqId );
static HERROR	umma_smbs_stop_server( UMMA_SMBS_t *pstSmbs );
static HERROR	umma_smbs_restart_server( UMMA_SMBS_t *pstSmbs );
static HERROR	umma_smbs_get_config( UMMA_SMBS_CONFIG_t *pstConfig );
static void		umma_smbs_mainLoop(void *arg);

#if defined(CONFIG_PROD_HGS1000S)
static HERROR umma_smbs_set_smbSharedConfig(void);
static HERROR umma_smbs_seek_hdd(HBOOL *pbHddMounted);
static HERROR	umma_smbs_update_sharedListByLine( UMMA_SMBS_USER_t *pstUser, HCHAR *pszline);
static HBOOL umma_smbs_update_userSharedList( void *pvUser, void *pvNotUse );

extern HERROR	UMMA_FS_GetStorageInfo(HUINT32 ulDeviceId, DxStorage_Info_t* pstStorageInfo);
#endif

#if defined(CONFIG_DEBUG)
static void		umma_smbs_debug_InitCommandTools(void);
#endif
#define __________Internal_Function__________

#if defined(CONFIG_PROD_HGS1000S)
static HERROR	umma_smbs_update_sharedListByLine( UMMA_SMBS_USER_t *pstUser, HCHAR *pszline)
{
	HCHAR szAccess[16] = { 0 };
	HCHAR *pPath = NULL, *pAccess = NULL, *pFoldeName = NULL;
	HCHAR *pTmpStr = NULL, *pTmpEnd = NULL, *pTmpEnd2 = NULL;
	HCHAR *pszAttrPath = "<share path=\"";
	HCHAR *pszAttrAccess = "\" access=\"";
	HCHAR *pszAttrEnd = "\"/>";

	HINT32 attrPathLen = HxSTD_StrLen(pszAttrPath);
	HINT32 attrAccessLen = HxSTD_StrLen(pszAttrAccess);

	UAPI_SMBS_SHARED_t *pShared = NULL;

	if ( NULL == pstUser )
		return ERR_FAIL;

	pTmpStr = HxSTD_StrStr( pszline, pszAttrPath );
	if ( NULL == pTmpStr )
		return ERR_FAIL;

	pTmpEnd = HxSTD_StrStr( pTmpStr, pszAttrAccess );
	if ( NULL == pTmpEnd )
		return ERR_FAIL;

	pShared	= (UAPI_SMBS_SHARED_t *)UMMA_Calloc(sizeof(UAPI_SMBS_SHARED_t));

	/************sample****************************************
	<share path="/media/drive1/Media/Video" access="ro"/>
	***********************************************************/

	pPath = pTmpStr + attrPathLen;
	HxSTD_MemCopy( pShared->szPath, pPath, pTmpEnd - pPath  );

	pAccess = pTmpEnd + attrAccessLen;
	pTmpEnd2 =  HxSTD_StrStr( pAccess, pszAttrEnd );
	HxSTD_MemCopy( szAccess, pAccess, pTmpEnd2 - pAccess  );

	HLIB_PATH_Normalize(pShared->szPath);
	pFoldeName = HxSTD_StrRChar(pShared->szPath, '/' ) + 1;
	if(pFoldeName)
	{
		HxSTD_StrNCpy( pShared->szName, pFoldeName, sizeof(pShared->szName)-1 );
	}
	else
	{
		HxLOG_Error("pFoldeName NULL.... pShared->szPath[%s] ...\n",pShared->szPath);
	}

	if ( !HxSTD_StrCmp(szAccess, "rw") )
		pShared->bWritable = TRUE;
	else
		pShared->bWritable = FALSE;

	pstUser->pstSharedList = HLIB_LIST_Append( pstUser->pstSharedList, (void *)pShared );

	return ERR_OK;
}

/** Description: Update structure user->sharedlist (before setting config)
* * \param[in]	 pvUser			user instance
* * \return      TRUE if update successfuly or FALSE if update failure
*/
static HBOOL umma_smbs_update_userSharedList( void *pvUser, void *pvNotUse )
{
	UMMA_SMBS_USER_t *pUser = (UMMA_SMBS_USER_t *)pvUser;

	HCHAR szline[512] = { 0 };
	HCHAR szPublic[16] = { 0 };
	FILE *fp = NULL;
	HCHAR *pTmpStr = NULL, *pTmpEnd = NULL, *pTmpEnd2 = NULL;
	HCHAR *pUserName = NULL;
	HCHAR *pPublic = NULL;
	HCHAR *pszAttrUser = "<user name=\"";
	HCHAR *pszAttrPulbic = "\" public=\"";
	HCHAR *pszAttrEnd = "\">";

	HINT32 attrUserLen = HxSTD_StrLen(pszAttrUser);
	HINT32 attrPublicLen = HxSTD_StrLen(pszAttrPulbic);

	if(NULL == pUser)
		return FALSE;

	fp = fopen( SMBS_SHARECONF_PATH, "r" );
	if (NULL == fp )
		return FALSE;

	//Delete shared list of user
	pUser->pstSharedList = HLIB_LIST_RemoveAll( pUser->pstSharedList);

	/********** cat /etc/samba/smb.shared  ******************
	<user name="admin" public="no">
	 <share path="/media/drive1/Media/Video" access="ro"/>
	 <share path="/media/drive1/Media/Music" access="ro"/>
	 <share path="/media/drive1/Media/Photo" access="ro"/>
	 <share path="/media/drive1/Media/Download" access="rw"/>
	</user>
	*************************************************************/
	while ( fgets(szline, sizeof(szline)-1, fp) )
	{
		pTmpStr = pTmpEnd = pTmpEnd2 = NULL;
		pTmpStr = HxSTD_StrStr( szline, pszAttrUser );
		if ( NULL == pTmpStr )
		{
			//Create new shared list by each line
			umma_smbs_update_sharedListByLine( pUser, szline);
			continue;
		}

		pTmpEnd = HxSTD_StrStr( pTmpStr, pszAttrPulbic );
		if ( NULL == pTmpEnd )
			continue;

		pUserName =  pTmpStr + attrUserLen;
		pPublic = pTmpEnd + attrPublicLen;
		pTmpEnd2 = HxSTD_StrStr( pPublic, pszAttrEnd );
		if ( NULL == pTmpEnd2 )
			continue;

		if (0 != HxSTD_StrNCmp(pUser->szName, pUserName, pTmpEnd - pUserName))
		{
			HxLOG_Warning("[UMMA_SMBS] Cannot update Shared List due to diffent username!!\n");
			fclose( fp );
			fp = NULL;
			return FALSE;
		}
		HxSTD_MemCopy( szPublic, pPublic, (pTmpEnd2 - pPublic) );
		if ( !HxSTD_StrCmp(szPublic, "yes") )
			pUser->bPublic = TRUE;
		else
			pUser->bPublic = FALSE;

	}

	fclose( fp );
	fp = NULL;
	return TRUE;

}

/** Description: Find any hdd attached to STB (INTHDD, DETACHABLE, ESATAHDD, USBHDD)
* * \param[out]	 pbHddMounted			boolean flag (hdd mounted or not)
* * \return      OK, or other Error on failure
*/
static HERROR umma_smbs_seek_hdd(HBOOL *pbHddMounted)
{
	HUINT32		i, j;
	DxStorage_Info_t	stStorageInfo;

	if (pbHddMounted == NULL)
		return ERR_FAIL;

	HxSTD_MemSet( &stStorageInfo, 0x00, sizeof(DxStorage_Info_t) );
	*pbHddMounted = FALSE;

	for (i = 0; i < eFS_Dev_MAX; i++)
	{
		UMMA_FS_GetStorageInfo(i, &stStorageInfo);

		// FTP Server use only HDD, not use USB
		if (stStorageInfo.eDevType >= eDxSTORAGE_DEVTYPE_INTHDD && stStorageInfo.eDevType <= eDxSTORAGE_DEVTYPE_ESATAHDD)
		{
			if(stStorageInfo.nPartitionNum > 0)
			{
				for (j = 0; j < stStorageInfo.nPartitionNum; j++)
				{
					if ((stStorageInfo.astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_MOUNTED) &&
						(stStorageInfo.astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) )
					{
						*pbHddMounted = TRUE;
						// Update FTP PATH
						HxSTD_MemSet(s_szsmbsMountPath, 0, sizeof(s_szsmbsMountPath));
						HxSTD_StrNCpy(s_szsmbsMountPath, stStorageInfo.szMountPath, sizeof(s_szsmbsMountPath)-1);
						return ERR_OK;
					}
				}
			}
		}
	}
	HxLOG_Warning("[UMMA_SMBS] Cannot find any hdd!!\n");
	return ERR_FAIL;
}

/** Description: Update new hdd-path for shared config (/etc/samba/smb.shared)
* * \param[]	 void
* * \return      OK, or other Error on failure
*/
static HERROR umma_smbs_set_smbSharedConfig(void)
{
	FILE *fp = NULL;
	HCHAR szsmbsName[SMBS_MOUNT_PATH_LEN];

	fp = fopen( SMBS_SHARECONF_PATH, "w" );
	if (NULL == fp)
	{
		return ERR_FAIL;
	}
	fprintf( fp, "<user name=\"%s\" public=\"%s\">\n", SMBS_ROOT_USER, "yes" );
	HxSTD_snprintf (szsmbsName, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_VIDEO);
	fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szsmbsName );
	HxSTD_snprintf (szsmbsName, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_MUSIC);
	fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szsmbsName );
	HxSTD_snprintf (szsmbsName, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_PHOTO);
	fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szsmbsName );
	HxSTD_snprintf (szsmbsName, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_DOWNLOAD);
	fprintf( fp, " <share path=\"%s\" access=\"rw\"/>\n", szsmbsName );
	fprintf( fp, "</user>\n");

	fclose(fp);
	fp = NULL;
	return ERR_OK;
}
#endif

STATIC void umma_smbs_notifyEventToClient( UMMA_SMBS_t *pstSmbs, UAPI_SMBS_EVT_e eEvent, void *pvData, int nReqId)
{
	UAPI_SMBS_INFO_t stInfo;
	UMMA_SMBS_DATA_t *pNotiData;
	HINT32	temp = 0;
	HINT32	nRetErr = ERR_FAIL;
	void	*pvResult = &temp;
	HINT32	nResultSize = sizeof(HINT32);
	UAPI_SMBS_SHARED_t tmpShared;
	HxLOG_Debug("[UMMA_SMBS] NOTIFY EVENT : %d \n", eEvent);

	if ( NULL == pvData )
		return ;

	pNotiData = (UMMA_SMBS_DATA_t *)pvData;
	nRetErr = pNotiData->nParam1;

	switch ( eEvent )
	{
		case eUAPI_SMBS_EVT_SERVER_START :
		case eUAPI_SMBS_EVT_SERVER_STOP :
		case eUAPI_SMBS_EVT_CONFIG_CHANGE :
		case eUAPI_SMBS_EVT_PASSWD_CHANGE :
		case eUAPI_SMBS_EVT_PERMISSION_CHANGE :
		case eUAPI_SMBS_EVT_RESPONE_SERVERINFO :
			break;
		case eUAPI_SMBS_EVT_RESPONE_SHAREDLIST :
			{
				HINT32 nCnt = 0;
				HINT32 listSize = 0;
				UAPI_SMBS_SHARED_t *pSharedList = NULL;
				nCnt = pNotiData->nParam2;

				listSize = nCnt ? sizeof(UAPI_SMBS_SHARED_t) * nCnt : sizeof(HINT32);
				pSharedList = nCnt ? (UAPI_SMBS_SHARED_t*)pNotiData->nParam3 : &tmpShared;

				pvResult = (void  *)pSharedList;
				nResultSize = listSize;
			}
			break;
		case eUAPI_SMBS_EVT_RESPONE_CHKPASSWD :
			break;
		case eUAPI_SMBS_EVT_STATUS_CHANGE :
			break;
		default :
			return ;
	}

	HxSTD_MemSet( &stInfo, 0x00, sizeof(UAPI_SMBS_INFO_t));
	stInfo.bStarted = pstSmbs->bIsStarted;
	HxSTD_MemCopy( &stInfo.stConf, &pstSmbs->stConfig, sizeof(UAPI_SMBS_CONF_t) );

	HLIB_RPC_NotifyWithRequestId(pstSmbs->nRpcHandle, RPC_UMMA_SMBS_onRequestResult, nReqId,
			"iibb", eEvent, nRetErr, &stInfo, sizeof(UAPI_SMBS_INFO_t), pvResult, nResultSize);
}

static UMMA_SMBS_t *	umma_smbs_getInstance(void)
{
	if (NULL == s_pstUmmaSmbs)
	{
		HULONG tid = 0;
		HERROR hErr;

		s_pstUmmaSmbs = (UMMA_SMBS_t *)UMMA_Calloc(sizeof(UMMA_SMBS_t));

		HxSEMT_Create(&s_pstUmmaSmbs->ulSemId, "ummaSmbsSem", 0);

		HxSEMT_Get(s_pstUmmaSmbs->ulSemId);
		hErr = UMMA_RpcProtocol_GetRpcHandle(&s_pstUmmaSmbs->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error!! (Get RPC Handle)\n");
			HxLOG_Assert(s_pstUmmaSmbs->nRpcHandle);
		}

		HLIB_MSG_Create( 128, sizeof(UMMA_EVT_t), "ummaSmbsEvt",  &s_pstUmmaSmbs->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO );

		VK_TASK_Create( umma_smbs_mainLoop, 70, SIZE_64K, "ummaSmbsMainHandler", (void *)s_pstUmmaSmbs, &tid, FALSE );
		VK_TASK_Start(tid);

		s_pstUmmaSmbs->ulMainTaskId = tid;

	}
	else
	{
		HxSEMT_Get(s_pstUmmaSmbs->ulSemId);
	}

	return s_pstUmmaSmbs;
}

static void		umma_smbs_releaseInstance(UMMA_SMBS_t *pstSmbs)
{
	HxSEMT_Release(pstSmbs->ulSemId);
}

static inline HERROR 	umma_smbs_get_bindPath( const HCHAR *pszUser, const HCHAR *pszPath, HCHAR *pszBindPath, HINT32 nBufSize,HBOOL *pbRecording)
{
#if defined (CONFIG_PROD_HGS1000S)
	HCHAR szsmbsSharedMedia[SMBS_MOUNT_PATH_LEN];
	UMMA_SMBS_t *pstSmbs = NULL;

	pstSmbs = umma_smbs_getInstance();
	if (!pstSmbs->bIntHddMounted)
	{
		umma_smbs_releaseInstance(pstSmbs);
		return ERR_FAIL;
	}
	umma_smbs_releaseInstance(pstSmbs);

	HxSTD_snprintf (szsmbsSharedMedia, SMBS_MOUNT_PATH_LEN, "%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA);
	if( HxSTD_StrStr( pszPath, szsmbsSharedMedia ) )
#else
	if( HxSTD_StrStr( pszPath, SMBS_SHARED_MEDIA ) )
#endif
	{
		HxSTD_snprintf( pszBindPath, nBufSize, "%s/%s/%s",
						SMBS_HOME_PATH, pszUser, "inter" );
	}
	*pbRecording = FALSE;
	if( HxSTD_StrStr( pszPath, SMBS_SHARED_RECORDING ) )
	{
		HxSTD_snprintf( pszBindPath, nBufSize, "%s/%s/%s", SMBS_HOME_PATH, pszUser, "int_recording" );
		*pbRecording = TRUE;
	}
	/* to do : Dettachable HDD ??? */

	return ERR_OK;
}

static inline HERROR 	umma_smbs_get_storagePath( const HCHAR *pszSharePath, HCHAR *pszStorPath, HINT32 nBufSize  )
{
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szsmbsSharedMedia[SMBS_MOUNT_PATH_LEN];
	UMMA_SMBS_t *pstSmbs = NULL;

	pstSmbs = umma_smbs_getInstance();
	if (!pstSmbs->bIntHddMounted)
	{
		umma_smbs_releaseInstance(pstSmbs);
		return ERR_FAIL;
	}
	umma_smbs_releaseInstance(pstSmbs);

	HxSTD_snprintf (szsmbsSharedMedia, SMBS_MOUNT_PATH_LEN, "%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA);
	if( HxSTD_StrStr( pszSharePath, szsmbsSharedMedia ) )
	{
		HxSTD_StrNCpy( pszStorPath, szsmbsSharedMedia, nBufSize -1 );
	}
#else
	if( HxSTD_StrStr( pszSharePath, SMBS_SHARED_MEDIA ) )
	{
		HxSTD_StrNCpy( pszStorPath, SMBS_SHARED_MEDIA, nBufSize -1 );
	}
#endif
	if( HxSTD_StrStr(pszSharePath, SMBS_SHARED_RECORDING) )
	{
		HxSTD_StrNCpy( pszStorPath, SMBS_SHARED_RECORDING, nBufSize -1 );
	}
	/* to do : Dettachable HDD ??? */

	return ERR_OK;
}

static HERROR	umma_smbs_check_configPath(void)
{
	if ( TRUE != HLIB_DIR_IsExist(SMBS_RW_LINK_PATH) )
	{
		HxLOG_Print( "[UMMA_SMBS] directory is created to Read/Write for SAMBA config.(%s)\n", SMBS_RW_LINK_PATH );
		if ( ERR_OK !=  HLIB_DIR_Make( SMBS_RW_LINK_PATH ))
		{
			HxLOG_Error( "[UMMA_SMBS] mkdir Failed : %s\n", SMBS_RW_LINK_PATH );
			return ERR_FAIL;
		}
	}

	if ( TRUE != HLIB_DIR_IsExist(SMBS_PRIVATE_PATH) )
	{
		HxLOG_Print( "[UMMA_SMBS] directory is created for samba private.(%s)\n", SMBS_PRIVATE_PATH );
		if ( ERR_OK != HLIB_DIR_Make( SMBS_PRIVATE_PATH ))
		{
			HxLOG_Error( "[UMMA_SMBS] mkdir Failed : %s\n", SMBS_PRIVATE_PATH );
			return ERR_FAIL;
		}
	}

	if ( TRUE != HLIB_DIR_IsExist(SMBS_LOCALSTATE_PATH) )
	{
		HxLOG_Print( "[UMMA_SMBS] directory is created to Read/Write for SAMBA state.(%s)\n", SMBS_LOCALSTATE_PATH );
		if ( ERR_OK !=  HLIB_DIR_Make( SMBS_LOCALSTATE_PATH ))
		{
			HxLOG_Error( "[UMMA_SMBS] mkdir Failed : %s\n", SMBS_LOCALSTATE_PATH );
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}

static HERROR	umma_smbs_check_configFile(void)
{
	FILE *fp = NULL;
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szsmbsPath[SMBS_MOUNT_PATH_LEN];
#endif
	if ( TRUE != HLIB_DIR_IsExist(SMBS_PASSWD_PATH) )
	{
		fp = fopen( SMBS_PASSWD_PATH, "w" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}
		fprintf( fp, "%s:%s:41163B31FF9B73BBAAD3B435B51404EE:EA7C23F2EFCA850E3FB547CA12567C33:[U          ]:LCT-52A5D670:\n", SMBS_SYSTEM_USER_NAME, SMBS_SYSTEM_USER_VALUE );

		if ( fp )
			fclose(fp);
	}

	if ( TRUE != HLIB_DIR_IsExist(SMBS_USERS_PATH) )
	{
		fp = fopen( SMBS_USERS_PATH, "w" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}
		fprintf( fp, "%s = %s \n", SMBS_SYSTEM_USER_NAME, SMBS_ROOT_USER );

		if ( fp )
			fclose(fp);
	}
#ifdef CONFIG_SUPPORT_UMMA_SAMBA_SHARE_RECORDINGS
	if ( TRUE == HLIB_DIR_IsExist(SMBS_SHARECONF_PATH) )
	{
		HCHAR line[512] = { 0 };
		HBOOL findRec= FALSE;
		fp = fopen( SMBS_SHARECONF_PATH, "r" );
		if ( NULL != fp )
		{
			while ( fgets(line, sizeof(line)-1, fp) )
			{
				if (HxSTD_StrStr( line, SMBS_SHARED_RECORDING  ))
				{
					findRec = TRUE;
					break;
				}
			}
			fclose(fp);
			fp = NULL;
		}
		if ( TRUE != findRec )
		{
			HLIB_FILE_Delete(SMBS_SHARECONF_PATH);
		}
	}
#endif
	if ( TRUE != HLIB_DIR_IsExist(SMBS_SHARECONF_PATH) )
	{
		fp = fopen( SMBS_SHARECONF_PATH, "w" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}
		fprintf( fp, "<user name=\"%s\" public=\"%s\">\n", SMBS_ROOT_USER, "yes" );
#if defined(CONFIG_PROD_HGS1000S)
		HxSTD_snprintf (szsmbsPath, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_VIDEO);
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szsmbsPath );
		HxSTD_snprintf (szsmbsPath, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_MUSIC);
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szsmbsPath );
		HxSTD_snprintf (szsmbsPath, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_PHOTO);
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szsmbsPath );
		HxSTD_snprintf (szsmbsPath, SMBS_MOUNT_PATH_LEN, "%s%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA, SMBS_SHARED_DOWNLOAD);
		fprintf( fp, " <share path=\"%s\" access=\"rw\"/>\n", szsmbsPath );
#else
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", SMBS_SHARED_VIDEO );
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", SMBS_SHARED_MUSIC );
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", SMBS_SHARED_PHOTO );
		fprintf( fp, " <share path=\"%s\" access=\"rw\"/>\n", SMBS_SHARED_DOWNLOAD );
#ifdef CONFIG_SUPPORT_UMMA_SAMBA_SHARE_RECORDINGS
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", SMBS_SHARED_RECORDING );
#endif
#endif
		fprintf( fp, "</user>\n");

		fclose(fp);
		fp = NULL;
	}

	return ERR_OK;
}

static HERROR	umma_smbs_init_config( UMMA_SMBS_CONFIG_t *pstConfig )
{
	HxSTD_MemSet( pstConfig, 0x00, sizeof(UMMA_SMBS_CONFIG_t) );

	if ( ERR_OK != umma_smbs_get_config( pstConfig ) )
	{
		/* set default value */
		HxLOG_Debug( "[UMMA_SMBS] set default value for server config.\n" );
		HxSTD_StrNCpy( pstConfig->szWorkgroup, SMBS_DEFAULT_WORKGROUP , sizeof(pstConfig->szWorkgroup)-1 );
		HxSTD_StrNCpy( pstConfig->szServerName, SMBS_DEFAULT_NAME , sizeof(pstConfig->szServerName)-1 );
		HLIB_STD_StrUpper(pstConfig->szServerName);
		HxSTD_StrNCpy( pstConfig->szServerDesc, SMBS_DEFAULT_DESC , sizeof(pstConfig->szServerDesc)-1 );
	}

	return ERR_OK;
}

static HERROR	umma_smbs_init_sharedListOfUser( UMMA_SMBS_USER_t *pstUser, HCHAR *line, HINT32 bufSize )
{
	HCHAR szAccess[16] = { 0 };
	HCHAR *pPath = NULL, *pAccess = NULL, *pFoldeName = NULL;
	HCHAR *pTmpStr = NULL, *pTmpEnd = NULL, *pTmpEnd2 = NULL;
	HCHAR *pszAttrPath = "<share path=\"";
	HCHAR *pszAttrAccess = "\" access=\"";
	HCHAR *pszAttrEnd = "\"/>";

	HINT32 attrPathLen = HxSTD_StrLen(pszAttrPath);
	HINT32 attrAccessLen = HxSTD_StrLen(pszAttrAccess);
	//HINT32 attrEndLen = HxSTD_StrLen(pszAttrEnd);

	UAPI_SMBS_SHARED_t *pShared = NULL;

	if ( NULL == pstUser )
		return ERR_FAIL;

	pTmpStr = HxSTD_StrStr( line, pszAttrPath );
	if ( NULL == pTmpStr )
		return ERR_FAIL;

	pTmpEnd = HxSTD_StrStr( pTmpStr, pszAttrAccess );
	if ( NULL == pTmpEnd )
		return ERR_FAIL;

	pShared	= (UAPI_SMBS_SHARED_t *)UMMA_Calloc(sizeof(UAPI_SMBS_SHARED_t));

	pPath = pTmpStr + attrPathLen;
	HxSTD_MemCopy( pShared->szPath, pPath, pTmpEnd - pPath  );

	pAccess = pTmpEnd + attrAccessLen;
	pTmpEnd2 =  HxSTD_StrStr( pAccess, pszAttrEnd );
	HxSTD_MemCopy( szAccess, pAccess, pTmpEnd2 - pAccess  );

	HLIB_PATH_Normalize(pShared->szPath);
	pFoldeName = HxSTD_StrRChar(pShared->szPath, '/' ) + 1;
	if (pFoldeName)
	{
		HxSTD_StrNCpy( pShared->szName, pFoldeName, sizeof(pShared->szName)-1 );
	}
	else
	{
		HxLOG_Error("pFoldeName NULL.... pShared->szPath[%s] ...\n",pShared->szPath);
	}

	if ( !HxSTD_StrCmp(szAccess, "rw") )
		pShared->bWritable = TRUE;
	else
		pShared->bWritable = FALSE;

	pstUser->pstSharedList = HLIB_LIST_Append( pstUser->pstSharedList, (void *)pShared );

	return ERR_OK;
}

static HERROR	umma_smbs_init_users( HxList_t **ppstUserList )
{
	UMMA_SMBS_USER_t *pUser = NULL;
	HCHAR line[512] = { 0 };
	HCHAR szPublic[16] = { 0 };
	FILE *fp = NULL;
	HCHAR *pTmpStr = NULL, *pTmpEnd = NULL, *pTmpEnd2 = NULL;
	HCHAR *pUserName = NULL;
	HCHAR *pPublic = NULL;
	HCHAR *pszAttrUser = "<user name=\"";
	HCHAR *pszAttrPulbic = "\" public=\"";
	HCHAR *pszAttrEnd = "\">";

	HINT32 attrUserLen = HxSTD_StrLen(pszAttrUser);
	HINT32 attrPublicLen = HxSTD_StrLen(pszAttrPulbic);
	//HINT32 attrEndLen = HxSTD_StrLen(pszAttrEnd);

	fp = fopen( SMBS_SHARECONF_PATH, "r" );
	if (NULL == fp )
		return ERR_FAIL;

	while ( fgets(line, sizeof(line)-1, fp) )
	{
		pTmpStr = pTmpEnd = pTmpEnd2 = NULL;
		pTmpStr = HxSTD_StrStr( line, pszAttrUser );
		if ( NULL == pTmpStr )
		{
			umma_smbs_init_sharedListOfUser( pUser, line, sizeof(line) );
			continue;
		}

		pTmpEnd = HxSTD_StrStr( pTmpStr, pszAttrPulbic );
		if ( NULL == pTmpEnd )
			continue;

		pUserName =  pTmpStr + attrUserLen;

		pPublic = pTmpEnd + attrPublicLen;
		pTmpEnd2 = HxSTD_StrStr( pPublic, pszAttrEnd );
		if ( NULL == pTmpEnd2 )
			continue;

		pUser = (UMMA_SMBS_USER_t *)UMMA_Calloc(sizeof(UMMA_SMBS_USER_t));
		HxSTD_MemCopy( pUser->szName, pUserName, (pTmpEnd - pUserName) );

		HxSTD_MemCopy( szPublic, pPublic, (pTmpEnd2 - pPublic) );
		if ( !HxSTD_StrCmp(szPublic, "yes") )
			pUser->bPublic = TRUE;
		else
			pUser->bPublic = FALSE;

		HxSTD_snprintf( pUser->szHomePath, sizeof(pUser->szHomePath), "%s/%s", SMBS_HOME_PATH, pUser->szName );

		*ppstUserList = HLIB_LIST_Append( *ppstUserList, (void *)pUser );
	}

	fclose( fp ) ;
	fp = NULL;

	return ERR_OK;
}

STATIC HBOOL umma_smbs_cb_mountBind( void *pvUser, void *pvNotUse )
{
	UMMA_SMBS_USER_t *pUser = (UMMA_SMBS_USER_t *)pvUser;
	UAPI_SMBS_SHARED_t *pShared = NULL;
	HxList_t *pSharedItem = NULL;
	HCHAR szBindPath[128] = { 0 };
	HCHAR szCompPath[128] = { 0 };
	HBOOL bRecording = FALSE;

	SEUAPI_DoCommand("mkdir -p", pUser->szHomePath);

	pSharedItem = pUser->pstSharedList;
	while ( pSharedItem )
	{
		bRecording = FALSE;
		pShared = (UAPI_SMBS_SHARED_t*)HLIB_LIST_Data(pSharedItem);

		HxSTD_MemSet( szBindPath, 0x00, sizeof(szBindPath) );
		umma_smbs_get_bindPath( pUser->szName, pShared->szPath, szBindPath, sizeof(szBindPath),&bRecording );

		if ( HxSTD_StrCmp( szBindPath, szCompPath ) && !HxSTD_StrEmpty(szBindPath) )
		{
			HCHAR *pBindPath = (HCHAR *)HLIB_STD_MemAlloc(sizeof(szBindPath));
			HCHAR szStorPath[128] = { 0 };
			HxSTD_StrNCpy( szCompPath, szBindPath, sizeof(szCompPath)-1 );
			HxSTD_StrNCpy( pBindPath, szBindPath, sizeof(szBindPath)-1 );
			//HLIB_DIR_Make( pBindPath );

			umma_smbs_get_storagePath( pShared->szPath, szStorPath, sizeof(szStorPath) );

			SEUAPI_MountPartition_Smbs(szStorPath, pBindPath);

			pUser->pstBindList = HLIB_LIST_Append( pUser->pstBindList, pBindPath );
			HxLOG_Print("[UMMA_SMBS] mount --bind  %s %s\n", szStorPath, pBindPath);
		}

		pSharedItem = HLIB_LIST_Next( pSharedItem );
	}
	return 0;
}

STATIC HBOOL umma_smbs_cb_unmountBind( void *pvBindPath, void *pvNotUse )
{
	HCHAR *pBindPath = (HCHAR *)pvBindPath;

	SEUAPI_DoCommand("umount -l", pBindPath);
	HxLOG_Print("[UMMA_SMBS] umount %s\n", pBindPath );
	return 0;
}

static void	umma_smbs_cb_freeBind( void *pvBindPath )
{
	HLIB_STD_MemFree(pvBindPath);
}

static HBOOL umma_smbs_cb_cmpareByUserName ( void *user_data, void *list_data )
{
	HCHAR *pszUserName = (HCHAR *)user_data;
	UMMA_SMBS_USER_t *pUser = (UMMA_SMBS_USER_t*)list_data;

	if ( !HxSTD_StrCmp( pszUserName, pUser->szName ))
		return TRUE;

	return FALSE;
}

static HBOOL umma_smbs_cb_cmpareBySharedPath ( void *user_data, void *list_data )
{
	HCHAR *pszPath = (HCHAR *)user_data;
	UAPI_SMBS_SHARED_t *pShared = (UAPI_SMBS_SHARED_t*)list_data;

	if ( !HxSTD_StrCmp( pszPath, pShared->szPath ))
		return TRUE;

	return FALSE;
}

static HBOOL umma_smbs_cb_writeShareConfig( void *pvUser, void *pvFp )
{
	UMMA_SMBS_USER_t *pstUser = (UMMA_SMBS_USER_t *)pvUser;
	FILE *pFp = (FILE *)pvFp;
	HxList_t *pSharedItem = NULL;
	UAPI_SMBS_SHARED_t *pShared = NULL;
	HCHAR szBindPath[128] = { 0 };
	HBOOL bRecording = FALSE;

	pSharedItem = pstUser->pstSharedList;
	while (pSharedItem)
	{
		bRecording = FALSE;
		pShared = (UAPI_SMBS_SHARED_t*)HLIB_LIST_Data(pSharedItem);
		HxSTD_MemSet( szBindPath, 0x00, sizeof(szBindPath) );
		umma_smbs_get_bindPath( pstUser->szName, pShared->szPath, szBindPath, sizeof(szBindPath),&bRecording  );

		if ( 0 == HxSTD_StrLen(szBindPath) )
		{
			pSharedItem = HLIB_LIST_Next(pSharedItem);
			continue;
		}

		fprintf( pFp, "[%s]\n", pShared->szName );
		fprintf( pFp, " commnet = %s %s Folder\n", pstUser->szName,  pShared->szName );
		if ( bRecording )
			fprintf( pFp, " path = %s\n", szBindPath );
		else
			fprintf( pFp, " path = %s/%s\n", szBindPath, pShared->szName );
		fprintf( pFp, " writable = %s\n", pShared->bWritable ? "yes" : "no" );
		fprintf( pFp, " browseable = %s\n", "yes" );
		if ( pstUser->bPublic )
		{
			fprintf( pFp, " public = %s\n", "yes" );
		}
		else
		{
			fprintf( pFp, " valid users = %s\n", SMBS_SYSTEM_USER_NAME );
		}
		pSharedItem = HLIB_LIST_Next(pSharedItem);
	}

	return FALSE;
}

static HERROR	umma_smbs_signal_onFormatOccurred(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HINT32 devId = 0;
	HCHAR *pMntPath = NULL;
	UMMA_SMBS_t *pstSmbs;
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szsmbsSharedMedia[SMBS_MOUNT_PATH_LEN];
#endif


	if ( 2 != nArgc )
	{
		HxLOG_Error("[UMMA_SMBS] %s() nArgc = %d/%d\n", __FUNCTION__, nArgc, 2 );
		return ERR_FAIL;
	}

	devId = HxOBJECT_INT(apArgv[0]);
	pMntPath = HxOBJECT_STR(apArgv[1]);

	HxLOG_Debug("[UMMA_SMBS] Received onFormatOccurred Event. devId : %d, pMntPath : %s\n", devId, pMntPath );
#if defined(CONFIG_PROD_HGS1000S)
	HxSTD_snprintf (szsmbsSharedMedia, SMBS_MOUNT_PATH_LEN, "%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA);
	if ( NULL == HxSTD_StrStr( szsmbsSharedMedia, pMntPath ))
#else
	if ( NULL == HxSTD_StrStr( SMBS_SHARED_MEDIA, pMntPath ))
#endif
	{
		return ERR_OK;
	}

	/* only Internal HDD  */
	pstSmbs = umma_smbs_getInstance();
	if ( pstSmbs->bIsStarted )
	{
		umma_smbs_stop_server( pstSmbs );
	}
	umma_smbs_releaseInstance(pstSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_signal_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	if ( pstSmbs->bIsStarted )
		umma_smbs_stop_server( pstSmbs );
	umma_smbs_releaseInstance(pstSmbs);

	SEUAPI_DoCommand("rm -rf", SMBS_RW_LINK_PATH);
	return ERR_OK;
}

static void umma_smbs_netconf_callback(HINT32 event, void* data)
{
	HUINT32 param1 = 0, param2 = 0, param3 = 0;

	UMMA_SMBS_t *pstSmbs;

	(void)param2;
	(void)param3;
	if(data)
		param1 = ((NAPINetConfInfo_t *)data)->id;

	switch( event )
	{
		case NAPI_EVENT_NETCONF_CONN_SUCCESS:
			pstSmbs = umma_smbs_getInstance();
			umma_smbs_restart_server( pstSmbs );
			umma_smbs_releaseInstance(pstSmbs);
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

static inline UMMA_SMBS_USER_t * umma_smbs_get_user ( HxList_t *pstUserList, const HCHAR *pszUserName )
{
	HxList_t *pUserItem = NULL;
	pUserItem = HLIB_LIST_FindEx( pstUserList, pszUserName, umma_smbs_cb_cmpareByUserName );
	if( NULL == pUserItem )
		return NULL;

	return  (UMMA_SMBS_USER_t *)HLIB_LIST_Data(pUserItem);
}

static HERROR	umma_smbs_set_config( UMMA_SMBS_t *pstSmbs, UMMA_SMBS_CONFIG_t *pstConfig )
{
	FILE *fp = NULL;
	UMMA_SMBS_USER_t *pUser = NULL;

	fp = fopen( SMBS_CONF_PATH, "w" );
	if (NULL == fp)
	{
		HxLOG_Error( "[UMMA_SMBS] file open failed : %s\n", SMBS_CONF_PATH );
		return ERR_FAIL;
	}
#if defined(SAMBA_SERVER_NAME_BY_MODEL)
	{
		/* Update Box Name FVP-5000T or FVP-4000T */
		HERROR hErr = ERR_OK;
		HCHAR szModelName[UMMA_SMBS_SRVNAME_LEN] = {0, };

		hErr=OAPI_SYSINFO_STB_GetModelName(szModelName,UMMA_SMBS_SRVNAME_LEN);
		HxSTD_snprintf( pstConfig->szServerName, UMMA_SMBS_SRVNAME_LEN ,"%s",szModelName);
		HLIB_STD_StrUpper(pstConfig->szServerName);
		
	}
#endif
	fprintf( fp, "[global]\n" );
	fprintf( fp, " workgroup = %s\n", pstConfig->szWorkgroup );
	fprintf( fp, " server string = %s\n", pstConfig->szServerDesc );
	{
		NAPINetConfInfo_t network_info;
		HINT32 devIdx = NAPI_NetConf_GetDefaultIndex();
		HCHAR *pHostIpOctet = NULL;

		HxSTD_MemSet(&network_info, 0x00, sizeof(NAPINetConfInfo_t));
		NAPI_NetConf_GetInfo(devIdx, &network_info);

		pHostIpOctet = HxSTD_StrRChar( network_info.ip_info.ipv4.ipAddr, '.' );
		if ( pHostIpOctet )
			pHostIpOctet = pHostIpOctet + 1;
		else
			pHostIpOctet = "none";

		fprintf( fp, " netbios name = %s:%s\n", pstConfig->szServerName, pHostIpOctet );
		HxSTD_snprintf( pstSmbs->szNetbiosName, sizeof(pstSmbs->szNetbiosName),
				"%s:%s", pstConfig->szServerName, pHostIpOctet );
#if 0
		if ( 0 != HxSTD_StrCmp( network_info.ip_info.ipv4.ipAddr, "0.0.0.0"))
		{
			HCHAR szBrdcastIp[16] = { 0 };
			HxSTD_StrNCpy( szBrdcastIp, network_info.ip_info.ipv4.ipAddr, sizeof(szBrdcastIp));

			pHostIpOctet = HxSTD_StrRChar( szBrdcastIp, '.' );
			if ( pHostIpOctet )
			{
				HxSTD_StrNCpy( pHostIpOctet, ".255", sizeof(szBrdcastIp) - (pHostIpOctet - szBrdcastIp) -1 );
				fprintf( fp, " remote announce = %s\n", szBrdcastIp );
			}
		}
#endif
	}

	pUser = umma_smbs_get_user( pstSmbs->pstUserList, SMBS_ROOT_USER );
	if (pUser)
	{
		fprintf( fp, " security = %s\n", pUser->bPublic ? "share" : "user" );

		fprintf( fp, " unix extensions = %s\n", "no" );

		//if not set guset account, samba use "nobody" account as default
		//but in LXC mode, nobody account is removed
		fprintf( fp, " guest account = %s\n", SMBS_SYSTEM_USER_NAME );
	}

	fprintf( fp, " usershare allow guests = Yes\n" );
	fprintf( fp, " log file = %s\n", "/var/log/samba/log.%m" );
	fprintf( fp, " max log size = %d\n", 32 );
	fprintf( fp, " smb passwd file = %s\n", SMBS_PASSWD_PATH );
	fprintf( fp, " username map = %s\n", SMBS_USERS_PATH );
	fprintf( fp, " socket options = %s\n", "TCP_NODELAY SO_RCVBUF=8192 SO_SNDBUF=8192");
	fprintf( fp, " domain master = %s\n", "yes");
	fprintf( fp, " name resolve order = bcast wins host lmhosts\n");
	fprintf( fp, " dns proxy = %s\n", "no" );

	/* share folder config */
	HLIB_LIST_ForeachEx( pstSmbs->pstUserList, umma_smbs_cb_writeShareConfig, (void *)fp );

	fclose(fp);

	// 2 pointer point to same struct -> no neeed to copy
	//HxSTD_MemCopy( &pstSmbs->stConfig, pstConfig, sizeof(UAPI_SMBS_CONF_t) );
	return ERR_OK;
}

static HERROR	umma_smbs_get_config( UMMA_SMBS_CONFIG_t *pstConfig )
{
	FILE *fp = NULL;
	HCHAR line[512] = { 0 };
	HCHAR *pSearch = NULL;
	HCHAR *cariChar = NULL;
	fp = fopen( SMBS_CONF_PATH, "r" );
	if (NULL == fp)
	{
		HxLOG_Error( "[UMMA_SMBS] file open failed : %s\n", SMBS_CONF_PATH );
		return ERR_FAIL;
	}

	while ( fgets(line, sizeof(line)-1, fp) )
	{
		if ( NULL != (pSearch = HxSTD_StrStr( line, "workgroup = " )) )
		{
			HxSTD_StrNCpy( pstConfig->szWorkgroup, pSearch + HxSTD_StrLen("workgroup = "), sizeof(pstConfig->szWorkgroup)-1 );
			cariChar = HxSTD_StrRChar( pstConfig->szWorkgroup, '\n' );
			if ( cariChar )
				*cariChar = '\0';
		}
		else if ( NULL != (pSearch = HxSTD_StrStr( line, "server string = " )) )
		{
			HxSTD_StrNCpy( pstConfig->szServerDesc, pSearch + HxSTD_StrLen("server string = "), sizeof(pstConfig->szServerDesc)-1 );
			cariChar = HxSTD_StrRChar( pstConfig->szServerDesc, '\n' );
			if ( cariChar )
				*cariChar = '\0';

		}
		else if ( NULL != (pSearch = HxSTD_StrStr( line, "netbios name = " )) )
		{
#if 0
			HxSTD_StrNCpy( pstConfig->szServerName, pSearch + HxSTD_StrLen("netbios name = "), sizeof(pstConfig->szServerName)-1 );
			cariChar = HxSTD_StrRChar( pstConfig->szServerName, '\n' );
			if ( cariChar )
				*cariChar = '\0';
#else
			HxSTD_StrNCpy( pstConfig->szServerName, SMBS_DEFAULT_NAME , sizeof(pstConfig->szServerName)-1 );
			HLIB_STD_StrUpper(pstConfig->szServerName);
#endif

			break;
		}
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("[%s,%d] work group  : %s\n", __FUNCTION__, __LINE__, pstConfig->szWorkgroup );
	HxLOG_Print("[%s,%d] Server Name : %s\n", __FUNCTION__, __LINE__, pstConfig->szServerName );
	HxLOG_Print("[%s,%d] Server Desc : %s\n", __FUNCTION__, __LINE__, pstConfig->szServerDesc );
#endif

	fclose(fp);

	return ERR_OK;
}

static HERROR	umma_smbs_start_server( UMMA_SMBS_t *pstSmbs )
{
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szsmbsSharedMedia[SMBS_MOUNT_PATH_LEN];
#endif

	if (pstSmbs->bIsStarted)
	{
		HxLOG_Warning("[UMMA_SMBS] already started!!\n");
		return ERR_OK;
	}

#if defined(CONFIG_PROD_HGS1000S)
	pstSmbs->bIntHddMounted = FALSE;

	HxSTD_snprintf(szsmbsSharedMedia, SMBS_MOUNT_PATH_LEN, "%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA);
	if ( 0 == access(szsmbsSharedMedia, F_OK) )
			pstSmbs->bIntHddMounted = TRUE;
	else
	{
		// Seek hard disk info
		if(umma_smbs_seek_hdd(&pstSmbs->bIntHddMounted) == ERR_FAIL)
			return ERR_FAIL;

		if(!pstSmbs->bIntHddMounted)
			return ERR_FAIL;

		// Modify /etc/samba/smb.shared
		if (umma_smbs_set_smbSharedConfig() == ERR_FAIL)
			return ERR_FAIL;

		// Update struct user->SharedList to update smb.conf
		HLIB_LIST_ForeachEx( pstSmbs->pstUserList, umma_smbs_update_userSharedList, NULL );
	}

	// Update smb.conf
	if(umma_smbs_set_config( pstSmbs, &pstSmbs->stConfig ) == ERR_FAIL)
		return ERR_FAIL;
#else
	umma_smbs_set_config( pstSmbs, &pstSmbs->stConfig );

	if ( 0 == access(SMBS_SHARED_MEDIA, F_OK) )
		pstSmbs->bIntHddMounted = TRUE;
	else
		pstSmbs->bIntHddMounted = FALSE;
#endif

	if ( pstSmbs->bIntHddMounted )
	{
		/* mount --bind */
		HLIB_LIST_ForeachEx( pstSmbs->pstUserList, umma_smbs_cb_mountBind, NULL );
	}
#if defined(CONFIG_PROD_HGS1000S)
	else
	{
		pstSmbs->nRetryCnt = 0;
		umma_smbs_sendMsgToMainTask( pstSmbs, eUMMA_SMBS_CMD_RETRY_MOUNT, NULL, 0 );
		return ERR_FAIL;
	}
#endif

	SEUAPI_Start_Smbs(SMBS_CMD_NMBD, SMBS_CONF_PATH);
	SEUAPI_Start_Smbs(SMBS_CMD_SMBD, SMBS_CONF_PATH);

	pstSmbs->bIsStarted = TRUE;
#if defined(CONFIG_PROD_HGS1000S)
#else
	if ( !pstSmbs->bIntHddMounted )
	{
		pstSmbs->nRetryCnt = 0;
		umma_smbs_sendMsgToMainTask( pstSmbs, eUMMA_SMBS_CMD_RETRY_MOUNT, NULL, 0 );
	}
#endif
	HxLOG_Print("[UMMA_SMBS] Server Start!!!!\n");
	return ERR_OK;
}

static HERROR	umma_smbs_stop_server( UMMA_SMBS_t *pstSmbs )
{
	UMMA_SMBS_USER_t *pUser = NULL;
	HxList_t *pUserItem = NULL;

	SEUAPI_DoCommand("killall", "nmbd");
	SEUAPI_DoCommand("killall", "smbd");

	pUserItem = pstSmbs->pstUserList;
	while ( pUserItem )
	{
		pUser = (UMMA_SMBS_USER_t *)HLIB_LIST_Data(pUserItem);
		/* umount bind path */
		HLIB_LIST_ForeachEx( pUser->pstBindList, umma_smbs_cb_unmountBind, NULL );
		HLIB_LIST_RemoveAllFunc( pUser->pstBindList, umma_smbs_cb_freeBind );
		pUser->pstBindList = NULL;

		pUserItem = HLIB_LIST_Next(pUserItem);
	}

	pstSmbs->bIsStarted = FALSE;

	HxLOG_Print("[UMMA_SMBS] Server Stop!!!!\n");
	return ERR_OK;
}

static HERROR	umma_smbs_restart_server( UMMA_SMBS_t *pstSmbs )
{
	if( !pstSmbs->bIsStarted )
		return ERR_FAIL;

	SEUAPI_DoCommand("killall", "nmbd");
	SEUAPI_DoCommand("killall", "smbd");

	VK_TASK_Sleep(1000);

	umma_smbs_set_config( pstSmbs, &pstSmbs->stConfig );

	SEUAPI_Start_Smbs(SMBS_CMD_NMBD, SMBS_CONF_PATH);
	SEUAPI_Start_Smbs(SMBS_CMD_SMBD, SMBS_CONF_PATH);

	HxLOG_Print("[UMMA_SMBS] Server Restart!!!!\n");
	return ERR_OK;
}

static HERROR	umma_smbs_isValid_passwd( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName, const HCHAR *pszPasswd, HINT32 *pbIsValid )
{
	UMMA_SMBS_USER_t *pUser = NULL;
	if ( 1 )
	{
		HxLOG_Error("[UMMA_SMBS] current not supported. %s()\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pUser = umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if ( NULL == pUser )
		return ERR_FAIL;

	return ERR_OK;
}


static HERROR	umma_smbs_add_user( const HCHAR *pszUserName, const HCHAR *pszPasswd )
{
	SEUAPI_DoCommand("smbpasswd -a -c", SMBS_CONF_PATH);
	return ERR_OK;
}

static HERROR	umma_smbs_remove_user( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName )
{
	UMMA_SMBS_USER_t *pUser = NULL;

	if ( 1 )
	{
		HxLOG_Error("[UMMA_SMBS] current not supported. %s()\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pUser = umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if ( NULL == pUser )
		return ERR_FAIL;

	pstSmbs->pstUserList = HLIB_LIST_Remove( pstSmbs->pstUserList, pUser );
	return ERR_OK;
}

static HERROR	umma_smbs_change_passwd( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName, const HCHAR *pszPasswd, const HCHAR *pszNewPasswd )
{
	HCHAR 	szCmd[256] = { 0 };
	UMMA_SMBS_USER_t *pUser = NULL;

	pUser =  umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if ( NULL == pUser )
		return ERR_FAIL;

	/* only settop user change*/
	HxSTD_PrintToStrN(szCmd, sizeof(szCmd), "%s -c %s -U \"%s\"", "/usr/bin/smbpasswd", SMBS_CONF_PATH, SMBS_SYSTEM_USER_NAME);

	SEUAPI_SetPassword_Smbs(szCmd, (HINT8 *)pszNewPasswd);

	HxLOG_Debug("\n[UMMA_SMBS] Change Passwd. user : %s\n", pszUserName);

	return ERR_OK;
}

static HERROR	umma_smbs_set_permission( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName,
									const HCHAR *pszPasswd, const UAPI_SMBS_SHARED_t *pstShared )
{
	UMMA_SMBS_USER_t *pUser = NULL;
	UAPI_SMBS_SHARED_t *pShared = NULL;
	FILE *fp = NULL;
	FILE *fp_backup = NULL;

	HCHAR line[512] = { 0 };
	HCHAR szCmd[256] = { 0 };
	HCHAR szSearch[256] = { 0 };
	HCHAR szUserSearch[256] = { 0 };
	HCHAR *pTmpStr = NULL, *pTmpName = NULL, *pTmpAccess = NULL;
	HCHAR *pszAttrUser = "<user name=\"";
	HCHAR *pszAttrShare = "<share path=\"";
	HCHAR *pszAttrAccess = "\" access=\"";
	//HCHAR *pszAttrEnd = "\"/>\n";

	HINT32 attrUserLen = HxSTD_StrLen(pszAttrUser);
	HBOOL bFindUser = FALSE;

	HxList_t *pSharedItem = NULL;

	pUser = umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}
	HxLOG_Debug("[UMMA_FTS] %s(), bWritabl : %d \n", __FUNCTION__, pstShared->bWritable );

	pSharedItem = HLIB_LIST_FindEx( pUser->pstSharedList, pstShared->szPath, umma_smbs_cb_cmpareBySharedPath );
	if ( NULL == pSharedItem )
	{
		HxLOG_Error("Not found SharedFolder : %s\n", pstShared->szPath );
		return ERR_FAIL;
	}
	pShared = (UAPI_SMBS_SHARED_t*)HLIB_LIST_Data(pSharedItem);

	if ( pShared->bWritable == pstShared->bWritable )
	{
		return ERR_OK;
	}
	pShared->bWritable = pstShared->bWritable;
	/*todo : permission change */
	HxLOG_Debug("[UMMA_FTS] %s(), permission change : %d \n", __FUNCTION__, pstShared->bWritable );

	/* backup shared file */
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s > %s",  SMBS_SHARECONF_PATH, SMBS_SHARECONF_BACKUP_PATH);
	VK_SYSTEM_Command(szCmd);

	fp = fopen( SMBS_SHARECONF_PATH, "w" );
	if (NULL == fp)
	{
		return ERR_FAIL;
	}
	fp_backup = fopen( SMBS_SHARECONF_BACKUP_PATH, "r" );
	if (NULL == fp_backup )
	{
		fclose(fp);
		return ERR_FAIL;
	}

	HxSTD_snprintf( szSearch, sizeof(szSearch), "%s%s%s", pszAttrShare, pstShared->szPath, pszAttrAccess );
	HxSTD_snprintf( szUserSearch, sizeof(szUserSearch), "%s%s", pszUserName, "\" public=\"" );

	while ( fgets(line, sizeof(line)-1, fp_backup) )
	{
		pTmpStr = HxSTD_StrStr( line, pszAttrUser );
		if (pTmpStr )
		{
			pTmpName = HxSTD_StrStr( pTmpStr + attrUserLen, szUserSearch );
			if ( pTmpName )
				bFindUser = TRUE;
			else
				bFindUser = FALSE;
		}

		if ( bFindUser && (pTmpAccess = HxSTD_StrStr( line, szSearch )))
		{
			fprintf( fp, " <share path=\"%s\" access=\"%s\"/>\n", pstShared->szPath,
				  pstShared->bWritable ? "rw" : "ro" );
		}
		else
		{
			fprintf( fp, "%s", line );
		}
	}

	fclose(fp);			fp = NULL;
	fclose(fp_backup);	fp_backup = NULL;

	/*Temp Code for HMS1000s PT2 */
	umma_smbs_restart_server( pstSmbs );
	return ERR_OK;
}

static HERROR	umma_smbs_get_sharedList( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName, HINT32 *pnCount, UAPI_SMBS_SHARED_t **ppstShared )
{
	UMMA_SMBS_USER_t *pUser = NULL;
	UAPI_SMBS_SHARED_t *pShared = NULL;
	HINT32 nSharedCnt = 0;
	HINT32 i = 0;

	HxList_t *pSharedItem = NULL;

	pUser = umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_OK;
	}

	nSharedCnt = HLIB_LIST_Length(pUser->pstSharedList);
	if ( nSharedCnt )
		*ppstShared = (UAPI_SMBS_SHARED_t *)HLIB_STD_MemAlloc(sizeof(UAPI_SMBS_SHARED_t)*nSharedCnt);
	else
		return ERR_OK;

	pSharedItem = pUser->pstSharedList;
	while (pSharedItem)
	{
		pShared = (UAPI_SMBS_SHARED_t*)HLIB_LIST_Data(pSharedItem);
		HxSTD_MemCopy( &(*ppstShared)[i], pShared, sizeof(UAPI_SMBS_SHARED_t) );
		pSharedItem = HLIB_LIST_Next(pSharedItem);
		i++;
	}

	*pnCount = nSharedCnt;
	HxLOG_Debug("[UMMA_SMBS] %s() nCount : %d\n", __FUNCTION__, *pnCount);
	return ERR_OK;
}

static HERROR	umma_smbs_set_publicAccess( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName, HBOOL bAccess )
{
	UMMA_SMBS_USER_t *pUser = NULL;
	HCHAR line[512] = { 0 };
	HCHAR szCmd[512] = { 0 };
	FILE *fp = NULL;
	FILE *fp_backup = NULL;
	HCHAR szSearch[256] = { 0 };
	HCHAR *pszAttrUser = "<user name=\"";
	HCHAR *pszAttrPulbic = "\" public=\"";
	HCHAR *pszAttrEnd = "\">";
	HCHAR *pTmpStr = NULL;
	pUser = umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}

	HxLOG_Debug("[UMMA_SMBS] %s() public %d -> %d \n", __FUNCTION__, pUser->bPublic, bAccess );
	if ( pUser->bPublic == bAccess )
		return ERR_OK;

	pUser->bPublic = bAccess;
	/*todo : ftpd.shard change */

	/* backup shared file */
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s > %s",  SMBS_SHARECONF_PATH, SMBS_SHARECONF_BACKUP_PATH);
	VK_SYSTEM_Command(szCmd);

	fp = fopen( SMBS_SHARECONF_PATH, "w" );
	if (NULL == fp)
	{
		return ERR_FAIL;
	}
	fp_backup = fopen( SMBS_SHARECONF_BACKUP_PATH, "r" );
	if (NULL == fp_backup )
	{
		fclose(fp);
		return ERR_FAIL;
	}

	HxSTD_snprintf( szSearch, sizeof(szSearch), "%s%s%s", pszAttrUser, pszUserName, pszAttrPulbic );

	while ( fgets(line, sizeof(line)-1, fp_backup) )
	{
		if ( (pTmpStr = HxSTD_StrStr( line, szSearch )) )
		{
			fprintf( fp, "%s%s%s%s%s\n", pszAttrUser, pszUserName, pszAttrPulbic,
					bAccess ? "yes":"no", pszAttrEnd);
		}
		else
		{
			fprintf( fp, "%s", line );
		}
	}

	fclose(fp);			fp = NULL;
	fclose(fp_backup);	fp_backup = NULL;

	/*Temp Code for HMS1000s PT2 */
	umma_smbs_restart_server( pstSmbs );
	return ERR_OK;
}

static HERROR	umma_smbs_get_publicAccess( UMMA_SMBS_t *pstSmbs, const HCHAR *pszUserName, HBOOL *pbAccess )
{
	UMMA_SMBS_USER_t *pUser = NULL;

	pUser = umma_smbs_get_user( pstSmbs->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}

	*pbAccess = pUser->bPublic;
	HxLOG_Debug("[UMMA_SMBS] %s() public %d \n", __FUNCTION__, pUser->bPublic );

	return ERR_OK;
}

static void umma_smbs_sendMsgToMainTask( UMMA_SMBS_t *pstSmbs, UMMA_SMBS_CMD_e eCmd, void *data, HINT32 nReqId )
{
	UMMA_SMBS_MSG_t	stMsg = { 0 };
	stMsg.nMessage = (HINT32)eCmd;
	stMsg.nParam1 = (HINT32)data;
	stMsg.nParam2 = (HINT32)nReqId;
	HLIB_MSG_Send(pstSmbs->ulMsgQueue, &stMsg, sizeof(UMMA_SMBS_MSG_t));
}

static UMMA_SMBS_DATA_t * umma_smbs_createMsgData( void *p1, void *p2, void *p3 )
{
	UMMA_SMBS_DATA_t *pstMsgData = NULL;
	pstMsgData = (UMMA_SMBS_DATA_t *)HLIB_STD_MemAlloc(sizeof(UMMA_SMBS_DATA_t));
	pstMsgData->nParam1 = (HINT32)p1;
	pstMsgData->nParam2 = (HINT32)p2;
	pstMsgData->nParam3 = (HINT32)p3;

	return pstMsgData;
}

static void umma_smbs_releaseMsgData( UMMA_SMBS_DATA_t **ppstMsgData )
{
	UMMA_SMBS_DATA_t *pstMsgData = *ppstMsgData;

	if( NULL == pstMsgData )
		return ;

	if ( pstMsgData->nParam1 )
		HLIB_STD_MemFree( (void *)pstMsgData->nParam1 );
	if ( pstMsgData->nParam2 )
		HLIB_STD_MemFree( (void *)pstMsgData->nParam2 );
	if ( pstMsgData->nParam3 )
		HLIB_STD_MemFree( (void *)pstMsgData->nParam3 );

	HLIB_STD_MemFree( pstMsgData );
	*ppstMsgData = NULL;
}

static inline void umma_smbs_set_notifyData( UMMA_SMBS_DATA_t *pstNotiData, HINT32 nErr, void *pvParam1, void *pvParam2)
{
	pstNotiData->nParam1 = nErr;
	pstNotiData->nParam2 = (HINT32)pvParam1;
	pstNotiData->nParam3 = (HINT32)pvParam2;
}


static HERROR umma_smbs_init( UMMA_SMBS_t *pstSmbs )
{
	HERROR ret = ERR_FAIL;

	if ( pstSmbs->bInit )
	{
		HxLOG_Debug("[UMMA_SMBS] Init is already. %s()\n", __FUNCTION__);
		return ERR_OK;
	}

	umma_smbs_init_config( &pstSmbs->stConfig );

	ret = umma_smbs_check_configPath();
	if ( ERR_OK != ret )
	{
		HxLOG_Error( "[UMMA_SMBS] umma_smbs_check_configPath() - Error(%d)\n", ret );
		return ERR_FAIL;
	}

	ret = umma_smbs_check_configFile();
	if( ERR_OK != ret )
	{
		HxLOG_Error( "[UMMA_SMBS] umma_smbs_check_configFile() - Error(%d)\n", ret );
		return ERR_FAIL;
	}

	umma_smbs_init_users( &pstSmbs->pstUserList );

	pstSmbs->bInit = TRUE;
	return ERR_OK;
}

#define	______________RPC_API____________________
static HERROR	umma_smbs_rpc_serverStart(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;

	UMMA_ASSERT(nArgc == 0);

	pSmbs = umma_smbs_getInstance();
	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_SERVER_START, NULL, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_serverStop(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;

	UMMA_ASSERT(nArgc == 0);

	pSmbs = umma_smbs_getInstance();
	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_SERVER_STOP, NULL, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_getServerInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;

	UMMA_ASSERT(nArgc == 0);

	pSmbs = umma_smbs_getInstance();
	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_GET_SERVERINFO, NULL, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_setConfig(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	UAPI_SMBS_CONF_t *pConfig = NULL;
	UMMA_SMBS_DATA_t *pData = NULL;

	UMMA_ASSERT(nArgc == 1);

	pConfig = (UAPI_SMBS_CONF_t *)HLIB_STD_MemAlloc(sizeof(UAPI_SMBS_CONF_t));
	pData = umma_smbs_createMsgData( pConfig, NULL, NULL );

	HxSTD_MemCopy( pConfig, HxOBJECT_BIN_DATA(apArgv[0]), sizeof(UAPI_SMBS_CONF_t) );

	HxLOG_Debug("setConfig : %s\n", pConfig->szServerName);
	pSmbs = umma_smbs_getInstance();
	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_SET_CONFIG, pData, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_setPasswd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	UMMA_SMBS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	HCHAR *pPasswd = NULL;
	HCHAR *pNewPasswd = NULL;

	UMMA_ASSERT(nArgc == 3);

	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pPasswd = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pNewPasswd = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pData = umma_smbs_createMsgData( pUserName, pPasswd, pNewPasswd );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	HxSTD_StrNCpy( pPasswd, HxOBJECT_STR(apArgv[1]), 128-1);
	HxSTD_StrNCpy( pNewPasswd, HxOBJECT_STR(apArgv[2]), 128-1);

	pSmbs = umma_smbs_getInstance();
	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_SET_PASSWD, pData, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_checkPasswd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	HCHAR *pUserName = NULL;
	HCHAR *pPasswd = NULL;
	HINT32 isValid = FALSE;
	HERROR	ret = ERR_FAIL;

	UMMA_ASSERT(nArgc == 2);

	pUserName = HxOBJECT_STR(apArgv[0]);
	pPasswd = HxOBJECT_STR(apArgv[1]);

	pSmbs = umma_smbs_getInstance();
	ret = umma_smbs_isValid_passwd( pSmbs, pUserName, pPasswd, &isValid );
	umma_smbs_releaseInstance(pSmbs);
	HLIB_RPC_Response(pSmbs->nRpcHandle, nClientHandle, nRequestId, ret, "i", isValid);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_setPermission(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	UMMA_SMBS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	UAPI_SMBS_SHARED_t *pShared = NULL;

	UMMA_ASSERT(nArgc == 2);
	pSmbs = umma_smbs_getInstance();
	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pShared = (UAPI_SMBS_SHARED_t *)HLIB_STD_MemAlloc(sizeof(UAPI_SMBS_SHARED_t));
	pData = umma_smbs_createMsgData( pUserName, pShared, NULL );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	HxSTD_MemCopy( pShared, HxOBJECT_BIN_DATA(apArgv[1]), sizeof(UAPI_SMBS_SHARED_t));

	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_SET_PERMISSION, pData, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_getSharedList(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	HCHAR *pUserName = NULL;
	HINT32 nCnt = 0;
	UAPI_SMBS_SHARED_t *pSharedList = NULL;
	HERROR	ret = ERR_FAIL;

	UMMA_ASSERT(nArgc == 1);

	pUserName = HxOBJECT_STR(apArgv[0]);

	pSmbs = umma_smbs_getInstance();
	ret = umma_smbs_get_sharedList( pSmbs, pUserName, &nCnt, &pSharedList );
	umma_smbs_releaseInstance(pSmbs);

	if (nCnt > 0)
	{
		HLIB_RPC_Response(pSmbs->nRpcHandle, nClientHandle, nRequestId, ret, "b", pSharedList, sizeof(UAPI_SMBS_SHARED_t) * nCnt);
	}
	else
	{
		HINT32 tmp = 0;

		HLIB_RPC_Response(pSmbs->nRpcHandle, nClientHandle, nRequestId, ret, "b", &tmp, sizeof(HINT32));
	}

	if (pSharedList)
		HLIB_STD_MemFree(pSharedList);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_setPublicAccess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	UMMA_SMBS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	HINT32 *pbAccess = NULL;

	UMMA_ASSERT(nArgc == 2);

	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pbAccess = (HINT32 *)HLIB_STD_MemAlloc(sizeof(HINT32));
	pData = umma_smbs_createMsgData( pUserName, pbAccess, NULL );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	*pbAccess =  HxOBJECT_INT(apArgv[1]) ? TRUE : FALSE;

	pSmbs = umma_smbs_getInstance();
	umma_smbs_sendMsgToMainTask( pSmbs, eUMMA_SMBS_CMD_SET_PUBLICACCESS, pData, nRequestId );
	umma_smbs_releaseInstance(pSmbs);

	return ERR_OK;
}

static HERROR	umma_smbs_rpc_getPublicAccess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_SMBS_t	*pSmbs;
	HCHAR *pUserName = NULL;
	HBOOL bAccess = FALSE;
	HERROR	ret = ERR_FAIL;

	UMMA_ASSERT(nArgc == 1);

	pUserName = HxOBJECT_STR(apArgv[0]);

	pSmbs = umma_smbs_getInstance();
	ret = umma_smbs_get_publicAccess( pSmbs, pUserName, &bAccess );
	umma_smbs_releaseInstance(pSmbs);
	HLIB_RPC_Response(pSmbs->nRpcHandle, nClientHandle, nRequestId, ret, "i", bAccess);
	return ERR_OK;
}


#define __________Public_Function__________
HERROR	UMMA_SMBS_Init(void)
{
	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	HxLOG_Trace();

	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_Start, NULL, umma_smbs_rpc_serverStart,
		NULL, "SAMBA Server Start: (void)");
	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_Stop, NULL, umma_smbs_rpc_serverStop,
		NULL, "SAMBA Server Stop: (void)");

	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_SetConfig, "b", umma_smbs_rpc_setConfig,
		NULL, "SMB Server Set Config: (UAPI_SMBS_CONF_t *pstConfig)");
	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_GetServerInfo, NULL, umma_smbs_rpc_getServerInfo,
		NULL, "SMB Server Get Info: (void)");

	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_SetPasswd, "sss", umma_smbs_rpc_setPasswd,
		NULL, "SAMBA Server Set Password: (void)");
	HLIB_RPC_RegisterRequest(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_CheckPasswd, "ss", umma_smbs_rpc_checkPasswd,
		NULL, "SAMBA Server check password: (void)");

	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_SetPermission, "sb", umma_smbs_rpc_setPermission,
		NULL, "SMB Server Set Permission: (HCHAR *pszUserName, UAPI_SMBS_SHARED_t *pstShared)");
	HLIB_RPC_RegisterRequest(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_GetSharedList, "s", umma_smbs_rpc_getSharedList,
		NULL, "SMB Server Get SharedList: (HCHAR *pszUserName)");
	HLIB_RPC_RegisterCall(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_SetPublicAccess, "si", umma_smbs_rpc_setPublicAccess,
		NULL, "FTP Server Set Public Access: (HCHAR *pszUserName, UAPI_SMBS_SHARED_t *pstShared)");
	HLIB_RPC_RegisterRequest(pstSmbs->nRpcHandle, RPC_UAPI_SMBS_GetPublicAccess, "s",
			umma_smbs_rpc_getPublicAccess, NULL, "FTP Server Get Public Access: (HCHAR *pszUserName)");

	HLIB_RPC_CreateNotification(pstSmbs->nRpcHandle, RPC_UMMA_SMBS_onRequestResult, "iibb", "notify for Server Status (HINT32 nRequestId, UAPI_STATUS_t *pstStatus)");
	umma_smbs_releaseInstance(pstSmbs);

	umma_smbs_init( pstSmbs );

	NAPI_Init();
	NAPI_NetConf_RegisterCallback(umma_smbs_netconf_callback);

	HAPI_ConnectSignal("signal:onFormatOccurred", NULL, umma_smbs_signal_onFormatOccurred);
	HAPI_ConnectSignal("signal:onFactoryDefault", NULL, umma_smbs_signal_onFactoryDefault);
#if defined(CONFIG_DEBUG)
	umma_smbs_debug_InitCommandTools();
#endif
	return 0;
}

HERROR	UMMA_SMBS_Start(void)
{
	HERROR hErr;
	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	hErr = umma_smbs_start_server( pstSmbs );
	umma_smbs_releaseInstance(pstSmbs);

	return hErr;
}

HERROR	UMMA_SMBS_Stop(void)
{
	HERROR hErr;
	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	hErr = umma_smbs_stop_server( pstSmbs );
	umma_smbs_releaseInstance(pstSmbs);

	return hErr;
}

HERROR	UMMA_SMBS_GetName(HCHAR *pszSvrName, HINT32 nBufSize)
{
	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();
	HxSTD_StrNCpy( pszSvrName, pstSmbs->szNetbiosName, nBufSize -1 );
	umma_smbs_releaseInstance(pstSmbs);
	return ERR_OK;
}

HBOOL	UMMA_SMBS_IsStarted(void)
{
	HBOOL bStarted = FALSE;
	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	bStarted = pstSmbs->bIsStarted;
	umma_smbs_releaseInstance(pstSmbs);
	return bStarted;
}

#define	______________API____________________

static void		umma_smbs_mainLoop(void *arg)
{
	UMMA_SMBS_t			*pstSmbs = (UMMA_SMBS_t *)arg;
	UMMA_SMBS_MSG_t		stMsg = { 0 };
	UMMA_SMBS_DATA_t	*pRcvData = NULL;
	UMMA_SMBS_DATA_t	stSendData;
	HINT32		nReqId = 0;
	HERROR		ret = ERR_FAIL;
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR		szsmbsSharedMedia[SMBS_MOUNT_PATH_LEN];
#endif

	while (1)
	{
		if ( VK_OK != HLIB_MSG_Receive(pstSmbs->ulMsgQueue, &stMsg, sizeof(UMMA_SMBS_MSG_t)) )
		{
			VK_TASK_Sleep(100);
			continue;
		}

		pRcvData = (UMMA_SMBS_DATA_t *)stMsg.nParam1;
		nReqId = (HINT32)stMsg.nParam2;
		HxSTD_MemSet( &stSendData, 0x00, sizeof(UMMA_SMBS_DATA_t) );
		switch ( stMsg.nMessage )
		{
			case eUMMA_SMBS_CMD_SERVER_START :
				ret = umma_smbs_start_server( pstSmbs );
				umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
				umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_SERVER_START, &stSendData, nReqId);
				break;
			case eUMMA_SMBS_CMD_SERVER_STOP :
				ret = umma_smbs_stop_server( pstSmbs );
				umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
				umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_SERVER_STOP, &stSendData, nReqId );
				break;
			case eUMMA_SMBS_CMD_SET_CONFIG :
				{
					UAPI_SMBS_CONF_t *pConf = (UAPI_SMBS_CONF_t*)pRcvData->nParam1;
					ret = umma_smbs_set_config( pstSmbs, pConf );
					umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_CONFIG_CHANGE, &stSendData, nReqId );
				}
				break;
			case eUMMA_SMBS_CMD_GET_SERVERINFO :
				{
					UAPI_SMBS_CONF_t stConf;
					ret = umma_smbs_get_config( &stConf );
					umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_RESPONE_SERVERINFO, &stSendData, nReqId );
				}
				break;
			case eUMMA_SMBS_CMD_SET_PASSWD :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HCHAR *pPasswd = (HCHAR *)pRcvData->nParam2;
					HCHAR *pNewPasswd = (HCHAR *)pRcvData->nParam3;

					ret = umma_smbs_change_passwd( pstSmbs, pUserName, pPasswd, pNewPasswd );
					umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_PASSWD_CHANGE, &stSendData, nReqId );
				}
				break;
			case eUMMA_SMBS_CMD_CHK_PASSWD :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HCHAR *pPasswd = (HCHAR *)pRcvData->nParam2;
					HINT32 isValid = FALSE;

					ret = umma_smbs_isValid_passwd( pstSmbs, pUserName, pPasswd, &isValid );
					umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_RESPONE_CHKPASSWD, &stSendData, nReqId );
				}
				break;
			case eUMMA_SMBS_CMD_SET_PERMISSION :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					UAPI_SMBS_SHARED_t *pShared = (UAPI_SMBS_SHARED_t *)pRcvData->nParam2;
					ret = umma_smbs_set_permission( pstSmbs, pUserName, "", pShared );
					umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_PERMISSION_CHANGE, &stSendData, nReqId );
				}
				break;

			case eUMMA_SMBS_CMD_GET_SHAREDLIST :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HINT32 nCnt = 0;
					UAPI_SMBS_SHARED_t *pSharedList = NULL;
					ret = umma_smbs_get_sharedList( pstSmbs, pUserName, &nCnt, &pSharedList );
					umma_smbs_set_notifyData( &stSendData, ret, (void *)nCnt, (void *)pSharedList );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_RESPONE_SHAREDLIST, &stSendData, nReqId );
					if ( pSharedList )
						HLIB_STD_MemFree(pSharedList);
				}
				break;
			case  eUMMA_SMBS_CMD_SET_PUBLICACCESS :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HINT32 *pAccess = (HINT32 *)pRcvData->nParam2;
					ret = umma_smbs_set_publicAccess( pstSmbs, pUserName, *pAccess ? TRUE : FALSE );
					umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_PERMISSION_CHANGE, &stSendData, nReqId );
				}
				break;
			case eUMMA_SMBS_CMD_RETRY_MOUNT :
				HxLOG_Debug("[UMMA_SMBS] retry mount...(%d)\n",pstSmbs->nRetryCnt );
				++pstSmbs->nRetryCnt;
				if ( pstSmbs->bIsStarted && !pstSmbs->bIntHddMounted )
				{
					VK_TASK_Sleep(3000);
#if defined(CONFIG_PROD_HGS1000S)
					HxSTD_snprintf (szsmbsSharedMedia, SMBS_MOUNT_PATH_LEN, "%s%s", s_szsmbsMountPath, SMBS_SHARED_MEDIA);
					if ( 0 == access(szsmbsSharedMedia, F_OK) )
#else
					if ( 0 == access(SMBS_SHARED_MEDIA, F_OK) )
#endif
					{
						pstSmbs->bIntHddMounted = TRUE;
						/* mount --bind */
						HLIB_LIST_ForeachEx( pstSmbs->pstUserList, umma_smbs_cb_mountBind, NULL );
					}
					else
					{
#if defined(CONFIG_PROD_HGS1000S)
						umma_smbs_seek_hdd(&pstSmbs->bIntHddMounted);
						if(!pstSmbs->bIntHddMounted)
#endif
							umma_smbs_sendMsgToMainTask( pstSmbs, eUMMA_SMBS_CMD_RETRY_MOUNT, NULL, 0 );
					}
				}
				break;
			default :
				break;
		}

		umma_smbs_releaseMsgData( &pRcvData );
	}
}

#if defined(CONFIG_DEBUG)
static int	umma_smbs_debug_cmdActionTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR subStr[20] = { 0 };
	HBOOL bResEvt = FALSE;
	UMMA_SMBS_DATA_t	stSendData;
	HERROR		ret = ERR_FAIL;

	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if ( aucArg )
	{
		sscanf(aucArg, "%s", (char *)subStr);
		HLIB_CMD_Printf("[SMBS_CMD] subCmd.\n", subStr);
		HLIB_STD_StrUpper(subStr);
		if ( !HxSTD_StrCmp( "EVT", subStr ) )
			bResEvt = TRUE;
	}

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[SMBS_CMD] Server %s, Event : %s\n", aucStr, bResEvt ? "TRUE":"FLASE");
	if ( !HxSTD_StrCmp( "START", aucStr ) )
	{
		umma_smbs_init_config( &pstSmbs->stConfig );
		umma_smbs_set_config( pstSmbs, &pstSmbs->stConfig );
		ret = umma_smbs_start_server( pstSmbs );
		umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_SERVER_START, &stSendData, 0 );
	}
	else if ( !HxSTD_StrCmp( "STOP", aucStr ) )
	{
		ret = umma_smbs_stop_server( pstSmbs );
		umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_SERVER_STOP, &stSendData, 0 );
	}
	else if ( !HxSTD_StrCmp( "RESTART", aucStr ) )
	{
		ret = umma_smbs_stop_server( pstSmbs );
		umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_SERVER_STOP, &stSendData, 0 );

		ret = umma_smbs_start_server( pstSmbs );
		umma_smbs_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_smbs_notifyEventToClient( pstSmbs, eUAPI_SMBS_EVT_SERVER_START, &stSendData, 0 );
	}
	else
	{
		HLIB_CMD_Printf("[SMBS_CMD] %s is not supported.\n", aucStr);
	}
	umma_smbs_releaseInstance(pstSmbs);

	return HxCMD_OK;
}

static int	umma_smbs_debug_cmdConfgTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szCmd[128] = { 0 };

	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[SMBS_CMD] %s Config.\n", aucStr);
	if ( !HxSTD_StrCmp( "NEW", aucStr ) )
	{
		UMMA_SMBS_CONFIG_t stConfig;
		HxSTD_MemSet( &stConfig, 0x00, sizeof(UMMA_SMBS_CONFIG_t) );

		umma_smbs_init_config ( &stConfig );
		umma_smbs_check_configPath();
		umma_smbs_check_configFile();
		HxSTD_StrNCpy( stConfig.szWorkgroup, "WORKGROUP", sizeof(stConfig.szWorkgroup)-1 );
		HxSTD_StrNCpy( stConfig.szServerName, "HMS1000S", sizeof(stConfig.szServerName)-1 );
		HxSTD_StrNCpy( stConfig.szServerDesc, "Samba Server", sizeof(stConfig.szServerDesc)-1 );

		umma_smbs_set_config( pstSmbs, &stConfig );

		HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", SMBS_CONF_PATH );
		VK_SYSTEM_Command (szCmd);
	}
	else if ( !HxSTD_StrCmp( "SHOW", aucStr ) )
	{
		HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", SMBS_CONF_PATH );
		VK_SYSTEM_Command (szCmd);
	}
	else if ( !HxSTD_StrCmp( "DEL", aucStr ) )
	{
		HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", SMBS_RW_LINK_PATH );
		VK_SYSTEM_Command (szCmd);
		HxSTD_snprintf( szCmd, sizeof(szCmd), "ls -al %s", "/etc" );
		VK_SYSTEM_Command (szCmd);
	}
	else
	{
		HLIB_CMD_Printf("[SMBS_CMD] %s is not supported.\n", aucStr);
	}
	umma_smbs_releaseInstance(pstSmbs);
	return HxCMD_OK;
}

static int	umma_smbs_debug_cmdPublicTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szCmd[128] = { 0 };
	HCHAR szUser[128] = { 0 };
	HBOOL bAccess = FALSE;
	HERROR ret =ERR_FAIL;

	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}

	HLIB_CMD_Printf("[SMBS_CMD] public info. user :%s\n", aucStr);
	HxSTD_StrNCpy( szUser, aucStr, sizeof(szUser)-1 );

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	HLIB_STD_StrUpper(aucStr);
	if ( !HxSTD_StrCmp( "TRUE", aucStr ) )
	{
		bAccess = TRUE;
		HLIB_CMD_Printf("[SMBS_CMD] public set : %s\n", aucStr);
		ret = umma_smbs_set_publicAccess( pstSmbs, szUser, bAccess );
	}
	else if ( !HxSTD_StrCmp( "FALSE", aucStr ) )
	{
		HLIB_CMD_Printf("[SMBS_CMD] public set : %s\n", aucStr);
		ret = umma_smbs_set_publicAccess( pstSmbs, szUser, bAccess );
	}
	else if ( !HxSTD_StrCmp( "PRINT", aucStr ) )
	{
		umma_smbs_get_publicAccess( pstSmbs, szUser, &bAccess );
		HLIB_CMD_Printf("[SMBS_CMD] public Access : %s \n", bAccess ? "true":"false");
	}

	HLIB_CMD_Printf("[SMBS_CMD] cmd : %s, print :%s\n",aucArg, SMBS_SHARECONF_PATH );
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", SMBS_SHARECONF_PATH );
	VK_SYSTEM_Command (szCmd);
	umma_smbs_releaseInstance(pstSmbs);
	return HxCMD_OK;
}

static int	umma_smbs_debug_cmdPermsTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szCmd[128] = { 0 };
	HCHAR szUser[128] = { 0 };
	HBOOL bRw = FALSE;
	HINT32 nCount = 0;
	HINT32 i = 0;
	UAPI_SMBS_SHARED_t *pSharedList = NULL;
	UAPI_SMBS_SHARED_t stShared;
	HERROR ret =ERR_FAIL;

	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL )
	{
		umma_smbs_releaseInstance(pstSmbs);

		return HxCMD_ERR;
	}
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}

	HLIB_CMD_Printf("[SMBS_CMD] public info. user :%s\n", aucStr);
	HxSTD_StrNCpy( szUser, aucStr, sizeof(szUser)-1 );

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	HLIB_STD_StrUpper(aucStr);


	umma_smbs_get_sharedList ( pstSmbs, szUser, &nCount, &pSharedList );
	if ( !HxSTD_StrCmp( "RW", aucStr ) )
	{
		HLIB_CMD_Printf("[SMBS_CMD] Permission set : %s\n", aucStr);
		bRw = TRUE;

	}
	else if ( !HxSTD_StrCmp( "RO", aucStr ) )
	{
		HLIB_CMD_Printf("[SMBS_CMD] Permission set : %s\n", aucStr);
		bRw = FALSE;
	}
	else
	{
		if ( pSharedList )
			HLIB_STD_MemFree(pSharedList);
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}

	for ( i = 0; i< nCount; ++i )
	{
		HxSTD_MemCopy( &stShared, &pSharedList[i], sizeof(UAPI_SMBS_SHARED_t) );
		stShared.bWritable = bRw;
		ret = umma_smbs_set_permission( pstSmbs, szUser, "",  &stShared );
	}

	HLIB_CMD_Printf("[SMBS_CMD] cmd : %s, print :%s\n",aucArg, SMBS_SHARECONF_PATH );
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", SMBS_SHARECONF_PATH );
	VK_SYSTEM_Command (szCmd);
	if ( pSharedList )
		HLIB_STD_MemFree(pSharedList);
	umma_smbs_releaseInstance(pstSmbs);
	return HxCMD_OK;
}

static int	umma_smbs_debug_cmdPasswdTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szUser[128] = { 0 };
	HCHAR szPasswd[128] = { 0 };

	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}

	HLIB_CMD_Printf("[SMBS_CMD] passwd change.... user :%s\n", aucStr);
	HxSTD_StrNCpy( szUser, aucStr, sizeof(szUser)-1 );

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	HxSTD_StrNCpy( szPasswd, aucStr, sizeof(szPasswd)-1 );

	umma_smbs_change_passwd ( pstSmbs, szUser, "", szPasswd );
	umma_smbs_releaseInstance(pstSmbs);
	return HxCMD_OK;
}


static int	umma_smbs_debug_cmdPrintInfo(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HINT32 i = 0;

	UMMA_SMBS_t *pstSmbs = umma_smbs_getInstance();

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )
	{
		umma_smbs_releaseInstance(pstSmbs);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[SMBS_CMD] print info :%s\n", aucStr);
	if ( !HxSTD_StrCmp( "USER", aucStr ) )
	{
		HxList_t *pUserItem = pstSmbs->pstUserList;
		UMMA_SMBS_USER_t *pUser = NULL;
		HLIB_CMD_Printf("[SMBS_CMD] USER Count : %2d\n", HLIB_LIST_Length(pstSmbs->pstUserList ));
		while ( pUserItem )
		{
			pUser = (UMMA_SMBS_USER_t *)HLIB_LIST_Data(pUserItem);
			HLIB_CMD_Printf("[SMBS_CMD] %2d. User\n", i);
			HLIB_CMD_Printf("[SMBS_CMD]   szName : %s \n", pUser->szName);
			HLIB_CMD_Printf("[SMBS_CMD]   bPublic : %s\n", pUser->bPublic ? "TRUE":"FALSE" );
			HLIB_CMD_Printf("[SMBS_CMD]   szHomePath: %s\n", pUser->szHomePath);
			i++;
			pUserItem = HLIB_LIST_Next(pUserItem);
		}
	}
	else if ( !HxSTD_StrCmp( "SHARED", aucStr ) )
	{
		HCHAR szUserName[128] = { 0 };
		HINT32 nCount = 0;
		UAPI_SMBS_SHARED_t *pSharedList = NULL;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( !aucArg )
		{
			umma_smbs_releaseInstance(pstSmbs);
			return HxCMD_OK;
		}
		sscanf(aucArg, "%s", (char *)szUserName);
		umma_smbs_get_sharedList ( pstSmbs, szUserName, &nCount, &pSharedList );
		HLIB_CMD_Printf("[SMBS_CMD] %s Shared Count : %2d\n", szUserName, nCount);
		for ( i = 0; i<nCount; ++i )
		{
			HLIB_CMD_Printf("[SMBS_CMD] %2d. Share Folder\n", i);
			HLIB_CMD_Printf("[SMBS_CMD]   szName : %s \n", pSharedList[i].szName);
			HLIB_CMD_Printf("[SMBS_CMD]   szPath : %s \n", pSharedList[i].szPath);
			HLIB_CMD_Printf("[SMBS_CMD]   bWritable : %s \n", pSharedList[i].bWritable ? "TRUE" : "FALSE");
			HLIB_CMD_Printf("[SMBS_CMD]   bMounted : %s \n", pSharedList[i].bMounted ? "TRUE" : "FALSE");
		}

		if ( pSharedList )
			HLIB_STD_MemFree(pSharedList);
	}
	umma_smbs_releaseInstance(pstSmbs);
	return HxCMD_OK;
}
static void		umma_smbs_debug_InitCommandTools(void)
{
	HLIB_CMD_RegisterWord("smbs", umma_smbs_debug_cmdActionTest,
						"act",
						"samba server action",
						"act [start|stop|restart] [evt| ]");
	HLIB_CMD_RegisterWord("smbs", umma_smbs_debug_cmdConfgTest,
						"conf",
						"conf samba server",
						"conf [new|del|show]");
	HLIB_CMD_RegisterWord("smbs", umma_smbs_debug_cmdPublicTest,
						"public",
						"ftp server public",
						"public [username] [true|false|print]");
	HLIB_CMD_RegisterWord("smbs", umma_smbs_debug_cmdPermsTest,
						"perms",
						"ftp server permission",
						"perms [username] [true|false|print]");

	HLIB_CMD_RegisterWord("smbs", umma_smbs_debug_cmdPasswdTest,
						"passwd",
						"ftp server set passwd",
						"passwd [username] [****]");


	HLIB_CMD_RegisterWord("smbs", umma_smbs_debug_cmdPrintInfo,
						"print",
						"samba server info",
						"print [user|shared]");

}
#endif


