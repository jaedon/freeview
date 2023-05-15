/**
 * Plugin: WebApp Upgrade
*/

/*
   Opera JavaScript plugin use case: Humax WebApp Upgrade OBJECT

   This plugin can be instantiated by eg
   <OBJECT TYPE="application/sotup" ID="upgradeObject"></OBJECT>
   (only TYPE is mandatory, but ID may be pretty useful)
   
   checkNewWebApplication()
   stopCheckNewWebApplication()
   addUpgradeResultListener(event e, object o)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>

#include "jsplugin.h"
#include "libplugin.h"

#include "oplwebappupgrade.h"
#include <hlib.h>
#include <hapi.h>


#define UPDATE_RESULT_EVENT "UpdateResult"

typedef struct _webapp_upgrade_t {
	jsplugin_obj *listener;
	OplWebAppUpgrade *opl_webapp_upgrade;
} webapp_upgrade_t;


static jsplugin_cap cap;

static const char* global_names[] = { NULL }; /** no global names */
static const char* object_types[] = { "application/sotup", NULL }; /** only one MIME type */

static struct jsplugin_callbacks* opera_callbacks;

std::queue<jsplugin_value*> event_queue;
pthread_mutex_t mutex;


/** MACRO */
int create_method( jsplugin_obj* obj, jsplugin_function *method, const char *sign, jsplugin_value *result )
{
	int r;

	jsplugin_obj *funcObj;
	r = opera_callbacks->create_function( obj, NULL, NULL, method, NULL, sign, NULL, NULL, &funcObj );

	if ( r < 0 )
	{
		return JSP_GET_ERROR;
	}
	else
	{
		result->type = JSP_TYPE_OBJECT;
		result->u.object = funcObj;
		return JSP_GET_VALUE;
	}
}

/** upgrade result event callback registered to OPL */
void onUpgradeResult(int event, int value)
{
	jsplugin_value *upgradeResult = NULL;
	upgradeResult = (jsplugin_value *)malloc(2 * sizeof(jsplugin_value));
	if ( upgradeResult == NULL )
	{
		PLUGIN_Print("memory alloc for event failed.\n");
		return;
	}

	upgradeResult[0].type = JSP_TYPE_NUMBER;
	upgradeResult[0].u.number = event;
	upgradeResult[1].type = JSP_TYPE_NUMBER;
	upgradeResult[1].u.number = value;

	pthread_mutex_lock(&mutex);

	event_queue.push(upgradeResult);

	pthread_mutex_unlock(&mutex);
}

/** dispatch all upgrade result events. this will be called every XX milliseconds (second parameter of set_poll_interval()) from Browser */
int dispatchAllEvents(const jsplugin_obj *obj)
{
	webapp_upgrade_t *webapp_upgrade_info = (webapp_upgrade_t *)obj->plugin_private;
	if ( webapp_upgrade_info->listener == NULL )
	{
		return 1;
	}

	pthread_mutex_lock(&mutex);

	while ( !event_queue.empty() )
	{
		jsplugin_value *upgradeResult = event_queue.front();
		event_queue.pop();
		opera_callbacks->call_function(obj, NULL, webapp_upgrade_info->listener, 2, upgradeResult, NULL, NULL);

		free(upgradeResult);
	}

	pthread_mutex_unlock(&mutex);

	return 1;
}

/******************************************************************
	Interfaces
******************************************************************/
int webapp_upgrade_check_new_webapp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	webapp_upgrade_t *webapp_upgrade_info = NULL;
	HAPI_BroadcastSignal("signal:sotupStart", 0, "i", 0);
	/** no arguments */
	if (argc > 0)
	{
		PLUGIN_Print("parameter mismatch, ignoring.\n");
		return JSP_CALL_EXCEPTION;
	}

	/** this can happen under certain circumstances (wrong ecmascript code) */
	if (this_obj == NULL)
	{
		PLUGIN_Print("called from NULL object.\n");
		return JSP_CALL_EXCEPTION;
	}

	webapp_upgrade_info = (webapp_upgrade_t *)this_obj->plugin_private;
	if ( webapp_upgrade_info->opl_webapp_upgrade == NULL )
	{
		PLUGIN_Print("OPL object is NULL.\n");
		return JSP_CALL_ERROR;
	}

	if ( webapp_upgrade_info->opl_webapp_upgrade->checkNewApplication() != BPL_STATUS_OK )
	{
		PLUGIN_Print("check new application failed.\n");
		return JSP_CALL_ERROR;
	}

	return JSP_CALL_NO_VALUE;
}

