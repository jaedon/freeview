/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_ftps.c
	@brief

	Description:  									\n
	Module: FTP server				\n

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
#include <time.h>
#include <vkernel.h>
#include <hlib.h>
#include <uapi.h>
#include <hapi.h>

#include <umma_int.h>
#include <umma_fm.h>
#include "umma_ftps.h"
#include <crypt.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include "umma_fs_int.h"
#include "se_uapi.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

//#define FTPS_CMD_FTPD			"/usr/bin/proftpd"			// pro ftp daemon
#define FTPS_CMD_FTPD			"proftpd"			// pro ftp daemon

#define FTPS_LOCALSTATE_PATH		"/tmp/proftpd"
#define FTPS_RW_LINK_PATH		"/var/lib/humaxtv_user/proftpd"
#define FTPS_CONF_PATH		"/etc/proftpd/proftpd.conf"
#define FTPS_PASSWD_PATH		"/etc/proftpd/ftpd.passwd"
#define FTPS_USERS_PATH		"/etc/proftpd/ftpd.users"
#define FTPS_SHARECONF_PATH		"/etc/proftpd/ftpd.shared"
#define FTPS_SHARECONF_BACKUP_PATH		"/etc/proftpd/ftpd.shared.backup"

#define FTPS_HOME_PATH		"/media/ftps"

#define UMMA_FTPS_WORKGROUP_LEN		64
#define UMMA_FTPS_SRVNAME_LEN		64
#define UMMA_FTPS_SVRDESC_LEN		128

/* Default value */
#define FTPS_DEFAULT_PORT	21
#define FTPS_DEFAULT_NAME	"Media FTP Server"
#define FTPS_DEFAULT_USER	"admin"

#define FTPS_SHARED_PATHS "/mnt/hd2/Media/Video;/mnt/hd2/Media/Music;/mnt/hd2/Media/Photo;/mnt/hd2/Media/Download"
#define FTPS_SHARED_MAX		8
#define FTPS_VALID_PASSWORD_LEN	512
#if defined(CONFIG_PROD_HGS1000S)
#define FTPS_SHARED_MEDIA		"/Media"
#define FTPS_SHARED_VIDEO		"/Video"
#define FTPS_SHARED_MUSIC		"/Music"
#define FTPS_SHARED_PHOTO		"/Photo"
#define FTPS_SHARED_DOWNLOAD	"/Download"
#define FTPS_SHARED_RECORDING	"/Recordings"
#define FTPS_MOUNT_PATH_LEN		128
#else
#define FTPS_SHARED_MEDIA		"/mnt/hd2/Media"
#define FTPS_SHARED_VIDEO		"/mnt/hd2/Media/Video"
#define FTPS_SHARED_MUSIC		"/mnt/hd2/Media/Music"
#define FTPS_SHARED_PHOTO		"/mnt/hd2/Media/Photo"
#define FTPS_SHARED_DOWNLOAD	"/mnt/hd2/Media/Download"
#define FTPS_SHARED_RECORDING	"/mnt/hd2/Recordings"
#endif

#define FTPS_CONFS_DEFAULT_PERMS	0440 /* proftpd is working under (mode_t)FTPS_CONFS_DEFAULT_PERMS permission */
#define FTPS_CONFS_WRITE_PERMS		0660

#define FTPS_USER_VALUE			"1004"	//umma
#define FTPS_GROUP_VALUE			"1600"	//pvr

#if defined(CONFIG_ENHANCED_SECURITY)
#define FTPS_USER_NAME				"umma"
#else
#define FTPS_USER_NAME				"nobody"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 0
typedef struct
{
	HCHAR	szServerName[UMMA_FTPS_SRVNAME_LEN];
	HINT32	nPort;

//	HxList_t *pstSharedList;
} UMMA_FTPS_CONFIG_t;
#else
typedef UAPI_FTPS_CONF_t UMMA_FTPS_CONFIG_t;
#endif
typedef struct
{
	HCHAR szName[64];
	HCHAR szHomePath[128];
	HxList_t *pstSharedList;
	HxList_t *pstBindList; //target path for mount. ex) /media/smbs/admin/inter
	HBOOL bPublic;
} UMMA_FTPS_USER_t;

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
	UMMA_FTPS_CONFIG_t stConfig;
	HBOOL bInit;
	HBOOL bIsStarted;
	HBOOL bIntHddMounted;
	HINT32 nRetryCnt;
} UMMA_FTPS_t;

typedef enum
{
	eUMMA_FTPS_CMD_SERVER_START = 0,
	eUMMA_FTPS_CMD_SERVER_STOP,
	eUMMA_FTPS_CMD_SET_CONFIG,
	eUMMA_FTPS_CMD_GET_SERVERINFO,
	eUMMA_FTPS_CMD_SET_PASSWD,
	eUMMA_FTPS_CMD_CHK_PASSWD,
	eUMMA_FTPS_CMD_SET_PERMISSION,
	eUMMA_FTPS_CMD_GET_SHAREDLIST,
	eUMMA_FTPS_CMD_SET_PUBLICACCESS,
	eUMMA_FTPS_CMD_RETRY_MOUNT,

	eUMMA_FTPS_CMD_MAX
} UMMA_FTPS_CMD_e;

typedef UMMA_MSG_t UMMA_FTPS_MSG_t;
typedef UMMA_MSG_t UMMA_FTPS_DATA_t;

#define STR_ENUM(Enum) #Enum

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static UMMA_FTPS_t	*s_pstUmmaFtps = NULL;
#if defined(CONFIG_PROD_HGS1000S)
static HCHAR s_szftpsMountPath[FTPS_MOUNT_PATH_LEN] = "/mnt/hd2";
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static HERROR	umma_ftps_stop_server( UMMA_FTPS_t *pstFtps );
static HERROR	umma_ftps_get_config( UMMA_FTPS_CONFIG_t *pstConfig );
static void umma_ftps_sendMsgToMainTask( UMMA_FTPS_t *pstFtpsCtx, UMMA_FTPS_CMD_e eCmd, void *data, HINT32 nReqId );
static void		umma_ftps_mainLoop(void *arg);
static HERROR	umma_ftps_get_sharedList( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, HINT32 *pnCount, UAPI_FTPS_SHARED_t **ppstShared );
static HERROR	umma_ftps_remove_user( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName );
static HERROR	umma_ftps_add_user( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, const HCHAR *pszPasswd );
static HERROR	umma_ftps_init_sharedListOfUser( UMMA_FTPS_USER_t *pstUser, HCHAR *line, HINT32 bufSize );

#if defined(CONFIG_PROD_HGS1000S)
static HERROR umma_ftps_set_ftpdSharedConfig(void);
static HERROR umma_ftps_seek_hdd(HBOOL *pbHddMounted);
static HERROR	umma_ftps_update_sharedListByLine( UMMA_FTPS_USER_t *pstUser, HCHAR *pszline);
static HBOOL umma_ftps_update_userSharedList( void *pvUser, void *pvNotUse );

extern HERROR	UMMA_FS_GetStorageInfo(HUINT32 ulDeviceId, DxStorage_Info_t* pstStorageInfo);
#endif

#if defined(CONFIG_DEBUG)
static void		umma_ftps_debug_InitCommandTools(void);
#endif
#define __________Internal_Function__________

#if defined(CONFIG_PROD_HGS1000S)
static HERROR	umma_ftps_update_sharedListByLine( UMMA_FTPS_USER_t *pstUser, HCHAR *pszline)
{
	HCHAR szAccess[16] = { 0 };
	HCHAR *pPath = NULL, *pAccess = NULL, *pFoldeName = NULL;
	HCHAR *pTmpStr = NULL, *pTmpEnd = NULL, *pTmpEnd2 = NULL;
	HCHAR *pszAttrPath = "<share path=\"";
	HCHAR *pszAttrAccess = "\" access=\"";
	HCHAR *pszAttrEnd = "\"/>";

	HINT32 attrPathLen = HxSTD_StrLen(pszAttrPath);
	HINT32 attrAccessLen = HxSTD_StrLen(pszAttrAccess);

	UAPI_FTPS_SHARED_t *pShared = NULL;

	if ( NULL == pstUser )
		return ERR_FAIL;

	pTmpStr = HxSTD_StrStr( pszline, pszAttrPath );
	if ( NULL == pTmpStr )
		return ERR_FAIL;

	pTmpEnd = HxSTD_StrStr( pTmpStr, pszAttrAccess );
	if ( NULL == pTmpEnd )
		return ERR_FAIL;

	pShared	= (UAPI_FTPS_SHARED_t *)UMMA_Calloc(sizeof(UAPI_FTPS_SHARED_t));

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
static HBOOL umma_ftps_update_userSharedList( void *pvUser, void *pvNotUse )
{
	UMMA_FTPS_USER_t *pUser = (UMMA_FTPS_USER_t *)pvUser;

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

	fp = fopen( FTPS_SHARECONF_PATH, "r" );
	if (NULL == fp )
		return FALSE;

	//Delete shared list of user
	pUser->pstSharedList = HLIB_LIST_RemoveAll( pUser->pstSharedList);

	/********** cat /etc/proftpd/ftpd.shared  ******************
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
			umma_ftps_update_sharedListByLine( pUser, szline);
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
			HxLOG_Warning("[UMMA_FTPS] Cannot update Shared List due to diffent username!!\n");
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
static HERROR umma_ftps_seek_hdd(HBOOL *pbHddMounted)
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
			if (stStorageInfo.nPartitionNum > 0)
			{
				for (j = 0; j < stStorageInfo.nPartitionNum; j++)
				{
					if ((stStorageInfo.astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_MOUNTED) &&
						(stStorageInfo.astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE))
					{
						*pbHddMounted = TRUE;
						// Update FTP PATH
						HxSTD_MemSet(s_szftpsMountPath, 0, sizeof(s_szftpsMountPath));
						HxSTD_StrNCpy(s_szftpsMountPath, stStorageInfo.szMountPath, sizeof(s_szftpsMountPath)-1);
						return ERR_OK;
					}
				}
			}
		}
	}
	HxLOG_Warning("[UMMA_FTPS] Cannot find any hdd!!\n");
	return ERR_FAIL;
}

/** Description: Update new hdd-path for shared config (/etc/proftd/ftpd.shared)
* * \param[]	 void
* * \return      OK, or other Error on failure
*/
static HERROR umma_ftps_set_ftpdSharedConfig(void)
{
	FILE *fp = NULL;
	HCHAR szftpsName[FTPS_MOUNT_PATH_LEN];

	fp = fopen( FTPS_SHARECONF_PATH, "w" );
	if (NULL == fp)
	{
		return ERR_FAIL;
	}
	fprintf( fp, "<user name=\"%s\" public=\"%s\">\n", FTPS_DEFAULT_USER, "yes" );
	HxSTD_snprintf (szftpsName, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_VIDEO);
	fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szftpsName );
	HxSTD_snprintf (szftpsName, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_MUSIC);
	fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szftpsName );
	HxSTD_snprintf (szftpsName, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_PHOTO);
	fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szftpsName );
	HxSTD_snprintf (szftpsName, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_DOWNLOAD);
	fprintf( fp, " <share path=\"%s\" access=\"rw\"/>\n", szftpsName );
	fprintf( fp, "</user>\n");

	fclose(fp);
	fp = NULL;
	return ERR_OK;
}
#endif

