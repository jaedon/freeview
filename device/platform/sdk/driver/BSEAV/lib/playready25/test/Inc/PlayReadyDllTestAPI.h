/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PLAYREADYDLL_TEST_API_H
#define __DRM_PLAYREADYDLL_TEST_API_H

#include <PKTestNameSpace.h>
#include <NetIOTestAPI.h>
#include <DRMManagerTestAPI.h>

ENTER_PKTEST_NAMESPACE;

/*
** ---------------------------------------
** Test Global Variable Names
** ---------------------------------------
*/
#define LICENSE_RESPONSE_VAR         "SharedLicenseResponse"
#define DECRYPT_CONTEXT_VAR          "SharedDecryptContext"
#define AES_COUNTER_MODE_CONTEXT_VAR "SharedAesCtrContext"

#define TEST_VAR_PRND_RXCTX          "ReceiverContext"

#define TEST_VAR_PRND_TXAPPCTX       "TransmitterAppContext"
#define TEST_VAR_PRND_TXAPPCTXOB     "TransmitterAppContextOpaqueBuffer"
#define TEST_VAR_PRND_TXAPPCTXDS     "TransmitterAppContextDeviceStore"
#define TEST_VAR_PRND_TXCTX          "TransmitterContext"

#define TEST_VAR_PRND_TXAPPCTX2      "TransmitterAppContext2"
#define TEST_VAR_PRND_TXAPPCTXOB2    "TransmitterAppContextOpaqueBuffer2"
#define TEST_VAR_PRND_TXCTX2         "TransmitterContext2"

#define TEST_VAR_PRND_RXAPPCTX2      "ReceiverAppContext2"
#define TEST_VAR_PRND_RXAPPCTXOB2    "ReceiverAppContextOpaqueBuffer2"
#define TEST_VAR_PRND_RXCTX2         "ReceiverContext2"

DRM_RESULT DRM_CALL PRActivationPRGC( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL PRSetHeader( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRSetHeaderByComponents( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRGetProperty( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRConvertFromWMDRMHeader( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL PRGenerateChallenge( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRProcessResponse( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL PRInitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRReinitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRUninitialize( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL PRReaderBind( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRCommit( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRInitDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRReaderDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRReaderClose( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL PRCleanupStore( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL GetContentKey( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL CipherEncrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL ReloadPlayReadyDll( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Sleep( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL Test_BasicActivation( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_BasicLicenseAcquisition( long argc, __in_ecount( argc ) char **argv );

DRM_RESULT DRM_CALL PRNDInitializeTxAppContexts( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDRegistration( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDProximityDetection( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDLicenseFetch( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDLicenseFetchChainedLicenseRebind( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDRenewal( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_PRNDTopLevelInvalidPRNDLicenseBind( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(PlayReadyDll)

EXIT_PKTEST_NAMESPACE;

#endif /* __DRM_PLAYREADYDLL_TEST_API_H */
