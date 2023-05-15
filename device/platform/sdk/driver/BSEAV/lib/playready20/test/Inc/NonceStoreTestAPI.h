/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __NONCESTORETESTAPI_H
#define __NONCESTORETESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** Shared Variable Names
*/  
#define NST_VAR                   "SharedNST"
#define LICSTORE_CONTEXT_VAR      "SharedLicStoreContext"
#define NST_PBSTOREDATA_VAR       "SharedNSTPBStoreData"
 
DRM_RESULT DRM_CALL TestNonceStoreOpen                  ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestNonceStoreClose                 ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestNonceStoreGenerateNonce         ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestNonceStoreAddLicense            ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestNonceStoreVerify                ( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( NonceStore )

EXIT_PKTEST_NAMESPACE

#endif /* __NONCESTORETESTAPI_H */
