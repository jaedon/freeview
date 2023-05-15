/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemrsaoaeppssimpl.h>
#include <oemshahash.h>
#include <drmutilities.h>
#include <byteorder.h>
#include <oem.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
**  Function:  OEM_RSA_OaepDecrypt
**
**  Synopsis:  Decrypts a cipher text that was encrypted using
**             PKCS #1 RSAES-OAEP v2.1. MGF1 is used as the mask
**             generation function and SHA1 is the hashing algorithm.
**             This version of OAEP decrypt uses an empty string for
**             the "Label".
**
**  Arguments:
**     [f_pPrivateKey]   : Specifies the private key to use.
**                         NULL indicates the private key baked into
**                         the device should be used.
**     [f_rgbCiphertext] : Specifies the encrypted text
**     [f_rgpbPlaintext] : Returns the decrypted data.
**     [f_pcbPlaintext]  : Returns the size of the original plaintext
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pcbPlaintext is NULL
**      DRM_E_OEM_RSA_DECRYPTION_ERROR
**          An error occurred while processing the data
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_OaepDecrypt(
    __in const                          DRM_RSA_PRIVATE_KEY *f_pPrivateKey,
    __in const                          DRM_DWORD            f_cbCiphertext,
    __in_ecount( f_cbCiphertext ) const DRM_BYTE            *f_pbCiphertext,
    __inout                             DRM_DWORD           *f_pcbPlaintext,
    __out_ecount( *f_pcbPlaintext )     DRM_BYTE            *f_pbPlaintext,
    __in                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_OaepDecrypt );

    ChkDR( OEM_RSA_OaepDecryptImpl(
        f_pPrivateKey,
        f_cbCiphertext,
        f_pbCiphertext,
        f_pcbPlaintext,
        f_pbPlaintext,
        f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
**  Function:  OEM_RSA_OaepEncrypt
**
**  Synopsis:  Encrypts a cipher text using PKCS #1 RSAES-OAEP v2.1.
**             MGF1 is used as the mask generation function and SHA1 is
**             the hashing algorithm. This version of OAEP encrypt uses
**             an empty string for the "Label".
**
**  Arguments:
**     [f_pPublicKey]    :  Specifies the public key to use
**     [f_rgbPlaintext]  :  Specifies the plain text to encrypt
**     [f_cbPlaintext]   :  Specifies the size of the plain text
**     [f_rgbCiphertext] :  Returns the encrypted data.
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_ENCRYPTION_ERROR
**          An error occurred while processing the data
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_OaepEncrypt(
    __in_opt                               DRM_VOID           *f_pOEMContext,
    __in                             const DRM_RSA_PUBLIC_KEY *f_pPublicKey,
    __in                             const DRM_DWORD           f_cbPlaintext,
    __in_ecount( f_cbPlaintext )     const DRM_BYTE           *f_pbPlaintext,
    __inout                                DRM_DWORD          *f_pcbCiphertext,
    __out_ecount( *f_pcbCiphertext ) const DRM_BYTE           *f_pbCiphertext,
    __in                                   DRM_CRYPTO_CONTEXT *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_OaepEncrypt );

    ChkDR( OEM_RSA_OaepEncryptImpl(
        f_pOEMContext,
        f_pPublicKey,
        f_cbPlaintext,
        f_pbPlaintext,
        f_pcbCiphertext,
        f_pbCiphertext,
        f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_PssSign
**
**  Synopsis: RSA signs the given data with the privkey.
**            Uses PSS (see PKCS #1 v2.1)
**
**  Arguments:
**     [f_pPrivateKey]   : Specifies the private key to use.
**                         NULL indicates the private key baked into
**                         the device should be used
**     [f_pbMsg]         : Specifies the Message to sign
**     [f_cbMsg]         : Specifies the length of the original message
**     [f_ibMsg]         : Specifies the offset from the beginning of the original message
**                         buffer
**     [f_prgbSignature] : Returns the computed RSA signature
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_PssSign(
    __in const                      DRM_RSA_PRIVATE_KEY *f_pPrivateKey,
    __in const                      DRM_DWORD            f_cbMsg,
    __in_ecount( f_cbMsg ) const    DRM_BYTE            *f_pbMsg,
    __in const                      DRM_DWORD            f_ibMsg,
    __inout                         DRM_DWORD           *f_pcbSignature,
    __out_ecount( *f_pcbSignature ) DRM_BYTE            *f_pbSignature,
    __in                            DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_PssSign );

    ChkDR( OEM_RSA_PssSignImpl(
        f_pPrivateKey,
        f_cbMsg,
        f_pbMsg,
        f_ibMsg,
        f_pcbSignature,
        f_pbSignature,
        f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_PssVerify
**
**  Synopsis: Verifies an RSA signature using the given pubkey.
**            Uses PSS (see PKCS #1 v2.1)
**
**  Arguments:
**     [f_pPublicKey]   : Specifies the public key to use.
**     [f_pbMsg]        : Specifies the original message
**     [f_cbMsg]        : Specifies the length of the original message
**     [f_ibMsg]        : Specifies the offset from the beginning of the original message
**                        buffer
**     [f_rgbSignature] : Specifies the signature to verify
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_RSA_SIGNATURE_ERROR
**          Failed to verify the signature
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_PssVerify(
    __in const                         DRM_RSA_PUBLIC_KEY  *f_pPublicKey,
    __in                               DRM_DWORD            f_cbMsg,
    __in_ecount( f_cbMsg ) const       DRM_BYTE            *f_pbMsg,
    __in const                         DRM_DWORD            f_ibMsg,
    __in const                         DRM_DWORD            f_cbSignature,
    __in_ecount( f_cbSignature ) const DRM_BYTE            *f_pbSignature,
    __in                               DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMRSA, PERF_FUNC_OEM_RSA_PssVerify );

    ChkDR( OEM_RSA_PssVerifyImpl(
        f_pPublicKey,
        f_cbMsg,
        f_pbMsg,
        f_ibMsg,
        f_cbSignature,
        f_pbSignature,
        f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

