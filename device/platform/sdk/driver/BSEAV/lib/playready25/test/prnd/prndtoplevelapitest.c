/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <prndprotocoltest.h>
#include <prndtoplevelapitest.h>
#include <DRMManagerTestAPI.h>
#include <drmmanager.h>
#include <licgenxmr.h>
#include <drmlicgen.h>
#include <drmmathsafe.h>
#include <blackboxhal.h>
#include <drmbcertparser.h>
#include <drmmeterimp.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API_25068, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

#define PRND_TEMPRX_APP_CONTEXT_VAR "PrndTempRxAppContext"
#define PRND_TEMPRX_DRMBUFFER_VAR   "PrndTempRxOpaqueBuffer"
#define PRND_TEMPRX_DEV_STORE_VAR   "PrndTempRxDeviceStore"
#define PRND_TEMPRX_NEWCTX_VAR      "PrndTempRxNewCtx"
#define PRND_TEMPRX_NEWREV_VAR      "PrndTempRxNewRev"

#define PRND_TEMPTX_APP_CONTEXT_VAR "PrndTempTxAppContext"
#define PRND_TEMPTX_DRMBUFFER_VAR   "PrndTempTxOpaqueBuffer"
#define PRND_TEMPTX_DEV_STORE_VAR   "PrndTempTxDeviceStore"
#define PRND_TEMPTX_NEWCTX_VAR      "PrndTempTxNewCtx"
#define PRND_TEMPTX_NEWREV_VAR      "PrndTempTxNewRev"

#define RX_IS_APPROPRIATE           TRUE
#define TX_IS_APPROPRIATE           FALSE

/* PRND message offsets as defined by the protocol */
#define TEST_PROXIMITY_START_MESSAGE_PROTOCOL_VERSION_OFFSET                8
#define TEST_PROXIMITY_START_MESSAGE_SESSION_ID_OFFSET                      28

#define TEST_PROXIMITY_CHALLENGE_MESSAGE_PROTOCOL_VERSION_OFFSET            8
#define TEST_PROXIMITY_CHALLENGE_MESSAGE_SESSION_ID_OFFSET                  28
#define TEST_PROXIMITY_CHALLENGE_MESSAGE_NONCE_OFFSET                       44

#define TEST_REGISTRATION_REQUEST_HEADER_SIZE                               16
#define TEST_REGISTRATION_REQUEST_PRND_PROTOCOL_VERSION_OFFSET              8
#define TEST_REGISTRATION_REQUEST_HEADER_LEN_OFFSET                         12
#define TEST_REGISTRATION_REQUEST_BASIC_INFO_LEN_OFFSET                     20
#define TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_LEN_OFFSET                44
#define TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_DATA_OFFSET               40
#define TEST_REGISTRATION_REQUEST_CUSTOMDATA_OFFSET_AFTER_BASIC_INFO        4
#define TEST_REGISTRATION_REQUEST_SIGNATURE_LEN_OFFSET_AFTER_CUSTOMDATA     4
#define TEST_REGISTRATION_REQUEST_SIGNATURE_OFFSET                          12

#define TEST_REGISTRATION_RESPONSE_REV_INFO_LEN_OFFSET                      44

#define TEST_LICENSE_REQUEST_HEADER_SIZE                                    16
#define TEST_LICENSE_REQUEST_PRND_PROTOCOL_VERSION_OFFSET                   8
#define TEST_LICENSE_REQUEST_HEADER_LEN_OFFSET                              12
#define TEST_LICENSE_REQUEST_BASIC_INFO_LEN_OFFSET                          20
#define TEST_LICENSE_REQUEST_BASIC_INFO_SESSION_ID_OFFSET                   28
#define TEST_LICENSE_REQUEST_BASIC_INFO_RIV_OFFSET                          44
#define TEST_LICENSE_REQUEST_INFO_LEN_OFFSET_AFTER_BASIC_INFO               4
#define TEST_LICENSE_REQUEST_INFO_ACTION_OFFSET_AFTER_BASIC_INFO            8
#define TEST_LICENSE_REQUEST_CUSTOMDATA_LEN_OFFSET_AFTER_REQUEST_INFO       4
#define TEST_LICENSE_REQUEST_SIGNATURE_OFFSET_AFTER_CUSTOMDATA              4
#define TEST_LICENSE_REQUEST_SIGNATURE_OFFSET                               12

#define TEST_LICENSE_TRANSMIT_RIV_LEN_OFFSET                                60

#define VERIFY_MESSAGE_TYPE( pb, cb, type )                             \
    do {                                                                \
        DRM_PRND_MESSAGE_TYPE eType = DRM_PRND_MESSAGE_TYPE_INVALID;    \
        ChkDR( Drm_Prnd_GetMessageType( pb, cb, &eType ) );             \
        ChkBOOL( eType == type, DRM_E_TEST_UNEXPECTED_OUTPUT );         \
    } while(FALSE)

static const DRM_GUID  g_guidPlayEnabler0 = { 0x86f457ea, 0x2c2a, 0x47fe, { 0xb6, 0x14, 0xf2, 0x44, 0xb9, 0xa4, 0xf4, 0xb8 } };
static const DRM_GUID  g_guidPlayEnabler1 = { 0x9dc2f715, 0xcea5, 0x4222, { 0x8c, 0x5b, 0x72, 0x2d, 0xb9, 0xc5, 0x2f, 0xf0 } };

static DRM_BOOL g_fExpectPlayEnablerCallback = FALSE;
static DRM_BOOL g_fActualPlayEnablerCallback = FALSE;

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(prndtoplevelapitest)(long lTCID, const char *strTCName)
{
    DRM_RESULT                       dr                                     = DRM_SUCCESS;
    DRM_BYTE                        *pbVar                                  = NULL;
    DRM_DWORD                        cbVar                                  = 0;
    DRM_BYTE                         rgbDefaultAESKey[DRM_AES_KEYSIZE_128]  = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    DRM_PRND_RECEIVER_CONTEXT       *pRxCtx                                 = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT    *pTxCtx                                 = NULL;
    DRM_APP_CONTEXT                 *pAppContext                            = NULL;

    /*
    ** Bank on pretestcase from prndprotocoltest to give us most of our initialization.
    ** ASSERT that it has been run by checking for the variables it creates.
    ** Note that pretestcase from prndprotocoltest also does other necessary initialization!
    */
    dr = DRM_TST_GetTestVar( TEST_VAR_PRND_RXCTX, ( DRM_VOID ** ) &pRxCtx, NULL );
    if( DRM_FAILED( dr ) )
    {
        DRMASSERT( FALSE );
        ChkDR( dr );
    }
    dr = DRM_TST_GetTestVar( TEST_VAR_PRND_TXCTX, ( DRM_VOID ** ) &pTxCtx, NULL );
    if( DRM_FAILED( dr ) )
    {
        DRMASSERT( FALSE );
        ChkDR( dr );
    }

    cbVar = DRM_AES_KEYSIZE_128;
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbVar, AES_KEY_VAR, cbVar );
    DRMCRT_memcpy( pbVar, rgbDefaultAESKey, cbVar );

    cbVar = DRMCIPHERKEYLEN_RC4 * SIZEOF( DRM_BYTE );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbVar, RC4_KEY_VAR, cbVar );

    cbVar = SIZEOF( DRM_TEST_XPOL_DATA );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbVar, RESPONSE_XPOL_RIGHT_VAR, cbVar );

    cbVar = SIZEOF( DRM_TEST_XPOL_DATA );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbVar, RESPONSE_XPOL_RESTRICTION_VAR, cbVar );

    cbVar = SIZEOF( DRM_GUID ) + 1;
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbVar, LICENSE_ID_VAR, cbVar );

    cbVar = SIZEOF( DRM_ID );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbVar, TEST_VAR_PROXIMITY_NONCE, cbVar );

    /* Set secure clock */
    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Oem_Clock_SetResetState( NULL, FALSE ) );
    ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->fClockSet = TRUE;

    g_fExpectPlayEnablerCallback = FALSE;
    g_fActualPlayEnablerCallback = FALSE;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(prndtoplevelapitest)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    /* Bank on posttestcase from prndprotocoltest to give us most of our uninitialization */
    FREE_APP_CONTEXT_BY_NAME_NO_DEV_STORE_OR_APP( PRND_TEMPRX_APP_CONTEXT_VAR, PRND_TEMPRX_DRMBUFFER_VAR, PRND_TEMPRX_DEV_STORE_VAR );
    FREE_APP_CONTEXT_BY_NAME_NO_DEV_STORE_OR_APP( PRND_TEMPTX_APP_CONTEXT_VAR, PRND_TEMPTX_DRMBUFFER_VAR, PRND_TEMPTX_DEV_STORE_VAR );

    g_fExpectPlayEnablerCallback = FALSE;
    g_fActualPlayEnablerCallback = FALSE;

    /* We don't need to uninitialize APP_CONTEXT_VAR.  Shared post test case will do that for us. */

    /* Do NOT define ErrorExit label here.  Do NOT use ChkDR (etc) above.  Doing so could skip cleanup operations. */
    return dr;
}

