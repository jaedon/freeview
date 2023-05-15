// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __CHANNELSCANOPTIONS
#define __CHANNELSCANOPTIONS

#include "FieldsContainer.h"

namespace Ooif
{
class ChannelScanOptions : public FieldsContainer
{
public:
	ChannelScanOptions();
	static const ClassType class_name = CLASS_CHANNELSCANOPTIONS;
};
} /* namespace Ooif */

#endif // __CHANNELSCANOPTIONS
#endif // OIPF
