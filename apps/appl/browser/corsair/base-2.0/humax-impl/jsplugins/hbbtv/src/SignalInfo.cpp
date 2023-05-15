// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "SignalInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

void SignalInfo::init()
{
	classType = class_name;
}

SignalInfo::SignalInfo(SignalInfoHandle i) : WrapperObject<SignalInfo>(i)
{
	ENTRY;
	init();
}

SignalInfo::SignalInfo(const SignalInfo &original) : WrapperObject<SignalInfo>(original.identifier)
{
	ENTRY;
	init();
}

SignalInfo::~SignalInfo()
{
	ENTRY;
}

int SignalInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_NUMBER("strength", native_SignalInfoGetStrength(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("quality", native_SignalInfoGetQuality(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("ber", native_SignalInfoGetBer(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("snr", native_SignalInfoGetSnr(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("lock", native_SignalInfoGetLock(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int SignalInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_READONLY("strength");
	SET_READONLY("quality");
	SET_READONLY("ber");
	SET_READONLY("snr");
	SET_READONLY("lock");

	return JSP_PUT_NOTFOUND;
}

void SignalInfo::releaseNative(SignalInfoHandle id)
{
	ENTRY;
	NATIVE_WARN_NOK(native_SignalInfoReleaseHandle(id));
}
#endif // OIPF
