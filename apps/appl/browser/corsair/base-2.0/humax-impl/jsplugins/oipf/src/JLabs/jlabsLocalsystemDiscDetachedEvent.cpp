/*
 * jlabsLocalsystemDiscDetachedEvent.cpp
 *
 *  Created on: 2012. 07. 27.
 *      Author: jykim2
 */

#ifdef OIPF

#include "jlabsLocalsystemDiscDetachedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

JlabsDiscDetachedEvent::~JlabsDiscDetachedEvent()
{
	ENTRY;
	HLIB_STD_MemFree(discId);
	HLIB_STD_MemFree(discName);
}

JlabsDiscDetachedEvent::JlabsDiscDetachedEvent(jsplugin_obj *context, const char *_discId, const char *_discName)
  : Event(context, "DiscDetached"), discId(HLIB_STD_StrDup(_discId)), discName(HLIB_STD_StrDup(_discName))
{
	ENTRY;
}

int JlabsDiscDetachedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_STRING("discId", discId, JSP_GET_VALUE_CACHE);
	GET_STRING("discName", discName, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int JlabsDiscDetachedEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int JlabsDiscDetachedEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_STRING;
	result[0].u.string = discId;
	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = discName;

	return 2;
}
#endif // OIPF