/*****************************************************************************
** Arguments: argc    - count of the number of arguments
**            argv[0] - OPTIONAL - when present, indicates the number of seconds (positive or negative)
**                      by which to adjust the current time, for the purpose of simulating clock drift
**                      on the Rx relative to the Tx.
** Notes:
**      The top-level registration APIs are essentially a straight pass-throughs to
**      the underlying protocol code.  See prndprotocoltest.c for the real set of test cases
**      related to it.  This test API just does simple validation that the API set works
**      and unblocks further messages in the PRND protocol.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelRegistration( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                            = NULL;
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                 = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                 = NULL;
    DRM_BYTE                            *pbMsgRxToTx                            = NULL;
    DRM_DWORD                            cbMsgRxToTx                            = 0;
    DRM_BYTE                            *pbMsgTxToRx                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                            = 0;
    DRM_ID                               TxSessionID                            = {0};
    DRM_ID                               RxSessionID                            = {0};
    DRM_PRND_PROXIMITY_DETECTION_TYPE    eProximityDetectionType                = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;
    DRM_BYTE                            *pbTransmitterProximityDetectionChannel = NULL;
    DRM_DWORD                            cbTransmitterProximityDetectionChannel = 0;
    DRM_DWORD                            dwFlagsOut                             = 0;
    long                                 lRxClockDriftSeconds                   = 0;

    if ( HASARG( 0 ) )
    {
        lRxClockDriftSeconds = atoi( argv[0] );
    }

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    /* Reset the Rx Context if it exists */
    if( ((DRM_APP_CONTEXT_INTERNAL *)pAppContext)->poPrndReceiverContext != NULL )
    {
        ChkDR( Drm_Prnd_Receiver_EndSession( pAppContext ) );
        ChkDR( DRM_TST_FreeTestVar( TEST_VAR_PRND_RXCTX ) );
        ((DRM_APP_CONTEXT_INTERNAL *)pAppContext)->poPrndReceiverContext = NULL;

        MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pRxCtx, TEST_VAR_PRND_RXCTX, SIZEOF( DRM_PRND_RECEIVER_CONTEXT ) );
    }

    /* Reset the Tx Context if it exists */
    if( ((DRM_APP_CONTEXT_INTERNAL *)pAppContext)->poPrndTransmitterContext != NULL )
    {
        ChkDR( Drm_Prnd_Transmitter_EndSession( pAppContext ) );
        ChkDR( DRM_TST_FreeTestVar( TEST_VAR_PRND_TXCTX ) );
        ((DRM_APP_CONTEXT_INTERNAL *)pAppContext)->poPrndTransmitterContext = NULL;

        MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pTxCtx, TEST_VAR_PRND_TXCTX, SIZEOF( DRM_PRND_TRANSMITTER_CONTEXT ) );
    }

    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    /* Rx: Begin Session */
    ChkDR( Drm_Prnd_Receiver_BeginSession( pAppContext, pRxCtx ) );

    /* Rx: Generate Registration Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    dr = Drm_Prnd_Receiver_RegistrationRequest_Generate(
        pAppContext,
        NULL,
        &g_badCustomDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx );

    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_INVALID_CUSTOM_DATA_TYPE );

    tChangeSystemTime( lRxClockDriftSeconds );

    ChkDR( Drm_Prnd_Receiver_RegistrationRequest_Generate(
        pAppContext,
        NULL,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST );

    tChangeSystemTime( -lRxClockDriftSeconds );

    /* Tx: Begin Session */
    ChkDR( Drm_Prnd_Transmitter_BeginSession( pAppContext, pTxCtx ) );

    /* Tx: Process Registration Request, Generate Registration Response */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    ChkDR( Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,    /* Test purposes only - real world caller would pass their own context */
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut ) );
    ChkBOOL( pbTransmitterProximityDetectionChannel != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbTransmitterProximityDetectionChannel  > 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

    dr = Drm_Prnd_Transmitter_RegistrationResponse_Generate(
        pAppContext,
        &g_badCustomDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_INVALID_CUSTOM_DATA_TYPE );

    ChkDR( Drm_Prnd_Transmitter_RegistrationResponse_Generate(
        pAppContext,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_RESPONSE );

    /* Verify the response message contains no rev info because we are using the same context */
    {
        DRM_DWORD dwRevInfoLen = 0;

        NETWORKBYTES_TO_DWORD( dwRevInfoLen, pbMsgTxToRx, TEST_REGISTRATION_RESPONSE_REV_INFO_LEN_OFFSET );
        ChkBOOL( dwRevInfoLen == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* Rx: Process Registration Response */
    /* Note: In the real world, this method will return DRM_E_PRND_MESSAGE_WRONG_TYPE if the message is an error message instead. */
    ChkDR( Drm_Prnd_Receiver_RegistrationResponse_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,    /* Test purposes only - real world caller would pass their own context */
        &RxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut ) );
    ChkBOOL( pbTransmitterProximityDetectionChannel != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbTransmitterProximityDetectionChannel  > 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

    /* Test: Verify that sessino IDs all match */
    ChkBOOL( 0 == DRMCRT_memcmp( &RxSessionID, &pTxCtx->oPrndTransmitterSessionContext.SessionID,   SIZEOF( DRM_GUID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0 == DRMCRT_memcmp( &RxSessionID, &pRxCtx->oPrndReceiverSessionContext.SessionID,      SIZEOF( DRM_GUID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0 == DRMCRT_memcmp( &RxSessionID, &TxSessionID,                                        SIZEOF( DRM_GUID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );
    return dr;
}

/*****************************************************************************
** Arguments:
**      None.
******************************************************************************/
DRM_RESULT DRM_CALL TestMalformedPRNDTopLevelRegistration( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                            = NULL;
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                 = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                 = NULL;
    DRM_BYTE                            *pbMsgRxToTx                            = NULL;
    DRM_DWORD                            cbMsgRxToTx                            = 0;
    DRM_BYTE                            *pbMsgTxToRx                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                            = 0;
    DRM_ID                               TxSessionID                            = {0};
    DRM_ID                               RxSessionID                            = {0};
    DRM_PRND_PROXIMITY_DETECTION_TYPE    eProximityDetectionType                = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;
    DRM_BYTE                            *pbTransmitterProximityDetectionChannel = NULL;
    DRM_DWORD                            cbTransmitterProximityDetectionChannel = 0;
    DRM_DWORD                            dwFlagsOut                             = 0;
    DRM_DWORD                            dwTotalLen                             = 0;
    DRM_DWORD                            dwBasicInfoLen                         = 0;
    DRM_DWORD                            dwCertLen                              = 0;
    DRM_DWORD                            dwCustomDataLen                        = 0;
    DRM_DWORD                            dwSigLen                               = 0;
    DRM_DWORD                            dwCertData                             = 0;
    PUBKEY_P256                          oPubKey                                = {0};

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    /* Rx: Begin Session */
    ChkDR( Drm_Prnd_Receiver_BeginSession( pAppContext, pRxCtx ) );

    /* Rx: Generate Registration Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    ChkDR( Drm_Prnd_Receiver_RegistrationRequest_Generate(
        pAppContext,
        NULL,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST );

    /* Parse the message to get the length out */
    NETWORKBYTES_TO_DWORD( dwTotalLen,      pbMsgRxToTx, TEST_REGISTRATION_REQUEST_HEADER_LEN_OFFSET );
    NETWORKBYTES_TO_DWORD( dwBasicInfoLen,  pbMsgRxToTx, TEST_REGISTRATION_REQUEST_BASIC_INFO_LEN_OFFSET );
    NETWORKBYTES_TO_DWORD( dwCertLen,       pbMsgRxToTx, TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_LEN_OFFSET );
    NETWORKBYTES_TO_DWORD( dwCustomDataLen, pbMsgRxToTx, TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen + TEST_REGISTRATION_REQUEST_CUSTOMDATA_OFFSET_AFTER_BASIC_INFO );
    NETWORKBYTES_TO_DWORD( dwSigLen,        pbMsgRxToTx, TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen + TEST_REGISTRATION_REQUEST_SIGNATURE_LEN_OFFSET_AFTER_CUSTOMDATA );

    ChkBOOL( dwCertLen  >  TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_DATA_OFFSET + SIZEOF( DRM_DWORD ) , DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwTotalLen == TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen + dwSigLen, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Change the PRND Protocol to 0 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_REGISTRATION_REQUEST_PRND_PROTOCOL_VERSION_OFFSET, 0 );
    ChkDR( DRM_BBX_ECC256_Sign(
        pRxCtx->pBBXContext,
        pbMsgRxToTx,
        TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen,
        &oPubKey,
        pbMsgRxToTx + TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen + TEST_REGISTRATION_REQUEST_SIGNATURE_OFFSET ) );

    /* Tx: Begin Session */
    ChkDR( Drm_Prnd_Transmitter_BeginSession( pAppContext, pTxCtx ) );

    /* Tx: Process Registration Request fails with invalid version */
    dr = Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /* Change the PRND Protocol to  DRM_PRND_CURRENT_VERSION + 1 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_REGISTRATION_REQUEST_PRND_PROTOCOL_VERSION_OFFSET, DRM_PRND_CURRENT_VERSION + 1 );
    ChkDR( DRM_BBX_ECC256_Sign(
        pRxCtx->pBBXContext,
        pbMsgRxToTx,
        TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen,
        &oPubKey,
        pbMsgRxToTx + TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen + TEST_REGISTRATION_REQUEST_SIGNATURE_OFFSET ) );

    /* Tx: Process Registration Request fails with invalid version */
    dr = Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /* Reset protocol version */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_REGISTRATION_REQUEST_PRND_PROTOCOL_VERSION_OFFSET, DRM_PRND_CURRENT_VERSION );

    /* Change the cert data */
    NETWORKBYTES_TO_DWORD( dwCertData, pbMsgRxToTx, TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_LEN_OFFSET + TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_DATA_OFFSET );
    ChkDR( Oem_Random_GetBytes(
        NULL,
        pbMsgRxToTx + TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_LEN_OFFSET + TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_DATA_OFFSET,
        SIZEOF( DRM_DWORD ) ) );

    ChkDR( DRM_BBX_ECC256_Sign(
        pRxCtx->pBBXContext,
        pbMsgRxToTx,
        TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen,
        &oPubKey,
        pbMsgRxToTx + TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen + TEST_REGISTRATION_REQUEST_SIGNATURE_OFFSET ) );

    /* Tx: Process Registration Request fails with invalid bcert */
    dr = Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    /* Reset cert data */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_LEN_OFFSET + TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_DATA_OFFSET, dwCertData );

    /* Set RevInfo to 0 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_REGISTRATION_REQUEST_BASIC_INFO_CERT_LEN_OFFSET + SIZEOF( DRM_DWORD ) + dwCertLen, 0 );
    ChkDR( DRM_BBX_ECC256_Sign(
        pRxCtx->pBBXContext,
        pbMsgRxToTx,
        TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen,
        &oPubKey,
        pbMsgRxToTx + TEST_REGISTRATION_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwCustomDataLen + TEST_REGISTRATION_REQUEST_SIGNATURE_OFFSET ) );

    ChkDR( Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut ) );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    ChkDR( Drm_Prnd_Transmitter_RegistrationResponse_Generate(
        pAppContext,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_RESPONSE );

    /* Verify the response message contains rev info */
    {
        DRM_DWORD dwRevInfoLen = 0;

        NETWORKBYTES_TO_DWORD( dwRevInfoLen, pbMsgTxToRx, TEST_REGISTRATION_RESPONSE_REV_INFO_LEN_OFFSET );
        ChkBOOL( dwRevInfoLen > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* Rx: Process Registration Response */
    ChkDR( Drm_Prnd_Receiver_RegistrationResponse_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,        /* Test purposes only - real world caller would pass their own context */
        &RxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut ) );
    ChkBOOL( pbTransmitterProximityDetectionChannel != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbTransmitterProximityDetectionChannel  > 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

    return dr;
}

/*****************************************************************************
** Arguments:
**      None.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelRenewal( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                            = NULL;
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                 = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                 = NULL;
    DRM_BYTE                            *pbMsgRxToTx                            = NULL;
    DRM_DWORD                            cbMsgRxToTx                            = 0;
    DRM_BYTE                            *pbMsgTxToRx                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                            = 0;
    DRM_ID                               TxSessionID                            = {0};
    DRM_ID                               RxSessionID                            = {0};
    DRM_PRND_PROXIMITY_DETECTION_TYPE    eProximityDetectionType                = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;
    DRM_BYTE                            *pbTransmitterProximityDetectionChannel = NULL;
    DRM_DWORD                            cbTransmitterProximityDetectionChannel = 0;

    DRM_APP_CONTEXT                     *pNewRxAppContext                       = NULL;
    DRM_PRND_RECEIVER_CONTEXT           *pNewRxCtx                              = NULL;
    DRM_APP_CONTEXT                     *pNewTxAppContext                       = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pNewTxCtx                              = NULL;
    DRM_DWORD                            dwFlagsOut                             = 0;

    /* Rx: Initialize a *NEW* app context. */
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pNewRxCtx, PRND_TEMPRX_NEWCTX_VAR, SIZEOF( DRM_PRND_RECEIVER_CONTEXT ) );
    INIT_APP_CONTEXT_BY_NAME_NO_DEV_STORE_WITH_REV( pNewRxAppContext, PRND_TEMPRX_APP_CONTEXT_VAR, PRND_TEMPRX_DRMBUFFER_VAR, PRND_TEMPRX_DEV_STORE_VAR, PRND_TEMPRX_NEWREV_VAR );

    /* Rx: Begin *NEW* Session */
    ChkDR( Drm_Prnd_Receiver_BeginSession( pNewRxAppContext, pNewRxCtx ) );

    /* Rx: Generate Registration (Renewal) Request using the *new* session, use the existing Rx Session ID.  Get an error because we need to use the existing session! */
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;
    dr = Drm_Prnd_Receiver_RegistrationRequest_Generate(
        pNewRxAppContext,
        &pRxCtx->oPrndReceiverSessionContext.SessionID,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_CANNOT_RENEW_USING_NEW_SESSION );

    /* Rx: End the *new* session */
    ChkDR( Drm_Prnd_Receiver_EndSession( pNewRxAppContext ) );

    /* Rx: Make sure it gave us nothing. */
    ChkBOOL( NULL == pbMsgRxToTx, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0    == cbMsgRxToTx, DRM_E_TEST_UNEXPECTED_OUTPUT );
    (DRM_VOID)Drm_Uninitialize( pNewRxAppContext );
    pNewRxAppContext = NULL;
    pNewRxCtx        = NULL;

    /* Rx: Generate Registration (Renewal) Request using the *existing* session, use the existing Rx Session ID.  This should succeed. */
    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Prnd_Receiver_RegistrationRequest_Generate(
        pAppContext,
        &pRxCtx->oPrndReceiverSessionContext.SessionID,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    pRxCtx = NULL;
    VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST );

    /* Tx: Initialize a *NEW* app context. */
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pNewTxCtx, PRND_TEMPTX_NEWCTX_VAR, SIZEOF( DRM_PRND_TRANSMITTER_CONTEXT ) );
    INIT_APP_CONTEXT_BY_NAME_NO_DEV_STORE_WITH_REV( pNewTxAppContext, PRND_TEMPTX_APP_CONTEXT_VAR, PRND_TEMPTX_DRMBUFFER_VAR, PRND_TEMPTX_DEV_STORE_VAR, PRND_TEMPTX_NEWREV_VAR );

    /* Tx: Begin *NEW* Session */
    ChkDR( Drm_Prnd_Transmitter_BeginSession( pNewTxAppContext, pNewTxCtx ) );

    /* Tx: Process Registration (Renewal) Request using the *new* session, get an error because it's an existing session! */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    dr = Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pNewTxAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pNewTxAppContext)->oBlackBoxContext.pKeyFileContext,    /* Test purposes only - real world caller would pass their own context */
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_CANNOT_RENEW_USING_NEW_SESSION );

    /* Tx: End the *new* session */
    ChkDR( Drm_Prnd_Transmitter_EndSession( pNewTxAppContext ) );

    /* Tx: Make sure it gave us the session ID but nothing else. */
    ChkBOOL( pbTransmitterProximityDetectionChannel == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbTransmitterProximityDetectionChannel == 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbMsgTxToRx == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsgTxToRx == 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    (DRM_VOID)Drm_Uninitialize( pNewTxAppContext );
    pNewTxAppContext = NULL;
    pNewTxCtx        = NULL;
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );
    ChkBOOL( 0    == DRMCRT_memcmp( &TxSessionID, &pTxCtx->oPrndTransmitterSessionContext.SessionID,   SIZEOF( DRM_GUID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    pTxCtx = NULL;

    /* Tx: Process Registration Request (Renewal) using the *existing* session, Generate Registration Response */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    ChkDR( Drm_Prnd_Transmitter_RegistrationRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,    /* Test purposes only - real world caller would pass their own context */
        &TxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut ) );
    ChkBOOL( pbTransmitterProximityDetectionChannel != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbTransmitterProximityDetectionChannel  > 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

    ChkDR( Drm_Prnd_Transmitter_RegistrationResponse_Generate(
        pAppContext,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_RESPONSE );

    /* Rx: Process Registration Response with the *existing* session */
    ChkDR( Drm_Prnd_Receiver_RegistrationResponse_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,    /* Test purposes only - real world caller would pass their own context */
        &RxSessionID,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannel,
        &cbTransmitterProximityDetectionChannel,
        &dwFlagsOut ) );
    ChkBOOL( pbTransmitterProximityDetectionChannel != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbTransmitterProximityDetectionChannel  > 0,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );

ErrorExit:
    (DRM_VOID)Drm_Uninitialize( pNewRxAppContext );
    (DRM_VOID)Drm_Uninitialize( pNewTxAppContext );

    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    DRM_PRND_SAFEMEMFREE( pbTransmitterProximityDetectionChannel );
    return dr;
}

#define TestPRNDTopLevelRegistration_Arg0_FLAGS_Expect_Success_Start                    0x00000001
#define TestPRNDTopLevelRegistration_Arg0_FLAGS_Start_Before_Registration               0x00000002

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDTopLevelProximityDetection_Arg0_FLAGS_* values above.
** Notes:
**      The top-level proximity APIs are essentially a straight pass-throughs to
**      the underlying protocol code.  See prndprotocoltest.c for the real set of test cases
**      related to it.  This test API just does simple validation that the API set works
**      and unblocks further messages in the PRND protocol.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelProximityDetection( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                            = DRM_SUCCESS;
    DRM_APP_CONTEXT *pAppContext                   = NULL;
    DRM_BYTE        *pbMsgRxToTx                   = NULL;
    DRM_DWORD        cbMsgRxToTx                   = 0;
    DRM_BYTE        *pbMsgTxToRx                   = NULL;
    DRM_DWORD        cbMsgTxToRx                   = 0;
    DRM_DWORD        cRetries                      = 2;    /* Default to 2 retries (i.e. 3 tries total) */
    DRM_UINT64       qwTick                        = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64       qwTickDiff                    = DRM_UI64LITERAL( 0, 0 );
    DRM_RESULT       drPD                          = DRM_SUCCESS;
    DRM_DWORD        dwFlagsOut                    = 0;
    DRM_BOOL         fExpectSuccessStart           = FALSE;
    DRM_BOOL         fStartBeforeRegistration      = FALSE;
    DRM_ID          *pNonce                        = NULL;

    GET_SHARED_APP_CONTEXT( pAppContext );

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );

        fExpectSuccessStart      = 0 != ( dwArg0Flags & TestPRNDTopLevelRegistration_Arg0_FLAGS_Expect_Success_Start );
        fStartBeforeRegistration = 0 != ( dwArg0Flags & TestPRNDTopLevelRegistration_Arg0_FLAGS_Start_Before_Registration );
    }

    if( fStartBeforeRegistration )
    {
        DRM_PRND_RECEIVER_CONTEXT    *pRxCtx    = NULL;
        DRM_PRND_TRANSMITTER_CONTEXT *pTxCtx    = NULL;

        MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
        MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

        ChkDR( Drm_Prnd_Receiver_BeginSession( pAppContext, pRxCtx ) );
        ChkDR( Drm_Prnd_Transmitter_BeginSession( pAppContext, pTxCtx ) );
    }

    MAP_TO_GLOBAL_TEST_VAR( pNonce, TEST_VAR_PROXIMITY_NONCE );

    do
    {
        /* Rx: Start proximity detection */
        DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
        cbMsgRxToTx = 0;
        ChkDR( Drm_Prnd_Receiver_ProximityDetectionStart_Generate(
            pAppContext,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgRxToTx,
            &cbMsgRxToTx ) );
        VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_START );

        /*
        ** It's possible we can get timeout (>7ms) even running completely locally.
        ** For example, running on a machine that's under very heavy CPU load.
        ** We don't want to fail the test case under those circumstances,
        ** so if a timeout occurs we retry up to 3 times.
        */
        /* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms on Desktop per MSDN.  We need 1 ms resolution. */
        qwTick =
            DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                      Oem_Clock_QueryPerformanceCounter( NULL ) ),
                         Oem_Clock_QueryPerformanceFrequency( NULL ) );

        Log( "Trace", "Time before proximity detection: %d:%d", DRM_UI64High32(qwTick), DRM_UI64Low32(qwTick) );

        /* Tx: Process start, Send proximity challenge */
        DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
        cbMsgTxToRx = 0;
        ChkDR( Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
            pAppContext,
            pbMsgRxToTx,
            cbMsgRxToTx,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgTxToRx,
            &cbMsgTxToRx,
            &dwFlagsOut ) );

        if( fExpectSuccessStart )
        {
            VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESULT );
        }
        else
        {
            VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_CHALLENGE );

            /* compare that the current nonce is different from the saved nonce */
            ChkBOOL( 0 != DRMCRT_memcmp( pNonce, pbMsgTxToRx + TEST_PROXIMITY_CHALLENGE_MESSAGE_NONCE_OFFSET, SIZEOF( DRM_ID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

            /* saved the new nonce */
            DRMCRT_memcpy(
                (DRM_BYTE*)pNonce,
                pbMsgTxToRx + TEST_PROXIMITY_CHALLENGE_MESSAGE_NONCE_OFFSET,
                SIZEOF( DRM_ID ) );

            /* Rx: Process challenge, Send proximity response */
            /* Note: In the real world, this method will return DRM_E_PRND_MESSAGE_WRONG_TYPE if the message is a result message instead. */
            DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
            cbMsgRxToTx = 0;
            ChkDR( Drm_Prnd_Receiver_ProximityDetectionChallenge_Process(
                pAppContext,
                pbMsgTxToRx,
                cbMsgTxToRx,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsgRxToTx,
                &cbMsgRxToTx,
                &dwFlagsOut ) );
            VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESPONSE );

            /* Tx: Process response, Send proximity result */
            DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
            cbMsgTxToRx = 0;
            ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(
                pAppContext,
                pbMsgRxToTx,
                cbMsgRxToTx,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsgTxToRx,
                &cbMsgTxToRx,
                &dwFlagsOut ) );
            VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESULT );

        }

        /* Rx: Process proximity result */
        ChkDR( Drm_Prnd_Receiver_ProximityDetectionResult_Process(
            pAppContext,
            pbMsgTxToRx,
            cbMsgTxToRx,
            &drPD,
            &dwFlagsOut ) );

        /*
        ** Refer to comments similar call above.
        */
        {
            /* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms on Desktop per MSDN.  We need 1 ms resolution. */
            DRM_UINT64 qwTick2 =
                DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                          Oem_Clock_QueryPerformanceCounter( NULL ) ),
                             Oem_Clock_QueryPerformanceFrequency( NULL ) );

            qwTickDiff = DRM_UI64Sub( qwTick2, qwTick );
            Log( "Trace", "Time  after proximity detection: %d:%d   Diff = %d:%d", DRM_UI64High32(qwTick2), DRM_UI64Low32(qwTick2), DRM_UI64High32(qwTickDiff), DRM_UI64Low32(qwTickDiff) );
        }

        if( drPD != DRM_E_PRND_PROXIMITY_DETECTION_RESPONSE_TIMEOUT )
        {
            /* Only retry on timeout error */
            cRetries = 0;
        }
        else if( cRetries > 0 )
        {
            if( DRM_UI64Les( DRM_UI64HL( 0, DRM_PRND_PROXIMITY_RTT_THRESHOLD_MS ), qwTickDiff ) )
            {
                Log( "Trace", "RTT was unexpectedly more than 7 ms.  This may be due to heavy load on the system.  Retrying..." );
            }
            else
            {
                Log( "Trace", "RTT was <= 7 ms but we got timeout.  This should NEVER HAPPEN and indicates a dev bug!" );
                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
            cRetries--;
        }
        else
        {
            Log( "Trace", "RTT was unexpectedly more than 7 ms too many times.  Halting retry loop." );
        }

    } while( cRetries > 0 );

    ChkDR( drPD );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    return dr;
}

