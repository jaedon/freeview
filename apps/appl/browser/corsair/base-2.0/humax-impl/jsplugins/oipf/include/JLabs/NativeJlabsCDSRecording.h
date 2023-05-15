/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSCDSRECORDING__
#define __NATIVE_JLABSCDSRECORDING__

#include "NativeOOIFTypes.h"
#include "NativeBookmark.h"
#include "NativeParentalControlManager.h"
#include "NativeJlabsCDSMetadata.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsCDSRecordingHandle;

OOIFReturnCode native_jlabsCDSRecordingGetObjectID( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetClass( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetRestricted( jlabsCDSRecordingHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsCDSRecordingGetObjectType( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetImportURI( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetThumbnailURI( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetTitle( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetDate( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetDuration( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetResolution( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetDigitalCopyControl( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetChildCount( jlabsCDSRecordingHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_jlabsCDSRecordingGetGenre( jlabsCDSMetaHandle identifier, int *count, char ***retval);
OOIFReturnCode native_jlabsCDSRecordingGetChannelName( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetChannelNumber( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetRadioBand( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetAnalogCopyControl( jlabsCDSMetaHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetParentalRatings(jlabsCDSRecordingHandle identifier, int *count, ParentalRatingHandle **handles);
OOIFReturnCode native_jlabsCDSRecordingGetBookmarks(jlabsCDSRecordingHandle identifier, int *count, BookmarkHandle **handles);
OOIFReturnCode native_jlabsCDSRecordingGetMediaProfile( jlabsCDSRecordingHandle identifier, const char **retval );
OOIFReturnCode native_jlabsCDSRecordingGetObjectParentId( jlabsCDSRecordingHandle identifier, const char **retval);

jlabsCDSMetaHandle native_jlabsCDSRecordingGetMetaData( jlabsCDSRecordingHandle identifier );

OOIFReturnCode native_jlabsCDSRecordingFlagUseMetaData( jlabsCDSMetaHandle identifier );
OOIFReturnCode native_jlabsCDSRecordingReleaseHandle( jlabsCDSRecordingHandle id );
OOIFReturnCode native_jlabsCDSRecordingGetTotalMatchs( jlabsCDSRecordingHandle identifier, int *cnt );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSCDSRECORDING__ */
