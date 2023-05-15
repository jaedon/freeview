#ifdef OIPF
#include "HMX_Audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

//##########################################
HMX_Audio::HMX_Audio(HMX_AudioHandle i) : WrapperObject<HMX_Audio>(i)
{
	ENTRY;
}

HMX_Audio::HMX_Audio(const HMX_Audio &original) : WrapperObject<HMX_Audio>(original.identifier)
{
	ENTRY;
}

HMX_Audio::~HMX_Audio()
{
	ENTRY;
}

int HMX_Audio::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	// Common Flag
	NATIVE_GET_STRING("name", native_HAudioGetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("type", native_HAudioGetType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("enabled", native_HAudioGetEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	// HW Check Flags
	NATIVE_GET_NUMBER("maxVolume", native_HAudioGetMaxVolume(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("minVolume", native_HAudioGetMinVolume(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lipSyncDelay", native_HAudioGetLipSyncDelay(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("soundMode", native_HAudioGetSoundMode(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("transcodingEnabled", native_HAudioGetTranscodingEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("modeDigitalAudioOutput", native_HAudioGetDigitalOutputMode(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("codecDigitalAudioOutput", native_HAudioGetDigitalOutputCodec(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("modeTranscoding", native_HAudioGetTranscodingMode(identifier, &RETVAL), JSP_GET_VALUE);


	NATIVE_GET_STRING_COLLECTION("supportedSoundModes", obj,
								 native_HAudioGetSupportedSoundOutputModes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedDigitalAudioOutputCodecs", obj,
								 native_HAudioGetSupportedDigitalOutputCodecs(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedDigitalAudioOutputModes", obj,
								 native_HAudioGetSupportedDigitalOutputModes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedTranscodingModes", obj,
								 native_HAudioGetSupportedTranscodingModes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HMX_Audio::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("enabled", native_HAudioSetEnabled(identifier, BOOLEAN_VALUE(value[0])));

	NATIVE_SET("lipSyncDelay", native_HAudioSetLipSyncDelay(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("soundMode", native_HAudioSetSoundMode(identifier, STRING_VALUE(value[0])));


	NATIVE_SET("modeDigitalAudioOutput", native_HAudioSetDigitalOutputMode(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("codecDigitalAudioOutput", native_HAudioSetDigitalOutputCodec(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("modeTranscoding", native_HAudioSetTranscodingMode(identifier, STRING_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

void HMX_Audio::releaseNative(HMX_AudioHandle id)
{
	NATIVE_WARN_NOK(native_HAudioReleaseHandle(id));
}

#endif // OIPF