STATIC void umma_ftps_notifyEventToClient( UMMA_FTPS_t *pstFtps, UAPI_FTPS_EVT_e eEvent, void *pvData, int nReqId)
{
	UAPI_FTPS_INFO_t stInfo;
	UMMA_FTPS_DATA_t *pNotiData;
	HINT32	temp = 0;
	HINT32	nRetErr = ERR_FAIL;
	void	*pvResult = &temp;
	HINT32	nResultSize = sizeof(HINT32);
	UAPI_FTPS_SHARED_t tmpShared;
	HxLOG_Debug("[UMMA_FTPS] NOTIFY EVENT : %d \n", eEvent);

	if ( NULL == pvData )
		return ;

	pNotiData = (UMMA_FTPS_DATA_t *)pvData;
	nRetErr = pNotiData->nParam1;

	switch ( eEvent )
	{
		case eUAPI_FTPS_EVT_SERVER_START :
		case eUAPI_FTPS_EVT_SERVER_STOP :
		case eUAPI_FTPS_EVT_CONFIG_CHANGE :
		case eUAPI_FTPS_EVT_PASSWD_CHANGE :
		case eUAPI_FTPS_EVT_PERMISSION_CHANGE :
			break;
		case eUAPI_FTPS_EVT_RESPONE_SHAREDLIST :
			{
				HINT32 nCnt = 0;
				HINT32 listSize = 0;
				UAPI_FTPS_SHARED_t *pSharedList = NULL;
				nCnt = pNotiData->nParam2;

				listSize = nCnt ? sizeof(UAPI_FTPS_SHARED_t) * nCnt : sizeof(HINT32);
				pSharedList = nCnt ? (UAPI_FTPS_SHARED_t*)pNotiData->nParam3 : &tmpShared;

				pvResult = (void  *)pSharedList;
				nResultSize = listSize;
			}
			break;
		case eUAPI_FTPS_EVT_RESPONE_SERVERINFO :
			break;
		case eUAPI_FTPS_EVT_STATUS_CHANGE :
			break;
		default :
			return ;
	}

	HxSTD_MemSet( &stInfo, 0x00, sizeof(UAPI_FTPS_INFO_t));
	stInfo.bStarted = pstFtps->bIsStarted;
	HxSTD_MemCopy( &stInfo.stConf, &pstFtps->stConfig, sizeof(UAPI_FTPS_CONF_t) );

	HLIB_RPC_NotifyWithRequestId(pstFtps->nRpcHandle, RPC_UMMA_FTPS_onRequestResult, nReqId,
			"iibb", eEvent, nRetErr, &stInfo, sizeof(UAPI_FTPS_INFO_t), pvResult, nResultSize);

}


static UMMA_FTPS_t *	umma_ftps_getInstance(void)
{
	if (NULL == s_pstUmmaFtps)
	{
		HULONG tid = 0;
		HERROR hErr;

		s_pstUmmaFtps = (UMMA_FTPS_t *)UMMA_Calloc(sizeof(UMMA_FTPS_t));

		HxSEMT_Create(&s_pstUmmaFtps->ulSemId, "ummaFtpsSem", 0);
		HxSEMT_Get(s_pstUmmaFtps->ulSemId);

		hErr = UMMA_RpcProtocol_GetRpcHandle(&s_pstUmmaFtps->nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error!! (Get RPC Handle)\n");
			HxLOG_Assert(s_pstUmmaFtps->nRpcHandle);
		}

		HLIB_MSG_Create( 128, sizeof(UMMA_EVT_t), "ummaFtpsEvt",  &s_pstUmmaFtps->ulMsgQueue, eHLIB_SUSPENDTYPE_FIFO );

		VK_TASK_Create( umma_ftps_mainLoop, 70, SIZE_64K, "ummaFtpsMainHandler", (void *)s_pstUmmaFtps, &tid, FALSE );
		VK_TASK_Start(tid);

		s_pstUmmaFtps->ulMainTaskId = tid;
	}
	else
	{
		HxSEMT_Get(s_pstUmmaFtps->ulSemId);
	}

	return s_pstUmmaFtps;
}

static void		umma_ftps_releaseInstance(UMMA_FTPS_t *pstFtps)
{
	HxSEMT_Release(pstFtps->ulSemId);
}

