/*************************************************************************************************************
	File 		: HWoon.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (징째Humax징짹) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
#include "HWoon.h"

using namespace Ooif;

#define RELEASE_HANDLE(x)	\
		if(NULL != x){		\
			delete x;		\
			x = NULL;		\
		}					\

typedef enum
{
	MAX_WOONID_LEN  = 512,
	MAX_FILEEXT_LEN = 256,
	MAX_PATH_LEN	= 512,
}Woon_e;

/*******************************************************************/
/************    		HWoon					     *********************/
/*******************************************************************/
HWoon *HWoon::g_pWoonInstance;

HWoon::HWoon()
{
	ENTRY;
	classType = CLASS_WOON;
	g_pWoonInstance = this;
}

HWoon::~HWoon()
{
	ENTRY;
	g_pWoonInstance = NULL;
}

int HWoon::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("server", obj, new HWoonServer(), JSP_GET_VALUE );
	GET_OBJECT("client", obj, new HWoonClient(), JSP_GET_VALUE );

	return JSP_GET_NOTFOUND;
}

int HWoon::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}
/*******************************************************************/
/************    		HWoonClient   *********************/
/*******************************************************************/
HWoonClient *HWoonClient::g_pWoonClientInstance;
HWoonClient::HWoonClient() : EventTarget()
{
	ENTRY;
	classType = CLASS_WOONCLIENT;
	g_pWoonClientInstance = this;
}

HWoonClient::~HWoonClient()
{
	ENTRY;
	g_pWoonClientInstance = NULL;
}

int HWoonClient::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(startScan, 			obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, 				obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(addFriend, 			obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeFriend, 			obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getWoonFriendList, 	obj, "", 	JSP_GET_VALUE_CACHE);

	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(FriendScan);

	return JSP_GET_NOTFOUND;
}

int HWoonClient::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	INTRINSIC_EVENT(FriendScan);

	return JSP_PUT_NOTFOUND;
}

