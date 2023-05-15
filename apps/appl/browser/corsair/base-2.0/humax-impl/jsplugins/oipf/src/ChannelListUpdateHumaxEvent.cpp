/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelListUpdateHumaxEvent.h"
#include "macros.h"
#include <string.h>

using namespace Ooif;

ChannelListUpdateHumaxEvent::ChannelListUpdateHumaxEvent(jsplugin_obj *c) : ChannelListUpdateEvent(c, "ChannelListUpdateHumax")
{
	ENTRY;
}

#ifdef	HMX_WEBUI
ChannelListUpdateHumaxEvent::ChannelListUpdateHumaxEvent(ChannelList *list, jsplugin_obj *c)
	: ChannelListUpdateEvent(list, c, "ChannelListUpdateHumax")
{
	ENTRY;
}
#endif

ChannelListUpdateHumaxEvent::~ChannelListUpdateHumaxEvent()
{
	ENTRY;
}
