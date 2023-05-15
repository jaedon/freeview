
#ifdef JLABS

#include "HNReservationFinishedEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "Recording.h"

using namespace Ooif;

HNReservationFinished::~HNReservationFinished()
{
	ENTRY;
}

HNReservationFinished::HNReservationFinished(ScheduledRecordingHandle recordingHandle, int errorCode, jsplugin_obj *c) 
  : Event(c, "HNReservationFinished"), recording(recordingHandle), errorCode(errorCode)
{
}

int HNReservationFinished::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_NUMBER("errorCode", errorCode, JSP_GET_VALUE_CACHE);
	GET_OBJECT("recording", obj, new ScheduledRecording(recording), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HNReservationFinished::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int HNReservationFinished::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;

	SET_SERIALIZED_OBJECT(new ScheduledRecording(recording), 0);

	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = errorCode;
	return 2;
}
#endif // JLABS
