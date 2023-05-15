/**************************************************************************************/
/**
 * @file jlabsEmmMessage.cpp
 *
 * jlabsEmmMessage module
 *
 * @author  sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/03/16
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/


 #if defined OIPF && defined JLABS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsEmmMessage.h"
#include "NativeJlabsEmmMessage.h"

using namespace Ooif;

void jlabsEmmMessage::init()
{
	classType = CLASS_JLABS_EMM_MESSAGE;
}

jlabsEmmMessage::jlabsEmmMessage(jlabsEmmMessageHandle i) : WrapperObject<jlabsEmmMessage>(i)
{
	ENTRY;
	init();
}

jlabsEmmMessage::jlabsEmmMessage(const jlabsEmmMessage &original) : WrapperObject<jlabsEmmMessage>(original.identifier)
{
	ENTRY;
	init();
}

jlabsEmmMessage::~jlabsEmmMessage()
{
	ENTRY;
}


int jlabsEmmMessage::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_NUMBER("msgId", native_jlabsEmmMessageGetMsgId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("msgType", native_jlabsEmmMessageGetMsgType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("networkTypeName", native_jlabsEmmMessageGetNetworkTypeName(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("date", native_jlabsEmmMessageGetDate(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_STRING("msgBody", native_jlabsEmmMessageGetMsgBody(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_BOOLEAN("readDoneFlag", native_jlabsEmmMessageGetReadDoneFlag(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("msgTitle", native_jlabsEmmMessageGetMsgTitle(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsEmmMessage::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

void jlabsEmmMessage::releaseNative(jlabsEmmMessageHandle id)
{
	NATIVE_WARN_NOK(native_jlabsEmmMessageReleaseHandle(id));
}

#endif // OIPF


