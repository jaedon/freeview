/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXPRNDTX_C
#include <drmblackbox.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Different parameters could be const, but these are stub functions and we cannot do that");
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Prefast Noise: Different parameters could be const, but these are stub functions and we cannot do that");
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
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_eKeyPairType);
    UNREFERENCED_PARAMETER(f_poECC256PubKeyTarget);
    UNREFERENCED_PARAMETER(f_pbCertificateTarget);
    UNREFERENCED_PARAMETER(f_cbCertificateTarget);
    UNREFERENCED_PARAMETER(f_pbCertificateTargetCRL);
    UNREFERENCED_PARAMETER(f_cbCertificateTargetCRL);
    UNREFERENCED_PARAMETER(f_pbRevInfo);
    UNREFERENCED_PARAMETER(f_cbRevInfo);
    UNREFERENCED_PARAMETER(f_dwRequestedSecurityLevel);
    UNREFERENCED_PARAMETER(f_poEncryptedAESKeyPair);
    UNREFERENCED_PARAMETER(f_phandleAESKeyPair);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_GenerateAESKeyPairAndAESECBEncrypt(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                              DRM_BBX_KEY_PAIR_TYPE            f_eKeyPairType,
    __in_ecount( 1 )                            const OEM_CRYPTO_HANDLE                f_handleAESRootEncryptionKey,
    __in                                              DRM_DWORD                        f_dwRequestedSecurityLevel,
    __out_ecount( DRM_AES_BLOCKLEN * 2 )              DRM_BYTE                         f_rgbEncryptedAESKeyPair[DRM_AES_BLOCKLEN * 2],
    __out_ecount( 1 )                                 OEM_CRYPTO_HANDLE               *f_phandleAESKeyPair )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_eKeyPairType);
    UNREFERENCED_PARAMETER(f_handleAESRootEncryptionKey);
    UNREFERENCED_PARAMETER(f_dwRequestedSecurityLevel);
    UNREFERENCED_PARAMETER(f_rgbEncryptedAESKeyPair);
    UNREFERENCED_PARAMETER(f_phandleAESKeyPair);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_GenerateAESGenericKey(
    __inout_ecount( 1 )                         DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                        OEM_HAL_KEY_TYPE                 f_eKeyType,
    __out_ecount( 1 )                           OEM_CRYPTO_HANDLE               *f_phandleAESGenericKey )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_eKeyType);
    UNREFERENCED_PARAMETER(f_phandleAESGenericKey);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_EncryptAESKeyPairWithAESECBUsingGenericKey(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESGenericKey,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESKeyPair,
    __out_ecount( DRM_AES_BLOCKLEN * 2 )    DRM_BYTE                         f_rgbEncryptedAESKeyPair[DRM_AES_BLOCKLEN * 2] )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESGenericKey);
    UNREFERENCED_PARAMETER(f_handleAESKeyPair);
    UNREFERENCED_PARAMETER(f_rgbEncryptedAESKeyPair);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_DecryptAESKeyPairWithAESECBUsingGenericKey(
    __inout_ecount( 1 )                         DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                      const OEM_CRYPTO_HANDLE                f_handleAESGenericKey,
    __in_ecount( DRM_AES_BLOCKLEN * 2 )   const DRM_BYTE                         f_rgbEncryptedAESKeyPair[DRM_AES_BLOCKLEN * 2],
    __out_ecount( 1 )                           OEM_CRYPTO_HANDLE               *f_phandleAESKeyPair )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESGenericKey);
    UNREFERENCED_PARAMETER(f_rgbEncryptedAESKeyPair);
    UNREFERENCED_PARAMETER(f_phandleAESKeyPair);
    return DRM_E_NOTIMPL;
}

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
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_pRootmostLicenseBindingInfo);
    UNREFERENCED_PARAMETER(f_poECC256PubKeyTarget);
    UNREFERENCED_PARAMETER(f_pbCertificateTarget);
    UNREFERENCED_PARAMETER(f_cbCertificateTargetCRL);
    UNREFERENCED_PARAMETER(f_pbRevInfo);
    UNREFERENCED_PARAMETER(f_cbRevInfo);
    UNREFERENCED_PARAMETER(f_phandleAESIntegrityKey);
    UNREFERENCED_PARAMETER(f_poEncryptedAESKeyPair);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_EncryptClearContentWithAESCTR(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESContentEncryptionKey,
    __inout_ecount( 1 )                     OEM_ENCRYPTION_HANDLE           *f_phandleEncryption,
    __out                                   DRM_UINT64                      *f_qwInitializationVector,
    __out_ecount_opt(1)                     OEM_HAL_SAMPLE_METADATA         *f_pMetadata,
    __in_opt                                OEM_OPAQUE_BUFFER_HANDLE         f_hInBuf,
    __in_opt                                OEM_OPAQUE_BUFFER_HANDLE         f_hOutBuf,
    __inout_bcount( f_cbData )              DRM_BYTE                        *f_pbData,
    __in                                    DRM_DWORD                        f_cbData )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESContentEncryptionKey);
    UNREFERENCED_PARAMETER(f_phandleEncryption);
    UNREFERENCED_PARAMETER(f_qwInitializationVector);
    UNREFERENCED_PARAMETER(f_pMetadata);
    UNREFERENCED_PARAMETER(f_hInBuf);
    UNREFERENCED_PARAMETER(f_hOutBuf);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_cbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_SignWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE        f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                *f_pbData,
    __in                                    DRM_DWORD                f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )        DRM_BYTE                 f_rgbSignature[DRM_AES_BLOCKLEN] )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESIntegrityKey);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_cbData);
    UNREFERENCED_PARAMETER(f_rgbSignature);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_CalculateChecksum(
    __inout_ecount( 1 )                         DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                      const OEM_CRYPTO_HANDLE                f_handleAESContentEncryptionKey,
    __in                                  const DRM_KID                         *f_pKID,
    __out_ecount( DRM_AES_CK_CHECKSUM_LENGTH )  DRM_BYTE                         f_rgbChecksum[ DRM_AES_CK_CHECKSUM_LENGTH ] )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESContentEncryptionKey);
    UNREFERENCED_PARAMETER(f_pKID);
    UNREFERENCED_PARAMETER(f_rgbChecksum);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_VerifySignatureWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT          *f_poBBXContext,
    __in_ecount( 1 )                        OEM_CRYPTO_HANDLE        f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                *f_pbData,
    __in                                    DRM_DWORD                f_cbData,
    __in_ecount( DRM_AES_BLOCKLEN )   const DRM_BYTE                 f_rgbSignature[DRM_AES_BLOCKLEN] )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESIntegrityKey);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_cbData);
    UNREFERENCED_PARAMETER(f_rgbSignature);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BBX_PRND_Tx_EncryptNonceWithAESECB(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                  const OEM_CRYPTO_HANDLE                f_handleAESMessageEncryptionKey,
    __inout_ecount( 1 )                     DRM_ID                          *f_pNonce )
{
    UNREFERENCED_PARAMETER(f_poBBXContext);
    UNREFERENCED_PARAMETER(f_handleAESMessageEncryptionKey);
    UNREFERENCED_PARAMETER(f_pNonce);
    return DRM_E_NOTIMPL;
}
PREFAST_POP /*__WARNING_NONCONST_BUFFER_PARAM */
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

EXIT_PK_NAMESPACE_CODE;

