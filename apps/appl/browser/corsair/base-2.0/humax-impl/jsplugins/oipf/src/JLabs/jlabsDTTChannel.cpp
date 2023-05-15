
#if defined OIPF && defined JLABS

#include "jlabsDTTChannel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"


using namespace Ooif;

void	jlabsDTTChannel::init()
{
	classType = CLASS_JLABS_DTT_CHANNEL;
}

jlabsDTTChannel::jlabsDTTChannel(jlabsChannelHandle i) : Channel(i)
{
	ENTRY;
	init();
}

jlabsDTTChannel::jlabsDTTChannel(const jlabsDTTChannel &original) : Channel(original)
{
	ENTRY;
	init();
}

jlabsDTTChannel::~jlabsDTTChannel()
{
	ENTRY;
}

int jlabsDTTChannel::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_BROADCAST);
#endif // HBBTV_SECURITY

	DATA_GET_NUMBER(serviceType);
	DATA_GET_NUMBER(threeDigit);
	NATIVE_GET_BOOLEAN("invisible", native_jlabsChannelGetInvisible(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longName", native_jlabsChannelGetProviderName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getLogo, obj, "nn", JSP_GET_VALUE_CACHE);

	/* getter가 겹치므로 우선 처리하도록 */
	return Channel::getter(obj, name, result);
}

int jlabsDTTChannel::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("invisible", native_jlabsChannelSetInvisible(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("manualBlock", native_ChannelSetManualBlock(getIdentifier(), BOOLEAN_VALUE(value[0])));

	return Channel::setter(obj, name, value);
}



int jlabsDTTChannel::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	jlabsDTTChannel *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsDTTChannel);

	const char *retval;
	NATIVE_CALL(native_jlabsChannelGetField(instance->getIdentifier(), STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

// TODO: getLogo 재정의해줘야 하는가.
int jlabsDTTChannel::getLogo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	jlabsDTTChannel *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsDTTChannel);

	const char *retval;
	NATIVE_CALL(native_jlabsChannelGetLogo(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

void jlabsDTTChannel::releaseNative(jlabsChannelHandle id)
{
	NATIVE_WARN_NOK(native_jlabsChannelReleaseHandle(id));
}

#endif
