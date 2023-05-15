/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CHANNEL_LIST_UPDATE_HUMAX_EVENT
#define __CHANNEL_LIST_UPDATE_HUMAX_EVENT

#include "ChannelListUpdateEvent.h"
#ifdef	HMX_WEBUI
#include "ChannelList.h"
#endif

namespace Ooif
{
	class ChannelListUpdateHumaxEvent : public ChannelListUpdateEvent
	{ 
    public:
        ChannelListUpdateHumaxEvent(jsplugin_obj *c);
#ifdef	HMX_WEBUI
        ChannelListUpdateHumaxEvent(ChannelList *list, jsplugin_obj *c);
#endif
        virtual ~ChannelListUpdateHumaxEvent(); 
	};
} /* namespace Ooif */

#endif // __CHANNEL_LIST_UPDATE_EVENT
