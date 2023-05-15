// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __WRAPPEROBJECT
#define __WRAPPEROBJECT

#include "jsplugin.h"
#include <map>

/**
   Super class for objects acting as wrapper for objects in the native
   platform.
*/

namespace Ooif
{
template<class T>
class WrapperObject
{
private:
	static std::map<void*,unsigned int> refs;
	void incRef();
	unsigned int decRef();
protected:
	void *identifier;
	bool isLastInstance();

public:
	WrapperObject(void *i);
	virtual ~WrapperObject();
	// Specific functions for this interface
	void *getIdentifier()
	{
		return identifier;
	};
};

template<class T>
WrapperObject<T>::~WrapperObject()
{
	if (decRef() == 0 )
	{
		refs.erase(identifier);
		T::releaseNative(identifier);
	}
}


template<class T>
WrapperObject<T>::WrapperObject(void *i) : identifier(i)
{
	incRef();
}

template<class T>
void WrapperObject<T>::incRef()
{
	refs[identifier]++;
}

template<class T>
unsigned int WrapperObject<T>::decRef()
{
	return --refs[identifier];
}

template<class T>
bool WrapperObject<T>::isLastInstance()
{
	return refs[identifier] == 1;
}

template<class T>
std::map<void*,unsigned int> WrapperObject<T>::refs;


template<class T, class DW>
class DataObject : public WrapperObject<T>
{
private:
	static std::map<void*, DW*> data_wrappers;
public:
	DW *data_wrapper;
	DataObject(void *i) : WrapperObject<T>(i)
	{
		typename std::map<void*, DW*>::iterator it;
		it = data_wrappers.find(i);
		if (it == data_wrappers.end())
		{
			data_wrapper = new DW();
			if(data_wrapper)
			{
				data_wrapper->init(i);
			}
			else
			{
				//Do nothing, and rely on accessors to check if data is NULL before trying to fetch its member variables.
			}
			data_wrappers[i]= data_wrapper;
		}
		else
		{
			data_wrapper = it->second;
		}
	}

	DataObject(const DataObject &src) : WrapperObject<T>(src.identifier)
	{
		data_wrapper = src.data_wrapper;
	}

	virtual ~DataObject()
	{
		if (WrapperObject<T>::isLastInstance())
		{
			delete data_wrapper;
			data_wrappers.erase(this->identifier);
		}
	}
};
template<class T, class D>
std::map<void*,D*> DataObject<T,D>::data_wrappers;
} /* namespace Ooif */
#endif // __WRAPPEROBJECT
