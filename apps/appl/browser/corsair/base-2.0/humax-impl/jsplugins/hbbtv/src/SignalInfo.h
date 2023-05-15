// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __SIGNAL_INFO
#define __SIGNAL_INFO

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeSignalInfo.h"

namespace Ooif
{
class SignalInfo : public ObjectInstance, public WrapperObject<SignalInfo>
{
private:
	void init();

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	SignalInfo(SignalInfoHandle i);
	SignalInfo(const SignalInfo &original);
	~SignalInfo();

	static void releaseNative(SignalInfoHandle id);
	static const ClassType class_name = CLASS_SIGNAL_INFO;
};
} /* namespace Ooif */

#endif // __SIGNAL_INFO
#endif // OIPF