static inline HERROR 	umma_ftps_get_bindPath( const HCHAR *pszUser, const HCHAR *pszPath, HCHAR *pszBindPath, HINT32 nBufSize, HBOOL *pbRecording  )
{
#if defined (CONFIG_PROD_HGS1000S)
	HCHAR szftpsSharedMedia[FTPS_MOUNT_PATH_LEN];
	UMMA_FTPS_t *pstFtps = NULL;
#endif

	if (NULL == pbRecording )
		return ERR_FAIL;

#if defined (CONFIG_PROD_HGS1000S)
	pstFtps = umma_ftps_getInstance();
	if (!pstFtps->bIntHddMounted)
	{
		umma_ftps_releaseInstance(pstFtps);
		return ERR_FAIL;
	}
	umma_ftps_releaseInstance(pstFtps);

	HxSTD_snprintf (szftpsSharedMedia, FTPS_MOUNT_PATH_LEN, "%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA);
	if( HxSTD_StrStr( pszPath, szftpsSharedMedia ) )
#else
	if( HxSTD_StrStr( pszPath, FTPS_SHARED_MEDIA ) )
#endif
	{
		HxSTD_snprintf( pszBindPath, nBufSize, "%s/%s", FTPS_HOME_PATH, pszUser );
	}
	*pbRecording = FALSE;
#ifdef CONFIG_SUPPORT_UMMA_FTP_SHARE_RECORDINGS
	if( HxSTD_StrStr( pszPath, FTPS_SHARED_RECORDING ) )
	{
		HxSTD_snprintf( pszBindPath, nBufSize, "%s/%s/%s", FTPS_HOME_PATH, pszUser, "Recordings" );
		*pbRecording = TRUE;
	}
#endif
	/* to do : Dettachable HDD ??? */

	return ERR_OK;
}

static inline HERROR 	umma_ftps_get_storagePath( const HCHAR *pszSharePath, HCHAR *pszStorPath, HINT32 nBufSize  )
{
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szftpsSharedMedia[FTPS_MOUNT_PATH_LEN];
	UMMA_FTPS_t *pstFtps = NULL;

	pstFtps = umma_ftps_getInstance();
	if (!pstFtps->bIntHddMounted)
	{
		umma_ftps_releaseInstance(pstFtps);
		return ERR_FAIL;
	}
	umma_ftps_releaseInstance(pstFtps);

	HxSTD_snprintf (szftpsSharedMedia, FTPS_MOUNT_PATH_LEN, "%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA);
	if( HxSTD_StrStr( pszSharePath, szftpsSharedMedia ) )
	{
		HxSTD_StrNCpy( pszStorPath, szftpsSharedMedia, nBufSize -1 );
	}
#else
	if( HxSTD_StrStr( pszSharePath, FTPS_SHARED_MEDIA ) )
	{
		HxSTD_StrNCpy( pszStorPath, FTPS_SHARED_MEDIA, nBufSize -1 );
	}
#endif
	if( HxSTD_StrStr(pszSharePath, FTPS_SHARED_RECORDING) )
	{
		HxSTD_StrNCpy( pszStorPath, FTPS_SHARED_RECORDING, nBufSize -1 );
	}
	/* to do : Dettachable HDD ??? */

	return ERR_OK;
}


static HERROR	umma_ftps_check_configPath(void)
{
	if ( TRUE != HLIB_DIR_IsExist(FTPS_RW_LINK_PATH) )
	{
		HxLOG_Print( "[UMMA_FTPS] directory is created to Read/Write for FTP config.(%s)\n", FTPS_RW_LINK_PATH );
		if ( ERR_OK !=  HLIB_DIR_Make( FTPS_RW_LINK_PATH ))
		{
			HxLOG_Error( "[UMMA_FTPS] mkdir Failed : %s\n", FTPS_RW_LINK_PATH );
			return ERR_FAIL;
		}
	}

	if ( TRUE != HLIB_DIR_IsExist(FTPS_LOCALSTATE_PATH) )
	{
		HxLOG_Print( "[UMMA_FTPS] directory is created to Read/Write for FTP state.(%s)\n", FTPS_LOCALSTATE_PATH );
		if ( ERR_OK !=  HLIB_DIR_Make( FTPS_LOCALSTATE_PATH ))
		{
			HxLOG_Error( "[UMMA_FTPS] mkdir Failed : %s\n", FTPS_LOCALSTATE_PATH );
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}

static HERROR	umma_ftps_check_configFile(void)
{
	FILE *fp = NULL;
	HINT32 ret = 0;
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szftpsPath[FTPS_MOUNT_PATH_LEN];
#endif
	if ( TRUE != HLIB_DIR_IsExist(FTPS_PASSWD_PATH) )
	{
		fp = fopen( FTPS_PASSWD_PATH, "w" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}
		/* fprintf( fp, "admin:$1$GcGWUAIi$zDJdxAxp1bMOZNALDnOG11:1010:1010::/media/ftps/admin:/bin/false \n" ); */
		fprintf( fp, "%s:$1$C9Gm17cY$TKkMVt2y2yodU3qgELLr4/:%s:%s::%s/%s:/bin/false \n", FTPS_DEFAULT_USER, FTPS_USER_VALUE, FTPS_GROUP_VALUE, FTPS_HOME_PATH, FTPS_DEFAULT_USER);

		if ( fp )
		{
			fclose(fp);
			fp = NULL;
		}

		ret = chmod( FTPS_PASSWD_PATH, FTPS_CONFS_DEFAULT_PERMS);
		if ( -1 == ret )
		{
			HxLOG_Error( "[UMMA_FTPS] chmod Failed. - File(%s)\n", FTPS_PASSWD_PATH );
		}
	}
	else
	{
		/* Temp code */
		HCHAR line[512] = { 0 };
		HBOOL bNeedChange = FALSE;
		HCHAR szValidPasswd[FTPS_VALID_PASSWORD_LEN];

		HxSTD_sprintf(szValidPasswd, "%s:$1$gvib2tx6$5qUiZCErl2eijLyWLkfm6.:%s:%s::%s/%s:/bin/false", FTPS_DEFAULT_USER, FTPS_USER_VALUE, FTPS_GROUP_VALUE, FTPS_USER_VALUE, FTPS_GROUP_VALUE);
		fp = fopen( FTPS_PASSWD_PATH, "r" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}

		while ( fgets(line, sizeof(line)-1, fp) )
		{
			if ( NULL != HxSTD_StrStr( line, szValidPasswd ))
			{
				bNeedChange = TRUE;
				break;
			}
		}
		fclose(fp);
		fp = NULL;
		if ( bNeedChange )
		{
			FILE *new_fp = NULL;
			ret = chmod( FTPS_PASSWD_PATH, FTPS_CONFS_WRITE_PERMS);
			new_fp = fopen(FTPS_PASSWD_PATH, "w");
			if ( NULL == new_fp )
			{
				ret = chmod( FTPS_PASSWD_PATH, FTPS_CONFS_DEFAULT_PERMS);
				return ERR_FAIL;
			}
			//fprintf( new_fp, "admin:$1$C9Gm17cY$TKkMVt2y2yodU3qgELLr4/:1004:1600::/media/ftps/admin:/bin/false \n");

			fprintf( new_fp, "%s:$1$C9Gm17cY$TKkMVt2y2yodU3qgELLr4/:%s:%s::%s/%s:/bin/false \n", FTPS_DEFAULT_USER, FTPS_USER_VALUE, FTPS_GROUP_VALUE, FTPS_HOME_PATH, FTPS_DEFAULT_USER);

			if ( new_fp )
				fclose(new_fp);
			ret = chmod( FTPS_PASSWD_PATH, FTPS_CONFS_DEFAULT_PERMS);
			if ( -1 == ret )
			{
				HxLOG_Error( "[UMMA_FTPS] chmod Failed. - File(%s)\n", FTPS_USERS_PATH );
			}
		}
	}

	if ( TRUE != HLIB_DIR_IsExist(FTPS_USERS_PATH) )
	{
		fp = fopen( FTPS_USERS_PATH, "w" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}
		fprintf( fp, "settop:x:50:admin\n" );

		if ( fp )
		{
			fclose(fp);
			fp = NULL;
		}
		ret = chmod( FTPS_USERS_PATH, FTPS_CONFS_DEFAULT_PERMS);
		if ( -1 == ret )
		{
			HxLOG_Error( "[UMMA_FTPS] chmod Failed. - File(%s)\n", FTPS_USERS_PATH );
		}
	}
#ifdef CONFIG_SUPPORT_UMMA_FTP_SHARE_RECORDINGS
	if ( TRUE == HLIB_DIR_IsExist(FTPS_SHARECONF_PATH) )
	{
		HCHAR line[512] = { 0 };
		HBOOL findRec= FALSE;
		fp = fopen( FTPS_SHARECONF_PATH, "r" );
		if ( NULL != fp )
		{
			while ( fgets(line, sizeof(line)-1, fp) )
			{
				if (HxSTD_StrStr( line, FTPS_SHARED_RECORDING  ))
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
			HLIB_FILE_Delete(FTPS_SHARECONF_PATH);
		}
	}
#endif
	if ( TRUE != HLIB_DIR_IsExist(FTPS_SHARECONF_PATH) )
	{
		fp = fopen( FTPS_SHARECONF_PATH, "w" );
		if (NULL == fp)
		{
			return ERR_FAIL;
		}
		fprintf( fp, "<user name=\"%s\" public=\"%s\">\n", FTPS_DEFAULT_USER, "yes" );
#if defined(CONFIG_PROD_HGS1000S)
		HxSTD_snprintf (szftpsPath, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_VIDEO);
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szftpsPath );
		HxSTD_snprintf (szftpsPath, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_MUSIC);
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szftpsPath );
		HxSTD_snprintf (szftpsPath, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_PHOTO);
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", szftpsPath );
		HxSTD_snprintf (szftpsPath, FTPS_MOUNT_PATH_LEN, "%s%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA, FTPS_SHARED_DOWNLOAD);
		fprintf( fp, " <share path=\"%s\" access=\"rw\"/>\n", szftpsPath );
#else
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", FTPS_SHARED_VIDEO );
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", FTPS_SHARED_MUSIC );
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", FTPS_SHARED_PHOTO );
		fprintf( fp, " <share path=\"%s\" access=\"rw\"/>\n", FTPS_SHARED_DOWNLOAD );
#ifdef CONFIG_SUPPORT_UMMA_FTP_SHARE_RECORDINGS
		fprintf( fp, " <share path=\"%s\" access=\"ro\"/>\n", FTPS_SHARED_RECORDING );
#endif
#endif
		fprintf( fp, "</user>\n");

		fclose(fp);
		fp = NULL;
	}

	return ERR_OK;
}

static HERROR	umma_ftps_init_config( UMMA_FTPS_CONFIG_t *pstConfig )
{
	HxSTD_MemSet( pstConfig, 0x00, sizeof(UMMA_FTPS_CONFIG_t) );


	if ( ERR_OK != umma_ftps_get_config( pstConfig ) )
	{
		/* set default value */
		HxLOG_Debug( "[UMMA_FTPS] set default value for server config.\n" );
		pstConfig->nPort = FTPS_DEFAULT_PORT;
		HxSTD_StrNCpy( pstConfig->szServerName, FTPS_DEFAULT_NAME, sizeof(pstConfig->szServerName)-1 );
	}

	return ERR_OK;
}

static HERROR	umma_ftps_init_sharedListOfUser( UMMA_FTPS_USER_t *pstUser, HCHAR *line, HINT32 bufSize )
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

	UAPI_FTPS_SHARED_t *pShared = NULL;

	if ( NULL == pstUser )
		return ERR_FAIL;

	pTmpStr = HxSTD_StrStr( line, pszAttrPath );
	if ( NULL == pTmpStr )
		return ERR_FAIL;

	pTmpEnd = HxSTD_StrStr( pTmpStr, pszAttrAccess );
	if ( NULL == pTmpEnd )
		return ERR_FAIL;

	pShared	= (UAPI_FTPS_SHARED_t *)UMMA_Calloc(sizeof(UAPI_FTPS_SHARED_t));

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

static HERROR	umma_ftps_init_users( HxList_t **ppstUserList )
{
	UMMA_FTPS_USER_t *pUser = NULL;
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

	fp = fopen( FTPS_SHARECONF_PATH, "r" );
	if (NULL == fp )
		return ERR_FAIL;

	while ( fgets(line, sizeof(line)-1, fp) )
	{
		pTmpStr = pTmpEnd = pTmpEnd2 = NULL;
		pTmpStr = HxSTD_StrStr( line, pszAttrUser );
		if ( NULL == pTmpStr )
		{
			umma_ftps_init_sharedListOfUser( pUser, line, sizeof(line) );
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

		pUser = (UMMA_FTPS_USER_t *)UMMA_Calloc(sizeof(UMMA_FTPS_USER_t));
		HxSTD_MemCopy( pUser->szName, pUserName, (pTmpEnd - pUserName) );

		HxSTD_MemCopy( szPublic, pPublic, (pTmpEnd2 - pPublic) );
		if ( !HxSTD_StrCmp(szPublic, "yes") )
			pUser->bPublic = TRUE;
		else
			pUser->bPublic = FALSE;

		HxSTD_snprintf( pUser->szHomePath, sizeof(pUser->szHomePath), "%s/%s", FTPS_HOME_PATH, pUser->szName );

		*ppstUserList = HLIB_LIST_Append( *ppstUserList, (void *)pUser );
	}

	fclose( fp ) ;
	fp = NULL;
	return ERR_OK;
}

STATIC HBOOL umma_ftps_cb_mountBind( void *pvUser, void *pvNotUse )
{
	UMMA_FTPS_USER_t *pUser = (UMMA_FTPS_USER_t *)pvUser;
	UAPI_FTPS_SHARED_t *pShared = NULL;
	HxList_t *pSharedItem = NULL;
	HCHAR szBindPath[128] = { 0 };
	HCHAR szCompPath[128] = { 0 };
	HBOOL bRecording = FALSE;

	SEUAPI_DoCommand("mkdir -p", pUser->szHomePath);

	pSharedItem = pUser->pstSharedList;
	while ( pSharedItem )
	{
		bRecording = FALSE;
		pShared = (UAPI_FTPS_SHARED_t*)HLIB_LIST_Data(pSharedItem);

		HxSTD_MemSet( szBindPath, 0x00, sizeof(szBindPath) );
		umma_ftps_get_bindPath( pUser->szName, pShared->szPath, szBindPath, sizeof(szBindPath), &bRecording );

		if ( HxSTD_StrCmp( szBindPath, szCompPath ) && !HxSTD_StrEmpty(szBindPath) )
		{
			HCHAR *pBindPath = (HCHAR *)HLIB_STD_MemAlloc(sizeof(szBindPath));
			HCHAR szStorPath[128] = { 0 };
			HxSTD_StrNCpy( szCompPath, szBindPath, sizeof(szCompPath)-1 );
			HxSTD_StrNCpy( pBindPath, szBindPath, sizeof(szBindPath)-1 );
			//HLIB_DIR_Make( pBindPath );

			umma_ftps_get_storagePath( pShared->szPath, szStorPath, sizeof(szStorPath) );

			SEUAPI_MountPartition_Ftps(szStorPath, pBindPath);
			pUser->pstBindList = HLIB_LIST_Append( pUser->pstBindList, pBindPath );
			HxLOG_Print("[UMMA_FTPS] mount --bind  %s %s\n", szStorPath, pBindPath);
		}

		pSharedItem = HLIB_LIST_Next( pSharedItem );
	}
	return 0;
}

STATIC HBOOL umma_ftps_cb_unmountBind( void *pvBindPath, void *pvNotUse )
{
	HCHAR *pBindPath = (HCHAR *)pvBindPath;

	SEUAPI_DoCommand("umount -l", pBindPath); // Added "-l" option for forced umount when device is busy.
	HxLOG_Print("[UMMA_FTPS] umount %s\n", pBindPath );
	return 0;
}

static void	umma_ftps_cb_freeBind( void *pvBindPath )
{
	HLIB_STD_MemFree(pvBindPath);
}


static HBOOL umma_ftps_cb_cmpareByUserName ( void *user_data, void *list_data )
{
	HCHAR *pszUserName = (HCHAR *)user_data;
	UMMA_FTPS_USER_t *pUser = (UMMA_FTPS_USER_t*)list_data;

	if ( !HxSTD_StrCmp( pszUserName, pUser->szName ))
		return TRUE;

	return FALSE;
}

static HBOOL umma_ftps_cb_cmpareBySharedPath ( void *user_data, void *list_data )
{
	HCHAR *pszPath = (HCHAR *)user_data;
	UAPI_FTPS_SHARED_t *pShared = (UAPI_FTPS_SHARED_t*)list_data;

	if ( !HxSTD_StrCmp( pszPath, pShared->szPath ))
		return TRUE;

	return FALSE;
}

static HBOOL umma_ftps_cb_writeShareConfig( void *pvUser, void *pvFp )
{
	UMMA_FTPS_USER_t *pstUser = (UMMA_FTPS_USER_t *)pvUser;
	FILE *fp = (FILE *)pvFp;
	HxList_t *pSharedItem = NULL;
	UAPI_FTPS_SHARED_t *pShared = NULL;
	HCHAR szBindPath[128] = { 0 };
	HBOOL bOnlyOne = FALSE;
	HBOOL bRecording = FALSE;

	if ( !bOnlyOne && pstUser->bPublic )
	{
		fprintf( fp, "<Anonymous %s/%s>\n", FTPS_HOME_PATH, pstUser->szName );
		fprintf( fp, " User %s\n", pstUser->szName );
		fprintf( fp, " Group %s\n", pstUser->szName );
		fprintf( fp, " UserAlias anonymous %s\n", pstUser->szName );
		fprintf( fp, " AnonRequirePassword off\n" );
	}

	pSharedItem = pstUser->pstSharedList;
	while ( pSharedItem )
	{
		bRecording = FALSE;
		pShared = HLIB_LIST_Data(pSharedItem);

		HxSTD_MemSet( szBindPath, 0x00, sizeof(szBindPath) );
		umma_ftps_get_bindPath( pstUser->szName, pShared->szPath, szBindPath, sizeof(szBindPath) , &bRecording);
		if ( bRecording )
			fprintf( fp, "<Directory %s>\n", szBindPath);
		else
			fprintf( fp, "<Directory %s/%s>\n", szBindPath, pShared->szName );
		if ( !pShared->bWritable )
		{
			fprintf( fp, " <Limit WRITE>\n" );
			fprintf( fp, "  DenyAll\n" );
			fprintf( fp, " </Limit>\n" );
		}

		fprintf( fp, "</Directory>\n" );
		pSharedItem = HLIB_LIST_Next(pSharedItem);
	}

	if ( !bOnlyOne && pstUser->bPublic )
	{
		fprintf( fp, "</Anonymous>\n" );
		bOnlyOne = TRUE;
	}

	return FALSE;
}

static HERROR	umma_ftps_signal_onFormatOccurred(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HINT32 devId = 0;
	HCHAR *pMntPath = NULL;
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szftpsSharedMedia[FTPS_MOUNT_PATH_LEN];
#endif
	UMMA_FTPS_t *pstFtps;
	HAPPY(devId);


	if ( 2 != nArgc )
		return ERR_FAIL;

	devId = HxOBJECT_INT(apArgv[0]);
	pMntPath = HxOBJECT_STR(apArgv[1]);

	HxLOG_Debug("[UMMA_FTPS] Received onFormatOccurred Event.\n" );
#if defined(CONFIG_PROD_HGS1000S)
	HxSTD_snprintf (szftpsSharedMedia, FTPS_MOUNT_PATH_LEN, "%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA);
	if ( NULL == HxSTD_StrStr( szftpsSharedMedia, pMntPath ))
#else
	if ( NULL == HxSTD_StrStr( FTPS_SHARED_MEDIA, pMntPath ))
#endif
	{
		/* Not Internal HDD  */
		return ERR_OK;
	}
	pstFtps = umma_ftps_getInstance();
	if ( pstFtps->bIsStarted )
		umma_ftps_stop_server( pstFtps );
	umma_ftps_releaseInstance(pstFtps);

	return ERR_OK;
}
static HERROR	umma_ftps_signal_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR szCmd[128] = { 0 };
	UMMA_FTPS_t *pstFtps = umma_ftps_getInstance();

	if ( pstFtps->bIsStarted )
		umma_ftps_stop_server( pstFtps );
	umma_ftps_releaseInstance(pstFtps);

	HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", FTPS_RW_LINK_PATH );
	VK_SYSTEM_Command (szCmd);
	return ERR_OK;
}


static inline UMMA_FTPS_USER_t * umma_ftps_get_user ( HxList_t *pstUserList, const HCHAR *pszUserName )
{
	HxList_t *pUserItem = NULL;
	pUserItem = HLIB_LIST_FindEx( pstUserList, pszUserName, umma_ftps_cb_cmpareByUserName );
	if( NULL == pUserItem )
		return NULL;

	return  (UMMA_FTPS_USER_t *)HLIB_LIST_Data(pUserItem);
}

static HERROR	umma_ftps_set_config( UMMA_FTPS_t *pstFtps, UMMA_FTPS_CONFIG_t *pstConfig )
{
	FILE *fp = NULL;

	if ( NULL == pstConfig )
	{
		HxLOG_Error( "[UMMA_FTPS] pstConfig is NULL. %s()\n", __FUNCTION__ );
		return ERR_FAIL;
	}
	fp = fopen( FTPS_CONF_PATH, "w" );
	if (NULL == fp)
	{
		HxLOG_Error( "[UMMA_FTPS] file open failed : %s\n", FTPS_CONF_PATH );
		return ERR_FAIL;
	}

	fprintf( fp, "ServerName %s\n", pstConfig->szServerName );
	fprintf( fp, "ServerType %s\n", "standalone" );
	fprintf( fp, "DefaultServer %s\n", "on" );
	fprintf( fp, "ServerIdent %s\n", "off" );

	fprintf( fp, "Port %d\n", pstConfig->nPort );
	fprintf( fp, "Umask %s\n", "022" );

	fprintf( fp, "MaxInstances %d\n", 8 );
	fprintf( fp, "MaxClients %d %s\n", 4, "\"The server is full, hosting %m users\"" );

	fprintf( fp, "User %s\n", FTPS_USER_NAME ); /* <-- by user privilege: settop, ... */
//	fprintf( fp, "Group %s\n", "nobody" );
	fprintf( fp, "DefaultRoot %s/%s\n", FTPS_HOME_PATH, FTPS_DEFAULT_USER);

	fprintf( fp, "AllowOverwrite %s\n", "on" );

	fprintf( fp, "<Limit SITE_CHMOD>\n DenyAll\n</Limit>\n" );

	/* share folder config */
	HLIB_LIST_ForeachEx( pstFtps->pstUserList, umma_ftps_cb_writeShareConfig, (void *)fp );

	fprintf( fp, "RequireValidShell %s\n", "off" );
	fprintf( fp, "AuthUserFile %s\n", FTPS_PASSWD_PATH );
	fprintf( fp, "AuthGroupFile %s\n", FTPS_USERS_PATH );

	fclose(fp);

	// 2 pointer point to same struct -> no neeed to copy
	//HxSTD_MemCopy( &pstFtps->stConfig, pstConfig, sizeof(UAPI_FTPS_CONF_t) );

	return ERR_OK;
}

static HERROR	umma_ftps_get_config( UMMA_FTPS_CONFIG_t *pstConfig )
{
	FILE *fp = NULL;
	HCHAR line[512] = { 0 };
	HCHAR *pSearch = NULL;
	fp = fopen( FTPS_CONF_PATH, "r" );
	if (NULL == fp)
	{
		HxLOG_Error( "[UMMA_FTPS] file open failed : %s\n", FTPS_CONF_PATH );
		return ERR_FAIL;
	}

	while ( fgets(line, sizeof(line)-1, fp) )
	{
		if ( NULL != (pSearch = HxSTD_StrStr( line, "ServerName " )) )
		{
			HCHAR *cariChar = 0;
			HxSTD_StrNCpy( pstConfig->szServerName, pSearch + HxSTD_StrLen("ServerName "), sizeof(pstConfig->szServerName)-1 );
			cariChar = HxSTD_StrRChar( pstConfig->szServerName, '\n' );
			if ( cariChar )
				*cariChar = '\0';
		}
		else if ( NULL != (pSearch = HxSTD_StrStr( line, "Port " )) )
		{
			sscanf( pSearch, "Port %d", &pstConfig->nPort );
			break;
		}
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("[%s,%d] Server Name : %s\n", __FUNCTION__, __LINE__, pstConfig->szServerName );
	HxLOG_Print("[%s,%d] Server Port : %d\n", __FUNCTION__, __LINE__, pstConfig->nPort );
#endif
	fclose(fp);

	return ERR_OK;
}

static HERROR	umma_ftps_start_server( UMMA_FTPS_t *pstFtps )
{
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR szftpsSharedMedia[FTPS_MOUNT_PATH_LEN];
#endif
	if (pstFtps->bIsStarted)
	{
		HxLOG_Warning("[UMMA_FTPS] already started!!\n");
		return ERR_OK;
	}
#if defined(CONFIG_PROD_HGS1000S)
	pstFtps->bIntHddMounted = FALSE;

	HxSTD_snprintf(szftpsSharedMedia, FTPS_MOUNT_PATH_LEN, "%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA);
	if ( 0 == access(szftpsSharedMedia, F_OK) )
			pstFtps->bIntHddMounted = TRUE;
	else
	{
		// Seek hard disk info
		if(umma_ftps_seek_hdd(&pstFtps->bIntHddMounted) == ERR_FAIL)
			return ERR_FAIL;

		if(!pstFtps->bIntHddMounted)
			return ERR_FAIL;

		// Modify /etc/proftpd/ftpd.shared
		if (umma_ftps_set_ftpdSharedConfig() == ERR_FAIL)
			return ERR_FAIL;

		// Update struct user->SharedList to update proftp.conf
		HLIB_LIST_ForeachEx( pstFtps->pstUserList, umma_ftps_update_userSharedList, NULL );
	}

	// Update proftp.conf
	if(umma_ftps_set_config( pstFtps, &pstFtps->stConfig ) == ERR_FAIL)
		return ERR_FAIL;
#else
	umma_ftps_set_config( pstFtps, &pstFtps->stConfig );

	if ( 0 == access(FTPS_SHARED_MEDIA, F_OK) )
		pstFtps->bIntHddMounted = TRUE;
	else
		pstFtps->bIntHddMounted = FALSE;
#endif

	if ( pstFtps->bIntHddMounted )
	{
		/* mount --bind */
		HLIB_LIST_ForeachEx( pstFtps->pstUserList, umma_ftps_cb_mountBind, NULL );
#if defined(CONFIG_PROD_HGS1000S)
	}else
	{
		pstFtps->nRetryCnt = 0;
		umma_ftps_sendMsgToMainTask( pstFtps, eUMMA_FTPS_CMD_RETRY_MOUNT, NULL, 0 );
		return ERR_FAIL;
#endif
	}

	SEUAPI_Start_Ftps(FTPS_CMD_FTPD, FTPS_CONF_PATH);

	pstFtps->bIsStarted = TRUE;
#if defined(CONFIG_PROD_HGS1000S)
#else
	if ( !pstFtps->bIntHddMounted )
	{
		pstFtps->nRetryCnt = 0;
		umma_ftps_sendMsgToMainTask( pstFtps, eUMMA_FTPS_CMD_RETRY_MOUNT, NULL, 0 );
	}
#endif
	HxLOG_Print("[UMMA_FTPS] Server Start!!!!\n");
	return ERR_OK;
}

static HERROR	umma_ftps_stop_server( UMMA_FTPS_t *pstFtps )
{
	UMMA_FTPS_USER_t *pUser = NULL;
	HxList_t *pUserItem = NULL;

	SEUAPI_DoCommand("killall", FTPS_CMD_FTPD);

	pUserItem = pstFtps->pstUserList;
	while ( pUserItem )
	{
		pUser = (UMMA_FTPS_USER_t *)HLIB_LIST_Data(pUserItem);
		/* umount bind path */
		HLIB_LIST_ForeachEx( pUser->pstBindList, umma_ftps_cb_unmountBind, NULL );
		HLIB_LIST_RemoveAllFunc( pUser->pstBindList, umma_ftps_cb_freeBind );
		pUser->pstBindList = NULL;
		/* Folder 삭제는 하지 않는다. Temp에 위차하고 있으며, umount 실패시 HLIB_DIR_Delete을 쓸경우
		 * sub file들까지 삭제될 위험 고려 */
		pUserItem = HLIB_LIST_Next(pUserItem);
	}

	pstFtps->bIsStarted = FALSE;

	HxLOG_Print("[UMMA_FTPS] Server Stop!!!!\n");
	return ERR_OK;
}

static HERROR	umma_ftps_restart_server( UMMA_FTPS_t *pstFtps )
{
	if( !pstFtps->bIsStarted )
		return ERR_FAIL;

	SEUAPI_DoCommand("killall", FTPS_CMD_FTPD);
	VK_TASK_Sleep(1000);

	umma_ftps_set_config( pstFtps, &pstFtps->stConfig );
	SEUAPI_Start_Ftps(FTPS_CMD_FTPD, FTPS_CONF_PATH);

	HxLOG_Print("[UMMA_FTPS] Server Restart!!!!\n");
	return ERR_OK;
}

static HERROR	umma_ftps_isValid_passwd( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, const HCHAR *pszPasswd, HINT32 *pbIsValid )
{
	UMMA_FTPS_USER_t *pUser = NULL;
	if ( 1 )
	{
		HxLOG_Error("[UMMA_FTPS] current not supported. %s()\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pUser = umma_ftps_get_user( pstFtps->pstUserList, pszUserName );
	if ( NULL == pUser )
		return ERR_FAIL;


	return ERR_OK;
}

static HERROR	umma_ftps_add_user( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, const HCHAR *pszPasswd )
{
	UMMA_FTPS_USER_t *pUser = NULL;

	if ( 1 )
	{
		HxLOG_Error("[UMMA_FTPS] current not supported. %s()\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pUser = (UMMA_FTPS_USER_t *)UMMA_Calloc(sizeof(UMMA_FTPS_USER_t));
	HxSTD_StrNCpy( pUser->szName, pszUserName, sizeof(pUser->szName) );

	pstFtps->pstUserList = HLIB_LIST_Append( pstFtps->pstUserList, pUser );

	return ERR_OK;
}

static HERROR	umma_ftps_remove_user( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName )
{
	UMMA_FTPS_USER_t *pUser = NULL;

	if ( 1 )
	{
		HxLOG_Error("[UMMA_FTPS] current not supported. %s()\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pUser = umma_ftps_get_user( pstFtps->pstUserList, pszUserName );
	if ( NULL == pUser )
		return ERR_FAIL;

	pstFtps->pstUserList = HLIB_LIST_Remove( pstFtps->pstUserList, pUser );
	return ERR_OK;
}

static HERROR umma_ftps_update_encrypt_passwd(const HCHAR *pszUserName, const HCHAR *pszPlainPassword)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	return ERR_OK;
#else
	HINT32 nret = (-1);
	struct stat file_st;
	FILE *fp = NULL;
	FILE *new_fp = NULL;
	struct flock rw_lock;
	int new_fd;
	HCHAR *pBuffer = NULL;
	HCHAR buffer[256];
	HLONG fileSize = 0;
	HINT32 uLine = 0;
	HUINT32 uUserLen;
	HBOOL bMatched = FALSE;
	HCHAR *pEncryptedPasswd = NULL;
	HULONG ulSeedNum[2];
	HCHAR szSalt[] = "$1$........";
	const HCHAR szSeedChars[] ="./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	HUINT32 i;
	HBOOL bRestartServer = FALSE;
	UMMA_FTPS_t *pstFtps = umma_ftps_getInstance();
	umma_ftps_releaseInstance(pstFtps);

	/* Get encrypted password by crypt() */
	{
		ulSeedNum[0] = time(NULL);
		ulSeedNum[1] = getpid() ^ (ulSeedNum[0] >> 14 & 0x30000);

		for (i = 0; i < 8; i++)
		{
			szSalt[3+i] = szSeedChars[(ulSeedNum[i/5] >> (i%5)*6) & 0x3f]; /* Borrowed salt generator */
		}

		pEncryptedPasswd = crypt(pszPlainPassword, szSalt);
		if (pEncryptedPasswd == NULL)
		{
			HxLOG_Error("[UMMA_FTPS] Error> crypt(%s) failed!\n", __func__, __LINE__, pszPlainPassword);
		}
	}

	if (0)
	{
		/* Dont restart proftpd deamon because proftpd's password is revised by each client connection */
		if (pstFtps->bIsStarted)
		{
			bRestartServer = TRUE;
			umma_ftps_stop_server( pstFtps ) ;
		}
	}

	/* Change file permission if there is no write(0440) permission */
	nret = stat(FTPS_PASSWD_PATH, &file_st);
	if (-1 == nret)
	{
		HxLOG_Error("[UMMA_FTPS] Error> Cant stat file(%s)!\n", FTPS_PASSWD_PATH);
	}

	if ((file_st.st_mode & S_IWGRP) != S_IWGRP)
	{
		nret = chmod( FTPS_PASSWD_PATH, FTPS_CONFS_WRITE_PERMS);
		if (-1 == nret)
		{
			HxLOG_Error("[UMMA_FTPS] Error> chmod(0x%x) failed... File(%s)\n", __func__, __LINE__, FTPS_CONFS_WRITE_PERMS, FTPS_PASSWD_PATH );
		}

		(void)stat(FTPS_PASSWD_PATH, &file_st);
		if ((file_st.st_mode & S_IWGRP) != S_IWGRP)
		{
			HxLOG_Error("[UMMA_FTPS] Error> No write permission under uid:%d!\n", __func__, __LINE__, getuid());
		}
	}

	fp = fopen(FTPS_PASSWD_PATH, "r");
	if (fp == NULL)
	{
		HxLOG_Error("[UMMA_FTPS] Error> Cant open file(%s)!\n", FTPS_PASSWD_PATH);
		goto func_error;
	}

	fileSize = file_st.st_size;
	uUserLen = HxSTD_StrLen(pszUserName);
	pBuffer = HLIB_STD_MemAlloc(fileSize + 1);
	if(pBuffer == NULL)
	{
		HxLOG_Error("[UMMA_FTPS] Error> Fail to Allocate Memory.\n");
		goto func_error;
	}

	uLine = 0;
	if (fileSize > 1)
	{
		while (fgets(pBuffer, fileSize, fp) != NULL)
		{
			if (HxSTD_StrLen(pBuffer) < HxSTD_StrLen(":::::")) /* minimum length */
			{
				continue;
			}

			// admin:$1$GcGWUAIi$zDJdxAxp1bMOZNALDnOG11:1010:1010::/media/ftps/admin:/bin/false
			if (HxSTD_StrNCmp(pszUserName, pBuffer, uUserLen) == 0)
			{
				const HCHAR *cptr = pBuffer + uUserLen; /* match with "username:"... */
				cptr = HxSTD_StrChar(cptr, ':');
				if (!cptr)
				{
					HxLOG_Error("[UMMA_FTPS] Error> Unexpected line(%s)!\n", pBuffer);
					continue;
				}
				else
				{
					cptr++;
				}
				cptr = HxSTD_StrChar(cptr, ':');
				if (cptr)  {
					cptr++;
				}
				sprintf(buffer, "%s:%s:%s", pszUserName, pEncryptedPasswd, cptr);
				bMatched = TRUE;
			}

			uLine++;
		}
	}
	else
	{
		HxLOG_Error("[UMMA_FTPS] Error> invalid filesize(%d)!\n", fileSize);
		goto func_error;
	}
	fclose(fp);
	fp = NULL;

	/* Write new encrypt password and change file permission with no write(0440) */
	if (bMatched == TRUE)
	{
		HINT32 chk = 0;
		new_fp = fopen(FTPS_PASSWD_PATH, "w");
		if(new_fp == NULL)
		{
			HxLOG_Error("[UMMA_FTPS] Error> Cant open(w) file: %s\n", FTPS_PASSWD_PATH);
			goto func_error;
		}
		new_fd = fileno(new_fp);
		rw_lock.l_type = F_WRLCK;
		rw_lock.l_whence = SEEK_SET;
		rw_lock.l_start = 0;
		rw_lock.l_len = 0;
		if (fcntl(new_fd, F_SETLK, &rw_lock) < 0)
		{
			HxLOG_Error("[UMMA_FTPS] Error> Cant set r/w lock!\n");
		}

		/* TODO: fread/fwrite ���ÿ� �Ͽ� old_fd unlink() �ϵ��� */
		fprintf(new_fp, "%s\n", buffer);
		fflush(new_fp); fsync(new_fd);
		rw_lock.l_type = F_UNLCK;
		chk = fcntl(new_fd, F_SETLK, &rw_lock);
		if(chk)
		{
			HxLOG_Debug("[UMMA_FTPS] fcntl - return(%d).\n", chk);
		}

		fclose(new_fp);
		new_fp = NULL;
	}
	if ((file_st.st_mode & S_IWGRP) == S_IWGRP)
	{
		(void)chmod( FTPS_PASSWD_PATH, FTPS_CONFS_DEFAULT_PERMS);
	}
	if (bRestartServer == TRUE) /* No need to restart daemon for changing password */
	{
		umma_ftps_start_server(pstFtps);
	}

	if (pBuffer)
		HLIB_STD_MemFree(pBuffer);

	return ERR_OK;

func_error:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	if ((file_st.st_mode & S_IWGRP) == S_IWGRP)
	{
		(void)chmod( FTPS_PASSWD_PATH, FTPS_CONFS_DEFAULT_PERMS);
	}
	if (bRestartServer == TRUE)
	{
		umma_ftps_start_server(pstFtps);
	}

	if (pBuffer)
		HLIB_STD_MemFree(pBuffer);
	return ERR_FAIL;
#endif /* CONFIG_PROD_OS_EMULATOR */
}

static HERROR	umma_ftps_change_passwd( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, const HCHAR *pszPasswd, const HCHAR *pszNewPasswd )
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return ERR_OK;
#else
	HCHAR szCmd[128] = { 0 };
	struct stat file_st;

#if 0
	HINT32 isValid = FALSE;
	if ( 1 )
	{
		HxLOG_Error("[UMMA_FTPS] current not supported. %s()\n", __FUNCTION__);
		return ERR_FAIL;
	}

	if ( ERR_OK != umma_ftps_isValid_passwd( pstFtps, pszUserName, pszPasswd, &isValid) )
	{
		HxLOG_Error("[UMMA_FTPS] Error> invalid password!\n");
		goto func_error;
	}
#endif

	/* Check args: userID and newPassword */
	if ((pszUserName == NULL) || (HxSTD_StrLen(pszUserName) == 0))
	{
		HxLOG_Error("[UMMA_FTPS] Error> Invalid username string!\n");
		goto func_error;
	}

	if ((pszNewPasswd == NULL) || (HxSTD_StrLen(pszNewPasswd) < 2))
	{
		HxLOG_Error("[UMMA_FTPS] Error> invalid password string:%s! Too small length!\n", pszUserName);
		goto func_error;
	}

	if (0)
	{
		/* Check here if some password characters limitation is needed to change password */
		if (HxSTD_StrStr((HCHAR *)pszUserName, "<") || HxSTD_StrStr((HCHAR *)pszUserName, ">"))
		{
			HxLOG_Error("[UMMA_FTPS] Error> Cant use string:%s!\n", pszUserName);
			goto func_error;
		}

		if (pszUserName[0] == '0' || pszUserName[0] == '1' || pszUserName[0] == '2' ||
			pszUserName[0] == '3' || pszUserName[0] == '4' ||  pszUserName[0] == '5' ||
			pszUserName[0] == '6' || pszUserName[0] == '7' || pszUserName[0] == '8' || pszUserName[0] == '9')
		{
			HxLOG_Error("[UMMA_FTPS] Error> Cant begin string with num:%s!\n", pszUserName);
			goto func_error;
		}
	}

	/* Is there no passwd file? */
	if (HLIB_FILE_Exist(FTPS_PASSWD_PATH) == FALSE)
	{
		if (1)
		{
			HxLOG_Error("[UMMA_FTPS] Error> No password file(%s)\n", __FUNCTION__, FTPS_PASSWD_PATH);
			return ERR_FAIL;
		}
		else
		{
			HxSTD_snprintf(szCmd, sizeof(szCmd)-1, "touch %s", FTPS_PASSWD_PATH);
			VK_SYSTEM_Command(szCmd);
		}
	}

	if (stat(FTPS_PASSWD_PATH, &file_st) != 0)
	{
		HxLOG_Error("[UMMA_FTPS] Error> Cant stat file:%s!\n", FTPS_PASSWD_PATH);
		return ERR_FAIL;
	}

	/* No need to restart daemon for changing password */
	if (umma_ftps_update_encrypt_passwd(pszUserName, pszNewPasswd) != ERR_OK)
	{
		HxLOG_Error("[UMMA_FTPS] Error> encrypt_passwd() failed!\n");
	}

	return ERR_OK;

func_error:
	return ERR_FAIL;
#endif	/* CONFIG_PROD_OS_EMULATOR */
}

static HERROR	umma_ftps_set_permission( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, const HCHAR *pszPasswd, const UAPI_FTPS_SHARED_t *pstShared )
{
	UMMA_FTPS_USER_t *pUser = NULL;
	UAPI_FTPS_SHARED_t *pShared = NULL;
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

	pUser = umma_ftps_get_user( pstFtps->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}

	HxLOG_Debug("[UMMA_FTS] %s(), bWritabl : %d \n", __FUNCTION__, pstShared->bWritable );

	pSharedItem = HLIB_LIST_FindEx( pUser->pstSharedList, pstShared->szPath, umma_ftps_cb_cmpareBySharedPath );
	if ( NULL == pSharedItem )
	{
		HxLOG_Error("Not found SharedFolder : %s\n", pstShared->szPath );
		return ERR_FAIL;
	}
	pShared = (UAPI_FTPS_SHARED_t*)HLIB_LIST_Data(pSharedItem);

	if ( pShared->bWritable == pstShared->bWritable )
	{
		return ERR_OK;
	}
	pShared->bWritable = pstShared->bWritable;
	/*todo : permission change */
	HxLOG_Debug("[UMMA_FTS] %s(), permission change : %d \n", __FUNCTION__, pstShared->bWritable );

	/* backup shared file */
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s > %s",  FTPS_SHARECONF_PATH, FTPS_SHARECONF_BACKUP_PATH);
	VK_SYSTEM_Command(szCmd);

	fp = fopen( FTPS_SHARECONF_PATH, "w" );
	fp_backup = fopen( FTPS_SHARECONF_BACKUP_PATH, "r" );
	if (NULL == fp || NULL == fp_backup)
	{
		if(fp)
			(void)fclose(fp);
		if(fp_backup)
			(void)fclose(fp_backup);
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
	umma_ftps_restart_server( pstFtps );
	return ERR_OK;
}

static HERROR	umma_ftps_get_sharedList( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, HINT32 *pnCount, UAPI_FTPS_SHARED_t **ppstShared )
{
	UMMA_FTPS_USER_t *pUser = NULL;
	UAPI_FTPS_SHARED_t *pShared = NULL;
	HINT32 nSharedCnt = 0;
	HINT32 i = 0;

	HxList_t *pSharedItem = NULL;

	pUser = umma_ftps_get_user( pstFtps->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}

	nSharedCnt = HLIB_LIST_Length(pUser->pstSharedList);
	if ( nSharedCnt )
		*ppstShared = (UAPI_FTPS_SHARED_t *)HLIB_STD_MemAlloc(sizeof(UAPI_FTPS_SHARED_t)*nSharedCnt);
	else
		return ERR_OK;

	pSharedItem = pUser->pstSharedList;
	while (pSharedItem)
	{
		pShared = (UAPI_FTPS_SHARED_t*)HLIB_LIST_Data(pSharedItem);
		HxSTD_MemCopy( &(*ppstShared)[i], pShared, sizeof(UAPI_FTPS_SHARED_t) );
		pSharedItem = HLIB_LIST_Next(pSharedItem);
		i++;
	}

	*pnCount = nSharedCnt;
	HxLOG_Debug("[UMMA_FTPS] %s() nCount : %d\n", __FUNCTION__, *pnCount);
	return ERR_OK;
}

static HERROR	umma_ftps_set_publicAccess( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, HBOOL bAccess )
{
	UMMA_FTPS_USER_t *pUser = NULL;
	HCHAR line[512] = { 0 };
	HCHAR szCmd[512] = { 0 };
	FILE *fp = NULL;
	FILE *fp_backup = NULL;
	HCHAR szSearch[256] = { 0 };
	HCHAR *pszAttrUser = "<user name=\"";
	HCHAR *pszAttrPulbic = "\" public=\"";
	HCHAR *pszAttrEnd = "\">";
	HCHAR *pTmpStr = NULL;
	pUser = umma_ftps_get_user( pstFtps->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}

	HxLOG_Debug("[UMMA_FTPS] %s() public %d -> %d \n", __FUNCTION__, pUser->bPublic, bAccess );
	if ( pUser->bPublic == bAccess )
		return ERR_OK;

	pUser->bPublic = bAccess;
	/*todo : ftpd.shard change */

	/* backup shared file */
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s > %s",  FTPS_SHARECONF_PATH, FTPS_SHARECONF_BACKUP_PATH);
	VK_SYSTEM_Command(szCmd);

	fp = fopen( FTPS_SHARECONF_PATH, "w" );
	fp_backup = fopen( FTPS_SHARECONF_BACKUP_PATH, "r" );
	if (NULL == fp || NULL == fp_backup)
	{
		if(fp)
			(void)fclose(fp);
		if(fp_backup)
			(void)fclose(fp_backup);
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
	umma_ftps_restart_server( pstFtps );
	return ERR_OK;
}

static HERROR	umma_ftps_get_publicAccess( UMMA_FTPS_t *pstFtps, const HCHAR *pszUserName, HBOOL *pbAccess )
{
	UMMA_FTPS_USER_t *pUser = NULL;

	pUser = umma_ftps_get_user( pstFtps->pstUserList, pszUserName );
	if( NULL == pUser )
	{
		HxLOG_Error("Not found user : %s\n", pszUserName );
		return ERR_FAIL;
	}

	*pbAccess = pUser->bPublic;
	HxLOG_Debug("[UMMA_FTPS] %s() public %d \n", __FUNCTION__, pUser->bPublic );

	return ERR_OK;
}

static void umma_ftps_sendMsgToMainTask( UMMA_FTPS_t *pstFtpsCtx, UMMA_FTPS_CMD_e eCmd, void *data, HINT32 nReqId )
{
	UMMA_FTPS_MSG_t	stMsg = { 0 };
	stMsg.nMessage = (HINT32)eCmd;
	stMsg.nParam1 = (HINT32)data;
	stMsg.nParam2 = (HINT32)nReqId;
	HLIB_MSG_Send(pstFtpsCtx->ulMsgQueue, &stMsg, sizeof(UMMA_FTPS_MSG_t));
}

static UMMA_FTPS_DATA_t * umma_ftps_createMsgData( void *p1, void *p2, void *p3 )
{
	UMMA_FTPS_DATA_t *pstMsgData = NULL;
	pstMsgData = (UMMA_FTPS_DATA_t *)HLIB_STD_MemAlloc(sizeof(UMMA_FTPS_DATA_t));
	pstMsgData->nParam1 = (HINT32)p1;
	pstMsgData->nParam2 = (HINT32)p2;
	pstMsgData->nParam3 = (HINT32)p3;

	return pstMsgData;
}

static void umma_ftps_releaseMsgData( UMMA_FTPS_DATA_t **ppstMsgData )
{
	UMMA_FTPS_DATA_t *pstMsgData = *ppstMsgData;

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

static inline void umma_ftps_set_notifyData( UMMA_FTPS_DATA_t *pstNotiData, HINT32 nErr, void *pvParam1, void *pvParam2)
{
	pstNotiData->nParam1 = nErr;
	pstNotiData->nParam2 = (HINT32)pvParam1;
	pstNotiData->nParam3 = (HINT32)pvParam2;
}

static HERROR umma_ftps_init( UMMA_FTPS_t *pstFtps )
{
	HERROR ret = ERR_FAIL;

	if ( pstFtps->bInit )
	{
		HxLOG_Debug("[UMMA_FTPS] Init is already. %s()\n", __FUNCTION__);
		return ERR_OK;
	}

	umma_ftps_init_config( &pstFtps->stConfig );

	ret = umma_ftps_check_configPath();
	if ( ERR_OK != ret )
	{
		HxLOG_Error( "[UMMA_FTPS] umma_ftps_check_configPath() - Error(%d)\n", ret );
		return ERR_FAIL;
	}

	ret = umma_ftps_check_configFile();
	if ( ERR_OK != ret )
	{
		HxLOG_Error( "[UMMA_FTPS] umma_ftps_check_configFile() - Error(%d)\n", ret );
		return ERR_FAIL;
	}

	umma_ftps_init_users( &pstFtps->pstUserList );

	pstFtps->bInit = TRUE;
	return ERR_OK;
}

#define	______________RPC_API____________________
static HERROR	umma_ftps_rpc_serverStart(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;

	UMMA_ASSERT(nArgc == 0);

	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_SERVER_START, NULL, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_serverStop(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;

	UMMA_ASSERT(nArgc == 0);

	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_SERVER_STOP, NULL, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_setConfig(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	UAPI_FTPS_CONF_t *pConfig = NULL;
	UMMA_FTPS_DATA_t *pData = NULL;

	UMMA_ASSERT(nArgc == 1);

	pConfig = (UAPI_FTPS_CONF_t *)HLIB_STD_MemAlloc(sizeof(UAPI_FTPS_CONF_t));
	pData = umma_ftps_createMsgData( pConfig, NULL, NULL );

	HxSTD_MemCopy( pConfig, HxOBJECT_BIN_DATA(apArgv[0]), sizeof(UAPI_FTPS_CONF_t) );

	HxLOG_Debug("setConfig : %s, %d\n", pConfig->szServerName, pConfig->nPort);
	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_SET_CONFIG, pData, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_getServerInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;

	UMMA_ASSERT(nArgc == 0);

	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_GET_SERVERINFO, NULL, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_setPasswd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	UMMA_FTPS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	HCHAR *pPasswd = NULL;
	HCHAR *pNewPasswd = NULL;

	UMMA_ASSERT(nArgc == 3);

	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pPasswd = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pNewPasswd = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pData = umma_ftps_createMsgData( pUserName, pPasswd, pNewPasswd );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	HxSTD_StrNCpy( pPasswd, HxOBJECT_STR(apArgv[1]), 128-1);
	HxSTD_StrNCpy( pNewPasswd, HxOBJECT_STR(apArgv[2]), 128-1);

	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_SET_PASSWD, pData, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_checkPasswd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	//UMMA_FTPS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	HCHAR *pPasswd = NULL;
	HINT32 isValid = FALSE;
	HERROR	ret = ERR_FAIL;

	UMMA_ASSERT(nArgc == 2);

	pUserName = HxOBJECT_STR(apArgv[0]);
	pPasswd = HxOBJECT_STR(apArgv[1]);

	pFtps = umma_ftps_getInstance();
	umma_ftps_releaseInstance(pFtps);
#if 0
	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pPasswd = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pData = umma_ftps_createMsgData( pUserName, pPasswd, nClientHandle );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	HxSTD_StrNCpy( pPasswd, HxOBJECT_STR(apArgv[1]), 128-1);

	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_CHK_PASSWD, pData, nRequestId );
#endif

	ret = umma_ftps_isValid_passwd( pFtps, pUserName, pPasswd, &isValid );
	HLIB_RPC_Response(pFtps->nRpcHandle, nClientHandle, nRequestId, ret, "i", isValid);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_setPermission(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	UMMA_FTPS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	UAPI_FTPS_SHARED_t *pShared = NULL;

	UMMA_ASSERT(nArgc == 2);

	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pShared = (UAPI_FTPS_SHARED_t *)HLIB_STD_MemAlloc(sizeof(UAPI_FTPS_SHARED_t));
	pData = umma_ftps_createMsgData( pUserName, pShared, NULL );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	HxSTD_MemCopy( pShared, HxOBJECT_BIN_DATA(apArgv[1]), sizeof(UAPI_FTPS_SHARED_t));

	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_SET_PERMISSION, pData, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_getSharedList(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	HCHAR *pUserName = NULL;
	HINT32 nCnt = 0;
	UAPI_FTPS_SHARED_t *pSharedList = NULL;
	HERROR	ret = ERR_FAIL;

	UMMA_ASSERT(nArgc == 1);

	pUserName = HxOBJECT_STR(apArgv[0]);

	pFtps = umma_ftps_getInstance();
	umma_ftps_releaseInstance(pFtps);

	ret = umma_ftps_get_sharedList( pFtps, pUserName, &nCnt, &pSharedList );

	if (nCnt >0 )
	{
		HLIB_RPC_Response(pFtps->nRpcHandle, nClientHandle, nRequestId, ret, "b", pSharedList, sizeof(UAPI_FTPS_SHARED_t) * nCnt);
	}
	else
	{
		HINT32 tmp = 0;

		HLIB_RPC_Response(pFtps->nRpcHandle, nClientHandle, nRequestId, ret, "b", &tmp, sizeof(HINT32));
	}

	if ( pSharedList )
		HLIB_STD_MemFree(pSharedList);
	return ERR_OK;
}

static HERROR	umma_ftps_rpc_setPublicAccess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	UMMA_FTPS_DATA_t *pData = NULL;
	HCHAR *pUserName = NULL;
	HINT32 *pbAccess = NULL;

	UMMA_ASSERT(nArgc == 2);

	pUserName = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR)*128);
	pbAccess = (HINT32 *)HLIB_STD_MemAlloc(sizeof(HINT32));

	pData = umma_ftps_createMsgData( pUserName, pbAccess, NULL );

	HxSTD_StrNCpy( pUserName, HxOBJECT_STR(apArgv[0]), 128-1);
	*pbAccess =  HxOBJECT_INT(apArgv[1]) ? TRUE : FALSE;

	pFtps = umma_ftps_getInstance();
	umma_ftps_sendMsgToMainTask( pFtps, eUMMA_FTPS_CMD_SET_PUBLICACCESS, pData, nRequestId );
	umma_ftps_releaseInstance(pFtps);

	return ERR_OK;
}

static HERROR	umma_ftps_rpc_getPublicAccess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	UMMA_FTPS_t	*pFtps;
	HCHAR *pUserName = NULL;
	HBOOL bAccess = FALSE;
	HERROR	ret = ERR_FAIL;

	UMMA_ASSERT(nArgc == 1);

	pUserName = HxOBJECT_STR(apArgv[0]);

	pFtps = umma_ftps_getInstance();
	umma_ftps_releaseInstance(pFtps);

	ret = umma_ftps_get_publicAccess( pFtps, pUserName, &bAccess );
	HLIB_RPC_Response(pFtps->nRpcHandle, nClientHandle, nRequestId, ret, "i", bAccess);
	return ERR_OK;
}


#define __________Public_Function__________
HERROR	UMMA_FTPS_Init(void)
{
	UMMA_FTPS_t *pstFtps = umma_ftps_getInstance();

	HxLOG_Trace();

	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_Start, NULL, umma_ftps_rpc_serverStart,
		NULL, "FTP Server Start: (void)");
	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_Stop, NULL, umma_ftps_rpc_serverStop,
		NULL, "FTP Server Stop: (void)");

	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_SetConfig, "b", umma_ftps_rpc_setConfig,
		NULL, "FTP Server Set Config: (UAPI_FTPS_CONF_t *pstConfig)");
	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_GetServerInfo, NULL, umma_ftps_rpc_getServerInfo,
		NULL, "FTP Server Get Info: (void)");

	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_SetPasswd, "sss", umma_ftps_rpc_setPasswd,
		NULL, "FTP Server Set Password: (HCHAR *pszUserName, HCHAR *pszPasswd, HCHAR *pszNewPasswd)");

	HLIB_RPC_RegisterRequest(pstFtps->nRpcHandle, RPC_UAPI_FTPS_CheckPasswd, "ss", umma_ftps_rpc_checkPasswd,
		NULL, "FTP Server check password: (HCHAR *pszUserName, HCHAR *pszPasswd)");

	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_SetPermission, "sb", umma_ftps_rpc_setPermission,
		NULL, "FTP Server Set Permission: (HCHAR *pszUserName, UAPI_FTPS_SHARED_t *pstShared)");
	HLIB_RPC_RegisterRequest(pstFtps->nRpcHandle, RPC_UAPI_FTPS_GetSharedList, "s", umma_ftps_rpc_getSharedList,
		NULL, "FTP Server Get SharedList: (HCHAR *pszUserName)");
	HLIB_RPC_RegisterCall(pstFtps->nRpcHandle, RPC_UAPI_FTPS_SetPublicAccess, "si", umma_ftps_rpc_setPublicAccess,
		NULL, "FTP Server Set Public Access: (HCHAR *pszUserName, UAPI_FTPS_SHARED_t *pstShared)");
	HLIB_RPC_RegisterRequest(pstFtps->nRpcHandle, RPC_UAPI_FTPS_GetPublicAccess, "s",
			umma_ftps_rpc_getPublicAccess, NULL, "FTP Server Get Public Access: (HCHAR *pszUserName)");

	HLIB_RPC_CreateNotification(pstFtps->nRpcHandle, RPC_UMMA_FTPS_onRequestResult, "iibb",
			"notify for Server Resopne (HINT32 nRequestId, UAPI_STATUS_t *pstStatus)");
	umma_ftps_releaseInstance(pstFtps);

	umma_ftps_init( pstFtps );

	HAPI_ConnectSignal("signal:onFormatOccurred", NULL, umma_ftps_signal_onFormatOccurred);
	HAPI_ConnectSignal("signal:onFactoryDefault", NULL, umma_ftps_signal_onFactoryDefault);
#if defined(CONFIG_DEBUG)
	umma_ftps_debug_InitCommandTools();
#endif
	return 0;
}

HERROR	UMMA_FTPS_Start(void)
{
	UMMA_FTPS_t *pstFtps = umma_ftps_getInstance();
	umma_ftps_releaseInstance(pstFtps);
	return umma_ftps_start_server( pstFtps ) ;
}

HERROR	UMMA_FTPS_Stop(void)
{
	UMMA_FTPS_t *pstFtps = umma_ftps_getInstance();
	umma_ftps_releaseInstance(pstFtps);
	return umma_ftps_stop_server( pstFtps );
}

#define	______________API____________________

static void		umma_ftps_mainLoop(void *arg)
{
	UMMA_FTPS_t 		*pstFtps = (UMMA_FTPS_t *)arg;
	UMMA_FTPS_MSG_t		stMsg = { 0 };
	UMMA_FTPS_DATA_t	*pRcvData = NULL;
	UMMA_FTPS_DATA_t	stSendData;
	HINT32		nReqId = 0;
	HERROR		ret = ERR_FAIL;
#if defined(CONFIG_PROD_HGS1000S)
	HCHAR		szftpsSharedMedia[FTPS_MOUNT_PATH_LEN];
#endif
	while (1)
	{
		if ( VK_OK != HLIB_MSG_Receive(pstFtps->ulMsgQueue, &stMsg, sizeof(UMMA_FTPS_MSG_t)) )
		{
			VK_TASK_Sleep(100);
			continue;
		}

		pRcvData = (UMMA_FTPS_DATA_t *)stMsg.nParam1;
		nReqId = (HINT32)stMsg.nParam2;
		HxSTD_MemSet( &stSendData, 0x00, sizeof(UMMA_FTPS_DATA_t) );
		switch ( stMsg.nMessage )
		{
			case  eUMMA_FTPS_CMD_SERVER_START :
				ret = umma_ftps_start_server( pstFtps );
				umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
				umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_SERVER_START, &stSendData, nReqId );
				break;
			case  eUMMA_FTPS_CMD_SERVER_STOP :
				ret = umma_ftps_stop_server( pstFtps );
				umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
				umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_SERVER_STOP, &stSendData, nReqId );
				break;
			case  eUMMA_FTPS_CMD_SET_CONFIG :
				{
					UAPI_FTPS_CONF_t *pConf = (UAPI_FTPS_CONF_t*)pRcvData->nParam1;
					ret = umma_ftps_set_config( pstFtps, pConf);
					umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_CONFIG_CHANGE, &stSendData, nReqId );
				}
				break;
			case  eUMMA_FTPS_CMD_GET_SERVERINFO :
				{
					UAPI_FTPS_CONF_t stConf;
					ret = umma_ftps_get_config( &stConf );
					umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_RESPONE_SERVERINFO, &stSendData, nReqId );
				}
				break;
			case  eUMMA_FTPS_CMD_SET_PASSWD :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HCHAR *pPasswd = (HCHAR *)pRcvData->nParam2;
					HCHAR *pNewPasswd = (HCHAR *)pRcvData->nParam3;

					ret = umma_ftps_change_passwd( pstFtps, pUserName, pPasswd, pNewPasswd );
					umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_PASSWD_CHANGE, &stSendData, nReqId );
				}
				break;
			case  eUMMA_FTPS_CMD_CHK_PASSWD :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HCHAR *pPasswd = (HCHAR *)pRcvData->nParam2;
					HINT32 isValid = FALSE;
					ret = umma_ftps_isValid_passwd( pstFtps, pUserName, pPasswd, &isValid );
					umma_ftps_set_notifyData( &stSendData, ret, (void *)isValid, NULL );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_RESPONE_CHKPASSWD, &stSendData, nReqId );
				}
				break;
			case  eUMMA_FTPS_CMD_SET_PERMISSION :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					UAPI_FTPS_SHARED_t *pShared = (UAPI_FTPS_SHARED_t *)pRcvData->nParam2;
					ret = umma_ftps_set_permission( pstFtps, pUserName, "", pShared );
					umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_PERMISSION_CHANGE, &stSendData, nReqId );
				}
				break;
			case  eUMMA_FTPS_CMD_GET_SHAREDLIST :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HINT32 nCnt = 0;
					HINT32 nClientHandle = pRcvData->nParam3;
					UAPI_FTPS_SHARED_t *pSharedList = NULL;

					ret = umma_ftps_get_sharedList( pstFtps, pUserName, &nCnt, &pSharedList );

					if(nCnt)
					{
						HLIB_RPC_Response(pstFtps->nRpcHandle, nClientHandle, nReqId, ret, "b", pSharedList,sizeof(UAPI_FTPS_SHARED_t) * nCnt);
					}
					else
					{
						HINT32 tmp = 0;
						HLIB_RPC_Response(pstFtps->nRpcHandle, nClientHandle, nReqId, ret, "b", &tmp, sizeof(HINT32));
					}
