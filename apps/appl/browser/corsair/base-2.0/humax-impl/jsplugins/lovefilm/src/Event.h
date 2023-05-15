// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __EVENT
#define __EVENT

#include <stddef.h>

#include "jsplugin.h"

#include "ObjectInstance.h"

#include "NativeNJSETypes.h"

namespace Lovefilm
{
class Event : public ObjectInstance
{
private:
	bool wait_for_gc;
	void initialize();

protected:
	jsplugin_obj *context;

public:
	char *eventName;
	char *stringRep;

	/** Garbage collection: needed to decide ownership of the
	 * event object. If the event is sent to the JS plugin the
	 * ownership will be handed over (wait_for_gc == true), if not
	 * it should be deleted within Lovefilm once the event is 'handled'.
	 */
	void setWaitForGC()
	{
		wait_for_gc = true;
	}
	bool waitForGC()
	{
		return wait_for_gc;
	}

	Event(jsplugin_obj *c, const char *name);
	virtual ~Event();
	virtual jsplugin_obj *getHost();
	static int toString(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int valueOf(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	// Serialize the state of the event to a set of parameters to dispatch to intrinsic listeners
	virtual int serialize(jsplugin_value **argv) = 0;
	virtual void dispatched() {}
	virtual void* getUserData()
	{
		return NULL;
	}
	virtual jsplugin_async_callback* getListenerCallback();
	virtual void notFired() {}

	static const ClassType class_name = CLASS_EVENT;
};
} /* namespace Lovefilm */

#endif // __EVENT
