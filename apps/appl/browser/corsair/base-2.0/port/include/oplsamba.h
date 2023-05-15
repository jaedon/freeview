/*************************************************************************************************************
	File 		: oplsamba.h
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

#ifndef __OPL_SAMBA_H__
#define __OPL_SAMBA_H__

#include "hlib.h"

#define 	OPL_MAX_PASSWORD_LEN						128

typedef enum
{
	eOPL_SAMBAC_SCAN_STARTED	= 0,
	eOPL_SAMBAC_SCAN_ADD,
	eOPL_SAMBAC_SCAN_REMOVE,
	eOPL_SAMBAC_SCAN_COMPLETE,
	eOPL_SAMBAC_SCAN_FAIL,

	eOPL_SAMBAC_MOUNT_OK 		= 10,
	eOPL_SAMBAC_MOUNT_FAIL,
	eOPL_SAMBAC_UMOUNT_OK,
	eOPL_SAMBAC_UMOUNT_FAIL,
} OplSamba_Event_e;

typedef enum
{
	eSAMBAC_MAX_MOUNT			= 128,

	eSAMBAC_MAX_IP_LEN			= 16,
	eSAMBAC_MAX_ID_LEN			= 64,
	eSAMBAC_MAX_PATH_LEN		= 512,
	eSAMBAC_MAX_PASSWORD_LEN	= 64,
	eSAMBAC_MAX_LOG_LEN 		= 128,
	eSAMBAC_MAX_FOLDER_LEN		= 128,
	eSAMBAC_MAX_SRVNAME_LEN		= 64,
} OPL_SAMBAC_MAX_e;

/* End of Content Manager (LocalMedia) *************************************************/

typedef void * OplSharedFolderHandle;
typedef void (*OplSambaManagerCallback)(int event, void *contentList);
/****************************************************************************************
 * OplBrowsedContent
 ***************************************************************************************/
class OplSharedFolder
{
public:
	OplSharedFolder( OplSharedFolderHandle Handle);
	OplSharedFolder( const OplSharedFolder &original );
	virtual ~OplSharedFolder( void );

	HINT32				GetState( void ) 			{ return m_nState; 			};
	HBOOL				GetMountState( void ) 	{ return m_bMount;			};
	HCHAR* 				GetServerName( void )	{ return m_szServerName; 	};
	HCHAR* 				GetSharedName( void )	{ return m_szSharedName;	};
	HCHAR*				GetIP( void )					{ return m_szIp;			};
	HCHAR* 				GetID( void )					{ return m_szId;			};
	HCHAR*				GetPassword( void )		{ return m_szPassword;		};
	HCHAR*				GetMountPath( void )		{ return m_szMountPath;		};
	HCHAR*				GetLogMsg( void ) 		{ return m_szLogMsg;		};
	HINT32				SetID(HCHAR *szId);
	HINT32 				SetPassword(HCHAR *szPassword);
private:
	//1 ### use field by  WEB_APPLICATION
	HCHAR				m_szServerName	[eSAMBAC_MAX_SRVNAME_LEN];
	HCHAR				m_szSharedName	[eSAMBAC_MAX_PATH_LEN];
	HCHAR 				m_szIp			[eSAMBAC_MAX_IP_LEN];
	HCHAR 				m_szId			[eSAMBAC_MAX_ID_LEN];
	HCHAR 				m_szPassword	[eSAMBAC_MAX_PASSWORD_LEN];
	HCHAR 				m_szMountPath	[eSAMBAC_MAX_PATH_LEN];
	HCHAR 				m_szLogMsg		[eSAMBAC_MAX_PATH_LEN];
	HBOOL				m_bMount;
	HINT32 				m_nState;

private:
	void 				print_SharedFolder_Info();

};

/****************************************************************************************
 * OplSambaClient
 ***************************************************************************************/
class OplSambaClient {
public:
	OplSambaClient();
	virtual 	~OplSambaClient();

	static void 					AddListener(OplSambaManagerCallback callback);
	static void						RemoveListener( void );
	static HBOOL 					StartScan( void );
	static HBOOL		 			StopScan( void );
	static HBOOL 					Mount( OplSharedFolder *instance );
	static HBOOL 					UnMount( OplSharedFolder *instance );
	static HBOOL					UnmountAll(void);
	static HBOOL				 	GetAvailableSharedList(  int *size, void **sharedList );
	static void* 					getSharedFolder(OplSharedFolder *instance);

private:
	static void 					sambac_apk_callback(const HCHAR *pszEventName, void *pvData, void *pvUserData);
	static void						printInfo(void *pSharedFolder);

	static OplSambaClient	*m_oplSambaClientManagerInstance;
	static OplSambaManagerCallback	m_callback;
};


/****************************************************************************************
 * OplSambaServer
 ***************************************************************************************/
typedef void (*OplSambaServerCallback)(int event, int retErr);
class OplSambaServer {
	public:
		OplSambaServer();
		virtual ~OplSambaServer();

		static void		AddListener(OplSambaServerCallback callback);
		static void		RemoveListener(OplSambaServerCallback callback);

		HINT32	Start(void);
		HINT32	Stop(void);

		HBOOL	IsStarted(void);

		HINT32	SetPassword(const HCHAR *pszUserName, const HCHAR *pszCurPasswd, const HCHAR *pszNewPasswd);
		HERROR	GetPassword(HCHAR *pszPasswd);
		HINT32	CheckPassword(const HCHAR *pszUserName, const HCHAR *pszPasswd, HBOOL *pbIsValid);

		HINT32 	GetSharedFolder(const HCHAR *pszUserName, HINT32 *pnCount, void **ppList);
		HINT32	SetSharedFolder(const HCHAR *pszUserName, const HCHAR *pszPath, const HCHAR *pszOpt);
		HINT32	GetPublicAccess(const HCHAR *pszUserName, HBOOL *pbAccess);
		HINT32	SetPublicAccess(const HCHAR *pszUserName, HBOOL bAccess);

		const HCHAR *	GetServerName();
		HINT32	SetServerName( const HCHAR *pszServerName );

		const HCHAR *	GetWorkgroup();
		HINT32	SetWorkgroup( const HCHAR *pszWorkgroup );

	private:
		static	void	apk_smbs_callback( HINT32 event, HINT32 retErr, void *pvApkData );
		HBOOL	m_bStarted;
		HCHAR	m_szServerName[64];
		HCHAR	m_szWorkgroup[64];

		static	OplSambaServerCallback	m_callback;
};

class OplSmbsShareFolder {
	public:
		OplSmbsShareFolder( const char *pszUser, void *apkHandle );
		virtual ~OplSmbsShareFolder();
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
