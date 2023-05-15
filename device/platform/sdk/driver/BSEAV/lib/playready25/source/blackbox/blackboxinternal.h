/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBLACKBOXINTERNAL_H__
#define __DRMBLACKBOXINTERNAL_H__

#include <drmblackbox.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL _GetDeviceECC256KeyPair(
    __in              DRM_BB_CONTEXT                      *f_poBBXContext,
    __in_opt    const PUBKEY_P256                         *f_poDevPubKeyIn,
    __out_opt         PUBKEY_P256                         *f_poDevPubKey,
    __out_ecount( 1 ) OEM_CRYPTO_HANDLE                   *f_phDevPrivKey,
    __in              eDRM_BINARY_DEVICE_CERT_KEYUSAGE     f_eDevKeyUsage );

DRM_API DRM_RESULT DRM_CALL _DecryptContentKeyXMR(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pContextBBX,
    __in                              DRM_BOOL             f_fIgnoreChecksum,
    __out_ecount( 1 )                 OEM_CRYPTO_HANDLE   *f_phKey,            /* the created key handle */
    __out_ecount_opt( 1 )             OEM_CRYPTO_HANDLE   *f_phPrivKey );      /* returns the privkey used to decrypt */

DRM_API DRM_RESULT DRM_CALL _SymmetricWrapKey(
    __inout_ecount( 1 )          DRM_BB_CONTEXT    *f_pBbxContext,
    __in_ecount( 1 )             OEM_CRYPTO_HANDLE  f_hKeyToWrap,
    __in                         DRM_DWORD          f_cbData,
    __out_bcount_opt( f_cbData ) DRM_BYTE          *f_pbDataOut );

DRM_API DRM_RESULT DRM_CALL _SymmetricUnwrapKey(
    __inout_ecount( 1 )           DRM_BB_CONTEXT    *f_poBBXContext,
    __in_bcount( f_cbData ) const DRM_BYTE          *f_pbDataIn,
    __in                          DRM_DWORD          f_cbData,
    __out_ecount( 1 )             OEM_CRYPTO_HANDLE *f_pbDataOut );

DRM_RESULT DRM_CALL _UpdateXMRWithSymmKey(
    __inout_ecount( 1 )                          DRM_BB_CONTEXT   *f_pcontextBBX,
    __in                                         OEM_CRYPTO_HANDLE f_hCIKey,
    __in_bcount_opt( DRM_AES_KEYSIZE_128 ) const DRM_BYTE         *f_rgbCIKey,
    __in_bcount( DRM_AES_KEYSIZE_128 * 2 ) const DRM_BYTE          f_rgbEncryptedKeys[__CB_DECL( DRM_AES_KEYSIZE_128 * 2 )],
    __inout_ecount( 1 )                          DRM_XMR_LICENSE  *f_pXmrLicense );

EXIT_PK_NAMESPACE;

#endif /* __DRMBLACKBOXINTERNAL_H__ */

