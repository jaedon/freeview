/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMLICGEN_C

#include <drmlicgen.h>
#include <drmlicgenxmrhelper.h>

#include <drmconstants.h>
#include <drmdatastore.h>
#include <drmest.h>
#include <drmprndprotocol.h>
#include <drmmanager.h>
#include <drmxmrbuilder.h>
#include <drmlicgentypes.h>
#include <drmbcertparser.h>
#include <drmbcrl.h>
#include <drmmanagerimpl.h>
#include <drmsecureclock.h>
#include <drmantirollbackclock.h>
#include <drmmathsafe.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
** EST Header Size plus a DWORD for priority of a license
** See the definition of DRM_EST_HEADER_LEN and DRM_EST_SLOTHEADER_LEN for format information
*/
#define EXTRA_EST_LEN_FOR_A_LICENSE     ( DRM_EST_HEADER_LEN + DRM_EST_SLOTHEADER_LEN + SIZEOF( DRM_DWORD ) )

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_LICGEN_IsLicGenSupported(DRM_VOID) { return TRUE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_LICGEN_IsLicGenUnsupported(DRM_VOID) { return FALSE; }

static DRM_NO_INLINE DRM_RESULT _FindAndCleanLicenseKeyCache(
    __inout                             DRM_APP_CONTEXT                      *f_poAppContext,
    __in                          const DRM_KID                              *f_pKid,
    __out_opt                           DRM_LOCAL_LICENSE_KEY_CACHE         **f_ppoLicenseKeyCache ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _FindAndCleanLicenseKeyCache(
    __inout                             DRM_APP_CONTEXT                      *f_poAppContext,
    __in                          const DRM_KID                              *f_pKid,
    __out_opt                           DRM_LOCAL_LICENSE_KEY_CACHE         **f_ppoLicenseKeyCache )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_KEY_CACHE *poLicenseKeyCache      = NULL;
    DRM_LOCAL_LICENSE_KEY_CACHE *poPrevLicenseKeyCache  = NULL;
    DRM_LOCAL_LICENSE_KEY_CACHE *poFoundLicenseKeyCache = NULL;

    ChkArg( f_poAppContext       != NULL );
    ChkArg( f_pKid               != NULL );

    ChkArg( ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext != NULL );

    poLicenseKeyCache = ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext->poLicenseKeyCache;

    while( poLicenseKeyCache != NULL )
    {
        if( OEM_SECURE_ARE_EQUAL( f_pKid, &poLicenseKeyCache->kid, SIZEOF( DRM_KID ) ) )
        {
            /* reset the access time */
            Oem_Clock_GetSystemTimeAsFileTime(
                NULL,
               &poLicenseKeyCache->ftAccessTime );

            DRMASSERT( poFoundLicenseKeyCache == NULL );
            poFoundLicenseKeyCache = poLicenseKeyCache;
        }
        else
        {
            if( DRM_UTL_IsTimeoutExceeded(
                    NULL,
                   &poLicenseKeyCache->ftAccessTime,
                    DRM_MAX_LOCAL_LICENSE_CACHE_TIME_IN_SECONDS ) )
            {
                DRM_LOCAL_LICENSE_KEY_CACHE *pTempLicense = poLicenseKeyCache->poNextLicenseCache;

                /* root case */
                if( poPrevLicenseKeyCache == NULL )
                {
                    ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext->poLicenseKeyCache = pTempLicense;
                }
                else
                {
                    poPrevLicenseKeyCache->poNextLicenseCache = pTempLicense;
                }

                Drm_Prnd_MemFree( (DRM_BYTE*)poLicenseKeyCache );

                /* previous license is not update and we will continue */
                poLicenseKeyCache = pTempLicense;

                continue;
            }
        }

        poPrevLicenseKeyCache = poLicenseKeyCache;
        poLicenseKeyCache     = poLicenseKeyCache->poNextLicenseCache;

    }

    if( f_ppoLicenseKeyCache != NULL )
    {
        *f_ppoLicenseKeyCache = poFoundLicenseKeyCache;
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _CreateLicenseCache(
    __inout                                                   DRM_BB_CONTEXT                      *f_poBBXContext,
    __in                                                const DRM_KID                             *f_pKid,
    __in                                                const OEM_CRYPTO_HANDLE                    f_hAESGenericKey,
    __in                                                const OEM_CRYPTO_HANDLE                    f_hContentEncryptionKey,
    __in_ecount( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES )    const DRM_BYTE                            *f_pbCICKEncryptedWithPubKey,
    __in_ecount( DRM_AES_BLOCKLEN * 2 )                 const DRM_BYTE                            *f_pbCICKENcryptedWithRootAESECBKey,
    __out                                                     DRM_LOCAL_LICENSE_KEY_CACHE        **f_ppoLicenseKeyCache ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CreateLicenseCache(
    __inout                                                   DRM_BB_CONTEXT                      *f_poBBXContext,
    __in                                                const DRM_KID                             *f_pKid,
    __in                                                const OEM_CRYPTO_HANDLE                    f_hAESGenericKey,
    __in                                                const OEM_CRYPTO_HANDLE                    f_hContentEncryptionKey,
    __in_ecount( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES )    const DRM_BYTE                            *f_pbCICKEncryptedWithPubKey,
    __in_ecount( DRM_AES_BLOCKLEN * 2 )                 const DRM_BYTE                            *f_pbCICKENcryptedWithRootAESECBKey,
    __out                                                     DRM_LOCAL_LICENSE_KEY_CACHE        **f_ppoLicenseKeyCache )
{
    DRM_RESULT                   dr               = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_KEY_CACHE *poLicenseKeyCache = NULL;

    ChkArg( f_poBBXContext                     != NULL );
    ChkArg( f_pKid                             != NULL );
    ChkArg( f_hAESGenericKey                   != OEM_CRYPTO_HANDLE_INVALID );
    ChkArg( f_hContentEncryptionKey            != OEM_CRYPTO_HANDLE_INVALID );
    ChkArg( f_pbCICKEncryptedWithPubKey        != NULL );
    ChkArg( f_pbCICKENcryptedWithRootAESECBKey != NULL );
    ChkArg( f_ppoLicenseKeyCache               != NULL );

    *f_ppoLicenseKeyCache = NULL;

    ChkMem( poLicenseKeyCache = (DRM_LOCAL_LICENSE_KEY_CACHE*)Drm_Prnd_MemAlloc( SIZEOF( DRM_LOCAL_LICENSE_KEY_CACHE ) ) );
    OEM_SECURE_ZERO_MEMORY( poLicenseKeyCache, SIZEOF( DRM_LOCAL_LICENSE_KEY_CACHE ) );

    ChkDR( DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(
        f_poBBXContext,
        f_hAESGenericKey,
        f_hContentEncryptionKey,
        poLicenseKeyCache->rgbEncryptedAESKeyPair ) );

    OEM_SECURE_MEMCPY(
       &poLicenseKeyCache->kid,
        f_pKid,
        SIZEOF( DRM_KID ) );

    OEM_SECURE_MEMCPY(
        poLicenseKeyCache->rgbCICKEncryptedWithPubkey,
        f_pbCICKEncryptedWithPubKey,
        ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

    OEM_SECURE_MEMCPY(
        poLicenseKeyCache->rgbCICKEncryptedWithROOTAESECBKey,
        f_pbCICKENcryptedWithRootAESECBKey,
        DRM_AES_BLOCKLEN * 2 );

    Oem_Clock_GetSystemTimeAsFileTime(
        NULL,
       &poLicenseKeyCache->ftAccessTime );

    poLicenseKeyCache->poNextLicenseCache = NULL;

    *f_ppoLicenseKeyCache = poLicenseKeyCache;
    poLicenseKeyCache     = NULL;

ErrorExit:
    {
        DRM_BYTE *pbLicenseKey = (DRM_BYTE*)poLicenseKeyCache;
        DRM_PRND_SAFEMEMFREE( pbLicenseKey );
    }
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _CreateRMHeader(
    __in_opt                      const DRM_KID                             *f_pKeyId,
    __out                               DRM_DWORD                           *f_pcbRMHeader,
    __out_bcount( *f_pcbRMHeader )      DRM_BYTE                           **f_ppbRMHeader ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CreateRMHeader(
    __in_opt                      const DRM_KID                             *f_pKeyId,
    __out                               DRM_DWORD                           *f_pcbRMHeader,
    __out_bcount( *f_pcbRMHeader )      DRM_BYTE                           **f_ppbRMHeader )
{
    DRM_RESULT         dr                                                     = DRM_SUCCESS;
    DRM_DWORD          cbRMHeader                                             = 0;
    DRM_BYTE          *pbRMHeader                                             = NULL;
    DRM_WCHAR          rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]  = { 0 };
    DRM_DWORD          cchKid                                                 = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
    DRM_CONST_STRING   dstrKid                                                = { 0 };

    ChkArg( f_pcbRMHeader != NULL );
    ChkArg( f_ppbRMHeader != NULL );

    *f_pcbRMHeader = 0;
    *f_ppbRMHeader = NULL;

    /* convert binary KID to b64 encode */
    if( f_pKeyId != NULL )
    {
        ChkDR( DRM_B64_EncodeW(
            (const DRM_BYTE*)f_pKeyId,
            SIZEOF( DRM_KID ),
            rgwchKid,
           &cchKid,
            0 ) );

        rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;

        /* cchKid of DSTR_FROM_PB is in BYTES and not WCHAR */
        DSTR_FROM_PB( &dstrKid, rgwchKid, cchKid * SIZEOF( DRM_WCHAR ) );
    }

    dr = DRM_PRO_CreateRMHeader(
        DRM_HEADER_VERSION_4_1,
        eDRM_AES_COUNTER_CIPHER,
        f_pKeyId != NULL ? &dstrKid : NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        f_pKeyId == NULL ? &g_dstrTagONDEMAND : NULL, /* if there's a KID present, we don't create OnDemand header */
        NULL,
       &cbRMHeader );

    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );

    DRMASSERT( cbRMHeader > 0 );
    ChkMem( pbRMHeader = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbRMHeader ) );

    ChkDR( DRM_PRO_CreateRMHeader(
        DRM_HEADER_VERSION_4_1,
        eDRM_AES_COUNTER_CIPHER,
        f_pKeyId != NULL ? &dstrKid : NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        f_pKeyId == NULL ? &g_dstrTagONDEMAND : NULL,
        pbRMHeader,
       &cbRMHeader ) );

    *f_pcbRMHeader = cbRMHeader;
    *f_ppbRMHeader = pbRMHeader;
    pbRMHeader     = NULL;

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbRMHeader );
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _CreatePRO(
    __in_opt                      const DRM_DWORD                            f_cbRMHeader,
    __in_opt                      const DRM_BYTE                            *f_pbRMHeader,
    __inout                             DRM_DWORD                           *f_pcbPROBlob,
    __out_bcount( *f_pcbPROBlob )       DRM_BYTE                           **f_ppbPROBlob,
    __in                          const DRM_DWORD                            f_cbEmbeddedStore ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CreatePRO(
    __in_opt                      const DRM_DWORD                            f_cbRMHeader,
    __in_opt                      const DRM_BYTE                            *f_pbRMHeader,
    __inout                             DRM_DWORD                           *f_pcbPROBlob,
    __out_bcount( *f_pcbPROBlob )       DRM_BYTE                           **f_ppbPROBlob,
    __in                          const DRM_DWORD                            f_cbEmbeddedStore )
{
    DRM_RESULT         dr        = DRM_SUCCESS;
    DRM_DWORD          cbPROBlob = 0;
    DRM_BYTE          *pbPROBlob = NULL;
    DRM_CONST_STRING   dstrPRO   = EMPTY_DRM_STRING;

    ChkArg( f_pcbPROBlob != NULL );
    ChkArg( f_ppbPROBlob != NULL );

    *f_pcbPROBlob = 0;
    *f_ppbPROBlob = NULL;

    if( f_pbRMHeader != NULL )
    {
        ChkArg( f_cbRMHeader > 0 );

        DSTR_FROM_PB( &dstrPRO, f_pbRMHeader, f_cbRMHeader );
    }

    dr = DRM_PRO_Create(
        f_pbRMHeader == NULL ? NULL : &dstrPRO,
        NULL,
        &cbPROBlob,
        f_cbEmbeddedStore );

    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );

    DRMASSERT( cbPROBlob > 0 );
    ChkMem( pbPROBlob = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbPROBlob ) );

    ChkDR( DRM_PRO_Create(
        f_pbRMHeader == NULL ? NULL : &dstrPRO,
        pbPROBlob,
        &cbPROBlob,
        f_cbEmbeddedStore ) );

    *f_pcbPROBlob = cbPROBlob;
    *f_ppbPROBlob = pbPROBlob;
    pbPROBlob     = NULL;

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbPROBlob );
    return dr;

}

static DRM_NO_INLINE DRM_RESULT _AddLicense(
    __inout       DRM_LICSTORE_CONTEXT                *pLicStoreCtx,
    __in    const DRM_LOCAL_LICENSE_SESSION_CONTEXT   *poLicenseSession ) DRM_NO_INLINE_ATTRIBUTE;


static DRM_NO_INLINE DRM_RESULT _AddLicense(
    __inout       DRM_LICSTORE_CONTEXT                *pLicStoreCtx,
    __in    const DRM_LOCAL_LICENSE_SESSION_CONTEXT   *poLicenseSession )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  cbLicenseToStore = 0;
    DRM_BYTE  *pbLicenseToStore = NULL;
    DRM_BYTE  *pbLicenseBuffer  = NULL;

    ChkArg( pLicStoreCtx     != NULL );
    ChkArg( poLicenseSession != NULL );

    ChkDR( DRM_DWordAdd( poLicenseSession->cbXMRLicense, SIZEOF( DRM_DWORD ), &cbLicenseToStore ) );

    ChkMem( pbLicenseToStore = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbLicenseToStore ) );

    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbLicenseToStore, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)&pbLicenseBuffer ) );
    OEM_SECURE_MEMCPY(
        pbLicenseBuffer,
        poLicenseSession->pbXMRLicense,
        poLicenseSession->cbXMRLicense );

    ChkDR( DRM_LST_AddLicense(
        pLicStoreCtx,
        poLicenseSession->cbXMRLicense, /* Size WITHOUT priority */
        pbLicenseToStore,               /* Buffer WITH Priority */
       &poLicenseSession->kid,
       &poLicenseSession->lid,
        0 ) );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbLicenseToStore );
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _VerifyValidLicenseInput(
    __inout                                    DRM_APP_CONTEXT                     *f_poAppContext,
    __in                                 const DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor,
    __in                                 const DRM_LOCAL_LICENSE_TYPE               f_eLicenseType,
    __inout                                    DRM_DWORD                           *f_pcbRemoteCert,
    __inout_ecount(*f_pcbRemoteCert)           DRM_BYTE                           **f_ppbRemoteCert,
    __out                                      DRM_DWORD                           *f_pcbRevDataBuffer,
    __out_ecount(*f_pcbRevDataBuffer)          DRM_BYTE                           **f_ppbRevDataBuffer,
    __out                                      DRM_PUBLIC_KEY_CONTEXT              *f_pRemotePubKey ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _VerifyValidLicenseInput(
    __inout                                    DRM_APP_CONTEXT                     *f_poAppContext,
    __in                                 const DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor,
    __in                                 const DRM_LOCAL_LICENSE_TYPE               f_eLicenseType,
    __inout                                    DRM_DWORD                           *f_pcbRemoteCert,
    __inout_ecount(*f_pcbRemoteCert)           DRM_BYTE                           **f_ppbRemoteCert,
    __out                                      DRM_DWORD                           *f_pcbRevDataBuffer,
    __out_ecount(*f_pcbRevDataBuffer)          DRM_BYTE                           **f_ppbRevDataBuffer,
    __out                                      DRM_PUBLIC_KEY_CONTEXT              *f_pRemotePubKey )
{
    DRM_RESULT                            dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL             *poAppContextInternal = NULL;
    DRM_BYTE                             *pbRevDataBuffer      = NULL;
    DRM_DWORD                             cbRevDataBuffer      = 0;
    DRM_BB_CONTEXT                       *pBBContext           = NULL;
    DRM_BINARY_DEVICE_CERT_CACHED_VALUES  oCache               = { 0 };
    DRM_REVOCATIONSTORE_CONTEXT          *pRevStoreContext     = NULL;
    DRM_DWORD                             dwIdx                = 0;
    DRM_DWORD                             dwIdx2               = 0;

    ChkArg( f_poAppContext     != NULL );
    ChkArg( f_poDescriptor     != NULL );
    ChkArg( f_pcbRemoteCert    != NULL );
    ChkArg( f_ppbRemoteCert    != NULL );
    ChkArg( f_pRemotePubKey    != NULL );
    ChkArg( f_ppbRevDataBuffer != NULL );
    ChkArg( f_pcbRevDataBuffer != NULL );

    *f_ppbRevDataBuffer = NULL;
    *f_pcbRevDataBuffer = 0;

    poAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *)f_poAppContext;
    pRevStoreContext     = &poAppContextInternal->oRevContext;

    /* Get the revocation data */
    dr = DRM_RVS_GetRevocationData(
        pRevStoreContext,
        &g_guidRevocationTypePlayReadySilverLightRuntime,
        pbRevDataBuffer,
        &cbRevDataBuffer );

    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        dr = DRM_SUCCESS; /* No error if there's no CRL available */
    }
    else if( dr == DRM_E_BUFFERTOOSMALL )
    {
        DRMASSERT( cbRevDataBuffer > 0 );
        ChkMem( pbRevDataBuffer = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbRevDataBuffer ) );

        ChkDR( DRM_RVS_GetRevocationData(
            pRevStoreContext,
            &g_guidRevocationTypePlayReadySilverLightRuntime,
            pbRevDataBuffer,
            &cbRevDataBuffer ) );

        DRMASSERT( cbRevDataBuffer > 0 );

        *f_ppbRevDataBuffer = pbRevDataBuffer;
        pbRevDataBuffer     = NULL;
        *f_pcbRevDataBuffer = cbRevDataBuffer;
        cbRevDataBuffer     = 0;
    }
    else
    {
        ChkDR( dr );
    }

    ChkArg( f_poDescriptor->cPlayEnablers <= DRM_MAX_LOCAL_LICENSE_PLAY_ENABLERS );

    /* check for duplicate PlayEnabler */
    for( dwIdx = 0; dwIdx < f_poDescriptor->cPlayEnablers; dwIdx++ )
    {
        for( dwIdx2 = dwIdx + 1; dwIdx2 < f_poDescriptor->cPlayEnablers; dwIdx2++ )
        {
            if( IDENTICAL_GUIDS( 
                    &f_poDescriptor->rgoPlayEnablers[dwIdx],
                    &f_poDescriptor->rgoPlayEnablers[dwIdx2] ) )
            {
                ChkDR( DRM_E_LICGEN_DUPLICATE_PLAY_ENABLER );
            }
        }
    }

    if(    f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT
        || f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE )
    {
        pBBContext = &poAppContextInternal->oBlackBoxContext;

        if( !f_poDescriptor->fCannotPersist )
        {
            ChkDR( DRM_E_LICGEN_PERSISTENT_REMOTE_LICENSE );
        }

        if( f_poDescriptor->oExpirationAfterFirstPlay.fValid )
        {
            ChkDR( DRM_E_LICGEN_EXPIRE_AFTER_FIRST_PLAY_REMOTE_LICENSE );
        }

        /* Only allow Passing To Unknown Output and Passing Constrained Resolution to Unknown Output PlayEnabler */
        for( dwIdx = 0; dwIdx < f_poDescriptor->cPlayEnablers; dwIdx++ )
        {
            if(    !IDENTICAL_GUIDS( &f_poDescriptor->rgoPlayEnablers[dwIdx],
                                     &DRM_PLAYENABLER_UNKNOWN_OUTPUT )
                && !IDENTICAL_GUIDS( &f_poDescriptor->rgoPlayEnablers[dwIdx],
                                     &DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT ) )
            {
                ChkDR( DRM_E_LICGEN_PLAY_ENABLER_REMOTE_LICENSE );
            }
        }

        if( *f_ppbRemoteCert != NULL )
        {
            DRM_BCERT_CERTIFICATE                   oCert                = { 0 };
            DRM_BCERT_CHAIN_HEADER                  oCertChainHeader     = { 0 };
            DRM_DWORD                               iChainOffset         = 0;

            /* Verify security level of the certificate */
            ChkDR( DRM_BCert_LoadPropertiesCache(
               *f_ppbRemoteCert,
               *f_pcbRemoteCert,
               &oCache,
                DRM_BBX_GetKeyHandleCallbacks( pBBContext ),
                pBBContext ) );

            ChkDR( DRM_BCert_GetChainHeader(
               *f_ppbRemoteCert,
               *f_pcbRemoteCert,
               &iChainOffset,
               &oCertChainHeader ) );

            ChkDR( DRM_BCert_GetCertificate(
               *f_ppbRemoteCert,
               *f_pcbRemoteCert,
               &iChainOffset,
               &oCert,
                DRM_BCERT_CERTTYPE_UNKNOWN ) );

            ChkBOOL( oCert.BasicInfo.dwSecurityLevel >= f_poDescriptor->wSecurityLevel,
                DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

            ChkDR( DRM_BBX_TransferPublicKeyContext( pBBContext,
                                                     f_pRemotePubKey,
                                                     &oCache.DeviceKeyEncrypt.PublicKeyContext ) );
        }
        else
        {
            /* If no certificate is supplied, the PRND Session's remote cert will be used */
            DRM_BOOL                              fSessionStillValid        = FALSE;
            DRM_PRND_TRANSMITTER_SESSION_CONTEXT *poPrndTransSessionContext = NULL;

            ChkBOOL( poAppContextInternal->poPrndTransmitterContext != NULL, DRM_E_DEVICE_NOT_REGISTERED );

            ChkDR( DRM_PRND_Transmitter_IsSessionStillValid(
                poAppContextInternal->poPrndTransmitterContext,
               &fSessionStillValid ) );

            if( !fSessionStillValid )
            {
                ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
            }

            poPrndTransSessionContext = &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext;

            ChkBOOL( poPrndTransSessionContext->dwReceiverSecurityLevel >= f_poDescriptor->wSecurityLevel,
                DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

            ChkDR( DRM_BBX_DuplicatePublicKeyContext( pBBContext,
                                                      f_pRemotePubKey,
                                                      &poPrndTransSessionContext->oECC256PubKeyContentEncryptReceiver ) );

            *f_ppbRemoteCert    = poPrndTransSessionContext->pbReceiverCertificate;
            *f_pcbRemoteCert    = poPrndTransSessionContext->cbReceiverCertificate;
        }
    }
    else
    {
        ChkBOOL( *f_ppbRemoteCert == NULL, DRM_E_LICGEN_LOCAL_LICENSE_WITH_REMOTE_CERTIFICATE );
    }

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbRevDataBuffer );

    if( pBBContext != NULL )
    {
        DRM_BCert_CleanupPropertiesCache( &oCache,
                                          DRM_BBX_GetKeyHandleCallbacks( pBBContext ),
                                          pBBContext );
    }

    return dr;
}

