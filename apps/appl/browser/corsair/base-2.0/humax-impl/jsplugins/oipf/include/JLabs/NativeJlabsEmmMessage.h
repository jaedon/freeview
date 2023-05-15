/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABS_EMMMESSAGE
#define __NATIVE_JLABS_EMMMESSAGE

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef void* jlabsEmmMessageHandle;

/*@@@shlee:temp */
typedef enum{
	EMM_MESSAGE_TYPE_EMM = 0,
	EMM_MESSAGE_TYPE_INTERNAL
}EmmMessageType_t;

typedef struct{
	int msgId;
	EmmMessageType_t msgType;
	char networkTypeName[10];
	char msgDate[20];
	char msgBody[30];
	bool bReadDone;
}jlabsEmmMessage_t;




OOIFReturnCode native_jlabsEmmMessageGetMsgId(jlabsEmmMessageHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsEmmMessageGetMsgType(jlabsEmmMessageHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsEmmMessageGetNetworkTypeName(jlabsEmmMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsEmmMessageGetDate(jlabsEmmMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsEmmMessageGetMsgBody(jlabsEmmMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsEmmMessageGetReadDoneFlag(jlabsEmmMessageHandle id, OOIFBoolean *retval);
OOIFReturnCode native_jlabsEmmMessageGetMsgTitle(jlabsEmmMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsEmmMessageReleaseHandle(jlabsEmmMessageHandle id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_EMMMESSAGE */

