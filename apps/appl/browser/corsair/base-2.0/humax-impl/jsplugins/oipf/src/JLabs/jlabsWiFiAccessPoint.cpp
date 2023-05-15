// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "jlabsWiFiAccessPoint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

/***********************************************************************************
 *  jlabsWiFiAccessPoint Class
 ***********************************************************************************
 */

jlabsWiFiAccessPoint::jlabsWiFiAccessPoint( void * id ) : WrapperObject<jlabsWiFiAccessPoint>(id)
{
	classType = CLASS_JLABS_NETWORK_ACCESSPOINT;
}

jlabsWiFiAccessPoint::jlabsWiFiAccessPoint( const jlabsWiFiAccessPoint &original) : WrapperObject<jlabsWiFiAccessPoint>(original.identifier)
{
	classType = CLASS_JLABS_NETWORK_ACCESSPOINT;

}
jlabsWiFiAccessPoint::~jlabsWiFiAccessPoint()
{
	ENTRY;
}

int jlabsWiFiAccessPoint::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result )
{
	/* read only */
	NATIVE_GET_STRING("ssid", native_jlabsWiFiAccessPointGetEssid(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("macAddress", native_jlabsWiFiAccessPointGetMacAddress(identifier, &RETVAL), JSP_GET_VALUE);
#if 1
	NATIVE_GET_NUMBER("security", native_jlabsWiFiAccessPointGetSecureTypeN(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("authen", native_jlabsWiFiAccessPointGetAuthenTypeN(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("encrypt", native_jlabsWiFiAccessPointGetEncryptTypeN(identifier, &RETVAL), JSP_GET_VALUE);
#else
	NATIVE_GET_STRING("security", native_jlabsWiFiAccessPointGetSecureType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("authen", native_jlabsWiFiAccessPointGetAuthenType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("encrypt", native_jlabsWiFiAccessPointGetEncryptType(identifier, &RETVAL), JSP_GET_VALUE);
#endif

	NATIVE_GET_NUMBER("mode", native_jlabsWiFiAccessPointGetMode(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("waveStrength", native_jlabsWiFiAccessPointGetWaveStrength(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("connectSpeed", native_jlabsWiFiAccessPointGetConnectSpeed(identifier, &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int jlabsWiFiAccessPoint::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value )
{
#if 0
	NATIVE_SET("essid", native_jlabsWiFiAccessPointSetEssid(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("secure", native_jlabsWiFiAccessPointSetSecureType(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("authen", native_jlabsWiFiAccessPointSetAuthenType(identifier, NUMBER_VALUE(value[0])));
	NATIVE_SET("encrypt", native_jlabsWiFiAccessPointSetEncryptType(identifier, NUMBER_VALUE(value[0])));
#endif
	return JSP_PUT_NOTFOUND;
}

//static
void jlabsWiFiAccessPoint::releaseNative( void * id )
{
	NATIVE_WARN_NOK(native_jlabsWiFiAccessPointReleaseHandle(id));
}

#endif // OIPF
