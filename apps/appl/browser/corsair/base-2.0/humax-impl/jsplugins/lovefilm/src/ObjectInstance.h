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

namespace Lovefilm
{
typedef enum
{
    CLASS_INVALID,
    CLASS_DEFAULT,
    CLASS_EVENT,
    CLASSTYPE_EVENTTARGET, // All classes after this point should be subclasses of EventTarget
    CLASSTYPE_VISUAL, //ALL classes after this point should be subclasses of VisualObject

    CLASS_LOVEFILM_DEVICE,
    CLASS_DEVICE_ERROR,
    
    CLASS_PLAYER,
    CLASS_BUFFERINGTYPES,
    CLASS_ABRCONFIG,
    CLASS_SMOOTHSTREAMINGABRCONFIG,
    CLASS_DRMCONFIG,
    CLASS_PLAYREADYDRMCONFIG,
    CLASS_METADATATYPES,
    CLASS_AUDIOSTREAMMETADATA,
    CLASS_VIDEOSTREAMMETADATA,
    CLASS_COMBINEDSTREAMMETADATA,
    CLASS_STREAMTYPES,
    CLASS_AUDIOSTREAM,
    CLASS_VIDEOSTREAM,
    CLASS_COMBINEDSTREAM, 
    CLASS_FRAGMENT,
    CLASS_FRAGMENTCOLLECTION
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

protected:
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result) = 0;
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
	virtual int destructor(jsplugin_obj *obj);
	virtual void gc(jsplugin_obj *obj);
	virtual void attr_change(jsplugin_obj *obj, const char *name, const char *value);
	virtual void param_set(jsplugin_obj *obj, const char *name, const char *value);

};
} /* namespace Lovefilm */

#endif // __OBJECTINSTANCE
