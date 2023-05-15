/*************************************************************************************************************
	File 		: HSamba.cpp
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


#ifdef OIPF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "HSamba.h"
#include "NativeHSamba.h"

using namespace Ooif;

/*****************************************************************************
 * HSambaManager
 ******************************************************************************/
HSambaManager* HSambaManager::m_pSambaInstance = NULL;
list<HSambaManager*> HSambaManager::instances;

HSambaManager::HSambaManager()
{
	ENTRY;
	classType = CLASS_SAMBA_MANAGER;
	if ( 0 == HSambaManager::instances.size() )
	{
		native_Sambac_AddListener(HSambaManager::callbackEventHandler);
	}
	HSambaManager::instances.push_back(this);
}

HSambaManager::~HSambaManager()
{
	ENTRY;
	HSambaManager::instances.remove(this);
	if ( 0 == HSambaManager::instances.size() )
	{
		native_Sambac_RemoveListener();
	}
}

int HSambaManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("server", obj, new HSambaServer(native_SambaManager_GetServerHandle()), JSP_GET_VALUE);
	GET_OBJECT("client", obj, new HSambaClient(), JSP_GET_VALUE );

	GET_FUNCTION(startScan, 			obj, "", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, 				obj, "", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(mount, 				obj, "o", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unmount, 				obj, "o",	 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unmountAll,			obj, "",		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAvailableList, 		obj, "", 		JSP_GET_VALUE_CACHE);

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ScanResult);
	INTRINSIC_EVENT_GET(MountResult);

	return JSP_GET_NOTFOUND;
}

int HSambaManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(ScanResult);
	INTRINSIC_EVENT(MountResult);

	return JSP_PUT_NOTFOUND;
}

