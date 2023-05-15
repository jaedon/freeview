/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DCPARSERTESTAPI_H
#define __DCPARSERTESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

extern DRM_CONST_STRING g_dstrDevCert;


DRM_RESULT DRM_CALL TEST_API_DevCertGetAttribute(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TEST_API_DevCertVerifyDeviceCert(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TEST_ChangeDevcertNode(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TEST_RollForwardClockInYears(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( DCParser )

EXIT_PKTEST_NAMESPACE

#endif /* __DCPARSERTESTAPI_H */
