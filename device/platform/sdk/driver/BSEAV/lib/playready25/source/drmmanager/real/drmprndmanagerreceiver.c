/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDMANAGERRECEIVER_C
#include <drmmanager.h>
#include <drmprndprotocol.h>
#include <drmcontract.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

#define COPY_KID( pKid, pXmrLic ) do {                                                                                                                                                      \
        ChkBOOL( XMR_IS_CONTENT_KEY_VALID( (pXmrLic) ), DRM_E_INVALID_LICENSE );                                                                                                            \
        OEM_SECURE_MEMCPY( (pKid), (pXmrLic)->containerOuter.containerKeys.ContentKey.pbguidKeyID + (pXmrLic)->containerOuter.containerKeys.ContentKey.iguidKeyID, SIZEOF( DRM_KID ) );     \
    } while(FALSE)
#define COPY_LID( pLid, pXmrLic ) do {                                                                          \
        ChkBOOL( XMR_IS_RID_VALID( (pXmrLic) ), DRM_E_INVALID_LICENSE );                                        \
        OEM_SECURE_MEMCPY( (pLid), (pXmrLic)->pbRightsIdBuffer + (pXmrLic)->iRightsId, SIZEOF( DRM_LID ) );     \
    } while(FALSE)
#define IS_LEAF( pXmrLic )         ( XMR_IS_UPLINK_KID_VALID( pXmrLic ) || XMR_IS_UPLINKX_VALID( pXmrLic ) )
#define GET_ISSUE_DATE( pXmrLic )  ( XMR_IS_ISSUEDATE_VALID( pXmrLic ) ? (pXmrLic)->containerOuter.containerGlobalPolicies.IssueDate.dwValue : MAX_UNSIGNED_TYPE( DRM_DWORD ) )

static DRM_VOID _RefreshPrndReceiverContextFromAppContext(
    __in                                  DRM_APP_CONTEXT                               *f_poAppContext,
    __inout                               DRM_PRND_RECEIVER_CONTEXT                     *f_poPrndReceiverContext )
{
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRMASSERT( f_poAppContext  != NULL );
    __analysis_assume( f_poAppContext != NULL );
    DRMASSERT( f_poPrndReceiverContext != NULL );
    __analysis_assume( f_poPrndReceiverContext != NULL );

    f_poPrndReceiverContext->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
    f_poPrndReceiverContext->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
    f_poPrndReceiverContext->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
    f_poPrndReceiverContext->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
    f_poPrndReceiverContext->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
    f_poPrndReceiverContext->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;
    f_poPrndReceiverContext->pfPRNDLSTOpened            = &poAppContextInternal->fPRNDRxLSTOpened;
    f_poPrndReceiverContext->pLSTContextPRNDStore       = &poAppContextInternal->oLSTContextPRNDRxStore;
    f_poPrndReceiverContext->pDSTContextPRNDStore       = &poAppContextInternal->oDSTContextPRNDRxStore;
    f_poPrndReceiverContext->pESTContextPRNDStore       = &poAppContextInternal->oESTContextPRNDRxStore;
    f_poPrndReceiverContext->pbPRNDStore                = poAppContextInternal->rgbPRNDRxStore;
    f_poPrndReceiverContext->cbPRNDStore                = SIZEOF( poAppContextInternal->rgbPRNDRxStore );
}

