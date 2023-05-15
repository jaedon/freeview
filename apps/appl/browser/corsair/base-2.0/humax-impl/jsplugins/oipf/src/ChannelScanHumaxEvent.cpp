/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelScanHumaxEvent.h"
#include "macros.h"
#include <string.h>

#define CHANNEL_SCAN_HUMAX = 1

using namespace Ooif;

ChannelScanHumaxEvent::ChannelScanHumaxEvent(jsplugin_obj *c,
			OOIFNumber type, OOIFNumber progress, OOIFNumber frequency,
			OOIFNumber signalStrength, OOIFNumber channelNumber, OOIFNumber channelType,
			OOIFNumber channelCount, OOIFNumber transponderCount,
			int _infoCount, humax::SearchedInfo_Data *_searchedInfos,
			humax::ChannelTuned_Data	*tunedInfo) :
			ChannelScanEvent(c, type, progress, frequency, signalStrength, channelNumber \
			    , channelType, channelCount, transponderCount, _infoCount , _searchedInfos \
			    , tunedInfo, "ChannelScanHumax")
{
	ENTRY; 
}

ChannelScanHumaxEvent::~ChannelScanHumaxEvent()
{
	ENTRY; 
} 