/*****************************************************************************
** Arguments:
**      None.
******************************************************************************/
DRM_RESULT DRM_CALL TestMalformedPRNDTopLevelProximityDetection( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_APP_CONTEXT         *pAppContext        = NULL;
    DRM_BYTE                *pbMsgRxToTx        = NULL;
    DRM_DWORD                cbMsgRxToTx        = 0;
    DRM_BYTE                *pbMsgTxToRx        = NULL;
    DRM_DWORD                cbMsgTxToRx        = 0;
    DRM_DWORD                dwFlagsOut         = 0;
    DRM_ID                   sessionID          = {0};

    GET_SHARED_APP_CONTEXT( pAppContext );

    /* Rx: Store the SessionID */
    ChkArg( ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->poPrndReceiverContext != NULL );
    DRMCRT_memcpy(
        &sessionID,
        &((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->poPrndReceiverContext->oPrndReceiverSessionContext.SessionID,
        SIZEOF( sessionID ) );

    /* Rx: Start proximity detection */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;
    ChkDR( Drm_Prnd_Receiver_ProximityDetectionStart_Generate(
        pAppContext,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    ChkBOOL( cbMsgRxToTx >= TEST_PROXIMITY_START_MESSAGE_SESSION_ID_OFFSET + SIZEOF( DRM_ID ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Set random session id */
    ChkDR( Oem_Random_GetBytes(
        NULL,
        pbMsgRxToTx + TEST_PROXIMITY_START_MESSAGE_SESSION_ID_OFFSET,
        SIZEOF( DRM_ID ) ) );

    /* Tx: Process start and fail for SessionID mismatch */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    dr = Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_SESSION_ID_INVALID );

    /* Rx: Reset session id */
    DRMCRT_memcpy(
        pbMsgRxToTx + TEST_PROXIMITY_START_MESSAGE_SESSION_ID_OFFSET,
        &sessionID,
        SIZEOF( DRM_ID ) );

    /* Rx: Set Protocol version to 0 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_PROXIMITY_START_MESSAGE_PROTOCOL_VERSION_OFFSET, 0 );
    dr = Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /* Rx: Set Protocol version to DRM_PRND_CURRENT_VERSION + 1 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_PROXIMITY_START_MESSAGE_PROTOCOL_VERSION_OFFSET, DRM_PRND_CURRENT_VERSION + 1 );
    dr = Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /* Rx: Reset Protocol version */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_PROXIMITY_START_MESSAGE_PROTOCOL_VERSION_OFFSET, DRM_PRND_CURRENT_VERSION );

    /* Tx: Process start */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut ) );

    /* Rx: Process challenge */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;
    ChkDR( Drm_Prnd_Receiver_ProximityDetectionChallenge_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx,
        &dwFlagsOut ) );
    ChkBOOL( cbMsgRxToTx >= TEST_PROXIMITY_CHALLENGE_MESSAGE_SESSION_ID_OFFSET + SIZEOF( DRM_ID ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Rx: set random session id */
    ChkDR( Oem_Random_GetBytes(
        NULL,
        pbMsgRxToTx + TEST_PROXIMITY_CHALLENGE_MESSAGE_SESSION_ID_OFFSET,
        SIZEOF( DRM_ID ) ) );

    /* Tx: Process response and failed with wrong SessionID */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;
    dr = Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_SESSION_ID_INVALID );

    /* Rx: Reset to the original SessionID */
    DRMCRT_memcpy(
        pbMsgRxToTx + TEST_PROXIMITY_CHALLENGE_MESSAGE_SESSION_ID_OFFSET,
        &sessionID,
        SIZEOF( DRM_ID ) );
    ChkBOOL( cbMsgRxToTx >= TEST_PROXIMITY_CHALLENGE_MESSAGE_NONCE_OFFSET + SIZEOF( DRM_ID ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Rx: Set random nonce */
    ChkDR( Oem_Random_GetBytes(
        NULL,
        pbMsgRxToTx + TEST_PROXIMITY_CHALLENGE_MESSAGE_NONCE_OFFSET,
        SIZEOF( DRM_ID ) ) );

    dr = Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_INVALID );

    /* Rx: sends proximity response before proximity start */
    {
        DRM_APP_CONTEXT                     *pNewTxAppContext                       = NULL;
        DRM_PRND_TRANSMITTER_CONTEXT        *pNewTxCtx                              = NULL;

        /* Tx: Initialize a *NEW* app context. */
        MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pNewTxCtx, PRND_TEMPTX_NEWCTX_VAR, SIZEOF( DRM_PRND_TRANSMITTER_CONTEXT ) );
        INIT_APP_CONTEXT_BY_NAME_NO_DEV_STORE_WITH_REV( pNewTxAppContext, PRND_TEMPTX_APP_CONTEXT_VAR, PRND_TEMPTX_DRMBUFFER_VAR, PRND_TEMPTX_DEV_STORE_VAR, PRND_TEMPTX_NEWREV_VAR );

        /* Tx: Begin *NEW* Session */
        ChkDR( Drm_Prnd_Transmitter_BeginSession( pNewTxAppContext, pNewTxCtx ) );

        /* Tx: Process Proxity! */
        DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
        cbMsgTxToRx = 0;

        dr = Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(
            pNewTxAppContext,
            pbMsgRxToTx,
            cbMsgRxToTx,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgTxToRx,
            &cbMsgTxToRx,
            &dwFlagsOut );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_NOT_REGISTERED );
    }

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    return dr;
}

static DRM_RESULT DRM_CALL _AddLicense(
    __inout       DRM_APP_CONTEXT                     *pAppContext,
    __in    const DRM_WCHAR                            rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ],
    __in    const DRM_DWORD                            dwExpireTime,
    __in    const DRM_DWORD                            dwExpireAfterFirstPlay,
    __in          DRM_BOOL                             fForceLicenseInPRNDRxStore,
    __in          DRM_BOOL                             fValidPlayEnabler,
    __in          DRM_BOOL                             fInvalidPlayEnabler,
    __in          DRM_BOOL                             fCannotPersistInNST,
    __in          DRM_BOOL                             fCannotPersistInEST,
    __in          DRM_LICENSE_HANDLE                   hRootLicense,
    __in          DRM_LOCAL_LICENSE_TYPE               eLicenseType,
    __out         DRM_LICENSE_HANDLE                  *phLicense )
{
    DRM_RESULT                          dr                                                     = DRM_SUCCESS;
    DRM_CHAR                            rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ]  = {0};
    DRM_CONST_STRING                    dstrKid                                                = EMPTY_DRM_STRING;
    DRM_ID                              drmidKID                                               = {0};
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR oLicensePolicyDecriptor                                = {0};
    DRM_LICENSE_HANDLE                  hLicenseHandle                                         = DRM_LICENSE_HANDLE_INVALID;
    DRMFILETIME                         ftCurrent                                              = {0};
    DRM_DWORD                           dwCurrentTime                                          = 0;
    DRM_BYTE                           *pbLicenseToStore                                       = NULL;
    DRM_WORD                            wPlayEnablerCount                                      = 0;
    DRM_DWORD                           cbHeader                                               = 0;
    DRM_BYTE                           *pbHeader                                               = NULL;
    DRM_DWORD                           cbPROHeader                                            = 0;
    DRM_BYTE                           *pbPROHeader                                            = NULL;

    /*
    ** Tx: For test purposes only, create and store a transmitter-bound license for oKid.
    ** Note: In the real world, this license would already be available, e.g. acquired from a server using license acquisition.
    */
    if( eLicenseType == eDRM_LOCAL_LICENSE_LEAF )
    {
        ChkArg( hRootLicense != DRM_LICENSE_HANDLE_INVALID );
    }

    /* returns void */
    DRM_UTL_DemoteUNICODEtoASCII( rgwchKid, rgchKid, SIZEOF( rgchKid ) );
    ChkDR( MakeDRMString( &dstrKid, rgchKid ) );
    ChkDR( DRM_UTL_DecodeKID( &dstrKid, &drmidKID ) );

    /* Call the local license generation code to generate a license */
    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

    oLicensePolicyDecriptor.wSecurityLevel   = 150;
    oLicensePolicyDecriptor.fCannotPersist   = fCannotPersistInNST || fCannotPersistInEST;     /* All licenses EXCEPT those in NST or EST are NOT cannot persist. */

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrent );
    FILE_TIME_TO_DWORD( ftCurrent, dwCurrentTime );
    oLicensePolicyDecriptor.oExpiration.fValid      = TRUE;
    oLicensePolicyDecriptor.oExpiration.dwBeginDate = DRM_LOCAL_LICENSE_EXPIRATION_MIN_BEGIN_DATE;

    if( dwExpireTime != DRM_LOCAL_LICENSE_INFINITE_EXPIRATION )
    {
        ChkDR( DRM_DWordAdd( dwCurrentTime, dwExpireTime, &oLicensePolicyDecriptor.oExpiration.dwEndDate ) );
    }
    else
    {
        oLicensePolicyDecriptor.oExpiration.dwEndDate = DRM_LOCAL_LICENSE_INFINITE_EXPIRATION;
    }

    if( !fForceLicenseInPRNDRxStore && !fCannotPersistInNST && !fCannotPersistInEST )
    {
        /* License in PRND Rx store or in NST or EST cannot have Expire After First Play */
        oLicensePolicyDecriptor.oExpirationAfterFirstPlay.fValid  = TRUE;
        oLicensePolicyDecriptor.oExpirationAfterFirstPlay.dwValue = dwExpireAfterFirstPlay;
    }

    if( fInvalidPlayEnabler )
    {
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[ wPlayEnablerCount ],     &g_guidPlayEnabler0, SIZEOF( DRM_GUID ) );
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[ wPlayEnablerCount + 1 ], &g_guidPlayEnabler1, SIZEOF( DRM_GUID ) );

        wPlayEnablerCount                     += 2;
        oLicensePolicyDecriptor.cPlayEnablers =  wPlayEnablerCount;
    }

    if( fValidPlayEnabler )
    {
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[ wPlayEnablerCount ],     &DRM_PLAYENABLER_UNKNOWN_OUTPUT, SIZEOF( DRM_GUID ) );
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[ wPlayEnablerCount + 1 ], &DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT, SIZEOF( DRM_GUID ) );

        wPlayEnablerCount                     += 2;
        oLicensePolicyDecriptor.cPlayEnablers =  wPlayEnablerCount;
    }

    if( eLicenseType == eDRM_LOCAL_LICENSE_LEAF )
    {
        ChkDR( Drm_LocalLicense_CreateLicense(
            pAppContext,
           &oLicensePolicyDecriptor,
            eDRM_LOCAL_LICENSE_LEAF,
           &drmidKID,
            0,
            NULL,
            hRootLicense,
           &hLicenseHandle ) );
    }
    else if( fCannotPersistInEST )
    {
        /* We can't embed a local-bound license in a EST, so we'll cheat by creating a "remote" bound license that's actually bound to our own device certificate! */
        DRM_BYTE  *pbCert = NULL;
        DRM_DWORD  cbCert = 0;

        /* DRM_KF_GetCertificate returns a direct pointer to internal data.  Do not free pbCert/cbCert. */
        ChkDR( DRM_KF_GetCertificate(
            ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
            eKF_CERT_TYPE_PLAYREADY,
            &pbCert,
            &cbCert ) );

        ChkDR( Drm_LocalLicense_CreateLicense(
            pAppContext,
            &oLicensePolicyDecriptor,
            eLicenseType,
            &drmidKID,
            cbCert,
            pbCert,
            NULL,
            &hLicenseHandle ) );
    }
    else
    {
        ChkDR( Drm_LocalLicense_CreateLicense(
            pAppContext,
            &oLicensePolicyDecriptor,
            eLicenseType,
            &drmidKID,
            0,
            NULL,
            NULL,
            &hLicenseHandle ) );
    }

    /* Tx: Shove the created license into the PRND Rx store, NST, EST, or HDS depending on parameter */
    if( fForceLicenseInPRNDRxStore )
    {
        DRM_DWORD  cbLicenseToStore = 0;
        DRM_BYTE  *pbLicenseBuffer  = NULL;
        const DRM_LOCAL_LICENSE_SESSION_CONTEXT *poLocalLicenseContext = (const DRM_LOCAL_LICENSE_SESSION_CONTEXT *)hLicenseHandle;
        ChkDR( DRM_DWordAdd( poLocalLicenseContext->cbXMRLicense, SIZEOF( DRM_DWORD ), &cbLicenseToStore ) );
        ChkMem( pbLicenseToStore = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbLicenseToStore ) );
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbLicenseToStore, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)&pbLicenseBuffer ) );
        MEMCPY( pbLicenseBuffer, poLocalLicenseContext->pbXMRLicense, poLocalLicenseContext->cbXMRLicense );
        ChkDR( DRM_LST_AddLicense(
            &((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oLSTContextPRNDRxStore,
            poLocalLicenseContext->cbXMRLicense,
            pbLicenseToStore,
            &poLocalLicenseContext->kid,
            &poLocalLicenseContext->lid,
            0 ) );
    }
    else if( fCannotPersistInNST )
    {
        DRM_DWORD  cbLicenseToStore = 0;
        DRM_BYTE  *pbLicenseBuffer  = NULL;
        const DRM_LOCAL_LICENSE_SESSION_CONTEXT *poLocalLicenseContext = (const DRM_LOCAL_LICENSE_SESSION_CONTEXT *)hLicenseHandle;
        ChkDR( DRM_DWordAdd( poLocalLicenseContext->cbXMRLicense, SIZEOF( DRM_DWORD ), &cbLicenseToStore ) );
        ChkMem( pbLicenseToStore = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbLicenseToStore ) );
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbLicenseToStore, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)&pbLicenseBuffer ) );
        MEMCPY( pbLicenseBuffer, poLocalLicenseContext->pbXMRLicense, poLocalLicenseContext->cbXMRLicense );
        ChkDR( DRM_LST_AddLicense(
            &((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oLicStoreContextNST,
            poLocalLicenseContext->cbXMRLicense,
            pbLicenseToStore,
            &poLocalLicenseContext->kid,
            &poLocalLicenseContext->lid,
            0 ) );

    }
    else if( fCannotPersistInEST )
    {
        ChkDR( Drm_LocalLicense_CreateRMHeader( &drmidKID, &cbHeader, &pbHeader ) );
        ChkDR( Drm_LocalLicense_CreatePlayReadyObject( hLicenseHandle, cbHeader, pbHeader, &cbPROHeader, &pbPROHeader ) );

        ChkDR( Drm_Reinitialize( pAppContext ) );
        ChkDR( Drm_Content_SetProperty(
            pAppContext,
            DRM_CSP_PLAYREADY_OBJ,
            pbPROHeader,
            cbPROHeader ) );
    }
    else
    {
        ChkDR( Drm_LocalLicense_StoreLicense(
            hLicenseHandle,
            eDRM_LOCAL_LICENSE_XMR_STORE ) );
    }

    *phLicense     = hLicenseHandle;
    hLicenseHandle = DRM_LICENSE_HANDLE_INVALID;

ErrorExit:
    FREEDRMSTRING( dstrKid );
    DRM_PRND_SAFEMEMFREE( pbLicenseToStore );
    DRM_PRND_SAFEMEMFREE( pbHeader );
    DRM_PRND_SAFEMEMFREE( pbPROHeader );

    if( hLicenseHandle != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hLicenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    return dr;
}
#define SECONDSIN25HOURS   90000

static DRM_RESULT DRM_CALL _OPLCallback(
    __in const DRM_VOID                 *f_pvOutputLevelsData,
    __in       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in const DRM_VOID                 *f_pv )
{
    DRM_RESULT        dr                = DRM_SUCCESS;

    if( f_dwCallbackType == DRM_PLAY_OPL_CALLBACK )
    {
        /* do nothing */
    }
    else if( f_dwCallbackType == DRM_COPY_OPL_CALLBACK )
    {
        ChkDR( DRM_E_TEST_INVALID_OPL_CALLBACK );
    }
    else if( f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK )
    {
        /* do nothing */
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK )
    {
        /* do nothing */
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK )
    {
        const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT *pExtCallback = (const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT*)f_pvOutputLevelsData;

        if(    pExtCallback->pRestriction->wType  == XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT
            && pExtCallback->pRestriction->cbData == SIZEOF( DRM_GUID ) )
        {
            if( !g_fExpectPlayEnablerCallback )
            {
                ChkDR( DRM_E_TEST_DATA_VERIFICATION_FAILURE );
            }
            else
            {
                g_fActualPlayEnablerCallback = TRUE;
            }
        }
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK )
    {
        /* do nothing */
    }
    else if ( f_dwCallbackType == DRM_RESTRICTED_SOURCEID_CALLBACK )
    {
        /* do nothing */
    }
    else
    {
        TRACE( ( "Unknown Callback type: %u", f_dwCallbackType ) );
        ChkDR( DRM_E_TEST_INVALID_OPL_CALLBACK );
    }

ErrorExit:
    return dr;
}

#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_RebindLicense                           0x00000001
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_KIDAlreadySet                           0x00000002
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ShortExpireAfterFirstPlay               0x00000004
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ZeroExpireAfterFirstPlay                0x00000008
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ExpireLicense                           0x00000010
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ForceLicenseInPRNDRxStore               0x00000020
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_LicenseAlreadyInStore                   0x00000040
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_StartBeforeRegistration                 0x00000080
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_LicenseHasMID                           0x00000100
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_RxLeafLicenseHasExpireAfterFirstPlay    0x00000200
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ValidPlayEnabler                        0x00000400
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_InvalidPlayEnabler                      0x00000800
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_CannotPersistInNST                      0x00001000
#define TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_CannotPersistInEST                      0x00002000

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelLicenseFetch( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                                     = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                                            = NULL;
    DRM_BYTE                            *pbMsgRxToTx                                            = NULL;
    DRM_DWORD                            cbMsgRxToTx                                            = 0;
    DRM_BYTE                            *pbMsgTxToRx                                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                                            = 0;
    DRM_KID                              oKid                                                   = {0};
    DRM_PRND_CONTENT_IDENTIFIER_TYPE     eCID                                                   = DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID;
    const DRM_BYTE                      *pbCID                                                  = (const DRM_BYTE*)&oKid;
    DRM_DWORD                            cbCID                                                  = SIZEOF( oKid );
    DRM_WCHAR                            rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = {0};
    const DRM_CONST_STRING              *pdstrPlay                                              = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DECRYPT_CONTEXT                  oTxEncryptCtx                                          = {0};
    DRM_DECRYPT_CONTEXT                  oRxDecryptCtx                                          = {0};
    DRM_BOOL                             fArg0RebindLicense                                     = FALSE;
    DRM_BOOL                             fArg0KidAlreadySet                                     = FALSE;
    DRM_BOOL                             fArg0ForceLicenseInPRNDRxStore                         = FALSE;
    DRM_BOOL                             fExpireLicense                                         = FALSE;
    DRM_BOOL                             fShortExpireAfterFirstPlay                             = FALSE;
    DRM_BOOL                             fZeroExpireAfterFirstPlay                              = FALSE;
    DRM_BOOL                             fLicenseAlreadyInStore                                 = FALSE;
    DRM_BOOL                             fStartBeforeRegistration                               = FALSE;
    DRM_BOOL                             fLicenseHasMID                                         = FALSE;
    DRM_BOOL                             fRxLeafLicenseHasExpireAfterFirstPlay                  = FALSE;
    DRM_BOOL                             fValidPlayEnabler                                      = FALSE;
    DRM_BOOL                             fInvalidPlayEnabler                                    = FALSE;
    DRM_BOOL                             fCannotPersistInNST                                    = FALSE;
    DRM_BOOL                             fCannotPersistInEST                                    = FALSE;
    DRM_AES_COUNTER_MODE_CONTEXT         oAesCtrCtx                                             = {0};
    DRM_CHAR                             rgbClearData[DRM_AES_BLOCKLEN]                         = "Hello, world!!!";
    DRM_CHAR                             rgbEncryptedData[DRM_AES_BLOCKLEN]                     = {0};
    DRM_LICENSE_HANDLE                   hLicenseHandle                                         = DRM_LICENSE_HANDLE_INVALID;
    DRM_UINT64                           qwIV                                                   = DRM_UI64HL( 0, 0 );
    DRM_DWORD                            dwFlagsOut                                             = 0;
    DRM_DWORD                            dwExpirationTime                                       = DRM_LOCAL_LICENSE_INFINITE_EXPIRATION;
    DRM_DWORD                            dwExpireAfterFirstPlay                                 = 24 * 60 * 60;  /* 24 hours */
    DRM_DWORD                            dwOffsetTime                                           = 0;
    DRM_KID                              oUplinkKidToDelete                                     = {0};
    const DRM_KID                       *pUplinkKidToDelete                                     = NULL;
    DRM_LGP_MID_EXTRA_DATA               oMIDExtraData                                          = { 0 };
    DRM_DWORD                            cbMIDExtraData                                         = SIZEOF( DRM_LGP_MID_EXTRA_DATA );

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );

        fArg0RebindLicense                      = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_RebindLicense );
        fArg0KidAlreadySet                      = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_KIDAlreadySet );
        fShortExpireAfterFirstPlay              = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ShortExpireAfterFirstPlay );
        fZeroExpireAfterFirstPlay               = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ZeroExpireAfterFirstPlay );
        fExpireLicense                          = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ExpireLicense );
        fArg0ForceLicenseInPRNDRxStore          = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ForceLicenseInPRNDRxStore );
        fLicenseAlreadyInStore                  = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_LicenseAlreadyInStore );
        fStartBeforeRegistration                = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_StartBeforeRegistration );
        fLicenseHasMID                          = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_LicenseHasMID );
        fRxLeafLicenseHasExpireAfterFirstPlay   = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_RxLeafLicenseHasExpireAfterFirstPlay );
        fValidPlayEnabler                       = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_ValidPlayEnabler );
        fInvalidPlayEnabler                     = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_InvalidPlayEnabler );
        fCannotPersistInNST                     = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_CannotPersistInNST );
        fCannotPersistInEST                     = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetch_Arg0_FLAGS_CannotPersistInEST );
    }

    ChkArg( !fArg0ForceLicenseInPRNDRxStore || !fCannotPersistInNST || !fCannotPersistInEST );  /* Doesn't make sense to put the license in multiple stores. */

    GET_SHARED_APP_CONTEXT( pAppContext );

    if( fStartBeforeRegistration )
    {
        DRM_PRND_RECEIVER_CONTEXT    *pRxCtx    = NULL;
        DRM_PRND_TRANSMITTER_CONTEXT *pTxCtx    = NULL;

        MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
        MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

        ChkDR( Drm_Prnd_Receiver_BeginSession( pAppContext, pRxCtx ) );
        ChkDR( Drm_Prnd_Transmitter_BeginSession( pAppContext, pTxCtx ) );
    }

    if ( fLicenseAlreadyInStore )
    {
        DRM_BYTE         rgbKID[CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR ) ] = { 0 };
        DRM_DWORD        cbKID                                                                 = SIZEOF( rgbKID );
        DRM_CONST_STRING dstrKID                                                               = EMPTY_DRM_STRING;

        ChkDR( Drm_Content_GetProperty( pAppContext, DRM_CGP_HEADER_KID, rgbKID, &cbKID ) );
        DSTR_FROM_PB( &dstrKID, rgbKID, cbKID );
        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &oKid ) );
    }
    else
    {
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );
    }

    if( fZeroExpireAfterFirstPlay )
    {
        dwExpireAfterFirstPlay = 0;
    }
    else if( fShortExpireAfterFirstPlay )
    {
        dwExpireAfterFirstPlay = DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY + 40;
    }

    if( fExpireLicense )
    {
        dwExpirationTime = DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY + 20;
    }

    /* Rx: Send License Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    dr = Drm_Prnd_Receiver_LicenseRequest_Generate(
        pAppContext,
        &DRM_ACTION_PLAY,
        NULL,
        eCID,
        pbCID,
        cbCID,
        &g_badCustomDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx );

    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_INVALID_CUSTOM_DATA_TYPE );

    ChkDR( Drm_Prnd_Receiver_LicenseRequest_Generate(
        pAppContext,
        &DRM_ACTION_PLAY,
        NULL,
        eCID,
        pbCID,
        cbCID,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST );

    /* Tx: Process License Request */
    ChkDR( Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,                   /* Test purposes only - real world caller would pass their own context */
        NULL,
        NULL,
        &dwFlagsOut ) );

    {
        DRM_DWORD   cchKid  = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
        ChkDR( DRM_B64_EncodeW(
            (const DRM_BYTE*)&oKid,
            SIZEOF( oKid ),
            rgwchKid,
            &cchKid,
            0 ) );
        rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;
    }

    /*
    ** Tx: Make sure the KID is set.  If it's already set (on Tx), use Drm_Reinitialize before setting it.
    */
    if( fArg0KidAlreadySet )
    {
        ChkDR( Drm_Reinitialize( pAppContext ) );
    }

    if ( !fLicenseAlreadyInStore )
    {
        ChkDR( Drm_Content_SetProperty(
            pAppContext,
            DRM_CSP_KID,
            (const DRM_BYTE*)rgwchKid,
            SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */
    }

    if( !fArg0RebindLicense )
    {
        DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR oLicensePolicyDecriptor = {0};

        ChkArg( !fArg0ForceLicenseInPRNDRxStore );  /* Only supported for rebinding */
        ChkArg( !fCannotPersistInNST );             /* Only supported for rebinding */
        ChkArg( !fCannotPersistInEST );             /* Only supported for rebinding */

        /* Tx: Use the local license generation to create REMOTE_BOUND_SIMPLE license */
        ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

        oLicensePolicyDecriptor.wSecurityLevel   = 150;
        oLicensePolicyDecriptor.fCannotPersist   = TRUE;

        ChkDR( Drm_LocalLicense_CreateLicense(
            pAppContext,
           &oLicensePolicyDecriptor,
            eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
           &oKid,
            0,
            NULL,
            NULL,
           &hLicenseHandle ) );

        ChkDR( Drm_LocalLicense_StoreLicense(
            hLicenseHandle,
            eDRM_LOCAL_LICENSE_PRND_SESSION_STORE ) );

    }
    else
    {
        if ( !fLicenseAlreadyInStore )
        {
            /*
            ** Tx: Add a license to the store.
            */
            ChkDR( _AddLicense(
                pAppContext,
                rgwchKid,
                dwExpirationTime,
                dwExpireAfterFirstPlay,
                fArg0ForceLicenseInPRNDRxStore,
                fValidPlayEnabler,
                fInvalidPlayEnabler,
                fCannotPersistInNST,
                fCannotPersistInEST,
                DRM_LICENSE_HANDLE_INVALID,
                fCannotPersistInEST ? eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE : eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,   /* EST must have "remote-bound" license */
                &hLicenseHandle ) );
        }
        else
        {
            ChkArg( !fCannotPersistInNST );  /* Only supported for newly generated licenses */
            ChkArg( !fCannotPersistInEST );  /* Only supported for newly generated licenses */
        }

        /*
        ** Tx: Bind to an existing license on the transmitter.  Assumption: Content being streamed starts out as PlayReady-encrypted content.
        ** Note: In the real world, we'd throw oTxEncryptCtx away in this scenario, as we don't need to actually decrypt on transmitter side.
        ** However, for test purposes, we're going to ENCRYPT using that context.
        ** This is valid because AES CTR is a symmetric operation, i.e. decrypt == encrypt.
        */
        ChkDR( Drm_Reader_Bind(
            pAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            &oTxEncryptCtx ) );

        /*
        ** Get the MID now; before the Rx calls Drm_Reader_Bind and appropriately ignores it.
        ** We need this MID later, when we verify that committing the license on the Rx
        ** did not put anything in the receiver's metering store.
        */
        if ( fLicenseHasMID )
        {
            DRM_DWORD  dwOutputData = 0;

            ChkDR( Drm_License_GetProperty(  pAppContext,
                                             DRM_LGP_MID,
                                (DRM_BYTE *)&oMIDExtraData,
                                            &cbMIDExtraData,
                                            &dwOutputData ) );
            ChkBOOL( dwOutputData == 1, DRM_E_TEST_UNEXPECTED_OUTPUT )
        }

        if( ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->dwChainDepth > 1 )
        {
            MEMCPY( &oUplinkKidToDelete, &((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oFFLicense.rgkid[1], SIZEOF( DRM_KID ) );
            pUplinkKidToDelete = &oUplinkKidToDelete;
        }

        /*
        ** Tx: Rebind the license to the receiver.
        */
        if( fArg0ForceLicenseInPRNDRxStore || fCannotPersistInEST )
        {
            dr = Drm_Prnd_Transmitter_PrepareLicensesForTransmit( pAppContext );
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_CANNOT_REBIND_PRND_RECEIVED_LICENSE );
            goto ErrorExit;     /* Done! */
        }
        else
        {
            ChkDR( Drm_Prnd_Transmitter_PrepareLicensesForTransmit( pAppContext ) );
        }
    }

    /*
    ** Tx: Generate license transmit message.  The license(s) from if/else above will be included because they are in the PRND store.
    */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    dr = Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        pAppContext,
        &g_badCustomDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx );

    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_INVALID_CUSTOM_DATA_TYPE );

    ChkDR( Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        pAppContext,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_LICENSE_TRANSMIT );

    /*
    ** Tx: Encrypt some content using oTxEncryptCtx.
    ** Note: In the real world, we'd only need to do encryption in the "new license" case (else clause above),
    ** but for test purposes we don't have an actual encrypted file here so we'll just fake it by encrypting some data.
    ** We can call Drm_Reader_Decrypt because AES CTR is a symmetric operation, i.e. decrypt == encrypt.
    */
    DRMCRT_memcpy( rgbEncryptedData, rgbClearData, SIZEOF( rgbEncryptedData ) );
    ZEROMEM( &oAesCtrCtx, SIZEOF( oAesCtrCtx ) );

    if( !fArg0RebindLicense )
    {
        /* Tx: Call the licgen encryptor function to encrypt */
        DRM_DWORD  rgbSubsampleCount[1]   = {0};
        DRM_BYTE  *rgbSubsamplePointer[1] = {0};

        rgbSubsampleCount[0]   = SIZEOF( rgbEncryptedData );
        rgbSubsamplePointer[0] = (DRM_BYTE*)rgbEncryptedData;

        ChkDR( Drm_LocalLicense_EncryptSample(
            hLicenseHandle,
            1,
            rgbSubsampleCount,
            rgbSubsamplePointer,
           &qwIV ) );
    }
    else
    {
        ChkDR( Drm_Reader_Decrypt(
            &oTxEncryptCtx,
            &oAesCtrCtx,
            (DRM_BYTE*)rgbEncryptedData,
            SIZEOF( rgbEncryptedData ) ) );

        ChkDR( Drm_Reader_Commit(
            pAppContext,
            NULL,
            NULL ) );
    }

    /*
    ** Rx: Process License Transmit
    ** Note: In the real world, this method will return DRM_E_PRND_MESSAGE_WRONG_TYPE if the message is an error message instead.
    */
    ChkDR( Drm_Prnd_Receiver_LicenseTransmit_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        NULL,                   /* Test purposes only - real world caller would pass their own context */
        &dwFlagsOut ) );

    if( fArg0RebindLicense )
    {
        /*
        ** Tx: For test purposes only, DELETE the license from the HDS so only the transmitted license is available.
        ** We only have to do this because this test case is using the same HDS for Tx and Rx.
        */
        ChkDR( Drm_StoreMgmt_DeleteLicenses( pAppContext, NULL, NULL ) );

        if( pUplinkKidToDelete != NULL )
        {
            DRM_LICSTOREENUM_CONTEXT oCtx = {0};
            DRM_LID                  lid  = {0};

            ChkDR( DRM_LST_InitEnum(
                &(( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oLicStoreContextXMR,
                pUplinkKidToDelete,
                FALSE,
                &oCtx ) );
            ChkDR( DRM_LST_EnumNext( &oCtx, NULL, &lid, NULL, NULL ) );
            ChkDR( DRM_LST_EnumDelete( &oCtx ) );
        }

        if( fCannotPersistInNST || fCannotPersistInEST )
        {
            /*
            ** Tx: Call Drm_Reinitialize and re-call Drm_Content_SetProperty with the KID so the license won't be found in the NST or EST.
            ** We only have to do this because this test case is using the same pAppContext for Tx and Rx.
            */
            ChkDR( Drm_Reinitialize( pAppContext ) );
            ChkDR( Drm_Content_SetProperty(
                pAppContext,
                DRM_CSP_KID,
                (const DRM_BYTE*)rgwchKid,
                SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */
        }

        ChkDR( TEST_Hal_PurgeKeyHandleCache( &(( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext, OEM_HAL_KEY_TYPE_AES_ECB_ROOT ) );
        ChkDR( TEST_Hal_PurgeKeyHandleCache( &(( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext, OEM_HAL_KEY_TYPE_AES_CTR_CONTENT ) );
    }

    /*
    ** Rx: Bind to the license received in Drm_Prnd_Receiver_LicenseTransmit_Process.
    */
    Log( "", "Rx Calling Drm_Reader_Bind()" );

    if( fValidPlayEnabler )
    {
        g_fExpectPlayEnablerCallback = TRUE;
    }

    g_fActualPlayEnablerCallback = FALSE;
    ChkDR( Drm_Reader_Bind(
        pAppContext,
        &pdstrPlay,
        1,
        _OPLCallback,
        NULL,
        &oRxDecryptCtx ) );

    /* If we expect PlayEnabler, then OPLCallBack needs to set the g_fActualPlayEnablerCallback to true */
    if( g_fExpectPlayEnablerCallback )
    {
        ChkBOOL( g_fActualPlayEnablerCallback, DRM_E_TEST_UNEXPECTED_OUTPUT );
        g_fExpectPlayEnablerCallback = FALSE;
        g_fActualPlayEnablerCallback = FALSE;
    }

    /*
    ** Rx: Verify that Expire After First Play in a leaf license is ignored
    */
    if ( fRxLeafLicenseHasExpireAfterFirstPlay )
    {
        tChangeSystemTime( SECONDSIN25HOURS );

        Drm_Reader_Close( &oRxDecryptCtx );

        Log( "", "Rx Calling Drm_Reader_Bind() a second time" );
        ChkDR( Drm_Reader_Bind(
            pAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            &oRxDecryptCtx ) );

        tChangeSystemTime( -SECONDSIN25HOURS );
    }

    /*
    ** Rx: Decrypt content for playback.
    */
    ZEROMEM( &oAesCtrCtx, SIZEOF( oAesCtrCtx ) );

    if( !fArg0RebindLicense )
    {
        /* Rx: In of the local licgen, we need to set initial vector */
        oAesCtrCtx.qwInitializationVector = qwIV;
    }

    ChkDR( Drm_Reader_Decrypt(
        &oRxDecryptCtx,
        &oAesCtrCtx,
        (DRM_BYTE*)rgbEncryptedData,
        SIZEOF( rgbEncryptedData ) ) );

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

    /*
    ** Rx: There should never be any metering data in the receiver's HDS
    **     This check must come after Drm_Reader_Commit()
    */
    if ( fLicenseHasMID )
    {
        DRM_SUBSTRING            dasstrMID             = EMPTY_DRM_SUBSTRING;
        DRM_MID                  oMID                  = { 0 };
        DRM_DWORD                cbMID                 = SIZEOF( DRM_MID );
        DRM_METERINGENUM_CONTEXT oMeteringEnumContext  = { 0 };
        DRM_KID                  oKID                  = { 0 };
        DRM_DWORD                cbData                = 0;

        dasstrMID.m_cch = ( cbMIDExtraData / 2 );
        dasstrMID.m_ich = 0;
        ChkDR( DRM_B64_DecodeA((DRM_CHAR *)&oMIDExtraData,
                                           &dasstrMID,
                                           &cbMID,
                              ( DRM_BYTE *)&oMID,
                                            0 ) );
        ChkDR( DRM_MTR_InitEnum( &oMID,
                                 &oMeteringEnumContext,
                                 &((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oDSTContextPRNDRxStore ) );

        dr = DRM_MTR_EnumNext( &oMeteringEnumContext, &oKID, &cbData );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_NOMORE );
    }

    /*
    ** Rx: For test purposes only, verify the decrypted content matches what we originally started with.
    */
    ChkBOOL( 0 == DRMCRT_memcmp( rgbClearData, rgbEncryptedData, SIZEOF( rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    if( fArg0RebindLicense && !fLicenseAlreadyInStore )
    {
        if( fZeroExpireAfterFirstPlay )
        {
            /* after 20 seconds, bind will still success because we change expiration time to current time + DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY */
            ChkDR( tChangeSystemTime( 20 ) );

            Drm_Reader_Close( &oRxDecryptCtx );

            ChkDR( Drm_Reader_Bind(
                pAppContext,
                &pdstrPlay,
                1,
                NULL,
                NULL,
                &oRxDecryptCtx ) );

            ChkDR( tChangeSystemTime( DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY + DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS + 1 ) );
            dwOffsetTime = DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY + DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS + 1;
        }
        else if( fExpireLicense )
        {
            /*
            ** if fExpireLicense and fShortExpireAfterFirstPlay are set, we should still expire
            ** with the shorter time which is dwExpirationTime
            */
            DRM_DWORD dw = (DRM_DWORD)( dwExpirationTime + DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS + 1 );
            ChkDR( tChangeSystemTime( (DRM_LONG)dw ) );
            dwOffsetTime = dw;
        }
        else
        {
            /* both short expiration and regular long expiration is the same */
            DRM_DWORD dw = (DRM_DWORD)( dwExpireAfterFirstPlay + DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS + 1 );
            ChkDR( tChangeSystemTime( (DRM_LONG)dw ) );
            dwOffsetTime  = dw;
        }

        Drm_Reader_Close( &oRxDecryptCtx );

        dr = Drm_Reader_Bind(
            pAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            &oRxDecryptCtx );

        if( fCannotPersistInNST || fCannotPersistInEST )
        {
            /* Licenses in EST or NST don't have any expiration */
            ChkDR( dr );
        }
        else
        {
            /* Licenses NOT in EST or NST are expired now */
            DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICENSE_EXPIRED );
        }
    }

ErrorExit:
    Drm_Reader_Close( &oTxEncryptCtx );
    Drm_Reader_Close( &oRxDecryptCtx );

    if( hLicenseHandle != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hLicenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    /* reset the time to the beginning of function */
    tChangeSystemTime( (DRM_LONG)( (DRM_LONG)-1 * (DRM_LONG)dwOffsetTime ) );

    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    return dr;
}

/*****************************************************************************
** Arguments:
**      None
******************************************************************************/
DRM_RESULT DRM_CALL TestMalformedPRNDTopLevelLicenseFetch( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                                     = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                                            = NULL;
    DRM_BYTE                            *pbMsgRxToTx                                            = NULL;
    DRM_DWORD                            cbMsgRxToTx                                            = 0;
    DRM_BYTE                            *pbMsgTxToRx                                            = NULL;
    DRM_DWORD                            cbMsgTxToRx                                            = 0;
    DRM_KID                              oKid                                                   = {0};
    DRM_PRND_CONTENT_IDENTIFIER_TYPE     eCID                                                   = DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID;
    const DRM_BYTE                      *pbCID                                                  = (const DRM_BYTE*)&oKid;
    DRM_DWORD                            cbCID                                                  = SIZEOF( oKid );
    DRM_LICENSE_HANDLE                   hLicenseHandle                                         = DRM_LICENSE_HANDLE_INVALID;
    DRM_DWORD                            dwFlagsOut                                             = 0;
    DRM_DWORD                            dwTotalLen                                             = 0;
    DRM_DWORD                            dwBasicInfoLen                                         = 0;
    DRM_DWORD                            dwRequestInfoLen                                       = 0;
    DRM_DWORD                            dwCustomDataLen                                        = 0;
    DRM_DWORD                            dwSigLen                                               = 0;
    DRM_ID                               sessionID                                              = {0};
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                                 = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                                 = NULL;
    DRM_DWORD                            dwSig                                                  = 0;

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );

    /* Rx: Send License Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    ChkDR( Drm_Prnd_Receiver_LicenseRequest_Generate(
        pAppContext,
        &DRM_ACTION_PLAY,
        NULL,
        eCID,
        pbCID,
        cbCID,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST );

    /* Calculate different lengths based on the message data */
    NETWORKBYTES_TO_DWORD( dwTotalLen,       pbMsgRxToTx, TEST_LICENSE_REQUEST_HEADER_LEN_OFFSET );
    NETWORKBYTES_TO_DWORD( dwBasicInfoLen,   pbMsgRxToTx, TEST_LICENSE_REQUEST_BASIC_INFO_LEN_OFFSET );
    NETWORKBYTES_TO_DWORD( dwRequestInfoLen, pbMsgRxToTx, TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen + TEST_LICENSE_REQUEST_INFO_LEN_OFFSET_AFTER_BASIC_INFO );
    NETWORKBYTES_TO_DWORD( dwCustomDataLen,  pbMsgRxToTx, TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen + dwRequestInfoLen + TEST_LICENSE_REQUEST_CUSTOMDATA_LEN_OFFSET_AFTER_REQUEST_INFO );
    NETWORKBYTES_TO_DWORD( dwSigLen,         pbMsgRxToTx, TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET_AFTER_CUSTOMDATA );

    ChkBOOL( dwTotalLen == TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + dwSigLen, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Save the first SIZEOF(DWORD) bytes of the signature so we can restore it later */
    NETWORKBYTES_TO_DWORD(
        dwSig,
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET );

    /* Change the first 4 bytes of signature randomly */
    ChkDR( Oem_Random_GetBytes(
        NULL,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET,
        SIZEOF( DRM_DWORD ) ) );

    /* Tx: Process License Request and fail with invalid signature */
    dr = Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,
        NULL,
        NULL,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_INVALID_SIGNATURE );

    /* Reset the signature to the original byte value */
    DWORD_TO_NETWORKBYTES(
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET,
        dwSig );

    /* Save session id */
    DRMCRT_memcpy(
        (DRM_BYTE*)&sessionID,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_BASIC_INFO_SESSION_ID_OFFSET,
        SIZEOF( DRM_ID ) );

    /* Generate random session id */
    ChkDR( Oem_Random_GetBytes(
        NULL,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_BASIC_INFO_SESSION_ID_OFFSET,
        SIZEOF( DRM_ID ) ) );

    ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
        pRxCtx->pBBXContext,
        pRxCtx->oPrndReceiverSessionContext.handleMIMK,
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET ) );

    /* Tx: Process License Requestion failed with invalid session id */
    dr = Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,
        NULL,
        NULL,
        &dwFlagsOut );
     DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_SESSION_ID_INVALID );

    /* Reset session id */
    DRMCRT_memcpy(
        pbMsgRxToTx + TEST_LICENSE_REQUEST_BASIC_INFO_SESSION_ID_OFFSET,
        (DRM_BYTE*)&sessionID,
        SIZEOF( DRM_ID ) );

    /* Rx: Set PRND protocol version to 0 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_LICENSE_REQUEST_PRND_PROTOCOL_VERSION_OFFSET, 0 );
    ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
        pRxCtx->pBBXContext,
        pRxCtx->oPrndReceiverSessionContext.handleMIMK,
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET ) );

    dr = Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,
        NULL,
        NULL,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /* Rx: Set PRND protocol version to DRM_PRND_CURRENT_VERSION + 1 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_LICENSE_REQUEST_PRND_PROTOCOL_VERSION_OFFSET,  DRM_PRND_CURRENT_VERSION + 1 );
    ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
        pRxCtx->pBBXContext,
        pRxCtx->oPrndReceiverSessionContext.handleMIMK,
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET ) );

    dr = Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,
        NULL,
        NULL,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_MESSAGE_VERSION_INVALID );

    /* Rx: Reset PRND protocol version */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_LICENSE_REQUEST_PRND_PROTOCOL_VERSION_OFFSET,  DRM_PRND_CURRENT_VERSION );

    /* Change Request Action */
    WORD_TO_NETWORKBYTES(
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  TEST_LICENSE_REQUEST_INFO_ACTION_OFFSET_AFTER_BASIC_INFO,
        (DRM_WORD)dwSig );
    ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
        pRxCtx->pBBXContext,
        pRxCtx->oPrndReceiverSessionContext.handleMIMK,
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET ) );

    dr = Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,
        NULL,
        NULL,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_PRND_LICENSE_REQUEST_INVALID_ACTION );

    /* Reset Action */
    WORD_TO_NETWORKBYTES(
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  TEST_LICENSE_REQUEST_INFO_ACTION_OFFSET_AFTER_BASIC_INFO,
        (DRM_WORD)DRM_PRND_ACTION_PLAY );

    /* Set RevInfo to 0 */
    DWORD_TO_NETWORKBYTES( pbMsgRxToTx, TEST_LICENSE_REQUEST_BASIC_INFO_RIV_OFFSET, 0 );
    ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
        pRxCtx->pBBXContext,
        pRxCtx->oPrndReceiverSessionContext.handleMIMK,
        pbMsgRxToTx,
        TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen,
        pbMsgRxToTx + TEST_LICENSE_REQUEST_HEADER_SIZE + dwBasicInfoLen +  dwRequestInfoLen + dwCustomDataLen + TEST_LICENSE_REQUEST_SIGNATURE_OFFSET ) );

    ChkDR( Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,
        NULL,
        NULL,
        &dwFlagsOut ) );

    /* Tx: Use the local license generation to create REMOTE_BOUND_SIMPLE license */
    {
        DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR oLicensePolicyDecriptor = {0};

        ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

        oLicensePolicyDecriptor.wSecurityLevel   = 150;
        oLicensePolicyDecriptor.fCannotPersist   = TRUE;

        ChkDR( Drm_LocalLicense_CreateLicense(
            pAppContext,
           &oLicensePolicyDecriptor,
            eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
           &oKid,
            0,
            NULL,
            NULL,
           &hLicenseHandle ) );

        ChkDR( Drm_LocalLicense_StoreLicense(
            hLicenseHandle,
            eDRM_LOCAL_LICENSE_PRND_SESSION_STORE ) );
    }

    /* Tx: Generate license transmit message */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    ChkDR( Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        pAppContext,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_LICENSE_TRANSMIT );

    /* Verify the response message contains rev info */
    {
        DRM_DWORD dwRevInfoLen = 0;

        NETWORKBYTES_TO_DWORD( dwRevInfoLen, pbMsgTxToRx, TEST_LICENSE_TRANSMIT_RIV_LEN_OFFSET );
        ChkBOOL( dwRevInfoLen > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* Rx: Process License Transmit */
    ChkDR( Drm_Prnd_Receiver_LicenseTransmit_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        NULL,
        &dwFlagsOut ) );

ErrorExit:
    if( hLicenseHandle != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hLicenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    return dr;
}

#define TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_ExpireRoot               0x00000001
#define TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_ExpireLeaf               0x00000002
#define TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_BindOnlyRoot             0x00000004

/* Root license expires faster than leaf */
#define TestPRNDTopLevelLicenseFetchChainedLicenseRebind_ExpireRoot_Time                     30
/* Leaf license's expiration date is adjusted for the clock drift, therefore we are doing it here to be in sync with root license */
#define TestPRNDTopLevelLicenseFetchChainedLicenseRebind_ExpireLeaf_Time                     ( 60 + DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS )

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelLicenseFetchChainedLicenseRebind( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                                         = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                                                = NULL;
    DRM_BYTE                            *pbMsgRxToTx                                                = NULL;
    DRM_DWORD                            cbMsgRxToTx                                                = 0;
    DRM_BYTE                            *pbMsgTxToRx                                                = NULL;
    DRM_DWORD                            cbMsgTxToRx                                                = 0;
    DRM_KID                              oKid                                                       = {0};
    DRM_PRND_CONTENT_IDENTIFIER_TYPE     eCID                                                       = DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID;
    const DRM_BYTE                      *pbCID                                                      = (const DRM_BYTE*)&oKid;
    DRM_DWORD                            cbCID                                                      = SIZEOF( oKid );
    DRM_WCHAR                            rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ]     = {0};
    DRM_WCHAR                            rgwchRootKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = {0};
    DRM_DWORD                            cchKid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    const DRM_CONST_STRING              *pdstrPlay                                                  = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DECRYPT_CONTEXT                  oTxEncryptCtx                                              = {0};
    DRM_DECRYPT_CONTEXT                  oRxDecryptCtx                                              = {0};
    DRM_BOOL                             fExpireRoot                                                = FALSE;
    DRM_BOOL                             fExpireLeaf                                                = FALSE;
    DRM_BOOL                             fBindOnlyRoot                                              = FALSE;
    DRM_AES_COUNTER_MODE_CONTEXT         oAesCtrCtx                                                 = {0};
    DRM_CHAR                             rgbClearData[DRM_AES_BLOCKLEN]                             = "Hello, world!!!";
    DRM_CHAR                             rgbEncryptedData[DRM_AES_BLOCKLEN]                         = {0};
    DRM_LICENSE_HANDLE                   hRootLicense                                               = DRM_LICENSE_HANDLE_INVALID;
    DRM_LICENSE_HANDLE                   hLeafLicense                                               = DRM_LICENSE_HANDLE_INVALID;
    DRM_UINT64                           qwIV                                                       = DRM_UI64HL( 0, 0 );
    DRM_DWORD                            dwFlagsOut                                                 = 0;
    DRM_DWORD                            dwOffsetTime                                               = 0;
    DRM_KID                              oRootKid                                                   = {0};
    DRM_RESULT                           drExpected                                                 = DRM_SUCCESS;
    DRM_DWORD                            rgbSubsampleCount[1]                                       = {0};
    DRM_BYTE                            *rgbSubsamplePointer[1]                                     = {0};

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );

        fExpireRoot     = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_ExpireRoot );
        fExpireLeaf     = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_ExpireLeaf );
        fBindOnlyRoot   = 0 != ( dwArg0Flags & TestPRNDTopLevelLicenseFetchChainedLicenseRebind_Arg0_FLAGS_BindOnlyRoot );
    }

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );
    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oRootKid, SIZEOF( oRootKid ) ) );

    /* Rx: Send License Request */
    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    ChkDR( Drm_Prnd_Receiver_LicenseRequest_Generate(
        pAppContext,
        &DRM_ACTION_PLAY,
        NULL,
        eCID,
        pbCID,
        cbCID,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );
    VERIFY_MESSAGE_TYPE( pbMsgRxToTx, cbMsgRxToTx, DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST );

    /* Tx: Process License Request */
    ChkDR( Drm_Prnd_Transmitter_LicenseRequest_Process(
        pAppContext,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        NULL,                   /* Test purposes only - real world caller would pass their own context */
        NULL,
        NULL,
        &dwFlagsOut ) );

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&oKid,
        SIZEOF( oKid ),
        rgwchKid,
        &cchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&oRootKid,
        SIZEOF( oRootKid ),
        rgwchRootKid,
        &cchKid,
        0 ) );
    rgwchRootKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_KID,
        (const DRM_BYTE*)rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */

    /* Tx: Add root and leaf license to the store */
    ChkDR( _AddLicense(
        pAppContext,
        rgwchRootKid,
        fExpireRoot ? TestPRNDTopLevelLicenseFetchChainedLicenseRebind_ExpireRoot_Time : DRM_LOCAL_LICENSE_INFINITE_EXPIRATION,
        100000,         /* Some big number for ExpireAfterFirstPlay */
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        DRM_LICENSE_HANDLE_INVALID,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT,
        &hRootLicense ) );

    ChkDR( _AddLicense(
        pAppContext,
        rgwchKid,
        fExpireLeaf ? TestPRNDTopLevelLicenseFetchChainedLicenseRebind_ExpireLeaf_Time : DRM_LOCAL_LICENSE_INFINITE_EXPIRATION,
        100000,        /* Some big number for ExpireAfterFirstPlay */
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        hRootLicense,
        eDRM_LOCAL_LICENSE_LEAF,
        &hLeafLicense ) );

    ChkDR( Drm_Reader_Bind(
        pAppContext,
        &pdstrPlay,
        1,
        NULL,
        NULL,
        &oTxEncryptCtx ) );

    if( fBindOnlyRoot )
    {
        DRM_DECRYPT_CONTEXT oTmpCtx = {0};

        /* Set the KID to the root KID. */
        ChkDR( Drm_Reinitialize( pAppContext ) );
        ChkDR( Drm_Content_SetProperty(
            pAppContext,
            DRM_CSP_KID,
            (const DRM_BYTE*)rgwchRootKid,
            SIZEOF( rgwchRootKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */

        /* Attempt to bind to the root with a decryption context - should fail with DRM_E_LIC_KEY_DECODE_FAILURE */
        dr = Drm_Reader_Bind(
            pAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            &oTmpCtx );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LIC_KEY_DECODE_FAILURE );

        /* Bind to just the root.  This will cause only the root to be sent to the Rx. */
        ChkDR( Drm_Reader_Bind(
            pAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            NULL ) );
    }

    ChkDR( Drm_Prnd_Transmitter_PrepareLicensesForTransmit( pAppContext ) );

    /*
    ** Tx: Generate license transmit message.
    */
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    ChkDR( Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        pAppContext,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_LICENSE_TRANSMIT );

    /* Tx: Encrypt some content using oTxEncryptCtx. */
    DRMCRT_memcpy( rgbEncryptedData, rgbClearData, SIZEOF( rgbEncryptedData ) );
    ZEROMEM( &oAesCtrCtx, SIZEOF( oAesCtrCtx ) );

    rgbSubsampleCount[0]   = SIZEOF( rgbEncryptedData );
    rgbSubsamplePointer[0] = (DRM_BYTE*)rgbEncryptedData;

    ChkDR( Drm_LocalLicense_EncryptSample(
        hLeafLicense,
        1,
        rgbSubsampleCount,
        rgbSubsamplePointer,
       &qwIV ) );

    /* Rx: Process License Transmit */
    ChkDR( Drm_Prnd_Receiver_LicenseTransmit_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        NULL,
        &dwFlagsOut ) );

    /*
    ** Tx: For test purposes only, DELETE the root license from the HDS so only the transmitted license is available.
    ** We only have to do this because this test case is using the same HDS for Tx and Rx.
    */
    ChkDR( DRM_LST_DeleteLicense(
        &(( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oLicStoreContextXMR,
        &oRootKid,
        &((DRM_LOCAL_LICENSE_SESSION_CONTEXT*)hRootLicense)->lid,
        NULL ) );

    if( fBindOnlyRoot )
    {
        /*
        ** Set the KID back to the leaf KID.
        ** We will find the leaf license in the HDS since Rx/Tx share it. Real DVR scenario would find the leaf embedded in content.
        ** Thus, we won't delete the leaf license from the HDS.
        ** The root must still be found in Rx PRND store even though we called Drm_Reinitialize (for key rotation scenario).
        */
        ChkDR( Drm_Reinitialize( pAppContext ) );
        ChkDR( Drm_Content_SetProperty(
            pAppContext,
            DRM_CSP_KID,
            (const DRM_BYTE*)rgwchKid,
            SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */
    }
    else
    {
        /*
        ** Delete the leaf license as well.  Both leaf and root should be found in Rx PRND store.
        ** Refer to comments around "Set the KID back to the leaf KID." for why we don't do this if fBindOnlyRoot is set.
        */
        ChkDR( DRM_LST_DeleteLicense(
            &(( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oLicStoreContextXMR,
            &oKid,
            &((DRM_LOCAL_LICENSE_SESSION_CONTEXT*)hLeafLicense)->lid,
            NULL ) );
    }

    /*
    ** Rx: Bind to the license received in Drm_Prnd_Receiver_LicenseTransmit_Process.
    */
    ChkDR( Drm_Reader_Bind(
        pAppContext,
        &pdstrPlay,
        1,
        NULL,
        NULL,
        &oRxDecryptCtx ) );

    /*
    ** Rx: Decrypt content for playback.
    */
    ZEROMEM( &oAesCtrCtx, SIZEOF( oAesCtrCtx ) );

    /* Rx: In of the local licgen, we need to set initial vector */
    oAesCtrCtx.qwInitializationVector = qwIV;

    ChkDR( Drm_Reader_Decrypt(
        &oRxDecryptCtx,
        &oAesCtrCtx,
        (DRM_BYTE*)rgbEncryptedData,
        SIZEOF( rgbEncryptedData ) ) );

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

    /*
    ** Rx: For test purposes only, verify the decrypted content matches what we originally started with.
    */
    ChkBOOL( 0 == DRMCRT_memcmp( rgbClearData, rgbEncryptedData, SIZEOF( rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    if( fExpireRoot )
    {
        dwOffsetTime = TestPRNDTopLevelLicenseFetchChainedLicenseRebind_ExpireRoot_Time + DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS + 1;
        drExpected   = DRM_E_UPLINK_LICENSE_NOT_FOUND;
    }
    else if( fExpireLeaf )
    {
        dwOffsetTime = TestPRNDTopLevelLicenseFetchChainedLicenseRebind_ExpireLeaf_Time + 1;
        drExpected   = DRM_E_LICENSE_EXPIRED;
    }
    else
    {
        drExpected = DRM_SUCCESS;
    }

    ChkDR( tChangeSystemTime( (DRM_LONG) dwOffsetTime ) );
    Drm_Reader_Close( &oRxDecryptCtx );

    ZEROMEM( &oAesCtrCtx, SIZEOF( oAesCtrCtx ) );

    dr = Drm_Reader_Bind(
        pAppContext,
        &pdstrPlay,
        1,
        NULL,
        NULL,
        &oRxDecryptCtx );

    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, drExpected );

    if( fBindOnlyRoot )
    {
        DRM_DECRYPT_CONTEXT oTmpCtx = {0};

        /*
        ** Verify that Drm_Reinitialize does NOT wipe the Rx PRND store (for key rotation scenario).
        */
        ChkDR( Drm_Reinitialize( pAppContext ) );
        ChkDR( Drm_Content_SetProperty(
            pAppContext,
            DRM_CSP_KID,
            (const DRM_BYTE*)rgwchKid,
            SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */

        DRMASSERT( drExpected == DRM_SUCCESS );     /* This test code doesn't currently support combining fBindOnlyRoot with expiration scenarios, hence this assert. */

        ChkDR( Drm_Reader_Bind(
            pAppContext,
            &pdstrPlay,
            1,
            NULL,
            NULL,
            &oTmpCtx ) );
        Drm_Reader_Close( &oTmpCtx );
    }

ErrorExit:
    Drm_Reader_Close( &oTxEncryptCtx );
    Drm_Reader_Close( &oRxDecryptCtx );

    if( hLeafLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hLeafLicense );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    if( hRootLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hRootLicense );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    /* reset the time to the beginning of function */
    tChangeSystemTime( (DRM_LONG)( (DRM_LONG)-1 * (DRM_LONG)dwOffsetTime ) );

    DRM_PRND_SAFEMEMFREE( pbMsgRxToTx );
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    return dr;
}

#define TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_ExpireAfterFirstPlay             0x00000001
#define TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_MeteringID                       0x00000002
#define TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_DomainID                         0x00000004

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelInvalidPRNDLicenseBind( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                                         = DRM_SUCCESS;
    DRM_APP_CONTEXT                     *pAppContext                                                = NULL;
    DRM_KID                              oKid                                                       = {0};
    DRM_LID                              oLid                                                       = {0};
    DRM_CHAR                             rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]       = {0};
    DRM_CHAR                             rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ) + 1 ]       = {0};
    DRM_DWORD                            cchKid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    DRM_DWORD                            cchLid                                                     = CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) );
    DRM_WCHAR                            rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]      = {0};
    DRM_DWORD                            cwchKid                                                    = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    const DRM_CONST_STRING              *pdstrPlay                                                  = &g_dstrWMDRM_RIGHT_PLAYBACK;
    const DRM_LONG                       argcInternal                                               = 38;
    DRM_CHAR                            *argvInternal[38]                                           = { 0 };
    DRM_BYTE                            *pbLicense                                                  = NULL;
    DRM_DWORD                            cbLicense                                                  = 0;
    DRM_BYTE                            *pbLicenseToStore                                           = NULL;
    DRM_DWORD                            cbLicenseToStore                                           = 0;
    DRM_DECRYPT_CONTEXT                  oDecryptCtx                                                = {0};
    DRM_BOOL                             fExpireAfterFirstPlay                                      = FALSE;
    DRM_BOOL                             fMeteringID                                                = FALSE;
    DRM_BOOL                             fDomainID                                                  = FALSE;
    const DRM_CHAR                       szKeySeed[]                                                = "XVBovsmzhP9gRIZxWfFta3VVRPzVEWmJsazEJ46I";
    const DRM_CHAR                       szMeteringID[]                                             = "FakeMeteringID";
    const DRM_CHAR                       szDomainID[]                                               = "FakeDomainID";
    const DRM_CHAR                       szExpireAfterFirstPlay[]                                   = "1000";
    const DRM_CHAR                       szOmit[]                                                   = "OMIT";
    DRM_CHAR                             rgchKeySeed[SIZEOF(szKeySeed)]                             = {0};
    DRM_CHAR                             rgchMeteringID[SIZEOF(szMeteringID)]                       = {0};
    DRM_CHAR                             rgchDomainID[SIZEOF(szDomainID)]                           = {0};
    DRM_CHAR                             rgchExpireAfterFirstPlay[SIZEOF(szExpireAfterFirstPlay)]   = {0};
    DRM_CHAR                             rgchOmit[SIZEOF(szOmit)]                                   = {0};
    DRM_DWORD                            dwArg0Flags                                                = 0;
    DRM_DWORD                            cch                                                        = 0;

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    ChkArg( (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->poPrndReceiverContext != NULL );
    ChkArg( HASARG(0) );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );

    fExpireAfterFirstPlay   = 0 != ( dwArg0Flags & TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_ExpireAfterFirstPlay );
    fMeteringID             = 0 != ( dwArg0Flags & TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_MeteringID );
    fDomainID               = 0 != ( dwArg0Flags & TestPRNDTopLevelInvalidPRNDLicenseBind_Arg0_FLAGS_DomainID );

    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );
    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oLid, SIZEOF( oLid ) ) );

    ChkDR( DRM_B64_EncodeA(
        (const DRM_BYTE*)&oKid,
        SIZEOF( oKid ),
        rgchKid,
       &cchKid,
        0 ) );
    rgchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_chNull;

    ChkDR( DRM_B64_EncodeA(
        (const DRM_BYTE*)&oLid,
        SIZEOF( oLid ),
        rgchLid,
       &cchLid,
        0 ) );
    rgchLid[ CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ) ] = g_chNull;

    DRMCRT_memcpy( rgchKeySeed, szKeySeed, SIZEOF( rgchKeySeed) );
    DRMCRT_memcpy( rgchMeteringID, szMeteringID, SIZEOF( rgchMeteringID ) );
    DRMCRT_memcpy( rgchDomainID, szDomainID, SIZEOF( rgchDomainID ) );
    DRMCRT_memcpy( rgchExpireAfterFirstPlay, szExpireAfterFirstPlay, SIZEOF( rgchExpireAfterFirstPlay ) );
    DRMCRT_memcpy( rgchOmit, szOmit, SIZEOF( rgchOmit ) );

    argvInternal[0] = rgchKid;                                                  /* KID */
    argvInternal[1] = rgchLid;                                                  /* LID */
    argvInternal[2] = rgchKeySeed;                                              /* Key Seed */

    if( fMeteringID )
    {
        argvInternal[6] = rgchMeteringID;                                       /* Metering ID */
    }

    if( fExpireAfterFirstPlay )
    {
        argvInternal[7] = rgchExpireAfterFirstPlay;                             /* ExpireAfterFirstPlay */
    }

    if( fDomainID )
    {
        argvInternal[17] = rgchDomainID;                                        /* Domain ID */
    }
    argvInternal[13] = rgchOmit;                                                /* Omit the copy right */

    ChkDR( Test_CreateXMRLicense( &pbLicense, &cbLicense, TRUE, argcInternal, argvInternal ) );

    cbLicenseToStore = cbLicense + SIZEOF( DRM_DWORD );
    ChkMem( pbLicenseToStore = Drm_Prnd_MemAlloc( cbLicenseToStore ) );
    DRMCRT_memcpy( pbLicenseToStore + SIZEOF( DRM_DWORD ), pbLicense, cbLicense );

    /*
    ** Rx: Store license in the receiver's PRND license store
    ** In regular case, this is done by Drm_Prnd_Receiver_LicenseTransmit_Process
    */
    ChkDR( DRM_LST_AddLicense(
        (( DRM_APP_CONTEXT_INTERNAL*)pAppContext)->poPrndReceiverContext->pLSTContextPRNDStore,
        cbLicense,        /* Size WITHOUT priority */
        pbLicenseToStore, /* Buffer WITH priority */
       &oKid,
       &oLid,
        0 ) );

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&oKid,
        SIZEOF( oKid ),
        rgwchKid,
       &cwchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_KID,
        (const DRM_BYTE*)rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );     /* Function *requires* the KID to NOT include the null terminator */

    dr = Drm_Reader_Bind(
        pAppContext,
        &pdstrPlay,
        1,
        NULL,
        NULL,
        &oDecryptCtx );

    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICEVAL_INVALID_PRND_LICENSE );