#if 0
					umma_ftps_set_notifyData( &stSendData, ret, (void *)nCnt, (void *)pSharedList );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_RESPONE_SHAREDLIST, &stSendData, nReqId );
#endif
					if ( pSharedList )
						HLIB_STD_MemFree(pSharedList);
				}
				break;
			case  eUMMA_FTPS_CMD_SET_PUBLICACCESS :
				{
					HCHAR *pUserName = (HCHAR *)pRcvData->nParam1;
					HINT32 *pAccess = (HINT32 *)pRcvData->nParam2;
					ret = umma_ftps_set_publicAccess( pstFtps, pUserName, *pAccess ? TRUE : FALSE );
					umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
					umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_PERMISSION_CHANGE, &stSendData, nReqId );
				}
				break;
			case eUMMA_FTPS_CMD_RETRY_MOUNT :
				HxLOG_Debug("[UMMA_FTPS] retry mount...(%d)\n",pstFtps->nRetryCnt );
				++pstFtps->nRetryCnt;
				if ( pstFtps->bIsStarted && !pstFtps->bIntHddMounted )
				{
					VK_TASK_Sleep(3000);
#if defined(CONFIG_PROD_HGS1000S)
					HxSTD_snprintf (szftpsSharedMedia, FTPS_MOUNT_PATH_LEN, "%s%s", s_szftpsMountPath, FTPS_SHARED_MEDIA);
					if ( 0 == access(szftpsSharedMedia, F_OK) )
#else
					if ( 0 == access(FTPS_SHARED_MEDIA, F_OK) )
#endif
					{
						pstFtps->bIntHddMounted = TRUE;
						/* mount --bind */
						HLIB_LIST_ForeachEx( pstFtps->pstUserList, umma_ftps_cb_mountBind, NULL );
					}
					else
					{
#if defined(CONFIG_PROD_HGS1000S)
						umma_ftps_seek_hdd(&pstFtps->bIntHddMounted);
						if(!pstFtps->bIntHddMounted)
#endif
							umma_ftps_sendMsgToMainTask( pstFtps, eUMMA_FTPS_CMD_RETRY_MOUNT, NULL, 0 );
					}
				}
				break;


			default :
				break;
		}

		umma_ftps_releaseMsgData( &pRcvData );
	}
}

