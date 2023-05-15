// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined OIPF && defined JLABS

#include "jlabsEmmAutoDisplayMessage.h"
#include "NativeJlabsEmmAutoDisplayMessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"

using namespace Ooif;

void jlabsEmmAutoDisplayMessage::init()
{
	ENTRY;
	classType = CLASS_JLABS_EMM_AUTO_DISPLAY_MESSAGE;
}

jlabsEmmAutoDisplayMessage::jlabsEmmAutoDisplayMessage(jlabsEmmAutoDisplayMessageHandle i) : WrapperObject<jlabsEmmAutoDisplayMessage>(i)
{
	ENTRY;
	init();
}

jlabsEmmAutoDisplayMessage::jlabsEmmAutoDisplayMessage(const jlabsEmmAutoDisplayMessage &original) : WrapperObject<jlabsEmmAutoDisplayMessage>(original.identifier)
{
	ENTRY;
	init();
}

int jlabsEmmAutoDisplayMessage::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	OOIF_LOG_DEBUG("=> Entering %s::%s", __FILE__, __FUNCTION__);

	NATIVE_GET_NUMBER("id", native_jlabsEmmAutoDisplayMessageGetId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("autoDisplayType", native_jlabsEmmAutoDisplayMessageGetAutoDisplayType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	return JSP_GET_NOTFOUND;
}

int jlabsEmmAutoDisplayMessage::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsEmmAutoDisplayMessage::releaseNative(jlabsEmmAutoDisplayMessageHandle id)
{
	NATIVE_WARN_NOK(native_jlabsEmmAutoDisplayMessageReleaseHandle(id));
}

#endif
