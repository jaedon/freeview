/**
 * Plugin: Humax Upgrade
*/

/*
   Opera JavaScript plugin use case: Humax Sysinfo OBJECT

   This plugin can be instantiated by eg
   <OBJECT TYPE="valups/upgrade" ID="upgradeObject"></OBJECT>
   (only TYPE is mandatory, but ID may be pretty useful)
   
   upgrade(url, version)
   forcedupgrade(url, version)
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "browser_ota.h"
#include "libplugin.h"

static jsplugin_cap cap;

static const char* global_names[] = { NULL }; /* no global names */
static const char* object_types[] = { "valups/upgrade", NULL }; /* only one MIME type */

/* Someone may want to compile and link under W*ndows */
# ifdef WIN32
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

static struct jsplugin_callbacks* opera_callbacks;

static int upgrade( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	browser_error err = ERR_BROWSER_OK;

	if (argc != 2)
	{
		return JSP_CALL_ERROR;
	}

	if (this_obj == NULL)
	{
		/* this can happen under certain circumstances (wrong ecmascript code) */
		return JSP_CALL_ERROR;
	}

	PLUGIN_Print("upgrade: param1=%s, param2=%s\n", argv[0].u.string, argv[1].u.string);

	err = browser_ota_upgrade(argv[0].u.string, argv[1].u.string);
	if ( err != ERR_BROWSER_OK )
	{
		return JSP_CALL_ERROR;
	}

	return JSP_CALL_NO_VALUE;
}

static int forced_upgrade( jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result )
{
	browser_error err = ERR_BROWSER_OK;

	if (argc != 1)
	{
		return JSP_CALL_ERROR;
	}

	if (this_obj == NULL)
	{
		/* this can happen under certain circumstances (wrong ecmascript code) */
		return JSP_CALL_ERROR;
	}

	PLUGIN_Print("forcedupgrade: param1=%s, param2=%s\n", argv[0].u.string, argv[1].u.string);

	err = browser_ota_upgrade(argv[0].u.string, argv[1].u.string);
	if ( err != ERR_BROWSER_OK )
	{
		return JSP_CALL_ERROR;
	}

	return JSP_CALL_NO_VALUE;
}


/* getter for the properties that the OBJECT has */
static int upgrade_getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	PLUGIN_Print("upgrade_getter: %s\n", name);

	if (strcmp(name, "upgrade") == 0)
	{
		jsplugin_obj *upgradeFunctionObj;
		PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
		int r = opera_callbacks->create_function(obj,
				NULL,
				NULL,
				upgrade,
				NULL,
				"",
				NULL,
				NULL,
				&upgradeFunctionObj);

		if (r < 0)
		{
		PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
			return JSP_GET_ERROR;
		}
		else
		{
		PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
			result->type = JSP_TYPE_OBJECT;
			result->u.object = upgradeFunctionObj;
			return JSP_GET_VALUE_CACHE;
		}

	}
	else if (strcmp(name, "forcedupgrade") == 0)
	{
		jsplugin_obj *upgradeFunctionObj;
		int r = opera_callbacks->create_function(obj,
				NULL,
				NULL,
				forced_upgrade,
				NULL,
				"",
				NULL,
				NULL,
				&upgradeFunctionObj);

		if (r < 0)
		{
			return JSP_GET_ERROR;
		}
		else
		{
			result->type = JSP_TYPE_OBJECT;
			result->u.object = upgradeFunctionObj;
			return JSP_GET_VALUE_CACHE;
		}
	}
	
	return JSP_GET_NOTFOUND;
}

/* destructor */
static int upgrade_destructor(jsplugin_obj *obj)
{
	PLUGIN_Print("upgrade_destructor\n");

	return JSP_DESTROY_OK;
}



/* called back when an OBJECT that the plugin claims to support is
   created. note that the plugin also needs to have the appropriate
   access policy set up, or it won't be taken into account at all. */
int upgrade_handle_object(jsplugin_obj *global_obj, jsplugin_obj *object_obj, int attrs_count, jsplugin_attr *attrs,
				  jsplugin_getter **getter, jsplugin_setter **setter, jsplugin_destructor **destructor,
				  jsplugin_notify **inserted, jsplugin_notify **removed)
{
	PLUGIN_Print("libplugin_upgrade: handle_object.\n");

	/* set callbacks */
	*getter = &upgrade_getter;
	*setter = NULL;
	*inserted = NULL;
	*destructor = NULL;
	*removed = NULL;

	return JSP_OBJECT_OK;
}

static void upgrade_init(jsplugin_obj *global_context)
{
	PLUGIN_Print("libplugin_upgrade: init.\n");
}

static void upgrade_destroy(jsplugin_obj *global_context)
{
	PLUGIN_Print("libplugin_upgrade: destroy.\n");
}


static int upgrade_allow_access(const char *protocol, const char *hostname, int port, const char *url)
{
	PLUGIN_Print("allow_access: proto=%s, name=%s, port=%d\n", protocol, hostname, port);

	return 1;
}

extern "C" {

/* the global plugin hook. */
DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c)
{
	PLUGIN_FPrint("====== upgrade plugin loaded ======\n");
	cap.global_names = global_names;
	cap.object_types = object_types;

	cap.global_getter = NULL;
	cap.global_setter = NULL;

	cap.init = upgrade_init;
	cap.destroy = upgrade_destroy;
	cap.gc_trace = NULL;

	cap.handle_object = upgrade_handle_object;
	cap.allow_access = upgrade_allow_access;
	cap.default_permission = NULL;
	
	*result = &cap;

	opera_callbacks = c;

	return 0;
}

}