#if defined(CONFIG_DEBUG)
static int	umma_ftps_debug_cmdActionTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR subStr[20] = { 0 };
	HBOOL bResEvt = FALSE;
	UMMA_FTPS_DATA_t	stSendData;
	HERROR		ret = ERR_FAIL;

	UMMA_FTPS_t *pstFtps;


	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)aucStr) == 0 )   	return HxCMD_ERR;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if ( aucArg )
	{
		sscanf(aucArg, "%s", (char *)subStr);
		HLIB_CMD_Printf("[FTPS_CMD] subCmd.\n", subStr);
		HLIB_STD_StrUpper(subStr);
		if ( !HxSTD_StrCmp( "EVT", subStr ) )
			bResEvt = TRUE;
	}

	pstFtps = umma_ftps_getInstance();
	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[FTPS_CMD] Server %s, Event : %s\n", aucStr, bResEvt ? "TRUE":"FLASE");
	if ( !HxSTD_StrCmp( "START", aucStr ) )
	{
		umma_ftps_init_config( &pstFtps->stConfig );
		umma_ftps_set_config( pstFtps, &pstFtps->stConfig );
		ret = umma_ftps_start_server( pstFtps );
		umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_SERVER_START, &stSendData, 0 );

	}
	else if ( !HxSTD_StrCmp( "STOP", aucStr ) )
	{
		ret = umma_ftps_stop_server( pstFtps );
		umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_SERVER_STOP, &stSendData, 0 );
	}
	else if ( !HxSTD_StrCmp( "RESTART", aucStr ) )
	{
		ret = umma_ftps_stop_server( pstFtps );
		umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_SERVER_STOP, &stSendData, 0 );
		umma_ftps_start_server( pstFtps );
		umma_ftps_set_notifyData( &stSendData, ret, NULL, NULL );
		if ( bResEvt )
			umma_ftps_notifyEventToClient( pstFtps, eUAPI_FTPS_EVT_SERVER_START, &stSendData, 0 );
	}
	else
	{
		HLIB_CMD_Printf("[FTPS_CMD] %s is not supported.\n", aucStr);
	}
	umma_ftps_releaseInstance(pstFtps);
	return HxCMD_OK;
}