/*
** Receiver should hold a reference to f_poNewPrndReceiverContext until after Drm_Prnd_Receiver_EndSession.
** After that, Receiver can safely delete f_poNewPrndReceiverContext.
** The Transmitter must use an independent DRM_APP_CONTEXT and DRM_PRND_TRANSMITTER_CONTEXT for each session.
*/
DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_BeginSession(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in                                                              DRM_PRND_RECEIVER_CONTEXT                        *f_poNewPrndReceiverContext )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );

    ChkArg( f_poNewPrndReceiverContext != NULL );
    ChkArg( poAppContextInternal->poPrndReceiverContext == NULL );
    poAppContextInternal->poPrndReceiverContext = f_poNewPrndReceiverContext;

    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );
    ChkDR( DRM_PRND_Receiver_BeginSession( poAppContextInternal->poPrndReceiverContext ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Receiver_BeginSession, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_RegistrationRequest_Generate(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_opt                                                    const DRM_ID                                           *f_pPreviousSessionID,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbReqMessage )                            DRM_BYTE                                        **f_ppbReqMessage,
    __out                                                             DRM_DWORD                                        *f_pcbReqMessage )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbReqMessage           = NULL;
    DRM_DWORD                    cbReqMessage           = 0;

    ChkArg( f_poAppContext  != NULL );
    ChkArg( f_ppbReqMessage != NULL );
    ChkArg( f_pcbReqMessage != NULL );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );

    DRM_CHECK_CUSTOM_DATA_TYPE( f_pCustomDataTypeID );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS || f_dwFlags == DRM_PRND_FLAG_REGISTRATION_REQUEST_SUPPORTS_LICENSE_PUSH );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_RegistrationRequest_Generate(
        poAppContextInternal->poPrndReceiverContext,
        f_pPreviousSessionID,
        f_pCustomDataTypeID,
        f_pbCustomData,
        f_cbCustomData,
        f_dwFlags,
        &pbReqMessage,
        &cbReqMessage ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbReqMessage = cbReqMessage;
    *f_ppbReqMessage = pbReqMessage;
    pbReqMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbReqMessage );
    ChkECC( ECC_Drm_Prnd_Receiver_RegistrationRequest_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_RegistrationResponse_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __inout_bcount( f_cbRespMessage )                           const DRM_BYTE                                         *f_pbRespMessage,
    __in                                                              DRM_DWORD                                         f_cbRespMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out_ecount( 1 )                                                 DRM_ID                                           *f_pSessionID,
    __out                                                             DRM_PRND_PROXIMITY_DETECTION_TYPE                *f_peProximityDetectionType,
    __deref_out_ecount( *f_pcbTransmitterProximityDetectionChannel )  DRM_BYTE                                        **f_ppbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pcbTransmitterProximityDetectionChannel,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                                             = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal                           = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BOOL                     fRIVUpdated                                    = FALSE;
    DRM_BYTE                    *pbTransmitterProximityDetectionChannelWeakRef  = NULL;
    DRM_BYTE                    *pbTransmitterProximityDetectionChannel         = NULL;
    DRM_DWORD                    cbTransmitterProximityDetectionChannel         = 0;

    ChkArg( f_poAppContext                              != NULL );
    ChkArg( f_pbRespMessage                             != NULL );
    ChkArg( f_cbRespMessage                              > 0    );
    ChkArg( f_pSessionID                                != NULL );
    ChkArg( f_peProximityDetectionType                  != NULL );
    ChkArg( f_ppbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pcbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pdwFlags                                  != NULL );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_RegistrationResponse_Process(
        poAppContextInternal->poPrndReceiverContext,
        f_pbRespMessage,
        f_cbRespMessage,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        &fRIVUpdated,
        f_peProximityDetectionType,
        &pbTransmitterProximityDetectionChannelWeakRef,
        &cbTransmitterProximityDetectionChannel,
        f_pdwFlags ) );

    DRMASSERT( cbTransmitterProximityDetectionChannel > 0 );
    ChkMem( pbTransmitterProximityDetectionChannel = Drm_Prnd_MemAlloc( cbTransmitterProximityDetectionChannel ) );
    OEM_SECURE_MEMCPY(
        pbTransmitterProximityDetectionChannel,
        pbTransmitterProximityDetectionChannelWeakRef,
        cbTransmitterProximityDetectionChannel );

    if( fRIVUpdated )
    {
        ChkDR( DRM_RVK_UpdateRevocationVersionsCache(
            &poAppContextInternal->oRevContext,
            &poAppContextInternal->oBlackBoxContext.CryptoContext,
            &poAppContextInternal->idCRLsCurrent,
            poAppContextInternal->pbRevocationBuffer,
            poAppContextInternal->cbRevocationBuffer,
            NULL ) );

        OEM_SECURE_MEMCPY(
            &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
            &poAppContextInternal->idCRLsCurrent,
            SIZEOF( poAppContextInternal->oLicEvalContext.idCRLsCurrent ) );
    }

    /* Only hand out the session ID if everything succeeds */
    OEM_SECURE_MEMCPY(
        f_pSessionID,
        &poAppContextInternal->poPrndReceiverContext->oPrndReceiverSessionContext.SessionID,
        SIZEOF( poAppContextInternal->poPrndReceiverContext->oPrndReceiverSessionContext.SessionID ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbTransmitterProximityDetectionChannel = cbTransmitterProximityDetectionChannel;
    *f_ppbTransmitterProximityDetectionChannel = pbTransmitterProximityDetectionChannel;
    pbTransmitterProximityDetectionChannel     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbTransmitterProximityDetectionChannel );
    ChkECC( ECC_Drm_Prnd_Receiver_RegistrationResponse_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_RegistrationError_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbErrMessage )                               const DRM_BYTE                                         *f_pbErrMessage,
    __in                                                              DRM_DWORD                                         f_cbErrMessage,
    __out                                                             DRM_RESULT                                       *f_pdrResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext  != NULL );
    ChkArg( f_pbErrMessage  != NULL );
    ChkArg( f_cbErrMessage   > 0    );
    ChkArg( f_pdrResult     != NULL );
    ChkArg( f_pdwFlags      != NULL );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_RegistrationError_Process(
        poAppContextInternal->poPrndReceiverContext,
        f_pbErrMessage,
        f_cbErrMessage,
        f_pdrResult,
        f_pdwFlags ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Receiver_RegistrationError_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_ProximityDetectionStart_Generate(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDStartMessage )                        DRM_BYTE                                        **f_ppbPDStartMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDStartMessage )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbPDStartMessage       = NULL;
    DRM_DWORD                    cbPDStartMessage       = 0;

    ChkArg( f_poAppContext       != NULL );
    ChkArg( f_ppbPDStartMessage != NULL );
    ChkArg( f_pcbPDStartMessage != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_ProximityDetectionStart_Generate(
        poAppContextInternal->poPrndReceiverContext,
        f_dwFlags,
        &pbPDStartMessage,
        &cbPDStartMessage ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbPDStartMessage = cbPDStartMessage;
    *f_ppbPDStartMessage = pbPDStartMessage;
    pbPDStartMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbPDStartMessage );
    ChkECC( ECC_Drm_Prnd_Receiver_ProximityDetectionStart_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_ProximityDetectionChallenge_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbPDChlgMessage )                            const DRM_BYTE                                         *f_pbPDChlgMessage,
    __in                                                              DRM_DWORD                                         f_cbPDChlgMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDRespMessage )                         DRM_BYTE                                        **f_ppbPDRespMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDRespMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbPDRespMessage           = NULL;
    DRM_DWORD                    cbPDRespMessage           = 0;

    ChkArg( f_poAppContext      != NULL );
    ChkArg( f_pbPDChlgMessage   != NULL );
    ChkArg( f_cbPDChlgMessage    > 0    );
    ChkArg( f_ppbPDRespMessage  != NULL );
    ChkArg( f_pcbPDRespMessage  != NULL );
    ChkArg( f_pdwFlags          != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_ProximityDetectionChallenge_Process(
        poAppContextInternal->poPrndReceiverContext,
        f_pbPDChlgMessage,
        f_cbPDChlgMessage,
        f_dwFlags,
        &pbPDRespMessage,
        &cbPDRespMessage,
        f_pdwFlags ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbPDRespMessage = cbPDRespMessage;
    *f_ppbPDRespMessage = pbPDRespMessage;
    pbPDRespMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbPDRespMessage );
    ChkECC( ECC_Drm_Prnd_Receiver_ProximityDetectionChallenge_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_ProximityDetectionResult_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbPDResultMessage )                          const DRM_BYTE                                         *f_pbPDResultMessage,
    __in                                                              DRM_DWORD                                         f_cbPDResultMessage,
    __out                                                             DRM_RESULT                                       *f_pdrPDResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext      != NULL );
    ChkArg( f_pbPDResultMessage != NULL );
    ChkArg( f_cbPDResultMessage  > 0    );
    ChkArg( f_pdrPDResult       != NULL );
    ChkArg( f_pdwFlags          != NULL );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_ProximityDetectionResult_Process(
        poAppContextInternal->poPrndReceiverContext,
        f_pbPDResultMessage,
        f_cbPDResultMessage,
        f_pdrPDResult,
        f_pdwFlags ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Receiver_ProximityDetectionResult_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_LicenseRequest_Generate(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
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
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbLicReqMessage           = NULL;
    DRM_DWORD                    cbLicReqMessage           = 0;

    ChkArg( f_poAppContext          != NULL );
    ChkArg( f_pguidRequestedAction  != NULL );
    ChkArg( f_pbContentIdentifier   != NULL );
    ChkArg( f_cbContentIdentifier    > 0    );
    ChkArg( f_ppbLicReqMessage != NULL );
    ChkArg( f_pcbLicReqMessage != NULL );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );

    DRM_CHECK_CUSTOM_DATA_TYPE( f_pCustomDataTypeID );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_LicenseRequest_Generate(
        poAppContextInternal->poPrndReceiverContext,
        f_pguidRequestedAction,
        f_pguidRequestedActionQualifier,
        f_eContentIdentifierType,
        f_pbContentIdentifier,
        f_cbContentIdentifier,
        f_pCustomDataTypeID,
        f_pbCustomData,
        f_cbCustomData,
        f_dwFlags,
        &pbLicReqMessage,
        &cbLicReqMessage ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbLicReqMessage = cbLicReqMessage;
    *f_ppbLicReqMessage = pbLicReqMessage;
    pbLicReqMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbLicReqMessage );
    ChkECC( ECC_Drm_Prnd_Receiver_LicenseRequest_Generate, dr );
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_GetLicenses(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_bcount( f_cbLicTransmitMessage )                       const DRM_BYTE                                         *f_pbLicTransmitMessage,
    __in                                                              DRM_DWORD                                         f_cbLicTransmitMessage,
    __out                                                             DRM_BOOL                                         *f_pfRIVUpdated,
    __deref_out_ecount( *f_pcReceiverBoundOrLeafXMRLicenses )         DRM_BYTE                                       ***f_prgpbReceiverBoundOrLeafXMRLicense,
    __deref_out_ecount( *f_pcReceiverBoundOrLeafXMRLicenses )         DRM_DWORD                                       **f_prgcbReceiverBoundOrLeafXMRLicense,
    __out                                                             DRM_DWORD                                        *f_pcReceiverBoundOrLeafXMRLicenses,
    __inout_bcount_opt( *f_pcbOpaqueBuffer )                          DRM_BYTE                                        **f_ppbOpaqueBufferRemaining,
    __inout                                                           DRM_DWORD                                        *f_pcbOpaqueBufferRemaining,
    __inout                                                           DRM_DWORD                                        *f_pcbLargestLicense,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_DWORD                                        *f_pcbOpaqueBufferNeeded,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT  dr                              = DRM_SUCCESS;
    DRM_DWORD   cbOpaqueBufferNeeded            = 0;
    DRM_DWORD   cbLargestNewLicense             = 0;
    DRM_DWORD   cReceiverBoundOrLeafXMRLicenses = 0;
    DRM_DWORD   dwResult                        = 0;

    DRMASSERT( f_poPrndReceiverContext                 != NULL );
    DRMASSERT( f_pbLicTransmitMessage                  != NULL );
    DRMASSERT( f_cbLicTransmitMessage                   > 0    );
    DRMASSERT( f_pfRIVUpdated                          != NULL );
    DRMASSERT( f_prgpbReceiverBoundOrLeafXMRLicense    != NULL );
    DRMASSERT( f_prgcbReceiverBoundOrLeafXMRLicense    != NULL );
    DRMASSERT( f_pcReceiverBoundOrLeafXMRLicenses      != NULL );
    DRMASSERT( f_ppbOpaqueBufferRemaining              != NULL );
    DRMASSERT( f_pcbOpaqueBufferRemaining              != NULL );
    DRMASSERT( f_pcbLargestLicense                     != NULL );
    DRMASSERT( f_pcbOpaqueBufferNeeded                 != NULL );
    DRMASSERT( f_pdwFlags                              != NULL );
    __analysis_assume( f_poPrndReceiverContext                 != NULL );
    __analysis_assume( f_pbLicTransmitMessage                  != NULL );
    __analysis_assume( f_cbLicTransmitMessage                   > 0    );
    __analysis_assume( f_pfRIVUpdated                          != NULL );
    __analysis_assume( f_prgpbReceiverBoundOrLeafXMRLicense    != NULL );
    __analysis_assume( f_prgcbReceiverBoundOrLeafXMRLicense    != NULL );
    __analysis_assume( f_pcReceiverBoundOrLeafXMRLicenses      != NULL );
    __analysis_assume( f_ppbOpaqueBufferRemaining              != NULL );
    __analysis_assume( f_pcbOpaqueBufferRemaining              != NULL );
    __analysis_assume( f_pcbLargestLicense                     != NULL );
    __analysis_assume( f_pcbOpaqueBufferNeeded                 != NULL );
    __analysis_assume( f_pdwFlags                              != NULL );

    dr = DRM_PRND_Receiver_LicenseTransmit_Process(
        f_poPrndReceiverContext,
        f_pbLicTransmitMessage,
        f_cbLicTransmitMessage,
        f_pfRIVUpdated,
        NULL,
        NULL,
        &cReceiverBoundOrLeafXMRLicenses,
        NULL,   /* Do not pass f_pfnDataCallback/f_pvDataCallbackContext down or caller will receive callback twice. */
        NULL,
        &cbLargestNewLicense,
        f_pdwFlags );
    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );
    dr = DRM_SUCCESS;

    DRMASSERT( cbLargestNewLicense > 0 );
    *f_pcbLargestLicense = MAX( *f_pcbLargestLicense, cbLargestNewLicense );

    /*
    ** The opaque buffer has the following added to it in this function:
    ** 1. An array of BYTE*  each of which points to one license.
    ** 2. An array of DWORDs each of which contains the size of the corresponding license in the first array.
    */
    /* Items 1 and 2 */
    ChkDR( DRM_DWordAdd( SIZEOF( DRM_BYTE* ), SIZEOF( DRM_DWORD ), &dwResult ) );
    ChkDR( DRM_DWordMult( cReceiverBoundOrLeafXMRLicenses, dwResult, &cbOpaqueBufferNeeded ) );
    /* Item 3 */
    ChkDR( DRM_DWordAdd( cbOpaqueBufferNeeded, SIZEOF( DRM_XMR_LICENSE ), &cbOpaqueBufferNeeded ) );
    /* Item 4 */
    ChkDR( DRM_DWordAdd( cbOpaqueBufferNeeded, *f_pcbLargestLicense, &cbOpaqueBufferNeeded ) );
    ChkDR( DRM_DWordAdd( cbOpaqueBufferNeeded, SIZEOF( DRM_DWORD ), &cbOpaqueBufferNeeded ) );
    /* Item 5 */
    ChkDR( DRM_DWordMult( *f_pcbLargestLicense, DRM_XMR_MAX_UNPACK_ALLOC_FACTOR, &dwResult ) );
    ChkDR( DRM_DWordAdd( cbOpaqueBufferNeeded, dwResult, &cbOpaqueBufferNeeded ) );

    if( *f_pcbOpaqueBufferRemaining < cbOpaqueBufferNeeded )
    {
        *f_pcbOpaqueBufferNeeded = cbOpaqueBufferNeeded;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Use the opaque buffer for our license pointers */
    DRMASSERT( ( *f_pcbOpaqueBufferRemaining ) >= ( ( cReceiverBoundOrLeafXMRLicenses ) * SIZEOF( DRM_BYTE* ) ) );
    *f_prgpbReceiverBoundOrLeafXMRLicense = (DRM_BYTE**)*f_ppbOpaqueBufferRemaining;

    ChkDR( DRM_DWordMult( cReceiverBoundOrLeafXMRLicenses, SIZEOF( DRM_BYTE* ), &dwResult ) );
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbOpaqueBufferRemaining, dwResult, (DRM_DWORD_PTR*)f_ppbOpaqueBufferRemaining ) );
    ChkDR( DRM_DWordSub( *f_pcbOpaqueBufferRemaining, dwResult, f_pcbOpaqueBufferRemaining ) );

    /* Use the opaque buffer for our license sizes */
    DRMASSERT( ( *f_pcbOpaqueBufferRemaining ) >= ( ( cReceiverBoundOrLeafXMRLicenses ) * SIZEOF( DRM_DWORD ) ) );
    *f_prgcbReceiverBoundOrLeafXMRLicense = (DRM_DWORD*)*f_ppbOpaqueBufferRemaining;

    ChkDR( DRM_DWordMult( cReceiverBoundOrLeafXMRLicenses, SIZEOF( DRM_DWORD ), &dwResult ) );
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbOpaqueBufferRemaining, dwResult, (DRM_DWORD_PTR*)f_ppbOpaqueBufferRemaining ) );
    ChkDR( DRM_DWordSub( *f_pcbOpaqueBufferRemaining, dwResult, f_pcbOpaqueBufferRemaining ) );

    ChkDR( DRM_PRND_Receiver_LicenseTransmit_Process(
        f_poPrndReceiverContext,
        f_pbLicTransmitMessage,
        f_cbLicTransmitMessage,
        f_pfRIVUpdated,
        *f_prgpbReceiverBoundOrLeafXMRLicense,
        *f_prgcbReceiverBoundOrLeafXMRLicense,
        &cReceiverBoundOrLeafXMRLicenses,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        NULL,
        f_pdwFlags ) );

    *f_pcReceiverBoundOrLeafXMRLicenses = cReceiverBoundOrLeafXMRLicenses;

