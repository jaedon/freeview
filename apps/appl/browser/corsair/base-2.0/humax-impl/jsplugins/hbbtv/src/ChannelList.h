// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

/*
  Since ParentalRatingCollection has an additional function beyond
  normal Collections, it is necessary to create a special subclass of
  Collection<ParentalRatingScheme*> to implement it.
 */

#ifndef __CHANNELLIST
#define __CHANNELLIST

#include "jsplugin.h"

#include "macros.h"
#include "Collection.h"

#include "Channel.h"

namespace Ooif
{
typedef enum
{
    ALL,
    RECORDABLE,
    NONE
} ChannelListType;

class ChannelList : public Collection<Channel>
{
public:
	ChannelList(ChannelListType type);
	ChannelList(ChannelList *original);
#ifdef OIPF
	ChannelList(ChannelList *original, OOIFBoolean has_blocked, OOIFBoolean blocked, OOIFBoolean has_favourite, OOIFBoolean favourite, OOIFBoolean has_hidden, OOIFBoolean hidden);
	ChannelList(const char* bdr);
#endif // OIPF
	virtual ~ChannelList();
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	// Javascript functions
	static int getChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getChannelByTriplet(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int getChannelBySourceID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	static const ClassType class_name = CLASS_CHANNELLIST;
};
} /* namespace Ooif */

#endif // __CHANNELLIST
