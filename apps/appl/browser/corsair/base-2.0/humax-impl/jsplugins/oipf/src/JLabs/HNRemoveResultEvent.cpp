
#ifdef JLABS

#include "HNRemoveResultEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Recording.h"

using namespace Ooif;

HNRemoveResult::~HNRemoveResult()
{
	ENTRY;
}

HNRemoveResult::HNRemoveResult(const char *id, int errorCode, jsplugin_obj *c)
  : Event(c, "HNRemoveResult"), id(id), errorCode(errorCode)
{
	if(id != NULL)
	{
		// to guarantee a 'release' callback to integration without the
		// getter being called on the channel in case of a non-intrinsic
		// event, we need to create a system id here
		id = HLIB_STD_StrDup(id);
	}
}

int HNRemoveResult::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	if(id)
	{
		GET_STRING("remove", id, JSP_GET_VALUE);
	}
	GET_NUMBER("result", errorCode, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HNRemoveResult::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HNRemoveResult::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	if(id)
	{
		result[0].type = JSP_TYPE_STRING;
		result[0].u.string = id;
	}
	else
	{
		result[0].type = JSP_TYPE_NULL;
	}

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = errorCode;
	return 2;
}
#endif // JLABS