int	HWoonClient::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_WoonClient_StartScan(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int	HWoonClient::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_WoonClient_StopScan(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}

int	HWoonClient::addFriend(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	OOIFBoolean retval 			= TRUE;
	char szWid[MAX_WOONID_LEN]	= {0};

	memset(szWid, '\0', sizeof(szWid));
	strncpy( szWid, 	STRING_VALUE(argv[0]), sizeof(szWid)-1);

	native_WoonClient_AddFriend(szWid, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
int	HWoonClient::removeFriend(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	OOIFBoolean retval 			= TRUE;
	char szWid[MAX_WOONID_LEN]	= {0};

	memset(szWid, '\0', sizeof(szWid));
	strncpy( szWid, 	STRING_VALUE(argv[0]), sizeof(szWid)-1);

	native_WoonClient_RemoveFriend(szWid, &retval);

	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int	HWoonClient::getWoonFriendList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFNumber retval = TRUE;
	int nFriendCount = 0;
	WoonFriendHandle *pWoonFriend = NULL;

	native_WoonClient_GetWoonFriendList(&retval,  &nFriendCount, pWoonFriend);

	HWoonFriendCollection *collection = new HWoonFriendCollection;
	for (int i=0; i<nFriendCount; i++) {
		collection->addItem(new HWoonFriend(pWoonFriend[i]));
	}
	HLIB_STD_MemFree(pWoonFriend);
	RETURN_OBJECT(this_obj, collection ,result, JSP_GET_VALUE);
}

void HWoonClient::callbackEventHandler( unsigned int event, unsigned int state, WoonFriendHandle stFriendInfo )
{
	ENTRY;

	Event *e = NULL;
	jsplugin_obj *context = NULL;
	HWoonClient *pWoonClientInstance = g_pWoonClientInstance;

	if ( NULL == pWoonClientInstance )
	{
		OOIF_LOG_DEBUG("HWoonClient has Null instance !!!! \n");
		return ;
	}

	context = pWoonClientInstance->getHost();
	OOIF_LOG_DEBUG("callbackEventHandler - context(%p), pWoonClientInstance(%p), WoonFriendHandle = %p\n", context, pWoonClientInstance, stFriendInfo);

	if(context){
		e = new HWoonClientResultEvent(context, "FriendScan", event, state, stFriendInfo);
		if( NULL != e)
		{
			OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
			pWoonClientInstance->dispatchEvent(e);
		}
	}

}

/*******************************************************************/
/************    		HWoonClientResultEvent   *********************/
/*******************************************************************/
HWoonClientResultEvent::HWoonClientResultEvent( jsplugin_obj *c, const char *eventName, int event, int state, WoonFriendHandle handleId )
															: Event( c, eventName )
{
	ENTRY;
	m_nEvent 	= event;
	m_handleId 	= handleId;
	m_nState 	= state;

	OOIF_LOG_DEBUG("HWoonClientResultEvent name(%s), state(%d), handleId(%p)\n", eventName, event, handleId);
}

HWoonClientResultEvent::~HWoonClientResultEvent()
{
	ENTRY;
}

int HWoonClientResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("event",				m_nEvent, JSP_GET_VALUE);
    GET_NUMBER("state", 			m_nState, JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HWoonClientResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HWoonClientResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	const int RETURN_PARAM_COUNT = 0;
#if 0
	*argv = new jsplugin_value[RETURN_PARAM_COUNT];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nEvent;

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nState;

	SET_SERIALIZED_OBJECT( new HWoonServerConnectInfo(m_handleId), 2 );
#endif
	return RETURN_PARAM_COUNT;
}

/*******************************************************************/
/************    		HWoonFriend   			 *********************/
/*******************************************************************/
HWoonFriend::HWoonFriend(WoonFriendHandle id) : EventTarget(), WrapperObject<HWoonFriend>(id)
{
	ENTRY;
	classType = CLASS_WOONFRIEND;

}
HWoonFriend::HWoonFriend(const HWoonFriend &original ) : WrapperObject<HWoonFriend>(original.identifier)
{
	ENTRY;
	classType = CLASS_WOONFRIEND;
}

HWoonFriend::~HWoonFriend()
{
	ENTRY;

}
int HWoonFriend::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	return JSP_GET_NOTFOUND;
}

int HWoonFriend::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

void HWoonFriend::releaseNative( WoonFriendHandle id)
{
	ENTRY;
}

/*******************************************************************/
/************    		HWoonContent   			 *********************/
/*******************************************************************/
HWoonContent::HWoonContent(WoonContentHandle id) : EventTarget(), WrapperObject<HWoonContent>(id)
{
	ENTRY;
	classType = CLASS_WOONCONTENT;

}
HWoonContent::HWoonContent(const HWoonContent &original ) : WrapperObject<HWoonContent>(original.identifier)
{
	ENTRY;
	classType = CLASS_WOONCONTENT;
}

HWoonContent::~HWoonContent()
{
	ENTRY;

}

int HWoonContent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	return JSP_GET_NOTFOUND;
}

int HWoonContent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

void HWoonContent::releaseNative( WoonContentHandle id)
{
	ENTRY;
}

/*******************************************************************/
/************    		HWoonServer   			 *********************/
/*******************************************************************/
HWoonServer *HWoonServer::g_pWoonServerInstance;
list<HWoonServer*> HWoonServer::instances;

HWoonServer::HWoonServer() : EventTarget()
{
	ENTRY;
	classType = CLASS_WOONSERVER;
	g_pWoonServerInstance = this;

	native_WoonServer_RegEventListener(HWoonServer::callbackEventHandler);
	HWoonServer::instances.push_back(this);
}
HWoonServer::~HWoonServer()
{
	ENTRY;
	native_WoonServer_UnRegEventListener();
	g_pWoonServerInstance = NULL;
	HWoonServer::instances.remove(this);
}

#if 0 //1 #### listener callback 으로 복수개 파라미터를 받고 싶은 경우 재정의하여 사용해야함.
int HWoonServer::addEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 3);
	if (argc != 2 && argc != 3)
	{
		OOIF_LOG_WARNING("Invalid argument count to addEventListener() %d "
		                 "instead of 2 or 3 - aborting", argc);
		return JSP_CALL_ERROR;
	}
	if (argv[0].type == JSP_TYPE_STRING && argv[1].type == JSP_TYPE_NATIVE_OBJECT)
	{
		NATIVE_CALL(g_pWoonServerInstance->internalAddEventListener(argv[0].u.string, argv[1].u.object, argv[2].u.boolean));
		return JSP_CALL_NO_VALUE;
	}
	else
	{
		OOIF_LOG_WARNING("Invalid argument passed to addEventListener() - aborting");
		return JSP_CALL_ERROR;
	}
}
#endif
int HWoonServer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(start, 				obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, 					obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(testConnection, 		obj, "", 	JSP_GET_VALUE_CACHE);

	GET_FUNCTION(addEventListener, 		obj, "sob", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, 	obj, "sos", JSP_GET_VALUE_CACHE);

	GET_BOOLEAN("started",				native_WoonServer_IsActivated(), 	JSP_GET_VALUE);
	GET_STRING(	"friendlyName",			native_WoonServer_GetServerName(),	JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(ResultEvent);
	INTRINSIC_EVENT_GET(NotifyPinCode);

	return JSP_GET_NOTFOUND;
}

int HWoonServer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("friendlyName", native_WoonServer_SetServerName(STRING_VALUE(value[0])));
	INTRINSIC_EVENT(ResultEvent);
	INTRINSIC_EVENT(NotifyPinCode);
	return JSP_PUT_NOTFOUND;
}

