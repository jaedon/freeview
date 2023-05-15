// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "Tuner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "NativeTuner.h"
#include "SignalInfo.h"

using namespace Ooif;

void TunerDataWrapper::init(void *id)
{
	memset(&data, 0, sizeof(TunerData));
	NATIVE_WARN_NOK(native_TunerInitialize(id, &data));
}

TunerDataWrapper::~TunerDataWrapper()
{
	DATA_FREE_STRING(name);
}

void Tuner::init()
{
	classType = class_name;
}

Tuner::Tuner(TunerHandle i) : DataObject<Tuner, TunerDataWrapper>(i)
{
	ENTRY;
	init();
}

Tuner::Tuner(const Tuner &original) : DataObject<Tuner, TunerDataWrapper>(original)
{
	ENTRY;
	init();
}

Tuner::~Tuner()
{
	ENTRY;
}

int Tuner::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	DATA_GET_NUMBER(id);
	DATA_GET_STRING(name);
	NATIVE_GET_BOOLEAN("enableTuner", native_TunerGetEnableTuner(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("powerOnExternal", native_TunerGetPowerOnExternal(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("signalInfo", obj, native_TunerGetSignalInfo(getIdentifier(), &RETVAL),
	                  new SignalInfo(RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("idTypes", obj,
	                             native_TunerGetIdTypes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int Tuner::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("enableTuner", native_TunerSetEnableTuner(getIdentifier(), BOOLEAN_VALUE(value[0])));
	NATIVE_SET("powerOnExternal", native_TunerSetPowerOnExternal(getIdentifier(), BOOLEAN_VALUE(value[0])));

	SET_READONLY("name");
	SET_READONLY("idTypes");
	SET_READONLY("signalInfo");

	return JSP_PUT_NOTFOUND;
}

void Tuner::releaseNative(TunerHandle id)
{
	ENTRY;
	NATIVE_WARN_NOK(native_TunerReleaseHandle(id));
}
#endif // OIPF