ErrorExit:
    SAFE_OEM_FREE( pbLicense );
    DRM_PRND_SAFEMEMFREE( pbLicenseToStore );

    return dr;
}

/*****************************************************************************
** Arguments:
**      None.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelErrorMessages( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_APP_CONTEXT             *pAppContext    = NULL;
    DRM_BYTE                    *pbMsgTxToRx    = NULL;
    DRM_DWORD                    cbMsgTxToRx    = 0;
    DRM_RESULT                   drError        = DRM_SUCCESS;
    DRM_RESULT                   drReturned     = DRM_SUCCESS;
    DRM_DWORD                    dwReturned     = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_RECEIVER_CONTEXT   *pRxCtx         = NULL;
    DRM_KID                      oKidEmpty      = {0};

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );

    /* Registration Error Message (Tx session available) */
    drError     = DRM_E_FILENOTFOUND;
    drReturned  = DRM_SUCCESS;
    dwReturned  = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    ChkDR( Drm_Prnd_Transmitter_RegistrationError_Generate(
        pAppContext,
        drError,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_ERROR );

    ChkDR( Drm_Prnd_Receiver_RegistrationError_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &drReturned,
        &dwReturned ) );

    ChkBOOL( drReturned == drError,                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwReturned == DRM_PRND_FLAG_NO_FLAGS,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Registration Error Message (Tx session unavailable) */
    drError     = DRM_E_CONDITION_NOT_SUPPORTED;
    drReturned  = DRM_SUCCESS;
    dwReturned  = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    ChkDR( Drm_Prnd_Transmitter_RegistrationError_Generate(
        NULL,
        drError,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_REGISTRATION_ERROR );

    ChkDR( Drm_Prnd_Receiver_RegistrationError_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &drReturned,
        &dwReturned ) );

    ChkBOOL( drReturned == drError,                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwReturned == DRM_PRND_FLAG_NO_FLAGS,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Proximity Detection Result Message (Tx session available) */
    drError     = DRM_E_FILEOPEN;
    drReturned  = DRM_SUCCESS;
    dwReturned  = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResult_Generate(
        pAppContext,
        NULL,
        drError,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESULT );

    ChkDR( Drm_Prnd_Receiver_ProximityDetectionResult_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &drReturned,
        &dwReturned ) );

    ChkBOOL( drReturned == drError,                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwReturned == DRM_PRND_FLAG_NO_FLAGS,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Proximity Detection Result Message (Tx session unavailable) */
    drError     = DRM_E_FILE_READ_ERROR;
    drReturned  = DRM_SUCCESS;
    dwReturned  = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    ChkDR( Drm_Prnd_Transmitter_ProximityDetectionResult_Generate(
        NULL,
        &pRxCtx->oPrndReceiverSessionContext.SessionID,
        drError,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESULT );

    ChkDR( Drm_Prnd_Receiver_ProximityDetectionResult_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &drReturned,
        &dwReturned ) );

    ChkBOOL( drReturned == drError,                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwReturned == DRM_PRND_FLAG_NO_FLAGS,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* License Error Message (Tx session available) */
    drError     = DRM_E_NOT_ALL_STORED;
    drReturned  = DRM_SUCCESS;
    dwReturned  = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    ChkDR( Drm_Prnd_Transmitter_LicenseError_Generate(
        pAppContext,
        NULL,
        drError,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_LICENSE_ERROR );

    ChkDR( Drm_Prnd_Receiver_LicenseError_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &drReturned,
        &dwReturned ) );

    ChkBOOL( drReturned == drError,                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwReturned == DRM_PRND_FLAG_NO_FLAGS,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* License Error Message (Tx session unavailable) */
    drError     = DRM_E_FAILED_TO_STORE_LICENSE;
    drReturned  = DRM_SUCCESS;
    dwReturned  = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    ChkDR( Drm_Prnd_Transmitter_LicenseError_Generate(
        NULL,
        &pRxCtx->oPrndReceiverSessionContext.SessionID,
        drError,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );
    VERIFY_MESSAGE_TYPE( pbMsgTxToRx, cbMsgTxToRx, DRM_PRND_MESSAGE_TYPE_LICENSE_ERROR );

    ChkDR( Drm_Prnd_Receiver_LicenseError_Process(
        pAppContext,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &drReturned,
        &dwReturned ) );

    ChkBOOL( drReturned == drError,                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwReturned == DRM_PRND_FLAG_NO_FLAGS,  DRM_E_TEST_UNEXPECTED_OUTPUT );

    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    dr = Drm_Prnd_Transmitter_LicenseTransmit_Generate(
        pAppContext,
        NULL,
        NULL,
        0,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_NOT_REGISTERED );

    dr = Drm_Prnd_Transmitter_PrepareLicensesForTransmit( pAppContext );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_NOT_REGISTERED );

    dr = Drm_Prnd_Receiver_LicenseRequest_Generate(
        pAppContext,
        &DRM_ACTION_PLAY,
        NULL,
        DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID,
        (const DRM_BYTE*)&oKidEmpty,
        SIZEOF( oKidEmpty ),
        NULL,
        NULL,
        0,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_NOT_REGISTERED );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbMsgTxToRx );
    return dr;
}


#define ISSUE_DATE_ALLOWED_SKEW   10   /* seconds */

#define TestPRNDTopLevelValidateGeneratedLicense_Arg0_FLAGS_HasOtherRights                0x00000001
#define TestPRNDTopLevelValidateGeneratedLicense_Arg0_FLAGS_HasOtherExternalParseObjects  0x00000002
/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDTopLevelValidateGeneratedLicense_Arg0_FLAGS_* values above.
**               Default: 0
**
** Notes: This function only works for PRND generated SIMPLE licenses.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDTopLevelValidateGeneratedLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                    dr                             = DRM_SUCCESS;
    DRM_APP_CONTEXT              *pAppContext                    = NULL;
    DRM_APP_CONTEXT_INTERNAL     *pAppContextInt                 = NULL;
    DRM_LICSTORE_CONTEXT         *poLSTContextPRNDRxStore        = NULL;
    DRM_LICSTOREENUM_CONTEXT      oLicEnumContext                = {0};
    DRM_KID                       oKID                           = {0};
    DRM_LID                       oLID                           = {0};
    DRM_BYTE                     *pbLicense                      = {0};
    DRM_DWORD                     cbLicense                      = 0;
    DRM_BYTE                     *pbXmrParserStack               = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT   stack                          = {0};
    DRM_XMR_LICENSE              *pXmrLicense                    = NULL;
    DRMFILETIME                   oDrmFileTimeNow                = {0};
    DRM_UINT64                    qwNow                          = DRM_UI64LITERAL(0,0);
    DRMFILETIME                   oDrmFileTimeLicenseIssueDate   = {0};
    DRM_UINT64                    qwLicenseIssueDate             = DRM_UI64LITERAL(0,0);
    const DRM_XMR_UNKNOWN_OBJECT *pUnknownObject                 = NULL;
    DRM_BOOL                      fFoundExtParseObj              = FALSE;
    DRM_BOOL                      fArg0HasOtherRights            = FALSE;
    DRM_BOOL                      fArg0HasOtherExtParseObjs      = FALSE;


    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fArg0HasOtherRights       = 0 != ( dwArg0Flags & TestPRNDTopLevelValidateGeneratedLicense_Arg0_FLAGS_HasOtherRights );
        fArg0HasOtherExtParseObjs = 0 != ( dwArg0Flags & TestPRNDTopLevelValidateGeneratedLicense_Arg0_FLAGS_HasOtherExternalParseObjects );
    }

    /* What time is it now? */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &oDrmFileTimeNow );
    FILETIME_TO_UI64( oDrmFileTimeNow, qwNow );

    /* Get the simple license from the PRND Rx Store */
    GET_SHARED_APP_CONTEXT( pAppContext );
    pAppContextInt = (DRM_APP_CONTEXT_INTERNAL *)pAppContext;

    poLSTContextPRNDRxStore = &( pAppContextInt->oLSTContextPRNDRxStore );
    ChkDR( DRM_LST_InitEnum( poLSTContextPRNDRxStore, NULL, FALSE, &oLicEnumContext ) );
    ChkDR( DRM_LST_EnumNext( &oLicEnumContext, &oKID, &oLID, NULL, &cbLicense ) );

    ChkMem( pbLicense = (DRM_BYTE *)Oem_MemAlloc( cbLicense ) );
    ZEROMEM( pbLicense, cbLicense );
    ChkDR( DRM_LST_GetLicense( poLSTContextPRNDRxStore, &oKID, &oLID, NULL, pbLicense, &cbLicense ) );

    /* Unpack the license into an XMR license structure */
    ChkMem( pbXmrParserStack = (DRM_BYTE *)Oem_MemAlloc( DRM_MAX_LICENSESIZE ) );
    ChkMem( pXmrLicense      = (DRM_XMR_LICENSE *)Oem_MemAlloc( SIZEOF( DRM_XMR_LICENSE ) ) );
    ZEROMEM( pbXmrParserStack, DRM_MAX_LICENSESIZE );
    ZEROMEM( pXmrLicense, SIZEOF( DRM_XMR_LICENSE ) );

    ChkDR( DRM_STK_Init( &stack, pbXmrParserStack, DRM_MAX_LICENSESIZE ) );
    ChkDR( DRM_XMR_UnpackLicense( pbLicense, cbLicense, &stack, pXmrLicense ) );


    /* Verify that the Issue Date is within ISSUE_DATE_ALLOWED_SKEW seconds of the current date/time */
    ChkBOOL( XMR_IS_ISSUEDATE_VALID( pXmrLicense ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    CREATE_FILE_TIME( pXmrLicense->containerOuter.containerGlobalPolicies.IssueDate.dwValue, oDrmFileTimeLicenseIssueDate );
    FILETIME_TO_UI64( oDrmFileTimeLicenseIssueDate, qwLicenseIssueDate );
    ChkBOOL( DRM_UI64Les( DRM_UI64Sub( DRM_UI64Div( qwNow, DRM_UI64( C_TICS_PER_SECOND ) ),
                                       DRM_UI64Div( qwLicenseIssueDate, DRM_UI64( C_TICS_PER_SECOND ) ) ),
                          DRM_UI64( ISSUE_DATE_ALLOWED_SKEW )),
             DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Verify:
    ** - no symmetrically optimized content key
    ** - no metering ID
    ** - no domain ID
    ** - no expire-after-first-play
    ** - no copy policy container
    */
    ChkBOOL( !XMR_IS_OPTIMIZED_CONTENT_KEY_VALID( pXmrLicense ),      DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !XMR_IS_METERING_VALID( pXmrLicense ),                   DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !XMR_IS_DOMAIN_ID_VALID( pXmrLicense ),                  DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID( pXmrLicense ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !XMR_IS_COPY_VALID( pXmrLicense ),                       DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Verify that the cannot-persist right is present, and that any existing rights are maintained.
    */
    ChkBOOL( XMR_IS_RIGHTS_VALID( pXmrLicense ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pXmrLicense->containerOuter.containerGlobalPolicies.Rights.wValue & XMR_RIGHTS_CANNOT_PERSIST, DRM_E_TEST_UNEXPECTED_OUTPUT );
    if ( fArg0HasOtherRights )
    {
        ChkBOOL( pXmrLicense->containerOuter.containerGlobalPolicies.Rights.wValue & ~XMR_RIGHTS_CANNOT_PERSIST, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /*
    ** Verify that any App-ID list objects have been removed from the playback container
    */
    for( pUnknownObject = pXmrLicense->containerOuter.containerPlaybackPolicies.pUnknownObjects;
         pUnknownObject != NULL && pUnknownObject->fValid;
         pUnknownObject = pUnknownObject->pNext )
    {
        ChkBOOL( pUnknownObject->wType != XMR_OBJECT_TYPE_APPLICATION_ID_LIST, DRM_E_TEST_UNEXPECTED_OUTPUT );
        fFoundExtParseObj = TRUE;
    }
    if ( fArg0HasOtherExtParseObjs )
    {
        ChkBOOL( fFoundExtParseObj, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkBOOL( !fFoundExtParseObj, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }


ErrorExit:
    SAFE_OEM_FREE( pbLicense );
    SAFE_OEM_FREE( pbXmrParserStack );
    SAFE_OEM_FREE( pXmrLicense );
    return dr;
}



BEGIN_TEST_API_LIST(prndtoplevelapitest)
    API_ENTRY(TestPRNDTopLevelRegistration)
    API_ENTRY(TestMalformedPRNDTopLevelRegistration)
    API_ENTRY(TestPRNDTopLevelRenewal)
    API_ENTRY(TestPRNDTopLevelProximityDetection)
    API_ENTRY(TestMalformedPRNDTopLevelProximityDetection)
    API_ENTRY(TestPRNDTopLevelLicenseFetch)
    API_ENTRY(TestMalformedPRNDTopLevelLicenseFetch)
    API_ENTRY(TestPRNDTopLevelLicenseFetchChainedLicenseRebind)
    API_ENTRY(TestPRNDTopLevelInvalidPRNDLicenseBind)
    API_ENTRY(TestPRNDTopLevelErrorMessages)
    API_ENTRY(TestPRNDTopLevelValidateGeneratedLicense)
    API_ENTRY(TestManagerStoreRevocationPackage)
    API_ENTRY(Test_ChangeTimeSeconds)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP /* __WARNING_USE_WIDE_API_25068 */

