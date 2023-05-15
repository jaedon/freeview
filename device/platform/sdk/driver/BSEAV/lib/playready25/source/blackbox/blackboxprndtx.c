/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXPRNDTX_C
#include <blackboxinternal.h>
#include <blackboxhal.h>
#include <drmblackbox.h>
#include <drmprnd.h>
#include <drmconstants.h>
#include <drmlastinclude.h>
#include <blackboxhal.h>
#include <blackboxdevcerts.h>

ENTER_PK_NAMESPACE_CODE;

/*
** Algorithm:
**  1. Generate a random AES key pair that is encryptable with ECC P256.
**  2. ECC P256 Encrypt key pair from #1 with provided ECC P256 public key.
**     If no ECC P256 public key is provided, the key type MUST be CI/CK and the current blackbox public content encryption key is used.
**  3. Return a handle to newly generated AES key pair from #1 for use in future methods.
** Usage Scenarios:
**  1. Transmitter: Generation of a new key pair that's later used for PRND Message Integrity and Message Encryption.
**     This keypair is known as MI/MK for Message Integrity/Message Key.  MI is used in OMAC1 sign/verify while MK is used in AES CTR and AES ECB.
**  2. Transmitter: Generation of a new key pair that's later used for Content Integrity and Content Encryption or Root Integrity and Root Encryption.
**     This keypair is known as CI/CK for Content Integrity/Content Key.  CI is used in OMAC1 sign/verify while CK is used in AES CTR.
**     This keypair is known as RI/RK for Root Integrity/Root Key.  RI is used in OMAC1 sign/verify while RK is used in AES ECB.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_GenerateAESKeyPairAndECCEncrypt(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                              DRM_BBX_KEY_PAIR_TYPE            f_eKeyPairType,
    __in_opt                                    const DRM_PUBLIC_KEY_CONTEXT          *f_poECC256PubKeyTarget,
    __in_ecount_opt( f_cbCertificateTarget )    const DRM_BYTE                        *f_pbCertificateTarget,
    __in                                              DRM_DWORD                        f_cbCertificateTarget,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                        *f_pbRevInfo,
    __in                                              DRM_DWORD                        f_cbRevInfo,
    __in                                              DRM_DWORD                        f_dwRequestedSecurityLevel,
    __out_ecount( 1 )                                 CIPHERTEXT_P256                 *f_poEncryptedAESKeyPair,
    __out_ecount( 1 )                                 OEM_CRYPTO_HANDLE               *f_phandleAESKeyPair )
{
    DRM_RESULT        dr              = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hPublicKey      = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hAESKeyPair     = OEM_CRYPTO_HANDLE_INVALID;
    OEM_HAL_KEY_TYPE  eAESKeyPairType = OEM_HAL_KEY_TYPE_INVALID;
    DRM_DWORD         cdWrappedKey    = SIZEOF( CIPHERTEXT_P256 );

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext              != NULL );
    ChkArg( f_poEncryptedAESKeyPair     != NULL );
    ChkArg( f_phandleAESKeyPair         != NULL );
    ChkArg( ( f_poECC256PubKeyTarget   == NULL ) == ( f_cbCertificateTarget    == 0 ) );    /* Pubkey and cert must come together */
    ChkArg( ( f_pbCertificateTarget    == NULL ) == ( f_cbCertificateTarget    == 0 ) );
    ChkArg( ( f_pbCertificateTargetCRL == NULL ) == ( f_cbCertificateTargetCRL == 0 ) );
    ChkArg( ( f_pbRevInfo              == NULL ) == ( f_cbRevInfo              == 0 ) );

    /*
    ** In this software-only implementation, the certificate and CRL are ignored.
    ** In a hardware-implementation, f_poECC256PubKeyTarget will be ignored instead.
    */
    UNREFERENCED_PARAMETER( f_pbCertificateTarget );
    UNREFERENCED_PARAMETER( f_cbCertificateTarget );
    UNREFERENCED_PARAMETER( f_pbCertificateTargetCRL );
    UNREFERENCED_PARAMETER( f_cbCertificateTargetCRL );
    UNREFERENCED_PARAMETER( f_pbRevInfo );
    UNREFERENCED_PARAMETER( f_cbRevInfo );

    if( f_poECC256PubKeyTarget == NULL )
    {
        /* Require the type to be CI/CK or RI/RK and use the blackbox's current public encryption key. */
        ChkBOOL( f_eKeyPairType == DRM_BBX_KEY_PAIR_TYPE_CICK
              || f_eKeyPairType == DRM_BBX_KEY_PAIR_TYPE_RIRK, DRM_E_NOT_FOUND );
        f_poECC256PubKeyTarget = &f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext;
    }

    /*
    ** Ensure that the latest RIV and CRL data is loaded by the OEM HAL,
    ** and that the public key is validated against this
    */
    if( DRM_BBX_IsHalDevCertValidationSupported() )
    {
        ChkDR( _ValidatePublicKey( f_poBBXContext,
                                   f_poECC256PubKeyTarget,
                                   f_pbCertificateTarget,
                                   f_cbCertificateTarget,
                                   f_pbCertificateTargetCRL,
                                   f_cbCertificateTargetCRL,
                                   f_pbRevInfo,
                                   f_cbRevInfo ) );
    }

    /*
    ** Make sure we have a valid key handle
    */
    ChkArg( f_poECC256PubKeyTarget->hPublicKey != OEM_CRYPTO_HANDLE_INVALID );

    /*
    ** Map the key pair type to the key handle types required
    */
    switch( f_eKeyPairType )
    {
    case DRM_BBX_KEY_PAIR_TYPE_MIMK:
        eAESKeyPairType = OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT;
        break;

    case DRM_BBX_KEY_PAIR_TYPE_CICK:
        eAESKeyPairType = OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT;
        break;

    case DRM_BBX_KEY_PAIR_TYPE_RIRK:
        eAESKeyPairType = OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT;
        break;

    default:
        ChkDR( DRM_E_INVALIDARG );
        break;
    }

    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             eAESKeyPairType,
                             f_dwRequestedSecurityLevel,
                             &hAESKeyPair ) );

    ChkDR( _Hal_WrapKey( f_poBBXContext,
                         hAESKeyPair,
                         f_poECC256PubKeyTarget->hPublicKey,
                         ( DRM_BYTE* )f_poEncryptedAESKeyPair,
                         &cdWrappedKey ) );

    DRMASSERT( cdWrappedKey == SIZEOF( CIPHERTEXT_P256 ) );

    *f_phandleAESKeyPair = hAESKeyPair;
    hAESKeyPair          = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPublicKey );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hAESKeyPair );
    return dr;
}

