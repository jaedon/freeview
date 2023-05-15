
#if defined OIPF && defined JLABS

#include "jlabsBSCATVChannel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"


using namespace Ooif;

void jlabsBSCATVChannel::init()
{
	classType = CLASS_JLABS_BS_CATV_CHANNEL;
}

jlabsBSCATVChannel::jlabsBSCATVChannel(jlabsChannelHandle i) : Channel(i)
{
	ENTRY;
	init();
}

jlabsBSCATVChannel::jlabsBSCATVChannel(const jlabsBSCATVChannel &original) : Channel(original)
{
	ENTRY;
	init();
}

jlabsBSCATVChannel::~jlabsBSCATVChannel()
{
	ENTRY;
}

int jlabsBSCATVChannel::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_BROADCAST);
#endif // HBBTV_SECURITY

	DATA_GET_NUMBER(serviceType);
	DATA_GET_NUMBER(threeDigit);
	NATIVE_GET_NUMBER("oneTouch", native_jlabsChannelGetOneTouch(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("invisible", native_jlabsChannelGetInvisible(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longName", native_jlabsChannelGetProviderName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getLogo, obj, "nn", JSP_GET_VALUE_CACHE);

	/* getter가 겹치므로 우선 처리하도록 */
	return Channel::getter(obj, name, result);
}

int jlabsBSCATVChannel::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("invisible", native_jlabsChannelSetInvisible(getIdentifier(), BOOLEAN_VALUE(value[0])));
	NATIVE_SET("oneTouch", native_jlabsChannelSetOneTouch(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("manualBlock", native_ChannelSetManualBlock(getIdentifier(), BOOLEAN_VALUE(value[0])));

	return Channel::setter(obj, name, value);
}


int jlabsBSCATVChannel::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	jlabsBSCATVChannel *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsBSCATVChannel);

	const char *retval;
	NATIVE_CALL(native_jlabsChannelGetField(instance->getIdentifier(), STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

// TODO: getLogo 재정의해줘야 하는가.
int jlabsBSCATVChannel::getLogo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	jlabsBSCATVChannel *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsBSCATVChannel);

	const char *retval;
	NATIVE_CALL(native_jlabsChannelGetLogo(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

void jlabsBSCATVChannel::releaseNative(jlabsChannelHandle id)
{
	NATIVE_WARN_NOK(native_jlabsChannelReleaseHandle(id));
}

#endif
