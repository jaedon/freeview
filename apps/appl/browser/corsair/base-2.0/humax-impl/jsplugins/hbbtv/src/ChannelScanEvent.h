/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __CHANNEL_SCAN_EVENT
#define __CHANNEL_SCAN_EVENT

#include "Event.h"

namespace Ooif
{
class ChannelScanEvent : public Event
{
private:
	OOIFNumber m_type;
	OOIFNumber m_progress;
	OOIFNumber m_frequency;
	OOIFNumber m_signalStrength;
	OOIFNumber m_channelNumber;
	OOIFNumber m_channelType;
	OOIFNumber m_channelCount;
	OOIFNumber m_transponderCount;
public:
	ChannelScanEvent(jsplugin_obj *c, OOIFNumber type, OOIFNumber progress, OOIFNumber frequency, OOIFNumber signalStrength,
	                 OOIFNumber channelNumber, OOIFNumber channelType, OOIFNumber channelCount, OOIFNumber transponderCount);
	virtual ~ChannelScanEvent();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int serialize(jsplugin_value **argv);
};
} /* namespace Ooif */

#endif // __CHANNEL_SCAN_EVENT
#endif // OIPF
