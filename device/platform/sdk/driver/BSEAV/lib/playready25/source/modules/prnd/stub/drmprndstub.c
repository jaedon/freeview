/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmprnd.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDSupported(DRM_VOID) { return DRM_PRND_IsPRNDTransmitterSupported() || DRM_PRND_IsPRNDReceiverSupported(); }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_PRND_IsPRNDUnsupported(DRM_VOID) { return !DRM_PRND_IsPRNDSupported(); }

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_REQUEST_MESSAGE         *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_RESPONSE_MESSAGE        *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_ERROR_MESSAGE    *f_pMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE    *f_pMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                          *f_pbStack,
    __in                                  DRM_DWORD                                          f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                          *f_pbMessage,
    __in                                  DRM_DWORD                                          f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE    *f_pMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE *f_pMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE   *f_pMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_REQUEST_MESSAGE              *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_TRANSMIT_MESSAGE             *f_pMessage,
    __in                                  DRM_PRND_FORMAT_SIGNATURE_CB                   f_pfnSignatureCallback,
    __in                                  DRM_VOID                                      *f_pvSignatureContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_bcount( f_cbMessage )   const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_ERROR_MESSAGE                *f_pMessage )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_Tx_SignWithAESOMAC(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT                  *f_poBBXContext,
    __in_ecount( 1 )                        OEM_CRYPTO_HANDLE                f_handleAESIntegrityKey,
    __in_bcount( f_cbData )           const DRM_BYTE                        *f_pbData,
    __in                                    DRM_DWORD                        f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )        DRM_BYTE                         f_rgbSignature[DRM_AES_BLOCKLEN] )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_TxRx_GetMessageType(
    __inout_bcount( f_cbUnknownMessage )                        const DRM_BYTE                                         *f_pbUnknownMessage,
    __in                                                              DRM_DWORD                                         f_cbUnknownMessage,
    __out                                                             DRM_PRND_MESSAGE_TYPE                            *f_peMessageType )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_OpenPRNDStore(
    __in_opt                                                DRM_VOID                                         *f_pOEMContext,
    __inout_ecount( 1 )                                     DRM_LICSTORE_CONTEXT                             *f_poLSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_DST                                          *f_poDSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_EST_CONTEXT                                  *f_poESTContextPRNDStore,
    __inout_ecount( f_cbPRNDStore )                   const DRM_BYTE                                         *f_pbPRNDStore,
    __in                                                    DRM_DWORD                                         f_cbPRNDStore,
    __out                                                   DRM_BOOL                                         *f_pfOpened )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_PRND_ClosePRNDStore(
    __inout_ecount( 1 )                                     DRM_LICSTORE_CONTEXT                             *f_poLSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_DST                                          *f_poDSTContextPRNDStore,
    __inout_ecount( 1 )                                     DRM_EST_CONTEXT                                  *f_poESTContextPRNDStore,
    __inout_ecount( f_cbPRNDStore )                         DRM_BYTE                                         *f_pbPRNDStore,
    __in                                                    DRM_DWORD                                         f_cbPRNDStore,
    __out                                                   DRM_BOOL                                         *f_pfOpened )
{
    return;
}

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
    __out_opt                                               DRM_BOOL                                         *f_pfRIVUpdated )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRND_INTERNAL_FormatSignatureCallback(
    __in_ecount( 1 )                                        DRM_VOID                                         *f_pvSignatureContext,
    __in                                                    DRM_PRND_SIGNATURE_OPERATION                      f_eSignatureOperation,
    __in_bcount( f_cbData )                           const DRM_BYTE                                         *f_pbData,
    __in                                                    DRM_DWORD                                         f_cbData,
    __out_bcount( f_cbSignatureBuffer )                     DRM_BYTE                                         *f_pbSignatureBuffer,
    __in                                                    DRM_DWORD                                         f_cbSignatureBuffer,
    __in                                                    DRM_DWORD                                         f_iSignatureBuffer,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaMIMKEncryptedWithReceiverPubkey,
    __in_opt                                          const DRM_XB_BYTEARRAY                                 *f_pxbbaReceiverCertificate )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_PRND_TxRx_IncrementLMID(
    __inout_ecount( 1 )                                               DRM_GUID                                         *f_pguidLMID )
{
    return;
}

EXIT_PK_NAMESPACE_CODE;

