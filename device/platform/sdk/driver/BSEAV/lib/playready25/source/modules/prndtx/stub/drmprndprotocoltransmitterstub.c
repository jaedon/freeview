/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmprndprotocol.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDTransmitterSupported(DRM_VOID) { return FALSE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDTransmitterUnsupported(DRM_VOID) { return TRUE; }

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Tx_RebindLicenseKeysToNewECCKey(
    __inout_ecount( 1 )                               DRM_BB_CONTEXT                  *f_poBBXContext,
    __in                                              DRM_REVOCATIONSTORE_CONTEXT     *f_poRevStoreContext,
    __in_ecount( 1 )                            const DRM_BINDING_INFO                *f_pRootmostLicenseBindingInfo,
    __in                                        const DRM_PUBLIC_KEY_CONTEXT          *f_poECC256PubKeyTarget,
    __in_ecount_opt( f_cbCertificateTarget )    const DRM_BYTE                        *f_pbCertificateTarget,
    __in                                              DRM_DWORD                        f_cbCertificateTarget,
    __in_ecount_opt( f_cbCertificateTargetCRL ) const DRM_BYTE                        *f_pbCertificateTargetCRL,
    __in                                              DRM_DWORD                        f_cbCertificateTargetCRL,
    __in                                        const DRM_ID                          *f_pidSession,
    __out_ecount( 1 )                                 OEM_CRYPTO_HANDLE               *f_phandleAESIntegrityKey,
    __out_ecount( 1 )                                 CIPHERTEXT_P256                 *f_poEncryptedAESKeyPair )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_BeginSession(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_RegistrationRequest_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbReqMessage )                               const DRM_BYTE                                         *f_pbReqMessage,
    __in                                                              DRM_DWORD                                         f_cbReqMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_PRND_PROXIMITY_DETECTION_TYPE                *f_peProximityDetectionType,
    __deref_out_ecount( *f_pcbTransmitterProximityDetectionChannel )  DRM_BYTE                                        **f_ppbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pcbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_RegistrationResponse_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbRespMessage )                           DRM_BYTE                                        **f_ppbRespMessage,
    __out                                                             DRM_DWORD                                        *f_pcbRespMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_RegistrationError_Generate(
    __inout_opt                                                       DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbErrMessage )                            DRM_BYTE                                        **f_ppbErrMessage,
    __out                                                             DRM_DWORD                                        *f_pcbErrMessage,
    __in                                                              DRM_RESULT                                        f_drResult )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ProximityDetectionResult_Generate(
    __inout_opt                                                       DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pSessionID,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDResultMessage,
    __in                                                              DRM_RESULT                                        f_drResult )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ProximityDetectionStart_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbPDStartMessage )                           const DRM_BYTE                                         *f_pbPDStartMessage,
    __in                                                              DRM_DWORD                                         f_cbPDStartMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDChlgMessage )                         DRM_BYTE                                        **f_ppbPDChlgMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDChlgMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ProximityDetectionResponse_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbPDRespMessage )                            const DRM_BYTE                                         *f_pbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_cbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_LicenseRequest_Process(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_bcount( f_cbLicReqMessage )                            const DRM_BYTE                                         *f_pbLicReqMessage,
    __in                                                              DRM_DWORD                                         f_cbLicReqMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __in_opt                                                          Drm_Prnd_Content_Identifier_Callback              f_pfnContentIdentifierCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvContentIdentifierCallbackContext,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_LicenseTransmit_Generate(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )                  DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __in_ecount( f_cReceiverBoundOrLeafXMRLicenses )            const DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __in                                                              DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicTransmitMessage )                    DRM_BYTE                                        **f_ppbLicTransmitMessage,
    __out                                                             DRM_DWORD                                        *f_pcbLicTransmitMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_ClearPRNDStore(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_LicenseError_Generate(
    __inout_opt                                                       DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __in_opt                                                    const DRM_ID                                           *f_pSessionID,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbErrMessage )                            DRM_BYTE                                        **f_ppbErrMessage,
    __out                                                             DRM_DWORD                                        *f_pcbErrMessage,
    __in                                                              DRM_RESULT                                        f_drResult )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_EndSession(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Transmitter_IsSessionStillValid(
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __out                                                             DRM_BOOL                                         *f_pfSessionStillValid )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

