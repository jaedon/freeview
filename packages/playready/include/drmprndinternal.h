/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMPRNDINTERNAL_H_
#define _DRMPRNDINTERNAL_H_ 1

#include <drmerr.h>
#include <drmblackbox.h>
#include <drmrevocation.h>
#include <drmprndprotocoltypes.h>

ENTER_PK_NAMESPACE;

typedef struct __tagDRM_PRND_INTERNAL_SIGNATURE_CONTEXT
{
    DRM_PRND_TRANSMITTER_CONTEXT    *poPrndTransmitterContext;
    DRM_PRND_RECEIVER_CONTEXT       *poPrndReceiverContext;
} DRM_PRND_INTERNAL_SIGNATURE_CONTEXT;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_ProcessCertRevInfoCustomData(
    __inout_opt                                             DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout_opt                                             DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCertificate,
    __in_opt                                          const DRM_ID                                           *f_pRemoteCustomDataTypeID,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteCustomData,
    __in_opt                                                Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                 DRM_VOID                                         *f_pvDataCallbackContext,
    __out_opt                                               DRM_PUBLIC_KEY_CONTEXT                           *f_poECC256PubKeyPrndEncryptRemote,
    __out_opt                                               DRM_PUBLIC_KEY_CONTEXT                           *f_poECC256PubKeyContentEncryptRemote,
    __in_ecount_opt( f_cbToVerifyData )               const DRM_BYTE                                         *f_pbToVerifyData,
    __in                                                    DRM_DWORD                                         f_cbToVerifyData,
    __in_opt                                          const SIGNATURE_P256                                   *f_pToVerifySig,
    __in_opt                                          const DRM_ID                                           *f_pSessionID,
    __in_opt                                                DRM_XB_BYTEARRAY                                 *f_pxbbaRemoteRevInfo,
    __out_opt                                               DRM_BOOL                                         *f_pfRIVUpdated ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_FetchCertAndRevInfo(
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __inout                                                 DRM_REVOCATIONSTORE_CONTEXT                      *f_pRevocationStoreContext,
    __deref_inout_bcount( *f_pcbStack )                     DRM_BYTE                                        **f_ppbStack,
    __inout                                                 DRM_DWORD                                        *f_pcbStack,
    __in_opt                                                DRM_KEYFILE_CONTEXT                              *f_pKeyFileContext,
    __out_ecount_opt( *f_pcbLocalCertificate )              DRM_BYTE                                        **f_ppbLocalCertificate,
    __out_opt                                               DRM_DWORD                                        *f_pcbLocalCertificate,
    __out_ecount( *f_pcbLocalRevInfo )                      DRM_BYTE                                        **f_ppbLocalRevInfo,
    __out                                                   DRM_DWORD                                        *f_pcbLocalRevInfo,
    __out_opt                                               DRM_DWORD                                        *f_pdwRIV ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_CheckRemoteCertificateExpiration(
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __in                                                    DRM_DWORD                                         f_dwRemoteCertificateExpiration ) DRM_NO_INLINE_ATTRIBUTE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const")
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_FormatSignatureCallback(
    __in_ecount( 1 )                                        DRM_VOID                                         *f_pvSignatureContext,
    __in                                                    DRM_PRND_SIGNATURE_OPERATION                      f_eSignatureOperation,
    __in_bcount( f_cbData )                           const DRM_BYTE                                         *f_pbData,
    __in                                                    DRM_DWORD                                         f_cbData,
    __out_bcount( f_cbSignatureBuffer )                     DRM_BYTE                                         *f_pbSignatureBuffer,
    __in                                                    DRM_DWORD                                         f_cbSignatureBuffer,
    __in                                                    DRM_DWORD                                         f_iSignatureBuffer,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaMIMKEncryptedWithReceiverPubkey,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaReceiverCertificate ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

EXIT_PK_NAMESPACE;

#endif /* _DRMPRNDINTERNAL_H_ */

