// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ObjectInstance.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>

#include "jsplugin.h"

#include "macros.h"

using namespace std;
using namespace Lovefilm;

ObjectInstance::ObjectInstance() : classType(CLASS_DEFAULT), host(NULL)
{
	ENTRY;
}

ObjectInstance::~ObjectInstance()
{
	ENTRY;
	if (host)
		host->plugin_private = NULL;
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
//	NJSE_LOG_DEBUG("I have been destroyed: %p", this);
	return JSP_DESTROY_OK;
}

void ObjectInstance::gc(jsplugin_obj *obj)
{
//	NJSE_LOG_DEBUG("GC TRACE for me %p", this);
	for (list<jsplugin_obj*>::iterator it=proteges.begin(); it!=proteges.end(); it++)
	{
		jsplugin_obj *protected_obj = *it;
//		NJSE_LOG_DEBUG("  Protecting me %p", protected_obj);
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
