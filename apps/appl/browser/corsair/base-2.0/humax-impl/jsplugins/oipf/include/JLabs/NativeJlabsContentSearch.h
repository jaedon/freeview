/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSCONTENTSEARCH__
#define __NATIVE_JLABSCONTENTSEARCH__

#include "NativeOOIFTypes.h"
#include "NativeJlabsContentResults.h"
#include "Query.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *QueryHandle;
typedef void *jlabsContentSearchHandle;
typedef void *jlabsContentResultsHandle;


OOIFReturnCode native_jlabsContentSearchGetQuery(jlabsContentSearchHandle identifier, QueryHandle *retval);
OOIFReturnCode native_jlabsContentSearchCreateQuery(const char *field, OOIFNumber comparison, const char *value, QueryHandle *retval);
OOIFReturnCode native_jlabsContentSearchOrderBy(jlabsContentSearchHandle identifier, const char *field, OOIFBoolean ascending);

OOIFReturnCode native_jlabsContentSearchGetResult(jlabsContentSearchHandle identifier, jlabsContentResultsHandle *retval);
OOIFReturnCode native_jlabsContentSearchSetQuery(jlabsContentSearchHandle identifier, QueryHandle *query);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSCONTENTSEARCH__ */

