/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSCDSMETADATA__
#define __NATIVE_JLABSCDSMETADATA__

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Properties */
typedef void *jlabsCDSMetaHandle;

OOIFReturnCode native_jlabsCDSMetaGetAlbum( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSMetaGetArtist( jlabsCDSMetaHandle identifier, const char **retval );

OOIFReturnCode native_jlabsCDSMetaGetDescription( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSMetaGetLongDescription( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSMetaGetAuthor( jlabsCDSMetaHandle identifier, const char **retval );

/* Methods */
OOIFReturnCode native_jlabsMetaReleaseHandle( jlabsCDSMetaHandle id );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_jlabsCDSMetadata__ */
