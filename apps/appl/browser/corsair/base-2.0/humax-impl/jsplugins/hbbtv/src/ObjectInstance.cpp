// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ObjectInstance.h"
#include "AsyncHandler.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>

#include "jsplugin.h"

#include "macros.h"

using namespace std;
using namespace Ooif;

#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
std::list<void*> ObjectInstance::instances;

#define CASE_OBJTYPE(x) case x: return string(#x).data();
__inline const char* ObjectInstance::__getType__(int _type)
{
    ClassType type = static_cast<ClassType>(_type);

    switch ( type )
    {
        CASE_OBJTYPE( CLASS_INVALID );
        CASE_OBJTYPE( CLASS_DEFAULT );
        CASE_OBJTYPE( CLASS_PROGRAMME );
        CASE_OBJTYPE( CLASS_BOOKMARK );
        CASE_OBJTYPE( CLASS_QUERY );
        CASE_OBJTYPE( CLASS_CHANNEL );
        CASE_OBJTYPE( CLASS_CHANNELLIST );
        CASE_OBJTYPE( CLASS_PARENTALRATINGSCHEME );
        CASE_OBJTYPE( CLASS_DOWNLOAD );
        CASE_OBJTYPE( CLASS_AVCOMPONENT );
        CASE_OBJTYPE( CLASS_PARENTALRATINGCOLLECTION );
#if defined(OIPF) || defined(HBBTV_1_2_1)
        CASE_OBJTYPE( CLASS_METADATASEARCH );
        CASE_OBJTYPE( CLASS_SEARCHRESULT );
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
        CASE_OBJTYPE( CLASS_BOOKMARKCOLLECTION );
#endif // OIPF
        CASE_OBJTYPE( CLASS_APPLICATIONPRIVATEDATA );
        CASE_OBJTYPE( CLASS_EVENT );
        CASE_OBJTYPE( CLASS_KEYSET );
        CASE_OBJTYPE( CLASS_FAVOURITELISTCOLLECTION );
#if defined(HBBTV_PVR) || defined(OIPF)
        CASE_OBJTYPE( CLASS_PVR );
#endif // HBBTV_PVR || OIPF
        CASE_OBJTYPE( CLASS_PARENTAL_RATING_SCHEME_COLLECTION );
#ifdef OIPF
        CASE_OBJTYPE( CLASS_SIGNAL_INFO );
        CASE_OBJTYPE( CLASS_TUNER );
        CASE_OBJTYPE( CLASS_CHANNELSCANOPTIONS );
        CASE_OBJTYPE( CLASS_CHANNELSCANPARAMETERS );
        CASE_OBJTYPE( CLASS_WIDGETDESCRIPTOR );
        CASE_OBJTYPE( CLASS_WIDGETDESCRIPTORCOLLECTION );
#endif // OIPF
        CASE_OBJTYPE( CLASSTYPE_EVENTTARGET ); // All classes after this point should be subclasses of EventTarget
        CASE_OBJTYPE( CLASS_DRMAGENT );
        CASE_OBJTYPE( CLASS_APPLICATIONMANAGER );
        CASE_OBJTYPE( CLASS_DOWNLOADMANAGER );
        CASE_OBJTYPE( CLASS_RECORDINGSCHEDULER );
        CASE_OBJTYPE( CLASS_APPLICATION );
#if defined(OIPF) || defined(HBBTV_1_2_1)
        CASE_OBJTYPE( CLASS_SEARCHMANAGER );
#endif // OIPF || HBBTV_1_2_1
#if defined(OIPF) || defined(HBBTV) //JUKEBOX_SUPPORT
        CASE_OBJTYPE( CLASS_LOCAL_SYSTEM );
#endif
#ifdef OIPF //JUKEBOX_SUPPORT
        CASE_OBJTYPE( CLASS_CHANNEL_CONFIG );
#endif // OIPF
        CASE_OBJTYPE( CLASSTYPE_VISUAL ); //ALL classes after this point should be subclasses of VisualObject
        CASE_OBJTYPE( CLASS_VIDEO_BROADCAST );
        CASE_OBJTYPE( CLASS_VIDEO_ON_DEMAND );
        default:
            OOIF_LOG_INFO("Unknown ObjectInstance (type: %d) \n", type);
            break;
    }
    return "";
}

