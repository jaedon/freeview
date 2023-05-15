/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_SCANNING
#define __NATIVE_JLABS_SCANNING

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* jlabsScanningHandle;

/* GETTER */

OOIFReturnCode native_jlabsScanningGetNetworkType(jlabsScanningHandle handle, const char **retval);
OOIFReturnCode native_jlabsScanningGetFavoriteID(jlabsScanningHandle handle, const char **retval);

OOIFReturnCode native_jlabsScanningSetFavoriteID(jlabsScanningHandle handle, const char *value);
OOIFReturnCode native_jlabsScanningReleaseHandle(jlabsScanningHandle id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_SCANNING */

#endif // #if defined OIPF && defined JLABS

