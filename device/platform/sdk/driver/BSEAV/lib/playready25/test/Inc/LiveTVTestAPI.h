/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_LIVETVTEST_TEST_API_H
#define __DRM_LIVETVTEST_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE
/*
** ---------------------------------------
** DerivedKey Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestStartLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL TestFinishLicense( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestUnpackLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL TestVerifyLicense( long argc, __in_ecount( argc )char **argv );
DRM_RESULT DRM_CALL TestAddAuxKeyObject( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestAddUplinkXObject( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestBBXCalculateKeysDerived( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestVerifyStoreSlotCount( __in DRM_LONG argc, __in_ecount( argc ) DRM_CHAR** argv );


DECLARE_MODULE_DEFS( LiveTV )

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_LIVETVTEST_TEST_API_H */
