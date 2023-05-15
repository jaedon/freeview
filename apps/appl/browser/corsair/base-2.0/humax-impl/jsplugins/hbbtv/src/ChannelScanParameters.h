// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __CHANNELSCANPARAMETERS
#define __CHANNELSCANPARAMETERS

#include "FieldsContainer.h"
#include "NativeChannel.h"

namespace Ooif
{
class ChannelScanParameters : public FieldsContainer
{
public:
	ChannelScanParameters();
	virtual ~ChannelScanParameters() {}
	enum CscType
	{
	    CSP_DVBT = 0,
	    CSP_DVBS = 1,
	    CSP_DVBC = 2
	};

	virtual int type() = 0;
	static const ClassType class_name = CLASS_CHANNELSCANPARAMETERS;
};

class DVBTChannelScanParameters : public ChannelScanParameters
{
public:
	DVBTChannelScanParameters();
	int type()
	{
		return CSP_DVBT;
	}
};

class DVBSChannelScanParameters : public ChannelScanParameters
{
public:
	DVBSChannelScanParameters();
	int type()
	{
		return CSP_DVBS;
	}
};

class DVBCChannelScanParameters : public ChannelScanParameters
{
public:
	DVBCChannelScanParameters();
	int type()
	{
		return CSP_DVBC;
	}
};
} /* namespace Ooif */

#endif // __CHANNELSCANPARAMETERS
#endif //OIPF
