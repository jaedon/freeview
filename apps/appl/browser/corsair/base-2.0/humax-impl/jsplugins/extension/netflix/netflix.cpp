/*************************************************************************************************************
	File 		: systeminfo.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/11/07
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

#include "netflix.h"
#include "plugtype.h"
#include "macro.h"
#include <hlib.h>
#include "libplugin.h"
#include "oplnetflix.h"

#include <cstdio>
#include <cstring>
#include <iostream>

using namespace Netflix;
using namespace std;


/******************************************************************************************
 * Netflix
 ******************************************************************************************/
std::vector<NetflixEventData *>	NetflixObject::s_events;
pthread_mutex_t NetflixObject::s_mtx;
jsplugin_obj *NetflixObject::s_globalContext = NULL;
bool NetflixObject::runPollInterval = false;

int NetflixObject::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(startWithType, obj, "i", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startWithTypeUrl, obj, "is", JSP_GET_VALUE_CACHE);
    GET_FUNCTION(startWith6params, obj, "isiiii", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopWithReason, obj, "i", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(notifyOverlayState, obj, "i", JSP_GET_VALUE_CACHE);

	//INTRINSIC_EVENT_GET(NetflixStatusChanged);

	if ( strcmp( name, "registerStatusChanged" ) == 0 )
	{
		int r;

		jsplugin_obj *thefunction;
		r = g_opera_callbacks->create_function( obj, NULL, NULL, registerStatusChanged, NULL, "o", NULL, NULL, &thefunction );
		if (r != JSP_CREATE_OK)
		{
			return JSP_GET_ERROR;
		}
		else
		{
			result->type = JSP_TYPE_OBJECT;
			result->u.object = thefunction;
			return JSP_GET_VALUE_CACHE;
		}
		//return create_method(ctx, NetflixStatusChangedCallback, "o", result);
	}
	GET_FAIL;
}

int NetflixObject::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	if ( strcmp( name, "statusChanged" ) == 0 )
	{
		int r;

		jsplugin_obj *thefunction;
		r = g_opera_callbacks->create_function( obj, NULL, NULL, registerStatusChanged, NULL, "o", NULL, NULL, &thefunction );
		if (r != JSP_CREATE_OK)
		{
			return JSP_GET_ERROR;
		}
		else
		{
			//result->type = JSP_TYPE_OBJECT;
			//result->u.object = thefunction;
			return JSP_GET_VALUE_CACHE;
		}
		//return create_method(ctx, NetflixStatusChangedCallback, "o", result);
	}

	return JSP_GET_NOTFOUND;
}

int NetflixObject::constructor( jsplugin_obj *this_obj, jsplugin_obj *function_obj,
			int argc, jsplugin_value *argv, jsplugin_value *result )
{
	pthread_mutex_init(&s_mtx, NULL);

    NetflixObject *netflixobj = new NetflixObject();

    if ( netflixobj )
    {    
        GLOBAL_RETURN_OBJECT(function_obj, netflixobj, getter, destructor, result, JSP_CALL_VALUE);
    }

	return JSP_CALL_ERROR;
}

int NetflixObject::destructor(jsplugin_obj *obj)
{
	NetflixObject *netflix_obj = static_cast<NetflixObject *>(obj->plugin_private);
	if ( netflix_obj ) delete netflix_obj;

	obj->plugin_private = NULL;

	pthread_mutex_destroy(&s_mtx);

	return JSP_DESTROY_OK;
}


int NetflixObject::startWithType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
					jsplugin_value *argv, jsplugin_value *result)
{
	OplNetflix::startWithType((int)NUMBER_VALUE(argv[0]));
	//OplNetflix::callStatusChangedCallback(1000);
	return PLUG_RETURN_OK;
}
int NetflixObject::startWithTypeUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
					jsplugin_value *argv, jsplugin_value *result)
{
	OplNetflix::startWithTypeUrl((int)NUMBER_VALUE(argv[0]),(unsigned char *)STRING_VALUE(argv[1]));
	//OplNetflix::callStatusChangedCallback(1000);
	return PLUG_RETURN_OK;
}
int NetflixObject::startWith6params(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
					jsplugin_value *argv, jsplugin_value *result)
{
	OplNetflix::startWith6params((int)NUMBER_VALUE(argv[0]),(unsigned char *)STRING_VALUE(argv[1]),(int)NUMBER_VALUE(argv[2]),
        (int)NUMBER_VALUE(argv[3]),(int)NUMBER_VALUE(argv[4]),(int)NUMBER_VALUE(argv[5]));
	//OplNetflix::callStatusChangedCallback(1000);
	return PLUG_RETURN_OK;
}
int NetflixObject::stopWithReason(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
					jsplugin_value *argv, jsplugin_value *result)
{
	OplNetflix::stopWithReason((int)NUMBER_VALUE(argv[0]));
	return PLUG_RETURN_OK;
}
int NetflixObject::notifyOverlayState(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
					jsplugin_value *argv, jsplugin_value *result)
{
	OplNetflix::notifyOverlayState((int)NUMBER_VALUE(argv[0]));
	return PLUG_RETURN_OK;
}

