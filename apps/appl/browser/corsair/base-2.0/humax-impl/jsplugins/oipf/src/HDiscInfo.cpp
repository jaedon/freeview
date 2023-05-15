// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#include "HDiscInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "NativeDisc.h"

using namespace Ooif;

HDiscInfo::HDiscInfo(HDiscInfoHandle i) : WrapperObject<HDiscInfo>(i)
{
	ENTRY;
	classType = CLASS_HDISC_INFO;

}
HDiscInfo::HDiscInfo(const HDiscInfo &original) : WrapperObject<HDiscInfo>(original.identifier)
{
	ENTRY;
	classType = CLASS_HDISC_INFO;
}

HDiscInfo::~HDiscInfo()
{
}

int HDiscInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#ifdef JLABS
	NATIVE_GET_NUMBER("free", native_HDiscInfoGetFree(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("total", native_HDiscInfoGetTotal(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("reserved", native_HDiscInfoGetReserved(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isFormatted", native_HDiscInfoGetIsFormatted(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("discId", native_HDiscInfoGetDiscId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_HDiscInfoGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#else
	NATIVE_GET_NUMBER("free", native_DiscInfoGetFree(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("total", native_DiscInfoGetTotal(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("reserved", native_DiscInfoGetReserved(&RETVAL), JSP_GET_VALUE);
#endif

	return JSP_GET_NOTFOUND;
}
int HDiscInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
#ifdef JLABS
	NATIVE_SET("name", native_HDiscInfoSetName(getIdentifier(), STRING_VALUE(value[0])));
#endif

	return JSP_PUT_NOTFOUND;
}
void HDiscInfo::releaseNative(HDiscInfoHandle id)
{
#ifdef JLABS
	NATIVE_WARN_NOK(native_HDiscInfoReleaseHandle(id));
#else
	NATIVE_WARN_NOK(0);
#endif
}

#endif // OIPF
