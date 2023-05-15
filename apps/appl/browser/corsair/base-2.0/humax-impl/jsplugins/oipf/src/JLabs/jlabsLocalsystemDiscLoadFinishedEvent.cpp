/*
 * jlabsLocalsystemDiscLoadFinishedEvent.cpp
 *
 *  Created on: 2012. 07. 27.
 *      Author: jykim2
 */

#ifdef OIPF

#include "jlabsLocalsystemDiscLoadFinishedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

JlabsDiscLoadFinishedEvent::~JlabsDiscLoadFinishedEvent()
{
	delete disc;
}

JlabsDiscLoadFinishedEvent::JlabsDiscLoadFinishedEvent(jsplugin_obj *context, HDiscInfoHandle handle)
  : Event(context, "DiscLoadFinished")
{
	ENTRY;
	if(handle != 0)		disc = new HDiscInfo(handle);
}

int JlabsDiscLoadFinishedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	if (strcmp(name, "disc") == 0) {
		if (disc) {
			RETURN_OBJECT(obj, new HDiscInfo(disc->getIdentifier()), result, JSP_GET_VALUE_CACHE);
		} else {
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}
	return JSP_GET_NOTFOUND;
}

int JlabsDiscLoadFinishedEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int JlabsDiscLoadFinishedEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new HDiscInfo(disc->getIdentifier()), 0);

	return 1;
}
#endif // OIPF