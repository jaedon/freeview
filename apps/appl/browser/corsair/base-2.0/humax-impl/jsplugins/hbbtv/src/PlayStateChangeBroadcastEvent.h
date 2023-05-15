// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __PLAYSTATE_CHANGE_BROADCAST_EVENT
#define __PLAYSTATE_CHANGE_BROADCAST_EVENT

#include "Event.h"

namespace Ooif
{
class PlayStateChangeBroadcastEvent : public Event
{
private:
	OOIFNumber state;
	OOIFBoolean has_error;
	OOIFNumber error;

public:
	PlayStateChangeBroadcastEvent(jsplugin_obj *c, OOIFNumber _state, OOIFBoolean _has_error, OOIFNumber _error );
	~PlayStateChangeBroadcastEvent();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __PLAYSTATE_CHANGE_BROADCAST_EVENT
