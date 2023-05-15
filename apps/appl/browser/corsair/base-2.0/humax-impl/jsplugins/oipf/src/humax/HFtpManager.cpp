/*************************************************************************************************************
	File 		: HFtpManager.cpp
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
#include "NativeHFtpManager.h"
#include "HFtpManager.h"

using namespace Ooif;

/*****************************************************************************
 * HFtpManager
 ******************************************************************************/

HFtpManager::HFtpManager()
{
	ENTRY;
	classType = CLASS_FTP_MANAGER;
}

HFtpManager::~HFtpManager()
{
	ENTRY;
}

int HFtpManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_OBJECT("server", obj, new HFtpServer( native_FtpManager_GetServerHandle() ), JSP_GET_VALUE);
	//GET_OBJECT("client", obj, NULL, JSP_GET_VALUE );
	return JSP_GET_NOTFOUND;
}
int HFtpManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

/*****************************************************************************
 * HFtpServer
 ******************************************************************************/
list<HFtpServer*> HFtpServer::instances;
FtpServerHandle HFtpServer::identifier = NULL;
HFtpServer::HFtpServer( FtpServerHandle handle )
{
	ENTRY;
	classType = CLASS_FTP_SERVER;
	if ( 0 == HFtpServer::instances.size() )
	{
		native_FtpServer_AddListener( callbackEventHandler );
	}
	HFtpServer::instances.push_back(this);
	identifier = handle;
}

HFtpServer::~HFtpServer()
{
	ENTRY;
	HFtpServer::instances.remove(this);
	if ( 0 == HFtpServer::instances.size() )
	{
		native_FtpServer_RemoveListener( callbackEventHandler );
	}
}