static int	umma_ftps_debug_cmdConfgTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szCmd[128] = { 0 };

	UMMA_FTPS_t *pstFtps;


	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[FTPS_CMD] %s Config.\n", aucStr);
	if ( !HxSTD_StrCmp( "NEW", aucStr ) )
	{
		pstFtps = umma_ftps_getInstance();
		umma_ftps_init_config( &pstFtps->stConfig );
		umma_ftps_check_configPath();
		umma_ftps_check_configFile();
		umma_ftps_set_config( pstFtps, &pstFtps->stConfig );

		HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", FTPS_CONF_PATH );
		VK_SYSTEM_Command (szCmd);
		umma_ftps_releaseInstance(pstFtps);
	}
	else if ( !HxSTD_StrCmp( "SHOW", aucStr ) )
	{
		HLIB_CMD_Printf("[FTPS_CMD] show : %s\n", FTPS_CONF_PATH);
		HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", FTPS_CONF_PATH );
		VK_SYSTEM_Command (szCmd);
		HLIB_CMD_Printf("\n[FTPS_CMD] show : %s\n", FTPS_USERS_PATH);
		HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", FTPS_USERS_PATH );
		VK_SYSTEM_Command (szCmd);
		HLIB_CMD_Printf("\n[FTPS_CMD] show : %s\n", FTPS_PASSWD_PATH);
		HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", FTPS_PASSWD_PATH );
		VK_SYSTEM_Command (szCmd);
	}
	else if ( !HxSTD_StrCmp( "DEL", aucStr ) )
	{
		HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", FTPS_RW_LINK_PATH );
		VK_SYSTEM_Command (szCmd);
		HxSTD_snprintf( szCmd, sizeof(szCmd), "ls -al %s", "/etc" );
		VK_SYSTEM_Command (szCmd);
	}
	else
	{
		HLIB_CMD_Printf("[FTPS_CMD] %s is not supported.\n", aucStr);
	}

	return HxCMD_OK;
}

