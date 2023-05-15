// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "HAlexaHelper.h"

#include "EmptyEvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "NativeHAlexaHelper.h"
#include "Exception.h"

using namespace Ooif;

//static
list<HAlexaHelper*> HAlexaHelper::instances;

HAlexaHelper::HAlexaHelper()
{
	ENTRY;
	classType = class_name;
	HAlexaHelper::instances.push_back(this);
}

HAlexaHelper::~HAlexaHelper()
{
	ENTRY;
	HAlexaHelper::instances.remove(this);
}


int HAlexaHelper::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

	GET_FUNCTION(processData, obj, "s", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HAlexaHelper::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

// static
int HAlexaHelper::processData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	NATIVE_CALL(native_HAlexaHelper_ProcessData(STRING_VALUE(argv[0])));
	return JSP_CALL_NO_VALUE;
}

#endif // OIPF
