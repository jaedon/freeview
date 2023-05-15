// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF && defined JLABS
#ifndef __JLABS_SCANNING
#define __JLABS_SCANNING

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "NativeJlabsScanning.h"
#include "WrapperObject.h"

namespace Ooif
{
	class jlabsScanning	 : public ObjectInstance, public WrapperObject<jlabsScanning>
	{
	public:
		int	idType;
		
		jlabsScanning(int id_type);	/* createChannelScanParametersObject() argument */		
		jlabsScanning(jlabsScanningHandle i);
		jlabsScanning(const jlabsScanning &original);
		~jlabsScanning();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static void releaseNative(jlabsScanningHandle id);

		//// Method
		static const ClassType class_name = CLASS_JLABS_SCANNING;
	};
} /* namespace Ooif */

#endif // __JLABS_SCANNING
#endif // defined OIPF && defined JLABS
