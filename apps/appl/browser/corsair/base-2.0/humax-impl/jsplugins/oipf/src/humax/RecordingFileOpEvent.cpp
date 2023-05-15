/*
 * RecordingFileOpEvent.cpp
 */

#ifdef HMX_WEBUI

#include <string.h>

#include "macros.h"
#include "RecordingFileOpEvent.h"

namespace Ooif {

RecordingFileOpEvent::RecordingFileOpEvent(int _ticket, int _state, jsplugin_obj *c) :
		Event(c, "FileOpEvent"), ticket(_ticket), state(_state)
{
	ENTRY;
}

RecordingFileOpEvent::~RecordingFileOpEvent()
{
	ENTRY;
}

int RecordingFileOpEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER("ticket", ticket, JSP_GET_VALUE_CACHE);
	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int RecordingFileOpEvent::serialize(jsplugin_value** argv)
{
	ENTRY;
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = ticket;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = state;

	return 2;
}

} /* namespace Ooif */

#endif /* HMX_WEBUI */

/* EOF */

