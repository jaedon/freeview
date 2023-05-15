/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmprndprotocol.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDReceiverSupported(DRM_VOID) { return FALSE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDReceiverUnsupported(DRM_VOID) { return TRUE; }

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_BeginSession(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_RegistrationRequest_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_opt                                                    const DRM_ID                                           *f_pPreviousSessionID,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbReqMessage )                            DRM_BYTE                                        **f_ppbReqMessage,
    __out                                                             DRM_DWORD                                        *f_pcbReqMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_RegistrationResponse_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbRespMessage )                              const DRM_BYTE                                         *f_pbRespMessage,
    __in                                                              DRM_DWORD                                         f_cbRespMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated,
    __out                                                             DRM_PRND_PROXIMITY_DETECTION_TYPE                *f_peProximityDetectionType,
    __deref_out_ecount( *f_pcbTransmitterProximityDetectionChannel )  DRM_BYTE                                        **f_ppbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pcbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_RegistrationError_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbErrMessage )                               const DRM_BYTE                                         *f_pbErrMessage,
    __in                                                              DRM_DWORD                                         f_cbErrMessage,
    __out                                                             DRM_RESULT                                       *f_pdrResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_ProximityDetectionStart_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDStartMessage )                        DRM_BYTE                                        **f_ppbPDStartMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDStartMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_ProximityDetectionChallenge_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbPDChlgMessage )                            const DRM_BYTE                                         *f_pbPDChlgMessage,
    __in                                                              DRM_DWORD                                         f_cbPDChlgMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDRespMessage )                         DRM_BYTE                                        **f_ppbPDRespMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDRespMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_ProximityDetectionResult_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbPDResultMessage )                          const DRM_BYTE                                         *f_pbPDResultMessage,
    __in                                                              DRM_DWORD                                         f_cbPDResultMessage,
    __out                                                             DRM_RESULT                                       *f_pdrPDResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_LicenseRequest_Generate(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in                                                        const DRM_GUID                                         *f_pguidRequestedAction,
    __in_opt                                                    const DRM_GUID                                         *f_pguidRequestedActionQualifier,
    __in                                                              DRM_PRND_CONTENT_IDENTIFIER_TYPE                  f_eContentIdentifierType,
    __in_bcount( f_cbContentIdentifier )                        const DRM_BYTE                                         *f_pbContentIdentifier,
    __in                                                              DRM_DWORD                                         f_cbContentIdentifier,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicReqMessage )                         DRM_BYTE                                        **f_ppbLicReqMessage,
    __out                                                             DRM_DWORD                                        *f_pcbLicReqMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_LicenseTransmit_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbLicTransmitMessage )                       const DRM_BYTE                                         *f_pbLicTransmitMessage,
    __in                                                              DRM_DWORD                                         f_cbLicTransmitMessage,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated,
    __inout_ecount_opt( *f_pcReceiverBoundOrLeafXMRLicenses )         DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __inout_ecount_opt( *f_pcReceiverBoundOrLeafXMRLicenses )         DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __inout                                                           DRM_DWORD                                        *f_pcReceiverBoundOrLeafXMRLicenses,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_DWORD                                        *f_pcbLargestLicense,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_LicenseError_Process(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbErrMessage )                               const DRM_BYTE                                         *f_pbErrMessage,
    __in                                                              DRM_DWORD                                         f_cbErrMessage,
    __out                                                             DRM_RESULT                                       *f_pdrResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Receiver_EndSession(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

