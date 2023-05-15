/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PRNDFORMATTEST_H
#define __DRM_PRNDFORMATTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** Test PRND Format Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestPRNDFormatRegistrationRequest( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatRegistrationResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatRegistrationError( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionStart( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionChallenge( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionResult( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatLicenseRequest( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatLicenseTransmit( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatLicenseError( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPRNDFormatIncrementLMID( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(prndformattest)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_PRNDFORMATTEST_H */

