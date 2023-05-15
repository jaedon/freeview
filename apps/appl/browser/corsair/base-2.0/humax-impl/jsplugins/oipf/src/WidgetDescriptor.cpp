// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#include <string.h>

#include "WidgetDescriptor.h"
#include "NativeApplicationManager.h"
#include "macros.h"

using namespace Ooif;

WidgetDescriptor::~WidgetDescriptor()
{
	ENTRY;
}

WidgetDescriptor::WidgetDescriptor(WidgetDescriptorHandle i) : WrapperObject<WidgetDescriptor>(i)
{
	ENTRY;
	classType = CLASS_WIDGETDESCRIPTOR;
}

WidgetDescriptor::WidgetDescriptor(const WidgetDescriptor& original) : WrapperObject<WidgetDescriptor>(original.identifier)
{
	ENTRY;
	classType = CLASS_WIDGETDESCRIPTOR;
}

int WidgetDescriptor::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	WidgetDescriptorHandle h = identifier;

	NATIVE_GET_STRING("localURI", native_WidgetDescriptorGetString(WIDGET_LOCAL_URL, h, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("downloadURI", native_WidgetDescriptorGetString(WIDGET_DOWNLOAD_URL, h, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING_COLLECTION("defaultIcon", obj, native_WidgetDescriptorGetStringVector(WIDGET_DEFAULT_ICON, h, &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING_COLLECTION("customIcons", obj, native_WidgetDescriptorGetStringVector(WIDGET_CUSTOM_ICONS, h, &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE_CACHE);

	NATIVE_GET_BOOLEAN("running", native_WidgetDescriptorGetRunning(identifier, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_STRING("author", native_WidgetDescriptorGetString(WIDGET_AUTHOR, h, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_WidgetDescriptorGetString(WIDGET_DESCRIPTION, h, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_WidgetDescriptorGetString(WIDGET_NAME, h, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("shortName", native_WidgetDescriptorGetString(WIDGET_SHORT_NAME, h, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("version", native_WidgetDescriptorGetString(WIDGET_VERSION, h, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("id", native_WidgetDescriptorGetString(WIDGET_ID, h, &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("authorEmail", native_WidgetDescriptorGetString(WIDGET_AUTHOR_EMAIL, h, &RETVAL), JSP_GET_VALUE); // ID never changes, rest can change on widget update
	NATIVE_GET_STRING("authorHref", native_WidgetDescriptorGetString(WIDGET_AUTHOR_HREF, h, &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_NUMBER("width", native_WidgetDescriptorGetNumber(WIDGET_WIDTH, h, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("height", native_WidgetDescriptorGetNumber(WIDGET_HEIGHT, h, &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

//static
void WidgetDescriptor::releaseNative(WidgetDescriptorHandle id)
{
	NATIVE_WARN_NOK(native_WidgetDescriptorReleaseHandle(id));
}
#endif // OIPF