/*
** Algorithm:
**  1. Generate a random AES key pair that is encryptable with AES ECB.
**  2. AES ECB Encrypt key pair from #1 using the key referenced by the provided handle.
**  3. Return a handle to newly generated AES key pair from #1 for use in future methods.
** Usage Scenarios:
**  1. Transmitter: Generation of a new key pair that's later used for Content Integrity and Content Encryption.
**     This keypair is known as CI/CK for Content Integrity/Content Key.  CI is used in OMAC1 sign/verify while CK is used in AES CTR.
** Notes:
**  f_handleAESRootEncryptionKey AES key must be RK.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                              DRM_BBX_KEY_PAIR_TYPE            f_eKeyPairType,
    __in_ecount( 1 )                            const OEM_CRYPTO_HANDLE                f_handleAESRootEncryptionKey,
    __in                                              DRM_DWORD                        f_dwRequestedSecurityLevel,
    __out_ecount( DRM_AES_BLOCKLEN * 2 )              DRM_BYTE                         f_rgbEncryptedAESKeyPair[DRM_AES_BLOCKLEN * 2],
    __out_ecount( 1 )                                 OEM_CRYPTO_HANDLE               *f_phandleAESKeyPair )
{
    DRM_RESULT        dr              = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hAESKeyPair     = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD         cdWrappedKey    = DRM_AES_BLOCKLEN * 2;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext              != NULL );
    ChkArg( f_rgbEncryptedAESKeyPair    != NULL );
    ChkArg( f_phandleAESKeyPair         != NULL );
    /* Only support ECB encrypting content keys (for leaf licenses) - no need to support message keys or root keys */
    ChkArg( f_eKeyPairType              == DRM_BBX_KEY_PAIR_TYPE_CICK );

    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT,
                             f_dwRequestedSecurityLevel,
                             &hAESKeyPair ) );

    ChkDR( _Hal_WrapKey( f_poBBXContext,
                         hAESKeyPair,
                         f_handleAESRootEncryptionKey,
                         f_rgbEncryptedAESKeyPair,
                         &cdWrappedKey ) );

    DRMASSERT( cdWrappedKey == DRM_AES_BLOCKLEN * 2 );

    *f_phandleAESKeyPair = hAESKeyPair;
    hAESKeyPair          = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hAESKeyPair );
    return dr;
}

