/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABSEMMAUTODISPLAYMESSAGE__
#define __NATIVE_JLABSEMMAUTODISPLAYMESSAGE__

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef void* jlabsEmmAutoDisplayMessageHandle;
OOIFReturnCode native_jlabsEmmAutoDisplayMessageGetId(jlabsEmmAutoDisplayMessageHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsEmmAutoDisplayMessageGetAutoDisplayType(jlabsEmmAutoDisplayMessageHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsEmmAutoDisplayMessageReleaseHandle(jlabsEmmAutoDisplayMessageHandle id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSEMMAUTODISPLAYMESSAGE__ */
#endif /*OIPF JLABS*/

