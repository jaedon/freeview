// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __SCHEDULEDRECORDING
#define __SCHEDULEDRECORDING

#if defined(HBBTV_PVR) || defined(OIPF)

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeRecording.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
	/*
	  The ScheduledRecording object on this level is really only a single int
	  identifying it to the native layer. In order to get the relevant
	  information, it needs to ask the native layer.
	 */

	class ScheduledRecording : public ObjectInstance, public WrapperObject<ScheduledRecording>
	{
	private:
		void init();
		ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);

	public:
		ScheduledRecording(ScheduledRecordingHandle i);
		ScheduledRecording(const ScheduledRecording &original);
		virtual ~ScheduledRecording();

		virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative(ScheduledRecordingHandle id);
		static const ClassType class_name = CLASS_SCHEDULEDRECORDING;
#ifdef JLABS
		static int setName(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif
	};

	template <>
	inline ScheduledRecording * EXTRACT_OBJECT<ScheduledRecording>(jsplugin_obj * arg)
	{
	    if (!arg)
	        return NULL;
	    ObjectInstance *o= (ObjectInstance*)(arg->plugin_private);
	    if (!o)
	        return NULL;
	    if (o->getType() == CLASS_RECORDING || o->getType() == CLASS_SCHEDULEDRECORDING)
	    {
	        return static_cast<ScheduledRecording*>(o);
	    }
	    return NULL;
	}

} /* namespace Ooif */

#endif // HBBTV_PVR || OIPF
#endif // __SCHEDULEDRECORDING

/* EOF */

