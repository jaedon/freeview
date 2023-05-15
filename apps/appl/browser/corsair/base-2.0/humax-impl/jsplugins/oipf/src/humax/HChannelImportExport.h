// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __H_CHANNEL_IMPORT_EXPORT
#define __H_CHANNEL_IMPORT_EXPORT

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeHConfiguration.h"

#include "opllocalsystem.h"


#define ____HMX_ChannelImportExport_functions____


typedef void (*hChannelImportExportEventListener)(HUINT32 type, HUINT32 result);

OOIFReturnCode native_HChannelImportExport_startChannelExport(hChannelImportExportEventListener listener);
OOIFReturnCode native_HChannelImportExport_startChannelImport(hChannelImportExportEventListener listener);
OOIFReturnCode native_HChannelImportExportSetListener(hChannelImportExportEventListener listener);

void native_HChannelImportExportEventListener(HUINT32 type, HUINT32 result);


namespace Ooif
{
	class HChannelImportExport : public EventTarget
	{
	private:
		static list<HChannelImportExport*> 	instances; // for hadling events.


	public:
		HChannelImportExport();
		~HChannelImportExport();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int startChannelExport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int startChannelImport(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);		

		static void onChannelImportExportEvent(HUINT32 type, HUINT32 result);

		static const ClassType class_name = CLASS_CHANNEL_IMPORT_EXPORT;

	};
} /* namespace Ooif */

#endif // __H_CHANNEL_IMPORT_EXPORT
#endif // OIPF