/*
** Algorithm:
**  1. Generate a random AES key.
**  2. Return a handle to the newly generated AES key from #1 for use in future methods.
** Usage Scenarios:
**  1. Transmitter: Generation of a new generic AES key that's later used for key caching.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_GenerateAESGenericKey(
    __inout_ecount( 1 )                         DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                        OEM_HAL_KEY_TYPE                 f_eKeyType,
    __out_ecount( 1 )                           OEM_CRYPTO_HANDLE               *f_phandleAESGenericKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext         != NULL );
    ChkArg( f_phandleAESGenericKey != NULL );
    ChkArg( f_eKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT
         || f_eKeyType == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT );    /* Only support the two generic key types */

    ChkDR( _Hal_GenerateKey( f_poBBXContext,
                             f_eKeyType,
                             0,
                             f_phandleAESGenericKey ) );

ErrorExit:
    return dr;
}

/*
** Algorithm:
**  1. Encrypt the given CI/CK or RI/RK AES Key Pair using the given Generic Key with AES ECB.
** Usage Scenarios:
**  1. Transmitter: Cache CI/CK or RI/RK outside the blackbox.
**     This enables the CI/CK or RI/RK handles to be released (conserving blackbox resources).
**     The caller can then cache just f_rgbEncryptedAESKeyPair until needed again.
** Notes:
**  f_handleAESKeyPair must be CI+CK or RI+RK.
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: temporarily disable while two keys passed in")
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESGenericKey,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESKeyPair,
    __out_ecount( DRM_AES_BLOCKLEN * 2 )    DRM_BYTE                         f_rgbEncryptedAESKeyPair[DRM_AES_BLOCKLEN * 2] )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT           dr                                     = DRM_SUCCESS;
    DRM_DWORD            cbWrappedKeyBuffer = DRM_AES_BLOCKLEN * 2;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext            != NULL );
    ChkArg( f_rgbEncryptedAESKeyPair  != NULL );

    ChkDR( _Hal_WrapKey( f_poBBXContext,
                         f_handleAESKeyPair,
                         f_handleAESGenericKey,
                         f_rgbEncryptedAESKeyPair,
                         &cbWrappedKeyBuffer ) );

ErrorExit:
    return dr;
}

/*
** Algorithm:
**  1. Decrypt the given CI/CK or RI/RK AES Key Pair using the given Generic Key with AES ECB.
**  2. Return handle to the newly decrypted key pair from #1 for use in future methods.
** Usage Scenarios:
**  1. Transmitter: Cache CI/CK or RI/RK outside the blackbox.
**     This places CI/CK or RI/RK (previously cached outside the blackbox) back inside the blackbox.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_DecryptAESKeyPairWithAESECBUsingGenericKey(
    __inout_ecount( 1 )                         DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                      const OEM_CRYPTO_HANDLE                f_handleAESGenericKey,
    __in_ecount( DRM_AES_BLOCKLEN * 2 )   const DRM_BYTE                         f_rgbEncryptedAESKeyPair[DRM_AES_BLOCKLEN * 2],
    __out_ecount( 1 )                           OEM_CRYPTO_HANDLE               *f_phandleAESKeyPair )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE eUnwrappedKeyType = OEM_HAL_KEY_TYPE_INVALID;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext            != NULL );
    ChkArg( f_rgbEncryptedAESKeyPair  != NULL );
    ChkArg( f_phandleAESKeyPair       != NULL );

    switch( _Hal_GetKeyType( f_poBBXContext, f_handleAESGenericKey ) )
    {
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
        eUnwrappedKeyType = OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT;
        break;

    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
        eUnwrappedKeyType = OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT;
        break;

    default:
        /*
        ** Logic error - should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
        break;
    }

    ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                           eUnwrappedKeyType,
                           f_handleAESGenericKey,
                           f_rgbEncryptedAESKeyPair,
                           DRM_AES_BLOCKLEN * 2,
                           NULL,
                           0,
                           f_rgbEncryptedAESKeyPair,
                           DRM_AES_BLOCKLEN * 2,
                           f_phandleAESKeyPair ) );

ErrorExit:

    return dr;
}

/*
** Algorithm:
**  1. ECC P256 Decrypt the given (by f_pRootmostLicenseBindingInfo) root or standard license key pair using this machine's ECC 256 private key.
**  2. Return a handle to the integrity key portion of #1.  (This handle will later be used in a call to DRM_BBX_PRND_Tx_SignWithAESOMAC.)
**  3. ECC P256 Encrypt key pair from #1 with provided ECC P256 public key.
**  4. Return the result from #2.
** Usage Scenarios:
**  1. Transmitter: Rebind Transmitter's existing root or standard license key material to the Receiver (and return a handle required for signing the license).
** Notes:
**  This method is only required if the Transmitter will have existing PlayReady licenses to rebind.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_RebindLicenseKeysToNewECCKey(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                            const DRM_BINDING_INFO                *f_pRootmostLicenseBindingInfo,
    __in                                        const DRM_PUBLIC_KEY_CONTEXT          *f_poECC256PubKeyTarget,
    __in_ecount_opt( f_cbCertificateTarget )    const DRM_BYTE                        *f_pbCertificateTarget,
    __in                                              DRM_DWORD                        f_cbCertificateTarget,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                        *f_pbRevInfo,
    __in                                              DRM_DWORD                        f_cbRevInfo,
    __out_ecount( 1 )                                 OEM_CRYPTO_HANDLE               *f_phandleAESIntegrityKey,
    __out_ecount( 1 )                                 CIPHERTEXT_P256                 *f_poEncryptedAESKeyPair )
{
    DRM_RESULT        dr                 = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hPublicKey         = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hContentKey        = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD         cbWrappedKeyBuffer = SIZEOF( CIPHERTEXT_P256 );

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext                != NULL );
    ChkArg( f_pRootmostLicenseBindingInfo != NULL );
    ChkArg( f_poECC256PubKeyTarget        != NULL );
    ChkArg( f_phandleAESIntegrityKey      != NULL );
    ChkArg( f_poEncryptedAESKeyPair       != NULL );
    ChkArg( ( f_poECC256PubKeyTarget   == NULL ) == ( f_cbCertificateTarget    == 0 ) );    /* Pubkey and cert must come together */
    ChkArg( ( f_pbCertificateTarget    == NULL ) == ( f_cbCertificateTarget    == 0 ) );
    ChkArg( ( f_pbCertificateTargetCRL == NULL ) == ( f_cbCertificateTargetCRL == 0 ) );
    ChkArg( ( f_pbRevInfo              == NULL ) == ( f_cbRevInfo              == 0 ) );

    /*
    ** In this software-only implementation, the certificate and CRL are ignored.
    ** In a hardware-implementation, f_poECC256PubKeyTarget will be ignored instead.
    */
    UNREFERENCED_PARAMETER( f_pbCertificateTarget );
    UNREFERENCED_PARAMETER( f_cbCertificateTarget );
    UNREFERENCED_PARAMETER( f_pbCertificateTargetCRL );
    UNREFERENCED_PARAMETER( f_cbCertificateTargetCRL );
    UNREFERENCED_PARAMETER( f_pbRevInfo );
    UNREFERENCED_PARAMETER( f_cbRevInfo );

    /*
    ** Ensure that the latest RIV and CRL data is loaded by the OEM HAL,
    ** and that the public key is validated against this
    */
    if( DRM_BBX_IsHalDevCertValidationSupported() )
    {
        ChkDR( _ValidatePublicKey( f_poBBXContext,
                                   f_poECC256PubKeyTarget,
                                   f_pbCertificateTarget,
                                   f_cbCertificateTarget,
                                   f_pbCertificateTargetCRL,
                                   f_cbCertificateTargetCRL,
                                   f_pbRevInfo,
                                   f_cbRevInfo ) );
    }

    ChkDR( _DecryptContentKeyXMR(
        f_pRootmostLicenseBindingInfo,
        1,      /* Single (rootmost) binding info means chain depth is 1 for this function even if this is a root license. */
        f_poBBXContext,
        TRUE,   /* Ignore checksum */
        &hContentKey,
        NULL ) );

    ChkDR( _Hal_WrapKey( f_poBBXContext,
                         hContentKey,
                         f_poECC256PubKeyTarget->hPublicKey,
                         ( DRM_BYTE* )f_poEncryptedAESKeyPair,
                         &cbWrappedKeyBuffer ) );

    *f_phandleAESIntegrityKey = hContentKey;
    hContentKey               = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hContentKey );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPublicKey );
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
/*
** Algorithm:
**  1. AES CTR Encrypt the given buffer in-place using the key referenced by the provided handle and using AES parameters specified.
** Usage Scenarios:
**  1. Transmitter: Encrypt clear content with content key from LEAF or STANDARD license.
** Notes:
**  f_handleAESContentEncryptionKey AES key must be CK.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESContentEncryptionKey,
    __inout_ecount( 1 )                     OEM_ENCRYPTION_HANDLE           *f_phandleEncryption,
    __out                                   DRM_UINT64                      *f_pqwInitializationVector,
    __out_ecount_opt(1)                     OEM_HAL_SAMPLE_METADATA         *f_pMetadata,
    __in_opt                                OEM_OPAQUE_BUFFER_HANDLE         f_hInBuf,
    __in_opt                                OEM_OPAQUE_BUFFER_HANDLE         f_hOutBuf,
    __inout_bcount_opt( f_cbData )          DRM_BYTE                        *f_pbData,
    __in                                    DRM_DWORD                        f_cbData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT             dr                = DRM_SUCCESS;
    OEM_ENCRYPTION_HANDLE  handleEncryption  = OEM_ENCRYPTION_HANDLE_INVALID;
    OEM_ENCRYPTION_HANDLE *pHandleEncryption = &handleEncryption;
    OEM_HAL_KEY_TYPE       eType             = OEM_HAL_KEY_TYPE_UNDEFINED;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext      != NULL );
    ChkArg( f_phandleEncryption != NULL );
    ChkArg( ( f_hInBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) != ( f_pbData == NULL ) );
    ChkArg( ( f_hInBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) == ( f_hOutBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) );

    eType = _Hal_GetKeyType( f_poBBXContext, f_handleAESContentEncryptionKey );
    ChkArg( eType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT );

    /*
    ** If this is the first sample to be encrypted with this license, initialize the 
    ** encryption context
    */
    if( *f_phandleEncryption == OEM_ENCRYPTION_HANDLE_INVALID )
    {
        ChkDR( _Hal_CreateSampleEncryptionContext( f_poBBXContext,
                                                   f_handleAESContentEncryptionKey,
                                                   OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL,
                                                   &handleEncryption ) );
    }
    else
    {
        pHandleEncryption = f_phandleEncryption;
    }

    ChkDR( _Hal_EncryptSampleData( f_poBBXContext->pOEMContext,
                                   *pHandleEncryption,
                                   f_pqwInitializationVector,
                                   f_pMetadata,
                                   f_hInBuf,
                                   f_hOutBuf,
                                   f_cbData,
                                   f_pbData ) );

    if( *f_phandleEncryption == OEM_ENCRYPTION_HANDLE_INVALID )
    {
        *f_phandleEncryption = handleEncryption;
        handleEncryption     = OEM_ENCRYPTION_HANDLE_INVALID;
    }

