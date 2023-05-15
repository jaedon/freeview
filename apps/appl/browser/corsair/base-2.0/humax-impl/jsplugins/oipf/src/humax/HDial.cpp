/*************************************************************************************************************
	File 		: HDlna.cpp
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
#include <pthread.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "HDial.h"

//#include "hlib.h"

using namespace Ooif;

/*******************************************************************/
/********************      		HDial      *************************/
/*******************************************************************/
HDial*HDial::m_pDialInstance = NULL;

HDial::HDial()
{
	ENTRY;
	classType = CLASS_DIAL;
	m_pDialInstance = this;
}

HDial::~HDial()
{
	ENTRY;
}
int HDial::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("server", obj, new HDialServer(), JSP_GET_VALUE );
	GET_OBJECT("client", obj, new HDialClient(), JSP_GET_VALUE );

	return JSP_GET_NOTFOUND;
}

int HDial::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

/*******************************************************************/
/*******************      		HDialClient ***********************/
/*******************************************************************/

HDialClient *HDialClient::m_pHDialClientInstance;
HDialClient::HDialClient() : EventTarget()
{
	ENTRY;
	classType = CLASS_DIALCLIENT;
	m_pHDialClientInstance = this;
}

HDialClient::~HDialClient()
{
	ENTRY;
	m_pHDialClientInstance = NULL;
}

int HDialClient::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int HDialClient::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}


/*******************************************************************/
/*******************      		HDialServer  ***********************/
/*******************************************************************/
HDialServer *HDialServer::m_pHDialServerInstance;
list<HDialServer*> HDialServer::instances;

HDialServer::HDialServer() : EventTarget()
{
	ENTRY;
	classType = CLASS_DIALSERVER;
	m_pHDialServerInstance = this;
	native_Dial_RegEventListener(HDialServer::callbackEventHandler);
	HDialServer::instances.push_back(this);
}

HDialServer::~HDialServer()
{
	ENTRY;
	m_pHDialServerInstance = NULL;
	HDialServer::instances.remove(this);
	if ( 0 == HDialServer::instances.size() )
	{
	//	native_Dmr_UnRegEventListener();
	}

}

int HDialServer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(start, 				obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, 					obj, "", 	JSP_GET_VALUE_CACHE);

	GET_FUNCTION(registApplication,  	obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unregistApplication, 	obj, "s", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(updateApplicationState, obj, "sn",  JSP_GET_VALUE_CACHE);

	GET_BOOLEAN("started",			native_Dial_IsActivated(), 		JSP_GET_VALUE);
	GET_STRING(	"friendlyName",		native_Dial_GetFriendlyName(),	JSP_GET_VALUE);

 	GET_FUNCTION(addEventListener, obj, "sob", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ReqLaunch);
	INTRINSIC_EVENT_GET(ReqStop);
	INTRINSIC_EVENT_GET(DialStatusEvent);
	return JSP_GET_NOTFOUND;
}

int HDialServer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("friendlyName", native_Dial_SetFriendlyName(STRING_VALUE(value[0])));

	INTRINSIC_EVENT(ReqLaunch);
	INTRINSIC_EVENT(ReqStop);
	INTRINSIC_EVENT(DialStatusEvent);

	return JSP_PUT_NOTFOUND;
}

int	HDialServer::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	native_Dial_Start(&retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HDialServer::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	native_Dial_Stop(&retval);
	RETURN_BOOLEAN(!retval, JSP_CALL_VALUE);
}

int	HDialServer::registApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	char szApp[64] = {0, };
	strncpy( szApp, STRING_VALUE(argv[0]), sizeof(szApp)-1);
	if(OOIF_RETURN_OK != native_Dial_RegistApplication(szApp))
		RETURN_BOOLEAN(0, JSP_CALL_VALUE);

	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}


int	HDialServer::unregistApplication(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);

	char szApp[64] = {0, };
	strncpy( szApp, STRING_VALUE(argv[0]), sizeof(szApp)-1);
	if(OOIF_RETURN_OK != native_Dial_UnregistApplication(szApp))
		RETURN_BOOLEAN(0, JSP_CALL_VALUE);

	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

