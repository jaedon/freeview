/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oempkcrypto.h>
#include <oemeccp160.h>
#include <oemeccp256.h>
#include <oemrsaoaeppss.h>

ENTER_PK_NAMESPACE_CODE;


/*********************************************************************
**
**  Function:  DRM_PK_GenKeyPair
**
**  Synopsis:  Generate a matched public and private key pair for the given algorithm
**
**  Arguments:
**     [f_pContext]      : Pointer to pk crypto context. Not required ECC 256
**     [f_eAlgorithm]    : Enumeration specifying algorithm to generate keys for
**     [f_eRSAKeyLength] : Length of RSA key in bits. Required for RSA algorithm only.
**     [f_cbPublicKey]   : Specifies the length of the public key buffer.
**                         Returns the length of the generated public key.
**     [f_pbPublicKey]   : Returns the generated public key
**     [f_cbPrivateKey]  : Specifies the length of the private key buffer.
**                         Returns the length of the generated private key.
**     [f_pbPrivateKey]  : Returns the generated private key
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or improperly initialized
**      DRM_E_BUFFERTOOSMALL
**          One of the key buffers was too small for the specified algorithm
**      DRM_E_UNSUPPORTEDALGORITHM
**          f_eAlgorithm specifies an invalid or not supported algorithm
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PK_GenKeyPair(
    __in_opt                        DRM_VOID                              *f_pContext,
    __in                            DRM_PKCRYPTO_SUPPORTED_ALGORITHMS      f_eAlgorithm,
    __in                            DRM_RSA_SUPPORTED_KEY_LENGTHS          f_eRSAKeyLength,
    __inout_ecount( 1 )             DRM_DWORD                             *f_pcbPublicKey,
    __out_bcount( *f_pcbPublicKey ) DRM_BYTE                              *f_pbPublicKey,
    __inout_ecount( 1 )             DRM_DWORD                             *f_pcbPrivateKey,
    __out_bcount( *f_pcbPrivateKey )DRM_BYTE                              *f_pbPrivateKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pcbPublicKey  != NULL );
    ChkArg( f_pbPublicKey   != NULL );
    ChkArg( f_pcbPrivateKey != NULL );
    ChkArg( f_pbPrivateKey  != NULL );

    switch ( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        ChkBOOL( *f_pcbPublicKey  >= DRM_ECC160_PUBLIC_KEY_LEN
              && *f_pcbPrivateKey >= DRM_ECC160_PRIVATE_KEY_LEN,
                 DRM_E_BUFFERTOOSMALL );
        ChkDR( OEM_ECC_GenKeyPair_P160( f_pContext, ( PUBKEY * )f_pbPublicKey, ( PRIVKEY * )f_pbPrivateKey ) );
        *f_pcbPublicKey  = DRM_ECC160_PUBLIC_KEY_LEN;
        *f_pcbPrivateKey = DRM_ECC160_PRIVATE_KEY_LEN;
        break;
    case eDRM_ECC_P256:
        ChkBOOL( *f_pcbPublicKey  >= ECC_P256_POINT_SIZE_IN_BYTES
              && *f_pcbPrivateKey >= ECC_P256_INTEGER_SIZE_IN_BYTES,
                 DRM_E_BUFFERTOOSMALL );
        /*
        ** Always generate ECC-p256 encryptable keys: the 'restricted' set
        */
        ChkDR( OEM_ECC_GenKeyPairRestrictedPriv_P256( ( PUBKEY_P256 * )f_pbPublicKey,
                                                      ( PRIVKEY_P256 * )f_pbPrivateKey, (struct bigctx_t *)f_pContext ) );
        *f_pcbPublicKey  = ECC_P256_POINT_SIZE_IN_BYTES;
        *f_pcbPrivateKey = ECC_P256_INTEGER_SIZE_IN_BYTES;
        break;
    case eDRM_RSA:

        ChkArg( f_pContext != NULL );
        ChkArg( f_eRSAKeyLength == eDRM_RSA_1024 || f_eRSAKeyLength == eDRM_RSA_2048 );
        ChkBOOL( *f_pcbPublicKey  >= DRM_RSA_CB_PUBKEY , DRM_E_BUFFERTOOSMALL );
        ChkBOOL( *f_pcbPrivateKey >= DRM_RSA_CB_PRIVKEY, DRM_E_BUFFERTOOSMALL );
        ChkDR( OEM_RSA_GenerateKeyPair(  f_eRSAKeyLength,
                                        ( DRM_RSA_PRIVATE_KEY * )f_pbPrivateKey,
                                        ( DRM_RSA_PUBLIC_KEY * )f_pbPublicKey,
                                        ( DRM_CRYPTO_CONTEXT * )f_pContext ) );
        *f_pcbPublicKey  = DRM_RSA_CB_PUBKEY;
        *f_pcbPrivateKey = DRM_RSA_CB_PRIVKEY;
        break;
    default:
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

