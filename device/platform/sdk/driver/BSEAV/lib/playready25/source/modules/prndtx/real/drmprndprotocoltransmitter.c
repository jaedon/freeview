/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDPROTOCOLTRANSMITTER_C
#include <drmprndprotocol.h>
#include <drmprndformat.h>
#include <drmprndinternal.h>
#include <drmbcrl.h>
#include <drmbytemanip.h>
#include <drmutilitieslite.h>
#include <drmconstants.h>
#include <oemcommon.h>
#include <oemndt.h>
#include <drmmanager.h>
#include <drmmathsafe.h>
#include <drmdeviceassets.h>
#include <drmlastinclude.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDTransmitterSupported(DRM_VOID) { return TRUE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDTransmitterUnsupported(DRM_VOID) { return FALSE; }

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Tx_RebindLicenseKeysToNewECCKey(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                              DRM_REVOCATIONSTORE_CONTEXT     *f_poRevStoreContext,
    __in_ecount( 1 )                            const DRM_BINDING_INFO                *f_pRootmostLicenseBindingInfo,
    __in                                        const DRM_PUBLIC_KEY_CONTEXT          *f_poECC256PubKeyTarget,
    __in_ecount_opt( f_cbCertificateTarget )    const DRM_BYTE                        *f_pbCertificateTarget,
    __in                                              DRM_DWORD                        f_cbCertificateTarget,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL,
    __in                                        const DRM_ID                          *f_pidSession,
    __out_ecount( 1 )                                 OEM_CRYPTO_HANDLE               *f_phandleAESIntegrityKey,
    __out_ecount( 1 )                                 CIPHERTEXT_P256                 *f_poEncryptedAESKeyPair )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbRevInfo = 0;
    DRM_BYTE  *pbRevInfo = NULL;

    ChkArg( f_poBBXContext      != NULL );
    ChkArg( f_poRevStoreContext != NULL );

    dr = DRM_RVK_CheckRevInfoForExpiration(
        &f_poBBXContext->CryptoContext,
        f_poRevStoreContext,
        &g_guidRevocationTypeRevInfo2,
        f_pidSession,
        NULL,
        &cbRevInfo );

    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    DRMASSERT( cbRevInfo > 0 );
    ChkMem( pbRevInfo = (DRM_BYTE*)Oem_MemAlloc( cbRevInfo ) );

    ChkDR( DRM_RVK_CheckRevInfoForExpiration(
        &f_poBBXContext->CryptoContext,
        f_poRevStoreContext,
        &g_guidRevocationTypeRevInfo2,
        f_pidSession,
        pbRevInfo,
        &cbRevInfo ) );

    ChkDR( DRM_BBX_PRND_Tx_RebindLicenseKeysToNewECCKey(
        f_poBBXContext,
        f_pRootmostLicenseBindingInfo,
        f_poECC256PubKeyTarget,
        f_pbCertificateTarget,
        f_cbCertificateTarget,
        f_pbCertificateTargetCRL,
        f_cbCertificateTargetCRL,
        pbRevInfo,
        cbRevInfo,
        f_phandleAESIntegrityKey,
        f_poEncryptedAESKeyPair ) );

