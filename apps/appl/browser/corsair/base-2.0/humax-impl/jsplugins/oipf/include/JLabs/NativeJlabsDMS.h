/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
//#if defined OIPF && defined JLABS
#if defined OIPF 

#ifndef __NATIVE_JLABS_DMS
#define __NATIVE_JLABS_DMS

#include "NativeOOIFTypes.h"
#include "NativeJlabsCDSRecording.h"
#include "NativeJlabsContentSearch.h"
#include "NativeJlabsRecordDestination.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Properties */
typedef void *jlabsDMSDeviceHandle;
OOIFReturnCode native_jlabsDMSDeviceGetDeviceHandle( jlabsDMSDeviceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsDMSDeviceGetUDN( jlabsDMSDeviceHandle identifier, const char **retval );
OOIFReturnCode native_jlabsDMSDeviceGetFriendlyName( jlabsDMSDeviceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsDMSDeviceGetIpAddress( jlabsDMSDeviceHandle identifier, const char **retval);

OOIFReturnCode native_jlabsDMSDeviceMACAddress( jlabsDMSDeviceHandle identifier, const char **retval );
OOIFReturnCode native_jlabsDMSDeviceGetTotalStorageSize( jlabsDMSDeviceHandle identifier, const char **retval );

OOIFReturnCode native_jlabsDMSDeviceGetSupportedSortCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval);
OOIFReturnCode native_jlabsDMSDeviceGetSupportedSearchCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval);
OOIFReturnCode native_jlabsDMSDeviceGetSupportedDlnaCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval);
OOIFReturnCode native_jlabsDMSDeviceGetSupportedJlabsCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval);
OOIFReturnCode native_jlabsDMSDeviceContentSearch(jlabsDMSDeviceHandle identifier, jlabsContentSearchHandle *retval);
OOIFReturnCode native_jlabsDMSDeviceGetRecordDestinations(jlabsDMSDeviceHandle identifier, OOIFBoolean *retval );

/* Methods */
OOIFReturnCode native_jlabsDMSDeviceReleaseHandle( jlabsDMSDeviceHandle id );
OOIFReturnCode native_jlabsDMSDeviceBrowseDirectChildren( jlabsDMSDeviceHandle identifier, char *cid, char *sort, int index, int request, OOIFBoolean *retval );
OOIFReturnCode native_jlabsDMSgetAvailableStorageSize( jlabsDMSDeviceHandle identifier, char *dst, OOIFBoolean *retval);

OOIFReturnCode native_jlabsDMSDeviceRegisterEventHandler( void (cbFunc)(int event, int size, void **data) );
OOIFReturnCode native_jlabsDMSDeviceUnRegisterEventHandler( void );

OOIFReturnCode _native_jlabsDMSgetAvailableStorageSize( void *pItem, double *pTotal, double *pAvailabe, char **dst );
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 

#endif /* __NATIVE_JLABS_CDSSEARCHER */



