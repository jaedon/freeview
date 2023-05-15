// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __WIDGET_INSTALLATION_EVENT
#define __WIDGET_INSTALLATION_EVENT

#include "Event.h"
#include "jsplugin.h"
#include "WidgetDescriptor.h"

namespace Ooif
{
class WidgetInstallationEvent : public Event
{
public:
	WidgetInstallationEvent(jsplugin_obj *c, WidgetDescriptorHandle handle,
	                        ApplicationManagerWidgetEventState state, ApplicationManagerWidgetEventStateReason reason);
	virtual ~WidgetInstallationEvent();
	virtual int serialize(jsplugin_value **argv);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

private:
	WidgetDescriptorHandle m_handle;
	ApplicationManagerWidgetEventState m_state;
	ApplicationManagerWidgetEventStateReason m_reason;
};
} /* namespace Ooif */

#endif // __WIDGET_INSTALLATION_EVENT
#endif //OIPF
