/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBLACKBOXDEVCERTS_H__
#define __DRMBLACKBOXDEVCERTS_H__

#include <drmblackbox.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _ValidatePublicKey(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                        const DRM_PUBLIC_KEY_CONTEXT          *f_poECC256PubKeyTarget,
    __in_ecount_opt( f_cbCertificateTarget )    const DRM_BYTE                        *f_pbCertificateTarget,
    __in                                              DRM_DWORD                        f_cbCertificateTarget,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                        *f_pbRevInfo,
    __in                                              DRM_DWORD                        f_cbRevInfo ) DRM_NO_INLINE_ATTRIBUTE;


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL _ResolveSigningChainKey( 
    __in_ecount(1)                  DRM_BB_CONTEXT    *f_pBlackboxContext,
    __in_bcount(f_cbCertData) const DRM_BYTE          *f_pbCertData,
    __in                      const DRM_DWORD          f_cbCertData,
    __in                            DRM_DWORD          f_dwChainLength,
    __in                            DRM_DWORD          f_idxResolveUpTo,
    __in                            DRM_DWORD          f_dwRequiredKeyUsageType,
    __in                            DRM_DWORD          f_bOffset,
    __out_ecount(1)                 OEM_CRYPTO_HANDLE *f_phSigningKey ) DRM_NO_INLINE_ATTRIBUTE;


EXIT_PK_NAMESPACE;

#endif /* __DRMBLACKBOXDEVCERTS_H__ */

