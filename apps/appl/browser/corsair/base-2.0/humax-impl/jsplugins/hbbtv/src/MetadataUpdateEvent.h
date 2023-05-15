// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __MDUPDATEEVENT
#define __MDUPDATEEVENT

#include "Event.h"

#include "NativeSearch.h"
#include "Channel.h"
#include "Programme.h"

namespace Ooif
{
class MetadataUpdateEvent : public Event
{
protected:
	int action;
	int info;
	Programme *programme;
	Channel *channel;
	SearchItemType objectType;

public:
	virtual ~MetadataUpdateEvent();
	MetadataUpdateEvent(jsplugin_obj *c, int _action, int _info, void *object, SearchItemType type);
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif //__MDUPDATEEVENT
#endif // OIPF
