/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDINTERNAL_C
#include <drmprnd.h>
#include <drmprndinternal.h>
#include <drmbcrl.h>
#include <drmconstants.h>
#include <drmbcertparser.h>
#include <drmdeviceassets.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_GUID *c_rgpguidRevocationTypes[4] = {
    &g_guidRevocationTypeWMDRMNET,
    &g_guidRevocationTypePlayReadySilverLightRuntime,
    &g_guidRevocationTypePlayReadySilverLightApplication,
    &g_guidRevocationTypeRevInfo2 };        /* MUST be last */
static const DRM_DWORD c_idxRevInfo2 = 3;   /* MUST be kept in sync with above array */

static DRM_NO_INLINE DRM_RESULT _ProcessRevInfo(
    __inout_opt                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_opt                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                                DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteRevInfo,
    __out_opt                                               DRM_BOOL                                         *f_pfRIVUpdated ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _ProcessRevInfo(
    __inout_opt                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_opt                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                    DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteRevInfo,
    __out_opt                                               DRM_BOOL                                         *f_pfRIVUpdated )
{
    DRM_RESULT                    dr                                                = DRM_SUCCESS;
    DRM_BB_CONTEXT               *pBBXContext                                       = NULL;
    DRM_REVOCATIONSTORE_CONTEXT  *pRevocationStoreContext                           = NULL;
    DRM_BYTE                     *pbRevocationBuffer                                = NULL;
    DRM_DWORD                     cbRevocationBuffer                                = 0;

    AssertChkArg( ( f_poPrndTransmitterContext != NULL ) != ( f_poPrndReceiverContext != NULL ) );

    if( f_poPrndTransmitterContext != NULL )
    {
        pBBXContext                                         = f_poPrndTransmitterContext->pBBXContext;
        pRevocationStoreContext                             = f_poPrndTransmitterContext->pRevocationStoreContext;
        pbRevocationBuffer                                  = f_poPrndTransmitterContext->pbRevocationBuffer;
        cbRevocationBuffer                                  = f_poPrndTransmitterContext->cbRevocationBuffer;
    }
    else
    {
        pBBXContext                                         = f_poPrndReceiverContext->pBBXContext;
        pRevocationStoreContext                             = f_poPrndReceiverContext->pRevocationStoreContext;
        pbRevocationBuffer                                  = f_poPrndReceiverContext->pbRevocationBuffer;
        cbRevocationBuffer                                  = f_poPrndReceiverContext->cbRevocationBuffer;
    }

    AssertChkArg( pBBXContext               != NULL );
    AssertChkArg( pRevocationStoreContext   != NULL );
    AssertChkArg( pbRevocationBuffer        != NULL );
    AssertChkArg( cbRevocationBuffer         > 0    );
    AssertChkArg( f_pxbbaRemoteRevInfo      != NULL );
    AssertChkArg( XBBA_TO_CB( *f_pxbbaRemoteRevInfo ) > 0 );

    {
        DRM_BOOL      rgfFound[NO_OF(c_rgpguidRevocationTypes)] = {0};
        DRM_DWORD     cbTotal                                   = XBBA_TO_CB( *f_pxbbaRemoteRevInfo );
        DRM_BYTE      *pbNext                                   = XBBA_TO_PB( *f_pxbbaRemoteRevInfo );

        AssertChkArg( f_pfRIVUpdated != NULL );

        while( cbTotal > 0 )
        {
            DRM_GUID  guidNext = {0};
            DRM_DWORD cbNext   = 0;
            DRM_BOOL  fUpdated = FALSE;
            DRM_DWORD idx      = 0;

            /* Must not have previously found RevInfo - it must be last */
            ChkBOOL( !rgfFound[c_idxRevInfo2], DRM_E_PRND_MESSAGE_INVALID  );

            /* Must have space for GUID + size + data */
            ChkBOOL( cbTotal > SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ), DRM_E_PRND_MESSAGE_INVALID );

            OEM_SECURE_MEMCPY( &guidNext, pbNext, SIZEOF( DRM_GUID ) );
            ChkDR( DRM_DWordSub( cbTotal, SIZEOF( DRM_GUID ), &cbTotal ) );
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbNext, SIZEOF( DRM_GUID ), (DRM_DWORD_PTR*)&pbNext ) );

            while( ( idx < NO_OF( c_rgpguidRevocationTypes ) )
                && ( 0 != MEMCMP( &guidNext, c_rgpguidRevocationTypes[idx], SIZEOF( DRM_GUID ) ) ) )
            {
                idx++;
            }

            /* Must be a GUID in the list */
            ChkBOOL( idx < NO_OF( c_rgpguidRevocationTypes ), DRM_E_PRND_MESSAGE_INVALID );
            DRMASSERT( 0 == MEMCMP( &guidNext, c_rgpguidRevocationTypes[idx], SIZEOF( DRM_GUID ) ) );

            /* Must not be a duplicate */
            ChkBOOL( !rgfFound[idx], DRM_E_PRND_MESSAGE_INVALID );
            rgfFound[idx] = TRUE;

            NETWORKBYTES_TO_DWORD( cbNext, pbNext, 0 /* Offset 0 into buffer */ );
            ChkDR( DRM_DWordSub( cbTotal, SIZEOF( DRM_DWORD ), &cbTotal ) );
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbNext, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)&pbNext ) );

            ChkBOOL( cbNext > 0, DRM_E_PRND_MESSAGE_INVALID );
            ChkBOOL( cbTotal >= cbNext, DRM_E_PRND_MESSAGE_INVALID );

            ChkDR( DRM_RVK_SetCRL(
                &pBBXContext->CryptoContext,
                pRevocationStoreContext,
                &guidNext,
                pbNext,
                cbNext,
                pbRevocationBuffer,
                cbRevocationBuffer,
                &fUpdated ) );

            ChkDR( DRM_DWordSub( cbTotal, cbNext, &cbTotal ) );
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbNext, cbNext, (DRM_DWORD_PTR *)&pbNext ) );
            *f_pfRIVUpdated = *f_pfRIVUpdated || fUpdated;
        }
        DRMASSERT( cbTotal == 0 );

        /* Must have found RevInfo2 */
        ChkBOOL( rgfFound[c_idxRevInfo2], DRM_E_PRND_MESSAGE_INVALID );
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _ValidateCertificate(
    __inout_opt                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_opt                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCertificate ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _ValidateCertificate(
    __inout_opt                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_opt                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCertificate )
{
    DRM_RESULT                        dr                                = DRM_SUCCESS;
    DRM_BB_CONTEXT                   *pBBXContext                       = NULL;
    DRM_SECSTORE_CONTEXT             *pSSTGlobalContext                 = NULL;
    DRM_DST                          *pDSTContext                       = NULL;
    DRM_REVOCATIONSTORE_CONTEXT      *pRevocationStoreContext           = NULL;
    DRM_BYTE                         *pbRevocationBuffer                = NULL;
    DRM_DWORD                         cbRevocationBuffer                = 0;
    DRM_BYTE                        **ppbReceiverCertificateCRL         = NULL;
    DRM_DWORD                        *pcbReceiverCertificateCRL         = NULL;
    DRMFILETIME                       ftCurrentTime                     = {0};
    DRM_SECSTORE_CONTEXT              oSST                              = {0};
    DRM_BCERT_VERIFICATIONCONTEXT     oBCertVerifyCtx                   = {0};
    DRM_DWORD                         cbRuntimeCRL                      = 0;
    DRM_DWORD                        *pdwRemoteCertificateExpiration    = NULL;

    AssertChkArg( ( f_poPrndTransmitterContext != NULL ) != ( f_poPrndReceiverContext != NULL ) );

    if( f_poPrndTransmitterContext != NULL )
    {
        pBBXContext                     = f_poPrndTransmitterContext->pBBXContext;
        pSSTGlobalContext               = f_poPrndTransmitterContext->pSSTGlobalContext;
        pDSTContext                     = f_poPrndTransmitterContext->pDSTContext;
        pRevocationStoreContext         = f_poPrndTransmitterContext->pRevocationStoreContext;
        pbRevocationBuffer              = f_poPrndTransmitterContext->pbRevocationBuffer;
        cbRevocationBuffer              = f_poPrndTransmitterContext->cbRevocationBuffer;
        ppbReceiverCertificateCRL       = &f_poPrndTransmitterContext->oPrndTransmitterSessionContext.pbReceiverCertificateCRL;
        pcbReceiverCertificateCRL       = &f_poPrndTransmitterContext->oPrndTransmitterSessionContext.cbReceiverCertificateCRL;
        pdwRemoteCertificateExpiration  = &f_poPrndTransmitterContext->oPrndTransmitterSessionContext.dwReceiverExpiration;
    }
    else
    {
        pBBXContext                     = f_poPrndReceiverContext->pBBXContext;
        pSSTGlobalContext               = f_poPrndReceiverContext->pSSTGlobalContext;
        pDSTContext                     = f_poPrndReceiverContext->pDSTContext;
        pRevocationStoreContext         = f_poPrndReceiverContext->pRevocationStoreContext;
        pbRevocationBuffer              = f_poPrndReceiverContext->pbRevocationBuffer;
        cbRevocationBuffer              = f_poPrndReceiverContext->cbRevocationBuffer;
        pdwRemoteCertificateExpiration  = &f_poPrndReceiverContext->oPrndReceiverSessionContext.dwTransmitterExpiration;
    }

    AssertChkArg( pBBXContext               != NULL );
    AssertChkArg( pSSTGlobalContext         != NULL );
    AssertChkArg( pDSTContext               != NULL );
    AssertChkArg( pRevocationStoreContext   != NULL );
    AssertChkArg( pbRevocationBuffer        != NULL );
    AssertChkArg( cbRevocationBuffer         > 0    );
    AssertChkArg( f_pxbbaRemoteCertificate  != NULL );
    AssertChkArg( XBBA_TO_CB( *f_pxbbaRemoteCertificate ) > 0 );

    /* returns void */
    Oem_Clock_GetSystemTimeAsFileTime( pBBXContext->pOEMContext, &ftCurrentTime );

    /* Parse and verify the certificate chain. */
    ChkDR( DRM_BCert_InitVerificationContext(
        &ftCurrentTime,
        (const PUBKEY_P256*) g_rgbMSPlayReadyRootIssuerPubKey,
        DRM_BCERT_CERTTYPE_UNKNOWN,     /* Accept PC or Device certs, so don't verify type.  Feature verification (below) will be sufficient. */
        &pBBXContext->CryptoContext,
        TRUE,                           /* Verify signatures */
        FALSE,                          /* *DO* fail on missing extended data */
        NULL,                           /* No required key usages at this step - will be verified below when we actually fetch the pubkeys. */
        0,                              /* Required key usage count (obviously 0). */
        FALSE,                          /* Immediately fail on any error. */
        pBBXContext,
        &oSST,
        pDSTContext,
        &oBCertVerifyCtx ) );
    ChkDR( DRM_BCert_ParseCertificateChain(
        XBBA_TO_PB( *f_pxbbaRemoteCertificate ),
        XBBA_TO_CB( *f_pxbbaRemoteCertificate ),
        &oBCertVerifyCtx ) );

    /* The remote certificate expiration was checked in DRM_BCert_ParseCertificateChain, but we'll need to check it again later during licensing.  Save it. */
    *pdwRemoteCertificateExpiration = oBCertVerifyCtx.dwCertificateChainExpiration;

    /* Check certificate for revocation. */
    cbRuntimeCRL = cbRevocationBuffer;
    dr = DRM_RVS_GetRevocationData(
        pRevocationStoreContext,
        &g_guidRevocationTypePlayReadySilverLightRuntime,
        pbRevocationBuffer,
        &cbRuntimeCRL );
    if ( dr == DRM_SUCCESS )
    {
        DRMASSERT( cbRuntimeCRL > 0 );
        ChkDR( DRM_BCrl_CheckIfPlayReadyCertRevoked(
            XBBA_TO_PB( *f_pxbbaRemoteCertificate ),
            XBBA_TO_CB( *f_pxbbaRemoteCertificate ),
            pbRevocationBuffer,
            cbRuntimeCRL,
            NULL,
            NULL,
            &pBBXContext->CryptoContext,
            pBBXContext,
            pSSTGlobalContext,
            pDSTContext ) );
        if( ppbReceiverCertificateCRL != NULL )
        {
            SAFE_OEM_FREE( *ppbReceiverCertificateCRL );
            *pcbReceiverCertificateCRL = 0;
            ChkMem( *ppbReceiverCertificateCRL = (DRM_BYTE*)Oem_MemAlloc( cbRuntimeCRL ) );
            *pcbReceiverCertificateCRL = cbRuntimeCRL;
            OEM_SECURE_MEMCPY( *ppbReceiverCertificateCRL, pbRevocationBuffer, cbRuntimeCRL );
        }
    }
    else if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        cbRuntimeCRL = 0;
        dr = DRM_SUCCESS; /* No CRL available, this is fine */
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _MakeDataCallbackWithCertificate(
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCertificate,
    __in_opt                                          const DRM_ID                                           *f_pRemoteCustomDataTypeID,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCustomData,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext,
    __inout                                                 DRM_BCERT_CERTIFICATE                            *f_pCert,
    __in                                              const DRM_BINARY_DEVICE_CERT_CACHED_VALUES             *f_pCache,
    __inout                                                 DRM_DWORD                                        *f_piChainOffset ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT _MakeDataCallbackWithCertificate(
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCertificate,
    __in_opt                                          const DRM_ID                                           *f_pRemoteCustomDataTypeID,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCustomData,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext,
    __inout                                                 DRM_BCERT_CERTIFICATE                            *f_pCert,
    __in                                              const DRM_BINARY_DEVICE_CERT_CACHED_VALUES             *f_pCache,
    __inout                                                 DRM_DWORD                                        *f_piChainOffset )
{
    DRM_RESULT                    dr                                                = DRM_SUCCESS;
    DRM_PRND_CERTIFICATE_DATA     oData                                             = {0};

    AssertChkArg( f_pxbbaRemoteCertificate  != NULL );
    AssertChkArg( XBBA_TO_CB( *f_pxbbaRemoteCertificate ) > 0 );
    AssertChkArg( f_pfnDataCallback         != NULL );
    AssertChkArg( f_pCert                   != NULL );
    AssertChkArg( f_pCache                  != NULL );
    AssertChkArg( f_piChainOffset           != NULL );
    AssertChkArg( ( f_pRemoteCustomDataTypeID  != NULL ) == ( f_pxbbaRemoteCustomData != NULL ) );

    DRMSIZEASSERT( SIZEOF( oData.rgbModelDigest ), SIZEOF( f_pCert->BasicInfo.DigestValue.m_rgbDigest ) );
    DRMSIZEASSERT( SIZEOF( oData.rgbClientID ), DRM_BCERT_CLIENT_ID_LENGTH );
    DRMSIZEASSERT( SIZEOF( oData.rgdwSupportedFeatureSet ), SIZEOF( f_pCert->FeatureInfo.rgdwFeatureSet ) );
    DRMSIZEASSERT( SIZEOF( oData.szModelManufacturerName ), SIZEOF( f_pCert->ManufacturerInfo.ManufacturerStrings.ManufacturerName.rgb ) + 1 ); /* +1 to ensure null-termination */

    oData.dwType = f_pCert->BasicInfo.dwType;
    if( f_pCert->SecurityVersion2.Header.cbLength > 0 )
    {
        oData.dwPlatformIdentifier  = f_pCert->SecurityVersion2.dwPlatformIdentifier;
    }
    else if( f_pCert->SecurityVersion.Header.cbLength > 0 )
    {
        oData.dwPlatformIdentifier  = f_pCert->SecurityVersion.dwPlatformIdentifier;
    }
    oData.dwSecurityLevel  = f_pCert->BasicInfo.dwSecurityLevel;

    oData.cSupportedFeatures = f_pCert->FeatureInfo.dwNumFeatureEntries;
    if( oData.cSupportedFeatures > 0 )
    {
        DRMCRT_memcpy( oData.rgdwSupportedFeatureSet, f_pCert->FeatureInfo.rgdwFeatureSet, SIZEOF( oData.rgdwSupportedFeatureSet ) );
    }

    DRMCRT_memcpy( oData.rgbClientID, f_pCache->DeviceSerialNumber.rgb, DRM_BCERT_CLIENT_ID_LENGTH );

    /* Get the parent (model) certificate for its model digest value and model information. */
    OEM_SECURE_ZERO_MEMORY( f_pCert, SIZEOF( DRM_BCERT_CERTIFICATE ) );
    ChkDR( DRM_BCert_GetCertificate(
        XBBA_TO_PB( *f_pxbbaRemoteCertificate ),
        XBBA_TO_CB( *f_pxbbaRemoteCertificate ),
        f_piChainOffset,
        f_pCert,
        DRM_BCERT_CERTTYPE_UNKNOWN ) );
    DRMCRT_memcpy( oData.rgbModelDigest,          f_pCert->BasicInfo.DigestValue.m_rgbDigest,                         SIZEOF( oData.rgbModelDigest ) );
    DRMCRT_memcpy( oData.szModelManufacturerName, f_pCert->ManufacturerInfo.ManufacturerStrings.ManufacturerName.rgb, SIZEOF( oData.szModelManufacturerName ) - 1 ); /* -1 to ensure null-termination */
    DRMCRT_memcpy( oData.szModelName,             f_pCert->ManufacturerInfo.ManufacturerStrings.ModelName.rgb,        SIZEOF( oData.szModelName )             - 1 ); /* -1 to ensure null-termination */
    DRMCRT_memcpy( oData.szModelNumber,           f_pCert->ManufacturerInfo.ManufacturerStrings.ModelNumber.rgb,      SIZEOF( oData.szModelNumber )           - 1 ); /* -1 to ensure null-termination */

    ChkDR( f_pfnDataCallback(
        f_pvDataCallbackContext,
        &oData,
        f_pxbbaRemoteCustomData != NULL ? f_pRemoteCustomDataTypeID              : NULL,
        f_pxbbaRemoteCustomData != NULL ? XBBA_TO_PB( *f_pxbbaRemoteCustomData ) : NULL,
        f_pxbbaRemoteCustomData != NULL ? XBBA_TO_CB( *f_pxbbaRemoteCustomData ) : 0 ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
    __inout_opt                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_opt                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCertificate,
    __in_opt                                          const DRM_ID                                           *f_pRemoteCustomDataTypeID,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCustomData,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext,
    __out_opt                                               DRM_PUBLIC_KEY_CONTEXT                           *f_poECC256PubKeyPrndEncryptRemote,
    __out_opt                                               DRM_PUBLIC_KEY_CONTEXT                           *f_poECC256PubKeyContentEncryptRemote,
    __in_ecount_opt( f_cbToVerifyData )               const DRM_BYTE                                         *f_pbToVerifyData,
    __in                                                    DRM_DWORD                                         f_cbToVerifyData,
    __in_opt                                          const SIGNATURE_P256                                   *f_pToVerifySig,
    __in_opt                                          const DRM_ID                                           *f_pSessionID,
    __in_opt                                                DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteRevInfo,
    __out_opt                                               DRM_BOOL                                         *f_pfRIVUpdated )
{
    DRM_RESULT                            dr          = DRM_SUCCESS;
    DRM_BOOL                              fOnReceiver = FALSE;
    DRM_BB_CONTEXT                       *pBBXContext = NULL;
    DRM_BINARY_DEVICE_CERT_CACHED_VALUES  oCache      = {0};
    DRM_BYTE                             *pbRevInfo   = NULL;
    DRM_DWORD                             cbRevInfo   = 0;

    AssertChkArg( ( f_poPrndTransmitterContext != NULL ) != ( f_poPrndReceiverContext != NULL ) );
    AssertChkArg( ( f_pRemoteCustomDataTypeID  != NULL ) == ( f_pxbbaRemoteCustomData != NULL ) );
    AssertChkArg( ( f_pbToVerifyData           != NULL ) == ( f_cbToVerifyData        != 0    ) );
    AssertChkArg( ( f_pbToVerifyData           != NULL ) == ( f_pToVerifySig          != NULL ) );

    if( f_poPrndTransmitterContext != NULL )
    {
        fOnReceiver                                         = FALSE;
        pBBXContext                                         = f_poPrndTransmitterContext->pBBXContext;
    }
    else
    {
        fOnReceiver                                         = TRUE;
        pBBXContext                                         = f_poPrndReceiverContext->pBBXContext;
    }

    AssertChkArg( pBBXContext != NULL );

    if( ( f_pxbbaRemoteRevInfo != NULL ) && ( XBBA_TO_CB( *f_pxbbaRemoteRevInfo ) > 0 ) )
    {
        ChkDR( _ProcessRevInfo( f_poPrndTransmitterContext, f_poPrndReceiverContext, f_pxbbaRemoteRevInfo, f_pfRIVUpdated ) );
    }

    if( ( f_pxbbaRemoteCertificate != NULL ) && ( XBBA_TO_CB( *f_pxbbaRemoteCertificate ) > 0 ) )
    {
        DRM_DWORD                               iChainOffset        = 0;
        DRM_BCERT_CERTIFICATE                   oCert               = {0};
        DRM_BCERT_CHAIN_HEADER                  oCertChainHeader    = {0};

        ChkDR( _ValidateCertificate( f_poPrndTransmitterContext, f_poPrndReceiverContext, f_pxbbaRemoteCertificate ) );

        /* Load certificate data and send via callback if requested. */
        ChkDR( DRM_BCert_LoadPropertiesCache(
            XBBA_TO_PB( *f_pxbbaRemoteCertificate ),
            XBBA_TO_CB( *f_pxbbaRemoteCertificate ),
            &oCache,
            DRM_BBX_GetKeyHandleCallbacks( pBBXContext ),
            pBBXContext ) );

        ChkDR( DRM_BCert_GetChainHeader(
            XBBA_TO_PB( *f_pxbbaRemoteCertificate ),
            XBBA_TO_CB( *f_pxbbaRemoteCertificate ),
            &iChainOffset,
            &oCertChainHeader ) );

        ChkDR( DRM_BCert_GetCertificate(
            XBBA_TO_PB( *f_pxbbaRemoteCertificate ),
            XBBA_TO_CB( *f_pxbbaRemoteCertificate ),
            &iChainOffset,
            &oCert,
            DRM_BCERT_CERTTYPE_UNKNOWN ) );

        if( fOnReceiver )
        {
            ChkBOOL( oCache.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_TRANSMITTER), DRM_E_PRND_TRANSMITTER_UNAUTHORIZED );
        }
        else
        {
            ChkBOOL( oCache.dwFeatureFlags & BCERT_FEATURE_BIT(DRM_BCERT_FEATURE_RECEIVER), DRM_E_PRND_CERTIFICATE_NOT_RECEIVER );
        }

        if( f_poECC256PubKeyContentEncryptRemote != NULL )
        {
            ChkDR( DRM_BBX_TransferPublicKeyContext( pBBXContext,
                                                     f_poECC256PubKeyContentEncryptRemote,
                                                     &oCache.DeviceKeyEncrypt.PublicKeyContext ) );
        }

        if( f_poECC256PubKeyPrndEncryptRemote != NULL )
        {
            ChkBOOL( oCache.fDeviceKeyPrndEncryptSet, DRM_E_PRND_CERTIFICATE_NOT_RECEIVER );

            ChkDR( DRM_BBX_TransferPublicKeyContext( pBBXContext,
                                                     f_poECC256PubKeyPrndEncryptRemote,
                                                     &oCache.DeviceKeyPrndEncrypt.PublicKeyContext ) );
        }

        if( f_pbToVerifyData != NULL )
        {
            DRMASSERT( fOnReceiver );

            /* Verify that the rev info we used to verify the Tx certificate is recent enough. */
            dr = DRM_RVK_CheckRevInfoForExpiration(
                &f_poPrndReceiverContext->pBBXContext->CryptoContext,
                f_poPrndReceiverContext->pRevocationStoreContext,
                &g_guidRevocationTypeRevInfo2,
                f_pSessionID,
                NULL,
                &cbRevInfo );
            DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
            DRMASSERT( cbRevInfo > 0 );
            ChkMem( pbRevInfo = (DRM_BYTE*)Oem_MemAlloc( cbRevInfo ) );
            ChkDR( DRM_RVK_CheckRevInfoForExpiration(
                &f_poPrndReceiverContext->pBBXContext->CryptoContext,
                f_poPrndReceiverContext->pRevocationStoreContext,
                &g_guidRevocationTypeRevInfo2,
                f_pSessionID,
                pbRevInfo,
                &cbRevInfo ) );

            /* Verify the signed data is correctly signed with the Tx certificate's signing key */
            dr = OEM_ECDSA_Verify_P256(
                f_pbToVerifyData,
                f_cbToVerifyData,
                &oCache.DeviceKeySign.PublicKeyContext.PublicKey,
                f_pToVerifySig,
                (struct bigctx_t *)f_poPrndReceiverContext->pBBXContext->CryptoContext.rgbCryptoContext );
            if( DRM_FAILED( dr ) )
            {
                /* Fail with DRM_E_PRND_TRANSMITTER_UNAUTHORIZED if verification fails */
                ChkDR( DRM_E_PRND_TRANSMITTER_UNAUTHORIZED );
            }

            TRACE(( "DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData Transmitter Authorized" ));
        }

        if( !fOnReceiver )
        {
            f_poPrndTransmitterContext->oPrndTransmitterSessionContext.dwReceiverSecurityLevel = oCert.BasicInfo.dwSecurityLevel;
        }

        if( f_pfnDataCallback != NULL )
        {
            ChkDR( _MakeDataCallbackWithCertificate(
                f_pxbbaRemoteCertificate,
                f_pRemoteCustomDataTypeID,
                f_pxbbaRemoteCustomData,
                f_pfnDataCallback,
                f_pvDataCallbackContext,
                &oCert,
                &oCache,
                &iChainOffset ) );
        }
    }
    else
    {
        if( f_pfnDataCallback != NULL )
        {
            ChkDR( f_pfnDataCallback(
                f_pvDataCallbackContext,
                NULL,
                f_pxbbaRemoteCustomData != NULL ? f_pRemoteCustomDataTypeID              : NULL,
                f_pxbbaRemoteCustomData != NULL ? XBBA_TO_PB( *f_pxbbaRemoteCustomData ) : NULL,
                f_pxbbaRemoteCustomData != NULL ? XBBA_TO_CB( *f_pxbbaRemoteCustomData ) : 0 ) );
        }
    }

ErrorExit:

    /*
    ** Cleanup the local properties cache
    */
    if( pBBXContext != NULL )
    {
        DRM_BCert_CleanupPropertiesCache( &oCache,
                                          DRM_BBX_GetKeyHandleCallbacks( pBBXContext ),
                                          pBBXContext );
    }

    SAFE_OEM_FREE( pbRevInfo );
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_FetchCertAndRevInfo(
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __inout                                                 DRM_REVOCATIONSTORE_CONTEXT                      *f_pRevocationStoreContext,
    __deref_inout_bcount( *f_pcbStack )                     DRM_BYTE                                        **f_ppbStack,
    __inout                                                 DRM_DWORD                                        *f_pcbStack,
    __in_opt                                                DRM_KEYFILE_CONTEXT                              *f_pKeyFileContext,
    __out_ecount_opt( *f_pcbLocalCertificate )              DRM_BYTE                                        **f_ppbLocalCertificate,
    __out_opt                                               DRM_DWORD                                        *f_pcbLocalCertificate,
    __out_ecount( *f_pcbLocalRevInfo )                      DRM_BYTE                                        **f_ppbLocalRevInfo,
    __out                                                   DRM_DWORD                                        *f_pcbLocalRevInfo,
    __out_opt                                               DRM_DWORD                                        *f_pdwRIV )
{
    DRM_RESULT  dr  = DRM_SUCCESS;
    DRM_DWORD   idx = 0;

    ChkArg( f_ppbStack              != NULL );
    ChkArg( f_pcbStack              != NULL );
    ChkArg( f_ppbLocalRevInfo       != NULL );
    ChkArg( f_pcbLocalRevInfo       != NULL );

    ChkArg( ( f_ppbLocalCertificate != NULL ) == ( f_pcbLocalCertificate != NULL ) );

    if( f_pdwRIV != NULL )
    {
        *f_pdwRIV = 0;
    }

    if( f_pcbLocalCertificate != NULL && DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        /* Use the first set of stack bytes for the Local cert */
        DRM_BYTE *pbCert = NULL;
        ChkDR( DRM_KF_GetCertificate(
            f_pKeyFileContext,
            eKF_CERT_TYPE_PLAYREADY,
            &pbCert,
            f_pcbLocalCertificate ) );
        ChkBOOL( *f_pcbStack > *f_pcbLocalCertificate, DRM_E_OUTOFMEMORY );
        OEM_SECURE_MEMCPY( *f_ppbStack, pbCert, *f_pcbLocalCertificate );
        *f_ppbLocalCertificate = *f_ppbStack;

        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbStack, *f_pcbLocalCertificate, (DRM_DWORD_PTR *)f_ppbStack ) );
        ChkDR( DRM_DWordSub( *f_pcbStack, *f_pcbLocalCertificate, f_pcbStack ) );
    }

    *f_ppbLocalRevInfo = *f_ppbStack;
    *f_pcbLocalRevInfo = 0;

    for( idx = 0; idx < NO_OF( c_rgpguidRevocationTypes ); idx++ )
    {
        DRM_DWORD cbNext = 0;
        dr = DRM_RVS_GetRevocationData(
            f_pRevocationStoreContext,
            c_rgpguidRevocationTypes[idx],
            NULL,
            &cbNext );
        if( dr == DRM_E_BUFFERTOOSMALL )
        {
            dr = DRM_SUCCESS;

            ChkBOOL( *f_pcbStack > SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ) + cbNext, DRM_E_OUTOFMEMORY );

            OEM_SECURE_MEMCPY( *f_ppbStack, c_rgpguidRevocationTypes[idx], SIZEOF( DRM_GUID ) );
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbStack, SIZEOF( DRM_GUID ), (DRM_DWORD_PTR*)f_ppbStack ) );
            ChkDR( DRM_DWordSub( *f_pcbStack, SIZEOF( DRM_GUID ), f_pcbStack ) );
            ChkDR( DRM_DWordAdd( *f_pcbLocalRevInfo, SIZEOF( DRM_GUID ), f_pcbLocalRevInfo ) );

            DWORD_TO_NETWORKBYTES( *f_ppbStack, 0 /* Offset 0 into buffer */, cbNext );
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbStack, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)f_ppbStack ) );
            ChkDR( DRM_DWordSub( *f_pcbStack, SIZEOF( DRM_DWORD ), f_pcbStack ) );
            ChkDR( DRM_DWordAdd( *f_pcbLocalRevInfo, SIZEOF( DRM_DWORD ), f_pcbLocalRevInfo ) );


            ChkDR( DRM_RVS_GetRevocationData(
                f_pRevocationStoreContext,
                c_rgpguidRevocationTypes[idx],
                *f_ppbStack,
                &cbNext ) );

            if( ( f_pdwRIV != NULL ) && ( idx == c_idxRevInfo2 ) )
            {
                DRM_RLVI  rlvi      = {0};
                DRM_DWORD dwUnused  = 0;
                ChkDR( DRM_RVK_ParseRevocationInfoHeader(
                    *f_ppbStack,
                    cbNext,
                    &rlvi,
                    &dwUnused ) );
                *f_pdwRIV = rlvi.head.dwRIV;
            }

            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbStack, cbNext, (DRM_DWORD_PTR*)f_ppbStack ) );
            ChkDR( DRM_DWordSub( *f_pcbStack, cbNext, f_pcbStack ) );
            ChkDR( DRM_DWordAdd( *f_pcbLocalRevInfo, cbNext, f_pcbLocalRevInfo ) );

        }
        else if( dr == DRM_E_DST_SLOT_NOT_FOUND )
        {
            /* Nothing available for this type.  This is fine. */
            dr = DRM_SUCCESS;
        }
        else
        {
            ChkDR( dr );
            AssertChkArg( FALSE );
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_CheckRemoteCertificateExpiration(
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __in                                                    DRM_DWORD                                         f_dwRemoteCertificateExpiration )
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRMFILETIME ftCertTime      = { 0 };
    DRM_UINT64  ui64CertTime    = DRM_UI64LITERAL( 0, 0 );
    DRMFILETIME ftCurrentTime   = { 0 };
    DRM_UINT64  ui64CurrentTime = DRM_UI64LITERAL( 0, 0 );

    if( f_dwRemoteCertificateExpiration != DRM_BCERT_DEFAULT_EXPIRATION_DATE )
    {
        CREATE_FILE_TIME( f_dwRemoteCertificateExpiration, ftCertTime );
        FILETIME_TO_UI64( ftCertTime, ui64CertTime );

        /* returns void */
        Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ftCurrentTime );
        FILETIME_TO_UI64( ftCurrentTime, ui64CurrentTime );

        ChkBOOL( !DRM_UI64Les( ui64CertTime, ui64CurrentTime ), DRM_E_BCERT_BASICINFO_CERT_EXPIRED );
    }

