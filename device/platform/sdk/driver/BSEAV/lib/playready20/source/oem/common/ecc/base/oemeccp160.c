/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <oemeccp160.h>
#include <oemeccp160impl.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_InitializeBignumStack( DRM_VOID* f_pContext ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_InitializeBignumStack( DRM_VOID* f_pContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( f_pContext ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_ThreadUnSafePKInit(
    DRM_VOID* f_pContext,
    DRM_VOID* f_pOEMContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_E_PKCRYPTO_FAILURE;

    ChkDR( OEM_ECC_ThreadUnSafePKInitImpl( f_pContext, f_pOEMContext ) );

ErrorExit:
    return dr;
}

DRM_API DRM_VOID DRM_CALL OEM_ECC_ZeroPublicKey_P160(
    __inout        PUBKEY     *f_pPubkey )
{
    OEM_ECC_ZeroPublicKey_P160Impl( f_pPubkey );
}

DRM_API DRM_VOID DRM_CALL OEM_ECC_ZeroPrivateKey_P160(
    __inout        PRIVKEY    *f_pPrivkey )
{
    OEM_ECC_ZeroPrivateKey_P160Impl( f_pPrivkey );
}

DRM_API DRM_VOID DRM_CALL DRM_RSA_PKInitSize(
    IN OUT DRM_VOID *f_pContext,
    IN     DRM_DWORD f_dwSize )
{
    DRM_RSA_PKInitSizeImpl( f_pContext, f_dwSize );
}

/*********************************************************************
**
**  Function:  OEM_ECC_GenKeyPair_P160
**
**  Synopsis:  Generate a matched public and private key pair
**
**  Arguments:
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]  -- New public key
**     [f_pprivkey] -- New private key
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenKeyPair_P160(
    IN     DRM_VOID *f_pContext,
       OUT PUBKEY   *f_ppubkey,
       OUT PRIVKEY  *f_pprivkey )
{
    DRM_RESULT dr = DRM_E_PKCRYPTO_FAILURE;

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_GenKeyPair_P160);
#endif

    ChkDR( OEM_ECC_GenKeyPair_P160Impl(
        f_pContext,
        f_ppubkey,
        f_pprivkey ) );

ErrorExit:

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
}


/*********************************************************************
**
**  Function:  OEM_ECC_Encrypt_P160
**
**  Synopsis:  Encrypt a block of data with a given public key.
**
**  Arguments:
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]  -- Public key to encrypt with
**     [f_rgbIn]    -- Data to be encrypted
**     [f_rgbOut]   -- Buffer to hold the encrypted data value.
**
**  Notes:  Data is not encrpyted in place.  It is put in the rgbOut buffer.
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_Encrypt_P160(
    IN        DRM_VOID *f_pContext,
    IN  const PUBKEY   *f_ppubkey,
    __in_bcount( DRM_ECC160_PLAINTEXT_LEN ) const DRM_BYTE  f_rgbIn[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)],
    __out_bcount( DRM_ECC160_CIPHERTEXT_LEN ) DRM_BYTE  f_rgbOut[__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( OEM_ECC_Encrypt_P160Impl(
        f_pContext,
        f_ppubkey,
        f_rgbIn,
        f_rgbOut ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  OEM_ECC_Decrypt_P160
**
**  Synopsis:
**
**  Arguments:
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_pprivkey] -- Private key to decrypt with
**     [f_rgbIn]    -- Encrypted bytes that are to be decrypted
**     [f_rgbOut]   -- Clear text result
**
**  Notes:  Data is not decrpyted in place.  It is put in the rgbOut buffer.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_Decrypt_P160(
    IN        DRM_VOID *f_pContext,
    IN  const PRIVKEY  *f_pprivkey,
    __in_bcount( DRM_ECC160_CIPHERTEXT_LEN ) const DRM_BYTE  f_rgbIn[__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)],
    __out_bcount( DRM_ECC160_PLAINTEXT_LEN ) DRM_BYTE  f_rgbOut[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_Decrypt_P160);
#endif

    ChkDR( OEM_ECC_Decrypt_P160Impl(
        f_pContext,
        f_pprivkey,
        f_rgbIn,
        f_rgbOut ) );

ErrorExit:

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
}


/*********************************************************************
**
**  Function:  OEM_ECC_Sign_P160
**
**  Synopsis:  Generate a digital signature with a private key,
**             using ECDSA algorithm.
**
**  Arguments:
**     [f_pContext]     -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_privkey]      -- Private key to create a signature with
**     [f_pbData]       -- Array of bytes to create a signature over
**     [f_cbData]       -- Length of pbBuffer in bytes
**     [f_rgbSignature] -- Buffer to hold result signature
**
**  Notes:     Notation used in the comments is the one of a book "Guide to ECC",
**             see page 184 for the implemented algorithm.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_Sign_P160(
    IN       DRM_VOID  *f_pContext,
    IN const PRIVKEY   *f_privkey,
    __out_bcount( f_cbData ) const DRM_BYTE  *f_pbData,
    IN       DRM_DWORD  f_cbData,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN ) DRM_BYTE   f_rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_Sign_P160);
#endif

    ChkDR( OEM_ECC_Sign_P160Impl(
        f_pContext,
        f_privkey,
        f_pbData,
        f_cbData,
        f_rgbSignature ) );

ErrorExit:

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECC_Sign_P160 */


/*********************************************************************
**
**  Function:  OEM_ECC_Verify_P160
**
**  Synopsis:  Verify an ECDSA digital signature created
**             by OEM_ECC_Sign_P160.
**
**  Arguments:
**     [f_pContext]     -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]      -- Pubkey to check the signature with
**     [f_pbData]       -- Data buffer that the signature was created over
**     [f_cbData]       -- Length of pbBuffer in bytes
**     [f_rgbSignature] -- The signature to verify
**
**  Returns:  TRUE if the signature verified correctly.  FALSE is it didn't
**
**  Notes:     Notation used in the comments is the one of a book "Guide to ECC",
**             see page 184 for the implemented algorithm.

*********************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL OEM_ECC_Verify_P160(
    IN       DRM_VOID  *f_pContext,
    IN const PUBKEY    *f_ppubkey,
    __in_bcount( f_cbData ) const DRM_BYTE  *f_pbData,
    IN       DRM_DWORD  f_cbData,
    __in_bcount( DRM_ECC160_SIGNATURE_LEN ) const DRM_BYTE   f_rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_BOOL fResult = FALSE;

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
#if DRM_SUPPORT_TRACING
    DRM_RESULT dr    = DRM_SUCCESS;
#endif
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRM_ECC_P160, PERF_FUNC_OEM_ECC_Verify_P160);
#endif

    fResult = OEM_ECC_Verify_P160Impl(
        f_pContext,
        f_ppubkey,
        f_pbData,
        f_cbData,
        f_rgbSignature );

#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_PC
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return fResult;
}

EXIT_PK_NAMESPACE_CODE;
