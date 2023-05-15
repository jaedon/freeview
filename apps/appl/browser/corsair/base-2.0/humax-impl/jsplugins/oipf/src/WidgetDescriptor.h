// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#ifndef __WIDGET_DESCRIPTOR
#define __WIDGET_DESCRIPTOR

#include "NativeApplicationManager.h"

#include "ObjectInstance.h"
#include "WrapperObject.h"

namespace Ooif
{
class WidgetDescriptor : public ObjectInstance, public WrapperObject<WidgetDescriptor>
{
public:
	WidgetDescriptor(WidgetDescriptorHandle i);
	WidgetDescriptor(const WidgetDescriptor &original);
	~WidgetDescriptor();

	static void releaseNative(WidgetDescriptorHandle id);
	static const ClassType class_name = CLASS_WIDGETDESCRIPTOR;

protected:
	int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
};
} /* namespace Ooif */

#endif // __WIDGET_DESCRIPTOR

#endif // OIPF
