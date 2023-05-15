// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __OBJECTINSTANCE
#define __OBJECTINSTANCE

#include "jsplugin.h"

#include <list>

namespace Ooif
{
typedef enum
{
    CLASS_INVALID,
    CLASS_DEFAULT,
    CLASS_PROGRAMME,
    CLASS_BOOKMARK,
    CLASS_QUERY,
    CLASS_CHANNEL,
    CLASS_CHANNELLIST,
    CLASS_PARENTALRATINGSCHEME,
    CLASS_DOWNLOAD,
    CLASS_AVCOMPONENT,
    CLASS_PARENTALRATINGCOLLECTION,
#if defined(OIPF) || defined(HBBTV_1_2_1)
    CLASS_METADATASEARCH,
    CLASS_SEARCHRESULT,
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
    CLASS_BOOKMARKCOLLECTION,
#endif // OIPF
    CLASS_APPLICATIONPRIVATEDATA,
    CLASS_EVENT,
    CLASS_KEYSET,
    CLASS_FAVOURITELISTCOLLECTION,
#if defined(HBBTV_PVR) || defined(OIPF)
    CLASS_PVR,
#endif // HBBTV_PVR || OIPF
    CLASS_PARENTAL_RATING_SCHEME_COLLECTION,
#ifdef OIPF
    CLASS_SIGNAL_INFO,
    CLASS_TUNER,
    CLASS_CHANNELSCANOPTIONS,
    CLASS_CHANNELSCANPARAMETERS,
    CLASS_WIDGETDESCRIPTOR,
    CLASS_WIDGETDESCRIPTORCOLLECTION,
#endif // OIPF
    CLASSTYPE_EVENTTARGET, // All classes after this point should be subclasses of EventTarget
    CLASS_DRMAGENT,
    CLASS_APPLICATIONMANAGER,
    CLASS_DOWNLOADMANAGER,
    CLASS_RECORDINGSCHEDULER,
    CLASS_APPLICATION,
#if defined(OIPF) || defined(HBBTV_1_2_1)
    CLASS_SEARCHMANAGER,
#endif // OIPF || HBBTV_1_2_1
#if defined(OIPF) || defined(HBBTV) //JUKEBOX_SUPPORT
    CLASS_LOCAL_SYSTEM,
#endif
#ifdef OIPF //JUKEBOX_SUPPORT
    CLASS_CHANNEL_CONFIG,
#endif // OIPF
    CLASSTYPE_VISUAL, //ALL classes after this point should be subclasses of VisualObject
    CLASS_VIDEO_BROADCAST,
    CLASS_VIDEO_ON_DEMAND
} ClassType;

class ObjectInstance
{
protected:
	// Used for class introspection to verify the type of objects passed to functions
	ClassType classType;
	// This is the jsplugin object which is hosting us (so host->plugin_private == this)
	jsplugin_obj *host;
	// List of objects that this plugin instance wants to protect from garbage collection
	std::list<jsplugin_obj*> proteges;

public:
	ObjectInstance();
	virtual ~ObjectInstance();
	virtual void setHost(jsplugin_obj *h)
	{
		host = h;
	}
	virtual jsplugin_obj *getHost()
	{
		return host;
	}
	ClassType getType()
	{
		return classType;
	}
	void setAttrs(int attrs_count, jsplugin_attr *attrs);

	void addProtege(jsplugin_obj *obj);
	void removeProtege(jsplugin_obj *obj);

	static int getter_proxy(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	static int setter_proxy(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	static int destructor_proxy(jsplugin_obj *obj);
	static void gc_proxy(jsplugin_obj *obj);
	static void unload_proxy(jsplugin_obj *obj);
	static void attr_change_proxy(jsplugin_obj *obj, const char *name, const char *value);
	static void param_set_proxy(jsplugin_obj *obj, const char *name, const char *value);

#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
    static std::list<void*> instances;
    static void debugPrintObjectInstanceCount();
    __inline static const char* __getType__(int _type);
#endif

protected:
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result) = 0;
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int destructor(jsplugin_obj *obj);
	virtual void gc(jsplugin_obj *obj);
	virtual void attr_change(jsplugin_obj *obj, const char *name, const char *value);
	virtual void param_set(jsplugin_obj *obj, const char *name, const char *value);

};
} /* namespace Ooif */

#endif // __OBJECTINSTANCE