int webapp_upgrade_stop_check_new_webapp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	webapp_upgrade_t *webapp_upgrade_info = NULL;
	HAPI_BroadcastSignal("signal:sotupStop", 0, "i", 0);
	/** no arguments */
	if (argc > 0)
	{
		PLUGIN_Print("parameter mismatch, ignoring.\n");
		return JSP_CALL_EXCEPTION;
	}

	/** this can happen under certain circumstances (wrong ecmascript code) */
	if (this_obj == NULL)
	{
		PLUGIN_Print("called from NULL object.\n");
		return JSP_CALL_EXCEPTION;
	}

	webapp_upgrade_info = (webapp_upgrade_t *)this_obj->plugin_private;
	if ( webapp_upgrade_info->opl_webapp_upgrade == NULL )
	{
		PLUGIN_Print("OPL object is NULL.\n");
		return JSP_CALL_ERROR;
	}

	if ( webapp_upgrade_info->opl_webapp_upgrade->stopCheckNewApplication() != BPL_STATUS_OK )
	{
		PLUGIN_Print("stop check new application failed.\n");
		return JSP_CALL_ERROR;
	}

	return JSP_CALL_NO_VALUE;
}
HERROR cb_webapp_upgrade_add_upgrade_result_listener(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
/*
	HUINT32 type, value;
	
	type=HxOBJECT_INT(apArgv[0]);
	if(type==0)		//result
	{
		value = HxOBJECT_INT(apArgv[1]);
		OOIF_LOG_ERROR("TestEvent_Progress : result(%d)\n", value);
	}
	else if(type ==1)		//progress
	{
		value = HxOBJECT_INT(apArgv[1]);
		OOIF_LOG_ERROR("TestEvent_Progress : progress(%d)\n", value);
	}
	*/
	HUINT32 event, value;
	
	event=HxOBJECT_INT(apArgv[0]);
	value=HxOBJECT_INT(apArgv[1]);
	
	onUpgradeResult((int)event,(int)value);
	
}

int webapp_upgrade_add_upgrade_result_listener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	webapp_upgrade_t *webapp_upgrade_info = (webapp_upgrade_t *)this_obj->plugin_private;
	/** only one listener */
	if ( webapp_upgrade_info->listener != NULL )
	{
		PLUGIN_Print("listener is already registered.\n");
		return JSP_CALL_NO_VALUE;
	}

	/** add listener */
	if ( argc == 2 && argv[0].type == JSP_TYPE_STRING && argv[1].type == JSP_TYPE_NATIVE_OBJECT ) 
	{
		if ( strcmp(argv[0].u.string, UPDATE_RESULT_EVENT) != 0 )
		{
			PLUGIN_Print("event(%s) is not supported.\n", argv[0].u.string);
			return JSP_CALL_EXCEPTION;
		}

		if ( argv[1].u.object == NULL )
		{
			PLUGIN_Print("passed listener object is NULL.\n");
			return JSP_CALL_EXCEPTION;
		}

		webapp_upgrade_info->listener = argv[1].u.object;

		HAPI_ConnectSignal("signal:sotupResult",NULL,cb_webapp_upgrade_add_upgrade_result_listener);
		
		if ( webapp_upgrade_info->opl_webapp_upgrade->registWebAppUpgradeResultCallback(onUpgradeResult) != BPL_STATUS_OK )
		{
			PLUGIN_Print("regist event callback to OPL failed.\n");
			return JSP_CALL_ERROR;
		}
	}
	else
	{
		PLUGIN_Print("parameter mismatch, ignoring.\n");
	}

	return JSP_CALL_NO_VALUE;
}


/** called when the JS object is being unloaded */
void webapp_upgrade_unload(jsplugin_obj *obj)
{
	webapp_upgrade_t *webapp_upgrade_info = (webapp_upgrade_t *)obj->plugin_private;
	/** delete event listener and OPL object */
	if ( webapp_upgrade_info != NULL )
	{
		if ( webapp_upgrade_info->listener != NULL )
		{
			webapp_upgrade_info->listener = NULL;
		}

		if ( webapp_upgrade_info->opl_webapp_upgrade != NULL )
		{
			delete webapp_upgrade_info->opl_webapp_upgrade;
			webapp_upgrade_info->opl_webapp_upgrade = NULL;
		}
	}
}

/** the getname function to be used for this OBJECT plugin */
int webapp_upgrade_getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	PLUGIN_Print("[WebApp Upgrade] getter - name: %s\n", name);

	if ( strcmp(name, "checkNewWebApplication") == 0 )
	{
		return create_method( obj, webapp_upgrade_check_new_webapp, "", result );
	}
	else if ( strcmp(name, "stopCheckNewWebApplication") == 0 )
	{		
		return create_method( obj, webapp_upgrade_stop_check_new_webapp, "", result );
	}
	else if ( strcmp(name, "addUpgradeResultListener") == 0 )
	{
		return create_method( obj, webapp_upgrade_add_upgrade_result_listener, "o", result );
	}
	
	return JSP_GET_NOTFOUND;
}