ErrorExit:

    /*
    ** release the encryption handle if it was allocated and then the operation failed
    */
    DRM_BBX_SafeDestroySampleEncryptionContext( &handleEncryption );

    return dr;
}

/*
** Algorithm:
**  1. AES OMAC Sign the given data buffer using the key referenced by the provided handle.
**  2. Return the result from #1.
** Usage Scenarios:
**  1. Transmitter: Sign PRND messages for integrity.  Sign PRND licenses for integrity.  Sign timestamps for integrity.
** Notes:
**  f_handleAESIntegrityKey AES key must be MI or CI.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_SignWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE        f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                *f_pbData,
    __in                                    DRM_DWORD                f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )        DRM_BYTE                 f_rgbSignature[DRM_AES_BLOCKLEN] )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cbSignature = DRM_AES_BLOCKLEN;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext  != NULL );
    ChkArg( f_pbData        != NULL );
    ChkArg( f_cbData         > 0    );
    ChkArg( f_rgbSignature  != NULL );

    ChkDR( _Hal_CreateOMAC1Signature( f_poBBXContext,
                                      f_pbData,
                                      f_cbData,
                                      f_rgbSignature,
                                      &cbSignature,
                                      f_handleAESIntegrityKey ) );

ErrorExit:
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
/*
** Algorithm:
**  1. AES ECB encrypt the given KID using the key referenced by the provided handle and truncate to DRM_AES_CK_CHECKSUM_LENGTH (8) bytes.
**  2. Return the result from #1.
** Usage Scenarios:
**  1. Transmitter: Calculate leaf license checksum during local license generation.
** Notes:
**  f_handleAESContentEncryptionKey AES key must be RK.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_CalculateChecksum(
    __inout_ecount( 1 )                         DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                      const OEM_CRYPTO_HANDLE                f_handleAESContentEncryptionKey,
    __in                                  const DRM_KID                         *f_pKID,
    __out_ecount( DRM_AES_CK_CHECKSUM_LENGTH )  DRM_BYTE                         f_rgbChecksum[ DRM_AES_CK_CHECKSUM_LENGTH ] )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT           dr                           = DRM_SUCCESS;
    DRM_BYTE             rgbBlock[SIZEOF( DRM_KID ) ] = { 0 };
    DRM_DWORD            cbBlock = SIZEOF( DRM_KID );

    DRMCASSERT( DRM_AES_CK_CHECKSUM_LENGTH < SIZEOF( DRM_KID ) );
    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext  != NULL );
    ChkArg( f_pKID          != NULL );
    ChkArg( f_rgbChecksum   != NULL );

    ChkArg( _Hal_GetKeyType( f_poBBXContext, f_handleAESContentEncryptionKey ) 
            == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT );

    ChkDR( _Hal_EncryptDataBlock( f_poBBXContext,
                                  f_handleAESContentEncryptionKey,
                                  f_pKID->rgb,
                                  SIZEOF( DRM_KID ),
                                  rgbBlock,
                                  &cbBlock ) );

    OEM_SECURE_MEMCPY( f_rgbChecksum, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH );

ErrorExit:
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
/*
** Algorithm:
**  1. AES OMAC Verify the given signature over the given data buffer using the key referenced by the provided handle.
**  2. Return success if the signature was verified and an error code otherwise.
** Usage Scenarios:
**  1. Transmitter: Verify PRND messages for integrity.  Verify timestamps for integrity.
** Notes:
**  f_handleAESIntegrityKey AES key must be MI.
*/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_VerifySignatureWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in_ecount( 1 )                        OEM_CRYPTO_HANDLE        f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                *f_pbData,
    __in                                    DRM_DWORD                f_cbData,
    __in_ecount( DRM_AES_BLOCKLEN )   const DRM_BYTE                 f_rgbSignature[DRM_AES_BLOCKLEN] )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext  != NULL );
    ChkArg( f_pbData        != NULL );
    ChkArg( f_cbData         > 0    );
    ChkArg( f_rgbSignature  != NULL );

    ChkDR( _Hal_VerifyOMAC1Signature( f_poBBXContext,
                                      f_pbData,
                                      f_cbData,
                                      f_rgbSignature,
                                      DRM_AES_BLOCKLEN,
                                      f_handleAESIntegrityKey ) );

