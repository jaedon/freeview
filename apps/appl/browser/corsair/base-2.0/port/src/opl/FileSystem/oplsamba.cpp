/*************************************************************************************************************
	File 		: oplsamba.cpp
	author 		: WEB TFT tft_webapp@humaxdigital.com
	comment		:
	date    	: 2012/12/12
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Â¡Â°HumaxÂ¡Â±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED Â¡Â°AS ISÂ¡Â± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/
#include "bpldb.h"
#include "oplsamba.h"
#include <apk.h>
#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

#define DOT_OR_DOTDOT(_basename)					\
	((_basename[0] == '.') &&					\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))

#define ENTRY					HxLOG_Debug("%s : %s : +++++++++++++++++++++++++++++\n", __FILE__ , __FUNCTION__);
#define LEAVE					HxLOG_Debug("%s : %s : -----------------------------\n", __FILE__ , __FUNCTION__);
#define SAFE_DELETE(p)			{if(p){ HLIB_STD_MemFree(p); (p)=NULL;}}
#define SAFE_DELETE_ARRAY(p) 	{if(p){ HLIB_STD_MemFree[](p); (p)=NULL;}}

OplSambaManagerCallback			g_pSambaClientListener 	= NULL;
ApkFileMgr_SMBSharedFolderInfo_t	g_stSharedFolderInfo;

/****************************************************************************************
 * OplSharedFolder
 ***************************************************************************************/
OplSharedFolder::OplSharedFolder( OplSharedFolderHandle Handle)
{
	ApkFileMgr_SMBSharedFolderInfo_t* pInstance = (ApkFileMgr_SMBSharedFolderInfo_t*)Handle;
	HxSTD_MemSet(m_szServerName, 	0x00, sizeof(m_szServerName));
	HxSTD_MemSet(m_szSharedName, 	0x00, sizeof(m_szSharedName));
	HxSTD_MemSet(m_szIp, 			0x00, sizeof(m_szIp));
	HxSTD_MemSet(m_szId, 			0x00, sizeof(m_szId));
	HxSTD_MemSet(m_szPassword, 		0x00, sizeof(m_szPassword));
	HxSTD_MemSet(m_szMountPath, 	0x00, sizeof(m_szMountPath));
	HxSTD_MemSet(m_szLogMsg, 		0x00, sizeof(m_szLogMsg));

	m_bMount = -1;
	m_nState = -1;

	if(NULL == Handle) return;

	HxSTD_PrintToStrN(m_szServerName, 	sizeof(m_szServerName), "%s", pInstance->szServerName);
	HxSTD_PrintToStrN(m_szSharedName, 	sizeof(m_szSharedName), "%s", pInstance->szSharedName);
	HxSTD_PrintToStrN(m_szIp, 			sizeof(m_szIp), 		"%s", pInstance->szIp);
	HxSTD_PrintToStrN(m_szId, 			sizeof(m_szId), 		"%s", pInstance->szId);
 	HxSTD_PrintToStrN(m_szPassword, 	sizeof(m_szPassword), 	"%s", pInstance->szPassword);
 	HxSTD_PrintToStrN(m_szMountPath, 	sizeof(m_szMountPath),	"%s", pInstance->szMountPath);
 	HxSTD_PrintToStrN(m_szLogMsg, 		sizeof(m_szLogMsg),		"%s", pInstance->szLogMsg);
	m_bMount = pInstance->bMounted;
	m_nState = pInstance->nState;
}

