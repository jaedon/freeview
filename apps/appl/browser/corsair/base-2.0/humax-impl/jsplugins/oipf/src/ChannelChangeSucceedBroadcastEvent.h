// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __CHANNEL_CHANGE_SUCCEED_BROADCAST_EVENT
#define __CHANNEL_CHANGE_SUCCEED_BROADCAST_EVENT

#include "Event.h"
#include "NativeChannel.h"
#include "Channel.h"

namespace Ooif
{
class ChannelChangeSucceedBroadcastEvent : public Event
{
private:
	Channel *channel;
public:
	ChannelChangeSucceedBroadcastEvent(jsplugin_obj *c, ChannelHandle id);
	~ChannelChangeSucceedBroadcastEvent();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __CHANNEL_CHANGE_SUCCEED_BROADCAST_EVENT
