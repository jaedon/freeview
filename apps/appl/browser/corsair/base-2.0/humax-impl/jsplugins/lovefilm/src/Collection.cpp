// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Collection.h"

#include "jsplugin.h"
#include "macros.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

namespace Lovefilm
{

template <class T>
T BaseCollection<T>::internalGetItem(int index)
{
	return items[index];
}

template <class T>
int BaseCollection<T>::getSize()
{
	return items.size();
}

template <class T>
void BaseCollection<T>::addItem(T item)
{
	items.push_back(item);
}

template <class T>
void BaseCollection<T>::internalInsertAt(T item, int index)
{
	items.insert(items.begin()+index, item);
}

template <class T>
void BaseCollection<T>::internalRemoveIndex(int index)
{
	items.erase(items.begin() + index);
}

template <class T>
void BaseCollection<T>::internalRemoveItem(T item)
{
	items.erase(find(items.begin(), items.end(), item));
}

//static
template <class T>
int BaseCollection<T>::item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	BaseCollection<T> *instance = static_cast<BaseCollection<T>*>(EXTRACT_OBJECT<ObjectInstance>(this_obj));
	if (!instance)
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	if (instance->getSize() > NUMBER_VALUE(argv[0])  && NUMBER_VALUE(argv[0]) >= 0)
	{
		return instance->getItem(this_obj, result, NUMBER_VALUE(argv[0]), JSP_CALL_VALUE);
	}
	else
	{
		RETURN_UNDEFINED(JSP_CALL_VALUE);
	}
}

template <class T>
int BaseCollection<T>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("length", this->getSize(), JSP_GET_VALUE);
	GET_FUNCTION(item, obj, "n", JSP_GET_VALUE_CACHE);

	// Array indexing (eg myCollection[0])
	int index = atoi(name);
	if (index > 0 || name[0] == '0')
	{
		if ( (unsigned int)index >= this->items.size())
			return JSP_GET_NOTFOUND;
		return getItem(obj, result, index, JSP_GET_VALUE);
	}

	return JSP_GET_NOTFOUND;
}

template <class T>
Collection<T>::~Collection()
{
	ENTRY;
	while (!this->items.empty())
	{
		delete this->items.back();
		this->items.pop_back();
	}
}
template <class T>
void Collection<T>::internalRemoveIndex(int index)
{
    delete this->items[index];
    this->items.erase(this->items.begin() + index);
}

template <class T>
int Collection<T>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_OBJECT(this_obj, new T(*(this->internalGetItem(index))), result, return_type);
}

int NumberCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_NUMBER( this->internalGetItem(index), return_type);
}

int StringCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_STRING(this->internalGetItem(index), return_type);
}

StringCollection::~StringCollection()
{
	ENTRY;
	while (!this->items.empty())
	{
		free((void*)this->items.back());
		this->items.pop_back();
	}
}

StringWithLength::StringWithLength(char* string, int string_len) : str(string), length(string_len)
{
	if(length < 0)
		length = strlen(string);
}

StringWithLength::~StringWithLength()
{
	free(str);
}

template <>
int Collection<StringWithLength>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	StringWithLength *item = this->internalGetItem(index);
	RETURN_STRING_WITH_LENGTH(item->str, item->length, return_type);
}

template <class T>
int Collection<T>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return BaseCollection<T*>::getter(obj, name, result);
}


template class BaseCollection<NJSENumber>;
template class BaseCollection<const char*>;
#define INSTANTIATE_COLLECTION(element) template class BaseCollection<element*>; template class Collection<element>


#undef INSTANTIATE_COLLECTION
} /*namespace Lovefilm */
