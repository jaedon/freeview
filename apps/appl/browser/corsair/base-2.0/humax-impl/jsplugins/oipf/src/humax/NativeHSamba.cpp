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

#include "NativeHSamba.h"
#include "macros.h"
#include "oplsamba.h"

#ifdef OIPF

/*****************************************************************************
 * HSambaManager
 ******************************************************************************/
static OplSambaServer *s_pSmbSeverHandle = NULL;
SambaServerHandle	native_SambaManager_GetServerHandle(void)
{
	if ( NULL == s_pSmbSeverHandle )
	{
		s_pSmbSeverHandle = new OplSambaServer;
	}
	return (SambaServerHandle)s_pSmbSeverHandle;
}

/*****************************************************************************
 * HSambaServer
 ******************************************************************************/
OOIFReturnCode native_SambaServer_AddListener(NativeSambaServerCallback listener)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	OplSambaServer::AddListener( listener );
	return eRet;
}

OOIFReturnCode native_SambaServer_RemoveListener(NativeSambaServerCallback listener)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	OplSambaServer::RemoveListener( listener );
	return eRet;
}

OOIFReturnCode native_SambaServer_Start( SambaServerHandle identifier, OOIFNumber *retval)
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->Start();
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_Stop( SambaServerHandle identifier, OOIFNumber *retval)
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->Stop();
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_IsStarted( SambaServerHandle identifier, OOIFBoolean *retval)
{
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	*retval = pSambaServer->IsStarted();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_GetName( SambaServerHandle identifier, const char **retval )
{
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	*retval =  pSambaServer->GetServerName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_SetName( SambaServerHandle identifier, const char *serverName, OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->SetServerName( serverName );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_GetWorkgroup( SambaServerHandle identifier, const char **retval )
{
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	*retval =  pSambaServer->GetWorkgroup();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_SetWorkgroup( SambaServerHandle identifier, const char *workgroup, 
												OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->SetWorkgroup( workgroup );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_GetShareFolder( SambaServerHandle identifier, 
						const char*userName, int *nCount, SambaServerHandle *SharedList )
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->GetSharedFolder( userName, nCount, SharedList );
	if ( ret != 0 )
	{
	    *nCount = 0;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_SetShareFolder( SambaServerHandle identifier,
						 const char *userName, const char *path, const char *opt, OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->SetSharedFolder( userName, path, opt );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_GetPassword( SambaServerHandle identifier, const char **retval)
{
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	char password[OPL_MAX_PASSWORD_LEN] = {0,};
	if ( ERR_OK == pSambaServer->GetPassword(password) )
	{
		*retval = HLIB_STD_StrDup(password);
	}
	else
	{
	    *retval = "";
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_SetPassword( SambaServerHandle identifier, const char *userName, 
						const char *curPassword, const char *newPasword, OOIFNumber*retval)
{
	HINT32 ret = 0;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->SetPassword( userName, curPassword, newPasword );
	*retval = ret;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_IsValidPassword( SambaServerHandle identifier, 
						const char *userName, const char *curPassword, OOIFBoolean*retval)
{
	HINT32 ret = 0;
	HBOOL isValid = FALSE;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->CheckPassword( userName, curPassword, &isValid );
	if ( ret == 0 )
	{
	    *retval = isValid ? TRUE : FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_SetPublicAccess( SambaServerHandle identifier, 
							const char *userName, OOIFBoolean bAccess, OOIFNumber*retval)
{
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	*retval = pSambaServer->SetPublicAccess( userName, bAccess ? TRUE : FALSE );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SambaServer_GetPublicAccess( SambaServerHandle identifier, 
							const char *userName, OOIFBoolean*retval)
{
	HINT32 ret = 0;
	HBOOL bAccess = FALSE;
	OplSambaServer *pSambaServer = static_cast<OplSambaServer *>(identifier);
	ret = pSambaServer->GetPublicAccess( userName, &bAccess );
	if ( ret == 0 )
	{
	    *retval = bAccess ? TRUE : FALSE;
	}
	return OOIF_RETURN_OK;
}

/*****************************************************************************
 * HSmbsShareFolder
 ******************************************************************************/

OOIFReturnCode native_SmbsShareFolder_GetPath( SambaServerHandle identifier, const char **retval )
{

	OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(identifier);
	*retval = pShared->GetPath();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SmbsShareFolder_GetName( SambaServerHandle identifier, const char **retval )
{

	OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(identifier);
	*retval = pShared->GetName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SmbsShareFolder_GetUser( SambaServerHandle identifier, const char **retval )
{

	OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(identifier);
	*retval = pShared->GetUser();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SmbsShareFolder_GetWritable( SambaServerHandle identifier, OOIFBoolean *retval )
{
	OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(identifier);
	*retval = pShared->GetWritable() ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SmbsShareFolder_SetWritable( SambaServerHandle identifier, OOIFBoolean retval )
{
	OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(identifier);
	pShared->SetWritable( retval ? TRUE : FALSE );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SmbsShareFolder_IsValid( SambaServerHandle identifier,  OOIFBoolean *retval )
{
	OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(identifier);
	*retval = pShared->IsValide() ? TRUE : FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SmbsShareFolder_ReleaseHandle( SambaServerHandle id )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id)
	{
		OplSmbsShareFolder *pShared = static_cast<OplSmbsShareFolder *>(id);
		delete pShared;
	}
	else 
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

/*****************************************************************************
 * HSambaClient
 ******************************************************************************/
OOIFReturnCode native_Sambac_AddListener(NativeSambaManagerCallback listener)
{
	OplSambaClient::AddListener(listener);
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_Sambac_RemoveListener( void )
{
	OplSambaClient::RemoveListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_Sambac_UnmountAll( OOIFBoolean *retval )
{
	*retval = OplSambaClient::UnmountAll();
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_Sambac_StartScan(OOIFBoolean *retval)
{
	*retval = TRUE;
	if(FALSE == OplSambaClient::StartScan())
	{
		*retval = FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_Sambac_StopScan(OOIFBoolean *retval)
{
	*retval = TRUE;
	if(FALSE == OplSambaClient::StopScan())
	{
		*retval = FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_Sambac_Mount(SambaServerHandle identifier, OOIFBoolean *retval)
{
	OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
	*retval = TRUE;

	OOIF_LOG_DEBUG("servername   : %s \n", sharedFolder->GetServerName());
	OOIF_LOG_DEBUG("sharedname   : %s \n", sharedFolder->GetSharedName());
	OOIF_LOG_DEBUG("ip           : %s \n", sharedFolder->GetIP());
	OOIF_LOG_DEBUG("mountpath    : %s \n", sharedFolder->GetMountPath());
	OOIF_LOG_DEBUG("id           : %s \n", sharedFolder->GetID());
	OOIF_LOG_DEBUG("password     : %s \n", sharedFolder->GetPassword());
	
	if(FALSE == OplSambaClient::Mount(sharedFolder))
	{
		*retval = FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_Sambac_Unmount(SambaServerHandle identifier, OOIFBoolean *retval)
{
	OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
	*retval = TRUE;
	if(FALSE == OplSambaClient::UnMount(sharedFolder))
	{
		*retval = FALSE;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_Sambac_GetAvailableServerList(int *size, SambaServerHandle **sharedList, OOIFBoolean *retval)
{
	*retval = TRUE;
	if(FALSE == OplSambaClient::GetAvailableSharedList(size, (void**)sharedList))
	{
		*retval = FALSE;
	}

	return OOIF_RETURN_OK;
}


/*****************************************************************************
 * HSambeServer
 ******************************************************************************/
OOIFReturnCode native_Sambac_GetSharedName( SambaServerHandle identifier, const char**retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetSharedName();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode native_Sambac_GetServerName( SambaServerHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetServerName();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;

}
OOIFReturnCode native_Sambac_GetServerIp( SambaServerHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetIP();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;

}
OOIFReturnCode native_Sambac_GetMountPath( SambaServerHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetMountPath();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode native_Sambac_GetLogMsg(SambaServerHandle identifier, const char **retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetLogMsg();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}	
	return eRet;
}

OOIFReturnCode native_Sambac_GetId( SambaServerHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetID();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode native_Sambac_GetPassword( SambaServerHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetPassword();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}

OOIFReturnCode natvie_Sambac_GetMountState( SambaServerHandle identifier, int *retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetMountState();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode natvie_Sambac_GetState( SambaServerHandle identifier, OOIFNumber *retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		*retval = sharedFolder->GetState();
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_Sambac_SetId( SambaServerHandle identifier, const char *newId)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		if(0 > sharedFolder->SetID((HCHAR*)newId)) 
		{
			eRet = OOIF_RETURN_ERROR;
		}
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_Sambac_SetPassword( SambaServerHandle identifier, const char *newPasword)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier)
	{
		OplSharedFolder *sharedFolder =  static_cast<OplSharedFolder*>(identifier);
		if(0 > sharedFolder->SetPassword((HCHAR*)newPasword)) 
		{
			eRet = OOIF_RETURN_ERROR;
		}
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_Sambac_ReleaseHandle( SambaServerHandle id )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id)
	{
		OplSharedFolder *content =  static_cast<OplSharedFolder *>(id);
		if ( NULL != content )
		{
			delete content;
			content = NULL;
		}
	}
	else 
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}


#endif // OIPF
