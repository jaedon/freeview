#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
//#include "opldmp.h"

#include "jlabsRecordDestination.h"

using namespace Ooif;

jlabsRecordDestination::jlabsRecordDestination( jlabsRecordDestinationHandle id ) : WrapperObject<jlabsRecordDestination>(id)
{
	ENTRY;
}

jlabsRecordDestination::jlabsRecordDestination(const jlabsRecordDestination&original ) : WrapperObject<jlabsRecordDestination>(original.identifier)
{
	ENTRY;
}

jlabsRecordDestination::~jlabsRecordDestination()
{
	ENTRY;
}

int jlabsRecordDestination::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	ENTRY;
	NATIVE_GET_STRING("destinationId", native_jlabsRecordDestinationId(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_jlabsRecordDestinationName(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int jlabsRecordDestination::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsRecordDestination::releaseNative( jlabsRecordDestinationHandle id )
{
	ENTRY;
}
