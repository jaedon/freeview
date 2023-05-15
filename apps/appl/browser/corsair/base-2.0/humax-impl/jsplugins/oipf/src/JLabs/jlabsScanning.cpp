// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF && defined JLABS
#include "jlabsScanning.h"
#include "humax/ChannelScanParameters.h"
#include "ChannelScanOptions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

jlabsScanning::jlabsScanning(int id_type) : WrapperObject<jlabsScanning>((void*)idType)
{
	ENTRY;
	idType = id_type;
	classType = CLASS_JLABS_SCANNING;	//class_name;
}

jlabsScanning::jlabsScanning(jlabsScanningHandle i) : WrapperObject<jlabsScanning>(i)
{
	ENTRY;

	classType = CLASS_JLABS_SCANNING;	//class_name;
}

jlabsScanning::jlabsScanning(const jlabsScanning &original) : WrapperObject<jlabsScanning>(original.identifier)
{
	ENTRY;
	classType = CLASS_JLABS_SCANNING;	//class_name;
}

jlabsScanning::~jlabsScanning()
{
	ENTRY;
}

int jlabsScanning::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("networkType", native_jlabsScanningGetNetworkType(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	GET_OBJECT("channelScanParameter", obj, new ChannelScanParameters(getIdentifier()), JSP_GET_VALUE_CACHE);
	GET_OBJECT("channelScanOptions", obj, new ChannelScanOptions(getIdentifier()), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("favId", native_jlabsScanningGetFavoriteID(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsScanning::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	NATIVE_SET("favId", native_jlabsScanningSetFavoriteID(getIdentifier(), STRING_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

void jlabsScanning::releaseNative(jlabsScanningHandle id)
{
	NATIVE_WARN_NOK(native_jlabsScanningReleaseHandle(id));
}

#endif // defined OIPF && defined JLABS
