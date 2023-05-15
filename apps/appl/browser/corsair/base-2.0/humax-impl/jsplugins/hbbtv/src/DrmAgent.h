// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __DRMAGENT
#define __DRMAGENT

#include "jsplugin.h"

#include "EventTarget.h"
#include "WrapperObject.h"
#include "NativeDrmAgent.h"

namespace Ooif
{
class DrmAgent : public EventTarget, public WrapperObject<DrmAgent>
{
private:
	static list<DrmAgent*> instances;
	static DrmAgent* GetInstance(DrmAgentHandle id);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	DrmAgent(DrmAgentHandle i);
	~DrmAgent();
	// Javascript functions
	static int sendDRMMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int DRMSystemStatus(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);

	// Static member functions
	static void onDrmAgentEvent(DrmEvent event, DrmEventInfo *info, DrmAgentHandle id);
	static void releaseNative(DrmAgentHandle id);
	static const ClassType class_name = CLASS_DRMAGENT;
};
} /* namespace Ooif */

#endif // __DRMAGENT