ErrorExit:
    SAFE_OEM_FREE( pbRevInfo );
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _GetSession(
    __in                                                              DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __out                                                             DRM_PRND_TRANSMITTER_SESSION_CONTEXT            **f_ppSession ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _GetSession(
    __in                                                              DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __out                                                             DRM_PRND_TRANSMITTER_SESSION_CONTEXT            **f_ppSession )
{
    DRM_RESULT dr = DRM_SUCCESS;
    AssertChkArg( f_poPrndTransmitterContext != NULL );
    AssertChkArg( f_ppSession                != NULL );
    *f_ppSession = &f_poPrndTransmitterContext->oPrndTransmitterSessionContext;
ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _ValidateTransmitterContext(
    __in                                                              DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __out                                                             DRM_PRND_TRANSMITTER_SESSION_CONTEXT            **f_ppSession ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _ValidateTransmitterContext(
    __in                                                              DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __out                                                             DRM_PRND_TRANSMITTER_SESSION_CONTEXT            **f_ppSession )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poPrndTransmitterContext != NULL );
    ChkArg( f_poPrndTransmitterContext->pBBXContext             != NULL );
    ChkArg( f_poPrndTransmitterContext->pSSTGlobalContext       != NULL );
    ChkArg( f_poPrndTransmitterContext->pDSTContext             != NULL );
    ChkArg( f_poPrndTransmitterContext->pRevocationStoreContext != NULL );
    ChkArg( f_poPrndTransmitterContext->pbRevocationBuffer      != NULL );
    ChkArg( f_poPrndTransmitterContext->cbRevocationBuffer       > 0    );

    ChkDR( _GetSession( f_poPrndTransmitterContext, f_ppSession ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_BeginSession(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

    TRACE(( "DRM_PRND_Transmitter_BeginSession Entered" ));

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );

    OEM_SECURE_ZERO_MEMORY( pSession, SIZEOF( DRM_PRND_TRANSMITTER_SESSION_CONTEXT ) );

    ChkDR( DRM_PRND_OpenPRNDStore(
        f_poPrndTransmitterContext->pBBXContext->pOEMContext,
        &pSession->oLSTContextPRNDStore,
        &pSession->oDSTContextPRNDStore,
        &pSession->oESTContextPRNDStore,
        pSession->rgbPRNDStore,
        SIZEOF( pSession->rgbPRNDStore ),
        &pSession->fPRNDLSTOpened ) );

    pSession->dwReceiverExpiration = DRM_BCERT_DEFAULT_EXPIRATION_DATE;

ErrorExit:
    TRACE(( "DRM_PRND_Transmitter_BeginSession Exited {dr = 0x%x}", dr ));
    return dr;
}

static DRMFORCEINLINE DRM_BYTEBLOB _ToByteBlob(
    __in_bcount( f_cbBlob ) DRM_BYTE  *f_pbBlob,
    __in                    DRM_DWORD  f_cbBlob ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_BYTEBLOB _ToByteBlob(
    __in_bcount( f_cbBlob ) DRM_BYTE  *f_pbBlob,
    __in                    DRM_DWORD  f_cbBlob )
{
    DRM_BYTEBLOB oBlob;
    oBlob.pbBlob = f_pbBlob;
    oBlob.cbBlob = f_cbBlob;
    return oBlob;
}

static DRMFORCEINLINE DRM_RESULT _GenerateUniqueSessionID(
    __in                                  DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __out                                 DRM_ID                                        *f_pNewSessionID ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _GenerateUniqueSessionID(
    __in                                  DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __out                                 DRM_ID                                        *f_pNewSessionID )
{
    DRM_RESULT      dr          = DRM_SUCCESS;

    DRMASSERT( f_poPrndTransmitterContext != NULL );
    DRMASSERT( f_pNewSessionID            != NULL );
    __analysis_assume( f_poPrndTransmitterContext != NULL );
    __analysis_assume( f_pNewSessionID            != NULL );

    do
    {
        ChkDR( Oem_Random_GetBytes( f_poPrndTransmitterContext->pBBXContext->pOEMContext, (DRM_BYTE *)f_pNewSessionID, DRM_ID_SIZE ) );
    } while( 0 == DRMCRT_memcmp( f_pNewSessionID, &g_guidNull, SIZEOF( DRM_ID ) ) );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _ValidateReceiverCertificate(
    __in                                                    DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in                                              const DRM_PRND_REGISTRATION_REQUEST_MESSAGE            *f_poRegistrationRequestMessage,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _ValidateReceiverCertificate(
    __in                                                    DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in                                              const DRM_PRND_REGISTRATION_REQUEST_MESSAGE            *f_poRegistrationRequestMessage,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

    DRMASSERT( f_poPrndTransmitterContext       != NULL );
    DRMASSERT( f_poRegistrationRequestMessage   != NULL );
    __analysis_assume( f_poPrndTransmitterContext       != NULL );
    __analysis_assume( f_poRegistrationRequestMessage   != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    ChkDR( DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
        f_poPrndTransmitterContext,
        NULL,
        &f_poRegistrationRequestMessage->BasicInformation.xbbaReceiverCertificate,
        f_poRegistrationRequestMessage->CustomData.fValid ? &f_poRegistrationRequestMessage->CustomData.customDataTypeID : NULL,
        f_poRegistrationRequestMessage->CustomData.fValid ? &f_poRegistrationRequestMessage->CustomData.xbbaCustomData   : NULL,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        &pSession->oECC256PubKeyPrndEncryptReceiver,
        &pSession->oECC256PubKeyContentEncryptReceiver,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        NULL ) );

    DRMASSERT( XBBA_TO_CB( f_poRegistrationRequestMessage->BasicInformation.xbbaReceiverCertificate ) <= pSession->cbReceiverCertificate );

    OEM_SECURE_MEMCPY(
        pSession->pbReceiverCertificate,
        XBBA_TO_PB( f_poRegistrationRequestMessage->BasicInformation.xbbaReceiverCertificate ),
        XBBA_TO_CB( f_poRegistrationRequestMessage->BasicInformation.xbbaReceiverCertificate ) );
    pSession->cbReceiverCertificate = XBBA_TO_CB( f_poRegistrationRequestMessage->BasicInformation.xbbaReceiverCertificate );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _RegisterDevice(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _RegisterDevice(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

    DRMASSERT( f_poPrndTransmitterContext   != NULL );
    __analysis_assume( f_poPrndTransmitterContext   != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    if( !pSession->fDeviceRegistrationAllowed )
    {
        DRM_BYTEBLOB                             blobReceiverCertificate = {0};
        DRM_BYTEBLOB                             blobNull                = {0};

        /* The Device ID for the device store in PRND is just the certificate, unlike WMDRMND which also has a serial number. */
        blobReceiverCertificate = _ToByteBlob( pSession->pbReceiverCertificate, pSession->cbReceiverCertificate );
        dr = Oem_Ndt_AllowNewDeviceRegistration( blobReceiverCertificate, blobNull );
        if( dr != DRM_SUCCESS )
        {
            ChkDR( dr );
            ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
        }

        pSession->fDeviceRegistrationAllowed = TRUE;
    }

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_BOOL _IsDeviceValid(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __inout                               DRM_PRND_TRANSMITTER_SESSION_CONTEXT          *f_poPrndTransmitterSessionContext ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_BOOL _IsDeviceValid(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __inout                               DRM_PRND_TRANSMITTER_SESSION_CONTEXT          *f_poPrndTransmitterSessionContext )
{
    DRM_RESULT  dr                                                                                = DRM_SUCCESS;
    DRMFILETIME ftSuccessfulProximity                                                             = {0};
    DRM_BOOL    fValid                                                                            = FALSE;
    DRM_BYTE    rgbSig[DRM_AES_BLOCKLEN]; /* Don't zero-initialize signature buffer. */

    DRM_DWORD   dwTimeout                                                                         = DRM_PRND_REGISTRATION_VALIDITY;
    DRM_DWORD   dwCbTimeout                                                                       = SIZEOF( DRM_DWORD );
    DRM_BYTE    rgbTimeout[ SIZEOF( DRM_DWORD )]                                                  = { 0 };
    DRM_CHAR    rgchId[ CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) ]                                    = { 0 };
    DRM_DWORD   cchId                                                                             = CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) );
    DRM_CHAR    rgbConfigKey[ Oem_Config_SETimeout_Key_LEN +
                              CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) + 1 ]                          = { 0 };

    DRMASSERT( f_poPrndTransmitterContext           != NULL );
    DRMASSERT( f_poPrndTransmitterSessionContext    != NULL );
    __analysis_assume( f_poPrndTransmitterContext           != NULL );
    __analysis_assume( f_poPrndTransmitterSessionContext    != NULL );

    OEM_SECURE_MEMCPY( &ftSuccessfulProximity, &f_poPrndTransmitterSessionContext->ftSuccessfulProximity,     SIZEOF( ftSuccessfulProximity ) );
    OEM_SECURE_MEMCPY( &rgbSig,                &f_poPrndTransmitterSessionContext->rgbSuccessfulProximitySig, SIZEOF( rgbSig                ) );

    /* Fail if we've been tampered with. */
    ChkDR( DRM_BBX_PRND_Tx_VerifySignatureWithAESOMAC(
        f_poPrndTransmitterContext->pBBXContext,
        f_poPrndTransmitterSessionContext->handleMIMK,
        (DRM_BYTE*)&ftSuccessfulProximity,
        SIZEOF( ftSuccessfulProximity ),
        rgbSig ) );

    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions.")
    /* Create ConfigKey to read config data from share memory */
    ChkDR( DRM_B64_EncodeA(
        (const DRM_BYTE*)&f_poPrndTransmitterSessionContext->SessionID,
        SIZEOF( DRM_ID ),
        rgchId,
        &cchId,
        0 ) );
    PREFAST_POP /* ignore prefast warning about ANSI functions */

    OEM_SECURE_MEMCPY( rgbConfigKey, Oem_Config_SETimeout_Key, Oem_Config_SETimeout_Key_LEN );
    OEM_SECURE_MEMCPY( rgbConfigKey + Oem_Config_SETimeout_Key_LEN, rgchId, CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) );

    dr = Oem_Config_Read( NULL, rgbConfigKey, rgbTimeout, &dwCbTimeout );

    /* Oem_Config_Read returns DRM_S_FALSE if there's no value */
    if( dr == DRM_SUCCESS )
    {
        DRMASSERT( dwCbTimeout == SIZEOF( DRM_DWORD ) );

        BYTES_TO_DWORD( dwTimeout, rgbTimeout );
        dwTimeout = MIN( dwTimeout, DRM_PRND_REGISTRATION_VALIDITY );
    }
    else
    {
        /* Ignore any errors */
        dr = DRM_SUCCESS;
    }

    /* Fail if timeout exceeded. */
    ChkBOOL( !DRM_UTL_IsTimeoutExceeded(
        f_poPrndTransmitterContext->pBBXContext->pOEMContext,
        &ftSuccessfulProximity,
        dwTimeout ), DRM_E_DEVICE_NOT_REGISTERED );

    /* Only set fValid if everything succeeded.  That way, any failure means the device is NOT valid. */
    fValid = TRUE;

ErrorExit:
    return fValid;
}

static DRMFORCEINLINE DRM_RESULT _MarkDeviceAsValid(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __inout                               DRM_PRND_TRANSMITTER_SESSION_CONTEXT          *f_poPrndTransmitterSessionContext ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _MarkDeviceAsValid(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __inout                               DRM_PRND_TRANSMITTER_SESSION_CONTEXT          *f_poPrndTransmitterSessionContext )
{
    DRM_RESULT  dr                          = DRM_SUCCESS;
    DRMFILETIME ftSuccessfulProximity       = {0};
    DRM_BYTE    rgbSig[DRM_AES_BLOCKLEN]; /* Don't zero-initialize signature buffer. */

    DRMASSERT( f_poPrndTransmitterContext           != NULL );
    DRMASSERT( f_poPrndTransmitterSessionContext    != NULL );
    __analysis_assume( f_poPrndTransmitterContext           != NULL );
    __analysis_assume( f_poPrndTransmitterSessionContext    != NULL );

    Oem_Clock_GetSystemTimeAsFileTime( f_poPrndTransmitterContext->pBBXContext->pOEMContext, &ftSuccessfulProximity );

    ChkDR( DRM_BBX_PRND_Tx_SignWithAESOMAC(
        f_poPrndTransmitterContext->pBBXContext,
        f_poPrndTransmitterSessionContext->handleMIMK,
        (DRM_BYTE*)&ftSuccessfulProximity,
        SIZEOF( ftSuccessfulProximity ),
        rgbSig ) );

    OEM_SECURE_MEMCPY( &f_poPrndTransmitterSessionContext->ftSuccessfulProximity,     &ftSuccessfulProximity,  SIZEOF( ftSuccessfulProximity ) );
    OEM_SECURE_MEMCPY( &f_poPrndTransmitterSessionContext->rgbSuccessfulProximitySig, &rgbSig,                 SIZEOF( rgbSig                ) );
    f_poPrndTransmitterSessionContext->fSuccessfulProximityAtLeastOnce = TRUE;

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_BOOL _HasDeviceTimedOut(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __inout                               DRM_PRND_TRANSMITTER_SESSION_CONTEXT          *f_poPrndTransmitterSessionContext ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_BOOL _HasDeviceTimedOut(
    __inout                               DRM_PRND_TRANSMITTER_CONTEXT                  *f_poPrndTransmitterContext,
    __inout                               DRM_PRND_TRANSMITTER_SESSION_CONTEXT          *f_poPrndTransmitterSessionContext )
{
    DRMASSERT( f_poPrndTransmitterContext           != NULL );
    DRMASSERT( f_poPrndTransmitterSessionContext    != NULL );
    __analysis_assume( f_poPrndTransmitterContext           != NULL );
    __analysis_assume( f_poPrndTransmitterSessionContext    != NULL );

    return DRM_UTL_IsTimeoutExceeded(
        f_poPrndTransmitterContext->pBBXContext->pOEMContext,
        &f_poPrndTransmitterSessionContext->ftRegistrationTimeoutStart,
        DRM_PRND_REGISTRATION_TIMEOUT );
}

static DRM_NO_INLINE DRM_RESULT _SetupInitialTransmitterContextBuffers(
    __inout                                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __in                                              const DRM_PRND_REGISTRATION_REQUEST_MESSAGE            *f_poRegistrationRequestMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _SetupInitialTransmitterContextBuffers(
    __inout                                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __in                                              const DRM_PRND_REGISTRATION_REQUEST_MESSAGE            *f_poRegistrationRequestMessage )
{
    DRM_RESULT                               dr                                     = DRM_SUCCESS;
    DRM_WORD                                 cbTransmitterProximityDetectionChannel = 0;
    DRM_DWORD                                cbReceiverCertificate                  = 0;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession                               = NULL;

    DRMASSERT( f_poPrndTransmitterContext       != NULL );
    DRMASSERT( f_poRegistrationRequestMessage   != NULL );
    __analysis_assume( f_poPrndTransmitterContext       != NULL );
    __analysis_assume( f_poRegistrationRequestMessage   != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    dr = Oem_Ndt_GetProximityDetectionChannel( NULL, &cbTransmitterProximityDetectionChannel );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    dr = DRM_SUCCESS;

    if( pSession->cbTransmitterProximityDetectionChannel <  cbTransmitterProximityDetectionChannel
     || pSession->pbTransmitterProximityDetectionChannel == NULL )
    {
        SAFE_OEM_FREE( pSession->pbTransmitterProximityDetectionChannel );
        pSession->cbTransmitterProximityDetectionChannel = 0;
        ChkMem( pSession->pbTransmitterProximityDetectionChannel = (DRM_BYTE*)Oem_MemAlloc( cbTransmitterProximityDetectionChannel ) );
        pSession->cbTransmitterProximityDetectionChannel = cbTransmitterProximityDetectionChannel;
    }
    OEM_SECURE_ZERO_MEMORY( pSession->pbTransmitterProximityDetectionChannel, pSession->cbTransmitterProximityDetectionChannel );

    cbReceiverCertificate = XBBA_TO_CB( f_poRegistrationRequestMessage->BasicInformation.xbbaReceiverCertificate );

    if( pSession->cbReceiverCertificate <  cbReceiverCertificate
     || pSession->pbReceiverCertificate == NULL )
    {
        SAFE_OEM_FREE( pSession->pbReceiverCertificate );
        pSession->cbReceiverCertificate = 0;
        ChkMem( pSession->pbReceiverCertificate = (DRM_BYTE*)Oem_MemAlloc( cbReceiverCertificate ) );
        pSession->cbReceiverCertificate = cbReceiverCertificate;
    }
    OEM_SECURE_ZERO_MEMORY( pSession->pbReceiverCertificate, pSession->cbReceiverCertificate );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_RegistrationRequest_CheckClockDrift(
    __inout                                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout                                                 DRM_PRND_TRANSMITTER_SESSION_CONTEXT             *f_poPrndTransmitterSessionContext,
    __in                                                    DRM_UINT64                                        f_qwReceiverCurrentTime ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_RegistrationRequest_CheckClockDrift(
    __inout                                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout                                                 DRM_PRND_TRANSMITTER_SESSION_CONTEXT             *f_poPrndTransmitterSessionContext,
    __in                                                    DRM_UINT64                                        f_qwReceiverCurrentTime )
{
    DRM_RESULT      dr                  = DRM_SUCCESS;
    DRMFILETIME     ftTransmitter       = {0};
    DRM_UINT64      qwTransmitter       = DRM_UI64LITERAL( 0, 0 );
    DRMFILETIME     ftReceiver          = {0};
    DRM_UINT64      qwReceiver          = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64      qwDiff              = DRM_UI64LITERAL( 0, 0 );
    DRM_INT64       i64Diff             = DRM_I64LITERAL( 0, 0 );
    DRM_INT64       i64DiffSecs         = DRM_I64LITERAL( 0, 0 );
    DRM_BOOL        fReceiverInFuture   = FALSE;
    DRM_DWORD       dwDiffSecs          = 0;

    /* f_qwReceiverCurrentTime from protocol is seconds since 1970.  To do time math, we convert to tics since 1601 (filetime). */
    CREATE_FILE_TIME( DRM_UI64Low32( f_qwReceiverCurrentTime ), ftReceiver );
    FILETIME_TO_UI64( ftReceiver, qwReceiver );

    Oem_Clock_GetSystemTimeAsFileTime( f_poPrndTransmitterContext->pBBXContext->pOEMContext, &ftTransmitter );
    FILETIME_TO_UI64( ftTransmitter, qwTransmitter );

    /* How far out of sync are the Transmitter and Receiver clocks?  Find out here. */
    if( DRM_UI64Les( qwTransmitter, qwReceiver ) )
    {
        fReceiverInFuture = TRUE;
        ChkDR( DRM_UInt64Sub( qwReceiver, qwTransmitter, &qwDiff ) );
    }
    else
    {
        fReceiverInFuture = FALSE;
        ChkDR( DRM_UInt64Sub( qwTransmitter, qwReceiver, &qwDiff ) );
    }
    ChkDR( DRM_UInt64ToInt64( qwDiff, &i64Diff ) );
    i64DiffSecs = DRM_I64Div( i64Diff, DRM_I64( C_TICS_PER_SECOND ) );  /* They are i64DiffSecs seconds out of sync. */

#if DRM_SUPPORT_NATIVE_64BIT_TYPES
    DRMCASSERT( DRM_I64Les( DRM_PRND_MAX_CLOCK_DRIFT_AT_REGISTRATION_SECS, DRM_I64Asgn( 0, MAX_UNSIGNED_TYPE(DRM_DWORD) ) ) );
#endif
    if( DRM_I64Les( DRM_PRND_MAX_CLOCK_DRIFT_AT_REGISTRATION_SECS, i64DiffSecs ) )
    {
        /* The clocks are so far out of sync that this Transmitter can't talk to this Receiver. Fail Registration. */
        ChkDR( DRM_E_PRND_CLOCK_OUT_OF_SYNC );
    }

    /* This will never fail since we know the difference is <= DRM_PRND_MAX_CLOCK_DRIFT_AT_REGISTRATION_SECS. */
    ChkDR( DRM_Int64ToDWord( i64DiffSecs, &dwDiffSecs ) );

    /* Allow the clocks to drift further apart by up to DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS until next Renewal. */
    f_poPrndTransmitterSessionContext->dwSubtractFromBeginDate = DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS;
    f_poPrndTransmitterSessionContext->dwAddToEndDate          = DRM_PRND_FURTHER_CLOCK_DRIFT_DURING_SESSION_SECS;

    if( fReceiverInFuture )
    {
        /* The Receiver's time is in the future, so licenses will expire early.  Add the difference to the end date.  */
        ChkDR( DRM_DWordAdd(
            f_poPrndTransmitterSessionContext->dwAddToEndDate,
            dwDiffSecs,
            &f_poPrndTransmitterSessionContext->dwAddToEndDate ) );
    }
    else
    {
        /* The Receiver's time is in the past, so licenses will not yet be valid.  Subtract the difference from the begin date.  */
        ChkDR( DRM_DWordAdd(
            f_poPrndTransmitterSessionContext->dwSubtractFromBeginDate,
            dwDiffSecs,
            &f_poPrndTransmitterSessionContext->dwSubtractFromBeginDate ) );
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_RegistrationRequest_Validate(
    __inout                                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbReqMessage )                     const DRM_BYTE                                         *f_pbReqMessage,
    __in                                                    DRM_DWORD                                         f_cbReqMessage,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                   DRM_BOOL                                         *f_pfRenewal ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_RegistrationRequest_Validate(
    __inout                                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbReqMessage )                     const DRM_BYTE                                         *f_pbReqMessage,
    __in                                                    DRM_DWORD                                         f_cbReqMessage,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                   DRM_BOOL                                         *f_pfRenewal )
{
    DRM_RESULT                               dr                                 = DRM_SUCCESS;
    DRM_PRND_REGISTRATION_REQUEST_MESSAGE    oPrndRegistrationRequestMessage    = {0};
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT      oPrndInternalSignatureContext      = {0};
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession                           = NULL;

    DRMASSERT( f_poPrndTransmitterContext   != NULL );
    DRMASSERT( f_pbReqMessage               != NULL );
    DRMASSERT( f_pfRenewal                  != NULL );
    __analysis_assume( f_poPrndTransmitterContext   != NULL );
    __analysis_assume( f_pbReqMessage               != NULL );
    __analysis_assume( f_pfRenewal                  != NULL );
    *f_pfRenewal = FALSE;

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    oPrndInternalSignatureContext.poPrndTransmitterContext = f_poPrndTransmitterContext;
    ChkDR( DRM_PRND_RegistrationRequestMessage_Parse(
        pSession->rgbStack,
        SIZEOF( pSession->rgbStack ),
        f_pbReqMessage,
        f_cbReqMessage,
        &oPrndRegistrationRequestMessage,
        &DRM_PRND_INTERNAL_FormatSignatureCallback,
        &oPrndInternalSignatureContext ) );

    ChkDR( _Transmitter_RegistrationRequest_CheckClockDrift(
        f_poPrndTransmitterContext,
        pSession,
        oPrndRegistrationRequestMessage.BasicInformation.qwReceiverCurrentTime ) );

    if( 0 != DRMCRT_memcmp( &oPrndRegistrationRequestMessage.BasicInformation.guidPreviousSessionID, &g_guidNull, SIZEOF( DRM_GUID ) ) )
    {
        /* Registration Request Message is attempting to renew an existing session. */
        if( 0 != DRMCRT_memcmp( &pSession->SessionID, &oPrndRegistrationRequestMessage.BasicInformation.guidPreviousSessionID, SIZEOF( DRM_ID ) ) )
        {
            /* This is not the session the Receiver is trying to renew. */
            if( 0 != DRMCRT_memcmp( &pSession->SessionID, &g_guidNull, SIZEOF( DRM_ID ) ) )
            {
                /* This is an existing, but incorrect, session.  Caller should not have even attempted to process this message with this session. */
                ChkDR( DRM_E_PRND_SESSION_ID_INVALID );
            }
            else
            {
                /* This is a new session.  Caller needs to know the session ID and then re-process the message with the correct session. */
                OEM_SECURE_MEMCPY(
                    &pSession->SessionID,
                    &oPrndRegistrationRequestMessage.BasicInformation.guidPreviousSessionID,
                    SIZEOF( pSession->SessionID ) );
                ChkDR( DRM_E_PRND_CANNOT_RENEW_USING_NEW_SESSION );
            }
        }
        else
        {
            /* This IS the session the Receiver is trying to renew. */
            *f_pfRenewal = TRUE;
        }
    }

    /* Per Protocol Spec: guidInitialLicenseMessageID must always be non-zero */
    ChkBOOL( DRMCRT_memcmp( &oPrndRegistrationRequestMessage.BasicInformation.guidInitialLicenseMessageID, &g_guidNull, SIZEOF( DRM_GUID ) ) != 0, DRM_E_PRND_MESSAGE_INVALID );

    /* Always update our known receiver RIV. */
    pSession->dwReceiverRIV = oPrndRegistrationRequestMessage.BasicInformation.dwReceiverRIV;

    if( *f_pfRenewal )
    {
        /* If this fails, the Receiver tried to renew with a different certificate. The whole message is invalid. */
        ChkBOOL( ( pSession->pbReceiverCertificate != NULL )
              && ( pSession->cbReceiverCertificate == XBBA_TO_CB( oPrndRegistrationRequestMessage.BasicInformation.xbbaReceiverCertificate ) )
              && ( 0 == DRMCRT_memcmp(
                pSession->pbReceiverCertificate,
                XBBA_TO_PB( oPrndRegistrationRequestMessage.BasicInformation.xbbaReceiverCertificate ),
                XBBA_TO_CB( oPrndRegistrationRequestMessage.BasicInformation.xbbaReceiverCertificate ) ) ),
                DRM_E_PRND_MESSAGE_INVALID );

        /* If this fails, the Receiver tried to change the Registration Request Flags during renewal. The whole message is invalid. */
        ChkBOOL( pSession->dwRegistrationRequestFlags == oPrndRegistrationRequestMessage.BasicInformation.dwFlags, DRM_E_PRND_MESSAGE_INVALID );

        /*
        ** We revalidate the receiver certificate in order to provide a callback to application with cert/custom data for re-authorization (if required)
        ** as well as check for things like certificate revocation and expiration (which may have changed since last registration/renewal).
        */
        ChkDR( _ValidateReceiverCertificate(
            f_poPrndTransmitterContext,
            &oPrndRegistrationRequestMessage,
            f_pfnDataCallback,
            f_pvDataCallbackContext ) );

        /* If this fails, the renewal came too late.  The Receiver has to start over with a new session. */
        ChkBOOL( _IsDeviceValid( f_poPrndTransmitterContext, pSession ), DRM_E_DEVICE_NOT_REGISTERED );

        /* Now that the receiver has started renewal, it MUST re-perform proximity detection before we will issue it another license! */
        pSession->ePrndProximityDetectionState = DRM_PRND_PROXIMITY_DETECTION_STATE_INIT;
    }
    else
    {
        ChkDR( _SetupInitialTransmitterContextBuffers(
            f_poPrndTransmitterContext,
            f_poPrndTransmitterContext->pBBXContext->pOEMContext,
            &oPrndRegistrationRequestMessage ) );

        ChkDR( _ValidateReceiverCertificate(
            f_poPrndTransmitterContext,
            &oPrndRegistrationRequestMessage,
            f_pfnDataCallback,
            f_pvDataCallbackContext ) );

        ChkDR( Oem_Ndt_GetProximityDetectionType( &pSession->eProximityDetectionType ) );
        ChkBOOL( 0 != ( pSession->eProximityDetectionType & oPrndRegistrationRequestMessage.BasicInformation.dwSupportedProximityDetectionTypes ),
            DRM_E_PRND_PROXIMITY_DETECTION_REQUEST_CHANNEL_TYPE_UNSUPPORTED );

        pSession->dwRegistrationRequestFlags = oPrndRegistrationRequestMessage.BasicInformation.dwFlags;
    }

    /* Always update guidInitialLicenseMessageID. */
    OEM_SECURE_MEMCPY(
        &pSession->guidInitialLicenseMessageID,
        &oPrndRegistrationRequestMessage.BasicInformation.guidInitialLicenseMessageID,
        SIZEOF( pSession->guidInitialLicenseMessageID ) );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_RegistrationResponse_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbRespMessage )                           DRM_BYTE                                        **f_ppbRespMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbRespMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_RegistrationResponse_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbRespMessage )                           DRM_BYTE                                        **f_ppbRespMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbRespMessage )
{
    DRM_RESULT                                               dr                                             = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                                   xbContext                                      = {0};
    DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION         oPrndRegistrationResponseBasicInformation      = {0};
    DRM_PRND_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION     oPrndRegistrationResponseProximityInformation  = {0};
    DRM_BYTE                                                *pbTransmitterCertificate                       = NULL;
    DRM_DWORD                                                cbTransmitterCertificate                       = 0;
    DRM_BYTE                                                *pbTransmitterRevInfo                           = NULL;
    DRM_DWORD                                                cbTransmitterRevInfo                           = 0;
    DRM_BYTE                                                *pbStack                                        = NULL;
    DRM_DWORD                                                cbStack                                        = 0;
    DRM_DWORD                                                dwTransmitterRIV                               = 0;
    DRM_PRND_CUSTOM_DATA                                     oPrndCustomData                                = {0};
    DRM_PRND_REGISTRATION_RESPONSE_TRANSMITTER_AUTH          oPrndRegistrationResponseTransmitterAuth       = {0};
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT                      oPrndInternalSignatureContext                  = {0};
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT                    *pSession                                       = NULL;
    DRM_BYTE                                                *pbRespMessage                                  = NULL;
    DRM_DWORD                                                cbRespMessage                                  = 0;
    SIGNATURE_P256                                           oTransmitterAuthSig                            = {0};

    DRMASSERT( f_poPrndTransmitterContext   != NULL );
    DRMASSERT( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    DRMASSERT( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    DRMASSERT( f_pcbRespMessage             != NULL );
    DRMASSERT( f_ppbRespMessage             != NULL );
    __analysis_assume( f_poPrndTransmitterContext   != NULL );
    __analysis_assume( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    __analysis_assume( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    __analysis_assume( f_pcbRespMessage             != NULL );
    __analysis_assume( f_ppbRespMessage             != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    ChkDR( DRM_PRND_INTERNAL_FetchCertAndRevInfo(
        f_poPrndTransmitterContext->pBBXContext->pOEMContext,
        f_poPrndTransmitterContext->pRevocationStoreContext,
        &pbStack,
        &cbStack,
        f_poPrndTransmitterContext->pBBXContext->pKeyFileContext,
        &pbTransmitterCertificate,
        &cbTransmitterCertificate,
        &pbTransmitterRevInfo,
        &cbTransmitterRevInfo,
        &dwTransmitterRIV ) );

    DRMASSERT( cbTransmitterCertificate > 0 == DRM_DEVICEASSETS_IsDeviceAssetsSupported() );

    ChkDR( DRM_PRND_RegistrationResponseMessage_Start( pbStack, cbStack, &xbContext ) );

    oPrndRegistrationResponseBasicInformation.fValid                                                        = TRUE;
    oPrndRegistrationResponseBasicInformation.dwFlags                                                       = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndRegistrationResponseBasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndRegistrationResponseBasicInformation.guidSessionID ) );
    if( dwTransmitterRIV > pSession->dwReceiverRIV )
    {
        oPrndRegistrationResponseBasicInformation.xbbaTransmitterRevInfo.pbDataBuffer                       = pbTransmitterRevInfo;
        oPrndRegistrationResponseBasicInformation.xbbaTransmitterRevInfo.cbData                             = cbTransmitterRevInfo;
    }
    oPrndRegistrationResponseBasicInformation.wMIMKEncryptionType                                           = DRM_PRND_ENCRYPTION_TYPE_ECC256_RECEIVER_PRND_ENCRYPTION_KEY;
    oPrndRegistrationResponseBasicInformation.xbbaMIMKEncryptedWithReceiverPubkey.pbDataBuffer              = pSession->rgbMIMKEncryptedWithReceiverPubkey;
    oPrndRegistrationResponseBasicInformation.xbbaMIMKEncryptedWithReceiverPubkey.cbData                    = SIZEOF( pSession->rgbMIMKEncryptedWithReceiverPubkey );

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_BASIC_INFORMATION,     &oPrndRegistrationResponseBasicInformation ) );

    {
        /* Check for override */
        DRM_CHAR    rgchId[ CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) ]                        = { 0 };
        DRM_DWORD   cchId                                                               = NO_OF(rgchId);
        DRM_CHAR    rgchConfigKey[ Oem_Config_NoTxAuth_Key_LEN + NO_OF(rgchId) + 1 ]    = { 0 };
        DRM_BYTE    rgbValue[ SIZEOF(DRM_DWORD) ]                                       = { 0 };
        DRM_DWORD   cbValue                                                             = SIZEOF(rgbValue);

        ChkDR( DRM_B64_EncodeA( (const DRM_BYTE*)&pSession->SessionID, SIZEOF(DRM_ID), rgchId, &cchId, 0 ) );
        OEM_SECURE_MEMCPY( rgchConfigKey, Oem_Config_NoTxAuth_Key, Oem_Config_NoTxAuth_Key_LEN );
        OEM_SECURE_MEMCPY( rgchConfigKey + Oem_Config_NoTxAuth_Key_LEN, rgchId, SIZEOF(rgchId) );
        dr = Oem_Config_Read( NULL, rgchConfigKey, rgbValue, &cbValue );

        /* Oem_Config_Read returns DRM_S_FALSE if no key is found */
        if( dr == DRM_SUCCESS )
        {
            DRMASSERT( cbValue == SIZEOF( DRM_DWORD ) );
            cchId = 0;
            if( !OEM_SECURE_ARE_EQUAL( rgbValue, &cchId, SIZEOF( DRM_DWORD ) ) )
            {
                /* Override is set: disable Tx Auth by setting cbTransmitterCertificate to 0 */
                cbTransmitterCertificate = 0;
            }
        }
        else
        {
            /* Ignore any errors */
            dr = DRM_SUCCESS;
        }
    }

    if( cbTransmitterCertificate > 0 )
    {
        DRM_BYTE    rgbToSign[SIZEOF(pSession->guidInitialLicenseMessageID) + SIZEOF(pSession->rgbMIMKEncryptedWithReceiverPubkey)] = {0};
        PUBKEY_P256 pubkey;

        oPrndRegistrationResponseTransmitterAuth.fValid                                     = TRUE;
        oPrndRegistrationResponseTransmitterAuth.xbbaTransmitterCertificate.pbDataBuffer    = pbTransmitterCertificate;
        oPrndRegistrationResponseTransmitterAuth.xbbaTransmitterCertificate.cbData          = cbTransmitterCertificate;
        oPrndRegistrationResponseTransmitterAuth.wTransmitterAuthSignatureType              = DRM_PRND_SIGNATURE_TYPE_ECC256_TRANSMITTER_SIGNING_KEY;

        /* Populate the signature by signing LMID | {MI|MK}PReceiver with Tx private signing key */
        OEM_SECURE_MEMCPY( rgbToSign,                                                 &pSession->guidInitialLicenseMessageID,        SIZEOF( pSession->guidInitialLicenseMessageID ) );
        OEM_SECURE_MEMCPY( &rgbToSign[SIZEOF(pSession->guidInitialLicenseMessageID)], &pSession->rgbMIMKEncryptedWithReceiverPubkey, SIZEOF( pSession->rgbMIMKEncryptedWithReceiverPubkey ) );
        ChkDR( DRM_BBX_ECC256_Sign(
            f_poPrndTransmitterContext->pBBXContext,
            rgbToSign,
            SIZEOF( rgbToSign ),
            &pubkey,
            (DRM_BYTE*)&oTransmitterAuthSig ) );
        oPrndRegistrationResponseTransmitterAuth.xbbaTransmitterAuthSignature.pbDataBuffer  = (DRM_BYTE*)&oTransmitterAuthSig;
        oPrndRegistrationResponseTransmitterAuth.xbbaTransmitterAuthSignature.cbData        = SIZEOF( oTransmitterAuthSig );

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_TRANSMITTER_AUTH, &oPrndRegistrationResponseTransmitterAuth ) );
    }

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

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_CUSTOM_DATA, &oPrndCustomData ) );
    }

    oPrndRegistrationResponseProximityInformation.fValid                                                    = TRUE;
    oPrndRegistrationResponseProximityInformation.dwProximityDetectionType                                  = pSession->eProximityDetectionType;
    oPrndRegistrationResponseProximityInformation.xbbaTransmitterProximityDetectionChannel.pbDataBuffer     = pSession->pbTransmitterProximityDetectionChannel;
    oPrndRegistrationResponseProximityInformation.xbbaTransmitterProximityDetectionChannel.cbData           = pSession->cbTransmitterProximityDetectionChannel;

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION, &oPrndRegistrationResponseProximityInformation ) );

    oPrndInternalSignatureContext.poPrndTransmitterContext = f_poPrndTransmitterContext;

    dr = DRM_PRND_RegistrationResponseMessage_Finish( &xbContext, NULL, &cbRespMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbRespMessage = (DRM_BYTE*)Oem_MemAlloc( cbRespMessage ) );
    ChkDR( DRM_PRND_RegistrationResponseMessage_Finish( &xbContext, pbRespMessage, &cbRespMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext ) );

    *f_pcbRespMessage = cbRespMessage;
    *f_ppbRespMessage = pbRespMessage;
    pbRespMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbRespMessage );
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_RegistrationRequest_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbReqMessage )                               const DRM_BYTE                                         *f_pbReqMessage,
    __in                                                              DRM_DWORD                                         f_cbReqMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_PRND_PROXIMITY_DETECTION_TYPE                *f_peProximityDetectionType,
    __deref_out_ecount( *f_pcbTransmitterProximityDetectionChannel )  DRM_BYTE                                        **f_ppbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pcbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;
    DRM_BOOL                                 fRenewal   = FALSE;
    DRM_DWORD                                cbRevInfo  = 0;
    DRM_BYTE                                *pbRevInfo  = 0;

    TRACE(( "DRM_PRND_Transmitter_RegistrationRequest_Process Entered" ));

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( f_pbReqMessage                              != NULL );
    ChkArg( f_cbReqMessage                               > 0    );
    ChkArg( f_peProximityDetectionType                  != NULL );
    ChkArg( f_ppbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pcbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pdwFlags                                  != NULL );
    ChkArg( !pSession->fReceivedRegistrationRequest );

    *f_ppbTransmitterProximityDetectionChannel = NULL;
    *f_pcbTransmitterProximityDetectionChannel = 0;

    ChkDR( _Transmitter_RegistrationRequest_Validate(
        f_poPrndTransmitterContext,
        f_pbReqMessage,
        f_cbReqMessage,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        &fRenewal ) );

    TRACE(( "DRM_PRND_Transmitter_RegistrationRequest_Process Info {fRenewal = %d}", fRenewal ? 1 : 0 ));

    /*
    ** We don't need to refetch the proximity detection channel, to generate a new session ID,
    ** or to create a new MI/MK when performing renewal, so skip the below block of code in the renewal case.
    ** Since the LicenseMessageID has changed, however, we can't give root/std licenses to the old session any more.
    */
    if( !fRenewal )
    {
        ChkDR( Oem_Ndt_GetProximityDetectionChannel(
            pSession->pbTransmitterProximityDetectionChannel,
            &pSession->cbTransmitterProximityDetectionChannel ) );

        ChkDR( _GenerateUniqueSessionID( f_poPrndTransmitterContext, &pSession->SessionID ) );

        DRMCASSERT( SIZEOF( pSession->rgbMIMKEncryptedWithReceiverPubkey ) == SIZEOF( CIPHERTEXT_P256 ) );

        if( pSession->handleMIMK == OEM_CRYPTO_HANDLE_INVALID )
        {
            dr = DRM_RVK_CheckRevInfoForExpiration(
                &f_poPrndTransmitterContext->pBBXContext->CryptoContext,
                f_poPrndTransmitterContext->pRevocationStoreContext,
                &g_guidRevocationTypeRevInfo2,
                &pSession->SessionID,
                NULL,
                &cbRevInfo );

            DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
            DRMASSERT( cbRevInfo > 0 );
            ChkMem( pbRevInfo = (DRM_BYTE*)Oem_MemAlloc( cbRevInfo ) );

            ChkDR( DRM_RVK_CheckRevInfoForExpiration(
                &f_poPrndTransmitterContext->pBBXContext->CryptoContext,
                f_poPrndTransmitterContext->pRevocationStoreContext,
                &g_guidRevocationTypeRevInfo2,
                &pSession->SessionID,
                pbRevInfo,
                &cbRevInfo ) );

            ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
                f_poPrndTransmitterContext->pBBXContext,
                DRM_BBX_KEY_PAIR_TYPE_MIMK,
                &pSession->oECC256PubKeyPrndEncryptReceiver,
                pSession->pbReceiverCertificate,
                pSession->cbReceiverCertificate,
                pSession->pbReceiverCertificateCRL,
                pSession->cbReceiverCertificateCRL,
                pbRevInfo,
                cbRevInfo,
                0,
                (CIPHERTEXT_P256*)pSession->rgbMIMKEncryptedWithReceiverPubkey,
                &pSession->handleMIMK ) );
        }
    }

    *f_peProximityDetectionType                = pSession->eProximityDetectionType;
    *f_ppbTransmitterProximityDetectionChannel = pSession->pbTransmitterProximityDetectionChannel;
    *f_pcbTransmitterProximityDetectionChannel = pSession->cbTransmitterProximityDetectionChannel;
    *f_pdwFlags                                = pSession->dwRegistrationRequestFlags;
    pSession->fReceivedRegistrationRequest     = TRUE;

ErrorExit:
    SAFE_OEM_FREE( pbRevInfo );
    TRACE(( "DRM_PRND_Transmitter_RegistrationRequest_Process Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_RegistrationResponse_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbRespMessage )                           DRM_BYTE                                        **f_ppbRespMessage,
    __out                                                             DRM_DWORD                                        *f_pcbRespMessage )
{
    DRM_RESULT                               dr             = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession       = NULL;
    DRM_BYTE                                *pbRespMessage  = NULL;
    DRM_DWORD                                cbRespMessage  = 0;

    TRACE(( "DRM_PRND_Transmitter_RegistrationResponse_Generate Entered" ));

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    ChkArg( f_pcbRespMessage                            != NULL );
    ChkArg( f_ppbRespMessage                            != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( pSession->fReceivedRegistrationRequest );
    pSession->fReceivedRegistrationRequest = FALSE;

    *f_ppbRespMessage = NULL;
    *f_pcbRespMessage = 0;

    ChkDR( _Transmitter_RegistrationResponse_Generate(
        f_poPrndTransmitterContext,
        f_pCustomDataTypeID,
        f_pbCustomData,
        f_cbCustomData,
        f_dwFlags,
        &pbRespMessage,
        &cbRespMessage ) );

    ChkDR( _RegisterDevice( f_poPrndTransmitterContext ) );

    Oem_Clock_GetSystemTimeAsFileTime( f_poPrndTransmitterContext->pBBXContext->pOEMContext, &pSession->ftRegistrationTimeoutStart );

    OEM_SECURE_MEMCPY(
        &pSession->guidCurrentLicenseMessageID,
        &pSession->guidInitialLicenseMessageID,
        SIZEOF( pSession->guidCurrentLicenseMessageID ) );

    *f_pcbRespMessage = cbRespMessage;
    *f_ppbRespMessage = pbRespMessage;
    pbRespMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbRespMessage );
    TRACE(( "DRM_PRND_Transmitter_RegistrationResponse_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_RegistrationError_Generate(
    __inout_opt                                                       DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbErrMessage )                            DRM_BYTE                                        **f_ppbErrMessage,
    __out                                                             DRM_DWORD                                        *f_pcbErrMessage,
    __in                                                              DRM_RESULT                                        f_drResult )
{
    DRM_RESULT                                       dr                                     = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                           xbContext                              = {0};
    DRM_PRND_REGISTRATION_ERROR_BASIC_INFORMATION    oPrndRegistrationErrorBasicInformation = {0};
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT            *pSession                               = NULL;
    DRM_BYTE                                        *pbErrMessage                           = NULL;
    DRM_DWORD                                        cbErrMessage                           = 0;
    DRM_BYTE                                        *pbStack                                = NULL;

    TRACE(( "DRM_PRND_Transmitter_RegistrationError_Generate Entered {f_drResult = 0x%x}", f_drResult ));

    if( f_poPrndTransmitterContext != NULL )
    {
        ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    }
    ChkArg( f_pcbErrMessage != NULL );
    ChkArg( f_ppbErrMessage != NULL );
    ChkArg( DRM_FAILED( f_drResult ) );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbErrMessage = 0;
    *f_ppbErrMessage = NULL;

    if( pSession == NULL )
    {
        ChkMem( pbStack = (DRM_BYTE*)Oem_MemAlloc( DRM_PRND_STACK_SIZE ) );

        ChkDR( DRM_PRND_RegistrationErrorMessage_Start(
            pbStack,
            DRM_PRND_STACK_SIZE,
            &xbContext ) );
    }
    else
    {
        ChkDR( DRM_PRND_RegistrationErrorMessage_Start(
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            &xbContext ) );
    }

    oPrndRegistrationErrorBasicInformation.fValid    = TRUE;
    oPrndRegistrationErrorBasicInformation.dwFlags   = f_dwFlags;
    oPrndRegistrationErrorBasicInformation.dwResult  = (DRM_DWORD)f_drResult;

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_ERROR_BASIC_INFORMATION, &oPrndRegistrationErrorBasicInformation ) );

    dr = DRM_PRND_RegistrationErrorMessage_Finish( &xbContext, NULL, &cbErrMessage );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbErrMessage = (DRM_BYTE*)Oem_MemAlloc( cbErrMessage ) );
    ChkDR( DRM_PRND_RegistrationErrorMessage_Finish( &xbContext, pbErrMessage, &cbErrMessage ) );

    *f_pcbErrMessage = cbErrMessage;
    *f_ppbErrMessage = pbErrMessage;
    pbErrMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbErrMessage );
    SAFE_OEM_FREE( pbStack );
    TRACE(( "DRM_PRND_Transmitter_RegistrationError_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionStart_Validate(
    __inout                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_bcount( f_cbStack )                         DRM_BYTE                                         *f_pbStack,
    __in                                                DRM_DWORD                                         f_cbStack,
    __in_bcount( f_cbPDStartMessage )             const DRM_BYTE                                         *f_pbPDStartMessage,
    __in                                                DRM_DWORD                                         f_cbPDStartMessage,
    __out                                               DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE       *f_pPrndProximityDetectionStartMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionStart_Validate(
    __inout                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_bcount( f_cbStack )                         DRM_BYTE                                         *f_pbStack,
    __in                                                DRM_DWORD                                         f_cbStack,
    __in_bcount( f_cbPDStartMessage )             const DRM_BYTE                                         *f_pbPDStartMessage,
    __in                                                DRM_DWORD                                         f_cbPDStartMessage,
    __out                                               DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE       *f_pPrndProximityDetectionStartMessage )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

    DRMASSERT( f_poPrndTransmitterContext            != NULL );
    DRMASSERT( f_pbPDStartMessage                    != NULL );
    DRMASSERT( f_cbPDStartMessage                     > 0    );
    DRMASSERT( f_pbStack                             != NULL );
    DRMASSERT( f_cbStack                              > 0    );
    DRMASSERT( f_pPrndProximityDetectionStartMessage != NULL );
    __analysis_assume( f_poPrndTransmitterContext            != NULL );
    __analysis_assume( f_pbPDStartMessage                    != NULL );
    __analysis_assume( f_cbPDStartMessage                     > 0    );
    __analysis_assume( f_pbStack                             != NULL );
    __analysis_assume( f_cbStack                              > 0    );
    __analysis_assume( f_pPrndProximityDetectionStartMessage != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    ChkDR( DRM_PRND_ProximityDetectionStartMessage_Parse(
        f_pbStack,
        f_cbStack,
        f_pbPDStartMessage,
        f_cbPDStartMessage,
        f_pPrndProximityDetectionStartMessage ) );

    ChkBOOL( 0 == DRMCRT_memcmp(
        &f_pPrndProximityDetectionStartMessage->BasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( f_pPrndProximityDetectionStartMessage->BasicInformation.guidSessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionChallenge_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDChlgMessage )                         DRM_BYTE                                        **f_ppbPDChlgMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbPDChlgMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionChallenge_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDChlgMessage )                         DRM_BYTE                                        **f_ppbPDChlgMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbPDChlgMessage )
{
    DRM_RESULT                                                   dr                                                 = DRM_SUCCESS;
    DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION     oPrndProximityDetectionChallengeBasicInformation   = {0};
    DRM_XB_BUILDER_CONTEXT                                       xbContext                                          = {0};
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT                        *pSession                                           = NULL;
    DRM_BYTE                                                    *pbPDChlgMessage                                    = NULL;
    DRM_DWORD                                                    cbPDChlgMessage                                    = 0;

    DRMASSERT( f_poPrndTransmitterContext   != NULL );
    DRMASSERT( f_pbStack                    != NULL );
    DRMASSERT( f_cbStack                     > 0    );
    DRMASSERT( f_pcbPDChlgMessage           != NULL );
    DRMASSERT( f_ppbPDChlgMessage           != NULL );
    __analysis_assume( f_poPrndTransmitterContext   != NULL );
    __analysis_assume( f_pbStack                    != NULL );
    __analysis_assume( f_cbStack                     > 0    );
    __analysis_assume( f_pcbPDChlgMessage           != NULL );
    __analysis_assume( f_ppbPDChlgMessage           != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    ChkDR( DRM_PRND_ProximityDetectionChallengeMessage_Start( f_pbStack, f_cbStack, &xbContext ) );

    ChkDR( Oem_Random_GetBytes( f_poPrndTransmitterContext->pBBXContext->pOEMContext, (DRM_BYTE*)&pSession->Nonce, SIZEOF( pSession->Nonce ) ) );

    oPrndProximityDetectionChallengeBasicInformation.fValid     = TRUE;
    oPrndProximityDetectionChallengeBasicInformation.dwFlags    = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndProximityDetectionChallengeBasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndProximityDetectionChallengeBasicInformation.guidSessionID ) );
    OEM_SECURE_MEMCPY(
        &oPrndProximityDetectionChallengeBasicInformation.guidNonce,
        &pSession->Nonce,
        SIZEOF( oPrndProximityDetectionChallengeBasicInformation.guidNonce ) );

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION, &oPrndProximityDetectionChallengeBasicInformation ) );

    dr = DRM_PRND_ProximityDetectionChallengeMessage_Finish( &xbContext, NULL, &cbPDChlgMessage );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbPDChlgMessage = (DRM_BYTE*)Oem_MemAlloc( cbPDChlgMessage ) );
    ChkDR( DRM_PRND_ProximityDetectionChallengeMessage_Finish( &xbContext, pbPDChlgMessage, &cbPDChlgMessage ) );

    *f_pcbPDChlgMessage = cbPDChlgMessage;
    *f_ppbPDChlgMessage = pbPDChlgMessage;
    pbPDChlgMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbPDChlgMessage );
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionResult_Generate(
    __in                                                        const DRM_ID                                           *f_pSessionID,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __in                                                              DRM_RESULT                                        f_drResult,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbPDResultMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionResult_Generate(
    __in                                                        const DRM_ID                                           *f_pSessionID,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __in                                                              DRM_RESULT                                        f_drResult,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbPDResultMessage )
{
    DRM_RESULT                                               dr                                             = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                                   xbContext                                      = {0};
    DRM_PRND_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION    oPrndProximityDetectionResultBasicInformation  = {0};
    DRM_BYTE                                                *pbPDResultMessage                              = NULL;
    DRM_DWORD                                                cbPDResultMessage                              = 0;

    DRMASSERT( f_pSessionID         != NULL );
    DRMASSERT( f_pbStack            != NULL );
    DRMASSERT( f_cbStack             > 0    );
    DRMASSERT( f_pcbPDResultMessage != NULL );
    DRMASSERT( f_ppbPDResultMessage != NULL );
    __analysis_assume( f_pSessionID         != NULL );
    __analysis_assume( f_pbStack            != NULL );
    __analysis_assume( f_cbStack             > 0    );
    __analysis_assume( f_pcbPDResultMessage != NULL );
    __analysis_assume( f_ppbPDResultMessage != NULL );

    ChkDR( DRM_PRND_ProximityDetectionResultMessage_Start( f_pbStack, f_cbStack, &xbContext ) );

    oPrndProximityDetectionResultBasicInformation.fValid    = TRUE;
    oPrndProximityDetectionResultBasicInformation.dwFlags   = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndProximityDetectionResultBasicInformation.guidSessionID,
        f_pSessionID,
        SIZEOF( oPrndProximityDetectionResultBasicInformation.guidSessionID ) );
    oPrndProximityDetectionResultBasicInformation.dwResult  = (DRM_DWORD)f_drResult;

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, &oPrndProximityDetectionResultBasicInformation ) );

    dr = DRM_PRND_ProximityDetectionResultMessage_Finish( &xbContext, NULL, &cbPDResultMessage );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbPDResultMessage = (DRM_BYTE*)Oem_MemAlloc( cbPDResultMessage ) );
    ChkDR( DRM_PRND_ProximityDetectionResultMessage_Finish( &xbContext, pbPDResultMessage, &cbPDResultMessage ) );

    *f_pcbPDResultMessage = cbPDResultMessage;
    *f_ppbPDResultMessage = pbPDResultMessage;
    pbPDResultMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbPDResultMessage );
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ProximityDetectionResult_Generate(
    __inout_opt                                                       DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pSessionID,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDResultMessage,
    __in                                                              DRM_RESULT                                        f_drResult )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;
    DRM_BYTE                                *pbStack    = NULL;

    TRACE(( "DRM_PRND_Transmitter_ProximityDetectionResult_Generate Entered {f_drResult = 0x%x}", f_drResult ));

    ChkArg( ( f_poPrndTransmitterContext == NULL ) != ( f_pSessionID == NULL ) );
    if( f_poPrndTransmitterContext != NULL )
    {
        ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    }
    ChkArg( f_pcbPDResultMessage != NULL );
    ChkArg( f_ppbPDResultMessage != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( DRM_FAILED( f_drResult )
        || ( ( pSession != NULL )
          && ( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED ) ) );

    *f_pcbPDResultMessage = 0;
    *f_ppbPDResultMessage = NULL;

    if( pSession == NULL )
    {
        ChkMem( pbStack = (DRM_BYTE*)Oem_MemAlloc( DRM_PRND_STACK_SIZE ) );

        ChkDR( _Transmitter_ProximityDetectionResult_Generate(
            f_pSessionID,
            pbStack,
            DRM_PRND_STACK_SIZE,
            f_dwFlags,
            f_drResult,
            f_ppbPDResultMessage,
            f_pcbPDResultMessage ) );
    }
    else
    {
        pSession->drProximityResult = f_drResult;

        ChkDR( _Transmitter_ProximityDetectionResult_Generate(
            &pSession->SessionID,
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            f_dwFlags,
            pSession->drProximityResult,
            f_ppbPDResultMessage,
            f_pcbPDResultMessage ) );
    }

ErrorExit:
    SAFE_OEM_FREE( pbStack );
    TRACE(( "DRM_PRND_Transmitter_ProximityDetectionResult_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ProximityDetectionStart_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbPDStartMessage )                           const DRM_BYTE                                         *f_pbPDStartMessage,
    __in                                                              DRM_DWORD                                         f_cbPDStartMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDChlgMessage )                         DRM_BYTE                                        **f_ppbPDChlgMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDChlgMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                                   dr                                     = DRM_SUCCESS;
    DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE   oPrndProximityDetectionStartMessage    = {0};
    DRM_BOOL                                     fSetLastResult                         = FALSE;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT        *pSession                               = NULL;
    DRM_BOOL                                     fTimerStarted                          = FALSE;

    TRACE(( "DRM_PRND_Transmitter_ProximityDetectionStart_Process Entered" ));

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( f_pbPDStartMessage  != NULL );
    ChkArg( f_cbPDStartMessage   > 0    );
    ChkArg( f_pcbPDChlgMessage  != NULL );
    ChkArg( f_ppbPDChlgMessage  != NULL );
    ChkArg( f_pdwFlags          != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbPDChlgMessage = 0;
    *f_ppbPDChlgMessage = NULL;

    if( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_INIT )
    {
        pSession->ePrndProximityDetectionState = DRM_PRND_PROXIMITY_DETECTION_STATE_REGISTERED;
    }

    fSetLastResult = TRUE;

    ChkBOOL( !_HasDeviceTimedOut( f_poPrndTransmitterContext, pSession ), DRM_E_DEVICE_NOT_REGISTERED );

    ChkDR( _Transmitter_ProximityDetectionStart_Validate(
        f_poPrndTransmitterContext,
        pSession->rgbStack,
        SIZEOF( pSession->rgbStack ),
        f_pbPDStartMessage,
        f_cbPDStartMessage,
        &oPrndProximityDetectionStartMessage ) );

    fSetLastResult = FALSE;

    if( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED )
    {
        /* When proximity detection is verified (i.e. already successful), just send a result message */
        DRMASSERT( DRM_SUCCEEDED( pSession->drProximityResult ) );
        ChkDR( _Transmitter_ProximityDetectionResult_Generate(
            &pSession->SessionID,
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            f_dwFlags,
            pSession->drProximityResult,
            f_ppbPDChlgMessage,
            f_pcbPDChlgMessage ) );
        TRACE(( "DRM_PRND_Transmitter_ProximityDetectionStart_Process Info {Directly Returning ProximityDetectionResult}" ));
    }
    else
    {
        ChkDR( _Transmitter_ProximityDetectionChallenge_Generate(
            f_poPrndTransmitterContext,
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            f_dwFlags,
            f_ppbPDChlgMessage,
            f_pcbPDChlgMessage ) );

        /* Note: Only trace BEFORE the timer has started.  The proximity detection timer of 7ms is extremely performance sensitive. */
        fTimerStarted = TRUE;
        TRACE(( "DRM_PRND_Transmitter_ProximityDetectionStart_Process Exited {dr = 0x%x}", dr ));

        /* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms on Desktop per MSDN.  We need 1 ms resolution. */
        pSession->qwProximityStartTimeMilliseconds  =
            DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                      Oem_Clock_QueryPerformanceCounter( f_poPrndTransmitterContext->pBBXContext->pOEMContext ) ),
                         Oem_Clock_QueryPerformanceFrequency( f_poPrndTransmitterContext->pBBXContext->pOEMContext ) );
        pSession->drProximityResult                 = DRM_E_LOGICERR;
        pSession->ePrndProximityDetectionState      = DRM_PRND_PROXIMITY_DETECTION_STATE_CHALLENGED;
    }

    *f_pdwFlags = oPrndProximityDetectionStartMessage.BasicInformation.dwFlags;

ErrorExit:
    if( fSetLastResult )
    {
        DRMASSERT( DRM_FAILED( dr ) );
        pSession->drProximityResult = dr;
    }
    if( !fTimerStarted )
    {
        /* Note: Do NOT trace if the timer has started.  The proximity detection timer of 7ms is extremely performance sensitive. */
        TRACE(( "DRM_PRND_Transmitter_ProximityDetectionStart_Process Exited {dr = 0x%x}", dr ));
    }
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionResponse_Validate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in_bcount( f_cbPDRespMessage )                            const DRM_BYTE                                         *f_pbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_cbPDRespMessage,
    __in                                                              DRM_UINT64                                        f_qwNowMilliseconds,
    __out                                                             DRM_DWORD                                        *f_pdwFlags ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_ProximityDetectionResponse_Validate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_bcount( f_cbStack )                                       DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __in_bcount( f_cbPDRespMessage )                            const DRM_BYTE                                         *f_pbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_cbPDRespMessage,
    __in                                                              DRM_UINT64                                        f_qwNowMilliseconds,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                                       dr                                             = DRM_SUCCESS;
    DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE    oPrndProximityDetectionResponseMessage         = {0};
    DRM_BOOL                                         fNonceVerified                                 = FALSE;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT            *pSession                                       = NULL;
    DRM_ID                                           NonceOnTxEncryptedWithMK                       = {0};

    DRMASSERT( f_poPrndTransmitterContext   != NULL );
    DRMASSERT( f_pbStack                    != NULL );
    DRMASSERT( f_cbStack                     > 0    );
    DRMASSERT( f_pbPDRespMessage            != NULL );
    DRMASSERT( f_cbPDRespMessage             > 0    );
    DRMASSERT( f_pdwFlags                   != NULL );
    __analysis_assume( f_poPrndTransmitterContext   != NULL );
    __analysis_assume( f_pbStack                    != NULL );
    __analysis_assume( f_cbStack                     > 0    );
    __analysis_assume( f_pbPDRespMessage            != NULL );
    __analysis_assume( f_cbPDRespMessage             > 0    );
    __analysis_assume( f_pdwFlags                   != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    pSession->drProximityResult = DRM_E_LOGICERR;

    ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Parse(
        f_pbStack,
        f_cbStack,
        f_pbPDRespMessage,
        f_cbPDRespMessage,
        &oPrndProximityDetectionResponseMessage ) );

    ChkBOOL( 0 == DRMCRT_memcmp(
        &oPrndProximityDetectionResponseMessage.BasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndProximityDetectionResponseMessage.BasicInformation.guidSessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );

    ChkBOOL( oPrndProximityDetectionResponseMessage.BasicInformation.wNonceEncryptionType == DRM_PRND_ENCRYPTION_TYPE_AES_ECB_MK, DRM_E_PRND_MESSAGE_INVALID );

    fNonceVerified = FALSE;

    /* Encrypt the clear nonce */
    OEM_SECURE_MEMCPY(
        &NonceOnTxEncryptedWithMK,
        &pSession->Nonce,
        SIZEOF( NonceOnTxEncryptedWithMK ) );
    ChkDR( DRM_BBX_PRND_Tx_EncryptNonceWithAESECB(
        f_poPrndTransmitterContext->pBBXContext,
        pSession->handleMIMK,
        &NonceOnTxEncryptedWithMK ) );

    /* Compare with nonce from Rx */
    fNonceVerified = ( 0 == DRMCRT_memcmp( &NonceOnTxEncryptedWithMK, &oPrndProximityDetectionResponseMessage.BasicInformation.guidNonceEncryptedWithMK, SIZEOF( NonceOnTxEncryptedWithMK ) ) );

    if( !fNonceVerified )
    {
        pSession->drProximityResult = DRM_E_PRND_PROXIMITY_DETECTION_RESPONSE_INVALID;
    }
    else if( DRM_UI64Les( DRM_UI64HL( 0, DRM_PRND_PROXIMITY_RTT_THRESHOLD_MS ), DRM_UI64Sub( f_qwNowMilliseconds, pSession->qwProximityStartTimeMilliseconds ) ) )
    {
        pSession->drProximityResult = DRM_E_PRND_PROXIMITY_DETECTION_RESPONSE_TIMEOUT;
    }
    else
    {
        pSession->drProximityResult = DRM_SUCCESS;
    }
    pSession->ePrndProximityDetectionState = DRM_PRND_PROXIMITY_DETECTION_STATE_COMPLETE;

    *f_pdwFlags = oPrndProximityDetectionResponseMessage.BasicInformation.dwFlags;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ProximityDetectionResponse_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbPDRespMessage )                            const DRM_BYTE                                         *f_pbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_cbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                               dr                 = DRM_SUCCESS;
    DRM_UINT64                               qwNowMilliseconds  = DRM_UI64LITERAL( 0, 0 );
    DRM_BOOL                                 fSetLastResult     = FALSE;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession           = NULL;
    DRM_BOOL                                 fExitTrace         = FALSE;

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( f_pbPDRespMessage       != NULL );
    ChkArg( f_cbPDRespMessage        > 0    );
    ChkArg( f_pcbPDResultMessage    != NULL );
    ChkArg( f_ppbPDResultMessage    != NULL );
    ChkArg( f_pdwFlags              != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbPDResultMessage = 0;
    *f_ppbPDResultMessage = NULL;

    /* Get the time to compare against as early as possible */
    /* Oem_Clock_GetTickCount has resolution too low, often 10-16 ms on Desktop per MSDN.  We need 1 ms resolution. */
    qwNowMilliseconds =
        DRM_UI64Div( DRM_UI64Mul( DRM_UI64HL( 0, 1000 ),
                                  Oem_Clock_QueryPerformanceCounter( f_poPrndTransmitterContext->pBBXContext->pOEMContext ) ),
                     Oem_Clock_QueryPerformanceFrequency( f_poPrndTransmitterContext->pBBXContext->pOEMContext ) );

    /* Note: Do NOT trace until the timer has stopped.  The proximity detection timer of 7ms is extremely performance sensitive. */
    TRACE(( "DRM_PRND_Transmitter_ProximityDetectionResponse_Process Entered" ));
    fExitTrace = TRUE;

    fSetLastResult = TRUE;

    ChkBOOL( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_CHALLENGED
          || pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_COMPLETE
          || pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED, DRM_E_DEVICE_NOT_REGISTERED );

    /* Treat rollback as timeout */
    ChkBOOL( !DRM_UI64Les( qwNowMilliseconds, pSession->qwProximityStartTimeMilliseconds ), DRM_E_PRND_PROXIMITY_DETECTION_RESPONSE_TIMEOUT );

    /* When proximity detection is verified (i.e. already successful), just send a result message */
    if( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_CHALLENGED
     || pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_COMPLETE )
    {
        ChkBOOL( !_HasDeviceTimedOut( f_poPrndTransmitterContext, pSession ), DRM_E_DEVICE_NOT_REGISTERED );

        ChkDR( _Transmitter_ProximityDetectionResponse_Validate(
            f_poPrndTransmitterContext,
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            f_pbPDRespMessage,
            f_cbPDRespMessage,
            qwNowMilliseconds,
            f_pdwFlags ) );

        if( DRM_SUCCEEDED( pSession->drProximityResult ) )
        {
            ChkDR( _MarkDeviceAsValid( f_poPrndTransmitterContext, pSession ) );
            pSession->ePrndProximityDetectionState = DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED;
        }
    }
    else
    {
        /* Just parse the message to get the flags */
        DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE    oPrndProximityDetectionResponseMessage = {0};
        ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Parse(
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            f_pbPDRespMessage,
            f_cbPDRespMessage,
            &oPrndProximityDetectionResponseMessage ) );
        *f_pdwFlags = oPrndProximityDetectionResponseMessage.BasicInformation.dwFlags;
    }

    fSetLastResult = FALSE;

    ChkDR( _Transmitter_ProximityDetectionResult_Generate(
        &pSession->SessionID,
        pSession->rgbStack,
        SIZEOF( pSession->rgbStack ),
        f_dwFlags,
        pSession->drProximityResult,
        f_ppbPDResultMessage,
        f_pcbPDResultMessage ) );

ErrorExit:
    if( fSetLastResult )
    {
        DRMASSERT( DRM_FAILED( dr ) );
        pSession->drProximityResult = dr;
    }
    if( fExitTrace )
    {
        /* Note: Only trace on exit if we traced on entry (see above). */
        TRACE(( "DRM_PRND_Transmitter_ProximityDetectionResponse_Process Exited {dr = 0x%x}", dr ));
    }
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_LicenseRequest_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbLicReqMessage )                            const DRM_BYTE                                         *f_pbLicReqMessage,
    __in                                                              DRM_DWORD                                         f_cbLicReqMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __in_opt                                                          Drm_Prnd_Content_Identifier_Callback              f_pfnContentIdentifierCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvContentIdentifierCallbackContext,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                               dr                             = DRM_SUCCESS;
    DRM_PRND_LICENSE_REQUEST_MESSAGE         oPrndLicenseRequestMessage     = {0};
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT      oPrndInternalSignatureContext  = {0};
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession                       = NULL;

    TRACE(( "DRM_PRND_Transmitter_LicenseRequest_Process Entered" ));

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( f_pbLicReqMessage   != NULL );
    ChkArg( f_cbLicReqMessage    > 0 );
    ChkArg( f_pdwFlags          != NULL );

    DRMSIZEASSERT( SIZEOF( DRM_GUID ), SIZEOF( DRM_KID ) );
    DRMSIZEASSERT( SIZEOF( DRM_GUID ), SIZEOF( DRM_LID ) );
    DRMSIZEASSERT( SIZEOF( DRM_GUID ), SIZEOF( DRM_ID )  );

    ChkBOOL( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED, DRM_E_DEVICE_NOT_REGISTERED );

    ChkBOOL( _IsDeviceValid( f_poPrndTransmitterContext, pSession ), DRM_E_DEVICE_NOT_REGISTERED );

    oPrndInternalSignatureContext.poPrndTransmitterContext = f_poPrndTransmitterContext;
    ChkDR( DRM_PRND_LicenseRequestMessage_Parse(
        pSession->rgbStack,
        SIZEOF( pSession->rgbStack ),
        f_pbLicReqMessage,
        f_cbLicReqMessage,
        &oPrndLicenseRequestMessage,
        &DRM_PRND_INTERNAL_FormatSignatureCallback,
        &oPrndInternalSignatureContext ) );
    pSession->dwReceiverRIV = oPrndLicenseRequestMessage.BasicInformation.dwReceiverRIV;

    ChkBOOL( 0 == DRMCRT_memcmp(
        &oPrndLicenseRequestMessage.BasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndLicenseRequestMessage.BasicInformation.guidSessionID ) ), DRM_E_PRND_SESSION_ID_INVALID );

    /* Verify that the receiver is not expired before allowing it to request any licenses. */
    ChkDR( DRM_PRND_INTERNAL_CheckRemoteCertificateExpiration( f_poPrndTransmitterContext->pBBXContext->pOEMContext, pSession->dwReceiverExpiration ) );

    ChkDR( DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
        f_poPrndTransmitterContext,
        NULL,
        NULL,
        oPrndLicenseRequestMessage.CustomData.fValid ? &oPrndLicenseRequestMessage.CustomData.customDataTypeID : NULL,
        oPrndLicenseRequestMessage.CustomData.fValid ? &oPrndLicenseRequestMessage.CustomData.xbbaCustomData   : NULL,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        NULL,
        NULL,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        NULL ) );

    /* Play with no qualifier is all that's supported in v1 of the protocol. */
    ChkBOOL( oPrndLicenseRequestMessage.LicenseInformation.wRequestedAction          == DRM_PRND_ACTION_PLAY,           DRM_E_PRND_LICENSE_REQUEST_INVALID_ACTION );
    ChkBOOL( oPrndLicenseRequestMessage.LicenseInformation.wRequestedActionQualifier == DRM_PRND_ACTION_QUALIFIER_NONE, DRM_E_PRND_LICENSE_REQUEST_INVALID_ACTION );

    /* Callback to application so it has the KID or can determine the KID based on custom CID */
    switch( oPrndLicenseRequestMessage.LicenseInformation.wContentIdentifierType )
    {
    case DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID:
        ChkBOOL( XBBA_TO_CB( oPrndLicenseRequestMessage.LicenseInformation.xbbaContentIdentifier ) == SIZEOF( DRM_KID ), DRM_E_PRND_MESSAGE_INVALID );
        if( f_pfnContentIdentifierCallback != NULL )
        {
            DRM_KID oKidRequestedContent;
            OEM_SECURE_MEMCPY(
                &oKidRequestedContent,
                XBBA_TO_PB( oPrndLicenseRequestMessage.LicenseInformation.xbbaContentIdentifier ),
                SIZEOF( oKidRequestedContent ) );
            ChkDR( f_pfnContentIdentifierCallback(
                f_pvContentIdentifierCallbackContext,
                (DRM_PRND_CONTENT_IDENTIFIER_TYPE)oPrndLicenseRequestMessage.LicenseInformation.wContentIdentifierType,
                XBBA_TO_PB( oPrndLicenseRequestMessage.LicenseInformation.xbbaContentIdentifier ),
                XBBA_TO_CB( oPrndLicenseRequestMessage.LicenseInformation.xbbaContentIdentifier ),
                &oKidRequestedContent ) );
        }
        break;
    case DRM_PRND_CONTENT_IDENTIFIER_TYPE_CUSTOM:
        ChkBOOL( f_pfnContentIdentifierCallback != NULL, DRM_E_PRND_LICENSE_REQUEST_CID_CALLBACK_REQUIRED );
        ChkDR( f_pfnContentIdentifierCallback(
            f_pvContentIdentifierCallbackContext,
            (DRM_PRND_CONTENT_IDENTIFIER_TYPE)oPrndLicenseRequestMessage.LicenseInformation.wContentIdentifierType,
            XBBA_TO_PB( oPrndLicenseRequestMessage.LicenseInformation.xbbaContentIdentifier ),
            XBBA_TO_CB( oPrndLicenseRequestMessage.LicenseInformation.xbbaContentIdentifier ),
            NULL ) );
        break;
    default:
        ChkDR( DRM_E_PRND_MESSAGE_INVALID );
        break;
    }

    *f_pdwFlags = oPrndLicenseRequestMessage.BasicInformation.dwFlags;

