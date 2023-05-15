// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __COLLECTION
#define __COLLECTION

#include "jsplugin.h"

#include <vector>

#include "ObjectInstance.h"

using namespace std;

namespace Lovefilm
{
template<class T>
class BaseCollection: public ObjectInstance
{
protected:
	vector<T> items;
	void internalInsertAt(T item, int index);
	void internalRemoveIndex(int index);
	void internalRemoveItem(T item);

public:
	virtual ~BaseCollection() {}
	T internalGetItem(int index);
	int getSize();
	virtual void addItem(T item);
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type) = 0;
	static int item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
};

template <class T>
class Collection : public BaseCollection<T*>
{
public:
	virtual ~Collection();
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	virtual int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type);
    virtual void internalRemoveIndex(int index);    
};

class NumberCollection: public BaseCollection<double>
{
public:
	virtual ~NumberCollection() {}
	virtual int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type);
};

class StringCollection: public BaseCollection<const char*>
{
public:
	virtual ~StringCollection();
	virtual int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type);
};

class StringWithLength {
private:
	StringWithLength(const StringWithLength &original);
	StringWithLength& operator=(const StringWithLength&);
public:
	StringWithLength(char* string, int string_len = -1);
	~StringWithLength();

	char* str;
	int length;
};

class StreamTypes;
typedef Collection<StreamTypes> StreamTypesCollection;
} /* namespace Lovefilm */

#endif // __COLLECTION
