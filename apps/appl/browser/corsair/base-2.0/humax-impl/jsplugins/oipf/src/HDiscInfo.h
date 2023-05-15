// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#ifndef __HDISCINFO
#define __HDISCINFO

#include "jsplugin.h"

#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeDisc.h"

namespace Ooif
{
class HDiscInfo : public ObjectInstance, public WrapperObject<HDiscInfo>
{
public:
	HDiscInfo(HDiscInfoHandle i);
	HDiscInfo(const HDiscInfo &original);
	virtual ~HDiscInfo();

	static void releaseNative(HDiscInfoHandle id);

	static const ClassType class_name = CLASS_HDISC_INFO;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	// Javascript functions
};
} /* namespace Ooif */

#endif // __HDISCINFO
#endif // OIPF
