// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include "ChannelScanParameters.h"
#include "FieldsContainer.h"

#include "macros.h"
#include "NativeChannel.h"

using namespace Ooif;

ChannelScanParameters::ChannelScanParameters()
{
	ENTRY;
	classType = class_name;
}

DVBTChannelScanParameters::DVBTChannelScanParameters()
{
	ENTRY;
	register_property("startFrequency");
	register_property("endFrequency");
	register_property("raster");
	register_property("ofdm");
	register_property("modulationModes");
	register_property("bandwidth");
}

DVBSChannelScanParameters::DVBSChannelScanParameters()
{
	ENTRY;
	register_property("startFrequency");
	register_property("endFrequency");
	register_property("modulationModes");
	register_property("symbolRate");
	register_property("polarisation");
	register_property("codeRate");
	register_property("orbitalPosition");
	register_property("networkId");
}

DVBCChannelScanParameters::DVBCChannelScanParameters()
{
	ENTRY;
	register_property("startFrequency");
	register_property("endFrequency");
	register_property("raster");
	register_property("startNetworkScanOnNIT");
	register_property("modulationModes");
	register_property("symbolRate");
	register_property("networkId");
}

#endif // OIPF