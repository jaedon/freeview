/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDPROTOCOLRECEIVER_C
#include <drmprndprotocol.h>
#include <drmprndformat.h>
#include <drmprndinternal.h>
#include <drmbcrl.h>
#include <drmbytemanip.h>
#include <drmconstants.h>
#include <oemcommon.h>
#include <oemndr.h>
#include <drmmanager.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDReceiverSupported(DRM_VOID) { return TRUE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDReceiverUnsupported(DRM_VOID) { return FALSE; }

static DRMFORCEINLINE DRM_RESULT _ActionGuidToActionEnum(
    __in                                          const DRM_GUID                                         *f_pguidRequestedAction,
    __out                                               DRM_WORD                                         *f_pwRequestedAction ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _ActionGuidToActionEnum(
    __in                                          const DRM_GUID                                         *f_pguidRequestedAction,
    __out                                               DRM_WORD                                         *f_pwRequestedAction )
{
    DRM_RESULT dr = DRM_SUCCESS;
    AssertChkArg( f_pguidRequestedAction != NULL );
    AssertChkArg( f_pwRequestedAction    != NULL );
    ChkBOOL( 0 == DRMCRT_memcmp( f_pguidRequestedAction, &DRM_ACTION_PLAY, SIZEOF( DRM_GUID ) ), DRM_E_PRND_MESSAGE_INVALID );
    *f_pwRequestedAction = DRM_PRND_ACTION_PLAY;
ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _ActionQualifierGuidToActionQualifierEnum(
    __in                                          const DRM_GUID                                         *f_pguidRequestedActionQualifier,
    __out                                               DRM_WORD                                         *f_pwRequestedActionQualifier ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _ActionQualifierGuidToActionQualifierEnum(
    __in                                          const DRM_GUID                                         *f_pguidRequestedActionQualifier,
    __out                                               DRM_WORD                                         *f_pwRequestedActionQualifier )
{
    DRM_RESULT dr = DRM_SUCCESS;
    AssertChkArg( f_pwRequestedActionQualifier != NULL );
    if( f_pguidRequestedActionQualifier != NULL )
    {
        ChkBOOL( 0 == DRMCRT_memcmp( f_pguidRequestedActionQualifier, &g_guidNull, SIZEOF( DRM_GUID ) ), DRM_E_PRND_MESSAGE_INVALID );
    }
    *f_pwRequestedActionQualifier = DRM_PRND_ACTION_QUALIFIER_NONE;
ErrorExit:
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

static DRMFORCEINLINE DRM_RESULT _ValidateReceiverContext(
    __inout                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __out                                               DRM_PRND_RECEIVER_SESSION_CONTEXT               **f_ppSession ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _ValidateReceiverContext(
    __inout                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __out                                               DRM_PRND_RECEIVER_SESSION_CONTEXT               **f_ppSession )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poPrndReceiverContext != NULL );
    ChkArg( f_poPrndReceiverContext->pBBXContext             != NULL );
    ChkArg( f_poPrndReceiverContext->pSSTGlobalContext       != NULL );
    ChkArg( f_poPrndReceiverContext->pDSTContext             != NULL );
    ChkArg( f_poPrndReceiverContext->pRevocationStoreContext != NULL );
    ChkArg( f_poPrndReceiverContext->pbRevocationBuffer      != NULL );
    ChkArg( f_poPrndReceiverContext->cbRevocationBuffer       > 0    );

    ChkDR( _GetSession( f_poPrndReceiverContext, f_ppSession ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_BeginSession(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext )
{
    DRM_RESULT                           dr         = DRM_SUCCESS;
    DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession   = NULL;

    TRACE(( "DRM_PRND_Receiver_BeginSession Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );

    OEM_SECURE_ZERO_MEMORY( pSession, SIZEOF( DRM_PRND_RECEIVER_SESSION_CONTEXT ) );

    ChkDR( DRM_PRND_OpenPRNDStore(
        f_poPrndReceiverContext->pBBXContext->pOEMContext,
        f_poPrndReceiverContext->pLSTContextPRNDStore,
        f_poPrndReceiverContext->pDSTContextPRNDStore,
        f_poPrndReceiverContext->pESTContextPRNDStore,
        f_poPrndReceiverContext->pbPRNDStore,
        f_poPrndReceiverContext->cbPRNDStore,
        f_poPrndReceiverContext->pfPRNDLSTOpened ) );

    pSession->dwTransmitterExpiration = DRM_BCERT_DEFAULT_EXPIRATION_DATE;

ErrorExit:
    TRACE(( "DRM_PRND_Receiver_BeginSession Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_RegistrationRequest_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                                    const DRM_ID                                           *f_pPreviousSessionID,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbReqMessage )                            DRM_BYTE                                        **f_ppbReqMessage,
    __out                                                             DRM_DWORD                                        *f_pcbReqMessage )
{
    DRM_RESULT                                       dr                                         = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                           xbContext                                  = {0};
    DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION  oPrndRegistrationRequestBasicInformation   = {0};
    DRM_PRND_CUSTOM_DATA                             oPrndCustomData                            = {0};
    DRM_BYTE                                        *pbReceiverCertificate                      = NULL;
    DRM_DWORD                                        cbReceiverCertificate                      = 0;
    DRM_BYTE                                        *pbReceiverRevInfo                          = NULL;
    DRM_DWORD                                        cbReceiverRevInfo                          = 0;
    DRM_BYTE                                        *pbStack                                    = NULL;
    DRM_DWORD                                        cbStack                                    = 0;
    DRM_DWORD                                        dwReceiverRIV                              = 0;
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT              oPrndInternalSignatureContext              = {0};
    DRM_PRND_RECEIVER_SESSION_CONTEXT               *pSession                                   = NULL;
    DRM_BYTE                                        *pbReqMessage                               = NULL;
    DRM_DWORD                                        cbReqMessage                               = 0;
    DRMFILETIME                                      ftCurrent                                  = {0};
    DRM_DWORD                                        dwCurrent                                  = 0;

    TRACE(( "DRM_PRND_Receiver_RegistrationRequest_Generate Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    ChkArg( f_pcbReqMessage != NULL );
    ChkArg( f_ppbReqMessage != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS || f_dwFlags == DRM_PRND_FLAG_REGISTRATION_REQUEST_SUPPORTS_LICENSE_PUSH );

    *f_pcbReqMessage = 0;
    *f_ppbReqMessage = NULL;

    if( NULL != f_pPreviousSessionID )
    {
        ChkBOOL( 0 != DRMCRT_memcmp( &g_guidNull,          &pSession->SessionID, SIZEOF( pSession->SessionID ) ), DRM_E_PRND_CANNOT_RENEW_USING_NEW_SESSION );
        ChkBOOL( 0 == DRMCRT_memcmp( f_pPreviousSessionID, &pSession->SessionID, SIZEOF( pSession->SessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );
    }
    else
    {
        /* We already have an active session. DRM_PRND_Receiver_EndSession must be called first. */
        ChkBOOL( 0 == DRMCRT_memcmp( &g_guidNull,          &pSession->SessionID, SIZEOF( pSession->SessionID ) ), DRM_E_PRND_CANNOT_REGISTER_USING_EXISTING_SESSION );
    }

    pSession->fInRenewal = ( f_pPreviousSessionID != NULL );

    TRACE(( "DRM_PRND_Receiver_RegistrationRequest_Generate Info {fRenewal = %d}", pSession->fInRenewal ? 1 : 0 ));

    if( pSession->fInRenewal )
    {
        /* Tear down everything in the session that is invalidated as renewal occurs. */

        /* 1. MI/MK have to be cleared and re-established. */
        DRM_BBX_SafeReleaseKey( f_poPrndReceiverContext->pBBXContext, &pSession->handleMIMK );

        OEM_SECURE_ZERO_MEMORY( pSession->rgbMIMKEncryptedWithReceiverPubkey, SIZEOF( pSession->rgbMIMKEncryptedWithReceiverPubkey ) );
    }

    /* Reset guidInitialLicenseMessageID on every registration. */
    do
    {
        ChkDR( Oem_Random_GetBytes( f_poPrndReceiverContext->pBBXContext->pOEMContext, (DRM_BYTE*)&pSession->guidInitialLicenseMessageID, SIZEOF( pSession->guidInitialLicenseMessageID ) ) );
    } while( ( DRMCRT_memcmp( &pSession->guidInitialLicenseMessageID, &g_guidNull,                            SIZEOF( DRM_GUID ) ) == 0 )
          || ( DRMCRT_memcmp( &pSession->guidInitialLicenseMessageID, &pSession->guidCurrentLicenseMessageID, SIZEOF( DRM_GUID ) ) == 0 ) );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_INTERNAL_FetchCertAndRevInfo(
        f_poPrndReceiverContext->pBBXContext->pOEMContext,
        f_poPrndReceiverContext->pRevocationStoreContext,
        &pbStack,
        &cbStack,
        f_poPrndReceiverContext->pBBXContext->pKeyFileContext,
        &pbReceiverCertificate,
        &cbReceiverCertificate,
        &pbReceiverRevInfo,
        &cbReceiverRevInfo,
        &dwReceiverRIV ) );

    ChkDR( DRM_PRND_RegistrationRequestMessage_Start( pbStack, cbStack, &xbContext ) );

    oPrndRegistrationRequestBasicInformation.fValid                               = TRUE;
    oPrndRegistrationRequestBasicInformation.dwFlags                              = f_dwFlags;
    if( pSession->fInRenewal )
    {
        OEM_SECURE_MEMCPY(
            &oPrndRegistrationRequestBasicInformation.guidPreviousSessionID,
            f_pPreviousSessionID,
            SIZEOF( oPrndRegistrationRequestBasicInformation.guidPreviousSessionID ) );
    }
    oPrndRegistrationRequestBasicInformation.xbbaReceiverCertificate.pbDataBuffer = pbReceiverCertificate;
    oPrndRegistrationRequestBasicInformation.xbbaReceiverCertificate.cbData       = cbReceiverCertificate;
    oPrndRegistrationRequestBasicInformation.dwReceiverRIV                        = dwReceiverRIV;
    ChkDR( Oem_Ndr_GetSupportedProximityDetectionTypes( &oPrndRegistrationRequestBasicInformation.dwSupportedProximityDetectionTypes ) );
    OEM_SECURE_MEMCPY(
        &oPrndRegistrationRequestBasicInformation.guidInitialLicenseMessageID,
        &pSession->guidInitialLicenseMessageID,
        SIZEOF( oPrndRegistrationRequestBasicInformation.guidInitialLicenseMessageID ) );

    /* Protocol uses seconds since 1970.  File Times are tics since 1601.  FILE_TIME_TO_DWORD does the conversion. */
    Oem_Clock_GetSystemTimeAsFileTime( f_poPrndReceiverContext->pBBXContext->pOEMContext, &ftCurrent );
    FILE_TIME_TO_DWORD( ftCurrent, dwCurrent );
    oPrndRegistrationRequestBasicInformation.qwReceiverCurrentTime = DRM_UI64HL( 0, dwCurrent );

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_BASIC_INFORMATION, &oPrndRegistrationRequestBasicInformation ) );

    if( f_pCustomDataTypeID != NULL )
    {
        DRMASSERT( f_pbCustomData != NULL );
        DRMASSERT( f_cbCustomData  > 0 );

        oPrndCustomData.fValid                               = TRUE;
        OEM_SECURE_MEMCPY(
            &oPrndCustomData.customDataTypeID,
            f_pCustomDataTypeID,
            SIZEOF( oPrndCustomData.customDataTypeID ) );

        oPrndCustomData.xbbaCustomData.pbDataBuffer          = (DRM_BYTE*)f_pbCustomData;  /* Cast off constness - the buffer won't be modified. */
        oPrndCustomData.xbbaCustomData.cbData                = f_cbCustomData;

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_CUSTOM_DATA, &oPrndCustomData ) );
    }

    oPrndInternalSignatureContext.poPrndReceiverContext = f_poPrndReceiverContext;

    dr = DRM_PRND_RegistrationRequestMessage_Finish( &xbContext, NULL, &cbReqMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbReqMessage = (DRM_BYTE*)Oem_MemAlloc( cbReqMessage ) );
    ChkDR( DRM_PRND_RegistrationRequestMessage_Finish( &xbContext, pbReqMessage, &cbReqMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext ) );

    *f_pcbReqMessage = cbReqMessage;
    *f_ppbReqMessage = pbReqMessage;
    pbReqMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbReqMessage );
    TRACE(( "DRM_PRND_Receiver_RegistrationRequest_Generate Exited {dr = 0x%x}", dr ));
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

static DRM_NO_INLINE DRM_RESULT _Receiver_RegistrationResponse_Validate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __inout                                                           DRM_PRND_REGISTRATION_RESPONSE_MESSAGE           *f_poRegistrationResponseMessage,
    __in_bcount( f_cbRespMessage )                              const DRM_BYTE                                         *f_pbRespMessage,
    __in                                                              DRM_DWORD                                         f_cbRespMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Receiver_RegistrationResponse_Validate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __inout                                                           DRM_PRND_REGISTRATION_RESPONSE_MESSAGE           *f_poRegistrationResponseMessage,
    __in_bcount( f_cbRespMessage )                              const DRM_BYTE                                         *f_pbRespMessage,
    __in                                                              DRM_DWORD                                         f_cbRespMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated )
{
    DRM_RESULT                           dr                             = DRM_SUCCESS;
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT  oPrndInternalSignatureContext  = {0};
    DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession                       = NULL;
    const DRM_BYTE                      *pbToVerifyData                 = NULL;
    DRM_DWORD                            cbToVerifyData                 = 0;
    const SIGNATURE_P256                *pToVerifySig                   = NULL;
    const DRM_XB_BYTEARRAY              *pxbbaTransmitterCertificate    = NULL;
    DRM_BYTE    rgbToVerify[SIZEOF(pSession->guidInitialLicenseMessageID) + SIZEOF(pSession->rgbMIMKEncryptedWithReceiverPubkey)] = {0};

    DRMASSERT( f_poPrndReceiverContext         != NULL );
    DRMASSERT( f_poRegistrationResponseMessage != NULL );
    DRMASSERT( f_pbRespMessage                 != NULL );
    DRMASSERT( f_cbRespMessage                  > 0    );
    DRMASSERT( f_pfRIVUpdated                  != NULL );
    __analysis_assume( f_poPrndReceiverContext         != NULL );
    __analysis_assume( f_poRegistrationResponseMessage != NULL );
    __analysis_assume( f_pbRespMessage                 != NULL );
    __analysis_assume( f_cbRespMessage                  > 0    );
    __analysis_assume( f_pfRIVUpdated                  != NULL );

    ChkDR( _GetSession( f_poPrndReceiverContext, &pSession ) );

    oPrndInternalSignatureContext.poPrndReceiverContext = f_poPrndReceiverContext;
    ChkDR( DRM_PRND_RegistrationResponseMessage_Parse(
        pSession->rgbStack,
        SIZEOF( pSession->rgbStack ),
        f_pbRespMessage,
        f_cbRespMessage,
        f_poRegistrationResponseMessage,
        &DRM_PRND_INTERNAL_FormatSignatureCallback,
        &oPrndInternalSignatureContext ) );

    ChkBOOL( f_poRegistrationResponseMessage->BasicInformation.wMIMKEncryptionType == DRM_PRND_ENCRYPTION_TYPE_ECC256_RECEIVER_PRND_ENCRYPTION_KEY,
             DRM_E_PRND_MESSAGE_INVALID );

    ChkBOOL( f_poRegistrationResponseMessage->ProximityInformation.dwProximityDetectionType == DRM_PRND_PROXIMITY_DETECTION_TYPE_UDP
          || f_poRegistrationResponseMessage->ProximityInformation.dwProximityDetectionType == DRM_PRND_PROXIMITY_DETECTION_TYPE_TCP
          || f_poRegistrationResponseMessage->ProximityInformation.dwProximityDetectionType == DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC,
             DRM_E_PRND_MESSAGE_INVALID );

    ChkDR( _SetupReceiverContextBuffers(
        f_poPrndReceiverContext,
        f_poRegistrationResponseMessage ) );

    ChkBOOL( XBBA_TO_CB( f_poRegistrationResponseMessage->BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey )
          == SIZEOF( pSession->rgbMIMKEncryptedWithReceiverPubkey ), DRM_E_PRND_MESSAGE_INVALID );

    if( f_poRegistrationResponseMessage->TransmitterAuth.fValid )
    {
        ChkBOOL( f_poRegistrationResponseMessage->TransmitterAuth.wTransmitterAuthSignatureType == DRM_PRND_SIGNATURE_TYPE_ECC256_TRANSMITTER_SIGNING_KEY, DRM_E_PRND_MESSAGE_INVALID );
        ChkBOOL( XBBA_TO_CB( f_poRegistrationResponseMessage->TransmitterAuth.xbbaTransmitterAuthSignature ) == SIZEOF( SIGNATURE_P256 ), DRM_E_PRND_MESSAGE_INVALID );

        /* Verify f_poRegistrationResponseMessage->TransmitterAuth.xbbaTransmitterAuthSignature over LMID | {MI|MK}PReceiver using the transmitter signing key */
        OEM_SECURE_MEMCPY(
            rgbToVerify,
            &pSession->guidInitialLicenseMessageID,
            SIZEOF( pSession->guidInitialLicenseMessageID ) );
        OEM_SECURE_MEMCPY(
            &rgbToVerify[SIZEOF(pSession->guidInitialLicenseMessageID)],
            XBBA_TO_PB( f_poRegistrationResponseMessage->BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey ),
            XBBA_TO_CB( f_poRegistrationResponseMessage->BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey ) );

        pbToVerifyData              = rgbToVerify;
        cbToVerifyData              = SIZEOF( rgbToVerify );
        pToVerifySig                = (const SIGNATURE_P256*)XBBA_TO_PB( f_poRegistrationResponseMessage->TransmitterAuth.xbbaTransmitterAuthSignature );
        pxbbaTransmitterCertificate = &f_poRegistrationResponseMessage->TransmitterAuth.xbbaTransmitterCertificate;
    }

    ChkDR( DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
        NULL,
        f_poPrndReceiverContext,
        pxbbaTransmitterCertificate,
        f_poRegistrationResponseMessage->CustomData.fValid ? &f_poRegistrationResponseMessage->CustomData.customDataTypeID : NULL,
        f_poRegistrationResponseMessage->CustomData.fValid ? &f_poRegistrationResponseMessage->CustomData.xbbaCustomData   : NULL,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        NULL,
        NULL,
        pbToVerifyData,
        cbToVerifyData,
        pToVerifySig,
        (const DRM_ID*)&f_poRegistrationResponseMessage->BasicInformation.guidSessionID,
        &f_poRegistrationResponseMessage->BasicInformation.xbbaTransmitterRevInfo,
        f_pfRIVUpdated ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_RegistrationResponse_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbRespMessage )                              const DRM_BYTE                                         *f_pbRespMessage,
    __in                                                              DRM_DWORD                                         f_cbRespMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated,
    __out                                                             DRM_PRND_PROXIMITY_DETECTION_TYPE                *f_peProximityDetectionType,
    __deref_out_ecount( *f_pcbTransmitterProximityDetectionChannel )  DRM_BYTE                                        **f_ppbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pcbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                                  dr                                  = DRM_SUCCESS;
    DRM_PRND_REGISTRATION_RESPONSE_MESSAGE      oPrndRegistrationResponseMessage    = {0};
    DRM_PRND_RECEIVER_SESSION_CONTEXT          *pSession                            = NULL;

    TRACE(( "DRM_PRND_Receiver_RegistrationResponse_Process Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pbRespMessage                             != NULL );
    ChkArg( f_cbRespMessage                              > 0    );
    ChkArg( f_pfRIVUpdated                              != NULL );
    ChkArg( f_peProximityDetectionType                  != NULL );
    ChkArg( f_ppbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pcbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pdwFlags                                  != NULL );

    *f_ppbTransmitterProximityDetectionChannel = NULL;
    *f_pcbTransmitterProximityDetectionChannel = 0;

    ChkDR( _Receiver_RegistrationResponse_Validate(
        f_poPrndReceiverContext,
        &oPrndRegistrationResponseMessage,
        f_pbRespMessage,
        f_cbRespMessage,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        f_pfRIVUpdated ) );

    OEM_SECURE_MEMCPY(
        &pSession->SessionID,
        &oPrndRegistrationResponseMessage.BasicInformation.guidSessionID,
        SIZEOF( pSession->SessionID ) );
    OEM_SECURE_MEMCPY(
        pSession->rgbMIMKEncryptedWithReceiverPubkey,
        XBBA_TO_PB( oPrndRegistrationResponseMessage.BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey ),
        XBBA_TO_CB( oPrndRegistrationResponseMessage.BasicInformation.xbbaMIMKEncryptedWithReceiverPubkey ) );
    OEM_SECURE_MEMCPY(
        &pSession->guidCurrentLicenseMessageID,
        &pSession->guidInitialLicenseMessageID,
        SIZEOF( pSession->guidCurrentLicenseMessageID ) );

    *f_peProximityDetectionType                = pSession->eProximityDetectionType;
    *f_ppbTransmitterProximityDetectionChannel = pSession->pbTransmitterProximityDetectionChannel;
    *f_pcbTransmitterProximityDetectionChannel = pSession->cbTransmitterProximityDetectionChannel;
    *f_pdwFlags = oPrndRegistrationResponseMessage.BasicInformation.dwFlags;

ErrorExit:
    TRACE(( "DRM_PRND_Receiver_RegistrationResponse_Process Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_RegistrationError_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbErrMessage )                               const DRM_BYTE                                         *f_pbErrMessage,
    __in                                                              DRM_DWORD                                         f_cbErrMessage,
    __out                                                             DRM_RESULT                                       *f_pdrResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                           dr                             = DRM_SUCCESS;
    DRM_PRND_REGISTRATION_ERROR_MESSAGE  oPrndRegistrationErrorMessage  = {0};
    DRM_BYTE                            *pbStack                        = NULL;
    DRM_DWORD                            cbStack                        = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession                       = NULL;

    TRACE(( "DRM_PRND_Receiver_RegistrationError_Process Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pbErrMessage  != NULL );
    ChkArg( f_cbErrMessage   > 0    );
    ChkArg( f_pdrResult     != NULL );
    ChkArg( f_pdwFlags      != NULL );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_RegistrationErrorMessage_Parse(
        pbStack,
        cbStack,
        f_pbErrMessage,
        f_cbErrMessage,
        &oPrndRegistrationErrorMessage ) );

    *f_pdrResult = (DRM_RESULT)oPrndRegistrationErrorMessage.BasicInformation.dwResult;
    *f_pdwFlags  = oPrndRegistrationErrorMessage.BasicInformation.dwFlags;

ErrorExit:
    TRACE(( "DRM_PRND_Receiver_RegistrationError_Process Exited {dr = 0x%x} {*f_pdrResult = 0x%x}", dr, f_pdrResult ? *f_pdrResult : DRM_E_INVALIDARG ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_ProximityDetectionStart_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDStartMessage )                        DRM_BYTE                                        **f_ppbPDStartMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDStartMessage )
{
    DRM_RESULT                                               dr                                             = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                                   xbContext                                      = {0};
    DRM_PRND_PROXIMITY_DETECTION_START_BASIC_INFORMATION     oPrndProximityDetectionStartBasicInformation   = {0};
    DRM_BYTE                                                *pbStack                                        = NULL;
    DRM_DWORD                                                cbStack                                        = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT                       *pSession                                       = NULL;
    DRM_BYTE                                                *pbPDStartMessage                               = NULL;
    DRM_DWORD                                                cbPDStartMessage                               = 0;

    TRACE(( "DRM_PRND_Receiver_ProximityDetectionStart_Generate Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pcbPDStartMessage != NULL );
    ChkArg( f_ppbPDStartMessage != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbPDStartMessage = 0;
    *f_ppbPDStartMessage = NULL;

    ChkBOOL( DRMCRT_memcmp( &pSession->guidInitialLicenseMessageID, &pSession->guidCurrentLicenseMessageID, SIZEOF( DRM_GUID ) ) == 0, DRM_E_DEVICE_NOT_REGISTERED );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_ProximityDetectionStartMessage_Start( pbStack, cbStack, &xbContext ) );

    oPrndProximityDetectionStartBasicInformation.fValid  = TRUE;
    oPrndProximityDetectionStartBasicInformation.dwFlags = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndProximityDetectionStartBasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndProximityDetectionStartBasicInformation.guidSessionID ) );

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_START_BASIC_INFORMATION, &oPrndProximityDetectionStartBasicInformation ) );

    dr = DRM_PRND_ProximityDetectionStartMessage_Finish( &xbContext, NULL, &cbPDStartMessage );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbPDStartMessage = (DRM_BYTE*)Oem_MemAlloc( cbPDStartMessage ) );
    ChkDR( DRM_PRND_ProximityDetectionStartMessage_Finish( &xbContext, pbPDStartMessage, &cbPDStartMessage ) );

    *f_pcbPDStartMessage = cbPDStartMessage;
    *f_ppbPDStartMessage = pbPDStartMessage;
    pbPDStartMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbPDStartMessage );
    TRACE(( "DRM_PRND_Receiver_ProximityDetectionStart_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Receiver_ProximityDetectionResponse_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in                                                              DRM_ID                                           *f_pNonce,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDRespMessage )                         DRM_BYTE                                        **f_ppbPDRespMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbPDRespMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Receiver_ProximityDetectionResponse_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in                                                              DRM_ID                                           *f_pNonce,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDRespMessage )                         DRM_BYTE                                        **f_ppbPDRespMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbPDRespMessage )
{
    DRM_RESULT                                               dr                                                 = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                                   xbContext                                          = {0};
    DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION  oPrndProximityDetectionResponseBasicInformation    = {0};
    DRM_PRND_RECEIVER_SESSION_CONTEXT                       *pSession                                           = NULL;
    DRM_BYTE                                                *pbPDRespMessage                                    = NULL;
    DRM_DWORD                                                cbPDRespMessage                                    = 0;

    DRMASSERT( f_poPrndReceiverContext  != NULL );
    DRMASSERT( f_pbStack                != NULL );
    DRMASSERT( f_cbStack                 > 0    );
    DRMASSERT( f_pNonce                 != NULL );
    DRMASSERT( f_pcbPDRespMessage       != NULL );
    DRMASSERT( f_ppbPDRespMessage       != NULL );
    __analysis_assume( f_poPrndReceiverContext  != NULL );
    __analysis_assume( f_pbStack                != NULL );
    __analysis_assume( f_cbStack                 > 0    );
    __analysis_assume( f_pNonce                 != NULL );
    __analysis_assume( f_pcbPDRespMessage       != NULL );
    __analysis_assume( f_ppbPDRespMessage       != NULL );

    ChkDR( _GetSession( f_poPrndReceiverContext, &pSession ) );

    ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Start( f_pbStack, f_cbStack, &xbContext ) );

    ChkDR( DRM_BBX_PRND_Rx_EncryptNonceWithAESECB(
        f_poPrndReceiverContext->pBBXContext,
        pSession->handleMIMK,
        f_pNonce ) );

    oPrndProximityDetectionResponseBasicInformation.fValid                                          = TRUE;
    oPrndProximityDetectionResponseBasicInformation.dwFlags                                         = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndProximityDetectionResponseBasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndProximityDetectionResponseBasicInformation.guidSessionID ) );
    oPrndProximityDetectionResponseBasicInformation.wNonceEncryptionType                            = DRM_PRND_ENCRYPTION_TYPE_AES_ECB_MK;
    OEM_SECURE_MEMCPY(
        &oPrndProximityDetectionResponseBasicInformation.guidNonceEncryptedWithMK,
        f_pNonce,
        SIZEOF( oPrndProximityDetectionResponseBasicInformation.guidNonceEncryptedWithMK ) );

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, &oPrndProximityDetectionResponseBasicInformation ) );

    dr = DRM_PRND_ProximityDetectionResponseMessage_Finish( &xbContext, NULL, &cbPDRespMessage );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbPDRespMessage = (DRM_BYTE*)Oem_MemAlloc( cbPDRespMessage ) );
    ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Finish( &xbContext, pbPDRespMessage, &cbPDRespMessage ) );

    *f_pcbPDRespMessage = cbPDRespMessage;
    *f_ppbPDRespMessage = pbPDRespMessage;
    pbPDRespMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbPDRespMessage );
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_ProximityDetectionChallenge_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbPDChlgMessage )                            const DRM_BYTE                                         *f_pbPDChlgMessage,
    __in                                                              DRM_DWORD                                         f_cbPDChlgMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDRespMessage )                         DRM_BYTE                                        **f_ppbPDRespMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDRespMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                                       dr                                         = DRM_SUCCESS;
    DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE   oPrndProximityDetectionChallengeMessage    = {0};
    DRM_BYTE                                        *pbStack                                    = NULL;
    DRM_DWORD                                        cbStack                                    = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT               *pSession                                   = NULL;
    DRM_ID                                           Nonce                                      = {0};

    /* Note: Do NOT trace from this method.  The proximity detection timer of 7ms is extremely performance sensitive. */

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pbPDChlgMessage   != NULL );
    ChkArg( f_cbPDChlgMessage    > 0    );
    ChkArg( f_pcbPDRespMessage  != NULL );
    ChkArg( f_ppbPDRespMessage  != NULL );
    ChkArg( f_pdwFlags          != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbPDRespMessage = 0;
    *f_ppbPDRespMessage = NULL;

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_ProximityDetectionChallengeMessage_Parse(
        pbStack,
        cbStack,
        f_pbPDChlgMessage,
        f_cbPDChlgMessage,
        &oPrndProximityDetectionChallengeMessage ) );
    ChkBOOL( 0 == DRMCRT_memcmp(
        &oPrndProximityDetectionChallengeMessage.BasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndProximityDetectionChallengeMessage.BasicInformation.guidSessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );

    OEM_SECURE_MEMCPY(
        &Nonce,
        &oPrndProximityDetectionChallengeMessage.BasicInformation.guidNonce,
        SIZEOF( Nonce ) );

    ChkDR( _Receiver_ProximityDetectionResponse_Generate(
        f_poPrndReceiverContext,
        pbStack,
        cbStack,
        &Nonce,
        f_dwFlags,
        f_ppbPDRespMessage,
        f_pcbPDRespMessage ) );

    *f_pdwFlags  = oPrndProximityDetectionChallengeMessage.BasicInformation.dwFlags;

ErrorExit:
    /* Note: Do NOT trace from this method.  The proximity detection timer of 7ms is extremely performance sensitive. */
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_ProximityDetectionResult_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbPDResultMessage )                          const DRM_BYTE                                         *f_pbPDResultMessage,
    __in                                                              DRM_DWORD                                         f_cbPDResultMessage,
    __out                                                             DRM_RESULT                                       *f_pdrPDResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                                   dr                                     = DRM_SUCCESS;
    DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE  oPrndProximityDetectionResultMessage   = {0};
    DRM_BYTE                                    *pbStack                                = NULL;
    DRM_DWORD                                    cbStack                                = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT           *pSession                               = NULL;

    TRACE(( "DRM_PRND_Receiver_ProximityDetectionResult_Process Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pbPDResultMessage != NULL );
    ChkArg( f_cbPDResultMessage  > 0    );
    ChkArg( f_pdrPDResult       != NULL );
    ChkArg( f_pdwFlags          != NULL );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_ProximityDetectionResultMessage_Parse(
        pbStack,
        cbStack,
        f_pbPDResultMessage,
        f_cbPDResultMessage,
        &oPrndProximityDetectionResultMessage ) );

    *f_pdrPDResult = (DRM_RESULT)oPrndProximityDetectionResultMessage.BasicInformation.dwResult;

    ChkBOOL( 0 == DRMCRT_memcmp(
        &oPrndProximityDetectionResultMessage.BasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndProximityDetectionResultMessage.BasicInformation.guidSessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );

    if( DRM_SUCCEEDED( *f_pdrPDResult ) )
    {
        /* We're no longer stuck in renewal until proximity detection succeeds. */
        pSession->fInRenewal = FALSE;
        pSession->fReadyForLicenses = TRUE;
    }

    *f_pdwFlags  = oPrndProximityDetectionResultMessage.BasicInformation.dwFlags;

ErrorExit:
    TRACE(( "DRM_PRND_Receiver_ProximityDetectionResult_Process Exited {dr = 0x%x} {*f_pdrPDResult = 0x%x}", dr, f_pdrPDResult ? *f_pdrPDResult : DRM_E_INVALIDARG ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_LicenseRequest_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                        const DRM_GUID                                         *f_pguidRequestedAction,
    __in_opt                                                    const DRM_GUID                                         *f_pguidRequestedActionQualifier,
    __in                                                              DRM_PRND_CONTENT_IDENTIFIER_TYPE                  f_eContentIdentifierType,
    __in_bcount( f_cbContentIdentifier )                        const DRM_BYTE                                         *f_pbContentIdentifier,
    __in                                                              DRM_DWORD                                         f_cbContentIdentifier,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicReqMessage )                         DRM_BYTE                                        **f_ppbLicReqMessage,
    __out                                                             DRM_DWORD                                        *f_pcbLicReqMessage )
{
    DRM_RESULT                                       dr                                     = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                           xbContext                              = {0};
    DRM_PRND_LICENSE_REQUEST_BASIC_INFORMATION       oPrndLicenseRequestBasicInformation    = {0};
    DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION     oPrndLicenseRequestLicenseInformation  = {0};
    DRM_BYTE                                        *pbStack                                = NULL;
    DRM_DWORD                                        cbStack                                = 0;
    DRM_BYTE                                        *pbReceiverCertificate                  = NULL;
    DRM_DWORD                                        cbReceiverCertificate                  = 0;
    DRM_BYTE                                        *pbReceiverRevInfo                      = NULL;
    DRM_DWORD                                        cbReceiverRevInfo                      = 0;
    DRM_DWORD                                        dwReceiverRIV                          = 0;
    DRM_PRND_CUSTOM_DATA                             oPrndCustomData                        = {0};
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT              oPrndInternalSignatureContext          = {0};
    DRM_PRND_RECEIVER_SESSION_CONTEXT               *pSession                               = NULL;
    DRM_BYTE                                        *pbLicReqMessage                        = NULL;
    DRM_DWORD                                        cbLicReqMessage                        = 0;

    TRACE(( "DRM_PRND_Receiver_LicenseRequest_Generate Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pguidRequestedAction  != NULL );
    ChkArg( f_eContentIdentifierType == DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID
         || f_eContentIdentifierType == DRM_PRND_CONTENT_IDENTIFIER_TYPE_CUSTOM );
    ChkArg( f_pbContentIdentifier   != NULL );
    ChkArg( f_cbContentIdentifier   != 0    );
    ChkArg( ( f_eContentIdentifierType != DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID ) || ( f_cbContentIdentifier == SIZEOF( DRM_KID ) ) );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    ChkArg( f_pcbLicReqMessage      != NULL );
    ChkArg( f_ppbLicReqMessage      != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbLicReqMessage = 0;
    *f_ppbLicReqMessage = NULL;

    ChkBOOL( pSession->fReadyForLicenses, DRM_E_DEVICE_NOT_REGISTERED );
    ChkBOOL( !pSession->fInRenewal, DRM_E_PRND_BUSY_PERFORMING_RENEWAL );

    DRMSIZEASSERT( SIZEOF( DRM_GUID ), SIZEOF( DRM_KID ) );
    DRMSIZEASSERT( SIZEOF( DRM_GUID ), SIZEOF( DRM_LID ) );
    DRMSIZEASSERT( SIZEOF( DRM_GUID ), SIZEOF( DRM_ID )  );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    /* Verify that the transmitter is not expired before requesting any licenses from it. */
    ChkDR( DRM_PRND_INTERNAL_CheckRemoteCertificateExpiration( f_poPrndReceiverContext->pBBXContext->pOEMContext, pSession->dwTransmitterExpiration ) );

    ChkDR( DRM_PRND_INTERNAL_FetchCertAndRevInfo(
        f_poPrndReceiverContext->pBBXContext->pOEMContext,
        f_poPrndReceiverContext->pRevocationStoreContext,
        &pbStack,
        &cbStack,
        f_poPrndReceiverContext->pBBXContext->pKeyFileContext,
        &pbReceiverCertificate,
        &cbReceiverCertificate,
        &pbReceiverRevInfo,
        &cbReceiverRevInfo,
        &dwReceiverRIV ) );

    ChkDR( DRM_PRND_LicenseRequestMessage_Start( pbStack, cbStack, &xbContext ) );

    oPrndLicenseRequestBasicInformation.fValid                               = TRUE;
    oPrndLicenseRequestBasicInformation.dwFlags                              = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndLicenseRequestBasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndLicenseRequestBasicInformation.guidSessionID ) );
    oPrndLicenseRequestBasicInformation.dwReceiverRIV                        = dwReceiverRIV;

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_BASIC_INFORMATION,   &oPrndLicenseRequestBasicInformation ) );

    if( f_pCustomDataTypeID != NULL )
    {
        DRMASSERT( f_pbCustomData != NULL );
        DRMASSERT( f_cbCustomData  > 0 );

        oPrndCustomData.fValid                               = TRUE;
        OEM_SECURE_MEMCPY(
            &oPrndCustomData.customDataTypeID,
            f_pCustomDataTypeID,
            SIZEOF( oPrndCustomData.customDataTypeID ) );

        oPrndCustomData.xbbaCustomData.pbDataBuffer          = (DRM_BYTE*)f_pbCustomData;  /* Cast off constness - the buffer won't be modified. */
        oPrndCustomData.xbbaCustomData.cbData                = f_cbCustomData;

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_CUSTOM_DATA, &oPrndCustomData ) );
    }

    oPrndLicenseRequestLicenseInformation.fValid = TRUE;
    ChkDR( _ActionGuidToActionEnum(                   f_pguidRequestedAction,          &oPrndLicenseRequestLicenseInformation.wRequestedAction ) );
    ChkDR( _ActionQualifierGuidToActionQualifierEnum( f_pguidRequestedActionQualifier, &oPrndLicenseRequestLicenseInformation.wRequestedActionQualifier ) );
    ChkDR( DRM_DWordToWord( f_eContentIdentifierType, &oPrndLicenseRequestLicenseInformation.wContentIdentifierType ) );
    oPrndLicenseRequestLicenseInformation.xbbaContentIdentifier.pbDataBuffer = (DRM_BYTE*)f_pbContentIdentifier;
    oPrndLicenseRequestLicenseInformation.xbbaContentIdentifier.cbData       = f_cbContentIdentifier;

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_LICENSE_INFORMATION, &oPrndLicenseRequestLicenseInformation ) );

    oPrndInternalSignatureContext.poPrndReceiverContext = f_poPrndReceiverContext;

    dr = DRM_PRND_LicenseRequestMessage_Finish( &xbContext, NULL, &cbLicReqMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbLicReqMessage = (DRM_BYTE*)Oem_MemAlloc( cbLicReqMessage ) );
    ChkDR( DRM_PRND_LicenseRequestMessage_Finish( &xbContext, pbLicReqMessage, &cbLicReqMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext ) );

    *f_pcbLicReqMessage = cbLicReqMessage;
    *f_ppbLicReqMessage = pbLicReqMessage;
    pbLicReqMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbLicReqMessage );
    TRACE(( "DRM_PRND_Receiver_LicenseRequest_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _PopulateLicenseList(
    __inout                                           const DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                    DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION    *f_poPrndLicenseTransmitLicenseInformationListHead,
    __in                                                    DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __inout_ecount_opt( f_cReceiverBoundOrLeafXMRLicenses ) DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __inout_ecount_opt( f_cReceiverBoundOrLeafXMRLicenses ) DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _PopulateLicenseList(
    __inout                                           const DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                    DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION    *f_poPrndLicenseTransmitLicenseInformationListHead,
    __in                                                    DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __inout_ecount_opt( f_cReceiverBoundOrLeafXMRLicenses ) DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __inout_ecount_opt( f_cReceiverBoundOrLeafXMRLicenses ) DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense )
{
    DRM_RESULT                                       dr                                 = DRM_SUCCESS;
    DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION   *pCurrent                           = NULL;
    DRM_DWORD                                        iReceiverBoundOrLeafXMRLicenses    = 0;

    DRMASSERT( f_poPrndReceiverContext                              != NULL );
    DRMASSERT( f_poPrndLicenseTransmitLicenseInformationListHead    != NULL );
    DRMASSERT( f_rgpbReceiverBoundOrLeafXMRLicense                  != NULL );
    DRMASSERT( f_rgcbReceiverBoundOrLeafXMRLicense                  != NULL );
    __analysis_assume( f_poPrndReceiverContext                              != NULL );
    __analysis_assume( f_poPrndLicenseTransmitLicenseInformationListHead    != NULL );
    __analysis_assume( f_rgpbReceiverBoundOrLeafXMRLicense                  != NULL );
    __analysis_assume( f_rgcbReceiverBoundOrLeafXMRLicense                  != NULL );

    for( pCurrent = f_poPrndLicenseTransmitLicenseInformationListHead; pCurrent != NULL; pCurrent = pCurrent->pNext )
    {
        /* Add the license to the output list for the caller to use. */
        f_rgpbReceiverBoundOrLeafXMRLicense[iReceiverBoundOrLeafXMRLicenses] = XBBA_TO_PB( pCurrent->xbbaReceiverBoundOrLeafXMRLicense );
        f_rgcbReceiverBoundOrLeafXMRLicense[iReceiverBoundOrLeafXMRLicenses] = XBBA_TO_CB( pCurrent->xbbaReceiverBoundOrLeafXMRLicense );

        /* Licenses can not be empty. */
        ChkBOOL( f_rgcbReceiverBoundOrLeafXMRLicense[iReceiverBoundOrLeafXMRLicenses] > 0, DRM_E_PRND_MESSAGE_INVALID );

        iReceiverBoundOrLeafXMRLicenses++;
    }

    DRMASSERT( iReceiverBoundOrLeafXMRLicenses == f_cReceiverBoundOrLeafXMRLicenses );
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_LicenseTransmit_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbLicTransmitMessage )                       const DRM_BYTE                                         *f_pbLicTransmitMessage,
    __in                                                              DRM_DWORD                                         f_cbLicTransmitMessage,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated,
    __inout_ecount_opt( *f_pcReceiverBoundOrLeafXMRLicenses )         DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __inout_ecount_opt( *f_pcReceiverBoundOrLeafXMRLicenses )         DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __inout                                                           DRM_DWORD                                        *f_pcReceiverBoundOrLeafXMRLicenses,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out_opt                                                         DRM_DWORD                                        *f_pcbLargestLicense,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                                           dr                                 = DRM_SUCCESS;
    DRM_PRND_LICENSE_TRANSMIT_MESSAGE                    oPrndLicenseTransmitMessage        = {0};
    DRM_DWORD                                            cReceiverBoundOrLeafXMRLicenses    = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT                   *pSession                           = NULL;
    const DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION *pCurrent                           = NULL;
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT                  oPrndInternalSignatureContext      = {0};

    TRACE(( "DRM_PRND_Receiver_LicenseTransmit_Process Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pbLicTransmitMessage             != NULL );
    ChkArg( f_cbLicTransmitMessage              > 0    );
    ChkArg( f_pfRIVUpdated                     != NULL );
    ChkArg( f_pcReceiverBoundOrLeafXMRLicenses != NULL );
    ChkArg( ( f_rgpbReceiverBoundOrLeafXMRLicense == NULL ) == ( *f_pcReceiverBoundOrLeafXMRLicenses == 0 ) );
    ChkArg( ( f_rgcbReceiverBoundOrLeafXMRLicense == NULL ) == ( *f_pcReceiverBoundOrLeafXMRLicenses == 0 ) );
    ChkArg( f_pdwFlags                         != NULL );

    ChkBOOL( !pSession->fInRenewal, DRM_E_PRND_BUSY_PERFORMING_RENEWAL );

    {
        /*
        ** IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
        **
        ** Do not add any expensive operations (e.g. new crypto) inside this block.
        ** This block should only do enough to parse the message and determine the output parameter sizes.
        ** DRM_PRND_LicenseTransmitMessage_Parse will verify the message signature twice when called
        ** in a two-pass manner, but that's a relatively inexpensive operation (OMAC1 verify).
        **
        ** IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
        */
        DRM_BYTE  *pbStack = pSession->rgbStack;
        DRM_DWORD  cbStack = SIZEOF( pSession->rgbStack );

        oPrndInternalSignatureContext.poPrndReceiverContext = f_poPrndReceiverContext;
        ChkDR( DRM_PRND_LicenseTransmitMessage_Parse(
            pbStack,
            cbStack,
            f_pbLicTransmitMessage,
            f_cbLicTransmitMessage,
            &oPrndLicenseTransmitMessage,
            &DRM_PRND_INTERNAL_FormatSignatureCallback,
            &oPrndInternalSignatureContext ) );

        ChkBOOL( 0 == DRMCRT_memcmp(
            &oPrndLicenseTransmitMessage.BasicInformation.guidSessionID,
            &pSession->SessionID,
            SIZEOF( oPrndLicenseTransmitMessage.BasicInformation.guidSessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );

        ChkBOOL( 0 == DRMCRT_memcmp(
            &oPrndLicenseTransmitMessage.BasicInformation.guidCurrentLicenseMessageID,
            &pSession->guidCurrentLicenseMessageID,
            SIZEOF( oPrndLicenseTransmitMessage.BasicInformation.guidCurrentLicenseMessageID ) ), DRM_E_PRND_LICENSE_RESPONSE_CLMID_INVALID );

        ChkBOOL( oPrndLicenseTransmitMessage.LicenseList.fValid, DRM_E_PRND_MESSAGE_INVALID );  /* Protocol requires at least one license */

        if( f_pcbLargestLicense != NULL )
        {
            *f_pcbLargestLicense = 0;
        }
        for( pCurrent = oPrndLicenseTransmitMessage.LicenseList.pLicenseListHead; pCurrent != NULL; pCurrent = pCurrent->pNext )
        {
            if( f_pcbLargestLicense != NULL )
            {
                *f_pcbLargestLicense = MAX( *f_pcbLargestLicense, XBBA_TO_CB( pCurrent->xbbaReceiverBoundOrLeafXMRLicense ) );
            }

            cReceiverBoundOrLeafXMRLicenses++;
        }
        ChkBOOL( cReceiverBoundOrLeafXMRLicenses > 0, DRM_E_PRND_MESSAGE_INVALID );  /* Protocol requires at least one license */
        if( *f_pcReceiverBoundOrLeafXMRLicenses < cReceiverBoundOrLeafXMRLicenses )
        {
            *f_pcReceiverBoundOrLeafXMRLicenses = cReceiverBoundOrLeafXMRLicenses;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
    }

    ChkDR( DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
        NULL,
        f_poPrndReceiverContext,
        NULL,
        oPrndLicenseTransmitMessage.CustomData.fValid ? &oPrndLicenseTransmitMessage.CustomData.customDataTypeID : NULL,
        oPrndLicenseTransmitMessage.CustomData.fValid ? &oPrndLicenseTransmitMessage.CustomData.xbbaCustomData   : NULL,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        NULL,
        NULL,
        NULL,
        0,
        NULL,
        NULL,
        &oPrndLicenseTransmitMessage.BasicInformation.xbbaTransmitterRevInfo,
        f_pfRIVUpdated ) );

    ChkDR( _PopulateLicenseList(
        f_poPrndReceiverContext,
        oPrndLicenseTransmitMessage.LicenseList.pLicenseListHead,
        cReceiverBoundOrLeafXMRLicenses,
        f_rgpbReceiverBoundOrLeafXMRLicense,
        f_rgcbReceiverBoundOrLeafXMRLicense ) );

    /* Everything succeeded, so hand out the license count to the caller. */
    *f_pcReceiverBoundOrLeafXMRLicenses = cReceiverBoundOrLeafXMRLicenses;

    /* Update dwCurrentLicenseMessageID as the last operation. */
    DRM_PRND_TxRx_IncrementLMID( &pSession->guidCurrentLicenseMessageID );

    *f_pdwFlags  = oPrndLicenseTransmitMessage.BasicInformation.dwFlags;

ErrorExit:
    TRACE(( "DRM_PRND_Receiver_LicenseTransmit_Process Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_LicenseError_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbErrMessage )                               const DRM_BYTE                                         *f_pbErrMessage,
    __in                                                              DRM_DWORD                                         f_cbErrMessage,
    __out                                                             DRM_RESULT                                       *f_pdrResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                           dr                         = DRM_SUCCESS;
    DRM_PRND_LICENSE_ERROR_MESSAGE       oPrndLicenseErrorMessage   = {0};
    DRM_BYTE                            *pbStack                    = NULL;
    DRM_DWORD                            cbStack                    = 0;
    DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession                   = NULL;

    TRACE(( "DRM_PRND_Receiver_LicenseError_Process Entered" ));

    ChkDR( _ValidateReceiverContext( f_poPrndReceiverContext, &pSession ) );
    ChkArg( f_pbErrMessage  != NULL );
    ChkArg( f_cbErrMessage   > 0    );
    ChkArg( f_pdrResult     != NULL );
    ChkArg( f_pdwFlags      != NULL );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_LicenseErrorMessage_Parse(
        pbStack,
        cbStack,
        f_pbErrMessage,
        f_cbErrMessage,
        &oPrndLicenseErrorMessage ) );

    *f_pdrResult = (DRM_RESULT)oPrndLicenseErrorMessage.BasicInformation.dwResult;
    *f_pdwFlags  = oPrndLicenseErrorMessage.BasicInformation.dwFlags;

ErrorExit:
    TRACE(( "DRM_PRND_Receiver_LicenseError_Process Exited {dr = 0x%x} {*f_pdrResult = 0x%x}", dr, f_pdrResult ? *f_pdrResult : DRM_E_INVALIDARG ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_EndSession(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    TRACE(( "DRM_PRND_Receiver_EndSession Entered" ));

    if( f_poPrndReceiverContext != NULL )
    {
        DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession   = NULL;

        dr = _GetSession( f_poPrndReceiverContext, &pSession );
        if( DRM_SUCCEEDED( dr ) )
        {
            SAFE_OEM_FREE( pSession->pbTransmitterProximityDetectionChannel );

            DRM_BBX_SafeReleaseKey( f_poPrndReceiverContext->pBBXContext, &pSession->handleMIMK );
        }

        OEM_SECURE_ZERO_MEMORY( &f_poPrndReceiverContext->oPrndReceiverSessionContext, SIZEOF( f_poPrndReceiverContext->oPrndReceiverSessionContext ) );
    }

    /* Do NOT define ErrorExit label here.  Do NOT use ChkDR (etc) above.  Doing so could skip cleanup operations. */
    TRACE(( "DRM_PRND_Receiver_EndSession Exited {dr = 0x%x}", dr ));
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

