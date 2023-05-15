/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_METERCERT_TEST_API_H
#define __DRM_METERCERT_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#include <drmmanager.h>

/*
** ---------------------------------------
** MeterCertStore Test Constants
** ---------------------------------------
*/
#define METERCERT_BUFFER_SIZE 5000
#define DRM_MAX_TEST_METERCERT_SIZE 10240

/*
** ---------------------------------------
** MeterCertStore Test Global Variable Names
** ---------------------------------------
*/
#ifndef METER_CERT_ENUM_CONTEXT_VAR
#define METER_CERT_ENUM_CONTEXT_VAR "SharedMeterCertEnumContext"
#endif
#ifndef DRM_METERCERT_CONTEXT_VAR
#define DRM_METERCERT_CONTEXT_VAR "SharedMeterCertContext"
#endif
#ifndef METER_CERT_BUFFER_VAR
#define METER_CERT_BUFFER_VAR "SharedMeterCertBuffer"
#endif

/*
** ---------------------------------------
** MeterCertStore Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestMeterCertUpdate( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestMeterCertGetByMID( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestDeleteMeterCert( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestInvalidateMeterCert( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestOpenMeterCertStore( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestCloseMeterCertStore( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestMeterCertInitEnum( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestMeterCertEnumNext( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL StopProfiling( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL StartProfiling( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL RestoreDeviceStore( long argc, __in_ecount(argc) char **argv );

DECLARE_MODULE_DEFS(MeterCertStore)
EXIT_PKTEST_NAMESPACE

#endif /* __DRM_METERCERT_TEST_API_H */