int HSambaManager::unmountAll(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaManager );

	native_Sambac_UnmountAll(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
int HSambaManager::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaManager );

	native_Sambac_StartScan(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int HSambaManager::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaManager );

	native_Sambac_StopScan(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int HSambaManager::mount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaManager );
	HSambaSearchedServer *d = OBJECT_VALUE_OR_THROW(d, argv[0], HSambaSearchedServer);
	NATIVE_CALL(native_Sambac_Mount(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int HSambaManager::unmount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaManager );
	HSambaSearchedServer *d = OBJECT_VALUE_OR_THROW(d, argv[0], HSambaSearchedServer);
	NATIVE_CALL(native_Sambac_Unmount(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
int HSambaManager::getAvailableList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	int nServerCount = 0;
	SambaServerHandle *pSambaServer = NULL;
	OOIFBoolean retval = TRUE;
	native_Sambac_GetAvailableServerList(&nServerCount, &pSambaServer, &retval);

	HSambaSearchedServerCollection *collection = new HSambaSearchedServerCollection;

	if(pSambaServer)
	{
		for (int i=0; i<nServerCount; i++) {
				collection->addItem(new HSambaSearchedServer(pSambaServer[i]));
		}
		HLIB_STD_MemFree(pSambaServer);
		pSambaServer = NULL;
	}
	RETURN_OBJECT(this_obj, collection ,result, JSP_GET_VALUE);

}

void HSambaManager::callbackEventHandler( int event, SambaServerHandle handle)
{
	ENTRY;
	for (list<HSambaManager*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		OOIF_LOG_DEBUG("[callbackEventHandler] received event [%d]\n", event);
		Event *e = NULL;
		jsplugin_obj *context = NULL;
		HSambaManager *pSambaInstance = (*it);

		if ( NULL == pSambaInstance )
		{
			OOIF_LOG_DEBUG("HSambaManager has Null instance !!!! \n");
			return ;
		}
		if(m_pSambaInstance == pSambaInstance)
		{
			context = pSambaInstance->getHost();
			OOIF_LOG_DEBUG("callbackEventHandler - context(%p), pSambaInstance(%p), SambaServerHandle = %p\n", context, pSambaInstance, handle);

			if(context){
				switch( event )
				{
					case eNATIVE_SAMBAC_SCAN_STARTED:
					case eNATIVE_SAMBAC_SCAN_ADD:
					case eNATIVE_SAMBAC_SCAN_REMOVE:
					case eNATIVE_SAMBAC_SCAN_COMPLETE:
					case eNATIVE_SAMBAC_SCAN_FAIL:
						e = new HSambaResultEvent(context, "ScanResult", event, handle);
						break;
					case eNATIVE_SAMBAC_MOUNT_OK:
					case eNATIVE_SAMBAC_MOUNT_FAIL:
					case eNATIVE_SAMBAC_UMOUNT_OK:
					case eNATIVE_SAMBAC_UMOUNT_FAIL:
						e = new HSambaResultEvent(context, "MountResult", event, handle);
						break;
					default :
						return ;
				}
				if( NULL != e)
				{
					OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
					pSambaInstance->dispatchEvent(e);
				}
			}
		}
	}
}

/*****************************************************************************
 * HSambaServer
 ******************************************************************************/
list<HSambaServer*> HSambaServer::instances;
SambaServerHandle  HSambaServer::identifier = NULL;
HSambaServer::HSambaServer( SambaServerHandle handle )
{
	ENTRY;
	classType = CLASS_SAMBA_SERVER;
	if ( 0 == HSambaServer::instances.size() )
	{
		native_SambaServer_AddListener( callbackEventHandler );
	}
	HSambaServer::instances.push_back(this);

	identifier = handle;
}

HSambaServer::~HSambaServer()
{
	ENTRY;
	HSambaServer::instances.remove(this);
	if ( 0 == HSambaServer::instances.size() )
	{
		native_SambaServer_RemoveListener( callbackEventHandler );
	}
}

int HSambaServer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING("serverName", native_SambaServer_GetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("workgroup", native_SambaServer_GetWorkgroup(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("started", native_SambaServer_IsStarted(identifier, &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(start, 			obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, 				obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPassword, 		obj, "sss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getPassword, 		obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isValidPassword, 	obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setShareFolder, 	obj, "sss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getShareFolder, 	obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setPublicAccess, 	obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getPublicAccess, 	obj, "s", JSP_GET_VALUE_CACHE);

	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	GET_OBJECT("users", obj, getUsers(), JSP_GET_VALUE );
	INTRINSIC_EVENT_GET(StateUpdated);

	return JSP_GET_NOTFOUND;
}

int HSambaServer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	OOIFNumber retval = 0;
	ENTRY;
	NATIVE_SET("serverName", native_SambaServer_SetName(identifier, STRING_VALUE(value[0]), &retval ));
	NATIVE_SET("workgroup", native_SambaServer_SetWorkgroup(identifier, STRING_VALUE(value[0]), &retval ));
	INTRINSIC_EVENT(StateUpdated);
	return JSP_PUT_NOTFOUND;
}

StringCollection * HSambaServer::getUsers()
{
	StringCollection *collection = new StringCollection;
	collection->addItem(HLIB_STD_StrDup("admin"));
	return collection;
}


int HSambaServer::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL( native_SambaServer_Start(identifier, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HSambaServer::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL( native_SambaServer_Stop(identifier, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HSambaServer::getPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	const char *retval;
	NATIVE_CALL( native_SambaServer_GetPassword( identifier, &retval) );
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int HSambaServer::setPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	const char *pUserName = NULL;
	const char *pCurPasswd = NULL;
	const char *pNewPasswd = NULL;
	ENTRY;
	CHECK_ARGUMENT_COUNT(3);

	pUserName = STRING_VALUE(argv[0]);
	pCurPasswd = STRING_VALUE(argv[1]);
	pNewPasswd = STRING_VALUE(argv[2]);

	NATIVE_CALL( native_SambaServer_SetPassword( identifier, pUserName, pCurPasswd, pNewPasswd, &retval ));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HSambaServer::isValidPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFBoolean retval = 0;
	const char *pUserName = NULL;
	const char *pCurPasswd = NULL;
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	pUserName = STRING_VALUE(argv[0]);
	pCurPasswd = STRING_VALUE(argv[1]);

	NATIVE_CALL( native_SambaServer_IsValidPassword( identifier, pUserName, pCurPasswd, &retval ));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int HSambaServer::setShareFolder(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	const char *pUserName = NULL;
	const char *pPath = NULL;
	const char *pOpt = NULL;
	ENTRY;
	CHECK_ARGUMENT_COUNT(3);
	pUserName = STRING_VALUE(argv[0]);
	pPath = STRING_VALUE(argv[1]);
	pOpt = STRING_VALUE(argv[2]);

	NATIVE_CALL( native_SambaServer_SetShareFolder( identifier, pUserName, pPath, pOpt, &retval ));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HSambaServer::getShareFolder(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	const char *pUserName = NULL;
	void **ppList = NULL;
	void *pShareHandle = NULL;
	int	nCount = 0;

	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	pUserName = STRING_VALUE(argv[0]);

	NATIVE_CALL( native_SambaServer_GetShareFolder( identifier, pUserName, &nCount, &pShareHandle ) );
	ppList = (void **)pShareHandle;

	HSmbsShareFolderCollection *collection = new HSmbsShareFolderCollection;
	if (ppList)
	{
		for (int i =0; i<nCount; i++)
		{
			collection->addItem( new HSmbsShareFolder(ppList[i]));
		}
		HLIB_STD_MemFree(ppList);
		ppList = NULL;
	}

	RETURN_OBJECT( this_obj, collection, result, JSP_GET_VALUE );
}

int HSambaServer::setPublicAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	const char *pUserName = NULL;
	OOIFBoolean bAccess = FALSE;
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	pUserName = STRING_VALUE(argv[0]);
	bAccess = BOOLEAN_VALUE(argv[1]);

	NATIVE_CALL( native_SambaServer_SetPublicAccess( identifier, pUserName, bAccess, &retval ));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HSambaServer::getPublicAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFBoolean retval = 0;
	const char *pUserName = NULL;

	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	pUserName = STRING_VALUE(argv[0]);

	NATIVE_CALL( native_SambaServer_GetPublicAccess( identifier, pUserName, &retval ));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
void HSambaServer::callbackEventHandler( int event, int result )
{
	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HSambaServer *pSmbsInstacne = NULL;

	for (list<HSambaServer*>::iterator it=instances.begin(); it!=instances.end(); ++it)
	{
		pSmbsInstacne = (*it);
		if ( NULL == pSmbsInstacne )
			continue;
		context = pSmbsInstacne->getHost();
		if (context) {
			switch ( event )
			{
				case 0 :
					break;
				case 1 :
					break;
				case 2 :
					break;
				default :
					return ;
			}

			e = new HSambaResultEvent(context, "StateUpdated", event );
			if ( NULL != e )
			{
				pSmbsInstacne->dispatchEvent(e);
			}
		}
	}
}

/*****************************************************************************
 * HSmbsShareFolder
 ******************************************************************************/
HSmbsShareFolder::HSmbsShareFolder( void *id ) : WrapperObject<HSmbsShareFolder>(id)
{
	ENTRY;
	classType = CLASS_SAMBA_SERVER_SHARE_FOLDER;
}

HSmbsShareFolder::HSmbsShareFolder( const HSmbsShareFolder &original ) : WrapperObject<HSmbsShareFolder>(original.identifier)
{
	ENTRY;
	classType = CLASS_SAMBA_SERVER_SHARE_FOLDER;
}

HSmbsShareFolder::~HSmbsShareFolder()
{
	ENTRY;
}

int HSmbsShareFolder::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING("fullPath", native_SmbsShareFolder_GetPath(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_SmbsShareFolder_GetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("user", native_SmbsShareFolder_GetUser(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("writable", native_SmbsShareFolder_GetWritable(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isValid", native_SmbsShareFolder_IsValid(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HSmbsShareFolder::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("writable", native_SmbsShareFolder_SetWritable( identifier, BOOLEAN_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}

void HSmbsShareFolder::releaseNative( void * id)
{
	ENTRY;
	if(NULL != id)
	{
		NATIVE_WARN_NOK(native_SmbsShareFolder_ReleaseHandle(id));
	}
}

/*****************************************************************************
 * HSambaClient
 ******************************************************************************/
HSambaClient* HSambaClient::m_pSambaInstance = NULL;
list<HSambaClient*> HSambaClient::instances;

HSambaClient::HSambaClient()
{
	ENTRY;
	classType = CLASS_SAMBA_CLIENT;
	if ( 0 == HSambaClient::instances.size() )
	{
		native_Sambac_AddListener(HSambaClient::callbackEventHandler);
	}
	HSambaClient::instances.push_back(this);
}

HSambaClient::~HSambaClient()
{
	ENTRY;
	HSambaClient::instances.remove(this);
	if ( 0 == HSambaClient::instances.size() )
	{
		native_Sambac_RemoveListener();
	}
}

int HSambaClient::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(startScan, 			obj, "", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, 				obj, "", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(mount, 				obj, "o", 		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unmount, 				obj, "o",	 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unmountAll,			obj, "",		JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAvailableList, 		obj, "", 		JSP_GET_VALUE_CACHE);

	/* EventHandler*/
	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ScanResult);
	INTRINSIC_EVENT_GET(MountResult);

	return JSP_GET_NOTFOUND;
}
int HSambaClient::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(ScanResult);
	INTRINSIC_EVENT(MountResult);

	return JSP_PUT_NOTFOUND;
}

int HSambaClient::unmountAll(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaClient );

	native_Sambac_UnmountAll(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
int HSambaClient::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaClient );

	native_Sambac_StartScan(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int HSambaClient::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaClient );

	native_Sambac_StopScan(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int HSambaClient::mount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaClient );
	HSambaSearchedServer *d = OBJECT_VALUE_OR_THROW(d, argv[0], HSambaSearchedServer);
	NATIVE_CALL(native_Sambac_Mount(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int HSambaClient::unmount(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;
	m_pSambaInstance = EXTRACT_OBJECT_OR_THROW( m_pSambaInstance, this_obj, HSambaClient );
	HSambaSearchedServer *d = OBJECT_VALUE_OR_THROW(d, argv[0], HSambaSearchedServer);
	NATIVE_CALL(native_Sambac_Unmount(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
int HSambaClient::getAvailableList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);

	int nServerCount = 0;
	SambaServerHandle *pSambaServer = NULL;
	OOIFBoolean retval = TRUE;
	native_Sambac_GetAvailableServerList(&nServerCount, &pSambaServer, &retval);

	HSambaSearchedServerCollection *collection = new HSambaSearchedServerCollection;

	if(pSambaServer)
	{
		for (int i=0; i<nServerCount; i++) {
				collection->addItem(new HSambaSearchedServer(pSambaServer[i]));
		}
		HLIB_STD_MemFree(pSambaServer);
		pSambaServer = NULL;
	}
	RETURN_OBJECT(this_obj, collection ,result, JSP_GET_VALUE);

}

void HSambaClient::callbackEventHandler( int event, SambaServerHandle handle)
{
	ENTRY;
	for (list<HSambaClient*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		OOIF_LOG_DEBUG("[callbackEventHandler] received event [%d]\n", event);
		Event *e = NULL;
		jsplugin_obj *context = NULL;
		HSambaClient *pSambaInstance = (*it);

		if ( NULL == pSambaInstance )
		{
			OOIF_LOG_DEBUG("HSambaClient has Null instance !!!! \n");
			return ;
		}
		if(m_pSambaInstance == pSambaInstance)
		{
			context = pSambaInstance->getHost();
			OOIF_LOG_DEBUG("callbackEventHandler - context(%p), pSambaInstance(%p), SambaServerHandle = %p\n", context, pSambaInstance, handle);

			if(context){
				switch( event )
				{
					case eNATIVE_SAMBAC_SCAN_STARTED:
					case eNATIVE_SAMBAC_SCAN_ADD:
					case eNATIVE_SAMBAC_SCAN_REMOVE:
					case eNATIVE_SAMBAC_SCAN_COMPLETE:
					case eNATIVE_SAMBAC_SCAN_FAIL:
						e = new HSambaResultEvent(context, "ScanResult", event, handle);
						break;
					case eNATIVE_SAMBAC_MOUNT_OK:
					case eNATIVE_SAMBAC_MOUNT_FAIL:
					case eNATIVE_SAMBAC_UMOUNT_OK:
					case eNATIVE_SAMBAC_UMOUNT_FAIL:
						e = new HSambaResultEvent(context, "MountResult", event, handle);
						break;
					default :
						return ;
				}
				if( NULL != e)
				{
					OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
					pSambaInstance->dispatchEvent(e);
				}
			}
		}
	}
}

/*****************************************************************************
 * HSambeScanResultEvent
 ******************************************************************************/

HSambaResultEvent::HSambaResultEvent( jsplugin_obj *c, const char *eventName, int state, SambaServerHandle handleId )
								: Event( c, eventName ), m_handleId( handleId ), m_nState(state)
{
	ENTRY;

}
HSambaResultEvent::~HSambaResultEvent()
{
	ENTRY;
}

int HSambaResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("server", obj, new HSambaSearchedServer(m_handleId), JSP_GET_VALUE);
    GET_NUMBER("state",  		m_nState, 					JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}
int HSambaResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}
int HSambaResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;

	switch(m_nState)
	{
		case eNATIVE_SAMBAC_SCAN_STARTED:
		case eNATIVE_SAMBAC_SCAN_ADD:
		case eNATIVE_SAMBAC_SCAN_REMOVE:
		case eNATIVE_SAMBAC_SCAN_COMPLETE:
		case eNATIVE_SAMBAC_SCAN_FAIL:
		case eNATIVE_SAMBAC_MOUNT_OK:
		case eNATIVE_SAMBAC_MOUNT_FAIL:
		case eNATIVE_SAMBAC_UMOUNT_OK:
		case eNATIVE_SAMBAC_UMOUNT_FAIL:
			{
				RETURN_PARAM_COUNT = 2;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;
				SET_SERIALIZED_OBJECT( new HSambaSearchedServer(m_handleId), 0 );
				result[1].type = JSP_TYPE_NUMBER;
				result[1].u.number = m_nState;
			}
			break;
		default:
			break;
	}
	return RETURN_PARAM_COUNT;
}


/*****************************************************************************
 * HSambeServer
 ******************************************************************************/

HSambaSearchedServer::HSambaSearchedServer(SambaServerHandle id) : WrapperObject<HSambaSearchedServer>(id)
{
	ENTRY;
	classType = CLASS_SAMBA_SEARCHED_SERVER;

}
HSambaSearchedServer::HSambaSearchedServer(const HSambaSearchedServer &original) : WrapperObject<HSambaSearchedServer>(original.identifier)
{
	ENTRY;
	classType = CLASS_SAMBA_SEARCHED_SERVER;

}

HSambaSearchedServer::~HSambaSearchedServer()
{
	ENTRY;

}
int HSambaSearchedServer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING( "serverName", 	native_Sambac_GetServerName(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "sharedName", 	native_Sambac_GetSharedName(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "ip", 			native_Sambac_GetServerIp(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isMounted", 	natvie_Sambac_GetMountState(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "mountPath", 	native_Sambac_GetMountPath(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "log",			native_Sambac_GetLogMsg(identifier, &RETVAL),		JSP_GET_VALUE);
	NATIVE_GET_STRING( "id", 			native_Sambac_GetId(identifier, &RETVAL), 			JSP_GET_VALUE);
	NATIVE_GET_STRING( "password", 		native_Sambac_GetPassword(identifier, &RETVAL), 	JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;

}
int HSambaSearchedServer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("id", 		native_Sambac_SetId(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("password", 	native_Sambac_SetPassword(identifier, STRING_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}
void HSambaSearchedServer::releaseNative( SambaServerHandle id)
{
	ENTRY;
	if(NULL != id)
	{
		NATIVE_WARN_NOK(native_Sambac_ReleaseHandle(id));
	}
}

#endif // OIPF