void ObjectInstance::debugPrintObjectInstanceCount()
{
    map<int, unsigned int> types;
    map<string, unsigned int> events;
    for ( list<void*>::iterator it = instances.begin(); \
            it != instances.end(); ++it )
    {
        ObjectInstance *instance = static_cast<ObjectInstance*>(*it);
        types[instance->getType()]++;

        if ( CLASS_EVENT == instance->getType() )
        {
            Event *evt = static_cast<Event*>(instance);
            events[string(evt->eventName)]++;
        }
    }
    OOIF_LOG_INFO("== List of existing Hbbtv ObjectInstances WebApplication is having at this moment ==\n");
    for( map<int, unsigned int>::iterator it = types.begin(); \
            it != types.end(); ++it )
    {
        OOIF_LOG_INFO(" ObjectInstance: %40s [ref count : %4d] \n", __getType__(it->first), it->second);
    }
    OOIF_LOG_INFO("==============================================================================\n\n");

    if ( 0 < events.size() )
    {
        OOIF_LOG_INFO("====================== List of detail for CLASS_EVENT ========================\n");
        for ( map<string, unsigned int>::iterator it = events.begin(); \
                it != events.end(); ++it )
        {
            string event_name = string(it->first);
            OOIF_LOG_INFO(" Event : %48s [ref count : %4d] \n", event_name.data(), it->second);
        }
        OOIF_LOG_INFO("==============================================================================\n\n");
    }
}
#endif

ObjectInstance::ObjectInstance() : classType(CLASS_DEFAULT), host(NULL)
{
	ENTRY;
    
#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
    instances.push_back(this);
#endif
}

ObjectInstance::~ObjectInstance()
{
	ENTRY;
	if (host)
	{
		host->plugin_private = NULL;
	}
    
#if defined(CONFIG_DEBUG) && defined(OBJ_INSTANCE_COUNTER)
    instances.remove(this);
#endif    
}

void ObjectInstance::addProtege(jsplugin_obj *obj)
{
#ifdef DEBUG
	if (find(proteges.begin(), proteges.end(), obj) != proteges.end())
		assert(false);
#endif
    if ( obj != NULL )
    {
        proteges.push_back(obj);
    }
}

void ObjectInstance::removeProtege(jsplugin_obj *obj)
{
#ifdef DEBUG
	if (find(proteges.begin(), proteges.end(), obj) == proteges.end())
		assert(false);
#endif
    if ( obj != NULL )
    {
        proteges.remove(obj);
    }
}

//static
int ObjectInstance::getter_proxy(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	return instance ? instance->getter(obj, name, result) : JSP_GET_NOTFOUND;
}

//static
int ObjectInstance::setter_proxy(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	return instance ? instance->setter(obj, name, value) : JSP_PUT_NOTFOUND;
}

//static
int ObjectInstance::destructor_proxy(jsplugin_obj *obj)
{
    ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
    if (instance)
    {
    	instance->destructor(obj);
    	delete instance;
        obj->plugin_private = NULL;
    }
    return JSP_DESTROY_OK;
}

//static
void ObjectInstance::gc_proxy(jsplugin_obj *obj)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
		instance->gc(obj);
}

//static
void ObjectInstance::unload_proxy(jsplugin_obj *obj)
{
	ENTRY;
#ifdef OOIF_ASYNC
	AsyncHandler::untrackToken(obj);
#endif // OOIF_ASYNC
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
	{
		delete instance;
		obj->plugin_private = NULL;
	}
}

//static
void ObjectInstance::attr_change_proxy(jsplugin_obj *obj, const char *name, const char *value)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
	{
		instance->attr_change(obj, name, value);
	}
}

//static
void ObjectInstance::param_set_proxy(jsplugin_obj *obj, const char *name, const char *value)
{
	ObjectInstance *instance = EXTRACT_OBJECT<ObjectInstance>(obj);
	if (instance)
	{
		instance->param_set(obj, name, value);
	}
}

int ObjectInstance::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

int ObjectInstance::destructor(jsplugin_obj *obj)
{
//	OOIF_LOG_DEBUG("I have been destroyed: %p", this);
	return JSP_DESTROY_OK;
}

void ObjectInstance::gc(jsplugin_obj *obj)
{
//	OOIF_LOG_DEBUG("GC TRACE for me %p", this);
	for (list<jsplugin_obj*>::iterator it=proteges.begin(); it!=proteges.end(); it++)
	{
		jsplugin_obj *protected_obj = *it;
//		OOIF_LOG_DEBUG("  Protecting me %p", protected_obj);
		g_opera_callbacks->gcmark(protected_obj);
	}
}

void ObjectInstance::attr_change(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
}

void ObjectInstance::param_set(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
}

void ObjectInstance::setAttrs(int attrs_count, jsplugin_attr *attrs)
{
	ENTRY;
	for (int i = 0; i < attrs_count; ++i)
		attr_change(getHost(), attrs[i].name, attrs[i].value);
}