/** the destructor */
int webapp_upgrade_destructor(jsplugin_obj *obj)
{
	webapp_upgrade_t *webapp_upgrade_info = (webapp_upgrade_t *)obj->plugin_private;

	/** clear out event queue */
	pthread_mutex_lock(&mutex);

	while ( !event_queue.empty() )
	{
		jsplugin_value *upgradeResult = event_queue.front();
		event_queue.pop();
		free(upgradeResult);
	}

	pthread_mutex_unlock(&mutex);

	pthread_mutex_destroy(&mutex);

	if ( webapp_upgrade_info == NULL )
	{
		return JSP_DESTROY_OK;
	}

	/** delete plugin private data */
	if ( webapp_upgrade_info->listener != NULL )
	{
		webapp_upgrade_info->listener = NULL;
	}

	if ( webapp_upgrade_info->opl_webapp_upgrade != NULL )
	{
		delete webapp_upgrade_info->opl_webapp_upgrade;
	}

	free(webapp_upgrade_info);
	webapp_upgrade_info = NULL;

	return JSP_DESTROY_OK;
}

/** called when there is a new (document) global context, which is roughly when there is a new document that the plugin can be used in */
void webapp_upgrade_init(jsplugin_obj *global_context)
{
	PLUGIN_Print("[WebApp Upgrade] init(%p).\n", global_context);
}

/** called when the (document) global context is garbage collected, which is some time after the document is destroyed */
void webapp_upgrade_destroy(jsplugin_obj *global_context)
{
	PLUGIN_Print("[WebApp Upgrade] destroy(%p).\n", global_context);
}

/** Called when an OBJECT element is created */
int webapp_upgrade_handle_object(jsplugin_obj *global_obj, jsplugin_obj *object_obj, int attrs_count, jsplugin_attr *attrs,
				  jsplugin_getter **getter, jsplugin_setter **setter, jsplugin_destructor **destructor, jsplugin_gc_trace **trace,
				  jsplugin_notify **inserted, jsplugin_notify **removed)
{
	int r;
	webapp_upgrade_t *webapp_upgrade_info = NULL;

	PLUGIN_Print("[WebApp Upgrade] plug-in object created.\n");

	/** create plugin private data */
	webapp_upgrade_info = (webapp_upgrade_t *)malloc(sizeof(webapp_upgrade_t));
	if ( webapp_upgrade_info == NULL )
	{
		return JSP_OBJECT_NOMEM;
	}
	memset(webapp_upgrade_info, 0x00, sizeof(webapp_upgrade_t));

	webapp_upgrade_info->opl_webapp_upgrade = new OplWebAppUpgrade();
	if ( webapp_upgrade_info->opl_webapp_upgrade == NULL )
	{
		return JSP_OBJECT_NOMEM;
	}

	object_obj->plugin_private = webapp_upgrade_info;

	/** set poll interval */
	r = opera_callbacks->set_poll_interval(object_obj, 100, dispatchAllEvents);
	if ( r != JSP_POLL_OK )
	{
		PLUGIN_Print("Failed to set up polling callbacks\n");
		return JSP_OBJECT_ERROR;
	}

	/** add unload listener */
	opera_callbacks->add_unload_listener(object_obj, webapp_upgrade_unload);

	/** mutex init */
	pthread_mutex_init(&mutex, NULL);

	/** set callbacks */
	*getter = &webapp_upgrade_getter;
	*setter = NULL;
	*destructor = &webapp_upgrade_destructor;
	*trace = NULL;
	*inserted = NULL;
	*removed = NULL;

	return JSP_OBJECT_OK;
}

/** Called when new document is created (before init, which will not be called at all if this function returns zero) */
int webapp_upgrade_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
	PLUGIN_Print("[WebApp Upgrade] allow_accss - proto: %s, name: %s, port: %d\n", protocol, hostname, port);

	return 1;
}

extern "C" {

/** the global plugin hook. */
int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== webapp_upgrade plugin loaded ======\n");

	cap.global_names = global_names;
	cap.object_types = object_types;

	cap.global_getter = NULL;
	cap.global_setter = NULL;

	cap.init = webapp_upgrade_init;
	cap.destroy = webapp_upgrade_destroy;

	cap.handle_object = webapp_upgrade_handle_object;

	cap.allow_access = webapp_upgrade_allow_access;
	cap.gc_trace = NULL;
	cap.default_permission = NULL;

	*result = &cap;

	opera_callbacks = c;

	return 0;
}

}
