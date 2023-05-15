/*************************************************************************************************************
	File 		: oplftpmgr.cpp
	author 		: WEB TFT tft_webapp@humaxdigital.com
	comment		:
	date    	: 2012/12/12
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
#include <iostream>
#include <apk.h>
#include "bpldb.h"
#include "oplftpmgr.h"

#define ENTRY					HxLOG_Debug("%s : %s : +++++++++++++++++++++++++++++\n", __FILE__ , __FUNCTION__);
#define LEAVE					HxLOG_Debug("%s : %s : -----------------------------\n", __FILE__ , __FUNCTION__);
#define SAFE_DELETE(p)			{if(p){ HLIB_STD_MemFree(p); (p)=NULL;}}
#define SAFE_DELETE_ARRAY(p) 	{if(p){ HLIB_STD_MemFree[](p); (p)=NULL;}}

/****************************************************************************************
 * OplFtpServer
 ***************************************************************************************/
OplFtpServerCallback OplFtpServer::m_callback = NULL;

OplFtpServer::OplFtpServer()
{
	APK_FILEMGR_FTPSERVER_RegisterCallback( OplFtpServer::apk_ftps_callback );
}

OplFtpServer::~OplFtpServer()
{
}

void 	OplFtpServer::AddListener(OplFtpServerCallback callback)
{
	m_callback = callback;
}

void 	OplFtpServer::RemoveListener(OplFtpServerCallback callback)
{
}

HINT32	OplFtpServer::Start(void)
{
	HINT32	nRet = 0;
	HCHAR	szPassword[OPL_MAX_PASSWORD_LEN];
	HERROR	hErr;
	APK_DB_SetInt(USERDB_FTPSERVER_ACTIVATION, (HUINT32)TRUE );

	nRet = APK_FILEMGR_FTPSERVER_Start();

	// Restore FTP Server password
	hErr = APK_DB_GetStr(USERDB_FTPSERVER_PASSWORD, szPassword, OPL_MAX_PASSWORD_LEN);
	if ((hErr == ERR_OK) && (HxSTD_StrCmp((const HCHAR*)szPassword, "") != 0))
	{
		APK_FILEMGR_FTPSERVER_SetPassword("admin","", (const HCHAR*)szPassword);
		APK_FILEMGR_FTPSERVER_SetPublicAccess("admin", FALSE);
	}
	return nRet;
}

HINT32	OplFtpServer::Stop(void)
{
	APK_DB_SetInt(USERDB_FTPSERVER_ACTIVATION, (HUINT32)FALSE );
	return APK_FILEMGR_FTPSERVER_Stop();
}

HBOOL	OplFtpServer::IsStarted(void)
{
	ApkFileMgr_FTPS_INFO_t stInfo;

	APK_FILEMGR_FTPSERVER_GetServerInfo( &stInfo );
	return  stInfo.bStarted;
}

HINT32	OplFtpServer::SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd)
{
	APK_DB_SetStr(USERDB_FTPSERVER_PASSWORD, pszNewPasswd);
	return APK_FILEMGR_FTPSERVER_SetPassword( pszUserName, pszCurPasswd, pszNewPasswd );
}

HINT32	OplFtpServer::GetPassword(HCHAR *pszPasswd)
{
	return APK_DB_GetStr(USERDB_FTPSERVER_PASSWORD, pszPasswd, OPL_MAX_PASSWORD_LEN);
}

HINT32	OplFtpServer::CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid)
{
	return APK_FILEMGR_FTPSERVER_CheckPassword( pszUserName, pszPasswd, pbIsValid );
}

HINT32	OplFtpServer::GetSharedFolder(const HCHAR *pszUserName, HINT32 *pnCount, void **ppList)
{
	HINT32 nCount = 0;
	HINT32 i = 0;
	ApkFileMgr_FTPS_SHARED_t *pstSharedList;
	void **ppShareList = NULL;
	HERROR hErr = ERR_FAIL;

	hErr = APK_FILEMGR_FTPSERVER_GetSharedList( pszUserName, &nCount, &pstSharedList );
	if ( hErr != ERR_OK )
		return -1;
	if ( 0 == nCount )
		return 0;

	ppShareList = (void **)HLIB_STD_MemAlloc(sizeof(OplFtpsShareFolder *)*nCount);
	HxSTD_MemSet( ppShareList, 0x00, sizeof(OplFtpsShareFolder *) * nCount)  ;
	for ( i = 0; i < nCount; i++ )
	{
		ppShareList[i] = new OplFtpsShareFolder( pszUserName, (void *)&pstSharedList[i] );
	}

	*pnCount = nCount;
	*ppList = (OplFtpsShareFolder *)ppShareList;

	return 0;
}

