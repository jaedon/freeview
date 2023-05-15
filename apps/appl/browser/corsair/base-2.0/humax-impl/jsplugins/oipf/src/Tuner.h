// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __TUNER
#define __TUNER

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeTuner.h"
#include "NativeHConfiguration.h"

namespace Ooif
{
/*
  The Tuner object caches the data from the time of
  creation.
 */

class TunerDataWrapper
{
public:
	TunerData data;
	void init(void *id);
	~TunerDataWrapper();
};

class Tuner : public ObjectInstance, public DataObject<Tuner, TunerDataWrapper>
{
private:
	void init();

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	Tuner(TunerHandle i);
	Tuner(const Tuner &original);
	~Tuner();

	static void releaseNative(TunerHandle id);

	static const ClassType class_name = CLASS_TUNER;
};
} /* namespace Ooif */

#endif // __TUNER
#endif // OIPF
