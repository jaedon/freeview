/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <prndprotocoltest.h>
#include <drmprnd.h>
#include <DRMManagerTestAPI.h>
#include <drmmathsafe.h>
#include <drmprndinternal.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define TEST_VAR_PRND_GENLIDS   "PrndGeneratedLIDs"
#define TEST_VAR_PRND_GENIDXS   "PrndGeneratedIDXs"

static const DRM_DWORD g_dwCRLTime           = 1206921600;  /* crl issue time of revinfo in revpackage.xml */
static       DRM_BOOL  g_fFailCertCallback   = FALSE;

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(prndprotocoltest)(long lTCID, const char *strTCName)
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_PRND_RECEIVER_CONTEXT       *pRxCtx         = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT    *pTxCtx         = NULL;

    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pRxCtx, TEST_VAR_PRND_RXCTX, SIZEOF( DRM_PRND_RECEIVER_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pTxCtx, TEST_VAR_PRND_TXCTX, SIZEOF( DRM_PRND_TRANSMITTER_CONTEXT ) );

    tResetSystemTimeOffset();

    g_fFailCertCallback = FALSE;

ErrorExit:
    return dr;
}

static DRM_RESULT _CleanupTxCtx()
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_CONTEXT    *pTxCtx         = NULL;
    DRM_APP_CONTEXT                 *pAppContext    = NULL;

    GET_SHARED_APP_CONTEXT_NOFAIL( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    {
        /*
        ** If the context is a member of pAppContext, then Drm_Uninitialize will EndSession and we shouldn't have to.
        ** If skipping EndSession causes us to leak, we found a bug!
        */
        const DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( const DRM_APP_CONTEXT_INTERNAL * )pAppContext;
        if( pAppContext == NULL || poAppContextInternal->poPrndTransmitterContext != pTxCtx )
        {
            ChkDR( DRM_PRND_Transmitter_EndSession( pTxCtx ) );
        }
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _CleanupRxCtx()
{
    DRM_RESULT                   dr         = DRM_SUCCESS;
    DRM_PRND_RECEIVER_CONTEXT   *pRxCtx     = NULL;
    DRM_APP_CONTEXT             *pAppContext    = NULL;

    GET_SHARED_APP_CONTEXT_NOFAIL( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );

    {
        /*
        ** If the context is a member of pAppContext, then Drm_Uninitialize will EndSession and we shouldn't have to.
        ** If skipping EndSession causes us to leak, we found a bug!
        */
        const DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( const DRM_APP_CONTEXT_INTERNAL * )pAppContext;
        if( pAppContext == NULL || poAppContextInternal->poPrndReceiverContext != pRxCtx )
        {
            ChkDR( DRM_PRND_Receiver_EndSession( pRxCtx ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(prndprotocoltest)(long lTCID, const char *strTCName)
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    if( DRM_PRND_IsPRNDTransmitterSupported() )
    {
        POSTTESTCASE_FUNCTION_CALL( _CleanupTxCtx() );
    }

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        POSTTESTCASE_FUNCTION_CALL( _CleanupRxCtx() );
    }

    tResetSystemTimeOffset();

    g_fFailCertCallback = FALSE;

    return dr;
}

DRM_BYTE  g_rgbCustomData[3]    = { 0x1, 0x2, 0x3 };
DRM_DWORD g_cbCustomData        = SIZEOF( g_rgbCustomData );
DRM_ID    g_customDataTypeID    = { 0xc7, 0x7a, 0x68, 0x74, 0xe0, 0x6c, 0x46, 0x89, 0xa5, 0x43, 0xca, 0xc9, 0x5f, 0x36, 0x77, 0x9f };
DRM_ID    g_badCustomDataTypeID = { 0x4d, 0x53, 0x46, 0x54, 0xc7, 0xc8, 0x47, 0x1c, 0xbd, 0x85, 0x44, 0x30, 0x31, 0x43, 0xb1, 0xc5 };


PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
DRM_RESULT DRM_CALL _CertificateCallback(
    __inout_ecount_opt( 1 )                    DRM_VOID                                        *f_pvCertificateCallbackContext,
    __in_opt                                   DRM_PRND_CERTIFICATE_DATA                       *f_pCertificateData,
    __in_opt                             const DRM_ID                                          *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )    const DRM_BYTE                                        *f_pbCustomData,
    __in                                       DRM_DWORD                                        f_cbCustomData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT           dr     = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *pkfCtx = (DRM_KEYFILE_CONTEXT*)f_pvCertificateCallbackContext;
    DRM_BYTE            *pbCert = NULL;
    DRM_DWORD            cbCert = 0;

    /*
    ** This callback is where we could look at custom data to do extra cert / custom data validation.
    ** For test purposes, just make sure the callback receives what it should have received.
    */
    if( pkfCtx != NULL )
    {
        ChkDR( DRM_KF_GetCertificate(
            pkfCtx,
            eKF_CERT_TYPE_PLAYREADY,
            &pbCert,
            &cbCert ) );

        ChkBOOL( f_pCertificateData != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Verify dwType - all scenarios using this test should have a device certificate. */
        ChkBOOL( f_pCertificateData->dwType == DRM_BCERT_CERTTYPE_DEVICE, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Verify dwSecurityLevel. */
        ChkBOOL( f_pCertificateData->dwSecurityLevel == 150, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Don't verify anything else. We don't know what values they will take. */
    }
    else
    {
        ChkBOOL( f_pCertificateData == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    ChkBOOL( f_pCustomDataTypeID != NULL,                                                                             DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0                   == DRMCRT_memcmp( f_pCustomDataTypeID, &g_customDataTypeID, SIZEOF(DRM_ID)),         DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_cbCustomData      == g_cbCustomData,                                                                   DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0                   == DRMCRT_memcmp( f_pbCustomData, g_rgbCustomData, f_cbCustomData ),                 DRM_E_TEST_UNEXPECTED_OUTPUT );

    if ( g_fFailCertCallback )
    {
        ChkDR( DRM_E_BCERT_VERIFICATION_ERRORS );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Arguments:
**      None.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDProtocolRegistration( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                            = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgRxToTx                                   = NULL;
    DRM_DWORD                            cbMsgRxToTx                                   = 0;
    DRM_BYTE                            *pbMsgTxToRx                                   = NULL;
    DRM_DWORD                            cbMsgTxToRx                                   = 0;
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                        = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                        = NULL;
    DRM_BOOL                             fRivUpdated                                   = FALSE;
    DRM_APP_CONTEXT                     *pAppContext                                   = NULL;
    DRM_PRND_PROXIMITY_DETECTION_TYPE    eProximityDetectionType;
    DRM_BYTE                            *pbTransmitterProximityDetectionChannelWeakRef = NULL;
    DRM_DWORD                            cbTransmitterProximityDetectionChannelWeakRef = 0;
    DRM_DWORD                            dwFlagsOut                                    = 0;

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    {
        DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )pAppContext;

        pRxCtx->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
        pRxCtx->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
        pRxCtx->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
        pRxCtx->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
        pRxCtx->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
        pRxCtx->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;

        pTxCtx->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
        pTxCtx->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
        pTxCtx->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
        pTxCtx->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
        pTxCtx->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
        pTxCtx->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;
    }

    /* Rx: Generate Registration Request */
    SAFE_OEM_FREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    ChkDR( DRM_PRND_Receiver_RegistrationRequest_Generate(
        pRxCtx,
        NULL,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );

    /* Tx: Process Registration Request, Generate Registration Response */
    SAFE_OEM_FREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    ChkDR( DRM_PRND_Transmitter_RegistrationRequest_Process(
        pTxCtx,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        pTxCtx->pBBXContext->pKeyFileContext,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannelWeakRef,
        &cbTransmitterProximityDetectionChannelWeakRef,
        &dwFlagsOut ) );

    ChkDR( DRM_PRND_Transmitter_RegistrationResponse_Generate(
        pTxCtx,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );

    /* Rx: Process Registration Response */
    ChkDR( DRM_PRND_Receiver_RegistrationResponse_Process(
        pRxCtx,
        pbMsgTxToRx,
        cbMsgTxToRx,
        _CertificateCallback,
        pRxCtx->pBBXContext->pKeyFileContext,
        &fRivUpdated,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannelWeakRef,
        &cbTransmitterProximityDetectionChannelWeakRef,
        &dwFlagsOut ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsgRxToTx );
    SAFE_OEM_FREE( pbMsgTxToRx );
    return dr;
}



static DRMFORCEINLINE DRM_RESULT _GetSession(
    __inout                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __out                                               DRM_PRND_RECEIVER_SESSION_CONTEXT               **f_ppSession ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _GetSession(
    __inout                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __out                                               DRM_PRND_RECEIVER_SESSION_CONTEXT               **f_ppSession )
{
    DRM_RESULT dr = DRM_SUCCESS;
    AssertChkArg( f_poPrndReceiverContext != NULL );
    AssertChkArg( f_ppSession             != NULL );
    *f_ppSession = &f_poPrndReceiverContext->oPrndReceiverSessionContext;
ErrorExit:
    return dr;
}



static DRM_NO_INLINE DRM_RESULT _SetupReceiverContextBuffers(
    __inout                                   DRM_PRND_RECEIVER_CONTEXT                 *f_poPrndReceiverContext,
    __in                                const DRM_PRND_REGISTRATION_RESPONSE_MESSAGE    *f_poRegistrationResponseMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _SetupReceiverContextBuffers(
    __inout                                   DRM_PRND_RECEIVER_CONTEXT                 *f_poPrndReceiverContext,
    __in                                const DRM_PRND_REGISTRATION_RESPONSE_MESSAGE    *f_poRegistrationResponseMessage )
{
    DRM_RESULT                           dr                                     = DRM_SUCCESS;
    DRM_WORD                             cbTransmitterProximityDetectionChannel = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession                               = NULL;

    DRMASSERT( f_poPrndReceiverContext         != NULL );
    DRMASSERT( f_poRegistrationResponseMessage != NULL );
    __analysis_assume( f_poPrndReceiverContext         != NULL );
    __analysis_assume( f_poRegistrationResponseMessage != NULL );

    ChkDR( _GetSession( f_poPrndReceiverContext, &pSession ) );

    ChkDR( DRM_DWordToWord( XBBA_TO_CB( f_poRegistrationResponseMessage->ProximityInformation.xbbaTransmitterProximityDetectionChannel ), &cbTransmitterProximityDetectionChannel ) );

    if( pSession->cbTransmitterProximityDetectionChannel <  cbTransmitterProximityDetectionChannel
     || pSession->pbTransmitterProximityDetectionChannel == NULL )
    {
        SAFE_OEM_FREE( pSession->pbTransmitterProximityDetectionChannel );
        pSession->cbTransmitterProximityDetectionChannel = 0;
        ChkMem( pSession->pbTransmitterProximityDetectionChannel = (DRM_BYTE*)Oem_MemAlloc( cbTransmitterProximityDetectionChannel ) );
        pSession->cbTransmitterProximityDetectionChannel = cbTransmitterProximityDetectionChannel;
    }
    OEM_SECURE_ZERO_MEMORY( pSession->pbTransmitterProximityDetectionChannel, pSession->cbTransmitterProximityDetectionChannel );

    OEM_SECURE_MEMCPY(
        pSession->pbTransmitterProximityDetectionChannel,
        XBBA_TO_PB( f_poRegistrationResponseMessage->ProximityInformation.xbbaTransmitterProximityDetectionChannel ),
        XBBA_TO_CB( f_poRegistrationResponseMessage->ProximityInformation.xbbaTransmitterProximityDetectionChannel ) );

    pSession->eProximityDetectionType = (DRM_PRND_PROXIMITY_DETECTION_TYPE)f_poRegistrationResponseMessage->ProximityInformation.dwProximityDetectionType;

ErrorExit:
    return dr;
}



#define TestPRNDINTERNALProcessCertRevInfoCustomData_Arg0_FLAGS_BreakTxCertSigning  0x00000001
#define TestPRNDINTERNALProcessCertRevInfoCustomData_Arg0_FLAGS_NoTxCertFeature     0x00000002
#define TestPRNDINTERNALProcessCertRevInfoCustomData_Arg0_FLAGS_FailCertCallback    0x00000004

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDProtocolLicenseFetch_Arg0_FLAGS_* values above.
**               Default: 0
**      argv[1]: Name of a file containing a device certificate to use as the Tx certificate.
**               Optional, but only used (and required) if argv[0] == NoTxCertFeature
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDINTERNALProcessCertRevInfoCustomData( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                                  dr                                            = DRM_SUCCESS;
    DRM_BOOL                                    fArg0BreakTxCertSigning                       = FALSE;
    DRM_BOOL                                    fArg0NoTxCertFeature                          = FALSE;
    DRM_BOOL                                    fArg0FailCertCallback                         = FALSE;
    DRM_APP_CONTEXT                            *pAppContext                                   = NULL;
    DRM_PRND_RECEIVER_CONTEXT                  *pRxCtx                                        = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT               *pTxCtx                                        = NULL;
    DRM_BYTE                                   *pbMsgRxToTx                                   = NULL;
    DRM_DWORD                                   cbMsgRxToTx                                   = 0;
    DRM_BYTE                                   *pbMsgTxToRx                                   = NULL;
    DRM_DWORD                                   cbMsgTxToRx                                   = 0;
    DRM_PRND_PROXIMITY_DETECTION_TYPE           eProximityDetectionType;
    DRM_BYTE                                   *pbTransmitterProximityDetectionChannelWeakRef = NULL;
    DRM_DWORD                                   cbTransmitterProximityDetectionChannelWeakRef = 0;
    DRM_DWORD                                   dwFlagsOut                                    = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT          *pSession                                      = NULL;
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT         oPrndInternalSignatureContext                 = {0};
    DRM_PRND_REGISTRATION_RESPONSE_MESSAGE      oPrndRegistrationResponseMessage              = {0};
    const DRM_BYTE                             *pbToVerifyData                                = NULL;
    DRM_DWORD                                   cbToVerifyData                                = 0;
    const SIGNATURE_P256                       *pToVerifySig                                  = NULL;
    DRM_XB_BYTEARRAY                           *pxbbaTransmitterCertificate                   = NULL;
    DRM_BOOL                                    fRivUpdated                                   = FALSE;
    DRM_BYTE                                   *pbAlternateTxCertificate                      = NULL;
    DRM_DWORD                                   cbAlternateTxCertificate                      = 0;
    DRM_BYTE    rgbToVerify[SIZEOF(pSession->guidInitialLicenseMessageID) + SIZEOF(pSession->rgbMIMKEncryptedWithReceiverPubkey)] = {0};

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    if( HASARG( 0 ) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fArg0BreakTxCertSigning = 0 != ( dwArg0Flags & TestPRNDINTERNALProcessCertRevInfoCustomData_Arg0_FLAGS_BreakTxCertSigning );
        fArg0NoTxCertFeature    = 0 != ( dwArg0Flags & TestPRNDINTERNALProcessCertRevInfoCustomData_Arg0_FLAGS_NoTxCertFeature    );
        fArg0FailCertCallback   = 0 != ( dwArg0Flags & TestPRNDINTERNALProcessCertRevInfoCustomData_Arg0_FLAGS_FailCertCallback   );
    }

    {
        DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )pAppContext;

        pRxCtx->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
        pRxCtx->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
        pRxCtx->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
        pRxCtx->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
        pRxCtx->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
        pRxCtx->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;

        pTxCtx->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
        pTxCtx->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
        pTxCtx->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
        pTxCtx->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
        pTxCtx->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
        pTxCtx->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;
    }

    /* Rx: Generate Registration Request */
    SAFE_OEM_FREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    ChkDR( DRM_PRND_Receiver_RegistrationRequest_Generate(
        pRxCtx,
        NULL,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );

    /* Tx: Process Registration Request, Generate Registration Response */
    SAFE_OEM_FREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    ChkDR( DRM_PRND_Transmitter_RegistrationRequest_Process(
        pTxCtx,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        pTxCtx->pBBXContext->pKeyFileContext,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannelWeakRef,
        &cbTransmitterProximityDetectionChannelWeakRef,
        &dwFlagsOut ) );

    ChkDR( DRM_PRND_Transmitter_RegistrationResponse_Generate(
        pTxCtx,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );

    ChkDR( _GetSession( pRxCtx, &pSession ) );

    oPrndInternalSignatureContext.poPrndReceiverContext = pRxCtx;
    ChkDR( DRM_PRND_RegistrationResponseMessage_Parse(
        pSession->rgbStack,
        SIZEOF( pSession->rgbStack ),
        pbMsgTxToRx,
        cbMsgTxToRx,
        &oPrndRegistrationResponseMessage,
        &DRM_PRND_INTERNAL_FormatSignatureCallback,
        &oPrndInternalSignatureContext ) );

    ChkBOOL( oPrndRegistrationResponseMessage.TransmitterAuth.fValid, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( _SetupReceiverContextBuffers(
        pRxCtx,
        &oPrndRegistrationResponseMessage ) );

    /* Verify f_poRegistrationResponseMessage->TransmitterAuth.xbbaTransmitterAuthSignature over LMID | {MI|MK}PReceiver using the transmitter signing key */
    OEM_SECURE_MEMCPY(
        rgbToVerify,
        &pSession->guidInitialLicenseMessageID,
        SIZEOF( pSession->guidInitialLicenseMessageID ) );
    OEM_SECURE_MEMCPY(
        &rgbToVerify[SIZEOF(pSession->guidInitialLicenseMessageID)],
        XBBA_TO_PB( oPrndRegistrationResponseMessage.BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey ),
        XBBA_TO_CB( oPrndRegistrationResponseMessage.BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey ) );

    pbToVerifyData              = rgbToVerify;
    cbToVerifyData              = SIZEOF( rgbToVerify );
    pToVerifySig                = (const SIGNATURE_P256*)XBBA_TO_PB( oPrndRegistrationResponseMessage.TransmitterAuth.xbbaTransmitterAuthSignature );
    pxbbaTransmitterCertificate = &oPrndRegistrationResponseMessage.TransmitterAuth.xbbaTransmitterCertificate;

    if ( fArg0NoTxCertFeature )  /* replace the Tx certificate with the one from the command line */
    {
        ChkArg( HASARG( 1 ) );
        ChkBOOL( LoadTestFile( NULL, argv[1], &pbAlternateTxCertificate, &cbAlternateTxCertificate ), DRM_E_TEST_FILE_LOAD_ERROR ); /* performs a malloc */
        pxbbaTransmitterCertificate->pbDataBuffer = pbAlternateTxCertificate;
        pxbbaTransmitterCertificate->cbData       = cbAlternateTxCertificate;
        pxbbaTransmitterCertificate->iData        = 0;
    }

    if ( fArg0BreakTxCertSigning )
    {
        cbToVerifyData -= 16;
    }

    if ( fArg0FailCertCallback )
    {
        g_fFailCertCallback = TRUE;
    }

    ChkDR( DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
        NULL,
        pRxCtx,
        pxbbaTransmitterCertificate,
        oPrndRegistrationResponseMessage.CustomData.fValid ? &oPrndRegistrationResponseMessage.CustomData.customDataTypeID : NULL,
        oPrndRegistrationResponseMessage.CustomData.fValid ? &oPrndRegistrationResponseMessage.CustomData.xbbaCustomData   : NULL,
        _CertificateCallback,
        pRxCtx->pBBXContext->pKeyFileContext,
        NULL,
        NULL,
        pbToVerifyData,
        cbToVerifyData,
        pToVerifySig,
        (const DRM_ID*)&oPrndRegistrationResponseMessage.BasicInformation.guidSessionID,
        &oPrndRegistrationResponseMessage.BasicInformation.xbbaTransmitterRevInfo,
        &fRivUpdated ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsgRxToTx              );
    SAFE_OEM_FREE( pbMsgTxToRx              );
    SAFE_OEM_FREE( pbAlternateTxCertificate );
    g_fFailCertCallback = FALSE;
    return dr;
}



#define TestPRNDProtocolProximityDetection_Arg0_FLAGS_DropChallenge                 0x00000001
#define TestPRNDProtocolProximityDetection_Arg0_FLAGS_DropResponse                  0x00000002
#define TestPRNDProtocolProximityDetection_Arg0_FLAGS_DropResult                    0x00000004
#define TestPRNDProtocolProximityDetection_Arg0_FLAGS_ExpectResultFromStartProcess  0x00000008

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDProtocolProximityDetection_Arg0_FLAGS_* values above.
**               Default: 0
**      argv[1]: Milliseconds to sleep between challenge and response.
**               Default: 0
**      argv[2]: Number of retries to make if Proximity Detection timeout occurs.
**               Default: 2
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDProtocolProximityDetection( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                       dr                                 = DRM_SUCCESS;
    DRM_BYTE                        *pbMsgRxToTx                        = NULL;
    DRM_DWORD                        cbMsgRxToTx                        = 0;
    DRM_BYTE                        *pbMsgTxToRx                        = NULL;
    DRM_DWORD                        cbMsgTxToRx                        = 0;
    DRM_PRND_RECEIVER_CONTEXT       *pRxCtx                             = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT    *pTxCtx                             = NULL;
    DRM_RESULT                       drPD                               = DRM_SUCCESS;
    DRM_BOOL                         fArg0DropChallenge                 = FALSE;
    DRM_BOOL                         fArg0DropResponse                  = FALSE;
    DRM_BOOL                         fArg0DropResult                    = FALSE;
    DRM_BOOL                         fArg0ExpectResultFromStartProcess  = FALSE;
    DRM_DWORD                        dwArg1Sleep                        = 0;
    DRM_DWORD                        dwArg2Retries                      = 2;    /* Default to 2 retries (i.e. 3 tries total) */
    DRM_UINT64                       qwTick                             = DRM_UI64LITERAL(0,0);
    DRM_UINT64                       qwTickDiff                         = DRM_UI64LITERAL(0,0);
    DRM_DWORD                        dwFlagsOut                         = 0;

    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fArg0DropChallenge                  = 0 != ( dwArg0Flags & TestPRNDProtocolProximityDetection_Arg0_FLAGS_DropChallenge );
        fArg0DropResponse                   = 0 != ( dwArg0Flags & TestPRNDProtocolProximityDetection_Arg0_FLAGS_DropResponse );
        fArg0DropResult                     = 0 != ( dwArg0Flags & TestPRNDProtocolProximityDetection_Arg0_FLAGS_DropResult );
        fArg0ExpectResultFromStartProcess   = 0 != ( dwArg0Flags & TestPRNDProtocolProximityDetection_Arg0_FLAGS_ExpectResultFromStartProcess );
    }

    if( HASARG(1) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[1], cch, 10, &dwArg1Sleep ) );
    }

    if( HASARG(2) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[2]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[2], cch, 10, &dwArg2Retries ) );
    }

    do
    {
        /* Rx: Start proximity detection */
        SAFE_OEM_FREE( pbMsgRxToTx );
        cbMsgRxToTx = 0;
        ChkDR( DRM_PRND_Receiver_ProximityDetectionStart_Generate(
            pRxCtx,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgRxToTx,
            &cbMsgRxToTx ) );

        if( !fArg0ExpectResultFromStartProcess )
        {
            /* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms on Desktop per MSDN.  We need 1 ms resolution. */
            qwTick =
                DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                          Oem_Clock_QueryPerformanceCounter( NULL ) ),
                             Oem_Clock_QueryPerformanceFrequency( NULL ) );

            Log( "Trace", "Time before proximity detection: %d:%d", DRM_UI64High32(qwTick), DRM_UI64Low32(qwTick) );
        }

        /* Tx: Process start, Send proximity challenge */
        SAFE_OEM_FREE( pbMsgTxToRx );
        cbMsgTxToRx = 0;
        ChkDR( DRM_PRND_Transmitter_ProximityDetectionStart_Process(
            pTxCtx,
            pbMsgRxToTx,
            cbMsgRxToTx,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsgTxToRx,
            &cbMsgTxToRx,
            &dwFlagsOut ) );

        if( dwArg1Sleep > 0 )
        {
            /* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms on Desktop per MSDN.  We need 1 ms resolution. */

            DRM_UINT64 qwStartms =
                DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                          Oem_Clock_QueryPerformanceCounter( NULL ) ),
                             Oem_Clock_QueryPerformanceFrequency( NULL ) );

            DRM_UINT64 qwNowms = qwStartms;
            while( DRM_UI64Les( qwNowms, DRM_UI64Add( qwStartms, DRM_UI64HL( 0, dwArg1Sleep ) ) ) )
            {
                qwNowms =
                    DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                              Oem_Clock_QueryPerformanceCounter( NULL ) ),
                                 Oem_Clock_QueryPerformanceFrequency( NULL ) );

            }
        }

        if( fArg0DropChallenge )
        {
            goto ErrorExit;
        }

        if( fArg0ExpectResultFromStartProcess )
        {
            SAFE_OEM_FREE( pbMsgRxToTx );
            cbMsgRxToTx = 0;
            /* Tx: attempt to process challenge, fail because it's an error message instead */
            dr = DRM_PRND_Receiver_ProximityDetectionChallenge_Process(
                pRxCtx,
                pbMsgTxToRx,
                cbMsgTxToRx,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsgRxToTx,
                &cbMsgRxToTx,
                &dwFlagsOut );
            if( dr != DRM_E_PRND_MESSAGE_WRONG_TYPE )
            {
                ChkDR( dr );
                ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
            dr = DRM_SUCCESS;
        }
        else
        {
            /* Rx: Process challenge, Send proximity response */
            SAFE_OEM_FREE( pbMsgRxToTx );
            cbMsgRxToTx = 0;
            ChkDR( DRM_PRND_Receiver_ProximityDetectionChallenge_Process(
                pRxCtx,
                pbMsgTxToRx,
                cbMsgTxToRx,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsgRxToTx,
                &cbMsgRxToTx,
                &dwFlagsOut ) );

            if( fArg0DropResponse )
            {
                goto ErrorExit;
            }

            /* Tx: Process response, Send proximity result */
            SAFE_OEM_FREE( pbMsgTxToRx );
            cbMsgTxToRx = 0;
            ChkDR( DRM_PRND_Transmitter_ProximityDetectionResponse_Process(
                pTxCtx,
                pbMsgRxToTx,
                cbMsgRxToTx,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsgTxToRx,
                &cbMsgTxToRx,
                &dwFlagsOut ) );
        }

        if( fArg0DropResult )
        {
            drPD = pTxCtx->oPrndTransmitterSessionContext.drProximityResult;
        }
        else
        {
            /* Rx: Process proximity result */
            ChkDR( DRM_PRND_Receiver_ProximityDetectionResult_Process(
                pRxCtx,
                pbMsgTxToRx,
                cbMsgTxToRx,
                &drPD,
                &dwFlagsOut ) );
        }
        if( !fArg0ExpectResultFromStartProcess )
        {
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
            dwArg2Retries = 0;
        }
        else if( dwArg2Retries > 0 )
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
            dwArg2Retries--;
        }
        else
        {
            Log( "Trace", "RTT was unexpectedly more than 7 ms too many times.  Halting retry loop." );
        }

    } while( dwArg2Retries > 0 );

    ChkDR( drPD );

ErrorExit:
    SAFE_OEM_FREE( pbMsgRxToTx );
    SAFE_OEM_FREE( pbMsgTxToRx );
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const")
static DRM_RESULT DRM_CALL _CIDCallback(
    __inout_ecount_opt( 1 )                        DRM_VOID                                    *f_pvContentIdentifierCallbackContext,
    __in                                           DRM_PRND_CONTENT_IDENTIFIER_TYPE             f_eContentIdentifierType,
    __in_bcount_opt( f_cbContentIdentifier ) const DRM_BYTE                                    *f_pbContentIdentifier,
    __in                                           DRM_DWORD                                    f_cbContentIdentifier,
    __in_ecount_opt( 1 )                           DRM_KID                                     *f_pkidContent )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( f_pvContentIdentifierCallbackContext != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    if( f_pkidContent == NULL )
    {
        ChkBOOL( f_eContentIdentifierType == DRM_PRND_CONTENT_IDENTIFIER_TYPE_CUSTOM,          DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_cbContentIdentifier    == g_cbCustomData,                                   DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pbContentIdentifier    != NULL,                                             DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( 0 == DRMCRT_memcmp( f_pbContentIdentifier, g_rgbCustomData, g_cbCustomData ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkBOOL( f_eContentIdentifierType == DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID,                             DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pbContentIdentifier    == NULL,                                                             DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_cbContentIdentifier    == 0,                                                                DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( 0 == DRMCRT_memcmp( f_pkidContent, f_pvContentIdentifierCallbackContext, SIZEOF( DRM_KID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    return dr;
}

#define TestPRNDProtocolLicenseFetch_Arg0_FLAGS_UseCIDCustom        0x00000001
#define TestPRNDProtocolLicenseFetch_Arg0_FLAGS_PassCIDCallback     0x00000002
#define TestPRNDProtocolLicenseFetch_Arg0_FLAGS_SkipLicenseRequest  0x00000004
#define TestPRNDProtocolLicenseFetch_Arg0_FLAGS_BreakCLMID          0x00000008

#define TestPRNDProtocolLicenseFetch_MaxLicenses    10

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDProtocolLicenseFetch_Arg0_FLAGS_* values above.
**               Default: 0
**      argv[1]: String representing number of licenses to use.
**               Default: "1"
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDProtocolLicenseFetch( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                                                             = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgRxToTx                                                                    = NULL;
    DRM_DWORD                            cbMsgRxToTx                                                                    = 0;
    DRM_BYTE                            *pbMsgTxToRx                                                                    = NULL;
    DRM_DWORD                            cbMsgTxToRx                                                                    = 0;
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                                                         = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                                                         = NULL;
    DRM_BOOL                             fRivUpdated                                                                    = FALSE;
    DRM_KID                              oKid                                                                           = {0};
    DRM_DWORD                            cLids                                                                          = 0;
    DRM_LID                              rgbLids[TestPRNDProtocolLicenseFetch_MaxLicenses]                              = {0};
    DRM_BYTE                            *rgpbLicenses[TestPRNDProtocolLicenseFetch_MaxLicenses]                         = {0};
    DRM_DWORD                            rgcbLicenses[TestPRNDProtocolLicenseFetch_MaxLicenses]                         = {0};
    DRM_DWORD                            cbLargestLicenseExpectedValue                                                  = 0;
    DRM_BYTE                            *rgpbReceiverBoundOrLeafXMRLicenses[TestPRNDProtocolLicenseFetch_MaxLicenses]   = {0};
    DRM_DWORD                            rgcbReceiverBoundOrLeafXMRLicenses[TestPRNDProtocolLicenseFetch_MaxLicenses]   = {0};
    DRM_DWORD                            cReceiverBoundOrLeafXMRLicenses                                                = 0;
    DRM_DWORD                            cbLargestLicense                                                               = 0;
    DRM_DWORD                            idx                                                                            = 0;
    DRM_DWORD                            idx2                                                                           = 0;
    DRM_BOOL                             fArg0UseCIDCustom                                                              = FALSE;
    DRM_BOOL                             fArg0PassCIDCallback                                                           = FALSE;
    DRM_BOOL                             fArg0SkipLicenseRequest                                                        = FALSE;
    DRM_BOOL                             fArg0BreakCLMID                                                                = FALSE;
    DRM_PRND_CONTENT_IDENTIFIER_TYPE     eCID                                                                           = DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID;
    const DRM_BYTE                      *pbCID                                                                          = (const DRM_BYTE*)&oKid;
    DRM_DWORD                            cbCID                                                                          = SIZEOF( oKid );
    DRM_DWORD                            dwFlagsOut                                                                     = 0;

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fArg0UseCIDCustom       = 0 != ( dwArg0Flags & TestPRNDProtocolLicenseFetch_Arg0_FLAGS_UseCIDCustom );
        fArg0PassCIDCallback    = 0 != ( dwArg0Flags & TestPRNDProtocolLicenseFetch_Arg0_FLAGS_PassCIDCallback );
        fArg0SkipLicenseRequest = 0 != ( dwArg0Flags & TestPRNDProtocolLicenseFetch_Arg0_FLAGS_SkipLicenseRequest );
        fArg0BreakCLMID         = 0 != ( dwArg0Flags & TestPRNDProtocolLicenseFetch_Arg0_FLAGS_BreakCLMID );
    }

    if( HASARG(1) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[1], cch, 10, &cLids ) );
    }
    else
    {
        cLids = 1;          /* Default to just one license */
    }

    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&oKid, SIZEOF( oKid ) ) );

    if( fArg0UseCIDCustom )
    {
        eCID  = DRM_PRND_CONTENT_IDENTIFIER_TYPE_CUSTOM;
        pbCID = g_rgbCustomData;
        cbCID = g_cbCustomData;
    }

    if( !fArg0SkipLicenseRequest )
    {
        /* Rx: Send License Request */
        SAFE_OEM_FREE( pbMsgRxToTx );
        cbMsgRxToTx = 0;

        ChkDR( DRM_PRND_Receiver_LicenseRequest_Generate(
            pRxCtx,
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

        /* Tx: Process License Request */
        ChkDR( DRM_PRND_Transmitter_LicenseRequest_Process(
            pTxCtx,
            pbMsgRxToTx,
            cbMsgRxToTx,
            _CertificateCallback,
            NULL,
            fArg0PassCIDCallback ? &_CIDCallback : NULL,
            fArg0PassCIDCallback ? &oKid         : NULL,
            &dwFlagsOut ) );
    }

    for( idx = 0; idx < TestPRNDProtocolLicenseFetch_MaxLicenses; idx++ )
    {
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&rgbLids[idx], SIZEOF( DRM_LID ) ) );
    }

    /*
    ** Tx: Create license(s) to send to the transmitter
    ** We're using a fake license here which just has:
    ** XMR_MAGIC_CONSTANT | XMR_VERSION_3 | DRM_LID | 0x55555555
    ** This is enough for the core protocol: real license creation is outside the core protocol scope
    ** and will be tested as part of the DRM_APP_CONTEXT level APIs.
    */
    for( idx = 0; idx < cLids; idx++ )
    {
        rgcbLicenses[idx] = 2 * SIZEOF( DRM_DWORD ) + SIZEOF( DRM_LID ) + SIZEOF( DRM_DWORD );
        ChkMem( rgpbLicenses[idx] = (DRM_BYTE*)Oem_MemAlloc( rgcbLicenses[idx] ) );
        DWORD_TO_NETWORKBYTES( rgpbLicenses[idx], 0, XMR_MAGIC_CONSTANT );
        DWORD_TO_NETWORKBYTES( rgpbLicenses[idx], SIZEOF( DRM_DWORD ), XMR_VERSION_3 );
        if( 0 != DRMCRT_memcmp( &rgbLids[idx], &g_guidNull, SIZEOF( DRM_LID ) ) )
        {
            DRMCRT_memcpy( &rgpbLicenses[idx][2 * SIZEOF( DRM_DWORD )], &rgbLids[idx], SIZEOF( DRM_LID ) );
        }
        else
        {
            ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)&rgpbLicenses[idx][2 * SIZEOF( DRM_DWORD )], SIZEOF( DRM_LID ) ) );
        }
        DWORD_TO_NETWORKBYTES( rgpbLicenses[idx], 2 * SIZEOF( DRM_DWORD ) + SIZEOF( DRM_LID ), 0x55555555 );
        cbLargestLicenseExpectedValue = MAX( cbLargestLicenseExpectedValue, rgcbLicenses[idx] );
    }

    if( fArg0BreakCLMID )
    {
        DRM_PRND_TxRx_IncrementLMID( &pTxCtx->oPrndTransmitterSessionContext.guidCurrentLicenseMessageID );
    }

    /* Tx: Send License Transmit */
    SAFE_OEM_FREE( pbMsgTxToRx );
    cbMsgTxToRx = 0;

    ChkDR( DRM_PRND_Transmitter_LicenseTransmit_Generate(
        pTxCtx,
        rgpbLicenses,
        rgcbLicenses,
        cLids,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgTxToRx,
        &cbMsgTxToRx ) );

    /* Rx: Process License Transmit */
    /* This is the only API that is still two-pass, and it is kept that way in order to simplify drmmanager code. */
    dr = DRM_PRND_Receiver_LicenseTransmit_Process(
        pRxCtx,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &fRivUpdated,
        NULL,
        NULL,
        &cReceiverBoundOrLeafXMRLicenses,
        _CertificateCallback,
        NULL,
        &cbLargestLicense,
        &dwFlagsOut );
    DRM_TEST_EXPECT_ERROR( dr, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( cReceiverBoundOrLeafXMRLicenses == cLids,            DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbLargestLicenseExpectedValue   == cbLargestLicense, DRM_E_TEST_UNEXPECTED_OUTPUT );
    /* We'd normally allocate a couple arrays here, but for test purposes we know we have enough space already */
    ChkDR( DRM_PRND_Receiver_LicenseTransmit_Process(
        pRxCtx,
        pbMsgTxToRx,
        cbMsgTxToRx,
        &fRivUpdated,
        rgpbReceiverBoundOrLeafXMRLicenses,
        rgcbReceiverBoundOrLeafXMRLicenses,
        &cReceiverBoundOrLeafXMRLicenses,
        _CertificateCallback,
        NULL,
        &cbLargestLicense,
        &dwFlagsOut ) );
    ChkBOOL( cReceiverBoundOrLeafXMRLicenses == cLids,            DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbLargestLicenseExpectedValue   == cbLargestLicense, DRM_E_TEST_UNEXPECTED_OUTPUT );
    for( idx = 0; idx < cLids; idx++ )
    {
        DRM_BOOL fMatched = FALSE;
        for( idx2 = 0; idx2 < cLids && !fMatched; idx2++ )
        {
            if( ( rgcbLicenses[idx] == rgcbReceiverBoundOrLeafXMRLicenses[idx2] )
             && ( 0 == DRMCRT_memcmp( rgpbLicenses[idx], rgpbReceiverBoundOrLeafXMRLicenses[idx2], rgcbLicenses[idx] ) ) )
            {
                fMatched = TRUE;
            }
        }
        ChkBOOL( fMatched, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    for( idx = 0; idx < cLids; idx++ )
    {
        SAFE_OEM_FREE( rgpbLicenses[idx] );
    }
    SAFE_OEM_FREE( pbMsgRxToTx );
    SAFE_OEM_FREE( pbMsgTxToRx );
    return dr;
}

/*****************************************************************************
** Arguments:
**      None.
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDCRLTimeOffset( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRMFILETIME ftCurrentTime  = {0};
    DRM_DWORD   dwCurrentTime  = 0;
    DRM_LONG    lOffsetTime    = 0;

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
    FILE_TIME_TO_DWORD( ftCurrentTime, dwCurrentTime );

    lOffsetTime = (DRM_LONG)( g_dwCRLTime - dwCurrentTime );
    ChkDR( tChangeSystemTime( lOffsetTime ) );

 ErrorExit:
    return dr;
}

#define TestPRNDRevInfoOffset_Arg0_FLAGS_Forward        0x00000001
#define TestPRNDRevInfoOffset_Arg0_FLAGS_OutOfSync      0x00000002

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDRevInfoOffset_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDRevInfoOffset( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                           dr                                            = DRM_SUCCESS;
    DRM_BYTE                            *pbMsgRxToTx                                   = NULL;
    DRM_DWORD                            cbMsgRxToTx                                   = 0;
    DRM_PRND_RECEIVER_CONTEXT           *pRxCtx                                        = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT        *pTxCtx                                        = NULL;
    DRM_APP_CONTEXT                     *pAppContext                                   = NULL;
    DRM_PRND_PROXIMITY_DETECTION_TYPE    eProximityDetectionType;
    DRM_BYTE                            *pbTransmitterProximityDetectionChannelWeakRef = NULL;
    DRM_DWORD                            cbTransmitterProximityDetectionChannelWeakRef = 0;
    DRM_DWORD                            dwFlagsOut                                    = 0;
    DRM_BOOL                             fArg0Forward                                  = FALSE;
    DRM_BOOL                             fArg0OutOfSync                                = FALSE;

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fArg0Forward  = 0 != ( dwArg0Flags & TestPRNDRevInfoOffset_Arg0_FLAGS_Forward );
        fArg0OutOfSync = 0 != ( dwArg0Flags & TestPRNDRevInfoOffset_Arg0_FLAGS_OutOfSync );
    }

    {
        DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )pAppContext;

        pRxCtx->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
        pRxCtx->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
        pRxCtx->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
        pRxCtx->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
        pRxCtx->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
        pRxCtx->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;

        pTxCtx->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
        pTxCtx->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
        pTxCtx->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
        pTxCtx->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
        pTxCtx->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
        pTxCtx->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;
    }

    /* Rx: Generate Registration Request */
    SAFE_OEM_FREE( pbMsgRxToTx );
    cbMsgRxToTx = 0;

    if( !fArg0OutOfSync )
    {
        if( fArg0Forward )
        {
            ChkDR( tChangeSystemTime( MAX_REVOCATION_EXPIRE_TIME + 30 ) );
        }
        else
        {
            ChkDR( tChangeSystemTime( MAX_REVOCATION_EXPIRE_TIME - 30 ) );
        }
    }

    ChkDR( DRM_PRND_Receiver_RegistrationRequest_Generate(
        pRxCtx,
        NULL,
        &g_customDataTypeID,
        g_rgbCustomData,
        g_cbCustomData,
        DRM_PRND_FLAG_NO_FLAGS,
        &pbMsgRxToTx,
        &cbMsgRxToTx ) );

    if( fArg0OutOfSync )
    {
        if( fArg0Forward )
        {
            ChkDR( tChangeSystemTime( MAX_REVOCATION_EXPIRE_TIME + 30 ) );
        }
        else
        {
            ChkDR( tChangeSystemTime( MAX_REVOCATION_EXPIRE_TIME - 30 ) );
        }
    }

    dr = DRM_PRND_Transmitter_RegistrationRequest_Process(
        pTxCtx,
        pbMsgRxToTx,
        cbMsgRxToTx,
        _CertificateCallback,
        pTxCtx->pBBXContext->pKeyFileContext,
        &eProximityDetectionType,
        &pbTransmitterProximityDetectionChannelWeakRef,
        &cbTransmitterProximityDetectionChannelWeakRef,
        &dwFlagsOut );

    if( fArg0OutOfSync )
    {
        ChkBOOL( dr == DRM_E_PRND_CLOCK_OUT_OF_SYNC, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dr = DRM_SUCCESS;
    }
    else if( fArg0Forward )
    {
        ChkBOOL( dr == DRM_E_OUTDATED_REVOCATION_LIST, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    SAFE_OEM_FREE( pbMsgRxToTx );
    return dr;
}

BEGIN_TEST_API_LIST(prndprotocoltest)
    API_ENTRY(TestPRNDProtocolRegistration)
    API_ENTRY(TestPRNDProtocolProximityDetection)
    API_ENTRY(TestPRNDProtocolLicenseFetch)
    API_ENTRY(TestPRNDCRLTimeOffset)
    API_ENTRY(TestPRNDRevInfoOffset)
    API_ENTRY(TestManagerStoreRevocationPackage)
    API_ENTRY(TestPRNDINTERNALProcessCertRevInfoCustomData)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

