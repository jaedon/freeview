/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
//#if defined OIPF && defined JLABS
#if defined OIPF 

#ifndef __NATIVE_JLABS_CONTENTRESULTS
#define __NATIVE_JLABS_CONTENTRESULTS

#include "NativeOOIFTypes.h"
#include "NativeJlabsCDSRecording.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsContentResultsHandle;
OOIFReturnCode native_jlabsContentResultsGetLength( jlabsContentResultsHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_jlabsContentResultsGetOffset( jlabsContentResultsHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_jlabsContentResultsGetTotalSize( jlabsContentResultsHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_jlabsContentResultsGetResults(jlabsContentResultsHandle identifier, OOIFNumber offset, OOIFNumber count, OOIFBoolean *retval);

OOIFReturnCode native_jlabsContentResultsAbort(jlabsContentResultsHandle identifier);
OOIFReturnCode native_jlabsContentResultsUpdate(jlabsContentResultsHandle identifier);
OOIFReturnCode native_jlabsContentResultsItem(jlabsContentResultsHandle identifier, OOIFNumber offset, void **result);
OOIFReturnCode native_jlabsContentResultsReleaseHandle(jlabsContentResultsHandle id);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 

#endif /* __NATIVE_JLABS_CONTENTSRESULT */