ErrorExit:
    return dr;
}

/*
** Algorithm:
**  1. AES ECB Encrypt the given Nonce in place using the key referenced by the provided handle.
** Usage Scenarios:
**  1. Transmitter: Verification of Nonce for proximity detection.
** Notes:
**  f_handleAESMessageEncryptionKey AES key must be MK.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_EncryptNonceWithAESECB(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESMessageEncryptionKey,
    __inout_ecount( 1 )                     DRM_ID                          *f_pNonce )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_ID     encryptedNonce   = {0};
    DRM_DWORD  cbEncryptedNonce = SIZEOF( DRM_ID );

    ChkBOOL( DRM_PRND_IsPRNDTransmitterSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext  != NULL );
    ChkArg( f_pNonce        != NULL );

    DRMCASSERT( SIZEOF( DRM_ID ) == SIZEOF( DRM_GUID ) );
    DRMCASSERT( SIZEOF( DRM_ID ) == DRM_AES_BLOCKLEN );

    ChkArg( _Hal_GetKeyType( f_poBBXContext, f_handleAESMessageEncryptionKey ) 
            == OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT );

    ChkDR( _Hal_EncryptDataBlock( f_poBBXContext,
                                  f_handleAESMessageEncryptionKey,
                                  (DRM_BYTE*)f_pNonce,
                                  SIZEOF( DRM_ID ),
                                  (DRM_BYTE*)&encryptedNonce,
                                  &cbEncryptedNonce ) );

    DRMASSERT( cbEncryptedNonce == SIZEOF( DRM_ID ) );

    OEM_SECURE_MEMCPY( (DRM_BYTE*)f_pNonce, 
                       (DRM_BYTE*)&encryptedNonce, 
                       SIZEOF( DRM_ID ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &encryptedNonce, SIZEOF( encryptedNonce ) );
    return dr;
}
EXIT_PK_NAMESPACE_CODE;

