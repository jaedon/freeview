/*************************************************************************************************************
	File 		: NativeHSamba.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/10/24
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "NativeHFtpManager.h"
#include "macros.h"
#include "oplftpmgr.h"

#ifdef OIPF

/*****************************************************************************
 * HFtpManager
 ******************************************************************************/
static OplFtpServer *s_pFtpsSeverHandle = NULL;
FtpServerHandle	native_FtpManager_GetServerHandle(void)
{
	if ( NULL == s_pFtpsSeverHandle )
	{
		s_pFtpsSeverHandle = new OplFtpServer();
	}
	return (FtpServerHandle)s_pFtpsSeverHandle;
}

/*****************************************************************************
 * HFtpServer
 ******************************************************************************/
OOIFReturnCode native_FtpServer_AddListener(NativeFtpServerCallback listener)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	OplFtpServer::AddListener( listener );
	return eRet;
}

OOIFReturnCode native_FtpServer_RemoveListener(NativeFtpServerCallback listener)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	OplFtpServer::RemoveListener( listener );
	return eRet;
}

OOIFReturnCode native_FtpServer_Start( FtpServerHandle identifier, OOIFNumber *retval)
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->Start();
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_Stop( FtpServerHandle identifier, OOIFNumber *retval)
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->Stop();
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_IsStarted( FtpServerHandle identifier, OOIFBoolean *retval)
{
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	*retval = pFtpServer->IsStarted();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_GetName( FtpServerHandle identifier, const char **retval )
{
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	*retval =  pFtpServer->GetServerName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_SetName( FtpServerHandle identifier, const char *serverName, OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->SetServerName( serverName );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_GetPort( FtpServerHandle identifier, OOIFNumber*retval )
{
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	*retval = pFtpServer->GetPortNumber();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_SetPort( FtpServerHandle identifier, HINT32 nPort, OOIFNumber*retval )
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->SetPortNumber( nPort );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_GetShareFolder( FtpServerHandle identifier, 
						const char*userName, int *nCount, FtpServerHandle *SharedList )
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);

	ret = pFtpServer->GetSharedFolder( userName, nCount, SharedList );
	if ( ret != 0 )
	{
	    *nCount = 0;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_SetShareFolder( FtpServerHandle identifier,
						 const char *userName, const char *path, const char *opt, OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->SetSharedFolder( userName, path, opt );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_GetPassword( FtpServerHandle identifier, const char **retval) 
{
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	char password[OPL_MAX_PASSWORD_LEN] = {0,};
	if ( ERR_OK == pFtpServer->GetPassword(password) )
	{
		*retval = HLIB_STD_StrDup(password);
	}
	else
	{
	    *retval = "";
	}
	return OOIF_RETURN_OK;	
}

OOIFReturnCode native_FtpServer_SetPassword( FtpServerHandle identifier, const char *userName, 
						const char *curPassword, const char *newPasword, OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->SetPassword( userName, curPassword, newPasword );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_IsValidPassword( FtpServerHandle identifier, 
						const char *userName, const char *curPassword, OOIFBoolean*retval)
{
	HINT32 ret = 0;
	HBOOL isValid = FALSE;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->CheckPassword( userName, curPassword, &isValid );
	if ( ret == 0 )
	{
	    *retval = isValid ? TRUE : FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_SetPublicAccess( FtpServerHandle identifier, 
					const char *userName, OOIFBoolean bAccess, OOIFNumber*retval)
{
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	*retval = pFtpServer->SetPublicAccess( userName, bAccess ? TRUE : FALSE );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpServer_GetPublicAccess( FtpServerHandle identifier, 
					const char *userName, OOIFBoolean*retval)
{
	HINT32 ret = 0;
	HBOOL bAccess = FALSE;
	OplFtpServer *pFtpServer = static_cast<OplFtpServer *>(identifier);
	ret = pFtpServer->GetPublicAccess( userName, &bAccess );
	if ( ret == 0 )
	{
	    *retval = bAccess ? TRUE : FALSE;
	}
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HFtpsShareFolder
 ******************************************************************************/
OOIFReturnCode native_FtpsShareFolder_GetPath( FtpServerHandle identifier, const char **retval )
{

	OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(identifier);
	*retval = pShared->GetPath();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpsShareFolder_GetName( FtpServerHandle identifier, const char **retval )
{

	OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(identifier);
	*retval = pShared->GetName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpsShareFolder_GetUser( FtpServerHandle identifier, const char **retval )
{

	OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(identifier);
	*retval = pShared->GetUser();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpsShareFolder_GetWritable( FtpServerHandle identifier, OOIFBoolean *retval )
{
	OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(identifier);
	*retval = pShared->GetWritable() ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpsShareFolder_SetWritable( FtpServerHandle identifier, OOIFBoolean retval )
{
	OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(identifier);
	pShared->SetWritable( retval ? TRUE : FALSE );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpsShareFolder_IsValid( FtpServerHandle identifier,  OOIFBoolean *retval )
{
	OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(identifier);
	*retval = pShared->IsValide() ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_FtpsShareFolder_ReleaseHandle( FtpServerHandle id )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id)
	{
		OplFtpsShareFolder *pShared = static_cast<OplFtpsShareFolder *>(id);
		delete pShared;
	}
	else 
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

#endif // OIPF

