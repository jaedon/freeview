// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __NETWORKINTERFACE
#define __NETWORKINTERFACE

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeConfiguration.h"

namespace Ooif
{
class NetworkInterface : public ObjectInstance, public WrapperObject<NetworkInterface>
{
public:
	NetworkInterface(NetworkInterfaceHandle i);
	NetworkInterface(const NetworkInterface &original);
	static void releaseNative(NetworkInterfaceHandle id);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
};
} /* namespace Ooif */

#endif // __NETWORKINTERFACE
#endif // OIPF
