/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_METERING_TEST_API_H
#define __DRM_METERING_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#include <drmmanager.h>

/*
** ---------------------------------------
** Metering Test Global Variables
** ---------------------------------------
*/
extern DRM_APP_CONTEXT *m_pContextManager;
extern const DRM_CHAR  *m_pLogFile;

/*
** ---------------------------------------
** Metering Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestDRM_VerifyTID( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_AddManyMeteringData( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_AddMeteringData( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_GeneratePartialMeterChallenge( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_GeneratePartialMeterChallenge( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_GenerateMeterChallenge( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_ProcessMeterResponse( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_CompareMeterCert( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_MTR_GenerateMeterCertChallenge( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_MTR_ProcessMeterCertResponse( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDRM_Manager_Initialize( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL RestoreDeviceStore( long argc, __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS(Metering)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_METERING_TEST_API_H */