static DRM_NO_INLINE DRM_RESULT _CleanLocalLicenseContext(
    __inout                             DRM_BB_CONTEXT                      *f_poBBXContext,
    __inout                             DRM_LOCAL_LICENSE_CONTEXT           *f_poLocalLicenseContext ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CleanLocalLicenseContext(
    __inout                             DRM_BB_CONTEXT                      *f_poBBXContext,
    __inout                             DRM_LOCAL_LICENSE_CONTEXT           *f_poLocalLicenseContext )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_KEY_CACHE *poLicenseKeyCache      = NULL;

    ChkArg( f_poBBXContext != NULL );

    if( f_poLocalLicenseContext == NULL )
    {
        /* Avoid ChkArg tracing for a common scenario. */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    poLicenseKeyCache = f_poLocalLicenseContext->poLicenseKeyCache;

    DRM_BBX_SafeReleaseKey(
        f_poBBXContext,
        &f_poLocalLicenseContext->hAESGenericKeyContent );

    DRM_BBX_SafeReleaseKey(
        f_poBBXContext,
        &f_poLocalLicenseContext->hAESGenericKeyRoot );

    while( poLicenseKeyCache != NULL )
    {
        DRM_LOCAL_LICENSE_KEY_CACHE *poTempLicenseKeyCache = poLicenseKeyCache->poNextLicenseCache;

        Drm_Prnd_MemFree( (DRM_BYTE*)poLicenseKeyCache );
        poLicenseKeyCache = poTempLicenseKeyCache;
    }

    Drm_Prnd_MemFree( (DRM_BYTE*)f_poLocalLicenseContext );
ErrorExit:
    return dr;
}



DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LOCALLICENSE_CleanCache(
    __inout                             DRM_APP_CONTEXT                     *f_poAppContext )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;

    if( f_poAppContext == NULL )
    {
        /* Avoid ChkArg tracing for a common scenario. */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    ChkDR( _CleanLocalLicenseContext(
        &((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->oBlackBoxContext,
        ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext ) );

    ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext = NULL;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_InitializePolicyDescriptor(
    __inout                             DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE(( "Drm_LocalLicense_InitializePolicyDescriptor Entered" ));

    ChkArg( f_poDescriptor != NULL );

    OEM_SECURE_ZERO_MEMORY( f_poDescriptor, SIZEOF( DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR ) );

    f_poDescriptor->wSecurityLevel                   = 2000;
    f_poDescriptor->fCannotPersist                   = TRUE;

ErrorExit:
    TRACE(( "Drm_LocalLicense_InitializePolicyDescriptor Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreateLicense(
    __inout                               DRM_APP_CONTEXT                     *f_poAppContext,
    __in                            const DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR *f_poDescriptor,
    __in                            const DRM_LOCAL_LICENSE_TYPE               f_eLicenseType,
    __in                            const DRM_KID                             *f_pKeyId,
    __in_opt                        const DRM_DWORD                            f_cbRemoteCert,
    __in_ecount_opt(f_cbRemoteCert) const DRM_BYTE                            *f_pbRemoteCert,
    __in_opt                        const DRM_LICENSE_HANDLE                   f_hRootLicense,
    __out                                 DRM_LICENSE_HANDLE                  *f_phLicense )
{
    DRM_RESULT                                dr                                                            = DRM_SUCCESS;
    DRM_BYTE                                 *pbXMRBuilder                                                  = NULL;
    DRM_DWORD                                 cbXMRBuilder                                                  = DRM_MAX_LICENSESIZE;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT        *poLocalLicenseSession                                         = NULL;
    DRM_APP_CONTEXT_INTERNAL                 *poAppContextInternal                                          = NULL;
    DRM_LOCAL_LICENSE_KEY_CACHE              *poLicenseKeyCache                                             = NULL;
    DRM_LOCAL_LICENSE_KEY_CACHE              *poNewLicenseKeyCache                                          = NULL;
    DRM_LOCAL_LICENSE_CONTEXT                *poLocalLicenseContext                                         = NULL;
    DRM_BYTE                                  rgbCICKEncryptedWithPubkey[ECC_P256_CIPHERTEXT_SIZE_IN_BYTES] = { 0 };
    DRM_BYTE                                  rgbCICKEncryptedWithROOTAESECBKey[DRM_AES_BLOCKLEN * 2]       = { 0 };
    DRM_BYTE                                 *pbRemoteCert                                                  = NULL;
    DRM_DWORD                                 cbRemoteCert                                                  = 0;
    DRM_BYTE                                 *pbRevDataBuffer                                               = NULL;
    DRM_DWORD                                 cbRevDataBuffer                                               = 0;
    DRM_BYTE                                 *pbRevInfo                                                     = NULL;
    DRM_DWORD                                 cbRevInfo                                                     = 0;
    DRM_PUBLIC_KEY_CONTEXT                    oRemotePubKey                                                 = { 0 };

    /* XMR builder objects */
    DRM_XMR_BUILDER_CONTEXT                   oXmrBuilder                                                   = { 0 };
    DRM_XMR_MINIMUM_ENVIRONMENT               oXmrMinimumEnvironment                                        = { 0 };
    DRM_XMR_RIGHTS                            oXmrRights                                                    = { 0 };
    DRM_XMR_EXPIRATION_AFTER_FIRSTUSE         oXmrExpirationAfterFirstPlay                                  = { 0 };
    DRM_XMR_SOURCEID                          oXmrSourceID                                                  = { 0 };
    DRM_XMR_EXPIRATION                        oXmrExpiration                                                = { 0 };
    DRM_XMR_REAL_TIME_EXPIRATION              oXmrRealTimeExpiration                                        = { 0 };
    DRM_XMR_RESTRICTED_SOURCEID               oXmrRestrictedSourceID                                        = { 0 };
    DRM_XMR_CONTENT_KEY                       oXmrContentKey                                                = { 0 };
    DRM_XMR_ECC_DEVICE_KEY                    oXmrECCDeviceKey                                              = { 0 };
    DRM_XMR_UPLINK_KID                        oXmrUplinkKid                                                 = { 0 };
    DRM_XMR_ISSUEDATE                         oXmrIssueDate                                                 = { 0 };
    DRM_XMR_REVOCATION_INFORMATION_VERSION    oXmrRIV                                                       = { 0 };
    DRM_XMR_EMBEDDING_BEHAVIOR                oXmrEmbBehavior                                               = { 0 };
    DRM_XMR_PLAYBACK_RIGHTS                   oXmrPlayback                                                  = { 0 };
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS  oXmrMinOPL                                                    = { 0 };
    DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION  oXmrExplicitAV                                                = { 0 };
    DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION oXmrExplicitDA                                                = { 0 };
    DRM_BYTE                                  rgbUplinkChecksum[ DRM_AES_CK_CHECKSUM_LENGTH ]               = { 0 };
    /* license data for callback */
    Local_License_SignLicense_Data            oLicenseData                                                  = { 0 };
    DRM_BOOL                                  fRoot                                                         = f_eLicenseType == eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
                                                                                                           || f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT;

    TRACE(( "Drm_LocalLicense_CreateLicense Entered" ));

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poDescriptor != NULL );
    ChkArg( f_pKeyId       != NULL );
    ChkArg( f_phLicense    != NULL );

    /* We can only have root license handle if it's a leaf license */
    ChkArg(    ( f_eLicenseType == eDRM_LOCAL_LICENSE_LEAF && f_hRootLicense != DRM_LICENSE_HANDLE_INVALID )
            || ( (    f_eLicenseType == eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
                   || f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT
                   || f_eLicenseType == eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE
                   || f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE
                 )
                   && f_hRootLicense == DRM_LICENSE_HANDLE_INVALID ) );

    pbRemoteCert = (DRM_BYTE*)f_pbRemoteCert;
    cbRemoteCert = f_cbRemoteCert;
    ChkDR( _VerifyValidLicenseInput(
        f_poAppContext,
        f_poDescriptor,
        f_eLicenseType,
       &cbRemoteCert,
       &pbRemoteCert,
       &cbRevDataBuffer,
       &pbRevDataBuffer,
       &oRemotePubKey ) );

    poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    *f_phLicense = DRM_LICENSE_HANDLE_INVALID;

    ChkMem( pbXMRBuilder = (DRM_BYTE *)Drm_Prnd_MemAlloc( cbXMRBuilder ) );

    /* Initilize the license session context */
    ChkMem( poLocalLicenseSession = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)Drm_Prnd_MemAlloc( SIZEOF( DRM_LOCAL_LICENSE_SESSION_CONTEXT ) ) );
    OEM_SECURE_ZERO_MEMORY( poLocalLicenseSession, SIZEOF( DRM_LOCAL_LICENSE_SESSION_CONTEXT ) );
    /* this line must be immediately after zero-mem */
    poLocalLicenseSession->dwRefCount               = 1;
    poLocalLicenseSession->poBBXContext             = &poAppContextInternal->oBlackBoxContext;
    poLocalLicenseSession->poAppContext             = f_poAppContext;
    poLocalLicenseSession->handleCICK               = OEM_CRYPTO_HANDLE_INVALID;
    poLocalLicenseSession->eLicenseType             = f_eLicenseType;

    /* initialize the local license context if it's not already */
    if( poAppContextInternal->poLocalLicenseContext == NULL )
    {
        ChkMem( poLocalLicenseContext = (DRM_LOCAL_LICENSE_CONTEXT *)Drm_Prnd_MemAlloc( SIZEOF( DRM_LOCAL_LICENSE_CONTEXT ) ) );
        OEM_SECURE_ZERO_MEMORY( poLocalLicenseContext, SIZEOF( DRM_LOCAL_LICENSE_CONTEXT ) );

        ChkDR( DRM_BBX_PRND_Tx_GenerateAESGenericKey(
            poLocalLicenseSession->poBBXContext,
            OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,
           &poLocalLicenseContext->hAESGenericKeyContent ) );
        ChkDR( DRM_BBX_PRND_Tx_GenerateAESGenericKey(
            poLocalLicenseSession->poBBXContext,
            OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,
           &poLocalLicenseContext->hAESGenericKeyRoot ) );

        poAppContextInternal->poLocalLicenseContext = poLocalLicenseContext;
        poLocalLicenseContext = NULL;
    }

    /* Randomly generate license ID */
    ChkDR( Oem_Random_GetBytes(
        poLocalLicenseSession->poBBXContext->pOEMContext,
        poLocalLicenseSession->lid.rgb,
        SIZEOF( poLocalLicenseSession->lid ) ) );

    /* Start building the license */
    OEM_SECURE_MEMCPY( poLocalLicenseSession->kid.rgb, f_pKeyId, SIZEOF( poLocalLicenseSession->kid ) );
    ChkDR( DRM_XMR_StartLicense( pbXMRBuilder, cbXMRBuilder, &poLocalLicenseSession->lid, XMR_VERSION_3, &oXmrBuilder ) );

    ChkDR( SetSecurityLevel( &oXmrBuilder, f_poDescriptor->wSecurityLevel, &oXmrMinimumEnvironment ) );
    ChkDR( SetCannotPersist( &oXmrBuilder, f_poDescriptor->fCannotPersist, &oXmrRights ) );
    /* Save persist attribute for PersistLicense in the license session */
    poLocalLicenseSession->fCannotPersist = f_poDescriptor->fCannotPersist;

    /* Playback policy container need to be present for license binding */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &oXmrPlayback ) );
    ChkDR( DRM_XMR_AddObject( &oXmrBuilder, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &oXmrPlayback ) );

    if( ( poAppContextInternal->poPrndTransmitterContext != NULL )
     && ( ( f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT )
       || ( f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE ) ) )
    {
        ChkDR( SetExpirationDate(
            &oXmrBuilder,
            &f_poDescriptor->oExpiration,
            poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.dwSubtractFromBeginDate,
            poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.dwAddToEndDate,
            &oXmrExpiration ) );
    }
    else
    {
        ChkDR( SetExpirationDate(
            &oXmrBuilder,
            &f_poDescriptor->oExpiration,
            0,
            0,
            &oXmrExpiration ) );
    }

    ChkDR( SetExpirationAfterFirstPlay( &oXmrBuilder, &f_poDescriptor->oExpirationAfterFirstPlay, &oXmrExpirationAfterFirstPlay ) );
    ChkDR( SetRealTimeExpiration( &oXmrBuilder, f_poDescriptor->fRealTimeExpiration, &oXmrRealTimeExpiration ) );
    ChkDR( SetSourceID( &oXmrBuilder, &f_poDescriptor->oSourceId, &oXmrSourceID, &oXmrRestrictedSourceID ) );

    ChkDR( SetOPLs(
        &oXmrBuilder,
         f_poDescriptor->wCompressedDigitalVideo,
         f_poDescriptor->wUncompressedDigitalVideo,
         f_poDescriptor->wAnalogVideo,
         f_poDescriptor->wCompressedDigitalAudio,
         f_poDescriptor->wUncompressedDigitalAudio,
        &oXmrMinOPL ) );

    ChkDR( SetPlayEnabler(
        &oXmrBuilder,
         f_poDescriptor->cPlayEnablers,
         f_poDescriptor->rgoPlayEnablers ) );

    ChkDR( SetExplicitOutputProtection(
        &oXmrBuilder,
         XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER,
         f_poDescriptor->cExplicitAnalogVideoOutputProtections,
         f_poDescriptor->rgoExplicitAnalogVideoOutputProtections,
        &oXmrExplicitAV ) );

    ChkDR( SetExplicitOutputProtection(
        &oXmrBuilder,
         XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER,
         f_poDescriptor->cExplicitDigitalAudioOoutputProtections,
         f_poDescriptor->rgoExplicitDigitalAudioOutputProtections,
        &oXmrExplicitDA ) );

    ChkDR( _FindAndCleanLicenseKeyCache(
        f_poAppContext,
       &poLocalLicenseSession->kid,
       &poLicenseKeyCache ) );

    if( poLicenseKeyCache != NULL )
    {
        ChkDR( DRM_BBX_PRND_Tx_DecryptAESKeyPairWithAESECBUsingGenericKey(
            poLocalLicenseSession->poBBXContext,
            poLicenseKeyCache->fRoot ? poAppContextInternal->poLocalLicenseContext->hAESGenericKeyRoot : poAppContextInternal->poLocalLicenseContext->hAESGenericKeyContent,
            poLicenseKeyCache->rgbEncryptedAESKeyPair,
           &poLocalLicenseSession->handleCICK ) );

        DRMASSERT( poLocalLicenseSession->handleCICK != OEM_CRYPTO_HANDLE_INVALID );

        OEM_SECURE_MEMCPY(
            rgbCICKEncryptedWithPubkey,
            poLicenseKeyCache->rgbCICKEncryptedWithPubkey,
            SIZEOF( rgbCICKEncryptedWithPubkey ) );

        OEM_SECURE_MEMCPY(
            rgbCICKEncryptedWithROOTAESECBKey,
            poLicenseKeyCache->rgbCICKEncryptedWithROOTAESECBKey,
            SIZEOF( rgbCICKEncryptedWithROOTAESECBKey ) );
    }

    /* Generate Content Integrity and Encryption Key */
    if( f_eLicenseType == eDRM_LOCAL_LICENSE_LEAF )
    {
        DRM_LOCAL_LICENSE_SESSION_CONTEXT *phRootLicenseSession = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hRootLicense;
        DRM_BOOL                           fSessionStillValid   = FALSE;

        /* We will fail to create leaf license for Remote Bound Root if PRND session is not valid */
        if( phRootLicenseSession->eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT )
        {
            ChkBOOL( poAppContextInternal->poPrndTransmitterContext != NULL, DRM_E_DEVICE_NOT_REGISTERED );

            ChkDR( DRM_PRND_Transmitter_IsSessionStillValid(
                poAppContextInternal->poPrndTransmitterContext,
               &fSessionStillValid ) );

            if( !fSessionStillValid )
            {
                ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
            }
        }

        if( poLocalLicenseSession->handleCICK == OEM_CRYPTO_HANDLE_INVALID )
        {
            ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
                poLocalLicenseSession->poBBXContext,
                DRM_BBX_KEY_PAIR_TYPE_CICK,
                phRootLicenseSession->handleCICK,
                f_poDescriptor->wSecurityLevel,
                rgbCICKEncryptedWithROOTAESECBKey,
               &poLocalLicenseSession->handleCICK ) );
        }

        /* Compute checksum for uplinkx */
        ChkDR( DRM_BBX_PRND_Tx_CalculateChecksum(
            phRootLicenseSession->poBBXContext,
            phRootLicenseSession->handleCICK,
           &phRootLicenseSession->kid,
            rgbUplinkChecksum ) );

        ChkDR( SetUplinkKID(
            &oXmrBuilder,
             phRootLicenseSession->kid.rgb,
             rgbUplinkChecksum,
            &oXmrUplinkKid ) );

    }
    else
    {
        dr = DRM_RVK_CheckRevInfoForExpiration(
            &poLocalLicenseSession->poBBXContext->CryptoContext,
            &poAppContextInternal->oRevContext,
            &g_guidRevocationTypeRevInfo2,
            poAppContextInternal->poPrndTransmitterContext != NULL ? &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.SessionID : NULL,
            NULL,
            &cbRevInfo );

        DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
        DRMASSERT( cbRevInfo > 0 );
        ChkMem( pbRevInfo = (DRM_BYTE*)Drm_Prnd_MemAlloc( cbRevInfo ) );

        ChkDR( DRM_RVK_CheckRevInfoForExpiration(
            &poLocalLicenseSession->poBBXContext->CryptoContext,
            &poAppContextInternal->oRevContext,
            &g_guidRevocationTypeRevInfo2,
            poAppContextInternal->poPrndTransmitterContext != NULL ? &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.SessionID : NULL,
            pbRevInfo,
            &cbRevInfo ) );

        if(    f_eLicenseType == eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
            || f_eLicenseType == eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE )
        {
            ChkDR( SetECCDeviceKey(
               &oXmrBuilder,
               SIZEOF( poLocalLicenseSession->poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext.PublicKey ),
               (DRM_BYTE*)&poLocalLicenseSession->poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext.PublicKey,
               &oXmrECCDeviceKey ) );

            if( poLocalLicenseSession->handleCICK == OEM_CRYPTO_HANDLE_INVALID )
            {
                ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
                    poLocalLicenseSession->poBBXContext,
                    fRoot ? DRM_BBX_KEY_PAIR_TYPE_RIRK : DRM_BBX_KEY_PAIR_TYPE_CICK,
                    NULL,
                    NULL,
                    0,
                    pbRevDataBuffer,
                    cbRevDataBuffer,
                    pbRevInfo,
                    cbRevInfo,
                    f_poDescriptor->wSecurityLevel,
                    (CIPHERTEXT_P256*)rgbCICKEncryptedWithPubkey,
                   &poLocalLicenseSession->handleCICK ) );
            }
        }
        else
        {
            ChkDR( SetECCDeviceKey(
               &oXmrBuilder,
               SIZEOF( oRemotePubKey.PublicKey ),
               (DRM_BYTE*)&oRemotePubKey.PublicKey,
               &oXmrECCDeviceKey ) );

            if( poLocalLicenseSession->handleCICK == OEM_CRYPTO_HANDLE_INVALID )
            {
                ChkDR( DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
                    poLocalLicenseSession->poBBXContext,
                    fRoot ? DRM_BBX_KEY_PAIR_TYPE_RIRK : DRM_BBX_KEY_PAIR_TYPE_CICK,
                   &oRemotePubKey,
                    pbRemoteCert,
                    cbRemoteCert,
                    pbRevDataBuffer,
                    cbRevDataBuffer,
                    pbRevInfo,
                    cbRevInfo,
                    f_poDescriptor->wSecurityLevel,
                    (CIPHERTEXT_P256*)rgbCICKEncryptedWithPubkey,
                   &poLocalLicenseSession->handleCICK ) );
            }
        }
    }

    if( poLicenseKeyCache == NULL )
    {
        ChkDR( _CreateLicenseCache(
            poLocalLicenseSession->poBBXContext,
           &poLocalLicenseSession->kid,
            fRoot ? poAppContextInternal->poLocalLicenseContext->hAESGenericKeyRoot : poAppContextInternal->poLocalLicenseContext->hAESGenericKeyContent,
            poLocalLicenseSession->handleCICK,
            rgbCICKEncryptedWithPubkey,
            rgbCICKEncryptedWithROOTAESECBKey,
           &poNewLicenseKeyCache ) );
        poNewLicenseKeyCache->fRoot = fRoot;

        poNewLicenseKeyCache->poNextLicenseCache = poAppContextInternal->poLocalLicenseContext->poLicenseKeyCache;
        poAppContextInternal->poLocalLicenseContext->poLicenseKeyCache = poNewLicenseKeyCache;
        poNewLicenseKeyCache = NULL;
    }

    if( f_eLicenseType == eDRM_LOCAL_LICENSE_LEAF )
    {
        ChkDR( SetContentKey(
            &oXmrBuilder,
             f_eLicenseType,
             poLocalLicenseSession->kid.rgb,
             SIZEOF( rgbCICKEncryptedWithROOTAESECBKey ),
             rgbCICKEncryptedWithROOTAESECBKey,
            &oXmrContentKey ) );
    }
    else
    {
        ChkDR( SetContentKey(
            &oXmrBuilder,
             f_eLicenseType,
             poLocalLicenseSession->kid.rgb,
             SIZEOF( rgbCICKEncryptedWithPubkey ),
             rgbCICKEncryptedWithPubkey,
            &oXmrContentKey ) );
    }

    /* Required license property */
    ChkDR( SetIssueDate( &oXmrBuilder, &oXmrIssueDate ) );
    ChkDR( SetRevInfo( &oXmrBuilder, poAppContextInternal->idCRLsCurrent.riv2, &oXmrRIV ) );
    ChkDR( SetEmbeddedBehavior( &oXmrBuilder, &oXmrEmbBehavior ) );

    if( oXmrExpirationAfterFirstPlay.fValid || oXmrExpiration.fValid )
    {
        /* We can't create a time-bound license on a system without a clock or on a system with a clock not set. */
        ChkBOOL( !_BlockTimeBoundLicenses( poAppContextInternal ),
            ( DRM_CLK_IsSecureClockSupported() || DRM_ARCLK_IsAntirollbackClockSupported() ) ? DRM_E_CLK_NOT_SET : DRM_E_CLK_NOT_SUPPORTED );
    }

    /* Setup license struct for callback */
    oLicenseData.poBlackBoxContext = &poAppContextInternal->oBlackBoxContext;
    oLicenseData.handleCI          = poLocalLicenseSession->handleCICK;

    poLocalLicenseSession->cbXMRLicense = 0;
    dr = DRM_XMR_FinishLicense(
        &oXmrBuilder,
        &oLicenseData,
         Local_License_SignLicense,
         NULL,
        &poLocalLicenseSession->cbXMRLicense );

    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );

    DRMASSERT( poLocalLicenseSession->cbXMRLicense > 0 );
    ChkMem( poLocalLicenseSession->pbXMRLicense = (DRM_BYTE *) Drm_Prnd_MemAlloc( poLocalLicenseSession->cbXMRLicense ) );

    ChkDR( DRM_XMR_FinishLicense(
        &oXmrBuilder,
        &oLicenseData,
         Local_License_SignLicense,
         poLocalLicenseSession->pbXMRLicense,
        &poLocalLicenseSession->cbXMRLicense ) );

    *f_phLicense           = ( DRM_LICENSE_HANDLE )poLocalLicenseSession;
     poLocalLicenseSession = NULL;

