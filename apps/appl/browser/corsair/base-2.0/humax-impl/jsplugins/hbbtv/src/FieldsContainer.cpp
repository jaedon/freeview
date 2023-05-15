// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "FieldsContainer.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

using namespace Ooif;

FieldsContainer::FieldsContainer()
{
}

FieldsContainer::~FieldsContainer()
{
	for (_l_properties::iterator it = _properties.begin(); it != _properties.end(); ++it)
	{
		// deallocate string properties and un-protege objects
		field& property = it->second;
		if (property.type == JSP_TYPE_STRING) //remove old string
		{
			HLIB_STD_MemFree(property.u.string);
			property.u.string = NULL;
		}
		else if (property.type == JSP_TYPE_OBJECT) //un-protege
		{
			removeProtege(property.u.object);
		}
	}
}

int FieldsContainer::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	if (_properties.find(name) != _properties.end())
	{
		field& property = _properties[name];
		switch (property.type)
		{
		case JSP_TYPE_NUMBER:
		{
			result->type = JSP_TYPE_NUMBER;
			result->u.number = property.u.number;
			return JSP_GET_VALUE;
		}
		case JSP_TYPE_STRING:
		{
			result->type = JSP_TYPE_STRING;
			result->u.string = property.u.string;
			return JSP_GET_VALUE;
		}
		case JSP_TYPE_BOOLEAN:
		{
			result->type = JSP_TYPE_BOOLEAN;
			result->u.boolean = property.u.boolean;
			return JSP_GET_VALUE;
		}
		case JSP_TYPE_OBJECT:
		{
			result->type = JSP_TYPE_OBJECT;
			result->u.object = property.u.object;
			return JSP_GET_VALUE;
		}
		case JSP_TYPE_NULL:
		{
			result->type = JSP_TYPE_NULL;
			return JSP_GET_VALUE;
		}
		default:
		{
			result->type = JSP_TYPE_UNDEFINED;
			return JSP_GET_VALUE;
		}
		}
	}
	return JSP_GET_NOTFOUND;
}

int FieldsContainer::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	if (_properties.find(name) != _properties.end())
	{
		field& property = _properties[name];

		if (property.type == JSP_TYPE_STRING) //remove old string
		{
			HLIB_STD_MemFree(property.u.string);
			property.u.string = NULL;
		}
		else if (property.type == JSP_TYPE_OBJECT) //un-protege
		{
			removeProtege(property.u.object);
		}

		switch (value->type)
		{
		case JSP_TYPE_NUMBER:
		{
			property.type = JSP_TYPE_NUMBER;
			property.u.number = value->u.number;
			return JSP_PUT_OK;
		}
		case JSP_TYPE_STRING:
		{
			property.type = JSP_TYPE_STRING;
			property.u.string = HLIB_STD_StrDup(value->u.string);
			if (property.u.string)
				return JSP_PUT_OK;
			else
				return JSP_PUT_NOMEM;
		}
		case JSP_TYPE_BOOLEAN:
		{
			property.type = JSP_TYPE_BOOLEAN;
			property.u.boolean = value->u.boolean;
			return JSP_PUT_OK;
		}
		case JSP_TYPE_OBJECT:
		{
			property.type = JSP_TYPE_OBJECT;
			property.u.object = value->u.object;
			addProtege(property.u.object);
			return JSP_PUT_OK;
		}
		case JSP_TYPE_NULL:
		{
			property.type = JSP_TYPE_NULL;
			return JSP_PUT_OK;
		}
		default:
		{
			property.type = JSP_TYPE_UNDEFINED;
			return JSP_PUT_OK;
		}
		}
	}
	return JSP_PUT_NOTFOUND;
}

void FieldsContainer::register_property(const char* name)
{
	field& property = _properties[name];
	property.type = JSP_TYPE_UNDEFINED;
}

int FieldsContainer::property_type(const char* name)
{
	field& property = _properties[name];
	return property.type;
}

double FieldsContainer::getNumber(const char* name)
{
	if (_properties.find(name) == _properties.end())
		OOIF_LOG_DEBUG("FieldsContainer: Number property not found with specified name!!!");
	return _properties[name].u.number;
}

int FieldsContainer::getBoolean(const char* name)
{
	if (_properties.find(name) == _properties.end())
		OOIF_LOG_DEBUG("FieldsContainer: Boolean property not found with specified name!!!");
	return _properties[name].u.boolean;
}

const char* FieldsContainer::getString(const char* name)
{
	if (_properties.find(name) == _properties.end())
		OOIF_LOG_DEBUG("FieldsContainer: String property not found with specified name!!!");
	return _properties[name].u.string;
}

jsplugin_obj* FieldsContainer::getObject(const char* name)
{
	if (_properties.find(name) == _properties.end())
		OOIF_LOG_DEBUG("FieldsContainer: Object property not found with specified name!!!");
	return _properties[name].u.object;
}