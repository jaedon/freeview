/*************************************************************************************************************
	File 		: HSatip.cpp
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
#include "HSatip.h"

using namespace Ooif;

/*******************************************************************/
/********************      		HSatip      *************************/
/*******************************************************************/
HSatip*HSatip::m_pSatipInstance = NULL;

HSatip::HSatip()
{
	ENTRY;
	classType = CLASS_SATIP;
	m_pSatipInstance = this;
}

HSatip::~HSatip()
{
	ENTRY;
}
int HSatip::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_OBJECT("server", obj, new HSatipServer(), JSP_GET_VALUE );
	GET_OBJECT("client", obj, new HSatipClient(), JSP_GET_VALUE );

	return JSP_GET_NOTFOUND;
}

int HSatip::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

/*******************************************************************/
/*******************      		HSatipClient ***********************/
/*******************************************************************/

HSatipClient *HSatipClient::m_pHSatipClientInstance;
HSatipClient::HSatipClient() : EventTarget()
{
	ENTRY;
	classType = CLASS_SATIPCLIENT;
	m_pHSatipClientInstance = this;
}

HSatipClient::~HSatipClient()
{
	ENTRY;
	m_pHSatipClientInstance = NULL;
}

int HSatipClient::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	return JSP_GET_NOTFOUND;
}

int HSatipClient::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}


/*******************************************************************/
/*******************      		HSatipServer  ***********************/
/*******************************************************************/
HSatipServer *HSatipServer::m_pHSatipServerInstance;
list<HSatipServer*> HSatipServer::instances;

HSatipServer::HSatipServer() : EventTarget()
{
	ENTRY;
	classType = CLASS_SATIPSERVER;
	m_pHSatipServerInstance = this;
	native_Satip_RegEventListener(HSatipServer::callbackEventHandler);
	HSatipServer::instances.push_back(this);
}

HSatipServer::~HSatipServer()
{
	ENTRY;
	m_pHSatipServerInstance = NULL;
	HSatipServer::instances.remove(this);
	if ( 0 == HSatipServer::instances.size() )
	{
	//	native_Dmr_UnRegEventListener();
	}

}

int HSatipServer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(start, 				obj, "", 	JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, 					obj, "", 	JSP_GET_VALUE_CACHE);

	GET_BOOLEAN("started",			native_Satip_IsActivated(), 		JSP_GET_VALUE);
	GET_STRING(	"friendlyName",		native_Satip_GetFriendlyName(),	JSP_GET_VALUE);

 	GET_FUNCTION(addEventListener, obj, "sob", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(Started);
	INTRINSIC_EVENT_GET(Stopped);

	INTRINSIC_EVENT_GET(SatipStatusEvent);
	return JSP_GET_NOTFOUND;
}

int HSatipServer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("friendlyName", native_Satip_SetFriendlyName(STRING_VALUE(value[0])));

	INTRINSIC_EVENT(Started);
	INTRINSIC_EVENT(Stopped);

	return JSP_PUT_NOTFOUND;
}

int	HSatipServer::start(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	native_Satip_Start(&retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

int	HSatipServer::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(0);
	OOIFBoolean retval = TRUE;
	native_Satip_Stop(&retval);
	RETURN_BOOLEAN(!retval, JSP_CALL_VALUE);
}

void HSatipServer::callbackEventHandler(unsigned int event,  char *app, void *data)
{
	ENTRY;

	for (list<HSatipServer*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		HSatipServer *pSatip = (*it);
		jsplugin_obj *context = NULL;

		if ( NULL == pSatip)
		{
			OOIF_LOG_DEBUG("HSatipServer has Null instance !!!! \n");
			return ;
		}

		if(m_pHSatipServerInstance == pSatip)
		{
			context = pSatip->getHost();
			if(context)
			{
				switch( event )
				{
					case eSATIP_START:
						e = new HSatipServerResultEvent(context, "Started", 	event, NULL);
						break;

					case eSATIP_STOP:
						e = new HSatipServerResultEvent(context, "Stopped", 	event, NULL);
						break;

					default :
						return ;
				}

				if( NULL != e)
				{
					OOIF_LOG_DEBUG("dispatchEvent(%p)'s ----> (%p)", pSatip, e);
					pSatip->dispatchEvent(e);
				}
			}
		}
	}
}

/*******************************************************************/
/***********    		HSatipServerResultEvent   *******************/
/*******************************************************************/
HSatipServerResultEvent::HSatipServerResultEvent( jsplugin_obj *c, const char *eventName, int event, void *data) : Event( c, eventName )
{
	ENTRY;

	m_nEvent = event;

	OOIF_LOG_DEBUG("HSatipServerResultEvent - event name(%s), event(%d)\n", eventName, event, (char *)data);
}

HSatipServerResultEvent:: ~HSatipServerResultEvent()
{
	ENTRY;
}

int HSatipServerResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("status",		getEvent(), 			JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int HSatipServerResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int HSatipServerResultEvent::getEvent(void)
{
	return m_nEvent;
}

int HSatipServerResultEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	int RETURN_PARAM_COUNT = 0;
	jsplugin_value *result = NULL;
	switch(m_nEvent)
	{
		case eSATIP_STOP:
		case eSATIP_START:
			{
				RETURN_PARAM_COUNT = 1;
				*argv = new jsplugin_value[RETURN_PARAM_COUNT];
				result = *argv;

				result[0].type = JSP_TYPE_NUMBER;
				result[0].u.number = (m_nEvent == eSATIP_START)? 0:1;
			}
			break;


		default :
			break;
	}

	return RETURN_PARAM_COUNT;
}

#endif // OIPF

