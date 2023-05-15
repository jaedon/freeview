// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __VIDEO_ON_DEMAND
#define __VIDEO_ON_DEMAND

#include "jsplugin.h"

#include "WrapperObject.h"
#include "VisualObject.h"
#include "Collection.h"

#include "NativeVideoOnDemand.h"

namespace Ooif
{
class VideoOnDemand : public VisualObject, public WrapperObject<VideoOnDemand>
{

private:
	// static list of all VideoOnDemand instances
	static list<VideoOnDemand*> instances;
	static VideoOnDemand* GetInstance(VodHandle id);
	static NumberCollection* getNumberCollection(int count, OOIFNumber *numbers);

protected:
	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
	virtual OOIFReturnCode updateNativeWindowState(WindowState requested_state);
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	VideoOnDemand(VodHandle i);
	virtual ~VideoOnDemand();
	void attr_change(jsplugin_obj *obj, const char *name, const char *value);
	void param_set(jsplugin_obj *obj, const char *name, const char *value);
	// JS functions
	static int play(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int seek(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int seekSegment(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int next(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int previous(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getCurrentActiveComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int selectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int unselectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int queue(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if defined(HBBTV_PVR) || defined(HBBTV_DOWNLOAD) || defined(OIPF)
	static int setSource(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // HBBTV_PVR || HBBTV_DOWNLOAD || OIPF
#ifdef OIPF
	static int getSinkProtocolInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getTransportStateVariables(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setTransportStateVariables(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getAvailablePlayTime(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setBufferingStrategy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setRepresentationStrategy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	// Callbacks from platform
	static void onVideoOnDemandEvent(VodEvent event, VodEventInfo *info, VodHandle id);
	//Visual Object functions
	virtual void videoVisible(int visibility, int unloading);
	virtual void videoPosition(int x, int y, int w, int h);
	static void releaseNative(VodHandle id);
	static const ClassType class_name = CLASS_VIDEO_ON_DEMAND;
};
} /* namespace Ooif */

#endif // __VIDEO_ON_DEMAND
