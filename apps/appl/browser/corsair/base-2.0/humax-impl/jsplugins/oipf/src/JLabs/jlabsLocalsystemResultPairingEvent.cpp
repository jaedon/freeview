/*
 * jlabsLocalsystemResultPairingEvent.cpp
 *
 *  Created on: 2012. 10. 17.
 *      Author: choism
 */

#ifdef OIPF

#include "jlabsLocalsystemResultPairingEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

using namespace Ooif;

JlabsResultPairingEvent::~JlabsResultPairingEvent()
{
	;
}

JlabsResultPairingEvent::JlabsResultPairingEvent(jsplugin_obj *context, OOIFNumber result)
  : Event(context, "ResultPairing")
{
	ENTRY;
	this->result = result;
}

int JlabsResultPairingEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	GET_NUMBER("result", this->result, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int JlabsResultPairingEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int JlabsResultPairingEvent::serialize(jsplugin_value **argv)
{
	ENTRY;
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = this->result;

	return 1;
}
#endif // OIPF

