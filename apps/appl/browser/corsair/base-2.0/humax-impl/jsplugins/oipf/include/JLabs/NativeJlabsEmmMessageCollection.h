/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_EMMMESSAGECOLLECTION
#define __NATIVE_JLABS_EMMMESSAGECOLLECTION

#include "NativeOOIFTypes.h"
#include "NativeJlabsEmmMessage.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



OOIFReturnCode native_jlabsEmmMessageDelete(int msgId);
OOIFReturnCode native_jlabsEmmMessageReadDone(jlabsEmmMessageHandle id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_EMMMESSAGECOLLECTION */

#endif

