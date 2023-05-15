// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __WIDGET_DESCRIPTOR_COLLECTION
#define __WIDGET_DESCRIPTOR_COLLECTION

#include "WidgetDescriptor.h"
#include "jsplugin.h"
#include "macros.h"
#include "Collection.h"

namespace Ooif
{
class WidgetDescriptorCollection : public Collection<WidgetDescriptor>
{
public:
	WidgetDescriptorCollection();
	~WidgetDescriptorCollection();
	static const ClassType class_name = CLASS_WIDGETDESCRIPTORCOLLECTION;
};
} /* namespace Ooif */

#endif // __WIDGET_DESCRIPTOR_COLLECTION
#endif // OIPF

