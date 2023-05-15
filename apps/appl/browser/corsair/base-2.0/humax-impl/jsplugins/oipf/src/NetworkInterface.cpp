// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "NetworkInterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

using namespace Ooif;

NetworkInterface::NetworkInterface(NetworkInterfaceHandle i) : WrapperObject<NetworkInterface>(i)
{
	ENTRY;
}

NetworkInterface::NetworkInterface(const NetworkInterface &original) : WrapperObject<NetworkInterface>(original.identifier)
{
	ENTRY;
}

int NetworkInterface::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("ipAddress", native_NetworkInterfaceGetIPAddress(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("macAddress", native_NetworkInterfaceGetMacAddress(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("connected", native_NetworkInterfaceGetConnected(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("enabled", native_NetworkInterfaceGetEnabled(identifier, &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int NetworkInterface::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("enabled", native_NetworkInterfaceSetEnabled(identifier, BOOLEAN_VALUE(value[0])));

	SET_READONLY("ipAddress");
	SET_READONLY("macAddress");
	SET_READONLY("connected");

	return JSP_PUT_NOTFOUND;
}

void NetworkInterface::releaseNative(NetworkInterfaceHandle id)
{
	NATIVE_WARN_NOK(native_NetworkInterfaceReleaseHandle(id));
}
#endif // OIPF
