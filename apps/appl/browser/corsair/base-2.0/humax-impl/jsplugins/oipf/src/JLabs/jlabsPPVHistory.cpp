#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsPPVHistory.h"
#include "NativeJlabsPPVHistory.h"

using namespace Ooif;

void jlabsPPVHistory::init()
{
	classType = CLASS_JLABS_PPV_HISTORY;
}

jlabsPPVHistory::jlabsPPVHistory(jlabsPPVHistoryHandle i) : WrapperObject<jlabsPPVHistory>(i)
{
	ENTRY;
	init();
}

jlabsPPVHistory::jlabsPPVHistory(const jlabsPPVHistory &original) : WrapperObject<jlabsPPVHistory>(original.identifier)
{
	ENTRY;
	init();
}

jlabsPPVHistory::~jlabsPPVHistory()
{
	ENTRY;
}

int jlabsPPVHistory::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("fee", native_jlabsPPVHistroyGetFee(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("eventData", native_jlabsPPVHistroyGetEventData(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("date", native_jlabsPPVHistroyGetDate(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	/* read only */
	NATIVE_GET_STRING("caSystemId", native_jlabsPPVHistroyGetCaSystemId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	/* function */
	GET_FUNCTION(get, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(set, obj, "ss", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsPPVHistory::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("fee", native_jlabsPPVHistroySetFee(getIdentifier(), STRING_VALUE(value[0])));
	NATIVE_SET("eventData", native_jlabsPPVHistroySetEventData(getIdentifier(), STRING_VALUE(value[0])));

	NATIVE_SET("date", native_jlabsPPVHistroySetDate(getIdentifier(), NUMBER_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

//static
int jlabsPPVHistory::get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_jlabsPPVHistoryGet(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int jlabsPPVHistory::set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_jlabsPPVHistorySet(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}

void jlabsPPVHistory::releaseNative(jlabsPPVHistoryHandle id)
{
	NATIVE_WARN_NOK(native_jlabsPPVHistroyReleaseHandle(id));
}
#endif // #if defined OIPF && defined JLABS
