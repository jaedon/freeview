// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __SEARCHMANAGER
#define __SEARCHMANAGER

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include <list>

#include "NativeSearch.h"
#include "EventTarget.h"

namespace Ooif
{
class SearchManager : public EventTarget, public WrapperObject<SearchManager>
{
protected:
	static list<SearchManager*> instances;
	static SearchManager* GetInstance(SearchManagerHandle id);
	virtual OOIFReturnCode checkListenerAccessAllowed(const char* eventName);

public:
	virtual ~SearchManager();
	SearchManager(SearchManagerHandle i);

	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	// Specific functions for this interface
	static int createSearch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	// Listener functions
	static void onMetadataEvent(SearchManagerEvent event, SearchManagerEventInfo *info, SearchManagerHandle id);
	static void releaseNative(SearchManagerHandle id);
	static const ClassType class_name = CLASS_SEARCHMANAGER;
};
} /* namespace Ooif */

#endif // __SEARCHMANAGER
#endif // OIPF || HBBTV_1_2_1
