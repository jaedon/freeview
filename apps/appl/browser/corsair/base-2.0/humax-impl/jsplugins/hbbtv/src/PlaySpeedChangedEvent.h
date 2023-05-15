// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PLAY_SPEED_CHANGED_EVENT
#define __PLAY_SPEED_CHANGED_EVENT

#include "Event.h"
#include "NativeOOIFTypes.h"

namespace Ooif
{
class PlaySpeedChangedEvent : public Event
{
private:
	OOIFNumber speed;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	/**
	   @param c object event occured on
	   @param s new speed
	 */
	PlaySpeedChangedEvent(jsplugin_obj *c, OOIFNumber s);
	virtual ~PlaySpeedChangedEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __PLAY_SPEED_CHANGED_EVENT
