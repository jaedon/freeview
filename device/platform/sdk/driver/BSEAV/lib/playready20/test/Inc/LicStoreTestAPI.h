/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_LICSTORETEST_H_
#define _DRM_LICSTORETEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** LicStore Test Global Variable Names
** ---------------------------------------
*/
#define DST_EMBEDDED_VAR             "SharedEmbeddedDST"
#define DST_EMBEDDED_CONTEXT_VAR     "SharedEmbeddedDSTContext"
#define DST_EMBEDDED_STORE_DATA_VAR  "SharedEmbeddedStoreData"

/* Max size of the default embedded store data buffer. */
#define MAX_EST_SIZE    ( 30 * 1024 )

/*
** ---------------------------------------
** LicStore Non-APIMAP Functions 
** ---------------------------------------
*/

/*
** ---------------------------------------
** LicStore APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL Test_DRM_LST_Open(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_Close(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_AddLicense(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_GetLicense(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_GetLicenseKeys(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_EnumDelete(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestLicStoreReinitialize( long iIndex, __in_opt char **argv );
DRM_RESULT DRM_CALL TestLicStoreAddMultiLicenses(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_InitEnum(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_InitReverseEnum(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_EnumNext(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestLicStoreLog(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_DeleteLicense(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_InitEnumMultiple(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_EnumNextMultiple(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_DRM_LST_EnumDeleteMultiple(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestLicStoreSetEmbeddedStore(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( LicStore )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_LICSTORETEST_H_ */
