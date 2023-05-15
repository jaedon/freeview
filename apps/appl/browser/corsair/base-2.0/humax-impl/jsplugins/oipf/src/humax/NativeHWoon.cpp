/*************************************************************************************************************
	File 		: NativeHWoon.cpp
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

#include "NativeHWoon.h"
#include "macros.h"
#include "oplwoon.h"

#ifdef OIPF

/*****************************************************************************
 * HWoonClient
 ***************************************************************************** */
OOIFReturnCode native_WoonClient_RegEventListener(WoonClientEventListener listener)
{

	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WoonClient_UnRegEventListener( void )
{

	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WoonClient_StartScan( OOIFBoolean *retval )
{

	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WoonClient_StopScan( OOIFBoolean *retval )
{

	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_WoonClient_AddFriend(const char *wid, OOIFBoolean *retval)
{

	return OOIF_RETURN_NOT_SUPPORTED;
}
OOIFReturnCode native_WoonClient_RemoveFriend(const char *wid, OOIFBoolean *retval)
{

	return OOIF_RETURN_NOT_SUPPORTED;
}
OOIFReturnCode native_WoonClient_GetWoonFriendList( OOIFNumber *retval, int *nCount, WoonFriendHandle *friendHandle )
{

	return OOIF_RETURN_NOT_SUPPORTED;
}


/*****************************************************************************
 * HWoonServer
 ***************************************************************************** */
typedef enum{
	MAX_WOONS_FRIENDLY_NAME_BUF_LEN =	64,
}NativeHWoon_e;

static char 	g_szWoonServerFriendlyName[MAX_WOONS_FRIENDLY_NAME_BUF_LEN];

OOIFReturnCode native_WoonServer_RegEventListener(WoonServerEventListener listener)
{
	OPL_WOONSERVER_RegEventListener(listener);
	return OOIF_RETURN_OK;	
}

OOIFReturnCode native_WoonServer_UnRegEventListener()
{
	OPL_WOONSERVER_UnRegEventListener();
	return OOIF_RETURN_OK;		
}

OOIFReturnCode native_WoonServer_Start(const char* szFriendlyName)
{
	OPL_WOONSERVER_Start(szFriendlyName);
	return OOIF_RETURN_OK;	
}
OOIFReturnCode native_WoonServer_Stop()
{
	OPL_WOONSERVER_Stop();
	return OOIF_RETURN_OK;	
}

char* native_WoonServer_GetServerName()
{
	memset(g_szWoonServerFriendlyName, 0x00, sizeof(char)*MAX_WOONS_FRIENDLY_NAME_BUF_LEN);
	OPL_WOONSERVER_GetServerName(g_szWoonServerFriendlyName, MAX_WOONS_FRIENDLY_NAME_BUF_LEN);
	return g_szWoonServerFriendlyName;
}

OOIFReturnCode native_WoonServer_SetServerName(const char *szFriendlyName)
{
	OPL_WOONSERVER_SetServerName(szFriendlyName);
	return OOIF_RETURN_OK;	
}

OOIFReturnCode native_WoonServer_TestConnection(OOIFBoolean *retval)
{
	OPL_WOONSERVER_TestConnection();
	*retval = TRUE;
	return OOIF_RETURN_OK;	
}

bool native_WoonServer_IsActivated()
{
    HBOOL bActivated = FALSE;
    OPL_WOONSERVER_IsActivated(&bActivated);
    return (bActivated==TRUE) ? true : false;
}

/*****************************************************************************
 * HWoonServerInfo
 ***************************************************************************** */
OOIFReturnCode native_WoonServerInfo_GetState( WoonServerInfoHandle identifier, OOIFNumber *retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(identifier);
		*retval = (OOIFNumber)content->nState;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}
OOIFReturnCode native_WoonServerInfo_GetLocalIp( WoonServerInfoHandle identifier, const char **retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(identifier);
		*retval = content->szLocalIp;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}	
	return eRet;
}
OOIFReturnCode native_WoonServerInfo_GetPublicIp( WoonServerInfoHandle identifier, const char **retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(identifier);
		*retval = content->szPubIp;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode native_WoonServerInfo_GetExternalIp( WoonServerInfoHandle identifier,const char **retval)
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(identifier);
		*retval = content->szExtIp;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_NULL;
	}
	return eRet;
}
OOIFReturnCode native_WoonServerInfo_GetPublicPort( WoonServerInfoHandle identifier, OOIFNumber *retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(identifier);
		*retval = (OOIFNumber)content->nPubPort;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}
OOIFReturnCode native_WoonServerInfo_GetExternalPort( WoonServerInfoHandle identifier, OOIFNumber *retval )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(identifier) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(identifier);
		*retval = (OOIFNumber)content->nExtPort;
	}
	else
	{
		eRet = OOIF_RETURN_VALUE_UNDEFINED;
	}
	return eRet;
}

OOIFReturnCode native_WoonServerInfo_ReleaseHandle( WoonServerInfoHandle id )
{
	OOIFReturnCode eRet = OOIF_RETURN_OK;
	if(id) 
	{
		WoonServerInfo *content =  static_cast<WoonServerInfo *>(id);
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
