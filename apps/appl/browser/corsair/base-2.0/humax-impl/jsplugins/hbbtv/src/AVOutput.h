// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __AVOUTPUT
#define __AVOUTPUT

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeConfiguration.h"

namespace Ooif
{
class AVOutput : public ObjectInstance, public WrapperObject<AVOutput>
{
public:
	AVOutput(AVOutputHandle i);
	AVOutput(const AVOutput &original);
	~AVOutput();

	static void releaseNative(AVOutputHandle id);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

};
} /* namespace Ooif */

#endif // __AVOUTPUT
#endif // OIPF
