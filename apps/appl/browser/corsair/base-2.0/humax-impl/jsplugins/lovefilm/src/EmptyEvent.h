// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __EMPTY_EVENT
#define __EMPTY_EVENT

#include "Event.h"

namespace Lovefilm
{
/**
   Generic class for events with no arguments.
*/

class EmptyEvent : public Event
{
public:
	virtual ~EmptyEvent();
	EmptyEvent(jsplugin_obj *c, const char* name);

	virtual int serialize(jsplugin_value **values);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
};
} /* namespace lovefilm */

#endif // __EMPTY_EVENT
