// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#ifndef __DOWNLOADTRIGGER
#define __DOWNLOADTRIGGER

#include "jsplugin.h"
#include "ObjectInstance.h"

namespace Ooif
{
class DownloadTrigger : public ObjectInstance
{
protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);

public:
	// Specific functions for this interface
	static int registerDownloadURL(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int registerDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#ifdef OIPF
	static int registerDownloadFromCRID(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	static int checkDownloadPossible(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
#endif // OIPF
};
} /* namespace Ooif */

#endif // __DOWNLOADTRIGGER
#endif // HBBTV_DOWNLOAD || OIPF