OplSharedFolder::OplSharedFolder( const OplSharedFolder &original )
{
	HxSTD_MemSet(m_szServerName, 	0x00, sizeof(m_szServerName));
	HxSTD_MemSet(m_szSharedName, 	0x00, sizeof(m_szSharedName));
	HxSTD_MemSet(m_szIp, 			0x00, sizeof(m_szIp));
	HxSTD_MemSet(m_szId, 			0x00, sizeof(m_szId));
	HxSTD_MemSet(m_szPassword, 		0x00, sizeof(m_szPassword));
	HxSTD_MemSet(m_szMountPath, 	0x00, sizeof(m_szMountPath));
	HxSTD_MemSet(m_szLogMsg, 		0x00, sizeof(m_szLogMsg));
	m_bMount = -1;
	m_nState = -1;

	if(NULL == &original) return;

	HxSTD_PrintToStrN(m_szServerName, 	sizeof(m_szServerName), "%s", original.m_szServerName);
	HxSTD_PrintToStrN(m_szSharedName, 	sizeof(m_szSharedName), "%s", original.m_szSharedName);
	HxSTD_PrintToStrN(m_szIp, 			sizeof(m_szIp), 		"%s", original.m_szIp);
	HxSTD_PrintToStrN(m_szId, 			sizeof(m_szId), 		"%s", original.m_szId);
 	HxSTD_PrintToStrN(m_szPassword, 	sizeof(m_szPassword), 	"%s", original.m_szPassword);
 	HxSTD_PrintToStrN(m_szMountPath, 	sizeof(m_szMountPath),	"%s", original.m_szMountPath);
 	HxSTD_PrintToStrN(m_szLogMsg, 		sizeof(m_szLogMsg),		"%s", original.m_szLogMsg);

	m_bMount = original.m_bMount;
	m_nState = original.m_nState;
}

OplSharedFolder::~OplSharedFolder( void )
{

}

void OplSharedFolder::print_SharedFolder_Info()
{
	ENTRY;

	HxLOG_Debug("SERVER_NAME  : %s \n", m_szServerName);
	HxLOG_Debug("SHARED_NAME  : %s \n", m_szSharedName);
	HxLOG_Debug("IP           : %s \n", m_szIp);
	HxLOG_Debug("ID           : %s \n", m_szId);
	HxLOG_Debug("PASSWORD     : %s \n", m_szPassword);
	HxLOG_Debug("MOUNT_PATH   : %s \n", m_szMountPath);
	HxLOG_Debug("MOUNT_LOG    : %s \n", m_szLogMsg);
	HxLOG_Debug("bMounted     : %s \n", m_bMount == TRUE? "TRUE" : "FALSE");
	HxLOG_Debug("state        : %s \n", m_nState);

	LEAVE;
}

HINT32 OplSharedFolder::SetID(HCHAR *szId)
{
	return HxSTD_PrintToStrN(m_szId, 			sizeof(m_szId), 		"%s", szId);

}

HINT32 OplSharedFolder::SetPassword(HCHAR *szPassword)
{
 	return HxSTD_PrintToStrN(m_szPassword, 	sizeof(m_szPassword), 	"%s", szPassword);
}

/****************************************************************************************
 * OplSambaClient
 ***************************************************************************************/
OplSambaClient* OplSambaClient::m_oplSambaClientManagerInstance;
OplSambaManagerCallback	OplSambaClient::m_callback;

OplSambaClient::OplSambaClient()
{
	ENTRY;
}

OplSambaClient::~OplSambaClient()
{
	ENTRY;
}

void 					OplSambaClient::AddListener(OplSambaManagerCallback callback)
{
	m_callback = callback;
	APK_FILEMGR_SAMBACLIENT_RegisterCallback(sambac_apk_callback);
}

void 					OplSambaClient::RemoveListener( void )
{
	APK_FILEMGR_SAMBACLIENT_UnregisterCallback(sambac_apk_callback);
	m_callback = NULL;
}

HBOOL					OplSambaClient::StartScan( void )
{
	HBOOL bRet = TRUE;
	if(ERR_FAIL == APK_FILEMGR_SAMBACLIENT_StartScan())
	{
		bRet = FALSE;
	}
	APK_DB_SetInt(USERDB_SAMBACLIENT_ACTIVATION, (HUINT32)TRUE );

	return bRet;
}

HBOOL					OplSambaClient::StopScan( void )
{
	HBOOL bRet = TRUE;
	APK_FILEMGR_SAMBACLIENT_StopScan();
	APK_DB_SetInt(USERDB_SAMBACLIENT_ACTIVATION, (HUINT32)FALSE );
	return bRet;
}