ErrorExit:
    TRACE(( "DRM_PRND_Transmitter_LicenseRequest_Process Exited {dr = 0x%x}", dr ));
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _Transmitter_LicenseTransmit_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __deref_inout_bcount( *f_pcbStack )                               DRM_BYTE                                        **f_ppbStack,
    __inout                                                           DRM_DWORD                                        *f_pcbStack,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )                  DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )            const DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )                  DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION    *f_rgPrndLicenseTransmitLicenseInformation,
    __in                                                              DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicTransmitMessage )                    DRM_BYTE                                        **f_ppbLicTransmitMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbLicTransmitMessage ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _Transmitter_LicenseTransmit_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __deref_inout_bcount( *f_pcbStack )                               DRM_BYTE                                        **f_ppbStack,
    __inout                                                           DRM_DWORD                                        *f_pcbStack,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )                  DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )            const DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )                  DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION    *f_rgPrndLicenseTransmitLicenseInformation,
    __in                                                              DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicTransmitMessage )                    DRM_BYTE                                        **f_ppbLicTransmitMessage,
    __inout                                                           DRM_DWORD                                        *f_pcbLicTransmitMessage )
{
    DRM_RESULT                                       dr                                     = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                           xbContext                              = {0};
    DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION      oPrndLicenseTransmitBasicInformation   = {0};
    DRM_DWORD                                        idxLicense                             = 0;
    DRM_BYTE                                        *pbTransmitterRevInfo                   = NULL;
    DRM_DWORD                                        cbTransmitterRevInfo                   = 0;
    DRM_DWORD                                        dwTransmitterRIV                       = 0;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT            *pSession                               = NULL;
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT              oPrndInternalSignatureContext          = {0};
    DRM_BYTE                                        *pbLicTransmitMessage                   = NULL;
    DRM_DWORD                                        cbLicTransmitMessage                   = 0;
    DRM_PRND_CUSTOM_DATA                             oPrndCustomData                        = {0};

    DRMASSERT( f_poPrndTransmitterContext != NULL );
    DRMASSERT( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    DRMASSERT( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    DRMASSERT( f_ppbStack                 != NULL );
    __analysis_assume( f_ppbStack                 != NULL );
    DRMASSERT( *f_ppbStack                != NULL );
    DRMASSERT( f_pcbStack                 != NULL );
    __analysis_assume( f_pcbStack                 != NULL );
    DRMASSERT( *f_pcbStack                 > 0    );
    DRMASSERT( f_pcbLicTransmitMessage    != NULL );
    DRMASSERT( f_ppbLicTransmitMessage    != NULL );
    __analysis_assume( f_poPrndTransmitterContext != NULL );
    __analysis_assume( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );
    __analysis_assume( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    __analysis_assume( *f_ppbStack                != NULL );
    __analysis_assume( *f_pcbStack                 > 0    );
    __analysis_assume( f_pcbLicTransmitMessage    != NULL );
    __analysis_assume( f_ppbLicTransmitMessage    != NULL );

    ChkDR( _GetSession( f_poPrndTransmitterContext, &pSession ) );

    ChkDR( DRM_PRND_INTERNAL_FetchCertAndRevInfo(
        f_poPrndTransmitterContext->pBBXContext->pOEMContext,
        f_poPrndTransmitterContext->pRevocationStoreContext,
        f_ppbStack,
        f_pcbStack,
        NULL,
        NULL,
        NULL,
        &pbTransmitterRevInfo,
        &cbTransmitterRevInfo,
        &dwTransmitterRIV ) );

    ChkDR( DRM_PRND_LicenseTransmitMessage_Start( *f_ppbStack, *f_pcbStack, &xbContext ) );

    oPrndLicenseTransmitBasicInformation.fValid                                     = TRUE;
    oPrndLicenseTransmitBasicInformation.dwFlags                                    = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndLicenseTransmitBasicInformation.guidSessionID,
        &pSession->SessionID,
        SIZEOF( oPrndLicenseTransmitBasicInformation.guidSessionID ) );
    OEM_SECURE_MEMCPY(
        &oPrndLicenseTransmitBasicInformation.guidCurrentLicenseMessageID,
        &pSession->guidCurrentLicenseMessageID,
        SIZEOF( oPrndLicenseTransmitBasicInformation.guidCurrentLicenseMessageID ) );

    if( dwTransmitterRIV > pSession->dwReceiverRIV )
    {
        oPrndLicenseTransmitBasicInformation.xbbaTransmitterRevInfo.pbDataBuffer    = pbTransmitterRevInfo;
        oPrndLicenseTransmitBasicInformation.xbbaTransmitterRevInfo.cbData          = cbTransmitterRevInfo;
    }

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_BASIC_INFORMATION, &oPrndLicenseTransmitBasicInformation ) );

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

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_CUSTOM_DATA, &oPrndCustomData ) );
    }

    /*
    ** Have to use an array for DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION because
    ** DRM_XB_AddEntry doesn't make a copy of the data.  It just caches a pointer to it.
    ** DRM_PRND_LicenseTransmitMessage_Finish is what actually makes the copy.
    */
    for( idxLicense = 0; idxLicense < f_cReceiverBoundOrLeafXMRLicenses; idxLicense++ )
    {
        f_rgPrndLicenseTransmitLicenseInformation[idxLicense].fValid                                          = TRUE;
        f_rgPrndLicenseTransmitLicenseInformation[idxLicense].xbbaReceiverBoundOrLeafXMRLicense.pbDataBuffer  = f_rgpbReceiverBoundOrLeafXMRLicense[idxLicense];
        f_rgPrndLicenseTransmitLicenseInformation[idxLicense].xbbaReceiverBoundOrLeafXMRLicense.cbData        = f_rgcbReceiverBoundOrLeafXMRLicense[idxLicense];

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, &f_rgPrndLicenseTransmitLicenseInformation[idxLicense] ) );
    }

    oPrndInternalSignatureContext.poPrndTransmitterContext = f_poPrndTransmitterContext;

    dr = DRM_PRND_LicenseTransmitMessage_Finish( &xbContext, NULL, &cbLicTransmitMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbLicTransmitMessage = (DRM_BYTE*)Oem_MemAlloc( cbLicTransmitMessage ) );
    ChkDR( DRM_PRND_LicenseTransmitMessage_Finish( &xbContext, pbLicTransmitMessage, &cbLicTransmitMessage, &DRM_PRND_INTERNAL_FormatSignatureCallback, &oPrndInternalSignatureContext ) );

    *f_pcbLicTransmitMessage = cbLicTransmitMessage;
    *f_ppbLicTransmitMessage = pbLicTransmitMessage;
    pbLicTransmitMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbLicTransmitMessage );
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_LicenseTransmit_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )                  DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )            const DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __in                                                              DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicTransmitMessage )                    DRM_BYTE                                        **f_ppbLicTransmitMessage,
    __out                                                             DRM_DWORD                                        *f_pcbLicTransmitMessage )
{
    DRM_RESULT                                       dr                                         = DRM_SUCCESS;
    DRM_BYTE                                        *pbStack                                    = NULL;
    DRM_DWORD                                        cbStack                                    = 0;
    DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION   *rgPrndLicenseTransmitLicenseInformation    = NULL;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT            *pSession                                   = NULL;
    DRM_DWORD                                        cbLicenseInfo                              = 0;

    TRACE(( "DRM_PRND_Transmitter_LicenseTransmit_Generate Entered {f_cReceiverBoundOrLeafXMRLicenses = %d}", f_cReceiverBoundOrLeafXMRLicenses ));

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( f_cReceiverBoundOrLeafXMRLicenses     >  0    );
    ChkArg( f_rgpbReceiverBoundOrLeafXMRLicense   != NULL );
    ChkArg( f_rgcbReceiverBoundOrLeafXMRLicense   != NULL );
    ChkArg( f_pcbLicTransmitMessage               != NULL );
    ChkArg( f_ppbLicTransmitMessage               != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS || f_dwFlags == DRM_PRND_FLAG_LICENSE_TRANSMIT_IS_LICENSE_PUSH );

    /*
    ** Don't allow the Tx application to specify DRM_PRND_FLAG_LICENSE_TRANSMIT_IS_LICENSE_PUSH
    ** unless the Rx included DRM_PRND_FLAG_REGISTRATION_REQUEST_SUPPORTS_LICENSE_PUSH during Registration.
    */
    ChkArg( ( ( f_dwFlags                            & DRM_PRND_FLAG_LICENSE_TRANSMIT_IS_LICENSE_PUSH           ) == 0 )
         || ( ( pSession->dwRegistrationRequestFlags & DRM_PRND_FLAG_REGISTRATION_REQUEST_SUPPORTS_LICENSE_PUSH ) != 0 ) );

    *f_pcbLicTransmitMessage = 0;
    *f_ppbLicTransmitMessage = NULL;

    ChkBOOL( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED, DRM_E_DEVICE_NOT_REGISTERED );

    ChkBOOL( _IsDeviceValid( f_poPrndTransmitterContext, pSession ), DRM_E_DEVICE_NOT_REGISTERED );

    pbStack = pSession->rgbStack;
    cbStack = SIZEOF( pSession->rgbStack );

    /*
    ** Have to use an array for DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION because
    ** DRM_XB_AddEntry doesn't make a copy of the data.  It just caches a pointer to it.
    ** DRM_PRND_LicenseTransmitMessage_Finish is what actually makes the copy.
    */
    cbLicenseInfo = f_cReceiverBoundOrLeafXMRLicenses * SIZEOF( DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION );
    ChkBOOL( cbStack > cbLicenseInfo, DRM_E_OUTOFMEMORY );

    OEM_SECURE_ZERO_MEMORY( pbStack, cbLicenseInfo );
    rgPrndLicenseTransmitLicenseInformation = (DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION*)pbStack;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbStack, cbLicenseInfo, (DRM_DWORD_PTR*)&pbStack ) );
    ChkDR( DRM_DWordSub( cbStack, cbLicenseInfo, &cbStack ) );

    ChkDR( _Transmitter_LicenseTransmit_Generate(
        f_poPrndTransmitterContext,
        &pbStack,
        &cbStack,
        f_rgpbReceiverBoundOrLeafXMRLicense,
        f_rgcbReceiverBoundOrLeafXMRLicense,
        rgPrndLicenseTransmitLicenseInformation,
        f_cReceiverBoundOrLeafXMRLicenses,
        f_pCustomDataTypeID,
        f_pbCustomData,
        f_cbCustomData,
        f_dwFlags,
        f_ppbLicTransmitMessage,
        f_pcbLicTransmitMessage ) );

    /* Update dwCurrentLicenseMessageID as the last operation. */
    DRM_PRND_TxRx_IncrementLMID( &pSession->guidCurrentLicenseMessageID );

