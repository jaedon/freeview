/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __LICACQV3TESTAPI_H
#define __LICACQV3TESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL _UseNullAppContext                                      ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL _AddDomainCert                                          ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL _SetRights                                              ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL _SetHeader                                              ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL _AddLicenseAcknowledgement                              ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL _SetTransactionID                                       ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_GenerateChallenge                    ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_GenerateAck                          ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse                      ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse_ServerErrorChecking  ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse_LicResult            ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessAckResponse                   ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyBasicFormat_GenerateLicChallenge                  ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyDeviceCertificate_GenerateLicChallenge            ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyContentHeader_GenerateLicChallenge                ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyDomainCertificateCollections_GenerateLicChallenge ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyTransactionID_GenerateAck                         ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyLicenseStorageResults_GenerateAck                 ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyDeviceCertificate_GenerateAck                     ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyBasicFormat_GenerateAck                           ( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( LicAcqV3 )

EXIT_PKTEST_NAMESPACE

#endif /* __LICACQV3TESTAPI_H */
