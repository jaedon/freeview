// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

 #if defined OIPF && defined JLABS


#include "jlabsBoardMessage.h"
#include "NativeJlabsBoardMessage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"


using namespace Ooif;

void jlabsBoardMessage::init()
{
	classType = CLASS_JLABS_BOARD_MESSAGE;
}

jlabsBoardMessage::jlabsBoardMessage(jlabsBoardMessageHandle i) : WrapperObject<jlabsBoardMessage>(i)
{
	ENTRY;
	init();
}

jlabsBoardMessage::jlabsBoardMessage(const jlabsBoardMessage &original) : WrapperObject<jlabsBoardMessage>(original.identifier)
{
	ENTRY;
	init();
}

jlabsBoardMessage::~jlabsBoardMessage()
{
	ENTRY;
}


int jlabsBoardMessage::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("msgBody", native_jlabsBoardMessageGetMsgBody(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("networkTypeName", native_jlabsBoardMessageGetNetworkTypeName(getIdentifier(),&RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("msgTitle", native_jlabsBoardMessageGetMsgTitle(getIdentifier(),&RETVAL), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsBoardMessage::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsBoardMessage::releaseNative(jlabsBoardMessageHandle id)
{
	NATIVE_WARN_NOK(native_jlabsBoardMessageReleaseHandle(id));
}

#endif // OIPF