ErrorExit:
    TRACE(( "DRM_PRND_Transmitter_LicenseTransmit_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ClearPRNDStore(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );

    /* Closing/reopening the store has the effect of wiping all licenses contained therein. */

    /* Returns void */
    DRM_PRND_ClosePRNDStore(
        &pSession->oLSTContextPRNDStore,
        &pSession->oDSTContextPRNDStore,
        &pSession->oESTContextPRNDStore,
        pSession->rgbPRNDStore,
        SIZEOF( pSession->rgbPRNDStore ),
        &pSession->fPRNDLSTOpened );

    ChkDR( DRM_PRND_OpenPRNDStore(
        f_poPrndTransmitterContext->pBBXContext->pOEMContext,
        &pSession->oLSTContextPRNDStore,
        &pSession->oDSTContextPRNDStore,
        &pSession->oESTContextPRNDStore,
        pSession->rgbPRNDStore,
        SIZEOF( pSession->rgbPRNDStore ),
        &pSession->fPRNDLSTOpened ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_LicenseError_Generate(
    __inout_opt                                                       DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pSessionID,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbErrMessage )                            DRM_BYTE                                        **f_ppbErrMessage,
    __out                                                             DRM_DWORD                                        *f_pcbErrMessage,
    __in                                                              DRM_RESULT                                        f_drResult )
{
    DRM_RESULT                                   dr                                 = DRM_SUCCESS;
    DRM_XB_BUILDER_CONTEXT                       xbContext                          = {0};
    DRM_PRND_LICENSE_ERROR_BASIC_INFORMATION     oPrndLicenseErrorBasicInformation  = {0};
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT        *pSession                           = NULL;
    DRM_BYTE                                    *pbErrMessage                       = NULL;
    DRM_DWORD                                    cbErrMessage                       = 0;
    DRM_BYTE                                    *pbStack                            = NULL;

    TRACE(( "DRM_PRND_Transmitter_LicenseError_Generate Entered {f_drResult = 0x%x}", f_drResult ));

    ChkArg( ( f_poPrndTransmitterContext == NULL ) != ( f_pSessionID == NULL ) );
    if( f_poPrndTransmitterContext != NULL )
    {
        ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    }
    ChkArg( f_pcbErrMessage != NULL );
    ChkArg( f_ppbErrMessage != NULL );
    ChkArg( DRM_FAILED( f_drResult ) );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    *f_pcbErrMessage = 0;
    *f_ppbErrMessage = NULL;

    if( pSession == NULL )
    {
        ChkMem( pbStack = (DRM_BYTE*)Oem_MemAlloc( DRM_PRND_STACK_SIZE ) );

        ChkDR( DRM_PRND_LicenseErrorMessage_Start(
            pbStack,
            DRM_PRND_STACK_SIZE,
            &xbContext ) );
    }
    else
    {
        ChkDR( DRM_PRND_LicenseErrorMessage_Start(
            pSession->rgbStack,
            SIZEOF( pSession->rgbStack ),
            &xbContext ) );
    }

    oPrndLicenseErrorBasicInformation.fValid    = TRUE;
    oPrndLicenseErrorBasicInformation.dwFlags   = f_dwFlags;
    OEM_SECURE_MEMCPY(
        &oPrndLicenseErrorBasicInformation.guidSessionID,
        pSession == NULL ? f_pSessionID : &pSession->SessionID,
        SIZEOF( oPrndLicenseErrorBasicInformation.guidSessionID ) );
    oPrndLicenseErrorBasicInformation.dwResult  = (DRM_DWORD)f_drResult;

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_ERROR_BASIC_INFORMATION, &oPrndLicenseErrorBasicInformation ) );

    dr = DRM_PRND_LicenseErrorMessage_Finish( &xbContext, NULL, &cbErrMessage );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    ChkMem( pbErrMessage = (DRM_BYTE*)Oem_MemAlloc( cbErrMessage ) );
    ChkDR( DRM_PRND_LicenseErrorMessage_Finish( &xbContext, pbErrMessage, &cbErrMessage ) );

    *f_pcbErrMessage = cbErrMessage;
    *f_ppbErrMessage = pbErrMessage;
    pbErrMessage     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbErrMessage );
    SAFE_OEM_FREE( pbStack );
    TRACE(( "DRM_PRND_Transmitter_LicenseError_Generate Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_EndSession(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    TRACE(( "DRM_PRND_Transmitter_EndSession Entered" ));

    DRMCASSERT( 0               == DRM_PRND_PROXIMITY_DETECTION_STATE_INIT );
    DRMCASSERT( (DRM_RESULT)0x0 == DRM_SUCCESS                             );

    if( f_poPrndTransmitterContext != NULL )
    {
        DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

        dr = _GetSession( f_poPrndTransmitterContext, &pSession );
        if( DRM_SUCCEEDED( dr ) )
        {
            SAFE_OEM_FREE( pSession->pbTransmitterProximityDetectionChannel );
            SAFE_OEM_FREE( pSession->pbReceiverCertificate );
            SAFE_OEM_FREE( pSession->pbReceiverCertificateCRL );
            SAFE_OEM_FREE( pSession->pbWorkingSpace );

            DRM_BBX_CleanupPublicKeyContext( f_poPrndTransmitterContext->pBBXContext, &pSession->oECC256PubKeyPrndEncryptReceiver );
            DRM_BBX_CleanupPublicKeyContext( f_poPrndTransmitterContext->pBBXContext, &pSession->oECC256PubKeyContentEncryptReceiver );

            DRM_BBX_SafeReleaseKey( f_poPrndTransmitterContext->pBBXContext, &pSession->handleMIMK );

            /* returns void */
            DRM_PRND_ClosePRNDStore(
                &pSession->oLSTContextPRNDStore,
                &pSession->oDSTContextPRNDStore,
                &pSession->oESTContextPRNDStore,
                pSession->rgbPRNDStore,
                SIZEOF( pSession->rgbPRNDStore ),
                &pSession->fPRNDLSTOpened );
        }

        OEM_SECURE_ZERO_MEMORY( &f_poPrndTransmitterContext->oPrndTransmitterSessionContext, SIZEOF( f_poPrndTransmitterContext->oPrndTransmitterSessionContext ) );
    }

    /* Do NOT define ErrorExit label here.  Do NOT use ChkDR (etc) above.  Doing so could skip cleanup operations. */
    TRACE(( "DRM_PRND_Transmitter_EndSession Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_IsSessionStillValid(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __out                                                             DRM_BOOL                                         *f_pfSessionStillValid )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *pSession   = NULL;

    ChkDR( _ValidateTransmitterContext( f_poPrndTransmitterContext, &pSession ) );
    ChkArg( f_pfSessionStillValid != NULL );

    /*
    ** The session is still valid (and therefore CANNOT be cleaned up by the application) if:
    **  1. The device has not yet EVER become valid (i.e. proximity detection has not yet succeeded for the first time).
    **     In this case, we are probably in the middle of registration or proximity detection for the first time,
    **      e.g. inside the proximity detection retry loop.  We don't want the session to die so early.
    **     If registration or proximity detection fails on the first attempt, we expect the idle timer to
    **      result in the session ending and getting removed from the active list, NOT due to a call to this function.
    **     In other words, we can't assume that the cleanup logic for expired sessions is smart enough to only
    **      cleanup sessions that have successfully completed proximity detection at least once, but we CAN assume
    **      that the recommended idle timer will cleanup sessions that failed initialization.
    **  OR
    **  2. The device is marked valid.
    **     In this case, we've successfully completed proximity detection at least once AND our session has not yet expired.
    */
    *f_pfSessionStillValid = ( !pSession->fSuccessfulProximityAtLeastOnce ) || ( _IsDeviceValid( f_poPrndTransmitterContext, pSession ) );

    /*
    ** Even if one of the above conditions are met, there's still a case where the session is NOT valid.
    ** When we're in the middle of proximity detection (either for the first time or during renewal),
    **  then pSession->ePrndProximityDetectionState != DRM_PRND_PROXIMITY_DETECTION_STATE_INIT
    **  and  pSession->ePrndProximityDetectionState != DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED.
    ** That's normally OK - we're not actually expired yet and the application should not be removing us from its active list.
    ** For renewal, it just means we can't handle any license messages until renewal is completed, i.e. the following functions will fail:
    **  DRM_PRND_Transmitter_LicenseRequest_Process   and   DRM_PRND_Transmitter_LicenseTransmit_Generate.
    ** However, if we're in the middle of proximity detection AND the device has timed out,
    **  then proximity detection can't possibly succeed for the session!  Thus, the session is NOT valid in this case.
    */
    if( pSession->ePrndProximityDetectionState != DRM_PRND_PROXIMITY_DETECTION_STATE_INIT
     && pSession->ePrndProximityDetectionState != DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED
     && _HasDeviceTimedOut( f_poPrndTransmitterContext, pSession ) )
    {
        *f_pfSessionStillValid = FALSE;
    }

    TRACE(( "DRM_PRND_Transmitter_IsSessionStillValid Info {*f_pfSessionStillValid = %d}", (*f_pfSessionStillValid ? 1 : 0) ));

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

