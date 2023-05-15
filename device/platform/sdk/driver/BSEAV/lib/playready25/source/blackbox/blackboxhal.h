/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __DRMBLACKBOXHAL_H__
#define __DRMBLACKBOXHAL_H__

#include <oemhal.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL _Hal_IsHandleCachingSupported(DRM_VOID) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL _Hal_Initialize(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBbxContext );


DRM_API DRM_RESULT DRM_CALL _Hal_Deinitialize(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBbxContext );


PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
DRM_API DRM_RESULT DRM_CALL _Hal_ReleaseKey( 
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey );
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */


DRM_API DRM_RESULT DRM_CALL _Hal_DuplicateKey( 
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey,
    __out_ecount( 1 )   OEM_CRYPTO_HANDLE *f_phDuplicate );


DRM_API OEM_HAL_KEY_REGISTER_INDEX DRM_CALL TEST_Hal_GetKeyRegisterIndex( 
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey );

DRM_API DRM_RESULT DRM_CALL TEST_Hal_PurgeKeyHandleCache( 
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_HAL_KEY_TYPE   f_eType );

DRM_API OEM_HAL_KEY_TYPE DRM_CALL _Hal_GetKeyType( 
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE  f_hKey );


DRM_API DRM_RESULT DRM_CALL _Hal_GetPreloadedKey( 
    __inout_ecount( 1 )    DRM_BB_CONTEXT    *f_pBbxContext,
    __in                   OEM_HAL_KEY_TYPE   f_eType,
    __in_ecount( 1 ) const DRM_GUID          *f_pguidKeyId,
    __out_ecount( 1 )      OEM_CRYPTO_HANDLE *f_phKey );


DRM_API DRM_RESULT DRM_CALL _Hal_VerifyMessageSignature( 
    __inout_ecount( 1 )                DRM_BB_CONTEXT            *f_pBbxContext,
    __in_bcount( f_cbMessage )   const DRM_BYTE                  *f_rgbMessage,
    __in                               DRM_DWORD                  f_cbMessage,
    __in_bcount( f_cbSignature ) const DRM_BYTE                  *f_rgbSignature,
    __in                               DRM_DWORD                  f_cbSignature,
    __in                               OEM_HAL_HASH_TYPE          f_eHashType,
    __in                               OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                               OEM_CRYPTO_HANDLE          f_hIntegrityKey );


DRM_API DRM_RESULT DRM_CALL _Hal_SignWithPrivateKey(
    __inout_ecount( 1 )                 DRM_BB_CONTEXT            *f_pBbxContext,
    __in_bcount( f_cbMessage )    const DRM_BYTE                  *f_rgbMessage,
    __in                                DRM_DWORD                  f_cbMessage,
    __out_bcount_opt( *f_pcbSignature ) DRM_BYTE                  *f_rgbSignature,
    __inout                             DRM_DWORD                 *f_pcbSignature,
    __in                                OEM_HAL_HASH_TYPE          f_eHashType,
    __in                                OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                                OEM_CRYPTO_HANDLE          f_hIntegrityKey );


DRM_API DRM_RESULT DRM_CALL _Hal_VerifyOMAC1Signature(
    __inout_ecount( 1 )                DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbData )      const DRM_BYTE          *f_rgbData,
    __in                               DRM_DWORD          f_cbData,
    __in_bcount( f_cbSignature ) const DRM_BYTE          *f_rgbSignature,
    __in                               DRM_DWORD          f_cbSignature,
    __in                               OEM_CRYPTO_HANDLE  f_hKey );


DRM_API DRM_RESULT DRM_CALL _Hal_CreateOMAC1Signature(
    __inout_ecount( 1 )                 DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbData )       const DRM_BYTE          *f_rgbData,
    __in                                DRM_DWORD          f_cbData,
    __out_bcount_opt( *f_pcbSignature ) DRM_BYTE          *f_rgbSignature,
    __inout                             DRM_DWORD         *f_pcbSignature,
    __in                                OEM_CRYPTO_HANDLE  f_hKey );

DRM_API DRM_RESULT DRM_CALL _Hal_AllocateKeyHandle(
    __inout_ecount( 1 )               DRM_BB_CONTEXT             *f_pBbxContext,
    __in                              OEM_HAL_KEY_TYPE            f_eType,
    __in_bcount( f_cbCacheKey ) const DRM_BYTE                   *f_pbCacheKey,
    __in                        const DRM_DWORD                   f_cbCacheKey,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE          *f_phKey,
    __out_ecount( 1 )                 DRM_BB_KEY_REGISTER_ENTRY **f_ppEntry,
    __out_ecount( 1 )                 DRM_BOOL                   *f_pCachedHandle );

