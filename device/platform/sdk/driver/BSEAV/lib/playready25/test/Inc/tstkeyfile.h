/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TSTKEYFILE_H_
#define _TSTKEYFILE_H_

#include <drmkeyfiletypes.h>
#include <drmkeyfileformat.h>
#include <drmstkalloc.h>
#include <drmdevcertcache.h>
#include <oemcommon.h>
#include <oemkeyfile.h>

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL TST_KF_Open(
    __inout  DRM_KEYFILE_CONTEXT *f_pKeyFile,
    __in     DRM_DWORD            f_fCreateIfNotExists,
    __in     DRM_DWORD            f_dwAccessMode ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL TST_KF_LoadFromFile(
    __inout   DRM_KEYFILE_CONTEXT        *f_pKeyFile,
    __in      DRM_BOOL                    f_fValidateSignature ) DRM_NO_INLINE_ATTRIBUTE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_KEYFILE_CONTEXT* should not be const.")
DRM_API DRM_RESULT DRM_CALL TST_KF_GetCertificate(
    __in                            DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                            DRM_KF_CERT_TYPE       f_eCertType,
    __out_ecount(*f_pcbCertData)    DRM_BYTE             **f_ppbCertData,
    __out_ecount(1)                 DRM_DWORD             *f_pcbCertData );
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL TST_KF_GetPrivateKey(
    __in                        const DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                              DRM_KF_KEY_TYPE        f_eKeyType,
    __in_bcount_opt(f_cbPubKey) const DRM_BYTE              *f_pbPubKey,
    __in                              DRM_DWORD              f_cbPubKey,
    __out_ecount(1)                   DRM_ENCRYPTED_KEY     *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL TST_DDC_GetBinaryDeviceCertificate(
    __in_opt                               DRM_VOID            *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE            *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD           *f_pcbBinaryDevCertChain,
    __in                                   const DRM_DWORD      f_dwFlags,
    __inout                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx );

EXIT_PKTEST_NAMESPACE;

#endif /* _TSTKEYFILE_H_ */