int NetflixObject::statusChangedPollingCallback(const jsplugin_obj* obj)
{
	pthread_mutex_lock(&s_mtx);

	for ( vector<NetflixEventData *>::iterator it = s_events.begin(); it != s_events.end(); ++it )
	{
		NetflixEventData *data = *it;
		if ( data->status != -1 )
		{

			jsplugin_value value[1];

			value[0].type = JSP_TYPE_NUMBER;
			value[0].u.number = data->status;

			if(data->funcObj)
			{
				g_opera_callbacks->call_function(s_globalContext, NULL,
											data->funcObj, 1, value, NULL, NULL);
			}
			data->status = -1;

	//		delete data;
	//		it = s_events.erase(it);
		}
	}

//	s_events.clear();

	pthread_mutex_unlock(&s_mtx);
return 1;

}

void NetflixObject::statusChangedCallback(int status)
{
	pthread_mutex_lock(&s_mtx);

	for ( vector<NetflixEventData *>::iterator it = s_events.begin(); it != s_events.end(); ++it )
		(*it)->status = status;

	pthread_mutex_unlock(&s_mtx);
}

int NetflixObject::registerStatusChanged( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{

	//CHECK_ARGUMENT_COUNT(1);
//	CHECK_OBJECT(this_obj);


	if ( argv[0].type != JSP_TYPE_NATIVE_OBJECT ) return JSP_CALL_ERROR;

	NetflixEventData *data = new NetflixEventData();
	if ( !data ) return JSP_CALL_ERROR;

	data->funcObj = argv[0].u.object;
	s_events.push_back(data);

	if ( !runPollInterval )
	{
		int pollResult = g_opera_callbacks->set_poll_interval(s_globalContext, 500, statusChangedPollingCallback);
		runPollInterval = true;
	}

	result->type = JSP_TYPE_NUMBER;
	result->u.number = 0;

	OplNetflix::registerStatusChangedCallback(statusChangedCallback);

	return JSP_CALL_VALUE;
}

namespace Netflix
{

	static jsplugin_cap cap;

	static const char* global_names[] = { "Netflix", NULL };

	static const char* object_types[] = { NULL }; /* only one MIME type */

	jsplugin_callbacks* g_opera_callbacks;

	/******************************************************************************************
	 * jsplugin
	 ******************************************************************************************/
	void netflix_init(jsplugin_obj *global_context)
	{
		if(NetflixObject::s_globalContext==NULL){
			NetflixObject::s_globalContext = global_context;
		}
	}

	void netflix_destroy(jsplugin_obj *global_context)
	{
		if(global_context == NetflixObject::s_globalContext)
		{
			vector<NetflixEventData *>::iterator it;
			for ( it = NetflixObject::s_events.begin(); it != NetflixObject::s_events.end(); ++it )
			{
				delete *it;
			}
			NetflixObject::s_events.clear();
			NetflixObject::runPollInterval = false;
			NetflixObject::s_globalContext = NULL;
		}
	}
	static int netflix_allow_access(const char *protocol, const char *hostname, int port, const char *url)
	{
		return 1;
	}

	static int netflix_global_getter(jsplugin_obj *ctx, const char *name, jsplugin_value *result)
	{
		ENTRY;
		HERROR err = ERR_OK;
		GLOBAL_GET_OBJECT("Netflix", ctx, NetflixObject::constructor,
				result, JSP_GET_VALUE_CACHE);
	    return JSP_GET_ERROR;
	}

	static void netflix_gc_trace(jsplugin_obj *this_obj )
	{
		vector<NetflixEventData *>::iterator it;
		for ( it = NetflixObject::s_events.begin(); it != NetflixObject::s_events.end(); ++it )
		{
			g_opera_callbacks->gcmark((*it)->funcObj);
		}
	}
}	/* namespace */

//using namespace Netflix;

extern "C" {

int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== Netflix plugin loaded ======\n");
	cap.global_names = global_names;
	cap.object_types = object_types;

	//cap.handle_object = netflix_handle_object;
	cap.allow_access = netflix_allow_access;
	cap.global_getter = netflix_global_getter;
	cap.global_setter = NULL;
	cap.default_permission = NULL;
	cap.gc_trace = netflix_gc_trace;
	cap.init = netflix_init;
	cap.destroy = netflix_destroy;

	*result = &cap;

	g_opera_callbacks = c;

	return 0;
}

}	/* extern C */

