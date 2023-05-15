// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __SELECTED_COMPONENT_CHANGED_EVENT
#define __SELECTED_COMPONENT_CHANGED_EVENT

#include "Event.h"
#include "macros.h"

namespace Ooif
{
class SelectedComponentChangedEvent : public Event
{
private:
	OOIFNumber _componentType;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	SelectedComponentChangedEvent(jsplugin_obj *c, OOIFNumber componentType);
	SelectedComponentChangedEvent(jsplugin_obj *c, OOIFNumber componentType, const char *name);
	virtual ~SelectedComponentChangedEvent();
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __SELECTED_COMPONENT_CHANGED_EVENT
#endif // OIPF || HBBTV_1_2_1
