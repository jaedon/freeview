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

namespace Ooif
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
#ifdef HBBTV_SECURITY
	virtual int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
#endif // HBBTV_SECURITY
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

#ifdef OIPF
class Application;
class ApplicationCollection: public BaseCollection<Application*>
{
public:
	virtual ~ApplicationCollection();
	virtual int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type);
	void addItem(Application* item);
};
#endif // OIPF

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
class Download;
typedef Collection<Download> DownloadCollection;
#endif // HBBTV_DOWNLOAD || OIPF

#if defined(HBBTV_PVR) || defined(OIPF)
class PVR;
typedef Collection<PVR> PVRCollection;
#endif // HBBTV_PVR || OIPF

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
class DRMControlInformation;
typedef Collection<DRMControlInformation> DRMControlInfoCollection;
#endif // HBBTV_DOWNLOAD || OIPF

#ifdef OIPF
class NetworkInterface;
typedef Collection<NetworkInterface> NetworkInterfaceCollection;

class AVOutput;
typedef Collection<AVOutput> AVOutputCollection;

class Tuner;
typedef Collection<Tuner> TunerCollection;
#endif // OIPF

class AVComponent;
typedef Collection<AVComponent> AVComponentCollection;

class Programme;
typedef Collection<Programme> ProgrammeCollection;

typedef Collection<StringWithLength> StringWithLengthCollection;
} /* namespace Ooif */

#endif // __COLLECTION
