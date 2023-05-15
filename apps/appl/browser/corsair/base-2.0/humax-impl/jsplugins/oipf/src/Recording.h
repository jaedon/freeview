// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __RECORDING
#define __RECORDING

#if defined(HBBTV_PVR) || defined(OIPF)

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeRecording.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"
#include "ScheduledRecording.h"

namespace Ooif
{
	/*
	  The Recording object on this level is really only a single int
	  identifying it to the native layer. In order to get the relevant
	  information, it needs to ask the native layer.
	 */

	class Recording : public ScheduledRecording//, public WrapperObject<Recording>
	{
	private:
		void init();
		ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);
	public:
		Recording(RecordingHandle i);
		Recording(const Recording &original);
		virtual ~Recording();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		NumberCollection *getCasIDs(int count, OOIFNumber *casIDs);
		static int getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void releaseNative(RecordingHandle id);
		static const ClassType class_name = CLASS_RECORDING;
		static int setName(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	};
} /* namespace Ooif */
#endif // HBBTV_PVR || OIPF
#endif // __RECORDING

/* EOF */

