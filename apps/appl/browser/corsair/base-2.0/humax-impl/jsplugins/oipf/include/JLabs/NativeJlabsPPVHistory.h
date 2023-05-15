/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_PPVHISTORY
#define __NATIVE_JLABS_PPVHISTORY

#include "oplppvhistory.h"
#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* jlabsPPVHistoryHandle;

/*** B.3.4.1.7. jlabsPPVHistory Å¬·¡½º ***/

/* GETTER */

/* ... */
OOIFReturnCode native_jlabsPPVHistroyGetFee(jlabsPPVHistoryHandle identifier, const char **retval);

/* ... */
OOIFReturnCode native_jlabsPPVHistroyGetEventData(jlabsPPVHistoryHandle identifier, const char **retval);

/* ... */
OOIFReturnCode native_jlabsPPVHistroyGetDate(jlabsPPVHistoryHandle identifier, OOIFNumber *retval);

/*** read only ***/
/* ... */
OOIFReturnCode native_jlabsPPVHistroyGetCaSystemId(jlabsPPVHistoryHandle identifier, const char **retval);

/* SETTER */

/* ... */
OOIFReturnCode native_jlabsPPVHistroySetFee(jlabsPPVHistoryHandle identifier, const char *value);

/* ... */
OOIFReturnCode native_jlabsPPVHistroySetEventData(jlabsPPVHistoryHandle identifier, const char *value);

/* ... */
OOIFReturnCode native_jlabsPPVHistroySetDate(jlabsPPVHistoryHandle identifier, OOIFNumber value);

/* ... */
OOIFReturnCode native_jlabsPPVHistoryGet(const char *name, const char **retval);

/* ... */
OOIFReturnCode native_jlabsPPVHistorySet(const char *name, const char *value);

/* ... */
OOIFReturnCode native_jlabsPPVHistroyReleaseHandle(jlabsPPVHistoryHandle id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_PPVHISTORY */

#endif // #if defined OIPF && defined JLABS
