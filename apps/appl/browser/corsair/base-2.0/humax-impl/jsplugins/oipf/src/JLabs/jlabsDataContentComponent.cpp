// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

 #if defined OIPF && defined JLABS

 #include "jlabsDataContentComponent.h"
#include "NativeJlabsDataContentComponent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"

using namespace Ooif;


void jlabsDataContentComponent::init()
{
	ENTRY;
	classType = CLASS_JLABS_DATA_CONTENT_COMPONENT;
}

jlabsDataContentComponent::jlabsDataContentComponent(AVComponentHandle i, ClassType owner_class_type)
 : AVComponent(i, owner_class_type)
{
	ENTRY;
	init();
}

jlabsDataContentComponent::jlabsDataContentComponent(const jlabsDataContentComponent &original)
 : AVComponent(original)

{
	ENTRY;
	init();
}


int jlabsDataContentComponent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	/* OIPF */
	DATA_GET_NUMBER(componentTag);
  	DATA_GET_NUMBER(pid);
  	DATA_GET_NUMBER(type);
	DATA_GET_STRING(encoding);
	DATA_GET_BOOLEAN(encrypted);

	DATA_GET_BOOLEAN(associatedContentFlag);
	DATA_GET_BOOLEAN(dataEntryComponentFlag);

	DATA_GET_NUMBER(digitalCopyControl);
	GET_STRING("dataText", data_wrapper->data.text, JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING_COLLECTION("multiview", obj,
								 native_jlabsDataComponentGetMultiView(&data_wrapper->data, &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int jlabsDataContentComponent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsDataContentComponent::releaseNative(AVComponentHandle id)
{
	NATIVE_WARN_NOK(native_jlabsDataComponentReleaseHandle(id));
}

#endif
