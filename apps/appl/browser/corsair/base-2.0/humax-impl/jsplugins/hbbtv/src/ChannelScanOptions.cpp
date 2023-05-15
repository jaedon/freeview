// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include "ChannelScanOptions.h"
#include "FieldsContainer.h"

#include "macros.h"

using namespace Ooif;

ChannelScanOptions::ChannelScanOptions()
{
	ENTRY;
	classType = class_name;
	register_property("channelType");
	register_property("replaceExisting");
}

#endif // OIPF
