// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(HBBTV_PVR) || defined(OIPF)
#ifndef __PVR
#define __PVR
#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeRecording.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
#ifdef OIPF
class PVRDataWrapper
{
public:
	static PVRData data;
	void init(void *id);
};

class PVR : public ObjectInstance, public DataObject<PVR, PVRDataWrapper>
#else
class PVR : public ObjectInstance, public WrapperObject<PVR>
#endif // OIPF
{
private:
	void init();
	ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
public:
	PVR(PVRHandle i);
	PVR(const PVR &original);
	~PVR();

	static void releaseNative(PVRHandle id);
	static const ClassType class_name = CLASS_PVR;
};
} /* Ooif */

#endif // __PVR
#endif // HBBTV_PVR || OIPF