int HFtpServer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING("serverName", native_FtpServer_GetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("started", native_FtpServer_IsStarted(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("port", native_FtpServer_GetPort(identifier, &RETVAL), JSP_GET_VALUE);
	GET_OBJECT("users", obj, getUsers(), JSP_GET_VALUE );

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

	INTRINSIC_EVENT_GET(StateUpdated);
	return JSP_GET_NOTFOUND;
}

int HFtpServer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	OOIFNumber retval = 0;
	ENTRY;
	NATIVE_SET("serverName", native_FtpServer_SetName(identifier, STRING_VALUE(value[0]), &retval ));
	NATIVE_SET("port", native_FtpServer_SetPort(identifier, NUMBER_VALUE(value[0]), &retval ));
	INTRINSIC_EVENT(StateUpdated);
	return JSP_PUT_NOTFOUND;
}

StringCollection * HFtpServer::getUsers()
{
	StringCollection *collection = new StringCollection;
	collection->addItem(HLIB_STD_StrDup("admin"));
	return collection;
}

int HFtpServer::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL( native_FtpServer_Start(identifier, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HFtpServer::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL( native_FtpServer_Stop(identifier, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HFtpServer::getPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	const char *retval;
	NATIVE_CALL( native_FtpServer_GetPassword( identifier, &retval ));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int HFtpServer::setPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
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

	NATIVE_CALL( native_FtpServer_SetPassword( identifier, pUserName, pCurPasswd, pNewPasswd, &retval ));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HFtpServer::isValidPassword(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFBoolean retval = 0;
	const char *pUserName = NULL;
	const char *pCurPasswd = NULL;
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	pUserName = STRING_VALUE(argv[0]);
	pCurPasswd = STRING_VALUE(argv[1]);

	NATIVE_CALL( native_FtpServer_IsValidPassword( identifier, pUserName, pCurPasswd, &retval ));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int HFtpServer::setShareFolder(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
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

	NATIVE_CALL( native_FtpServer_SetShareFolder( identifier, pUserName, pPath, pOpt, &retval ));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HFtpServer::getShareFolder(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	const char *pUserName = NULL;
	void **ppList = NULL;
	void *pShareHandle = NULL;
	int	nCount = 0;

	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	pUserName = STRING_VALUE(argv[0]);

	NATIVE_CALL( native_FtpServer_GetShareFolder( identifier, pUserName, &nCount, &pShareHandle ) );
	ppList = (void **)pShareHandle;

	HFtpsShareFolderCollection *collection = new HFtpsShareFolderCollection;
	if (ppList)
	{
		for (int i =0; i<nCount; i++)
		{
			collection->addItem( new HFtpsShareFolder(ppList[i]));
		}
		HLIB_STD_MemFree(ppList);
		ppList = NULL;
	}

	RETURN_OBJECT( this_obj, collection, result, JSP_GET_VALUE );
}

int HFtpServer::setPublicAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFNumber retval = 0;
	const char *pUserName = NULL;
	OOIFBoolean bAccess = FALSE;
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);
	pUserName = STRING_VALUE(argv[0]);
	bAccess = BOOLEAN_VALUE(argv[1]);

	NATIVE_CALL( native_FtpServer_SetPublicAccess( identifier, pUserName, bAccess, &retval ));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HFtpServer::getPublicAccess(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFBoolean retval = 0;
	const char *pUserName = NULL;

	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	pUserName = STRING_VALUE(argv[0]);

	NATIVE_CALL( native_FtpServer_GetPublicAccess( identifier, pUserName, &retval ));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void HFtpServer::callbackEventHandler( int event, int result )
{
	Event *e = NULL;
	int oipfEvt = 0;
	jsplugin_obj *context = NULL;
	HFtpServer *pFtpsInstacne = NULL;

	for (list<HFtpServer*>::iterator it=instances.begin(); it!=instances.end(); ++it)
	{
		pFtpsInstacne = (*it);
		if ( NULL == pFtpsInstacne )
			continue;

		context = pFtpsInstacne->getHost();

		if (context) {
			switch ( event )
			{
				case 0 :
					oipfEvt = eSERVER_START;
					break;
				case 1 :
					oipfEvt = eSERVER_STOP;
					break;
				case 2 :
					oipfEvt = eCONFIG_CHANGE;
					break;
				default :
					return ;
			}
			e = new HFtpServerResultEvent(context, "StateUpdated", oipfEvt );

			if ( NULL != e )
			{
				pFtpsInstacne->dispatchEvent(e);
			}
		}
	}
}

/*****************************************************************************
 * HSambeScanResultEvent
 ******************************************************************************/

HFtpServerResultEvent::HFtpServerResultEvent( jsplugin_obj *c, const char *eventName, int state )
								: Event( c, eventName ),  m_nState(state)
{
	ENTRY;

}

HFtpServerResultEvent::~HFtpServerResultEvent()
{
	ENTRY;
}

int HFtpServerResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
    GET_NUMBER("state",  		m_nState, 					JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}
int HFtpServerResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}
int HFtpServerResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;

	switch(m_nState)
	{

		default:
			break;
	}
	return RETURN_PARAM_COUNT;
}

/*****************************************************************************
 * HFtpsShareFolder
 ******************************************************************************/
HFtpsShareFolder::HFtpsShareFolder( void *id ) : WrapperObject<HFtpsShareFolder>(id)
{
	ENTRY;
	classType = CLASS_FTP_SERVER_SHARE_FOLDER;
}

HFtpsShareFolder::HFtpsShareFolder( const HFtpsShareFolder &original ) : WrapperObject<HFtpsShareFolder>(original.identifier)
{
	ENTRY;
	classType = CLASS_FTP_SERVER_SHARE_FOLDER;
}

HFtpsShareFolder::~HFtpsShareFolder()
{
	ENTRY;
}

int HFtpsShareFolder::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_STRING("fullPath", native_FtpsShareFolder_GetPath(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_FtpsShareFolder_GetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("user", native_FtpsShareFolder_GetUser(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("writable", native_FtpsShareFolder_GetWritable(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isValid", native_FtpsShareFolder_IsValid(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HFtpsShareFolder::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("writable", native_FtpsShareFolder_SetWritable( identifier, BOOLEAN_VALUE(value[0])));
	return JSP_PUT_NOTFOUND;
}

void HFtpsShareFolder::releaseNative( void * id)
{
	ENTRY;
	if(NULL != id)
	{
		NATIVE_WARN_NOK(native_FtpsShareFolder_ReleaseHandle(id));
	}
}

#endif // OIPF