int	HWoonServer::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	OOIFBoolean retval = TRUE;

	native_WoonServer_Start(STRING_VALUE(argv[0]));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
int	HWoonServer::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_WoonServer_Stop();

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}

int	HWoonServer::testConnection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;

	native_WoonServer_TestConnection(&retval);

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);

}
void HWoonServer::callbackEventHandler( unsigned int event, unsigned int state, WoonServerInfoHandle stServerInfo )
{
	ENTRY;

	for (list<HWoonServer*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = NULL;
		//HWoonServer *pWoonServerInstance = g_pWoonServerInstance;
		HWoonServer *pWoonServerInstance = (*it);

		if ( NULL == pWoonServerInstance )
		{
			OOIF_LOG_DEBUG("HWoonServer has Null instance !!!! \n");
			return ;
		}

		//if(g_pWoonServerInstance == pDial)
		//{
		context = pWoonServerInstance->getHost();
		OOIF_LOG_DEBUG("callbackEventHandler - context(%p), pWoonServerInstance(%p), WoonServerInfoHandle = %p\n", context, pWoonServerInstance, stServerInfo);

		if(context)
		{
			switch(event)
			{
				case eNATIVE_WOONS_STARTED:
				case eNATIVE_WOONS_STOPPED:
				case eNATIVE_WOONS_RENAMED:
				case eNATIVE_WOONS_TESTCONNECTION:
					e = new HWoonServerResultEvent(context, "ResultEvent", event, state, stServerInfo);
					break;
				case eNATIVE_WOONS_NOTIFYPINCODE:
					e = new HWoonServerResultEvent(context, "NotifyPinCode", event, state, stServerInfo);
					break;
				case eNATIVE_WOONS_UNKNOWN:
				default:
					break;
			}

			if( NULL != e)
			{
				OOIF_LOG_DEBUG("dispatchEvent(e)'s ----> (%p)\n", e);
				pWoonServerInstance->dispatchEvent(e);
			}
		}
		//}
	}
}
/*******************************************************************/
/************    		HWoonServerResultEvent   *********************/
/*******************************************************************/
HWoonServerResultEvent::HWoonServerResultEvent( jsplugin_obj *c, const char *eventName, int event, int state, void* pstInfo )
															: Event( c, eventName )
{
	ENTRY;
	m_nEvent 	= event;
	m_nState 	= state;
	m_handleId 	= pstInfo;
	OOIF_LOG_DEBUG("HWoonServerResultEvent name(%s), state(%d), handleId(%p)\n", eventName, event, pstInfo);
}