HINT32	OplFtpServer::SetSharedFolder(const HCHAR *pszUserName, const HCHAR *pszPath, const HCHAR *pszOpt)
{
	ApkFileMgr_FTPS_SHARED_t stShared;
	HxSTD_MemSet( &stShared, 0x00, sizeof(ApkFileMgr_FTPS_SHARED_t));

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

	return APK_FILEMGR_FTPSERVER_SetPermission( pszUserName, &stShared );
}

HINT32	OplFtpServer::GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbAccess)
{
	return APK_FILEMGR_FTPSERVER_GetPublicAccess( pszUserName, pbAccess );
}

HINT32	OplFtpServer::SetPublicAccess(const HCHAR *pszUserName, HBOOL bAccess)
{
	return APK_FILEMGR_FTPSERVER_SetPublicAccess( pszUserName, bAccess );
}

const HCHAR * OplFtpServer::GetServerName()
{
	ApkFileMgr_FTPS_INFO_t stInfo;
	APK_FILEMGR_FTPSERVER_GetServerInfo( &stInfo );
	HxSTD_StrNCpy( m_szServerName, stInfo.stConf.szServerName, sizeof(m_szServerName)-1 );
	return m_szServerName;
}

HINT32 OplFtpServer::SetServerName( const HCHAR *pszServerName )
{
	HxSTD_StrNCpy( m_szServerName, pszServerName, sizeof(m_szServerName) -1);
	return 0;
}

HINT32 OplFtpServer::GetPortNumber()
{
	ApkFileMgr_FTPS_INFO_t stInfo;
	APK_FILEMGR_FTPSERVER_GetServerInfo( &stInfo );
	m_nPort =  stInfo.stConf.nPort;
	return  stInfo.stConf.nPort;
}

HINT32 OplFtpServer::SetPortNumber( HINT32 nPort )
{
	m_nPort = nPort;
	return  0;
}

void	OplFtpServer::apk_ftps_callback( HINT32 event, HINT32 retErr, void *pvApkData )
{
	if ( NULL == m_callback )
		return ;

	switch ( event )
	{
		case eAPK_FILEMGR_FTPS_EVT_SERVER_START :
			break;
		case eAPK_FILEMGR_FTPS_EVT_SERVER_STOP :
			break;
		case eAPK_FILEMGR_FTPS_EVT_CONFIG_CHANGE :
			break;
		case eAPK_FILEMGR_FTPS_EVT_PASSWD_CHANGE :
			break;
		case eAPK_FILEMGR_FTPS_EVT_PERMISSION_CHANGE :
			break;
		case eAPK_FILEMGR_FTPS_EVT_STATUS_CHANGE :
			break;
		default :
			return ;
	}

	m_callback( event, retErr );
}

/****************************************************************************************
 * OplFtpsShareFolder
 ***************************************************************************************/
OplFtpsShareFolder::OplFtpsShareFolder( const char *pszUser, void *apkHandle )
{
	ApkFileMgr_FTPS_SHARED_t *pApkShared = (ApkFileMgr_FTPS_SHARED_t *)apkHandle;

	HxSTD_StrNCpy( m_szPath, pApkShared->szPath, sizeof(m_szPath)-1 );
	HxSTD_StrNCpy( m_szName, pApkShared->szName, sizeof(m_szName)-1 );
	HxSTD_StrNCpy( m_szUser, pszUser, sizeof(m_szUser)-1 );
	m_bIsWritable =  pApkShared->bWritable;
	m_bIsValide = pApkShared->bMounted;
}

OplFtpsShareFolder::~OplFtpsShareFolder()
{
}

HINT32	OplFtpsShareFolder::SetWritable( HBOOL bWritable )
{
	ApkFileMgr_FTPS_SHARED_t stShared;
	HxSTD_MemSet( &stShared, 0x00, sizeof(ApkFileMgr_FTPS_SHARED_t));

	HxSTD_StrNCpy( stShared.szPath, m_szPath, sizeof(stShared.szPath)-1 );
	stShared.bWritable = bWritable;

	return APK_FILEMGR_FTPSERVER_SetPermission( m_szUser, &stShared );
}

