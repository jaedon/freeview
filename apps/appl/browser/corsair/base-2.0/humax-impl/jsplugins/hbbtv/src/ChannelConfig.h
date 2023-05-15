// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __CHANNELCONFIG
#define __CHANNELCONFIG

#include "jsplugin.h"

#include "ChannelList.h"
#ifdef OIPF
#include "EventTarget.h"
#include "EmptyEvent.h"
#endif // OIPF

namespace Ooif
{
typedef enum
{
    DEFAULT,
    RECORDING
} ChannelConfigSelection;
#ifdef OIPF
class ChannelConfig : public EventTarget
{
private:
	static list<ChannelConfig*> instances;
#else
class ChannelConfig : public ObjectInstance
{
#endif // OIPF
protected:
	ChannelList *channels;

public:
	ChannelConfig();
	ChannelConfig(ChannelConfigSelection selection);
	~ChannelConfig();
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
#ifdef OIPF
	// Specific functions for this interface
	static int createFilteredList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelScanParametersObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelScanOptionsObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static void onChannelConfigEvent(ChannelConfigEvent event, ChannelConfigEventInfo *info);

	static const ClassType class_name = CLASS_CHANNEL_CONFIG;
#endif // OIPF
};
} /* namespace Ooif */

#endif // __CHANNELCONFIG