ErrorExit:
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_CheckStoreForDuplicateLicense(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_ecount( 1 )                                            const DRM_KID                                          *f_poKid,
    __in_ecount( 1 )                                            const DRM_LID                                          *f_poLid,
    __out_ecount( 1 )                                                 DRM_BOOL                                         *f_pfIsDuplicate )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_DWORD                                cbUnused   = 0;
    const DRM_PRND_RECEIVER_SESSION_CONTEXT *pSession   = NULL;

    DRMASSERT( f_poPrndReceiverContext != NULL );
    DRMASSERT( f_poKid                 != NULL );
    DRMASSERT( f_poLid                 != NULL );
    DRMASSERT( f_pfIsDuplicate         != NULL );
    __analysis_assume( f_poPrndReceiverContext != NULL );
    __analysis_assume( f_poKid                 != NULL );
    __analysis_assume( f_poLid                 != NULL );
    __analysis_assume( f_pfIsDuplicate         != NULL );

    pSession = &f_poPrndReceiverContext->oPrndReceiverSessionContext;

    *f_pfIsDuplicate = FALSE;

    dr = DRM_LST_GetLicense(
        f_poPrndReceiverContext->pLSTContextPRNDStore,
        f_poKid,
        f_poLid,
        NULL,
        NULL,
        &cbUnused );

    if( dr == DRM_E_LICENSE_NOT_FOUND )
    {
        /* License is new.  We're going to add it. */
        dr = DRM_SUCCESS;
    }
    else if( dr == DRM_E_BUFFERTOOSMALL )
    {
        /* License already present in the store. */
        dr = DRM_SUCCESS;
        *f_pfIsDuplicate = TRUE;
    }
    else
    {
        /* An error occurred. */
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_GetLargestExistingLicense(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __out                                                             DRM_DWORD                                        *f_pcbLargestExistingLicense )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    const DRM_PRND_RECEIVER_SESSION_CONTEXT *pSession   = NULL;
    DRM_LICSTOREENUM_CONTEXT                 oLicEnum   = {0};
    DRM_KID                                  oKid       = {0};
    DRM_LID                                  oLid       = {0};
    DRM_DWORD                                cbLicense  = 0;

    DRMASSERT( f_poPrndReceiverContext     != NULL );
    DRMASSERT( f_pcbLargestExistingLicense != NULL );
    __analysis_assume( f_poPrndReceiverContext     != NULL );
    __analysis_assume( f_pcbLargestExistingLicense != NULL );

    pSession = &f_poPrndReceiverContext->oPrndReceiverSessionContext;

    *f_pcbLargestExistingLicense = 0;

    ChkDR( DRM_LST_InitEnum(
        f_poPrndReceiverContext->pLSTContextPRNDStore,
        NULL,   /* Any KID */
        FALSE,  /* Not prioritized */
        &oLicEnum ) );

    do
    {
        *f_pcbLargestExistingLicense = MAX( *f_pcbLargestExistingLicense, cbLicense );
        cbLicense = 0;
        dr = DRM_LST_EnumNext(
            &oLicEnum,
            &oKid,
            &oLid,
            NULL,
            &cbLicense );
        if( dr == DRM_E_NOMORE )
        {
            /* We've finished enumerating through the store.  We're done. */
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
    } while( DRM_SUCCEEDED( dr ) );
    ChkDR( dr );

ErrorExit:
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_DeleteOneLicense(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_ecount( f_cbLicenseWithPriority )                            DRM_BYTE                                         *f_pbLicenseWithPriority,
    __in                                                              DRM_DWORD                                         f_cbLicenseWithPriority,
    __in_ecount( f_cbStack )                                          DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __inout                                                           DRM_XMR_LICENSE                                  *f_pXmrLicense )
{
    DRM_RESULT                               dr                     = DRM_SUCCESS;
    const DRM_PRND_RECEIVER_SESSION_CONTEXT *pSession               = NULL;
    DRM_LICSTOREENUM_CONTEXT                 oLicEnum               = {0};
    DRM_KID                                  oKid                   = {0};
    DRM_LID                                  oLid                   = {0};
    DRM_DWORD                                cbLicense              = 0;
    DRM_BOOL                                 fToDeleteFound         = FALSE;
    DRM_KID                                  oKidToDelete           = {0};
    DRM_LID                                  oLidToDelete           = {0};
    DRM_BOOL                                 fToDeleteIsLeaf        = FALSE;
    DRM_DWORD                                dwToDeleteIssueDate    = MAX_UNSIGNED_TYPE( DRM_DWORD );

    DRMASSERT( f_poPrndReceiverContext != NULL );
    __analysis_assume( f_poPrndReceiverContext != NULL );

    pSession = &f_poPrndReceiverContext->oPrndReceiverSessionContext;

    ChkDR( DRM_LST_InitEnum(
        f_poPrndReceiverContext->pLSTContextPRNDStore,
        NULL,   /* Any KID */
        FALSE,  /* Not prioritized */
        &oLicEnum ) );

    do
    {
        DRM_STACK_ALLOCATOR_CONTEXT  oStack = {0};

        OEM_SECURE_ZERO_MEMORY( f_pbStack, f_cbStack );
        OEM_SECURE_ZERO_MEMORY( f_pXmrLicense, SIZEOF( DRM_XMR_LICENSE ) );

        ChkDR( DRM_STK_Init( &oStack, f_pbStack, f_cbStack ) );

        dr = DRM_LST_EnumNext(
            &oLicEnum,
            &oKid,
            &oLid,
            NULL,
            &cbLicense );
        if( dr != DRM_E_NOMORE )
        {
            ChkDR( dr );

            /* f_cbLicenseWithPriority should be the largest license we could possibly be dealing with plus sizeof( DRM_DWORD ) */
            DRMASSERT( f_cbLicenseWithPriority > cbLicense );

            ChkDR( DRM_LST_GetLicense(
                f_poPrndReceiverContext->pLSTContextPRNDStore,
                &oKid,
                &oLid,
                NULL,
                f_pbLicenseWithPriority,
                &cbLicense ) );

            ChkDR( DRM_XMR_UnpackLicense(
                f_pbLicenseWithPriority,
                cbLicense,
                &oStack,
                f_pXmrLicense ) );

            if( fToDeleteFound )
            {
                /* Determine if this license is better to delete than the previous one. */

                if( !fToDeleteIsLeaf && IS_LEAF( f_pXmrLicense ) )
                {
                    /* Prioritize deleting leaf licenses over non-leaf. */
                    fToDeleteFound = FALSE;
                }
                else if( dwToDeleteIssueDate > GET_ISSUE_DATE( f_pXmrLicense ) )
                {
                    /* Prioritize deleting licenses with older issue dates over newer ones. */
                    fToDeleteFound = FALSE;
                }
            }

            /* This can't be an else clause: fToDeleteFound is modified above. */
            if( !fToDeleteFound )
            {
                /* Either we hadn't found a license to delete yet OR we decided this license is better to delete. */
                fToDeleteFound = TRUE;
                COPY_KID( &oKidToDelete, f_pXmrLicense );
                COPY_LID( &oLidToDelete, f_pXmrLicense );
                fToDeleteIsLeaf     = IS_LEAF( f_pXmrLicense );
                dwToDeleteIssueDate = GET_ISSUE_DATE( f_pXmrLicense );
            }
        }
    } while( dr == DRM_E_NOMORE );

    dr = DRM_SUCCESS;

    if( fToDeleteFound )
    {
        ChkDR( DRM_LST_DeleteLicense(
            f_poPrndReceiverContext->pLSTContextPRNDStore,
            &oKidToDelete,
            &oLidToDelete,
            NULL ) );
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_FreePRNDLicenseStoreSpace(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_ecount( f_cbLicenseWithPriority )                            DRM_BYTE                                         *f_pbLicenseWithPriority,
    __in                                                              DRM_DWORD                                         f_cbLicenseWithPriority,
    __in_ecount( f_cbStack )                                          DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __inout                                                           DRM_XMR_LICENSE                                  *f_pXmrLicense,
    __in                                                              DRM_DWORD                                         f_cLicensesRemainingIncludingCurrent )
{
    DRM_RESULT  dr  = DRM_SUCCESS;
    DRM_DWORD   idx = 0;

    DRMASSERT( f_poPrndReceiverContext              != NULL );
    DRMASSERT( f_cLicensesRemainingIncludingCurrent  > 0    );
    __analysis_assume( f_poPrndReceiverContext              != NULL );
    __analysis_assume( f_cLicensesRemainingIncludingCurrent  > 0    );

    /* Free f_cLicensesRemainingIncludingCurrent + 1 to leave a bit of space free.  Use <= to accomplish this. */
    for( idx = 0; idx <= f_cLicensesRemainingIncludingCurrent; idx++ )
    {
        ChkDR( _Prnd_Receiver_LicenseTransmit_DeleteOneLicense(
            f_poPrndReceiverContext,
            f_pbLicenseWithPriority,
            f_cbLicenseWithPriority,
            f_pbStack,
            f_cbStack,
            f_pXmrLicense ) );
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_StoreLicense(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __in_ecount( f_cbLicenseWithPriority )                            DRM_BYTE                                         *f_pbLicenseWithPriority,
    __in                                                              DRM_DWORD                                         f_cbLicenseWithPriority,
    __in_ecount( f_cbLicenseWithoutPriority )                   const DRM_BYTE                                         *f_pbLicenseWithoutPriority,
    __in                                                              DRM_DWORD                                         f_cbLicenseWithoutPriority,
    __in_ecount( 1 )                                            const DRM_KID                                          *f_poKid,
    __in_ecount( 1 )                                            const DRM_LID                                          *f_poLid,
    __in_ecount( f_cbStack )                                          DRM_BYTE                                         *f_pbStack,
    __in                                                              DRM_DWORD                                         f_cbStack,
    __inout                                                           DRM_XMR_LICENSE                                  *f_pXmrLicense,
    __in                                                              DRM_DWORD                                         f_cLicensesRemainingIncludingCurrent )
{
    DRM_RESULT                                 dr         = DRM_SUCCESS;
    const DRM_PRND_RECEIVER_SESSION_CONTEXT   *pSession   = NULL;

    DRMASSERT( f_poPrndReceiverContext      != NULL );
    DRMASSERT( f_pbLicenseWithPriority      != NULL );
    DRMASSERT( f_pbLicenseWithoutPriority   != NULL );
    DRMASSERT( f_cbLicenseWithoutPriority    > 0    );
    DRMASSERT( f_poKid                      != NULL );
    DRMASSERT( f_poLid                      != NULL );
    __analysis_assume( f_poPrndReceiverContext      != NULL );
    __analysis_assume( f_pbLicenseWithPriority      != NULL );
    __analysis_assume( f_pbLicenseWithoutPriority   != NULL );
    __analysis_assume( f_cbLicenseWithoutPriority    > 0    );
    __analysis_assume( f_poKid                      != NULL );
    __analysis_assume( f_poLid                      != NULL );

    pSession = &f_poPrndReceiverContext->oPrndReceiverSessionContext;

    OEM_SECURE_MEMCPY( f_pbLicenseWithPriority + SIZEOF( DRM_DWORD ), f_pbLicenseWithoutPriority, f_cbLicenseWithoutPriority );
    dr = DRM_LST_AddLicense(
        f_poPrndReceiverContext->pLSTContextPRNDStore,
        f_cbLicenseWithoutPriority,
        f_pbLicenseWithPriority,
        f_poKid,
        f_poLid,
        0 );

    if( dr == DRM_E_DST_STORE_FULL )
    {
        ChkDR( _Prnd_Receiver_LicenseTransmit_FreePRNDLicenseStoreSpace(
            f_poPrndReceiverContext,
            f_pbLicenseWithPriority,
            f_cbLicenseWithPriority,
            f_pbStack,
            f_cbStack,
            f_pXmrLicense,
            f_cLicensesRemainingIncludingCurrent ) );

        /* This time, storing the license should succeed. */
        OEM_SECURE_MEMCPY( f_pbLicenseWithPriority + SIZEOF( DRM_DWORD ), f_pbLicenseWithoutPriority, f_cbLicenseWithoutPriority );
        ChkDR( DRM_LST_AddLicense(
            f_poPrndReceiverContext->pLSTContextPRNDStore,
            f_cbLicenseWithoutPriority,
            f_pbLicenseWithPriority,
            f_poKid,
            f_poLid,
            0 ) );
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _Prnd_Receiver_LicenseTransmit_StoreLicenses(
    __inout                                                           DRM_PRND_RECEIVER_CONTEXT                        *f_poPrndReceiverContext,
    __deref_inout_ecount( *f_pcReceiverBoundOrLeafXMRLicenses )       DRM_BYTE                                        **f_rgpbReceiverBoundOrLeafXMRLicense,
    __deref_inout_ecount( *f_pcReceiverBoundOrLeafXMRLicenses ) const DRM_DWORD                                        *f_rgcbReceiverBoundOrLeafXMRLicense,
    __in                                                              DRM_DWORD                                         f_cReceiverBoundOrLeafXMRLicenses,
    __in                                                              DRM_DWORD                                         f_cbLargestLicense,
    __inout_bcount_opt( *f_pcbOpaqueBuffer )                          DRM_BYTE                                        **f_ppbOpaqueBufferRemaining,
    __inout                                                           DRM_DWORD                                        *f_pcbOpaqueBufferRemaining )
{
    DRM_RESULT                               dr             = DRM_SUCCESS;
    DRM_XMR_LICENSE                         *pXmrLicense    = NULL;
    DRM_DWORD                                idx            = 0;
    DRM_BYTE                                *pbStack        = NULL;
    DRM_DWORD                                cbStack        = 0;
    DRM_BYTE                                *pbStoreLicense = NULL;
    DRM_DWORD                                cbStoreLicense = 0;
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_LOCAL, "Prefast Noise: Session context locals should never be const")
    DRM_PRND_RECEIVER_SESSION_CONTEXT       *pSession       = NULL;
PREFAST_POP /* __WARNING_NONCONST_LOCAL */

    DRMASSERT( f_poPrndReceiverContext             != NULL );
    DRMASSERT( f_rgpbReceiverBoundOrLeafXMRLicense != NULL );
    DRMASSERT( f_rgcbReceiverBoundOrLeafXMRLicense != NULL );
    DRMASSERT( f_cReceiverBoundOrLeafXMRLicenses    > 0    );
    DRMASSERT( f_ppbOpaqueBufferRemaining          != NULL );
    DRMASSERT( f_pcbOpaqueBufferRemaining          != NULL );
    __analysis_assume( f_poPrndReceiverContext             != NULL );
    __analysis_assume( f_rgpbReceiverBoundOrLeafXMRLicense != NULL );
    __analysis_assume( f_rgcbReceiverBoundOrLeafXMRLicense != NULL );
    __analysis_assume( f_cReceiverBoundOrLeafXMRLicenses    > 0    );
    __analysis_assume( f_ppbOpaqueBufferRemaining          != NULL );
    __analysis_assume( f_pcbOpaqueBufferRemaining          != NULL );

    pSession = &f_poPrndReceiverContext->oPrndReceiverSessionContext;

    /*
    ** The opaque buffer has the following added to it in this function:
    ** 3. A single DRM_XMR_LICENSE structure for license parsing.
    ** 4. Sufficient space for the largest license plus SIZEOF( DRM_DWORD ), i.e. enough to call DRM_LST_AddLicense which requires space for priority.
    ** 5. Sufficient space for the largest license to be parsed with DRM_XMR_UnpackLicense.
    */
    DRMASSERT( *f_pcbOpaqueBufferRemaining >= SIZEOF( DRM_XMR_LICENSE ) );
    pXmrLicense = (DRM_XMR_LICENSE*)*f_ppbOpaqueBufferRemaining;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbOpaqueBufferRemaining, SIZEOF( DRM_XMR_LICENSE ), (DRM_DWORD_PTR*)f_ppbOpaqueBufferRemaining ) );
    ChkDR( DRM_DWordSub( *f_pcbOpaqueBufferRemaining, SIZEOF( DRM_XMR_LICENSE ), f_pcbOpaqueBufferRemaining ) );

    cbStoreLicense = f_cbLargestLicense + SIZEOF( DRM_DWORD );
    DRMASSERT( *f_pcbOpaqueBufferRemaining >= cbStoreLicense );
    pbStoreLicense = *f_ppbOpaqueBufferRemaining;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)*f_ppbOpaqueBufferRemaining, cbStoreLicense, (DRM_DWORD_PTR*)f_ppbOpaqueBufferRemaining ) );
    ChkDR( DRM_DWordSub( *f_pcbOpaqueBufferRemaining, cbStoreLicense, f_pcbOpaqueBufferRemaining ) );

    /* Use the rest of the opaque buffer for the license processing stack */
    pbStack = *f_ppbOpaqueBufferRemaining;
    cbStack = *f_pcbOpaqueBufferRemaining;
    *f_pcbOpaqueBufferRemaining = 0;

    for( idx = 0; idx < f_cReceiverBoundOrLeafXMRLicenses; idx++ )
    {
        DRM_KID                      oKid   = {0};
        DRM_LID                      oLid   = {0};
        DRM_STACK_ALLOCATOR_CONTEXT  oStack = {0};
        DRM_BOOL                     fDupe  = TRUE;

        OEM_SECURE_ZERO_MEMORY( pbStack, cbStack );
        OEM_SECURE_ZERO_MEMORY( pXmrLicense, SIZEOF( DRM_XMR_LICENSE ) );

        ChkDR( DRM_STK_Init( &oStack, pbStack, cbStack ) );

        ChkDR( DRM_XMR_UnpackLicense(
            f_rgpbReceiverBoundOrLeafXMRLicense[idx],
            f_rgcbReceiverBoundOrLeafXMRLicense[idx],
            &oStack,
            pXmrLicense ) );

        COPY_KID( &oKid, pXmrLicense );
        COPY_LID( &oLid, pXmrLicense );

        ChkDR( _Prnd_Receiver_LicenseTransmit_CheckStoreForDuplicateLicense(
            f_poPrndReceiverContext,
            &oKid,
            &oLid,
            &fDupe ) );

        if( !fDupe )
        {
            ChkDR( _Prnd_Receiver_LicenseTransmit_StoreLicense(
                f_poPrndReceiverContext,
                pbStoreLicense,                                 /* Buffer WITH priority */
                cbStoreLicense,                                 /* Size WITH priority */
                f_rgpbReceiverBoundOrLeafXMRLicense[idx],       /* Buffer WITHOUT priority */
                f_rgcbReceiverBoundOrLeafXMRLicense[idx],       /* Size WITHOUT priority */
                &oKid,
                &oLid,
                pbStack,
                cbStack,
                pXmrLicense,
                f_cReceiverBoundOrLeafXMRLicenses - idx ) );    /* Licenses remaining (including current) */
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_LicenseTransmit_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbLicTransmitMessage )                       const DRM_BYTE                                         *f_pbLicTransmitMessage,
    __in                                                              DRM_DWORD                                         f_cbLicTransmitMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                    dr                                = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL     *poAppContextInternal              = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BOOL                      fRIVUpdated                       = FALSE;
    DRM_DWORD                     cReceiverBoundOrLeafXMRLicenses   = 0;
    DRM_BYTE                    **rgpbReceiverBoundOrLeafXMRLicense = NULL;
    DRM_DWORD                    *rgcbReceiverBoundOrLeafXMRLicense = NULL;
    DRM_BYTE                     *pbOpaqueBufferRemaining           = NULL;
    DRM_DWORD                     cbOpaqueBufferRemaining           = 0;
    DRM_DWORD                     cbLargestLicense                  = 0;
    DRM_BYTE                     *pbOpaqueBuffer                    = NULL;
    DRM_DWORD                     cbOpaqueBuffer                    = 0;

    ChkArg( f_poAppContext          != NULL );
    ChkArg( f_pbLicTransmitMessage  != NULL );
    ChkArg( f_cbLicTransmitMessage   > 0    );
    ChkArg( f_pdwFlags              != NULL );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( _Prnd_Receiver_LicenseTransmit_GetLargestExistingLicense(
        poAppContextInternal->poPrndReceiverContext,
        &cbLargestLicense ) );

    /* This call just fetches some sizes/counts. */
    dr = _Prnd_Receiver_LicenseTransmit_GetLicenses(
        poAppContextInternal->poPrndReceiverContext,
        f_pbLicTransmitMessage,
        f_cbLicTransmitMessage,
        &fRIVUpdated,
        &rgpbReceiverBoundOrLeafXMRLicense,
        &rgcbReceiverBoundOrLeafXMRLicense,
        &cReceiverBoundOrLeafXMRLicenses,
        &pbOpaqueBuffer,
        &cbOpaqueBuffer,
        &cbLargestLicense,
        NULL,   /* Do not pass f_pfnDataCallback/f_pvDataCallbackContext down or caller will receive callback twice. */
        NULL,
        &cbOpaqueBuffer,
        f_pdwFlags );

    DRM_REQUIRE_BUFFER_TOO_SMALL( dr );

    DRMASSERT( cbOpaqueBuffer > 0 );
    ChkMem( pbOpaqueBuffer = Drm_Prnd_MemAlloc( cbOpaqueBuffer ) );

    pbOpaqueBufferRemaining = pbOpaqueBuffer;
    cbOpaqueBufferRemaining = cbOpaqueBuffer;

    ChkDR( _Prnd_Receiver_LicenseTransmit_GetLicenses(
        poAppContextInternal->poPrndReceiverContext,
        f_pbLicTransmitMessage,
        f_cbLicTransmitMessage,
        &fRIVUpdated,
        &rgpbReceiverBoundOrLeafXMRLicense,
        &rgcbReceiverBoundOrLeafXMRLicense,
        &cReceiverBoundOrLeafXMRLicenses,
        &pbOpaqueBufferRemaining,
        &cbOpaqueBufferRemaining,
        &cbLargestLicense,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        &cbOpaqueBuffer,
        f_pdwFlags ) );

    if( fRIVUpdated )
    {
        ChkDR( DRM_RVK_UpdateRevocationVersionsCache(
            &poAppContextInternal->oRevContext,
            &poAppContextInternal->oBlackBoxContext.CryptoContext,
            &poAppContextInternal->idCRLsCurrent,
            poAppContextInternal->pbRevocationBuffer,
            poAppContextInternal->cbRevocationBuffer,
            NULL ) );

        OEM_SECURE_MEMCPY(
            &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
            &poAppContextInternal->idCRLsCurrent,
            SIZEOF( poAppContextInternal->oLicEvalContext.idCRLsCurrent ) );
    }

    ChkDR( _Prnd_Receiver_LicenseTransmit_StoreLicenses(
        poAppContextInternal->poPrndReceiverContext,
        rgpbReceiverBoundOrLeafXMRLicense,
        rgcbReceiverBoundOrLeafXMRLicense,
        cReceiverBoundOrLeafXMRLicenses,
        cbLargestLicense,
        &pbOpaqueBufferRemaining,
        &cbOpaqueBufferRemaining ) );

ErrorExit:
    Drm_Prnd_MemFree( pbOpaqueBuffer );
    ChkECC( ECC_Drm_Prnd_Receiver_LicenseTransmit_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_LicenseError_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbErrMessage )                               const DRM_BYTE                                         *f_pbErrMessage,
    __in                                                              DRM_DWORD                                         f_cbErrMessage,
    __out                                                             DRM_RESULT                                       *f_pdrResult,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext  != NULL );
    ChkArg( f_pbErrMessage  != NULL );
    ChkArg( f_cbErrMessage   > 0    );
    ChkArg( f_pdrResult     != NULL );
    ChkArg( f_pdwFlags      != NULL );

    ChkArg( poAppContextInternal->poPrndReceiverContext != NULL );
    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_LicenseError_Process(
        poAppContextInternal->poPrndReceiverContext,
        f_pbErrMessage,
        f_cbErrMessage,
        f_pdrResult,
        f_pdwFlags ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Receiver_LicenseError_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Receiver_EndSession(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext  != NULL );
    if( poAppContextInternal->poPrndReceiverContext == NULL )
    {
        /* Avoid ChkArg tracing for a common scenario. */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    _RefreshPrndReceiverContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndReceiverContext );

    ChkDR( DRM_PRND_Receiver_EndSession( poAppContextInternal->poPrndReceiverContext ) );

    poAppContextInternal->poPrndReceiverContext = NULL;

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Receiver_EndSession, dr );
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;

