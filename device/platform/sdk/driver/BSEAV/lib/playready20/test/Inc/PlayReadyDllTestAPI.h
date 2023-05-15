/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PLAYREADYDLL_TEST_API_H
#define __DRM_PLAYREADYDLL_TEST_API_H

#include "PKTestNameSpace.h"
#include "NetIOTestAPI.h"
#include "DrmManagerTestAPI.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** Test Global Variable Names
** ---------------------------------------
*/
#define LICENSE_RESPONSE_VAR    "SharedLicenseResponse"
#define DECRYPT_CONTEXT_VAR "SharedDecryptContext"
#define AES_COUNTER_MODE_CONTEXT_VAR "SharedAesCtrContext"

DRM_RESULT DRM_CALL TestManagerPRActivationPRGC( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL TestManagerPRSetHeader( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRSetHeaderByComponents( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRGetProperty( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRConvertFromWMDRMHeader( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL TestManagerPRGenerateChallenge( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRProcessResponse( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL TestManagerPRInitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRReinitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRUninitialize( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL TestManagerPRReaderBind( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRCommit( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRInitDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRReaderDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPRCleanupStore( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL GetContentKey( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerPREncrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL ReloadPlayReadyDll( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Sleep( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(PlayReadyDll)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_PLAYREADYDLL_TEST_API_H */