int	HDialServer::updateApplicationState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	unsigned int state = NUMBER_VALUE(argv[1]);
	char szApp[64] = {0, };
	strncpy( szApp, STRING_VALUE(argv[0]), sizeof(szApp)-1);
	if(OOIF_RETURN_OK != native_Dial_ApplicationStateUpdate(szApp, state))
		RETURN_BOOLEAN(0, JSP_CALL_VALUE);

	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

void HDialServer::callbackEventHandler(unsigned int event,  char *app, void *data)
{
	ENTRY;

	for (list<HDialServer*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		HDialServer *pDial = (*it);
		jsplugin_obj *context = NULL;

		if ( NULL == pDial)
		{
			OOIF_LOG_DEBUG("HDialServer has Null instance !!!! \n");
			return ;
		}

		context = pDial->getHost();
		if(context)
		{
			switch( event )
			{
				case eDIAL_APP_LAUNCH:
					e = new HDialServerResultEvent(context, "ReqLaunch", 		event, app, data);
					break;

				case eDIAL_APP_STOP:
					e = new HDialServerResultEvent(context, "ReqStop", 			event, app, NULL);
					break;

					//	case eDIAL_APP_DELETE: /*  Option  */

				case eDIAL_START:
				case eDIAL_STOP:
					e = new HDialServerResultEvent(context, "DialStatusEvent", 	event, NULL, NULL);
					break;

				default :
					return ;
			}

			if( NULL != e)
			{
				OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pDial, e);
				pDial->dispatchEvent(e);
			}
		}
	}
}

/*******************************************************************/
/***********    		HDialServerResultEvent   *******************/
/*******************************************************************/
HDialServerResultEvent::HDialServerResultEvent( jsplugin_obj *c, const char *eventName, int event, char *app, void *data) : Event( c, eventName )
{
	ENTRY;

	m_nEvent = event;
	if(app) m_szApplicationName = HLIB_STD_StrDup(app); else m_szApplicationName = NULL;
	if(data) m_szUrl = HLIB_STD_StrDup((char *)data); else m_szUrl = NULL;

	OOIF_LOG_DEBUG("HDialServerResultEvent - event name(%s), event(%d), app(%s), url(%s)\n", eventName, event, app, (char *)data);
}

HDialServerResultEvent:: ~HDialServerResultEvent()
{
	ENTRY;
	if(m_szApplicationName) HLIB_STD_MemFree(m_szApplicationName);
	if(m_szUrl) HLIB_STD_MemFree(m_szUrl);
}

int HDialServerResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("status",		getEvent(), 			JSP_GET_VALUE);
	GET_STRING("name",			getApplicationName(),	JSP_GET_VALUE);
	GET_STRING("url",			getUrl(), 				JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HDialServerResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int HDialServerResultEvent::getEvent(void)
{
	return m_nEvent;
}

char *HDialServerResultEvent::getApplicationName(void)
{
	return m_szApplicationName;
}

char *HDialServerResultEvent::getUrl(void)
{
	return (char *)m_szUrl;
}

int HDialServerResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;
	switch(m_nEvent)
	{
		case eDIAL_APP_LAUNCH:
			{
				RETURN_PARAM_COUNT = 2;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;

				result[0].type = JSP_TYPE_STRING;
				result[0].u.string = m_szApplicationName;

				result[1].type = JSP_TYPE_STRING;
				result[1].u.string = (char *)m_szUrl;
			}
			break;

		case eDIAL_APP_STOP:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;

				result[0].type = JSP_TYPE_STRING;
				result[0].u.string = m_szApplicationName;
			}
			break;

		case eDIAL_STOP:
		case eDIAL_START:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;

				result[0].type = JSP_TYPE_NUMBER;
				result[0].u.number = (m_nEvent == eDIAL_START)? 0:1;
			}
			break;


		default :
			break;
	}

	return RETURN_PARAM_COUNT;
}

#endif // OIPF