static int	umma_ftps_debug_cmdPasswordTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };

	UMMA_FTPS_t *pstFtps;


	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_CMD_Printf("[FTPS_CMD] New Passwd:%s\n", aucStr);
	if ( HxSTD_StrLen(aucStr) )
	{
		pstFtps = umma_ftps_getInstance();
		umma_ftps_change_passwd(pstFtps, "admin", "", aucStr);
		umma_ftps_releaseInstance(pstFtps);
	}
	else
	{
		HLIB_CMD_Printf("[FTPS_CMD] %s is not supported.\n", aucStr);
	}

	return HxCMD_OK;
}

static int	umma_ftps_debug_cmdPublicTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szCmd[128] = { 0 };
	HCHAR szUser[128] = { 0 };
	HBOOL bAccess = FALSE;
	HERROR ret = ERR_FAIL;
	UMMA_FTPS_t *pstFtps;

	HAPPY(ret);


	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_CMD_Printf("[FTPS_CMD] public info. user :%s\n", aucStr);
	HxSTD_StrNCpy( szUser, aucStr, sizeof(szUser)-1 );

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;
	HLIB_STD_StrUpper(aucStr);
	pstFtps = umma_ftps_getInstance();
	if ( !HxSTD_StrCmp( "TRUE", aucStr ) )
	{
		bAccess = TRUE;
		HLIB_CMD_Printf("[FTPS_CMD] public set : %s\n", aucStr);
		ret = umma_ftps_set_publicAccess( pstFtps, szUser, bAccess );
	}
	else if ( !HxSTD_StrCmp( "FALSE", aucStr ) )
	{
		HLIB_CMD_Printf("[FTPS_CMD] public set : %s\n", aucStr);
		ret = umma_ftps_set_publicAccess( pstFtps, szUser, bAccess );
	}
	else if ( !HxSTD_StrCmp( "PRINT", aucStr ) )
	{
		umma_ftps_get_publicAccess( pstFtps, szUser, &bAccess );
		HLIB_CMD_Printf("[FTPS_CMD] public Access : %s \n", bAccess ? "true":"false");
	}
	umma_ftps_releaseInstance(pstFtps);

	HLIB_CMD_Printf("[FTPS_CMD] cmd : %s, print :%s\n",aucArg, FTPS_SHARECONF_PATH );
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", FTPS_SHARECONF_PATH );
	VK_SYSTEM_Command (szCmd);
	return HxCMD_OK;
}

