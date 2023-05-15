// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __FIELDS_CONTAINER
#define __FIELDS_CONTAINER

#include "jsplugin.h"
#include "ObjectInstance.h"
#include <string.h>
#include <map>

namespace Ooif
{
class FieldsContainer : public ObjectInstance
{
public:
	typedef struct
	{
		int type;
		union
		{
			char* string;
			double number;
			int boolean;
			jsplugin_obj* object;
		} u;
	} field;
private:
	struct _l_cmp
	{
		bool operator()(const char* a, const char *b)
		{
			return strcmp(a, b) < 0;
		}
	};
	typedef std::map<const char*, field, _l_cmp> _l_properties;
	_l_properties _properties;
public:
	FieldsContainer();
	virtual ~FieldsContainer();
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

	/**
	 * Adds new property to the container
	 */
	void register_property(const char* name);

	/**
	 * Returns type of the property
	 */
	int property_type(const char* name);

	/**
	 * Returns number value stored in property
	 */
	double getNumber(const char* name);

	/**
	 * Returns boolean value stored in property
	 */
	int getBoolean(const char* name);

	/**
	 * Returns string value stored in property
	 */
	const char* getString(const char* name);

	/**
	 * Returns object stored in property
	 */
	jsplugin_obj* getObject(const char* name);
};
} // namespace Ooif
#endif // __FIELDS_CONTAINER
