/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __TESTNDR_H
#define __TESTNDR_H

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL Test_SetSerialNumber(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetCertificate(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetPrivateKey(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackRegistrationMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetUnpackRegistrationResponseMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackProximityStartMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetUnpackProximityChallengeMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackProximityResponseMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetUnpackProximityResultMessage(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_DrmXmrUnpackRootLicense(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_DrmXmrUnpackLeafLicense(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_AddRevocationListToResponse(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_InitializeRevocationListResponse(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListResponseMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListRequestMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_VerifyRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_DumpRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_WmdrmNetUnpackRevocationListRequestMessage( long argc, __in_ecount(argc) char *argv[] );

DECLARE_MODULE_DEFS(NDRFunctionalTest)

EXIT_PKTEST_NAMESPACE

#endif /*__TESTNDR_H*/