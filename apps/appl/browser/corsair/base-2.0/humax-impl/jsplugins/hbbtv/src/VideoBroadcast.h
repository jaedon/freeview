// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __VIDEOBROADCAST
#define __VIDEOBROADCAST

#include "jsplugin.h"
#include "WrapperObject.h"
#include "VisualObject.h"
#include "NativeVideoBroadcast.h"
#include "Collection.h"

#include <string.h>
#include <stdlib.h>

namespace Ooif
{
class VideoBroadcast : public VisualObject, public WrapperObject<VideoBroadcast>
{
private:
	static list<VideoBroadcast*> instances;
	static VideoBroadcast* GetInstance(VideoBroadcastHandle id);

	ProgrammeCollection *getProgrammes(int count, void **handles);
#if defined(HBBTV_PVR) || defined(OIPF)
	NumberCollection *getPlaySpeeds(int count, OOIFNumber *speeds);
#endif // HBBTV_PVR || OIPF

	BroadcastPlaystate playstate;

	// StreamEvent support
	void addStreamEventListener(const char *streamEventName, jsplugin_obj *listener);
	void internalRemoveStreamEventListener(const char *streamEventName, jsplugin_obj *listener);
	bool isAddStreamEventListenerAllowed();
	void removeAllStreamEventListeners();

protected:
	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
	virtual OOIFReturnCode updateNativeWindowState(WindowState requested_state);
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	void attr_change(jsplugin_obj* obj, const char* name, const char* value);

public:
	virtual ~VideoBroadcast();
	VideoBroadcast(VideoBroadcastHandle i);

	// JS functions
	static int getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createChannelObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int bindToCurrentChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int prevChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int nextChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setFullScreen(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int setVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	static int release(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#if defined(HBBTV_PVR) || defined(OIPF)
	static int recordNow(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stopRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int pause(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int resume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int setSpeed(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int seek(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int stopTimeshift(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // HBBTV_PVR || OIPF
	static int getComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getCurrentActiveComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int selectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int unselectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int internalAddStreamEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int internalAddXMLStreamEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int removeStreamEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	// Specific functions for this interface
	virtual void videoVisible(int visibility, int unloading);
	virtual void videoPosition(int x, int y, int w, int h);
	static void onVideoBroadcastEvent(BroadcastEvent event, BroadcastEventInfo *info, VideoBroadcastHandle id);
	static void onVideoBroadcastStreamEvent(const char *name, void *listener_id, const char *data, const char *text, const char *status, VideoBroadcastHandle id);
	static void releaseNative(VideoBroadcastHandle id);
	static const ClassType class_name = CLASS_VIDEO_BROADCAST;
};
} /* namespace Ooif */

#endif // __VIDEOBROADCAST
