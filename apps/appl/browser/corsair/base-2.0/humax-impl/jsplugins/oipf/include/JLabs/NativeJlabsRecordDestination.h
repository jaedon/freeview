/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSRECORDDESTINATION__
#define __NATIVE_JLABSRECORDDESTINATION__

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsRecordDestinationHandle;

OOIFReturnCode native_jlabsRecordDestinationId(jlabsRecordDestinationHandle identifier,  const char **retval );
OOIFReturnCode native_jlabsRecordDestinationName(jlabsRecordDestinationHandle identifier, const char **retval );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSCDSRECORDING__ */
