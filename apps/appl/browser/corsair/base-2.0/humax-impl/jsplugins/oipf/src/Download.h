// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#ifndef __DOWNLOAD
#define __DOWNLOAD

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"

#include "NativeDownload.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{
/**
  Wrapper class for Download objects in the native layer. (OIPF DAE 7.4.4)
*/

class Download : public ObjectInstance, public WrapperObject<Download>
{
private:
	void init();
	ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);
	DRMControlInfoCollection *getDRMControl(int count, DRMControlInformationHandle *handles);
	int returnStartTime(jsplugin_obj *obj, jsplugin_value *result);

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
	int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

public:
	Download(DownloadHandle i);
	Download(const Download &original);
	~Download();

	static void releaseNative(DownloadHandle id);
	static const ClassType class_name = CLASS_DOWNLOAD;
};
} /* namespace Ooif */

#endif // __DOWNLOAD
#endif // HBBTV_DOWNLOAD || OIPF
