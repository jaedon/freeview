/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_BOARDMESSAGE
#define __NATIVE_JLABS_BOARDMESSAGE

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef void* jlabsBoardMessageHandle;

/*@@@shlee:temp */
typedef struct{
	char networkTypeName[10];
	char msgBody[30];

}jlabsBoardMessage_t;



OOIFReturnCode native_jlabsBoardMessageGetMsgBody(jlabsBoardMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsBoardMessageGetNetworkTypeName(jlabsBoardMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsBoardMessageGetMsgTitle(jlabsBoardMessageHandle id, const char **retval);
OOIFReturnCode native_jlabsBoardMessageReleaseHandle(jlabsBoardMessageHandle id);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_BOARDMESSAGE */

#endif