void 					OplSambaClient::printInfo(void *p)
{
	ApkFileMgr_SMBSharedFolderInfo_t *pSharedFolder = (ApkFileMgr_SMBSharedFolderInfo_t*)p;
	HxLOG_Debug("[OPL-CALLUP] servername   : %s \n", pSharedFolder->szServerName);
	HxLOG_Debug("[OPL-CALLUP] sharedname   : %s \n", pSharedFolder->szSharedName);
	HxLOG_Debug("[OPL-CALLUP] ip           : %s \n", pSharedFolder->szIp);
	HxLOG_Debug("[OPL-CALLUP] mountpath    : %s \n", pSharedFolder->szMountPath);
	HxLOG_Debug("[OPL-CALLUP] log          : %s \n", pSharedFolder->szLogMsg);
	HxLOG_Debug("[OPL-CALLUP] id           : %s \n", pSharedFolder->szId);
	HxLOG_Debug("[OPL-CALLUP] password     : %s \n", pSharedFolder->szPassword);
	HxLOG_Debug("[OPL-CALLUP] bMounted     : %d \n", pSharedFolder->bMounted);
}

HBOOL					OplSambaClient::Mount( OplSharedFolder *instance )
{
	HBOOL bRet = TRUE;
	ApkFileMgr_SMBSharedFolderInfo_t *pSharedFolder = (ApkFileMgr_SMBSharedFolderInfo_t*)OplSambaClient::getSharedFolder(instance);
	if( ERR_FAIL == APK_FILEMGR_SAMBACLIENT_Mount(pSharedFolder))
	{
		bRet = FALSE;
	}
	return bRet;
}

HBOOL					OplSambaClient::UnMount( OplSharedFolder *instance )
{
	HBOOL bRet = TRUE;
	ApkFileMgr_SMBSharedFolderInfo_t *pSharedFolder = (ApkFileMgr_SMBSharedFolderInfo_t*)OplSambaClient::getSharedFolder(instance);
	if( ERR_FAIL == APK_FILEMGR_SAMBACLIENT_Unmount(pSharedFolder))
	{
		bRet = FALSE;
	}
	return bRet;
}
HBOOL					OplSambaClient::UnmountAll(void)
{
	HBOOL bRet = TRUE;
	if( ERR_FAIL == APK_FILEMGR_SAMBACLIENT_UnmountAll())
	{
		bRet = FALSE;
	}
	return bRet;

}

HBOOL					OplSambaClient::GetAvailableSharedList( int *size, void **sharedList)
{
	HBOOL bRet = TRUE;
	//1 #### sambac_apk_callback °á°ú¸¦ º¸°í ÀúÀåµÈ ¸®½ºÆ®¸¦ ¿Ã·ÁÁÙ °Í.

	return bRet;
}

void* OplSambaClient::getSharedFolder(OplSharedFolder *instance)
{
	if( NULL != instance)
	{
		HxSTD_MemSet(&g_stSharedFolderInfo, 0x00, sizeof(ApkFileMgr_SMBSharedFolderInfo_t));
		g_stSharedFolderInfo.bMounted 	= instance->GetMountState();
		g_stSharedFolderInfo.nState		= instance->GetState();

		HxSTD_PrintToStrN(g_stSharedFolderInfo.szId, 			eSAMBAC_MAX_ID_LEN, 		"%s", instance->GetID());
		HxSTD_PrintToStrN(g_stSharedFolderInfo.szIp, 			eSAMBAC_MAX_IP_LEN, 		"%s", instance->GetIP());
		HxSTD_PrintToStrN(g_stSharedFolderInfo.szMountPath, 	eSAMBAC_MAX_PATH_LEN, 		"%s", instance->GetMountPath());
		HxSTD_PrintToStrN(g_stSharedFolderInfo.szPassword, 		eSAMBAC_MAX_PASSWORD_LEN, 	"%s", instance->GetPassword());
		HxSTD_PrintToStrN(g_stSharedFolderInfo.szServerName, 	eSAMBAC_MAX_SRVNAME_LEN, 	"%s", instance->GetServerName());
		HxSTD_PrintToStrN(g_stSharedFolderInfo.szSharedName, 	eSAMBAC_MAX_FOLDER_LEN, 	"%s", instance->GetSharedName());
		HxSTD_PrintToStrN(g_stSharedFolderInfo.szLogMsg, 		eSAMBAC_MAX_LOG_LEN, 		"%s", instance->GetLogMsg());

	}
	return (void*)&g_stSharedFolderInfo;
}