ErrorExit:
    (DRM_VOID)FreeExplicitOutputProtection( &oXmrExplicitAV );
    (DRM_VOID)FreeExplicitOutputProtection( &oXmrExplicitDA );
    (DRM_VOID)Drm_LocalLicense_Release( ( DRM_LICENSE_HANDLE *)&poLocalLicenseSession );
    DRM_PRND_SAFEMEMFREE( pbXMRBuilder );
    DRM_PRND_SAFEMEMFREE( pbRevInfo );
    if( poAppContextInternal != NULL )
    {
        (DRM_VOID)_CleanLocalLicenseContext( &poAppContextInternal->oBlackBoxContext, poLocalLicenseContext );

        DRM_BBX_CleanupPublicKeyContext( &poAppContextInternal->oBlackBoxContext,
                                         &oRemotePubKey );
    }
    DRM_PRND_SAFEMEMFREE( pbRevDataBuffer );
    TRACE(( "Drm_LocalLicense_CreateLicense Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreatePlayReadyObject(
    __in_opt                      const DRM_LICENSE_HANDLE                   f_hLicense,
    __in_opt                      const DRM_DWORD                            f_cbRMHeader,
    __in_opt                      const DRM_BYTE                            *f_pbRMHeader,
    __out                               DRM_DWORD                           *f_pcbPROBlob,
    __out_bcount( *f_pcbPROBlob )       DRM_BYTE                           **f_ppbPROBlob )
{
    DRM_RESULT                         dr                   = DRM_SUCCESS;

    DRM_DWORD                          cbPROBlob            = 0;
    DRM_BYTE                          *pbPROBlob            = NULL;
    DRM_BYTE                          *pbEmbeddedRecord     = NULL;
    DRM_DWORD                          cbEmbeddedRecord     = 0;
    DRM_DWORD                          cbInitEmbeddedRecord = 0;

    DRM_EST_CONTEXT                    oESTContextStore     = { 0 };
    DRM_DST                            oDSTContextStore     = { eDRM_DST_NONE, { 0 }, NULL };
    DRM_LICSTORE_CONTEXT               oLSTContextStore     = { 0 };

    DRM_LOCAL_LICENSE_SESSION_CONTEXT *pLicenseSession      = NULL;

    TRACE(( "Drm_LocalLicense_CreatePlayReadyObject Entered" ));

    ChkArg( f_pcbPROBlob != NULL );
    ChkArg( f_ppbPROBlob != NULL );

    if( f_hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        pLicenseSession = ( DRM_LOCAL_LICENSE_SESSION_CONTEXT* )f_hLicense;

        /* We should only embed remote or leaf licenses and not local bound license */
        ChkBOOL(    pLicenseSession->eLicenseType != eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
                 && pLicenseSession->eLicenseType != eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
                    DRM_E_LICGEN_EMBED_LOCAL_LICENSE );

        cbInitEmbeddedRecord = PADDED_LEN( pLicenseSession->cbXMRLicense ) + EXTRA_EST_LEN_FOR_A_LICENSE;
    }
    else
    {
        cbInitEmbeddedRecord = MAX_EMBEDDED_STORE_LEN;
    }

    *f_pcbPROBlob = 0;
    *f_ppbPROBlob = NULL;

    ChkDR( _CreatePRO(
         f_cbRMHeader,
         f_pbRMHeader,
        &cbPROBlob,
        &pbPROBlob,
         cbInitEmbeddedRecord ) );

    /* if no license handle is present, we just create a PRO with empty ELS */
    if( f_hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        ChkDR( DRM_PRO_GetRecord(
            pbPROBlob,
            cbPROBlob,
            PLAYREADY_EMBEDDED_LICENSE_STORE,
           &pbEmbeddedRecord,
           &cbEmbeddedRecord ) );

        ChkDR( DRM_DST_Init(
            eDRM_DST_EMBEDDED,
            ( DRM_DST_CONTEXT* )&oESTContextStore,
            SIZEOF( DRM_EST_CONTEXT ),
           &oDSTContextStore ) );

        ChkDR( DRM_DST_OpenStore(
            pLicenseSession->poBBXContext->pOEMContext,
            pbEmbeddedRecord,
            cbEmbeddedRecord,
            0,
           &oDSTContextStore ) );

        ChkDR( DRM_LST_Open(
           &oLSTContextStore,
           &oDSTContextStore,
            eDRM_LICENSE_STORE_XMR ) );

        ChkDR( _AddLicense(
           &oLSTContextStore,
            pLicenseSession ) );
    }

    *f_pcbPROBlob = cbPROBlob;
    *f_ppbPROBlob = pbPROBlob;
    pbPROBlob     = NULL;

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbPROBlob );
    (DRM_VOID) DRM_LST_Close(      &oLSTContextStore );
    (DRM_VOID) DRM_DST_CloseStore( &oDSTContextStore );
    (DRM_VOID) DRM_DST_Uninit(     &oDSTContextStore );

    TRACE(( "Drm_LocalLicense_CreatePlayReadyObject Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_CreateRMHeader(
    __in_opt                      const DRM_KID                             *f_pKeyId,
    __out                               DRM_DWORD                           *f_pcbRMHeader,
    __out_bcount( *f_pcbRMHeader )      DRM_BYTE                           **f_ppbRMHeader )
{
    DRM_RESULT         dr = DRM_SUCCESS;

    TRACE(( "Drm_LocalLicense_CreateRMHeader Entered" ));

    ChkArg( f_pcbRMHeader != NULL );
    ChkArg( f_ppbRMHeader != NULL );

    *f_pcbRMHeader = 0;
    *f_ppbRMHeader = NULL;

    ChkDR( _CreateRMHeader( f_pKeyId,
                            f_pcbRMHeader,
                            f_ppbRMHeader ) );
ErrorExit:
    TRACE(( "Drm_LocalLicense_CreateRMHeader Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_StoreLicense(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __in                          const DRM_LOCAL_LICENSE_STORE              f_eLicenseStore )
{
    DRM_RESULT                         dr                    = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT *poLocalLicenseContext = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense;

    TRACE(( "Drm_LocalLicense_StoreLicense Entered" ));

    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );

    if( f_eLicenseStore == eDRM_LOCAL_LICENSE_PRND_SESSION_STORE )
    {
        DRM_BOOL                      fSessionStillValid       = FALSE;
        DRM_PRND_TRANSMITTER_CONTEXT *poPrndTransmitterContext = NULL;

        ChkArg( (DRM_APP_CONTEXT_INTERNAL *)poLocalLicenseContext->poAppContext != NULL );

        poPrndTransmitterContext = ((DRM_APP_CONTEXT_INTERNAL *)poLocalLicenseContext->poAppContext)->poPrndTransmitterContext;

        ChkArg( poPrndTransmitterContext            != NULL );
        ChkArg( poLocalLicenseContext->eLicenseType != eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT );
        ChkArg( poLocalLicenseContext->eLicenseType != eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE );

        ChkDR( DRM_PRND_Transmitter_IsSessionStillValid(
            poPrndTransmitterContext,
           &fSessionStillValid ) );

        if( !fSessionStillValid )
        {
            ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
        }

        ChkDR( _AddLicense(
            &poPrndTransmitterContext->oPrndTransmitterSessionContext.oLSTContextPRNDStore,
             poLocalLicenseContext ) );
    }
    else if( f_eLicenseStore == eDRM_LOCAL_LICENSE_XMR_STORE )
    {
        ChkArg( poLocalLicenseContext->eLicenseType != eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT );
        ChkArg( poLocalLicenseContext->eLicenseType != eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE );
        ChkBOOL( !poLocalLicenseContext->fCannotPersist, DRM_E_LICGEN_CANNOT_PERSIST_LICENSE );

        ChkDR( _AddLicense(
           &((DRM_APP_CONTEXT_INTERNAL *)poLocalLicenseContext->poAppContext)->oLicStoreContextXMR,
            poLocalLicenseContext ) );
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    TRACE(( "Drm_LocalLicense_StoreLicense Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_EncryptSample(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __in                          const DRM_DWORD                            f_cSubsamples,
    __in_ecount( f_cSubsamples )  const DRM_DWORD                           *f_pcbSubamples,
    __inout_ecount( f_cSubsamples )     DRM_BYTE                           **f_ppbSubamples,
    __out                               DRM_UINT64                          *f_pqwIV )
{
    DRM_RESULT                         dr                = DRM_SUCCESS;
    DRM_DWORD                          iSample           = 0;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT *poLicenseSession  = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense;
    DRM_UINT64                         qwTempIV          = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64                        *pqwIV             = &qwTempIV;

    TRACE(( "Drm_LocalLicense_EncryptSample Entered" ));

    ChkArg( f_cSubsamples   > 0    );
    ChkArg( f_pcbSubamples != NULL );
    ChkArg( f_ppbSubamples != NULL );
    ChkArg( f_pqwIV        != NULL );
    ChkArg( f_hLicense     != DRM_LICENSE_HANDLE_INVALID );

    ChkBOOL(    poLicenseSession->eLicenseType != eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
             && poLicenseSession->eLicenseType != eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT,
                DRM_E_LICGEN_ROOT_LICENSE_CANNOT_ENCRYPT );

    ChkDR( _FindAndCleanLicenseKeyCache(
        poLicenseSession->poAppContext,
       &poLicenseSession->kid,
        NULL ) );

    for( iSample = 0; iSample < f_cSubsamples; iSample++ )
    {
        ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
            poLicenseSession->poBBXContext,
            poLicenseSession->handleCICK,
           &poLicenseSession->handleEncryption,
            pqwIV,
            NULL,
            OEM_OPAQUE_BUFFER_HANDLE_INVALID,
            OEM_OPAQUE_BUFFER_HANDLE_INVALID,
            f_ppbSubamples[iSample],
            f_pcbSubamples[iSample] ) );

        /*
        ** After the first loop, set the IV parameter to NULL, so that
        ** the same IV is used for remaining samples
        */
        pqwIV = NULL;
    }

    /*
    ** Success - copy the IV to the output parameter
    */
    *f_pqwIV = qwTempIV;

ErrorExit:
    TRACE(( "Drm_LocalLicense_EncryptSample Exited {dr = 0x%x}", dr ));
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_EncryptOpaqueSample(
    __in                          const DRM_LICENSE_HANDLE               f_hLicense,
    __out_ecount_opt(1)                 OEM_HAL_SAMPLE_METADATA         *f_pMetadata,
    __in                                OEM_OPAQUE_BUFFER_HANDLE         f_hInBuf,
    __in                                OEM_OPAQUE_BUFFER_HANDLE         f_hOutBuf,
    __in                                DRM_DWORD                        f_cbData,
    __out                               DRM_UINT64                      *f_pqwIV )
{
    DRM_RESULT                         dr                = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT *poLicenseSession  = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense;
    DRM_UINT64                         qwTempIV          = DRM_UI64LITERAL( 0, 0 );

    TRACE(( "Drm_LocalLicense_EncryptOpaqueSample Entered" ));

    ChkArg( f_hInBuf   != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkArg( f_hOutBuf  != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkArg( f_pqwIV    != NULL );
    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );

    ChkBOOL(    poLicenseSession->eLicenseType != eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
             && poLicenseSession->eLicenseType != eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT,
                DRM_E_LICGEN_ROOT_LICENSE_CANNOT_ENCRYPT );

    ChkDR( _FindAndCleanLicenseKeyCache(
        poLicenseSession->poAppContext,
       &poLicenseSession->kid,
        NULL ) );

    ChkDR( DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
           poLicenseSession->poBBXContext,
           poLicenseSession->handleCICK,
           &poLicenseSession->handleEncryption,
           &qwTempIV,
           f_pMetadata,
           f_hInBuf,
           f_hOutBuf,
           NULL,
           f_cbData ) );

    /*
    ** Success - copy the IV to the output parameter
    */
    *f_pqwIV = qwTempIV;

ErrorExit:
    TRACE(( "Drm_LocalLicense_EncryptSample Exited {dr = 0x%x}", dr ));
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_AddRef(
    __inout                             DRM_LICENSE_HANDLE                   f_hLicense )
{
    DRM_RESULT dr = DRM_SUCCESS;

    TRACE(( "Drm_LocalLicense_AddRef Entered" ));

    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );

    ((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense)->dwRefCount++;

ErrorExit:
    TRACE(( "Drm_LocalLicense_AddRef Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_GetKID(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __out                               DRM_KID                             *f_pKeyID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    TRACE(( "Drm_LocalLicense_GetKID Entered" ));

    ChkArg( f_hLicense != DRM_LICENSE_HANDLE_INVALID );
    ChkArg( f_pKeyID   != NULL );

    OEM_SECURE_MEMCPY( f_pKeyID, &((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense)->kid, SIZEOF( DRM_KID ) );

ErrorExit:
    TRACE(( "Drm_LocalLicense_GetKID Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_GetXMRLicense(
    __in                          const DRM_LICENSE_HANDLE                   f_hLicense,
    __out                               DRM_DWORD                           *f_pcbXMRLicense,
    __out                               DRM_BYTE                           **f_ppbXMRLicense )
{
    DRM_RESULT                               dr                = DRM_SUCCESS;
    DRM_BYTE                                *pbXMRLicense      = NULL;

    TRACE(( "Drm_LocalLicense_GetXMRLicense Entered" ));

    ChkArg( f_hLicense      != DRM_LICENSE_HANDLE_INVALID );
    ChkArg( f_pcbXMRLicense != NULL );
    ChkArg( f_pcbXMRLicense != NULL );

    ChkMem( pbXMRLicense = (DRM_BYTE*)Drm_Prnd_MemAlloc( ((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense)->cbXMRLicense ) );
    OEM_SECURE_MEMCPY(
        pbXMRLicense,
        ((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense)->pbXMRLicense,
        ((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense)->cbXMRLicense );

    *f_ppbXMRLicense = pbXMRLicense;
    *f_pcbXMRLicense = ((DRM_LOCAL_LICENSE_SESSION_CONTEXT *)f_hLicense)->cbXMRLicense;
    pbXMRLicense     = NULL;

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbXMRLicense );
    TRACE(( "Drm_LocalLicense_GetXMRLicense Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Drm_LocalLicense_Release(
    __inout                             DRM_LICENSE_HANDLE                  *f_phLicense )
{
    DRM_RESULT                         dr                = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_SESSION_CONTEXT *poLicenseSession  = NULL;

    TRACE(( "Drm_LocalLicense_Release Entered" ));

    ChkArg( f_phLicense != NULL );

    if( *f_phLicense == DRM_LICENSE_HANDLE_INVALID )
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    poLicenseSession  = (DRM_LOCAL_LICENSE_SESSION_CONTEXT *)*f_phLicense;

    poLicenseSession->dwRefCount--;

    ChkDR( _FindAndCleanLicenseKeyCache(
        poLicenseSession->poAppContext,
       &poLicenseSession->kid,
        NULL ) );

    if( poLicenseSession->dwRefCount == 0 )
    {
        DRM_BYTE                             *pbLicense                  = (DRM_BYTE*)poLicenseSession;

        DRM_PRND_SAFEMEMFREE( poLicenseSession->pbXMRLicense );
        poLicenseSession->pbXMRLicense = NULL;
        poLicenseSession->cbXMRLicense  = NULL;

        DRM_BBX_SafeDestroySampleEncryptionContext( &poLicenseSession->handleEncryption );
        DRM_BBX_SafeReleaseKey( poLicenseSession->poBBXContext, &poLicenseSession->handleCICK );

        /* DRM_PRND_SAFEMEMFREE requires BYTE* */
        DRM_PRND_SAFEMEMFREE( pbLicense );

        *f_phLicense = DRM_LICENSE_HANDLE_INVALID;
    }

ErrorExit:
    TRACE(( "Drm_LocalLicense_Release Exited {dr = 0x%x}", dr ));
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

