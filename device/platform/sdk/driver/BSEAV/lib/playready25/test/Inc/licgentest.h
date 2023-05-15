/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_LICGENTEST_H
#define __DRM_LICGENTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** Test License Generation Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestLicGenCreateLicense( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicGenCreateWDRMHeaderAndPRO( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicGenCreatePROAndDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicGenTopLevel( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicGenRootLeafLicense( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicGenCreateLicenseWithRemoteCert( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicGenInvalidDescriptor( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(licgentest)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_LICGENTEST_H */

