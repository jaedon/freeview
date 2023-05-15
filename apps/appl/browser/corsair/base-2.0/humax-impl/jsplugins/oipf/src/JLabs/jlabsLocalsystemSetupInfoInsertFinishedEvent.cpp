/*
 * jlabsLocalsystemSetupInfoInsertFinishedEvent.cpp
 *
 *  Created on: 2012. 01. 24.
 *      Author: shlee3
 */

#ifdef OIPF

#include "jlabsLocalsystemSetupInfoInsertFinishedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

jlabsSetupInfoInsertFinishedEvent::~jlabsSetupInfoInsertFinishedEvent()
{
	ENTRY;
}

jlabsSetupInfoInsertFinishedEvent::jlabsSetupInfoInsertFinishedEvent(jsplugin_obj *context, OOIFNumber error)
  : Event(context, "SetupInfoInsertFinished"), error_Number(error)
{
	ENTRY;
}

int jlabsSetupInfoInsertFinishedEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("errorNumber", error_Number, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

int jlabsSetupInfoInsertFinishedEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int jlabsSetupInfoInsertFinishedEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = error_Number;
	return 1;
}
#endif // OIPF