ErrorExit:

    return dr;
}


/*********************************************************************
**
**  Function:  DRM_PK_Encrypt
**
**  Synopsis:  Encrypt a block of data with a given public key, using the
**             specified algorithm.
**
**  Arguments:
**     [f_pOEMContext]   : Pointer to OEM-specific context (optional)
**     [f_pContext]      : Pointer to pk crypto context. Not required for ECC 256
**     [f_eAlgorithm]    : Enumeration specifying algorithm to use for encryption
**     [f_cbPublicKey]   : Byte count of the public key
**     [f_pbPublicKey]   : Specifies the public key to use
**     [f_cbPlainText]   : Byte count of the plaintext buffer
**     [f_pbPlainText]   : Specifies the plaintext buffer
**     [f_pcbCipherText] : Byte count of the ciphertext buffer. Returns the length of the
**                         actual ciphertext.
**     [f_pbCipherText]  : Specifies the ciphertext buffer
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or improperly initialized
**      DRM_E_BUFFERTOOSMALL
**          One of the buffers was too small for the specified algorithm
**      DRM_E_UNSUPPORTEDALGORITHM
**          f_eAlgorithm specifies an invalid or not supported algorithm
**
**  Notes:  Data is not encrypted in place.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PK_Encrypt(
    __in_opt                           DRM_VOID                           *f_pOEMContext,
    __in_opt                           DRM_VOID                           *f_pContext,
    __in                               DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __in                               DRM_DWORD                           f_cbPublicKey,
    __in_bcount( f_cbPublicKey ) const DRM_BYTE                           *f_pbPublicKey,
    __in                               DRM_DWORD                           f_cbPlainText,
    __in_bcount( f_cbPlainText ) const DRM_BYTE                           *f_pbPlainText,
    __inout_ecount( 1 )                DRM_DWORD                          *f_pcbCipherText,
    __out_bcount( *f_pcbCipherText )   DRM_BYTE                           *f_pbCipherText )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbPublicKey   != NULL );
    ChkArg( f_pbPlainText   != NULL );
    ChkArg( f_pcbCipherText != NULL );
    ChkArg( f_pbCipherText  != NULL );

    switch ( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        ChkArg( f_cbPublicKey  == DRM_ECC160_PUBLIC_KEY_LEN );

        ChkBOOL( *f_pcbCipherText >= DRM_ECC160_CIPHERTEXT_LEN, DRM_E_BUFFERTOOSMALL );
        ChkBOOL(  f_cbPlainText   >= DRM_ECC160_PLAINTEXT_LEN, DRM_E_BUFFERTOOSMALL );

        ChkDR( OEM_ECC_Encrypt_P160( f_pContext,
                                     ( const PUBKEY * )f_pbPublicKey,
                                     f_pbPlainText,
                                     f_pbCipherText ) );

        *f_pcbCipherText = DRM_ECC160_CIPHERTEXT_LEN;
        break;
    case eDRM_ECC_P256:
        ChkArg( f_cbPublicKey == ECC_P256_POINT_SIZE_IN_BYTES );

        ChkBOOL( *f_pcbCipherText >= ECC_P256_CIPHERTEXT_SIZE_IN_BYTES, DRM_E_BUFFERTOOSMALL );
        ChkBOOL(  f_cbPlainText   >= ECC_P256_PLAINTEXT_SIZE_IN_BYTES,  DRM_E_BUFFERTOOSMALL );

        ChkDR( OEM_ECC_Encrypt_P256( ( const PUBKEY_P256 * )f_pbPublicKey,
                                     ( const PLAINTEXT_P256 * )f_pbPlainText,
                                     ( CIPHERTEXT_P256 * )f_pbCipherText,
                                     (struct bigctx_t *)f_pContext )  );

        *f_pcbCipherText = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;
        break;
    case eDRM_RSA:
        ChkArg( f_cbPublicKey == SIZEOF( DRM_RSA_PUBLIC_KEY ) );

        ChkDR( OEM_RSA_OaepEncrypt(                               f_pOEMContext,
                                    ( const DRM_RSA_PUBLIC_KEY * )f_pbPublicKey,
                                                                  f_cbPlainText,
                                                                  f_pbPlainText,
                                                                  f_pcbCipherText,
                                                                  f_pbCipherText,
                                           (DRM_CRYPTO_CONTEXT*)  f_pContext ) );
        break;
    default:
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }


ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_PK_Decrypt
**
**  Synopsis:  Decrypt a buffer using the given private key and algorithm type
**
**  Arguments:
**     [f_pContext]      : Pointer to pk crypto context. Not required for ECC 256
**     [f_eAlgorithm]    : Enumeration specifying algorithm to use for decryption
**     [f_cbPrivateKey]  : Byte count of the private key
**     [f_pbPrivateKey]  : Specifies the private key to use
**     [f_cbCipherText]  : Byte count of the ciphertext buffer.
**     [f_pbCipherText]  : Specifies the ciphertext buffer
**     [f_pcbPlainText]  : Byte count of the plaintext buffer. Returns the length of the
**                         actual plaintext.
**     [f_pbPlainText]   : Specifies the plaintext buffer
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or improperly initialized
**      DRM_E_BUFFERTOOSMALL
**          One of the buffers was too small for the specified algorithm
**      DRM_E_UNSUPPORTEDALGORITHM
**          f_eAlgorithm specifies an invalid or not supported algorithm
**
**  Notes:  Data is not decrypted in place.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PK_Decrypt(
    __in_opt                            DRM_VOID                           *f_pContext,
    __in                                DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __in                                DRM_DWORD                           f_cbPrivateKey,
    __in_bcount( f_cbPrivateKey ) const DRM_BYTE                           *f_pbPrivateKey,
    __in                                DRM_DWORD                           f_cbCipherText,
    __in_bcount( f_cbCipherText ) const DRM_BYTE                           *f_pbCipherText,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPlainText,
    __out_bcount( *f_pcbPlainText )     DRM_BYTE                           *f_pbPlainText )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbPrivateKey  != NULL );
    ChkArg( f_pbPlainText   != NULL );
    ChkArg( f_pcbPlainText  != NULL );
    ChkArg( f_pbCipherText  != NULL );

    switch ( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        ChkArg( f_cbPrivateKey == DRM_ECC160_PRIVATE_KEY_LEN );
        ChkArg( f_cbCipherText == DRM_ECC160_CIPHERTEXT_LEN );

        ChkBOOL( *f_pcbPlainText >= DRM_ECC160_PLAINTEXT_LEN, DRM_E_BUFFERTOOSMALL );

        ChkDR( OEM_ECC_Decrypt_P160( f_pContext,
                                     ( const PRIVKEY * )f_pbPrivateKey,
                                     f_pbCipherText,
                                     f_pbPlainText ) );

        *f_pcbPlainText = DRM_ECC160_PLAINTEXT_LEN;
        break;
    case eDRM_ECC_P256:
        ChkArg( f_cbPrivateKey == ECC_P256_INTEGER_SIZE_IN_BYTES );
        ChkArg( f_cbCipherText == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

        ChkBOOL( *f_pcbPlainText >= ECC_P256_PLAINTEXT_SIZE_IN_BYTES, DRM_E_BUFFERTOOSMALL );

        ChkDR( OEM_ECC_Decrypt_P256( ( const PRIVKEY_P256 * )f_pbPrivateKey,
                                     ( const CIPHERTEXT_P256 * )f_pbCipherText,
                                     ( PLAINTEXT_P256 * )f_pbPlainText,
                                     (struct bigctx_t *)f_pContext ) );

        *f_pcbPlainText = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;
        break;
    case eDRM_RSA:
        ChkArg( f_cbPrivateKey == SIZEOF( DRM_RSA_PRIVATE_KEY ) );

        ChkDR( OEM_RSA_OaepDecrypt( ( const DRM_RSA_PRIVATE_KEY * )f_pbPrivateKey,
                                                                   f_cbCipherText,
                                                                   f_pbCipherText,
                                                                   f_pcbPlainText,
                                                                   f_pbPlainText,
                                            (DRM_CRYPTO_CONTEXT*)  f_pContext ) );
        break;
    default:
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_PK_Sign
**
**  Synopsis:  Generate a digital signature with a private key
**
**  Arguments:
**     [f_pContext]      : Pointer to pk crypto context. Not required for ECC 256
**     [f_eAlgorithm]    : Enumeration specifying algorithm to use for signing
**     [f_cbPrivateKey]  : Byte count of the private key
**     [f_pbPrivateKey]  : Specifies the private key to use
**     [f_cbData]        : Byte count of the input buffer.
**     [f_pbData]        : Specifies the input buffer
**     [f_pcbSignature]  : Byte count of the signature buffer. Returns the length of the
**                         computed signature.
**     [f_pbSignature]   : Specifies the signature buffer
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or improperly initialized
**      DRM_E_BUFFERTOOSMALL
**          One of the buffers was too small for the specified algorithm
**      DRM_E_UNSUPPORTEDALGORITHM
**          f_eAlgorithm specifies an invalid or not supported algorithm
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PK_Sign(
    __in_opt                            DRM_VOID                           *f_pContext,
    __in                                DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __in                                DRM_DWORD                           f_cbPrivateKey,
    __in_bcount( f_cbPrivateKey ) const DRM_BYTE                           *f_pbPrivateKey,
    __in                                DRM_DWORD                           f_cbData,
    __in_bcount( f_cbData ) const       DRM_BYTE                           *f_pbData,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbSignature,
    __out_bcount( *f_pcbSignature )     DRM_BYTE                           *f_pbSignature )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbPrivateKey  != NULL );
    ChkArg( f_pbData        != NULL );
    ChkArg( f_pcbSignature  != NULL );
    ChkArg( f_pbSignature   != NULL );

    switch ( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        ChkArg( f_cbPrivateKey == DRM_ECC160_PRIVATE_KEY_LEN );

        ChkBOOL( *f_pcbSignature >= DRM_ECC160_SIGNATURE_LEN, DRM_E_BUFFERTOOSMALL );

        ChkDR( OEM_ECC_Sign_P160( f_pContext,
                                  ( const PRIVKEY * )f_pbPrivateKey,
                                  f_pbData,
                                  f_cbData,
                                  f_pbSignature ) );

        *f_pcbSignature = DRM_ECC160_SIGNATURE_LEN;
        break;
    case eDRM_ECC_P256:
        ChkArg( f_cbPrivateKey == ECC_P256_INTEGER_SIZE_IN_BYTES );

        ChkBOOL( *f_pcbSignature >= ECDSA_P256_SIGNATURE_SIZE_IN_BYTES, DRM_E_BUFFERTOOSMALL );

        ChkDR( OEM_ECDSA_Sign_P256( f_pbData,
                                    f_cbData,
                                    ( const PRIVKEY_P256 * )f_pbPrivateKey,
                                    ( SIGNATURE_P256 * )f_pbSignature,
                                    (struct bigctx_t *)f_pContext ) );

        *f_pcbSignature = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
        break;
    case eDRM_RSA:
        ChkArg( f_cbPrivateKey == SIZEOF( DRM_RSA_PRIVATE_KEY ) );

        ChkDR( OEM_RSA_PssSign( ( const DRM_RSA_PRIVATE_KEY * )f_pbPrivateKey,
                                                               f_cbData,
                                                               f_pbData,
                                                               0,
                                                               f_pcbSignature,
                                                               f_pbSignature,
                                        (DRM_CRYPTO_CONTEXT*)  f_pContext ) );
        break;
    default:
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_PK_Verify
**
**  Synopsis:  Verify a digital signature created by DRM_PK_Sign.
**
**  Arguments:
**     [f_pContext]      : Pointer to pk crypto context. Not required for ECC 256
**     [f_eAlgorithm]    : Enumeration specifying algorithm to use for signing
**     [f_cbPublicKey]   : Byte count of the public key
**     [f_pbPublicKey]   : Specifies the public key to use
**     [f_cbData]        : Byte count of the input buffer.
**     [f_pbData]        : Specifies the input buffer
**     [f_cbSignature]   : Byte count of the signature buffer.
**     [f_pbSignature]   : Specifies the signature buffer
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALID_SIGNATURE
**          Failed to verify the signature
**      DRM_E_P256_INVALID_SIGNATURE
**          Failed to verify the signature (ECC P256 algorithm)
**      DRM_E_P256_ECDSA_VERIFICATION_ERROR
**          Failed to verify the signature due to ECSDA error (ECC P256 algorithm)
**      DRM_E_INVALIDARG
**          A required argument was NULL or improperly initialized
**      DRM_E_BUFFERTOOSMALL
**          One of the buffers was too small for the specified algorithm
**      DRM_E_UNSUPPORTEDALGORITHM
**          f_eAlgorithm specifies an invalid or not supported algorithm
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PK_Verify(
    __in_opt                           DRM_VOID                           *f_pContext,
    __in                               DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __in                               DRM_DWORD                           f_cbPublicKey,
    __in_bcount( f_cbPublicKey ) const DRM_BYTE                           *f_pbPublicKey,
    __in                               DRM_DWORD                           f_cbData,
    __in_bcount( f_cbData )      const DRM_BYTE                           *f_pbData,
    __in                               DRM_DWORD                           f_cbSignature,
    __in_bcount( f_cbSignature ) const DRM_BYTE                           *f_pbSignature )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbPublicKey   != NULL );
    ChkArg( f_pbData        != NULL );
    ChkArg( f_cbData        > 0 );
    ChkArg( f_pbSignature   != NULL );

     switch ( f_eAlgorithm )
    {
    case eDRM_ECC_P160:
        ChkArg( f_pContext    != NULL );
        ChkArg( f_cbPublicKey == DRM_ECC160_PUBLIC_KEY_LEN );

        ChkBOOL( f_cbSignature == DRM_ECC160_SIGNATURE_LEN, DRM_E_INVALID_SIGNATURE );

        ChkBOOL( OEM_ECC_Verify_P160( f_pContext,
                                      ( const PUBKEY * )f_pbPublicKey,
                                      f_pbData,
                                      f_cbData,
                                      f_pbSignature ), DRM_E_INVALID_SIGNATURE );
        break;
    case eDRM_ECC_P256:
        ChkArg( f_cbPublicKey == ECC_P256_POINT_SIZE_IN_BYTES );

        ChkBOOL( f_cbSignature == ECDSA_P256_SIGNATURE_SIZE_IN_BYTES, DRM_E_INVALID_SIGNATURE );

        ChkDR( OEM_ECDSA_Verify_P256( f_pbData,
                                      f_cbData,
                                      ( const PUBKEY_P256 * )f_pbPublicKey,
                                      ( const SIGNATURE_P256 * )f_pbSignature,
                                      (struct bigctx_t *)f_pContext ) );
        break;
    case eDRM_RSA:
        ChkArg( f_cbPublicKey == SIZEOF( DRM_RSA_PUBLIC_KEY ) );

        ChkDR( OEM_RSA_PssVerify( ( const DRM_RSA_PUBLIC_KEY * )f_pbPublicKey,
                                                                f_cbData,
                                                                f_pbData,
                                                                0,
                                                                f_cbSignature,
                                                                f_pbSignature,
                                          (DRM_CRYPTO_CONTEXT*) f_pContext ) );
        break;
    default:
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
    }

ErrorExit:
    return dr;
}




EXIT_PK_NAMESPACE_CODE;

