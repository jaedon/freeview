/*************************************************************************************************************
	File 		: oplftpmgr.h
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

#ifndef __OPL_FTP_H__
#define __OPL_FTP_H__

#include "hlib.h"

#define 	OPL_MAX_PASSWORD_LEN						128

typedef enum
{
	eOPL_FTPSERVER_SCAN_STARTED	= 0,
	eOPL_FTPSERVER_SCAN_ADD,
	eOPL_FTPSERVER_SCAN_REMOVE,
	eOPL_FTPSERVER_SCAN_COMPLETE,
	eOPL_FTPSERVER_SCAN_FAIL,

	eOPL_FTPSERVER_MOUNT_OK 		= 10,
	eOPL_FTPSERVER_MOUNT_FAIL,
	eOPL_FTPSERVER_UMOUNT_OK,
	eOPL_FTPSERVER_UMOUNT_FAIL,
} OplSamba_Event_e;

typedef enum
{
	eFTPSERVER_MAX_MOUNT			= 128,

	eFTPSERVER_MAX_USER_LEN			= 64,
	eFTPSERVER_MAX_PASSWORD_LEN	= 64,
	eFTPSERVER_MAX_LOG_LEN 		= 128,
	eFTPSERVER_MAX_FOLDER_LEN		= 128,
	eFTPSERVER_MAX_SRVNAME_LEN		= 64,
} OPL_FTPSERVER_MAX_e;

/****************************************************************************************
 * OplFtpServer
 ***************************************************************************************/
typedef void (*OplFtpServerCallback)(int event, int retErr);

class OplFtpServer {
	public:
		OplFtpServer();
		virtual ~OplFtpServer();

		static void	AddListener(OplFtpServerCallback callback);
		static void	RemoveListener(OplFtpServerCallback callback);

		HINT32	Start(void);
		HINT32	Stop(void);

		HBOOL	IsStarted(void);

		HINT32	SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd);
		HINT32	GetPassword(HCHAR *pszPasswd);
		HINT32	CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid);

		HINT32	GetSharedFolder(const HCHAR *pszUserName, HINT32 *pnCount, void **ppList);
		HINT32	SetSharedFolder(const HCHAR *pszUserName, const HCHAR *pszPath, const HCHAR *pszOpt);
		HINT32	GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbAccess);
		HINT32	SetPublicAccess(const HCHAR *pszUserName, HBOOL bAccess);

		const HCHAR *	GetServerName();
		HINT32	SetServerName( const HCHAR *pszServerName );

		HINT32	GetPortNumber();
		HINT32	SetPortNumber(HINT32 nPort);

	private:
		static	void	apk_ftps_callback( HINT32 event, HINT32 retErr, void *pvApkData );
		HBOOL	m_bStarted;
		HINT32	m_nPort;
		HCHAR	m_szServerName[64];

		static	OplFtpServerCallback	m_callback;
};

class OplFtpsShareFolder {
	public:
		OplFtpsShareFolder( const char *pszUser, void *handle );
		virtual ~OplFtpsShareFolder();
		HCHAR * GetPath(void) { return m_szPath; };
		HCHAR * GetName(void) { return m_szName; };
		HCHAR * GetUser(void) { return m_szUser; };
		HBOOL	GetWritable(void) { return m_bIsWritable; };
		HINT32	SetWritable( HBOOL bWritable );
		HBOOL	IsValide(void) { return m_bIsValide; };

	private:
		HCHAR m_szName[128];
		HCHAR m_szPath[512];
		HCHAR m_szUser[128];
		HBOOL m_bIsWritable;
		HBOOL m_bIsValide;
};


#endif