void 					OplSambaClient::sambac_apk_callback(const HCHAR *pszEventName, void *pvData, void *pvUserData)
{
	OplSharedFolder *pSharedFolder = NULL;
	HUINT32	event = -1;

	if (HxSTD_StrCmp(pszEventName, "started") == 0)
	{
		HINT32	nStarted = (HINT32)pvData;

		event = (nStarted ? eOPL_SAMBAC_SCAN_STARTED : eOPL_SAMBAC_SCAN_FAIL);
	} else
	if (HxSTD_StrCmp(pszEventName, "stopped") == 0)
	{
		HINT32	nStopped = (HINT32)pvData;

		event = (nStopped ? eOPL_SAMBAC_SCAN_COMPLETE : eOPL_SAMBAC_SCAN_FAIL);
	} else
	if (HxSTD_StrCmp(pszEventName, "mounted") == 0)
	{
		ApkFileMgr_SMBSharedFolderInfo_t *p = (ApkFileMgr_SMBSharedFolderInfo_t*)pvData;

		printInfo(p);
		event = (TRUE == p->bMounted ? eOPL_SAMBAC_MOUNT_OK : eOPL_SAMBAC_MOUNT_FAIL);
		pSharedFolder = new OplSharedFolder((OplSharedFolderHandle)pvData);
	} else
	if (HxSTD_StrCmp(pszEventName, "unmounted") == 0)
	{
		ApkFileMgr_SMBSharedFolderInfo_t *p = (ApkFileMgr_SMBSharedFolderInfo_t*)pvData;
		printInfo(p);
		event = (FALSE == p->bMounted ? eOPL_SAMBAC_UMOUNT_OK : eOPL_SAMBAC_UMOUNT_FAIL);
		pSharedFolder = new OplSharedFolder((OplSharedFolderHandle)pvData);
	} else
	if (HxSTD_StrCmp(pszEventName, "addshared") == 0)
	{
		pSharedFolder = new OplSharedFolder((OplSharedFolderHandle)pvData);
		event = eOPL_SAMBAC_SCAN_ADD;
	} else
	if (HxSTD_StrCmp(pszEventName, "removeshared") == 0)
	{
		pSharedFolder = new OplSharedFolder((OplSharedFolderHandle)pvData);
		event = eOPL_SAMBAC_SCAN_REMOVE;
	}

	if(NULL != m_callback)
	{
		HxLOG_Debug("sambac_apk_callback ----- event [%d] --- \n", event);
		m_callback((int)event, (void*)pSharedFolder);
	} else {
		if(pSharedFolder)
		{
			delete pSharedFolder;
			pSharedFolder = NULL;
		}
	}
}

/****************************************************************************************
 * OplSambaServer
 ***************************************************************************************/
OplSambaServerCallback OplSambaServer::m_callback = NULL;
OplSambaServer::OplSambaServer()
{
	APK_FILEMGR_SAMBASERVER_RegisterCallback( OplSambaServer::apk_smbs_callback );
}

OplSambaServer::~OplSambaServer()
{
}

void 	OplSambaServer::AddListener(OplSambaServerCallback callback)
{
	m_callback = callback;
}

void 	OplSambaServer::RemoveListener(OplSambaServerCallback callback)
{
}

