#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsSystemInfo.h"
#include "NativeJlabsSystemInfo.h"

using namespace Ooif;

int jlabsSystemInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	/* read only */
	NATIVE_GET_NUMBER("docsisModemStatus", native_jlabsSystemInfoGetDocsisModemStatus(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisDownFrequency", native_jlabsSystemInfoGetDocsisDownFrequency(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("docsisQamType", native_jlabsSystemInfoGetDocsisQamType(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisDownSignalLevel", native_jlabsSystemInfoGetDocsisDownSignalLevel(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisDownBer", native_jlabsSystemInfoGetdDocsisDownBer(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisDownSnr", native_jlabsSystemInfoGetDocsisDownSnr(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisUpFrequency", native_jlabsSystemInfoGetDocsisUpFrequency(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisUpChannelId", native_jlabsSystemInfoGetDocsisUpChannelId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisUpBandWidth", native_jlabsSystemInfoGetDocsisUpBandWidth(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("docsisUpSignalLevel", native_jlabsSystemInfoGetDocsisUpSignalLevel(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(setDocsisDownStreamFrequency, obj, "n", JSP_GET_VALUE_CACHE);

	NATIVE_GET_STRING("cardIdBCAS", native_jlabsSystemInfoGetCardIdBCAS(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("discernBcas", native_jlabsSystemInfoGetDiscernBCAS(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("groupIdBcas", obj,
								 native_jlabsSystemInfoGetGroupIdBCAS(&SIZE, &NATIVE_ARRAY),JSP_GET_VALUE);
	NATIVE_GET_STRING("cardIdCCAS", native_jlabsSystemInfoGetCardIdCCAS(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("discernCcas", native_jlabsSystemInfoGetDiscernCCAS(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("groupIdCcas", obj,
								 native_jlabsSystemInfoGetGroupIdCCAS(&SIZE, &NATIVE_ARRAY),JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceId", native_jlabsSystemInfoGetDeviceId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceModel", native_jlabsSystemInfoGetDeviceModel(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceSerialNumber", native_jlabsSystemInfoGetDeviceSerialNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceSoftwareVersion", native_jlabsSystemInfoGetDeviceSoftwareVersion(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceHardwareVersion", native_jlabsSystemInfoGetDeviceHardwareVersion(&RETVAL), JSP_GET_VALUE);

	NATIVE_GET_NUMBER("systemUpdateDate", native_jlabsSystemInfoGetSystemUpdateDate(&RETVAL), JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("cardStatusBCAS", native_jlabsSystemInfoGetCardStatusBCAS(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("cardStatusCCAS", native_jlabsSystemInfoGetCardStatusCCAS(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(get, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startSettingInfo, obj, "n", JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}

//static
int jlabsSystemInfo::get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_jlabsSystemInfoGet(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int jlabsSystemInfo::startSettingInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	OOIFBoolean retVal = FALSE;
	NATIVE_CALL(native_jlabsSystemInfoStartSettingInfo(NUMBER_VALUE(argv[0]), &retVal));
	RETURN_BOOLEAN(retVal, JSP_CALL_VALUE);
}

int jlabsSystemInfo::setDocsisDownStreamFrequency(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	OOIFBoolean retVal = FALSE;
	NATIVE_CALL(native_jlabsSystemInfoSetDocsisDownFrequency(NUMBER_VALUE(argv[0]), &retVal));
	RETURN_BOOLEAN(retVal, JSP_CALL_VALUE);
}

#endif // #if defined OIPF && defined JLABS