DRM_API DRM_RESULT DRM_CALL _Hal_UnwrapKey(
    __inout                                 DRM_BB_CONTEXT     *f_pBbxContext,
    __in                                    OEM_HAL_KEY_TYPE    f_eKeyType,
    __in                                    OEM_CRYPTO_HANDLE   f_hWrappingKey,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE           *f_pbWrappedKeyData,
    __in                                    DRM_DWORD           f_cbWrappedKeyData,
    __in_bcount( f_cbParameterData )  const DRM_BYTE           *f_pbParameterData,
    __in                                    DRM_DWORD           f_cbParameterData,
    __in_bcount( f_cbCacheKey )       const DRM_BYTE           *f_pbCacheKey,
    __in                              const DRM_DWORD           f_cbCacheKey,
    __out_ecount( 1 )                       OEM_CRYPTO_HANDLE  *f_phKey);


DRM_API DRM_RESULT DRM_CALL _Hal_WrapKey(
    __inout_ecount( 1 )                  DRM_BB_CONTEXT     *f_pBbxContext,
    __in                                 OEM_CRYPTO_HANDLE   f_hKey,
    __in                                 OEM_CRYPTO_HANDLE   f_hWrappingKey,
    __out_bcount_opt( *f_pcbWrappedKey ) DRM_BYTE           *f_pbWrappedKey,
    __inout_ecount(1)                    DRM_DWORD          *f_pcbWrappedKey );


DRM_API DRM_RESULT DRM_CALL _Hal_GenerateKey(
    __inout_ecount( 1 ) DRM_BB_CONTEXT     *f_pBbxContext,
    __in                OEM_HAL_KEY_TYPE    f_eKeyType,
    __in                DRM_DWORD           f_dwRequestedSecurityLevel,
    __out_ecount(1)     OEM_CRYPTO_HANDLE  *f_phKey );


DRM_API DRM_RESULT DRM_CALL _Hal_CreateSampleEncryptionContext(
    __inout_ecount( 1 ) DRM_BB_CONTEXT                 *f_pBbxContext,
    __in                OEM_CRYPTO_HANDLE               f_hKey,
    __in                OEM_HAL_SAMPLE_ENCRYPTION_MODE  f_eSampleEncryptionMode,
    __out_ecount(1)     OEM_ENCRYPTION_HANDLE          *f_phEncryptionContext );


DRM_API DRM_RESULT DRM_CALL _Hal_EncryptDataBlock(
    __inout_ecount( 1 )                       DRM_BB_CONTEXT             *f_pBbxContext,
    __in                                      OEM_CRYPTO_HANDLE           f_hKey,
    __in_bcount( f_cbData )             const DRM_BYTE                   *f_rgbBlock,
    __in                                      DRM_DWORD                   f_cbBlock,
    __out_bcount_opt( *f_pcbEncryptedBlock )  DRM_BYTE                   *f_rgbEncryptedBlock,
    __inout                                   DRM_DWORD                  *f_pcbEncryptedBlock );
    
#if 0
//Added For Get Content Key    
DRM_API DRM_RESULT DRM_CALL _Hal_GetContentKey(
    __inout_ecount( 1 )         DRM_BB_CONTEXT               *f_pBbxContext,
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext) ;
#endif

DRM_API DRM_RESULT DRM_CALL _Hal_DecryptContentOpaque(
    __inout_ecount( 1 )         DRM_BB_CONTEXT               *f_pBbxContext,
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout                     DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        OEM_OPAQUE_BUFFER_HANDLE      f_hInBuf,
    __in                        OEM_OPAQUE_BUFFER_HANDLE      f_hOutBuf,
    __in                        DRM_DWORD                     f_cbData );


DRM_API DRM_RESULT DRM_CALL _Hal_DecryptContent(
    __inout_ecount( 1 )         DRM_BB_CONTEXT               *f_pBbxContext,
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout_opt                 DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )  DRM_BYTE                     *f_pbData );


DRM_API DRM_RESULT DRM_CALL _Hal_EncryptSampleData(
    __in                           DRM_VOID                 *f_pOEMContext,
    __in                           OEM_ENCRYPTION_HANDLE     f_hEncryptionContext,
    __out_ecount_opt(1)            DRM_UINT64               *f_pqwInitializationVector,
    __out_ecount_opt(1)            OEM_HAL_SAMPLE_METADATA  *f_pMetadata,
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hInBuf,
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hOutBuf,
    __in                           DRM_DWORD                 f_cbData,
    __inout_bcount_opt( f_cbData ) DRM_BYTE                 *f_pbData );


DRM_API DRM_RESULT DRM_CALL _Hal_LoadPlayReadyRevocationInfo(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbRevocationInfo ) const DRM_BYTE          *f_pbRevocationInfo,
    __in                                    DRM_DWORD          f_cbRevocationInfo,
    __in                                    OEM_CRYPTO_HANDLE  f_hSigningKey );


DRM_API DRM_RESULT DRM_CALL _Hal_LoadPlayReadyCrl(
    __inout_ecount( 1 )          DRM_BB_CONTEXT    *f_pBbxContext,
    __in_bcount( f_cbCRL ) const DRM_BYTE          *f_pbCRL,
    __in                         DRM_DWORD          f_cbCRL,
    __in                         OEM_CRYPTO_HANDLE  f_hSigningKey );


EXIT_PK_NAMESPACE;

#endif /* __DRMBLACKBOXHAL_H__ */