static int	umma_ftps_debug_cmdPermsTest(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HCHAR szCmd[128] = { 0 };
	HCHAR szUser[128] = { 0 };
	HBOOL bRw = FALSE;
	HINT32 nCount = 0;
	HINT32 i = 0;
	UAPI_FTPS_SHARED_t *pSharedList = NULL;
	UAPI_FTPS_SHARED_t stShared;
	HERROR ret = ERR_FAIL;
	UMMA_FTPS_t *pstFtps;

	HAPPY(ret);

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_CMD_Printf("[FTPS_CMD] public info. user :%s\n", aucStr);
	HxSTD_StrNCpy( szUser, aucStr, sizeof(szUser)-1 );

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;
	HLIB_STD_StrUpper(aucStr);

	pstFtps = umma_ftps_getInstance();
	umma_ftps_get_sharedList ( pstFtps, szUser, &nCount, &pSharedList );
	umma_ftps_releaseInstance(pstFtps);
	if ( !HxSTD_StrCmp( "RW", aucStr ) )
	{
		HLIB_CMD_Printf("[FTPS_CMD] Permission set : %s\n", aucStr);
		bRw = TRUE;

	}
	else if ( !HxSTD_StrCmp( "RO", aucStr ) )
	{
		HLIB_CMD_Printf("[FTPS_CMD] Permission set : %s\n", aucStr);
		bRw = FALSE;
	}
	else
	{
		if ( pSharedList )
			HLIB_STD_MemFree(pSharedList);
		return HxCMD_ERR;
	}

	pstFtps = umma_ftps_getInstance();
	for ( i = 0; i< nCount; ++i )
	{
		HxSTD_MemCopy( &stShared, &pSharedList[i], sizeof(UAPI_FTPS_SHARED_t) );
		stShared.bWritable = bRw;
		ret = umma_ftps_set_permission( pstFtps, szUser, "",  &stShared );
	}
	umma_ftps_releaseInstance(pstFtps);

	HLIB_CMD_Printf("[FTPS_CMD] cmd : %s, print :%s\n",aucArg, FTPS_SHARECONF_PATH );
	HxSTD_snprintf( szCmd, sizeof(szCmd), "cat %s", FTPS_SHARECONF_PATH );
	VK_SYSTEM_Command (szCmd);
	if ( pSharedList )
		HLIB_STD_MemFree(pSharedList);
	return HxCMD_OK;
}

static int	umma_ftps_debug_cmdPrintInfo(void *szArgs)
{
	HCHAR    *aucArg=NULL, aucStr[20] = { 0 };
	HINT32 i = 0;

	UMMA_FTPS_t *pstFtps;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
	HLIB_CMD_Printf("[FTPS_CMD] print info :%s\n", aucStr);
	pstFtps = umma_ftps_getInstance();
	if ( !HxSTD_StrCmp( "USER", aucStr ) )
	{
		HxList_t *pUserItem = pstFtps->pstUserList;
		UMMA_FTPS_USER_t *pUser = NULL;
		HLIB_CMD_Printf("[FTPS_CMD] USER Count : %2d\n", HLIB_LIST_Length(pstFtps->pstUserList ));
		while ( pUserItem )
		{
			pUser = (UMMA_FTPS_USER_t *)HLIB_LIST_Data(pUserItem);
			HLIB_CMD_Printf("[FTPS_CMD] %2d. User\n", i);
			HLIB_CMD_Printf("[FTPS_CMD]   szName : %s \n", pUser->szName);
			HLIB_CMD_Printf("[FTPS_CMD]   bPublic : %s\n", pUser->bPublic ? "TRUE":"FALSE" );
			HLIB_CMD_Printf("[FTPS_CMD]   szHomePath: %s\n", pUser->szHomePath);
			i++;
			pUserItem = HLIB_LIST_Next(pUserItem);
		}
	}
	else if ( !HxSTD_StrCmp( "SHARED", aucStr ) )
	{
		HCHAR szUserName[128] = { 0 };
		HINT32 nCount = 0;
		UAPI_FTPS_SHARED_t *pSharedList = NULL;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
		if ( !aucArg )
		{
			return HxCMD_OK;
		}
		sscanf(aucArg, "%s", (char *)szUserName);
		umma_ftps_get_sharedList ( pstFtps, szUserName, &nCount, &pSharedList );
		HLIB_CMD_Printf("[FTPS_CMD] %s Shared Count : %2d\n", szUserName, nCount);
		for ( i = 0; i<nCount; ++i )
		{
			HLIB_CMD_Printf("[FTPS_CMD] %2d. Share Folder\n", i);
			HLIB_CMD_Printf("[FTPS_CMD]   szName : %s \n", pSharedList[i].szName);
			HLIB_CMD_Printf("[FTPS_CMD]   szPath : %s \n", pSharedList[i].szPath);
			HLIB_CMD_Printf("[FTPS_CMD]   bWritable : %s \n", pSharedList[i].bWritable ? "TRUE" : "FALSE");
			HLIB_CMD_Printf("[FTPS_CMD]   bMounted : %s \n", pSharedList[i].bMounted ? "TRUE" : "FALSE");
		}

		if ( pSharedList )
			HLIB_STD_MemFree(pSharedList);
	}
	umma_ftps_releaseInstance(pstFtps);
	return HxCMD_OK;
}

static void		umma_ftps_debug_InitCommandTools(void)
{
	HLIB_CMD_RegisterWord("ftps", umma_ftps_debug_cmdActionTest,
						"act",
						"ftp server action",
						"act [start|stop|restart] [evt| ]");
	HLIB_CMD_RegisterWord("ftps", umma_ftps_debug_cmdConfgTest,
						"conf",
						"conf ftp server",
						"conf [new|del|show]");
	HLIB_CMD_RegisterWord("ftps", umma_ftps_debug_cmdPasswordTest,
						"passwd",
						"ftp server passwd",
						"passwd [arg]");
	HLIB_CMD_RegisterWord("ftps", umma_ftps_debug_cmdPublicTest,
						"public",
						"ftp server public",
						"public [username] [true|false|print]");
	HLIB_CMD_RegisterWord("ftps", umma_ftps_debug_cmdPermsTest,
						"perms",
						"ftp server permission",
						"perms [username] [true|false|print]");
	HLIB_CMD_RegisterWord("ftps", umma_ftps_debug_cmdPrintInfo,
						"print",
						"ftp server info",
						"print [user|shared]");
}
#endif