ErrorExit:
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_FormatSignatureCallback(
    __in_ecount( 1 )                                        DRM_VOID                                         *f_pvSignatureContext,
    __in                                                    DRM_PRND_SIGNATURE_OPERATION                      f_eSignatureOperation,
    __in_bcount( f_cbData )                           const DRM_BYTE                                         *f_pbData,
    __in                                                    DRM_DWORD                                         f_cbData,
    __out_bcount( f_cbSignatureBuffer )                     DRM_BYTE                                         *f_pbSignatureBuffer,
    __in                                                    DRM_DWORD                                         f_cbSignatureBuffer,
    __in                                                    DRM_DWORD                                         f_iSignatureBuffer,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaMIMKEncryptedWithReceiverPubkey,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaReceiverCertificate )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT                            dr                                                = DRM_SUCCESS;
    DRM_PRND_INTERNAL_SIGNATURE_CONTEXT  *poPrndInternalSignatureContext                    = (DRM_PRND_INTERNAL_SIGNATURE_CONTEXT*)f_pvSignatureContext;
    DRM_BYTE                             *pbSig                                             = NULL;
    DRM_BB_CONTEXT                       *pBBXContext                                       = NULL;
    PUBKEY_P256                           oPubKey                                           = {0};
    OEM_CRYPTO_HANDLE                    *phandleMIMK                                       = NULL;

    AssertChkArg( poPrndInternalSignatureContext != NULL );
    AssertChkArg( f_pbData                       != NULL );
    AssertChkArg( f_cbData                       >  0    );
    AssertChkArg( f_pbSignatureBuffer            != NULL );
    AssertChkArg( f_cbSignatureBuffer            >  0    );

    if( poPrndInternalSignatureContext->poPrndTransmitterContext != NULL )
    {
        pBBXContext                             = poPrndInternalSignatureContext->poPrndTransmitterContext->pBBXContext;
        phandleMIMK                             = &poPrndInternalSignatureContext->poPrndTransmitterContext->oPrndTransmitterSessionContext.handleMIMK;
    }
    else if( poPrndInternalSignatureContext->poPrndReceiverContext != NULL )
    {
        pBBXContext                             = poPrndInternalSignatureContext->poPrndReceiverContext->pBBXContext;
        phandleMIMK                             = &poPrndInternalSignatureContext->poPrndReceiverContext->oPrndReceiverSessionContext.handleMIMK;
    }
    else
    {
        DRMASSERT( FALSE );
    }

    AssertChkArg( pBBXContext != NULL );

    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pbSignatureBuffer, f_iSignatureBuffer, (DRM_DWORD_PTR*)&pbSig ) );

    switch( f_eSignatureOperation )
    {
    case DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_ECC256_RECEIVER_SIGNING_KEY:
        AssertChkArg( f_cbSignatureBuffer == DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY );
        AssertChkArg( f_pxbbaMIMKEncryptedWithReceiverPubkey                == NULL );
        AssertChkArg( f_pxbbaReceiverCertificate                            == NULL );
        AssertChkArg( poPrndInternalSignatureContext->poPrndReceiverContext != NULL );

        DRMCASSERT( DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY == ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

        ChkDR( DRM_BBX_ECC256_Sign(
            pBBXContext,
            f_pbData,
            f_cbData,
            &oPubKey,
            pbSig ) );

        break;
    case DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_ECC256_RECEIVER_SIGNING_KEY:
        AssertChkArg( f_cbSignatureBuffer == DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY );
        AssertChkArg( f_pxbbaMIMKEncryptedWithReceiverPubkey     == NULL );
        AssertChkArg( f_pxbbaReceiverCertificate                 != NULL );

        DRMCASSERT( DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY == SIZEOF( SIGNATURE_P256 ) );

        /* Don't re-verify a signature we just verified */
        ChkDR( DRM_BCert_GetPublicKeyByUsage(
            XBBA_TO_PB( *f_pxbbaReceiverCertificate ),
            XBBA_TO_CB( *f_pxbbaReceiverCertificate ),
            0, /* Leaf cert */
            DRM_BCERT_KEYUSAGE_SIGN,
            &oPubKey,
            NULL,
            NULL,
            NULL ) );

        ChkDR( OEM_ECDSA_Verify_P256(
            f_pbData,
            f_cbData,
            &oPubKey,
            (const SIGNATURE_P256*)pbSig,
            (struct bigctx_t *)pBBXContext->CryptoContext.rgbCryptoContext ) );

        break;
    case DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI:
        AssertChkArg( f_cbSignatureBuffer == DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI );
        AssertChkArg( f_pxbbaMIMKEncryptedWithReceiverPubkey    == NULL );
        AssertChkArg( f_pxbbaReceiverCertificate                == NULL );
        AssertChkArg( phandleMIMK                               != NULL );

        DRMCASSERT( DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI == DRM_AES_BLOCKLEN );

        if( poPrndInternalSignatureContext->poPrndTransmitterContext != NULL )
        {
            ChkDR( DRM_BBX_PRND_Tx_SignWithAESOMAC(
                pBBXContext,
                *phandleMIMK,
                f_pbData,
                f_cbData,
                pbSig ) );
        }
        else if( poPrndInternalSignatureContext->poPrndReceiverContext != NULL )
        {
            ChkDR( DRM_BBX_PRND_Rx_SignWithAESOMAC(
                pBBXContext,
                *phandleMIMK,
                f_pbData,
                f_cbData,
                pbSig ) );
        }
        else
        {
            DRMASSERT( FALSE );
        }
        break;
    case DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI:
        AssertChkArg( f_cbSignatureBuffer        == DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI );
        AssertChkArg( f_pxbbaReceiverCertificate == NULL );

        AssertChkArg( phandleMIMK != NULL );

        if( ( f_pxbbaMIMKEncryptedWithReceiverPubkey != NULL )
         && ( *phandleMIMK == OEM_CRYPTO_HANDLE_INVALID ) )
        {
            DRMCASSERT( DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI == DRM_AES_BLOCKLEN );
            ChkBOOL( XBBA_TO_CB( *f_pxbbaMIMKEncryptedWithReceiverPubkey ) == SIZEOF( CIPHERTEXT_P256 ), DRM_E_PRND_MESSAGE_INVALID );
            ChkDR( DRM_BBX_PRND_Rx_ECCDecryptAESKeyPair(
                pBBXContext,
                DRM_BBX_KEY_PAIR_TYPE_MIMK,
                (const CIPHERTEXT_P256*) XBBA_TO_PB( *f_pxbbaMIMKEncryptedWithReceiverPubkey ),
                phandleMIMK ) );
        }

        if( poPrndInternalSignatureContext->poPrndTransmitterContext != NULL )
        {
            ChkDR( DRM_BBX_PRND_Tx_VerifySignatureWithAESOMAC(
                    pBBXContext,
                   *phandleMIMK,
                    f_pbData,
                    f_cbData,
                    pbSig ) );

        }
        else if( poPrndInternalSignatureContext->poPrndReceiverContext != NULL )
        {
            ChkDR( DRM_BBX_PRND_Rx_VerifySignatureWithAESOMAC(
                    pBBXContext,
                    *phandleMIMK,
                    f_pbData,
                    f_cbData,
                    pbSig ) );
        }
        else
        {
            DRMASSERT( FALSE );
        }
        break;
    default:
        AssertChkArg( FALSE );
        break;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_OpenPRNDStore(
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __inout_ecount( 1 )                                     DRM_LICSTORE_CONTEXT                             *f_poLSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_DST                                          *f_poDSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_EST_CONTEXT                                  *f_poESTContextPRNDStore,
    __inout_ecount( f_cbPRNDStore )                   const DRM_BYTE                                         *f_pbPRNDStore,
    __in                                                    DRM_DWORD                                         f_cbPRNDStore,
    __out                                                   DRM_BOOL                                         *f_pfOpened )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    AssertChkArg( f_poLSTContextPRNDStore != NULL );
    AssertChkArg( f_poDSTContextPRNDStore != NULL );
    AssertChkArg( f_poESTContextPRNDStore != NULL );
    AssertChkArg( f_pbPRNDStore           != NULL );
    AssertChkArg( f_cbPRNDStore            > 0    );
    AssertChkArg( f_pfOpened              != NULL );

    if( !(*f_pfOpened) )
    {
        ChkDR( DRM_DST_Init(
            eDRM_DST_EMBEDDED,
            ( DRM_DST_CONTEXT* )f_poESTContextPRNDStore,
            SIZEOF( DRM_EST_CONTEXT ),
            f_poDSTContextPRNDStore ) );

        ChkDR( DRM_DST_CreateStore(
            f_pOEMContext,
            f_pbPRNDStore,
            f_cbPRNDStore,
            0,
            eDRM_DSTBLKNUM_DWORD,
            FALSE,
            0,
            f_poDSTContextPRNDStore ) );

        ChkDR( DRM_LST_Open(
            f_poLSTContextPRNDStore,
            f_poDSTContextPRNDStore,
            eDRM_LICENSE_STORE_XMR ) );

        *f_pfOpened = TRUE;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_PRND_ClosePRNDStore(
    __inout_ecount( 1 )                                     DRM_LICSTORE_CONTEXT                             *f_poLSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_DST                                          *f_poDSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_EST_CONTEXT                                  *f_poESTContextPRNDStore,
    __inout_ecount( f_cbPRNDStore )                         DRM_BYTE                                         *f_pbPRNDStore,
    __in                                                    DRM_DWORD                                         f_cbPRNDStore,
    __out                                                   DRM_BOOL                                         *f_pfOpened )
{
    DRMASSERT( f_poLSTContextPRNDStore != NULL );
    DRMASSERT( f_poDSTContextPRNDStore != NULL );
    DRMASSERT( f_poESTContextPRNDStore != NULL );
    DRMASSERT( f_pbPRNDStore           != NULL );
    DRMASSERT( f_pfOpened              != NULL );
    __analysis_assume( f_poLSTContextPRNDStore != NULL );
    __analysis_assume( f_poDSTContextPRNDStore != NULL );
    __analysis_assume( f_poESTContextPRNDStore != NULL );
    __analysis_assume( f_pbPRNDStore           != NULL );
    __analysis_assume( f_pfOpened              != NULL );

    (DRM_VOID) DRM_LST_Close(      f_poLSTContextPRNDStore );
    (DRM_VOID) DRM_DST_CloseStore( f_poDSTContextPRNDStore );
    (DRM_VOID) DRM_DST_Uninit(     f_poDSTContextPRNDStore );

    OEM_SECURE_ZERO_MEMORY( f_poLSTContextPRNDStore, SIZEOF( DRM_LICSTORE_CONTEXT ) );
    OEM_SECURE_ZERO_MEMORY( f_poDSTContextPRNDStore, SIZEOF( DRM_DST              ) );
    OEM_SECURE_ZERO_MEMORY( f_poESTContextPRNDStore, SIZEOF( DRM_EST_CONTEXT      ) );
    OEM_SECURE_ZERO_MEMORY( f_pbPRNDStore,           f_cbPRNDStore                  );

    *f_pfOpened = FALSE;
}

EXIT_PK_NAMESPACE_CODE;

