// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#ifndef __DOWNLOADMANAGER
#define __DOWNLOADMANAGER

#include "jsplugin.h"
#include "EventTarget.h"
#include "Download.h"

namespace Ooif
{
// Avoid collisions with internal namespaces (grmbl!)
#define DownloadManager DAEDownloadManager

class DownloadManager : public EventTarget
{
private:
	static list<DownloadManager*> instances;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	DownloadManager();
	~DownloadManager();
	// Specific functions for this interface
	static int getADownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getDownloads(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int createFilteredList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int updateRegisteredDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
	static int pause(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int resume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	// Event Listener
	static void onDownloadEvent(DownloadEvent event, DownloadEventInfo* info);
	virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);
	static const ClassType class_name = CLASS_DOWNLOADMANAGER;
};
} /* namespace Ooif */

#endif // __DOWNLOADMANAGER
#endif // HBBTV_DOWNLOAD || OIPF
