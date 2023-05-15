// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include "WidgetDescriptorCollection.h"
#include "NativeApplicationManager.h"

using namespace Ooif;

WidgetDescriptorCollection::WidgetDescriptorCollection()
{
	ENTRY;
	classType = CLASS_WIDGETDESCRIPTORCOLLECTION;
	int count = 0;
	WidgetDescriptorHandle *handles = NULL;
	NATIVE_WARN_NOK(native_AppManGetWidgets(&count, &handles));
	for (int i=0; i<count; i++)
	{
		addItem(new WidgetDescriptor(handles[i]));
	}
	HLIB_STD_MemFree(handles);
}

WidgetDescriptorCollection::~WidgetDescriptorCollection()
{
	ENTRY;
}

#endif // OIPF
