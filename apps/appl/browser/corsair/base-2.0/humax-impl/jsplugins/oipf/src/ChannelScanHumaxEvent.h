/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CHANNEL_SCAN_HUMAX_EVENT
#define __CHANNEL_SCAN_HUMAX_EVENT

#include "Event.h"
#include "ChannelConfig.h"
#include "ChannelScanEvent.h"

namespace Ooif
{
	class ChannelScanHumaxEvent : public ChannelScanEvent
	{
    public:
        ChannelScanHumaxEvent(jsplugin_obj *c, OOIFNumber type, OOIFNumber progress, OOIFNumber frequency, OOIFNumber signalStrength,
                OOIFNumber channelNumber, OOIFNumber channelType, OOIFNumber channelCount, OOIFNumber transponderCount,
                int _infoCount, humax::SearchedInfo_Data *_searchedInfos, humax::ChannelTuned_Data	*tunedInfo);
        virtual ~ChannelScanHumaxEvent(); 
	};
} /* namespace Ooif */

#endif // __CHANNEL_SCAN_HUMAX_EVENT
