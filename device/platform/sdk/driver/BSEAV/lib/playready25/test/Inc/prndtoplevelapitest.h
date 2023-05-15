/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PRNDTOPLEVELAPITEST_H
#define __DRM_PRNDTOPLEVELAPITEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** Test PRND Top-level API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestPRNDTopLevelRegistration( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestMalformedPRNDTopLevelRegistration( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDTopLevelProximityDetection( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestMalformedPRNDTopLevelProximityDetection( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDTopLevelLicenseFetch( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestMalformedPRNDTopLevelLicenseFetch( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDTopLevelLicenseFetchChainedLicenseRebind( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDTopLevelInvalidPRNDLicenseBind( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(prndtoplevelapitest)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_PRNDTOPLEVELAPITEST_H */

