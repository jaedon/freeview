// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2012 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CHANNEL
#define __CHANNEL

#include "NativeChannel.h"

#include "ObjectInstance.h"
#include "WrapperObject.h"

#ifdef OIPF
#include "Collection.h"
#endif // OIPF

namespace Ooif
{
/*
  The Channel object caches most of the data from the time of
  creation. However the getField and getLogo are still dynamically
  fetched (and not even lazily cached)
 */

class ChannelDataWrapper
{
public:
	ChannelData data;
	void init(void *id);
	~ChannelDataWrapper();
};

class Channel : public ObjectInstance, public DataObject<Channel, ChannelDataWrapper>
{
private:
	void init();

#if defined(OIPF) && defined(HMX_WEBUI)
	NumberCollection *getCasIDs(int count, OOIFNumber *casIDs);
#endif

public:
	Channel(ChannelHandle i);
	Channel(const Channel &original);
	~Channel();

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

#ifdef OIPF
	// Specific functions
	static int getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getLogo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	static void releaseNative(ChannelHandle id);

#if defined(OIPF) && defined(HMX_WEBUI)
	void changeMajorChannel(int newMajorChannel);
#endif

	static const ClassType class_name = CLASS_CHANNEL;
};
} /* namespace Ooif */

#endif // __CHANNEL
