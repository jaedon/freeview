/*
 * jlabsLocalsystemResidentAppFinishEvent.cpp
 *
 *  Created on: 2012. 10. 17.
 *      Author: choism
 */

#ifdef OIPF

#include "jlabsLocalsystemResidentAppFinishEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

JlabsResidentAppFinishEvent::~JlabsResidentAppFinishEvent()
{
	HLIB_STD_MemFree(appName);
}

JlabsResidentAppFinishEvent::JlabsResidentAppFinishEvent(jsplugin_obj *context, char *appName)
  : Event(context, "ResidentAppFinish")
{
	ENTRY;
	this->appName = HLIB_STD_StrDup(appName);
}

int JlabsResidentAppFinishEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_STRING("appName", this->appName, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int JlabsResidentAppFinishEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int JlabsResidentAppFinishEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = this->appName;

	return 1;
}
#endif // OIPF

