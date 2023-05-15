#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>
#include "jsplugin.h"

#include "opllocalsystem.h"
#include "libplugin.h"

static jsplugin_cap cap;

static const char* global_names[] = { NULL }; /* no global names */
static const char* object_types[] = { "humax/console", NULL }; /* only one MIME type */

/* Someone may want to compile and link under W*ndows */
# ifdef WIN32
#  define DECLSPEC __declspec(dllexport)
# else
#  define DECLSPEC
# endif

static const char* ANSI_RESET = "\033[22;39m";
static const char* ANSI_BLACK = "\033[22;30m";
static const char* ANSI_RED = "\033[22;31m";
static const char* ANSI_GREEN = "\033[22;32m";
static const char* ANSI_YELLOW = "\033[22;33m";
static const char* ANSI_BLUE = "\033[22;34m";
static const char* ANSI_PURPLE = "\033[22;35m";
static const char* ANSI_CYAN = "\033[22;36m";
static const char* ANSI_WHITE = "\033[22;37m";

static const char* ANSI_DARK = "\033[01;30m";
static const char* ANSI_LIGHT_RED = "\033[01;31m";
static const char* ANSI_LIGHT_GREEN = "\033[01;32m";
static const char* ANSI_LIGHT_YELLOW = "\033[01;33m";
static const char* ANSI_LIGHT_BLUE = "\033[01;34m";
static const char* ANSI_LIGHT_PURPLE = "\033[01;35m";
static const char* ANSI_LIGHT_CYAN = "\033[01;36m";
static const char* ANSI_LIGHT_WHITE = "\033[01;37m";

static const char* STR_RESET = "reset";
static const char* STR_BLACK = "black";
static const char* STR_RED = "red";
static const char* STR_GREEN = "green";
static const char* STR_YELLOW = "yellow";
static const char* STR_BLUE = "blue";
static const char* STR_PURPLE = "purple";
static const char* STR_CYAN = "cyan";
static const char* STR_WHITE = "white";

static const char* STR_DARK = "dark";
static const char* STR_LIGHT_RED = "lightred";
static const char* STR_LIGHT_GREEN = "lightgreen";
static const char* STR_LIGHT_YELLOW = "lightyellow";
static const char* STR_LIGHT_BLUE = "lightblue";
static const char* STR_LIGHT_PURPLE = "lightpurple";
static const char* STR_LIGHT_CYAN = "lightcyan";
static const char* STR_LIGHT_WHITE = "lightwhite";

typedef struct {
	const char* iColor;
} TConsole;


static struct jsplugin_callbacks* opera_callbacks;
static int console_log(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result) {
	if (argc != 1) {
		return JSP_CALL_ERROR;
	}

	if (this_obj == NULL) {
		/* this can happen under certain circumstances (wrong ecmascript code) */
		return JSP_CALL_ERROR;
	}

	TConsole* console = (TConsole*)this_obj->plugin_private;
	PLUGIN_Print("%s[WEBAPP] %s%s\n", console->iColor, argv[0].u.string, ANSI_RESET);
	return JSP_CALL_NO_VALUE;
}
static int console_setColor(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result) {
	if (argc != 1) {
		return JSP_CALL_ERROR;
	}

	if (this_obj == NULL) {
		/* this can happen under certain circumstances (wrong ecmascript code) */
		return JSP_CALL_ERROR;
	}

	TConsole* console = (TConsole*)this_obj->plugin_private;
	const char* paramColor = argv[0].u.string;
	if (strcmp(paramColor, STR_BLACK) == 0) {
		console->iColor = ANSI_BLACK;
	}
	else if (strcmp(paramColor, STR_RED) == 0) {
		console->iColor = ANSI_RED;
	}
	else if (strcmp(paramColor, STR_GREEN) == 0) {
		console->iColor = ANSI_GREEN;
	}
	else if (strcmp(paramColor, STR_YELLOW) == 0) {
		console->iColor = ANSI_YELLOW;
	}
	else if (strcmp(paramColor, STR_BLUE) == 0) {
		console->iColor = ANSI_BLUE;
	}
	else if (strcmp(paramColor, STR_PURPLE) == 0) {
		console->iColor = ANSI_PURPLE;
	}
	else if (strcmp(paramColor, STR_CYAN) == 0) {
		console->iColor = ANSI_CYAN;
	}
	else if (strcmp(paramColor, STR_WHITE) == 0) {
		console->iColor = ANSI_WHITE;
	}
	else if (strcmp(paramColor, STR_DARK) == 0) {
		console->iColor = ANSI_DARK;
	}
	else if (strcmp(paramColor, STR_LIGHT_RED) == 0) {
		console->iColor = ANSI_LIGHT_RED;
	}
	else if (strcmp(paramColor, STR_LIGHT_GREEN) == 0) {
		console->iColor = ANSI_LIGHT_GREEN;
	}
	else if (strcmp(paramColor, STR_LIGHT_YELLOW) == 0) {
		console->iColor = ANSI_LIGHT_YELLOW;
	}
	else if (strcmp(paramColor, STR_LIGHT_BLUE) == 0) {
		console->iColor = ANSI_LIGHT_BLUE;
	}
	else if (strcmp(paramColor, STR_LIGHT_PURPLE) == 0) {
		console->iColor = ANSI_LIGHT_PURPLE;
	}
	else if (strcmp(paramColor, STR_LIGHT_CYAN) == 0) {
		console->iColor = ANSI_LIGHT_CYAN;
	}
	else if (strcmp(paramColor, STR_LIGHT_WHITE) == 0) {
		console->iColor = ANSI_LIGHT_WHITE;
	}
	return JSP_CALL_NO_VALUE;
}
/* getter for the properties that the OBJECT has */
static int console_getter(jsplugin_obj *obj, const char *name,
		jsplugin_value *result) {
	PLUGIN_Print("console_getter: %s -> ", name);
	if (strcmp(name, "log") == 0) {
		jsplugin_obj *logFunctionObj;
		int r = opera_callbacks->create_function(obj, NULL, NULL, console_log,
				NULL, "", NULL, NULL, &logFunctionObj);
		PLUGIN_Print("%d", r);
		if (r < 0) {
			PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
			return JSP_GET_ERROR;
		} else {
			PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
			result->type = JSP_TYPE_OBJECT;
			result->u.object = logFunctionObj;
			return JSP_GET_VALUE_CACHE;
		}
	}
	else if (strcmp(name, "setColor") == 0) {
		jsplugin_obj *logFunctionObj;
		int r = opera_callbacks->create_function(obj, NULL, NULL, console_setColor,
				NULL, "", NULL, NULL, &logFunctionObj);
		PLUGIN_Print("%d", r);
		if (r < 0) {
			PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
			return JSP_GET_ERROR;
		} else {
			PLUGIN_Print("##JW::%s:%d\n", __FUNCTION__, __LINE__);
			result->type = JSP_TYPE_OBJECT;
			result->u.object = logFunctionObj;
			return JSP_GET_VALUE_CACHE;
		}
	}
	PLUGIN_Print("\n");
	return JSP_GET_NOTFOUND;
}