HINT32	OplSambaServer::Start(void)
{
	HINT32	nRet = 0;
	HCHAR	szPassword[OPL_MAX_PASSWORD_LEN];
	HERROR	hErr;

	APK_DB_SetInt(USERDB_SAMBASERVER_ACTIVATION, (HUINT32)TRUE );

	nRet = APK_FILEMGR_SAMBASERVER_Start();

	hErr = APK_DB_GetStr(USERDB_SAMBASERVER_PASSWORD, szPassword, OPL_MAX_PASSWORD_LEN);
	if ((hErr == ERR_OK) && (HxSTD_StrCmp((const HCHAR*)szPassword, "") != 0))
	{
		APK_FILEMGR_SAMBASERVER_SetPassword("admin","", (const HCHAR*)szPassword);
		APK_FILEMGR_SAMBASERVER_SetPublicAccess("admin", FALSE);
	}
	return nRet;
}

HINT32	OplSambaServer::Stop(void)
{
	APK_DB_SetInt(USERDB_SAMBASERVER_ACTIVATION, (HUINT32)FALSE );
	return APK_FILEMGR_SAMBASERVER_Stop();
}

HBOOL OplSambaServer::IsStarted(void)
{
	ApkFileMgr_SMBS_INFO_t stInfo;

	APK_FILEMGR_SAMBASERVER_GetServerInfo( &stInfo );
	return stInfo.bStarted;
}

HINT32	OplSambaServer::SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd)
{
	APK_DB_SetStr(USERDB_SAMBASERVER_PASSWORD, pszNewPasswd);
	return APK_FILEMGR_SAMBASERVER_SetPassword( pszUserName, pszCurPasswd, pszNewPasswd );
}

HERROR	OplSambaServer::GetPassword(HCHAR *pszPasswd)
{
	return APK_DB_GetStr(USERDB_SAMBASERVER_PASSWORD, pszPasswd, OPL_MAX_PASSWORD_LEN);
}

HINT32	OplSambaServer::CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid)
{
	return APK_FILEMGR_SAMBASERVER_CheckPassword( pszUserName, pszPasswd, pbIsValid );
}

HINT32	OplSambaServer::GetSharedFolder(const HCHAR *pszUserName, HINT32 *pnCount, void **ppList)
{
	HINT32 nCount = 0;
	HINT32 i = 0;
	ApkFileMgr_SMBS_SHARED_t *pstSharedList;
	void **ppShareList = NULL;
	HERROR hErr = ERR_FAIL;

	hErr = APK_FILEMGR_SAMBASERVER_GetSharedList( pszUserName, &nCount, &pstSharedList );
	if ( hErr != ERR_OK )
		return -1;
	if ( 0 == nCount )
		return 0;

	ppShareList = (void **)HLIB_STD_MemAlloc(sizeof(OplSmbsShareFolder *)*nCount);
	HxSTD_MemSet( ppShareList, 0x00, sizeof(OplSmbsShareFolder *) * nCount ) ;
	for ( i = 0; i < nCount; i++ )
	{
		ppShareList[i] = new OplSmbsShareFolder( pszUserName, (void *)&pstSharedList[i] );
	}

	*pnCount = nCount;
	*ppList = (OplSmbsShareFolder *)ppShareList;

	return 0;
}

HINT32	OplSambaServer::SetSharedFolder(const HCHAR *pszUserName, const HCHAR *pszPath, const HCHAR *pszOpt)
{
	ApkFileMgr_SMBS_SHARED_t stShared;
	HxSTD_MemSet( &stShared, 0x00, sizeof(ApkFileMgr_SMBS_SHARED_t));

	HxSTD_StrNCpy( stShared.szPath, pszPath, sizeof(stShared.szPath)-1 );

	if ( !HxSTD_StrCmp( pszOpt, "ro" ) )
	{
		stShared.bWritable = FALSE;
	}
	else if ( !HxSTD_StrCmp( pszOpt, "rw" ) )
	{
		stShared.bWritable = TRUE;
	}
	else
		return -1;

	return APK_FILEMGR_SAMBASERVER_SetPermission( pszUserName, &stShared );
}

