/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * @file NativeHmxMediaScan.h
 *
 * Copyright (C) 2011 HUMAX. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_HMX_DLNA__
#define __NATIVE_HMX_DLNA__

#include "NativeOOIFTypes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined OIPF

/*****************************************************************************
 * HMX_DMSSearcher
 *****************************************************************************
*/
typedef void *DMSSearcherHandle;
typedef void *DMSDeviceHandle;
/* Methods */
OOIFReturnCode native_DMPAction( int actionType, OOIFNumber *retval );
OOIFReturnCode native_DMSSearchGetAvailableDMSList( OOIFNumber *retval );
OOIFReturnCode native_DMSSearchGetCurrentDMS( DMSDeviceHandle *identifier );


OOIFReturnCode native_DMSSearchRegisterEventHandler(void  (cbFunc)(int event, int size, void *data) );
OOIFReturnCode native_DMSSearchUnRegisterEventHandler(void);

/*****************************************************************************
 * HMX_DMS
 *****************************************************************************
*/
/* Properties */
OOIFReturnCode native_DMSDeviceGetUDN( DMSDeviceHandle identifier, const char **retval );
OOIFReturnCode native_DMSDeviceGetFriendlyName( DMSDeviceHandle identifier, const char **retval);
OOIFReturnCode native_DMSDeviceGetIpAddress( DMSDeviceHandle identifier, const char **retval);

/* Methods */
OOIFReturnCode native_DMSDeviceReleaseHandle( DMSDeviceHandle id );
OOIFReturnCode native_DMSDeviceBrowseDirectChildren( DMSDeviceHandle identifier, char *cid, int pageNum, OOIFNumber *retval );
OOIFReturnCode native_DMSDeviceRegisterEventHandler( void (cbFunc)(int event, char *udn, int reqId, int size, void **data, int total) );
OOIFReturnCode native_DMSDeviceUnRegisterEventHandler( void );

/*****************************************************************************
 * HMX_DLNAContent
 *****************************************************************************
*/
typedef void *DLNAContentHandle;
typedef void *DLNAContentMetaHandle;
OOIFReturnCode native_DLNAContentGetObjectID( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetParentID( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetClass( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetRestricted( DLNAContentHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_DLNAContentGetObjectType( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetImportURI( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetThumbnailURI( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetTitle( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetDate( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetDuration( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetResolution( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetDigitalCopyControl( DLNAContentHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentGetChildCount( DLNAContentHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_DLNAContentGetGenre( DLNAContentMetaHandle identifier, const char **retval );
DLNAContentMetaHandle  native_DLNAContentGetMetaData( DLNAContentHandle identifier );
/* Methods */
OOIFReturnCode native_DLNAContentReleaseHandle( DLNAContentHandle id );

/*****************************************************************************
 * HMX_DLNAContentMetaData
 *****************************************************************************
*/
/* Properties */
typedef void *DLNAContentMetaHandle;
OOIFReturnCode native_DLNAContentMetaGetAlbum( DLNAContentMetaHandle identifier, const char **retval );
OOIFReturnCode native_DLNAContentMetaGetArtist( DLNAContentMetaHandle identifier, const char **retval );


#endif /* OIPF */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_HMX_DLNA__ */
