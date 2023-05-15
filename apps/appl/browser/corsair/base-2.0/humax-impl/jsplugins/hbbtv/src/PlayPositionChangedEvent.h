// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PLAY_POSITION_CHANGED_EVENT
#define __PLAY_POSITION_CHANGED_EVENT

#include "Event.h"

namespace Ooif
{
class PlayPositionChangedEvent : public Event
{
private:
	OOIFBoolean has_position;
	OOIFNumber position;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	/**
	   @param c object event occured on
	   @param has_pos dermine if the position should be undefined (when this is set to false) or be specified by the following parameter
	   @param pos the new position if has_pos is true, ignored otherwise
	 */
	PlayPositionChangedEvent(jsplugin_obj *c, OOIFBoolean has_pos, OOIFNumber pos);
	virtual ~PlayPositionChangedEvent();

	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __PLAY_POSITION_CHANGED_EVENT
