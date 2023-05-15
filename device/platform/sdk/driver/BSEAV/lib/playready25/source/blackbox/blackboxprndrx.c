/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXPRNDRX_C
#include <blackboxinternal.h>
#include <blackboxhal.h>
#include <drmblackbox.h>
#include <drmprnd.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
** Algorithm:
**  1. AES OMAC Sign the given data buffer using the key referenced by the provided handle.
**  2. Return the result from #1.
** Usage Scenarios:
**  1. Receiver: Sign PRND messages for integrity.
** Notes:
**  f_handleAESIntegrityKey AES key must be MI.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Rx_SignWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE        f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                *f_pbData,
    __in                                    DRM_DWORD                f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )        DRM_BYTE                 f_rgbSignature[DRM_AES_BLOCKLEN] )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cbSignature = DRM_AES_BLOCKLEN;

    ChkBOOL( DRM_PRND_IsPRNDReceiverSupported(), DRM_E_NOTIMPL );

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
**  1. AES OMAC Verify the given signature over the given data buffer using the key referenced by the provided handle.
**  2. Return success if the signature was verified and an error code otherwise.
** Usage Scenarios:
**  1. Receiver: Verify PRND messages for integrity. 
** Notes:
**  f_handleAESIntegrityKey AES key must be MI
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Rx_VerifySignatureWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in_ecount( 1 )                        OEM_CRYPTO_HANDLE        f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                *f_pbData,
    __in                                    DRM_DWORD                f_cbData,
    __in_ecount( DRM_AES_BLOCKLEN )   const DRM_BYTE                 f_rgbSignature[DRM_AES_BLOCKLEN] )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_PRND_IsPRNDReceiverSupported(), DRM_E_NOTIMPL );

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
**  1. ECC P256 Decrypt the given AES Key pair using this machine's ECC 256 private key.
**  2. Return a handle to decrypted generated AES key pair from #1 for use in future methods.
** Usage Scenarios:
**  1. Receiver: Storing of a key pair received from transmitter that's later used for PRND Message Integrity and Message Encryption (MI/MK).
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Rx_ECCDecryptAESKeyPair(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in                                    DRM_BBX_KEY_PAIR_TYPE    f_eKeyPairType,
    __in_ecount( 1 )                  const CIPHERTEXT_P256         *f_poEncryptedAESKeyPair,
    __out_ecount( 1 )                       OEM_CRYPTO_HANDLE       *f_phandleAESKeyPair )
{
    DRM_RESULT             dr                 = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE      hPrivKey           = OEM_CRYPTO_HANDLE_INVALID;

    ChkBOOL( DRM_PRND_IsPRNDReceiverSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext              != NULL );
    ChkArg( f_poEncryptedAESKeyPair     != NULL );
    ChkArg( f_phandleAESKeyPair         != NULL );
    ChkArg( *f_phandleAESKeyPair        == OEM_CRYPTO_HANDLE_INVALID );

    ChkArg( f_eKeyPairType == DRM_BBX_KEY_PAIR_TYPE_MIMK ); /* Only support ECC decrypting message encryption keys - no need to support license keys */

    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == DRM_AES_KEYSIZE_128 * 2 );

    /* ECC P256 decrypt the AES keypair with our private encryption key */
    ChkDR( _GetDeviceECC256KeyPair(
        f_poBBXContext,
        NULL,
        NULL,
        &hPrivKey,
        DRM_BINARY_DEVICE_CERT_KEY_PRND_ENCRYPT ) );

    ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,
                           hPrivKey,
                           ( const DRM_BYTE* )f_poEncryptedAESKeyPair,
                           SIZEOF( CIPHERTEXT_P256 ),
                           NULL,
                           0,
                           ( const DRM_BYTE* )f_poEncryptedAESKeyPair,
                           SIZEOF( CIPHERTEXT_P256 ),
                           f_phandleAESKeyPair ) );

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );

    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
/*
** Algorithm:
**  1. AES ECB Encrypt the given Nonce in place using the key referenced by the provided handle.
** Usage Scenarios:
**  1. Receiver: Processing of Nonce for proximity detection.
** Notes:
**  f_handleAESMessageEncryptionKey AES key must be MK.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Rx_EncryptNonceWithAESECB(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESMessageEncryptionKey,
    __inout_ecount( 1 )                     DRM_ID                          *f_pNonce )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_ID     encryptedNonce   = {0};
    DRM_DWORD  cbEncryptedNonce = SIZEOF( DRM_ID );

    ChkBOOL( DRM_PRND_IsPRNDReceiverSupported(), DRM_E_NOTIMPL );

    ChkArg( f_poBBXContext  != NULL );
    ChkArg( f_pNonce        != NULL );

    DRMCASSERT( SIZEOF( DRM_ID ) == SIZEOF( DRM_GUID ) );
    DRMCASSERT( SIZEOF( DRM_ID ) == DRM_AES_BLOCKLEN );

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

