// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "AVOutput.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

AVOutput::AVOutput(AVOutputHandle i) : WrapperObject<AVOutput>(i)
{
	ENTRY;
}

AVOutput::AVOutput(const AVOutput &original) : WrapperObject<AVOutput>(original.identifier)
{
	ENTRY;
}

AVOutput::~AVOutput()
{
	ENTRY;
}

int AVOutput::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("name", native_AVOutputGetName(identifier, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("type", native_AVOutputGetType(identifier, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_BOOLEAN("enabled", native_AVOutputGetEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("subtitleEnabled", native_AVOutputGetSubtitleEnabled(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("videoMode", native_AVOutputGetVideoMode(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("digitalAudioMode", native_AVOutputGetDigitalAudioMode(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("audioRange", native_AVOutputGetAudioRange(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("hdVideoFormat", native_AVOutputGetHDVideoFormat(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("tvAspectRatio", native_AVOutputGetTVAspectRatio(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedVideoModes", obj,
	                             native_AVOutputGetSupportedVideoModes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING_COLLECTION("supportedDigitalAudioModes", obj,
	                             native_AVOutputGetSupportedDigitalAudioModes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING_COLLECTION("supportedAudioRanges", obj,
	                             native_AVOutputGetSupportedAudioRanges(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING_COLLECTION("supportedHdVideoFormats", obj,
	                             native_AVOutputGetSupportedHdVideoFormats(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING_COLLECTION("supportedAspectRatios", obj,
	                             native_AVOutputGetSupportedAspectRatios(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int AVOutput::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("enabled", native_AVOutputSetEnabled(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("subtitleEnabled", native_AVOutputSetSubtitleEnabled(identifier, BOOLEAN_VALUE(value[0])));
	NATIVE_SET("videoMode", native_AVOutputSetVideoMode(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("digitalAudioMode", native_AVOutputSetDigitalAudioMode(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("audioRange", native_AVOutputSetAudioRange(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("hdVideoFormat", native_AVOutputSetHDVideoFormat(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("tvAspectRatio", native_AVOutputSetTVAspectRatio(identifier, STRING_VALUE(value[0])));

	SET_READONLY("name");
	SET_READONLY("type");
	SET_READONLY("supportedVideoModes");
	SET_READONLY("supportedDigitalAudioModes");
	SET_READONLY("supportedAudioRanges");
	SET_READONLY("supportedHdVideoFormats");
	SET_READONLY("supportedAspectRatios");

	return JSP_PUT_NOTFOUND;
}

void AVOutput::releaseNative(AVOutputHandle id)
{
	ENTRY;
	NATIVE_WARN_NOK(native_AVOutputReleaseHandle(id));
}

#endif // OIPF
