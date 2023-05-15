/*
 * BmlVodEvent.cpp
 *
 */

#ifdef OIPF

#include "BmlVodEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

BmlVodEvent::~BmlVodEvent()
{
	ENTRY;
	HLIB_STD_MemFree(appName);
	HLIB_STD_MemFree(param);
}

BmlVodEvent::BmlVodEvent(jsplugin_obj *context, const char *_appName, const char *_param)
  : Event(context, "BmlVod"), appName(HLIB_STD_StrDup(_appName)), param(HLIB_STD_StrDup(_param))
{
	ENTRY;
	OOIF_LOG_PRINT("%s %d appName:%s, param:%s\n", __FUNCTION__, __LINE__, _appName, _param);
}

int BmlVodEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_STRING("appName", appName, JSP_GET_VALUE_CACHE);
	GET_STRING("param", param, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int BmlVodEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int BmlVodEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = appName;
	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = param;

	return 2;
}
#endif // OIPF
