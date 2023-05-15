/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrmnet.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_INVALID_PARAM_VALUE_1, "*f_ppEntries should not yield this warning given __checkReturn and declaration of DRM_RESULT, but it still does." )
DRM_RESULT DRM_NO_INLINE DRM_CALL DRM_RVK_GetWMDRMNETRevocationEntries(
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")
    __inout_bcount( f_cbRevocationData )  DRM_BYTE            *f_pbRevocationData,
PREFAST_POP
    __in                                  DRM_DWORD            f_cbRevocationData,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")
    __out_ecount(1)                       DRM_DWORD           *f_pcEntries,
PREFAST_POP
    __deref_out_ecount( *f_pcEntries )    WMDRMNET_CRL_ENTRY **f_ppEntries )
PREFAST_POP /* __WARNING_INVALID_PARAM_VALUE_1 */
{
    UNREFERENCED_PARAMETER( f_pbRevocationData );
    UNREFERENCED_PARAMETER( f_cbRevocationData );
    UNREFERENCED_PARAMETER( f_pcEntries );
    UNREFERENCED_PARAMETER( f_ppEntries );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_RVK_UpdateWMDRMNETRevocationList(
    __in                                                                                 DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                                                                 DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_rgchBase length defined by f_pdasstrRevocationList")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")
    __inout_ecount( f_pdasstrRevocationList->m_ich + f_pdasstrRevocationList->m_cch )    DRM_CHAR                    *f_rgchBase,
PREFAST_POP
PREFAST_POP
    __in                                                                           const DRM_SUBSTRING               *f_pdasstrRevocationList,
    __in_bcount( f_cbRevocationBuffer )                                            const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                                                                 DRM_DWORD                    f_cbRevocationBuffer,
    __in                                                                           const PUBKEY                      *f_ppubkey )
{
    UNREFERENCED_PARAMETER( f_pcontextCrypto );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_rgchBase );
    UNREFERENCED_PARAMETER( f_pdasstrRevocationList );
    UNREFERENCED_PARAMETER( f_pbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_cbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_ppubkey );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetWMDRMNETList(
    __in                                           DRM_CRYPTO_CONTEXT           *f_pcontextCRYP,
    __in                                           DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                     *f_pbRevocationData,
    __inout                                        DRM_DWORD                    *f_pcbRevocationData,
    __out                                          DRM_DWORD                    *f_pidSerial )
{
    UNREFERENCED_PARAMETER( f_pcontextCRYP );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbRevocationData );
    UNREFERENCED_PARAMETER( f_pcbRevocationData );
    UNREFERENCED_PARAMETER( f_pidSerial );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_RESULT DRM_RVK_VerifyWMDRMNETRevocationList(
    __in_opt DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in     DRM_CONST_STRING            *f_pdstrList,
    __out    DRM_DWORD                   *f_pdwVersion,
    __in     DRM_CRYPTO_CONTEXT          *f_pCryptoCtx )
{
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pdstrList );
    UNREFERENCED_PARAMETER( f_pdwVersion );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_RESULT DRM_RVK_UpdateWMDRMNETRevocationListDecoded(
    __in                                     DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_ecount(f_cbRevocationList)          DRM_BYTE                    *f_pbRevocationList,
    __in                                     DRM_DWORD                    f_cbRevocationList,
    __in_ecount(f_cbRevocationBuffer)  const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                     DRM_DWORD                    f_cbRevocationBuffer,
    __out_opt                                DRM_BOOL                    *f_pfUpdated,
    __out_opt                                DRM_DWORD                   *f_pdwVersion )
{
    UNREFERENCED_PARAMETER( f_pcontextCrypto );
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbRevocationList );
    UNREFERENCED_PARAMETER( f_cbRevocationList );
    UNREFERENCED_PARAMETER( f_pbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_cbRevocationBuffer );
    UNREFERENCED_PARAMETER( f_pfUpdated );
    UNREFERENCED_PARAMETER( f_pdwVersion );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyBinaryWMDRMNETSignature(
    __in_opt                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_bcount( f_cbData ) const DRM_BYTE                    *f_pbData,
    __in                          DRM_DWORD                    f_cbData,
    __inout                       DRM_DWORD                   *f_pidSerial,
    __in                          DRM_CRYPTO_CONTEXT          *f_pCryptoCtx )
{
    UNREFERENCED_PARAMETER( f_pContextRev );
    UNREFERENCED_PARAMETER( f_pbData );
    UNREFERENCED_PARAMETER( f_cbData );
    UNREFERENCED_PARAMETER( f_pidSerial );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyCRLSignature(
    __in_opt                             DRM_BB_CONTEXT       *f_pcontextBBX,
    __in_opt                             DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                             DRM_DST              *f_pDatastore,
    __in_bcount( f_cbSignedBytes ) const DRM_BYTE             *f_pbSignedBytes,
    __in                                 DRM_DWORD             f_cbSignedBytes,
    __in_bcount( f_cbSignature )   const DRM_BYTE             *f_pbSignature,
    __in                                 DRM_DWORD             f_cbSignature,
    __in                                 DRM_SUBSTRING         f_dasstrCertificate,
    __in                                 DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    UNREFERENCED_PARAMETER( f_pcontextBBX );
    UNREFERENCED_PARAMETER( f_pcontextSST );
    UNREFERENCED_PARAMETER( f_pDatastore );
    UNREFERENCED_PARAMETER( f_pbSignedBytes );
    UNREFERENCED_PARAMETER( f_cbSignedBytes );
    UNREFERENCED_PARAMETER( f_pbSignature );
    UNREFERENCED_PARAMETER( f_cbSignature );
    UNREFERENCED_PARAMETER( f_dasstrCertificate );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL DRM_XMB_RSASignAndCloseCurrNodeA(
    __inout         _XMBContextA                *f_pbContextXMLA,
    __in      const DRM_RSA_PRIVATE_KEY_2048BIT *f_pprivkeyRSA,
    __in            DRM_BOOL                     f_fIncludeTag,
    __out           DRM_DWORD                   *f_pcbSignature,
    __out_bcount    (*f_pcbSignature)  DRM_BYTE *f_pbSignature,
    __out_ecount_opt(*f_pcchSignature) DRM_CHAR *f_pszSignature,
    __inout         DRM_DWORD                   *f_pcchSignature,
    __out_opt       DRM_SUBSTRING               *f_pdasstrXMLFragment,
    __in            DRM_CRYPTO_CONTEXT          *f_pCryptCtx)
{
    UNREFERENCED_PARAMETER( f_pbContextXMLA );
    UNREFERENCED_PARAMETER( f_pprivkeyRSA );
    UNREFERENCED_PARAMETER( f_fIncludeTag );
    UNREFERENCED_PARAMETER( f_pcbSignature );
    UNREFERENCED_PARAMETER( f_pbSignature );
    UNREFERENCED_PARAMETER( f_pszSignature );
    UNREFERENCED_PARAMETER( f_pcchSignature );
    UNREFERENCED_PARAMETER( f_pdasstrXMLFragment );
    UNREFERENCED_PARAMETER( f_pCryptCtx );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_CALL DRM_XMB_HashAndRSASignAndCloseCurrNodeA(
    __inout                                                         _XMBContextA                *f_pbContextXMLA,
    __in                                                            DRM_SHA1_CONTEXT            *f_pcontextSHA,
    __in                                                      const DRM_RSA_PRIVATE_KEY_2048BIT *f_pprivkeyRSA,
    __in                                                            DRM_BOOL                     f_fIncludeTag,
    __out_bcount(DRM_SHA1_DIGEST_LEN)                               DRM_BYTE                     f_rgbSHA  [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out_ecount(DRM_SHA1_B64ENC_DIGEST_LEN)                        DRM_CHAR                     f_rgchSHA [__CB_DECL(DRM_SHA1_B64ENC_DIGEST_LEN)],
    __out_ecount(1)                                                 DRM_DWORD                   *f_pcbSignature,
    __out_bcount(*f_pcbSignature)                                   DRM_BYTE                    *f_pbSignature,
    __out                                                           DRM_DWORD                   *f_pcchSignature,
    __out_ecount(DRM_RSA_B64ENC_PRIME_MAX)                          DRM_CHAR                     f_rgchSignature [__CB_DECL(DRM_RSA_B64ENC_PRIME_MAX)],
    __out_opt                                                       DRM_SUBSTRING               *f_pdasstrXMLFragment,
    __in                                                            DRM_CRYPTO_CONTEXT          *f_pCryptCtx )
{
    UNREFERENCED_PARAMETER( f_pbContextXMLA );
    UNREFERENCED_PARAMETER( f_pcontextSHA );
    UNREFERENCED_PARAMETER( f_pprivkeyRSA );
    UNREFERENCED_PARAMETER( f_fIncludeTag );
    UNREFERENCED_PARAMETER( f_rgbSHA );
    UNREFERENCED_PARAMETER( f_rgchSHA );
    UNREFERENCED_PARAMETER( f_pcbSignature );
    UNREFERENCED_PARAMETER( f_pbSignature );
    UNREFERENCED_PARAMETER( f_pcchSignature );
    UNREFERENCED_PARAMETER( f_rgchSignature );
    UNREFERENCED_PARAMETER( f_pdasstrXMLFragment );
    UNREFERENCED_PARAMETER( f_pCryptCtx );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_NDCert_CreateDeviceCertificate(
    __in_opt       DRM_VOID                    *f_pOEMContext,
    __inout        DRM_ANSI_STRING             *f_pdastrBuffer,
    __in           DRM_DWORD                    f_cchBuffer,
    __in     const DRM_RSA_PRIVATE_KEY_2048BIT *f_poRSAModelPrivKey,
    __in     const DRM_RSA_PUBLIC_KEY_2048BIT  *f_poRSADevicePubKey,
    __in           DRM_CRYPTO_CONTEXT          *f_pCryptoCtx  )
{
    UNREFERENCED_PARAMETER( f_pOEMContext );
    UNREFERENCED_PARAMETER( f_pdastrBuffer );
    UNREFERENCED_PARAMETER( f_cchBuffer );
    UNREFERENCED_PARAMETER( f_poRSAModelPrivKey );
    UNREFERENCED_PARAMETER( f_poRSADevicePubKey );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_NDCert_GetDeviceCertificate(
    __in_opt       DRM_VOID            *f_pOEMContext,
    __inout        DRM_ANSI_STRING     *f_pdastrNDCert,
    __in    const  DRM_DWORD            f_dwFlags,
    __inout        DRM_CRYPTO_CONTEXT  *f_pCryptoCtx )
{
    UNREFERENCED_PARAMETER( f_pOEMContext );
    UNREFERENCED_PARAMETER( f_pdastrNDCert );
    UNREFERENCED_PARAMETER( f_dwFlags );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_NDCert_Certificate_Initialize(
    __in        DRM_APP_CONTEXT  *f_pAppContext,
    __in_opt    DRM_VOID         *f_pOEMContext )
{
    UNREFERENCED_PARAMETER( f_pAppContext );
    UNREFERENCED_PARAMETER( f_pOEMContext );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