/* destructor */
static int console_destructor(jsplugin_obj *obj) {
	PLUGIN_Print("console_destructor\n");
	TConsole* console = (TConsole*)obj->plugin_private;
	HLIB_STD_MemFree(console);
	return JSP_DESTROY_OK;
}

/* called back when an OBJECT that the plugin claims to support is
 created. note that the plugin also needs to have the appropriate
 access policy set up, or it won't be taken into account at all. */
static int console_handle_object(jsplugin_obj *global_context,
		jsplugin_obj *object_obj, int attrs_count, jsplugin_attr *attrs,
		jsplugin_getter **getter, jsplugin_setter **setter,
		jsplugin_destructor **destructor, jsplugin_gc_trace **trace,
		jsplugin_notify **inserted, jsplugin_notify **removed) {
	TConsole *priv_data = NULL;
	priv_data = (TConsole *)HLIB_STD_MemAlloc(sizeof(TConsole));
	if ( priv_data == NULL )
	{
		PLUGIN_Print("libplugin_sysinfo: priv_data is NULL.\n");
		return JSP_OBJECT_NOMEM;
	}
	memset(priv_data, 0, sizeof(TConsole));
	priv_data->iColor = ANSI_LIGHT_YELLOW;
	object_obj->plugin_private = (void *)priv_data;
	*getter = &console_getter;
	*setter = NULL;
	*inserted = NULL;
	*destructor = &console_destructor;
	*removed = NULL;
	return JSP_OBJECT_OK;
}

static void console_init(jsplugin_obj *global_context) {
	PLUGIN_Print("libplugin_console: init.\n");
}

static void console_destroy(jsplugin_obj *global_context) {
	PLUGIN_Print("libplugin_console: destroy.\n");
}

static int console_allow_access(const char *protocol, const char *hostname,
		int port, const char *url) {
	PLUGIN_Print("libplugin_console: allow_access: proto=%s, name=%s, port=%d\n",
			protocol, hostname, port);
	return 1;
}

/* the global plugin hook. */
DECLSPEC int jsplugin_capabilities(jsplugin_cap **result, jsplugin_callbacks *c) {
	PLUGIN_FPrint("====== console plugin loaded ======\n");
	cap.global_names = global_names;
	cap.object_types = object_types;

	cap.global_getter = NULL;
	cap.global_setter = NULL;

	cap.init = console_init;
	cap.destroy = console_destroy;
	cap.gc_trace = NULL;

	cap.handle_object = console_handle_object;
	cap.allow_access = console_allow_access;
	cap.default_permission = NULL;

	*result = &cap;

	opera_callbacks = c;

	return 0;
}

/* end of file */
