#ifdef OIPF
#include "HMX_Output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

//##########################################
HMX_Output::HMX_Output(HMX_OutputHandle i) : WrapperObject<HMX_Output>(i)
{
	ENTRY;
}

HMX_Output::HMX_Output(const HMX_Output &original) : WrapperObject<HMX_Output>(original.identifier)
{
	ENTRY;
}

HMX_Output::~HMX_Output()
{
	ENTRY;
}

int HMX_Output::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	// Common Flag
	NATIVE_GET_STRING("name", native_HOutputGetName(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("type", native_HOutputGetType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("enabled", native_HOutputGetEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	// HW Check Flags
	NATIVE_GET_BOOLEAN("scartEnabled", native_HOutputGetScartEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("compositeEnabled", native_HOutputGetCompositeEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("componentEnabled", native_HOutputGetComponentEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("rfEnabled", native_HOutputGetRfEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hdmiEnabled", native_HOutputGetHdmiEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("spdifEnabled", native_HOutputGetSpdifEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	// SCART Specific.
	NATIVE_GET_BOOLEAN("tvscartEnabled", native_HOutputGetTvScartEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("tvscartFormat", native_HOutputGetTvScartFormat(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedTvScartFormats", obj,
								 native_HOutputGetSupportedTvScartFormats(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("vcrscartEnabled", native_HOutputGetVcrScartEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("vcrscartFormat", native_HOutputGetVcrScartFormat(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedVcrScartFormats", obj,
								 native_HOutputGetSupportedVcrScartFormats(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);

	// SCART Specific.
	NATIVE_GET_STRING("videoStandard", native_HOutputGetVideoStandard(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("avAdditionalSignal", native_HOutputGetAvAddtionalSignal(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("tvAspectRatio", native_HOutputGetTVAspectRatio(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("wss2hdmiMode", native_HOutputGetWss2HdmiMode(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("videoDisplayFormat", native_HOutputGetVideoDisplayFormat(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("curResolution", native_HOutputGetCurrentResolution(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING_COLLECTION("supportedResolutions", obj,
								 native_HOutputGetSupportedResolutions(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);

	NATIVE_GET_BOOLEAN("audioDescriptionEnabled", native_HOutputGetAudioDescriptionEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("subtitleEnabled", native_HOutputGetSubtitleEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hardOfHearingEnabled", native_HOutputGetHardOfHearingEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HMX_Output::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("enabled", native_HOutputSetEnabled(identifier, BOOLEAN_VALUE(value[0])));

	NATIVE_SET("tvscartFormat", native_HOutputSetTvScartFormat(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("vcrscartFormat", native_HOutputSetVcrScartFormat(identifier, STRING_VALUE(value[0])));

	NATIVE_SET("tvAspectRatio", native_HOutputSetTVAspectRatio(identifier, STRING_VALUE(value[0])));

	NATIVE_SET("videoDisplayFormat", native_HOutputSetVideoDisplayFormat(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("curResolution", native_HOutputSetCurrentResolution(identifier, STRING_VALUE(value[0])));

	NATIVE_SET("audioDescriptionEnabled", native_HOutputSetAudioDescriptionEnabled(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("subtitleEnabled", native_HOutputSetSubtitleEnabled(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("hardOfHearingEnabled", native_HOutputSetHardOfHearingEnabled(identifier, BOOLEAN_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

void HMX_Output::releaseNative(HMX_OutputHandle id)
{
	NATIVE_WARN_NOK(native_HOutputReleaseHandle(id));
}

#endif // OIPF