HWoonServerResultEvent::~HWoonServerResultEvent()
{
	ENTRY;
}

int HWoonServerResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("event",				m_nEvent, JSP_GET_VALUE);
    GET_NUMBER("state", 			m_nState, JSP_GET_VALUE);
    GET_STRING("pincode",			(char*)m_handleId, JSP_GET_VALUE);
	GET_OBJECT("serverinfo", obj, 	new HWoonServerConnectInfo(m_handleId), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HWoonServerResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

int HWoonServerResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;

	switch(m_nEvent)
	{
		case eNATIVE_WOONS_STARTED:
		case eNATIVE_WOONS_STOPPED:
		case eNATIVE_WOONS_RENAMED:
		case eNATIVE_WOONS_TESTCONNECTION:
			RETURN_PARAM_COUNT = 3;
			*argv = new jsplugin_value[RETURN_PARAM_COUNT];
			result = *argv;
			result[0].type = JSP_TYPE_NUMBER;
			result[0].u.number = m_nEvent;
			result[1].type = JSP_TYPE_NUMBER;
			result[1].u.number = m_nState;
			SET_SERIALIZED_OBJECT( new HWoonServerConnectInfo(m_handleId), 2 );
			break;
		case eNATIVE_WOONS_NOTIFYPINCODE:
			RETURN_PARAM_COUNT = 2;
			*argv = new jsplugin_value[RETURN_PARAM_COUNT];
			result = *argv;
			result[0].type = JSP_TYPE_NUMBER;
			result[0].u.number = m_nState;
			result[1].type = JSP_TYPE_STRING;
			result[1].u.string = (char*)m_handleId;
			break;
		case eNATIVE_WOONS_UNKNOWN:
		default:
			break;
	}
	return RETURN_PARAM_COUNT;
}



/*******************************************************************/
/************    		HWoonServerConnectInfo    *********************/
/*******************************************************************/
HWoonServerConnectInfo::HWoonServerConnectInfo(WoonServerInfoHandle id) : EventTarget(), WrapperObject<HWoonServerConnectInfo>(id)
{
	ENTRY;
	classType = CLASS_WOONSERVERINFO;

}
HWoonServerConnectInfo::HWoonServerConnectInfo(const HWoonServerConnectInfo &original ) : WrapperObject<HWoonServerConnectInfo>(original.identifier)
{
	ENTRY;
	classType = CLASS_WOONSERVERINFO;
}

HWoonServerConnectInfo::~HWoonServerConnectInfo()
{
	ENTRY;

}

int HWoonServerConnectInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	NATIVE_GET_NUMBER( "state", 		native_WoonServerInfo_GetState(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( "localip", 		native_WoonServerInfo_GetLocalIp(identifier, &RETVAL), 		JSP_GET_VALUE);
	NATIVE_GET_STRING( "publicip", 		native_WoonServerInfo_GetPublicIp(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_STRING( "externalip", 	native_WoonServerInfo_GetExternalIp(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_NUMBER( "publicport", 	native_WoonServerInfo_GetPublicPort(identifier, &RETVAL), 	JSP_GET_VALUE);
	NATIVE_GET_NUMBER( "externalport", 	native_WoonServerInfo_GetExternalPort(identifier, &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HWoonServerConnectInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}
void HWoonServerConnectInfo::releaseNative( WoonServerInfoHandle id)
{
	ENTRY;
	if(NULL != id)
	{
		//NATIVE_WARN_NOK(native_WoonServerInfo_ReleaseHandle(id));
	}
}


#endif // OIPF

