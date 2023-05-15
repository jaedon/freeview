// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __JLABS_WIFI_ACCESSPOINT_H__
#define __JLABS_WIFI_ACCESSPOINT_H__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "NativeJlabsWiFiAccessPoint.h"

namespace Ooif
{

	/* jlabsWiFiAccessPoint Class */
	class jlabsWiFiAccessPoint : public ObjectInstance , public WrapperObject<jlabsWiFiAccessPoint>
	{
	public:
		jlabsWiFiAccessPoint( jlabsWiFiAccessPointHandle id );
		jlabsWiFiAccessPoint( const jlabsWiFiAccessPoint &original);
		virtual ~jlabsWiFiAccessPoint();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative( jlabsWiFiAccessPointHandle id );
		static const ClassType class_name = CLASS_JLABS_NETWORK_ACCESSPOINT;
	};
} /* namespace Ooif */

#endif // __JLABS_WIFI_ACCESSPOINT_H__
#endif // OIPF