HINT32	OplSambaServer::GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbAccess)
{
	return APK_FILEMGR_SAMBASERVER_GetPublicAccess( pszUserName, pbAccess );
}

HINT32	OplSambaServer::SetPublicAccess(const HCHAR *pszUserName, HBOOL bAccess)
{
	return APK_FILEMGR_SAMBASERVER_SetPublicAccess( pszUserName, bAccess );
}

const HCHAR * OplSambaServer::GetServerName()
{
	ApkFileMgr_SMBS_INFO_t stInfo;
	APK_FILEMGR_SAMBASERVER_GetServerInfo( &stInfo );
	HxSTD_StrNCpy( m_szServerName, stInfo.stConf.szServerName, sizeof(m_szServerName)-1 );
	return m_szServerName;
}

HINT32 OplSambaServer::SetServerName( const HCHAR *pszServerName )
{
	HxSTD_StrNCpy( m_szServerName, pszServerName, sizeof(m_szServerName) -1);
	return 0;
}

const HCHAR * OplSambaServer::GetWorkgroup()
{
	ApkFileMgr_SMBS_INFO_t stInfo;
	APK_FILEMGR_SAMBASERVER_GetServerInfo( &stInfo );
	HxSTD_StrNCpy( m_szWorkgroup, stInfo.stConf.szWorkgroup, sizeof(m_szWorkgroup)-1 );
	return m_szWorkgroup;
}

HINT32 OplSambaServer::SetWorkgroup( const HCHAR *pszWorkgroup )
{
	HxSTD_StrNCpy( m_szWorkgroup, pszWorkgroup, sizeof(m_szWorkgroup)-1 );
	return 0;
}

void	OplSambaServer::apk_smbs_callback( HINT32 event, HINT32 retErr, void *pvApkData )
{
	if ( NULL == m_callback )
		return ;

	switch ( event )
	{
		case eAPK_FILEMGR_SMBS_EVT_SERVER_START :
			break;
		case eAPK_FILEMGR_SMBS_EVT_SERVER_STOP :
			break;
		case eAPK_FILEMGR_SMBS_EVT_CONFIG_CHANGE :
			break;
		case eAPK_FILEMGR_SMBS_EVT_PASSWD_CHANGE :
			break;
		case eAPK_FILEMGR_SMBS_EVT_PERMISSION_CHANGE :
			break;
		case eAPK_FILEMGR_SMBS_EVT_STATUS_CHANGE :
			break;
		default :
			return ;
	}

	m_callback( event, retErr );
}

/****************************************************************************************
 * OplSmbsShareFolder
 ***************************************************************************************/
OplSmbsShareFolder::OplSmbsShareFolder( const char *pszUser, void *apkHandle )
{
	ApkFileMgr_SMBS_SHARED_t *pApkShared = (ApkFileMgr_SMBS_SHARED_t *)apkHandle;

	HxSTD_StrNCpy( m_szPath, pApkShared->szPath, sizeof(m_szPath)-1 );
	HxSTD_StrNCpy( m_szName, pApkShared->szName, sizeof(m_szName)-1 );
	HxSTD_StrNCpy( m_szUser, pszUser, sizeof(m_szUser)-1 );
	m_bIsWritable =  pApkShared->bWritable;
	m_bIsValide = pApkShared->bMounted;
}

OplSmbsShareFolder::~OplSmbsShareFolder()
{
}

HINT32	OplSmbsShareFolder::SetWritable( HBOOL bWritable )
{
	ApkFileMgr_SMBS_SHARED_t stShared;
	HxSTD_MemSet( &stShared, 0x00, sizeof(ApkFileMgr_SMBS_SHARED_t));

	HxSTD_StrNCpy( stShared.szPath, m_szPath, sizeof(stShared.szPath)-1 );
	stShared.bWritable = bWritable;

	return APK_FILEMGR_SAMBASERVER_SetPermission( m_szUser, &stShared );
}

