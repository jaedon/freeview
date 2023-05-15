/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_LICPARSERTEST_H_
#define _DRM_LICPARSERTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** LicParser Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** LicParser Non-APIMAP Functions
** ---------------------------------------
*/

/*
** ---------------------------------------
** LicParser APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL Test_MakeLicense(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetTestKey(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetTestKeyByName(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseGetAttribute(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetLicenseString(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseGetEvent(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseVerifySignature(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseVerifyCertChain(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseGetContentRevocation(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseVerifyContentRevocation(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_LicenseGetEnablingBits(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( LicParser )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_LICPARSERTEST_H_ */
