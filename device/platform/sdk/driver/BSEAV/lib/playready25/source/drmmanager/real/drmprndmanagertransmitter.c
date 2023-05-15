/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPRNDMANAGERTRANSMITTER_C
#include <drmmanager.h>
#include <drmprndprotocol.h>
#include <drmxmrbuilder.h>
#include <drmconstants.h>
#include <drmcontract.h>
#include <drmmathsafe.h>

#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

static DRM_VOID _RefreshPrndTransmitterContextFromAppContext(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __inout                                                           DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext )
{
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRMASSERT( f_poAppContext  != NULL );
    __analysis_assume( f_poAppContext != NULL );
    DRMASSERT( f_poPrndTransmitterContext != NULL );
    __analysis_assume( f_poPrndTransmitterContext != NULL );

    f_poPrndTransmitterContext->pBBXContext                = &poAppContextInternal->oBlackBoxContext;
    f_poPrndTransmitterContext->pSSTGlobalContext          = &poAppContextInternal->oSecStoreGlobalContext;
    f_poPrndTransmitterContext->pDSTContext                = &poAppContextInternal->oDatastoreHDS;
    f_poPrndTransmitterContext->pRevocationStoreContext    = &poAppContextInternal->oRevContext;
    f_poPrndTransmitterContext->pbRevocationBuffer         = poAppContextInternal->pbRevocationBuffer;
    f_poPrndTransmitterContext->cbRevocationBuffer         = poAppContextInternal->cbRevocationBuffer;
}

/*
** Transmitter should hold a reference to f_poNewPrndTransmitterContext until after Drm_Prnd_Transmitter_EndSession.
** After that, Transmitter can safely delete f_poNewPrndTransmitterContext.
** The Transmitter must use an independent DRM_APP_CONTEXT and DRM_PRND_TRANSMITTER_CONTEXT for each session.
*/
DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_BeginSession(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in                                                              DRM_PRND_TRANSMITTER_CONTEXT                     *f_poNewPrndTransmitterContext )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );

    ChkArg( f_poNewPrndTransmitterContext != NULL );
    ChkArg( poAppContextInternal->poPrndTransmitterContext == NULL );
    poAppContextInternal->poPrndTransmitterContext = f_poNewPrndTransmitterContext;

    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );
    ChkDR( DRM_PRND_Transmitter_BeginSession( poAppContextInternal->poPrndTransmitterContext ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Transmitter_BeginSession, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_RegistrationRequest_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbReqMessage )                               const DRM_BYTE                                         *f_pbReqMessage,
    __in                                                              DRM_DWORD                                         f_cbReqMessage,
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
    DRM_BYTE                    *pbTransmitterProximityDetectionChannelWeakRef  = NULL;
    DRM_BYTE                    *pbTransmitterProximityDetectionChannel         = NULL;
    DRM_DWORD                    cbTransmitterProximityDetectionChannel         = 0;

    ChkArg( f_poAppContext                              != NULL );
    ChkArg( f_pbReqMessage                              != NULL );
    ChkArg( f_cbReqMessage                               > 0    );
    ChkArg( f_pSessionID                                != NULL );
    ChkArg( f_peProximityDetectionType                  != NULL );
    ChkArg( f_ppbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pcbTransmitterProximityDetectionChannel   != NULL );
    ChkArg( f_pdwFlags                                  != NULL );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    dr = DRM_PRND_Transmitter_RegistrationRequest_Process(
        poAppContextInternal->poPrndTransmitterContext,
        f_pbReqMessage,
        f_cbReqMessage,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        f_peProximityDetectionType,
        &pbTransmitterProximityDetectionChannelWeakRef,
        &cbTransmitterProximityDetectionChannel,
        f_pdwFlags );

    if( dr == DRM_E_PRND_CANNOT_RENEW_USING_NEW_SESSION )
    {
        /* Renewal scenario: hand out the session ID and return the same error code. */
        OEM_SECURE_MEMCPY(
            f_pSessionID,
            &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.SessionID,
            SIZEOF( poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.SessionID ) );
        ChkDR( DRM_E_PRND_CANNOT_RENEW_USING_NEW_SESSION );
    }
    else
    {
        ChkDR( dr );
    }

    OEM_SECURE_MEMCPY(
        f_pSessionID,
        &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.SessionID,
        SIZEOF( poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext.SessionID ) );

    DRMASSERT( cbTransmitterProximityDetectionChannel > 0 );
    ChkMem( pbTransmitterProximityDetectionChannel = Drm_Prnd_MemAlloc( cbTransmitterProximityDetectionChannel ) );
    OEM_SECURE_MEMCPY(
        pbTransmitterProximityDetectionChannel,
        pbTransmitterProximityDetectionChannelWeakRef,
        cbTransmitterProximityDetectionChannel );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbTransmitterProximityDetectionChannel = cbTransmitterProximityDetectionChannel;
    *f_ppbTransmitterProximityDetectionChannel = pbTransmitterProximityDetectionChannel;
    pbTransmitterProximityDetectionChannel     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbTransmitterProximityDetectionChannel );
    ChkECC( ECC_Drm_Prnd_Transmitter_RegistrationRequest_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_RegistrationResponse_Generate(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbRespMessage )                           DRM_BYTE                                        **f_ppbRespMessage,
    __out                                                             DRM_DWORD                                        *f_pcbRespMessage )
{
    DRM_RESULT                   dr                                             = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal                           = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbRespMessage                                  = NULL;
    DRM_DWORD                    cbRespMessage                                  = 0;

    ChkArg( f_poAppContext                              != NULL );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_cbCustomData == 0 ) );
    ChkArg( f_ppbRespMessage                            != NULL );
    ChkArg( f_pcbRespMessage                            != NULL );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );

    DRM_CHECK_CUSTOM_DATA_TYPE( f_pCustomDataTypeID );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    ChkDR( DRM_PRND_Transmitter_RegistrationResponse_Generate(
        poAppContextInternal->poPrndTransmitterContext,
        f_pCustomDataTypeID,
        f_pbCustomData,
        f_cbCustomData,
        f_dwFlags,
        &pbRespMessage,
        &cbRespMessage ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbRespMessage                          = cbRespMessage;
    *f_ppbRespMessage                          = pbRespMessage;
    pbRespMessage                              = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbRespMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_RegistrationResponse_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_RegistrationError_Generate(
    __in_opt                                                          DRM_APP_CONTEXT                                  *f_poAppContext,
    __in                                                              DRM_RESULT                                        f_drResult,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbErrMessage )                            DRM_BYTE                                        **f_ppbErrMessage,
    __out                                                             DRM_DWORD                                        *f_pcbErrMessage )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbErrMessage           = NULL;
    DRM_DWORD                    cbErrMessage           = 0;

    ChkArg( f_ppbErrMessage != NULL );
    ChkArg( f_pcbErrMessage != NULL );
    ChkArg( DRM_FAILED( f_drResult ) );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    if( f_poAppContext != NULL )
    {
        ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
        _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );
    }

    ChkDR( DRM_PRND_Transmitter_RegistrationError_Generate(
        f_poAppContext == NULL ? NULL : poAppContextInternal->poPrndTransmitterContext,
        f_dwFlags,
        &pbErrMessage,
        &cbErrMessage,
        f_drResult ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbErrMessage = cbErrMessage;
    *f_ppbErrMessage = pbErrMessage;
    pbErrMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbErrMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_RegistrationError_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_ProximityDetectionResult_Generate(
    __in_opt                                                          DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_opt                                                    const DRM_ID                                           *f_pSessionID,
    __in                                                              DRM_RESULT                                        f_drResult,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDResultMessage )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbPDResultMessage      = NULL;
    DRM_DWORD                    cbPDResultMessage      = 0;

    ChkArg( ( f_poAppContext == NULL ) != ( f_pSessionID == NULL ) );
    ChkArg( f_ppbPDResultMessage != NULL );
    ChkArg( f_pcbPDResultMessage != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    if( f_poAppContext != NULL )
    {
        ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
        _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );
    }

    ChkDR( DRM_PRND_Transmitter_ProximityDetectionResult_Generate(
        f_poAppContext == NULL ? NULL : poAppContextInternal->poPrndTransmitterContext,
        f_pSessionID,
        f_dwFlags,
        &pbPDResultMessage,
        &cbPDResultMessage,
        f_drResult ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbPDResultMessage = cbPDResultMessage;
    *f_ppbPDResultMessage = pbPDResultMessage;
    pbPDResultMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbPDResultMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_ProximityDetectionResult_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_ProximityDetectionStart_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbPDStartMessage )                           const DRM_BYTE                                         *f_pbPDStartMessage,
    __in                                                              DRM_DWORD                                         f_cbPDStartMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDChlgMessage )                         DRM_BYTE                                        **f_ppbPDChlgMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDChlgMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbPDChlgMessage        = NULL;
    DRM_DWORD                    cbPDChlgMessage        = 0;

    ChkArg( f_poAppContext      != NULL );
    ChkArg( f_pbPDStartMessage  != NULL );
    ChkArg( f_cbPDStartMessage   > 0    );
    ChkArg( f_ppbPDChlgMessage  != NULL );
    ChkArg( f_pcbPDChlgMessage  != NULL );
    ChkArg( f_pdwFlags          != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    ChkDR( DRM_PRND_Transmitter_ProximityDetectionStart_Process(
        poAppContextInternal->poPrndTransmitterContext,
        f_pbPDStartMessage,
        f_cbPDStartMessage,
        f_dwFlags,
        &pbPDChlgMessage,
        &cbPDChlgMessage,
        f_pdwFlags ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbPDChlgMessage = cbPDChlgMessage;
    *f_ppbPDChlgMessage = pbPDChlgMessage;
    pbPDChlgMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbPDChlgMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_ProximityDetectionStart_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_ProximityDetectionResponse_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbPDRespMessage )                            const DRM_BYTE                                         *f_pbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_cbPDRespMessage,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbPDResultMessage )                       DRM_BYTE                                        **f_ppbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pcbPDResultMessage,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbPDResultMessage      = NULL;
    DRM_DWORD                    cbPDResultMessage      = 0;

    ChkArg( f_poAppContext        != NULL );
    ChkArg( f_pbPDRespMessage     != NULL );
    ChkArg( f_cbPDRespMessage      > 0    );
    ChkArg( f_ppbPDResultMessage  != NULL );
    ChkArg( f_pcbPDResultMessage  != NULL );
    ChkArg( f_pdwFlags            != NULL );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    ChkDR( DRM_PRND_Transmitter_ProximityDetectionResponse_Process(
        poAppContextInternal->poPrndTransmitterContext,
        f_pbPDRespMessage,
        f_cbPDRespMessage,
        f_dwFlags,
        &pbPDResultMessage,
        &cbPDResultMessage,
        f_pdwFlags ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbPDResultMessage = cbPDResultMessage;
    *f_ppbPDResultMessage = pbPDResultMessage;
    pbPDResultMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbPDResultMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_ProximityDetectionResponse_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_LicenseRequest_Process(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_bcount( f_cbLicReqMessage )                            const DRM_BYTE                                         *f_pbLicReqMessage,
    __in                                                              DRM_DWORD                                         f_cbLicReqMessage,
    __in_opt                                                          Drm_Prnd_Data_Callback                            f_pfnDataCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvDataCallbackContext,
    __in_opt                                                          Drm_Prnd_Content_Identifier_Callback              f_pfnContentIdentifierCallback,
    __inout_ecount_opt( 1 )                                           DRM_VOID                                         *f_pvContentIdentifierCallbackContext,
    __out                                                             DRM_DWORD                                        *f_pdwFlags )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext      != NULL );
    ChkArg( f_pbLicReqMessage   != NULL );
    ChkArg( f_cbLicReqMessage    > 0    );
    ChkArg( f_pdwFlags          != NULL );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    ChkDR( DRM_PRND_Transmitter_LicenseRequest_Process(
        poAppContextInternal->poPrndTransmitterContext,
        f_pbLicReqMessage,
        f_cbLicReqMessage,
        f_pfnDataCallback,
        f_pvDataCallbackContext,
        f_pfnContentIdentifierCallback,
        f_pvContentIdentifierCallbackContext,
        f_pdwFlags ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Transmitter_LicenseRequest_Process, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_LicenseTransmit_Generate(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_opt                                                    const DRM_ID                                           *f_pCustomDataTypeID,
    __in_bcount_opt( f_cbCustomData )                           const DRM_BYTE                                         *f_pbCustomData,
    __in                                                              DRM_DWORD                                         f_cbCustomData,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbLicTransmitMessage )                    DRM_BYTE                                        **f_ppbLicTransmitMessage,
    __out                                                             DRM_DWORD                                        *f_pcbLicTransmitMessage )
{
    DRM_RESULT                                dr                                    = DRM_SUCCESS;
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_LOCAL, "Prefast Noise: App context locals should never be const")
    DRM_APP_CONTEXT_INTERNAL                 *poAppContextInternal                  = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
PREFAST_POP /* __WARNING_NONCONST_LOCAL */
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT     *pSession                              = NULL;
    DRM_LICSTOREENUM_CONTEXT                  oLicEnum                              = {0};
    DRM_DWORD                                 cLicenses                             = 0;
    DRM_DWORD                                 idx                                   = 0;
    DRM_DWORD                                 cbRequiredWorkingSpace                = 0;
    DRM_BYTE                                 *pbRemainingWorkingSpace               = NULL;
    DRM_DWORD                                 cbRemainingWorkingSpace               = 0;
    DRM_BYTE                                **rgpbReceiverBoundOrLeafXMRLicense     = NULL;
    DRM_DWORD                                *rgcbReceiverBoundOrLeafXMRLicense     = NULL;
    DRM_BYTE                                 *pbLicTransmitMessage                  = NULL;
    DRM_DWORD                                 cbLicTransmitMessage                  = 0;
    DRM_DWORD                                 dwResult                              = 0;

    ChkArg( f_poAppContext           != NULL );
    ChkArg( f_ppbLicTransmitMessage  != NULL );
    ChkArg( f_pcbLicTransmitMessage  != NULL );
    ChkArg( ( f_pbCustomData == NULL ) == ( f_pCustomDataTypeID == NULL ) );

    DRM_CHECK_CUSTOM_DATA_TYPE( f_pCustomDataTypeID );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS || f_dwFlags == DRM_PRND_FLAG_LICENSE_TRANSMIT_IS_LICENSE_PUSH );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    pSession = &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext;

    ChkBOOL( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED, DRM_E_DEVICE_NOT_REGISTERED );

    /*
    ** Determine how many licenses we are dealing with.
    */
    ChkDR( DRM_LST_InitEnum( &pSession->oLSTContextPRNDStore, NULL, FALSE, &oLicEnum ) );
    ChkDR( DRM_LST_LicCount( &oLicEnum, &cLicenses ) );
    ChkBOOL( cLicenses > 0, DRM_E_LICENSE_NOT_FOUND );  /* The protocol requires at least one license. */

    /*
    ** The working space buffer contains:
    ** 1. An array of BYTE*  each of which points to one license.
    ** 2. An array of DWORDs each of which contains the size of the corresponding license in the first array.
    */
    cbRequiredWorkingSpace = ( cLicenses * ( SIZEOF( DRM_BYTE* ) + SIZEOF( DRM_DWORD ) ) );
    if( pSession->cbWorkingSpace <  cbRequiredWorkingSpace
     || pSession->pbWorkingSpace == NULL )
    {
        SAFE_OEM_FREE( pSession->pbWorkingSpace );
        pSession->cbWorkingSpace = 0;
        ChkMem( pSession->pbWorkingSpace = (DRM_BYTE*)Oem_MemAlloc( cbRequiredWorkingSpace ) );
        pSession->cbWorkingSpace = cbRequiredWorkingSpace;
    }
    OEM_SECURE_ZERO_MEMORY( pSession->pbWorkingSpace, pSession->cbWorkingSpace );

    pbRemainingWorkingSpace =  pSession->pbWorkingSpace;
    cbRemainingWorkingSpace =  pSession->cbWorkingSpace;

    DRMASSERT( cbRemainingWorkingSpace >= ( cLicenses * SIZEOF( DRM_BYTE* ) ) );
    rgpbReceiverBoundOrLeafXMRLicense = (DRM_BYTE**)pbRemainingWorkingSpace;
    ChkDR( DRM_DWordMult( cLicenses, SIZEOF( DRM_BYTE* ), &dwResult ) );
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbRemainingWorkingSpace, dwResult, (DRM_DWORD_PTR*)&pbRemainingWorkingSpace ) );
    ChkDR( DRM_DWordSub( cbRemainingWorkingSpace, dwResult, &cbRemainingWorkingSpace ) );

    DRMASSERT( cbRemainingWorkingSpace >= ( cLicenses * SIZEOF( DRM_DWORD ) ) );
    rgcbReceiverBoundOrLeafXMRLicense = (DRM_DWORD*)pbRemainingWorkingSpace;
    ChkDR( DRM_DWordMult( cLicenses, SIZEOF( DRM_DWORD ), &dwResult ) );
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbRemainingWorkingSpace, dwResult, (DRM_DWORD_PTR*)&pbRemainingWorkingSpace ) );
    ChkDR( DRM_DWordSub( cbRemainingWorkingSpace, dwResult, &cbRemainingWorkingSpace ) );

    /*
    ** Walk through the store and fetch the pointer, size, and LID for each license.
    */
    ChkDR( DRM_LST_InitEnum( &pSession->oLSTContextPRNDStore, NULL, FALSE, &oLicEnum ) );

    for( idx = 0; idx < cLicenses; idx++ )
    {
        DRM_KID   oKid  = {0};
        DRM_LID   oLid  = {0};
        DRM_DWORD cbLic = 0;
        DRM_DWORD dwPri = 0;

        ChkDR( DRM_LST_EnumNext( &oLicEnum, &oKid, &oLid, NULL, &cbLic ) );

        ChkDR( DRM_LST_GetLicensePtr( &pSession->oLSTContextPRNDStore, &oKid, &oLid, NULL, &rgpbReceiverBoundOrLeafXMRLicense[idx], &rgcbReceiverBoundOrLeafXMRLicense[idx], &dwPri ) );
    }

    ChkDR( DRM_PRND_Transmitter_LicenseTransmit_Generate(
        poAppContextInternal->poPrndTransmitterContext,
        rgpbReceiverBoundOrLeafXMRLicense,
        rgcbReceiverBoundOrLeafXMRLicense,
        cLicenses,
        f_pCustomDataTypeID,
        f_pbCustomData,
        f_cbCustomData,
        f_dwFlags,
        &pbLicTransmitMessage,
        &cbLicTransmitMessage ) );

    /*
    ** Clear the PRND store.  The encryptors (if any) should already be created, and any licenses are no longer needed on Transmitter.
    ** The PRND protocol defines failure of this message to reach Receiver as a fatal playback error.  There's no retry.
    */
    ChkDR( DRM_PRND_Transmitter_ClearPRNDStore( poAppContextInternal->poPrndTransmitterContext ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbLicTransmitMessage = cbLicTransmitMessage;
    *f_ppbLicTransmitMessage = pbLicTransmitMessage;
    pbLicTransmitMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbLicTransmitMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_LicenseTransmit_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_LicenseError_Generate(
    __in_opt                                                          DRM_APP_CONTEXT                                  *f_poAppContext,
    __in_opt                                                    const DRM_ID                                           *f_pSessionID,
    __in                                                              DRM_RESULT                                        f_drResult,
    __in                                                              DRM_DWORD                                         f_dwFlags,
    __deref_out_ecount( *f_pcbErrMessage )                            DRM_BYTE                                        **f_ppbErrMessage,
    __out                                                             DRM_DWORD                                        *f_pcbErrMessage )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BYTE                    *pbErrMessage           = NULL;
    DRM_DWORD                    cbErrMessage           = 0;

    ChkArg( ( f_poAppContext == NULL ) != ( f_pSessionID == NULL ) );
    ChkArg( f_ppbErrMessage  != NULL );
    ChkArg( f_pcbErrMessage  != NULL );
    ChkArg( DRM_FAILED( f_drResult ) );

    ChkArg( f_dwFlags == DRM_PRND_FLAG_NO_FLAGS );

    if( f_poAppContext != NULL )
    {
        ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
        _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );
    }

    ChkDR( DRM_PRND_Transmitter_LicenseError_Generate(
        f_poAppContext == NULL ? NULL : poAppContextInternal->poPrndTransmitterContext,
        f_pSessionID,
        f_dwFlags,
        &pbErrMessage,
        &cbErrMessage,
        f_drResult ) );

    /* Everything succeeded.  Transfer ownership. */
    *f_pcbErrMessage = cbErrMessage;
    *f_ppbErrMessage = pbErrMessage;
    pbErrMessage     = NULL;

ErrorExit:
    Drm_Prnd_MemFree( pbErrMessage );
    ChkECC( ECC_Drm_Prnd_Transmitter_LicenseError_Generate, dr );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_EndSession(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );

    ChkDR( DRM_PRND_Transmitter_EndSession( poAppContextInternal->poPrndTransmitterContext ) );

    poAppContextInternal->poPrndTransmitterContext = NULL;

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Transmitter_EndSession, dr );
    return dr;
}

typedef struct __tag_Prnd_Transmitter_SignLicense_Data
{
    DRM_BB_CONTEXT      *poBlackBoxContext;
    OEM_CRYPTO_HANDLE    handleCI;
} Prnd_Transmitter_SignLicense_Data;

static DRM_RESULT DRM_CALL _Prnd_Transmitter_SignLicense(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount(f_cbData)               const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer )
{
    DRM_RESULT                           dr         = DRM_SUCCESS;
    DRM_DWORD                            dwResult   = 0;
    DRM_BYTE                            *pbResult   = NULL;
    Prnd_Transmitter_SignLicense_Data   *pSignData  = (Prnd_Transmitter_SignLicense_Data*)f_pvSignatureContext;

    ChkArg( f_pvSignatureContext != NULL );

    /*
    ** f_pbData / f_cbData is the unsigned portion of the license.
    ** f_pbSignatureBuffer / f_cbSignatureBuffer is the whole license buffer, not just the buffer for the actual signature.
    ** This ChkArg makes sure we have at least enough space.
    */
    ChkDR( DRM_DWordAdd( f_ibSignatureBuffer, DRM_AES_BLOCKLEN, &dwResult ) );
    ChkArg( dwResult <= f_cbSignatureBuffer );

    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pbSignatureBuffer, f_ibSignatureBuffer, (DRM_DWORD_PTR*)&pbResult ) );

    ChkDR( DRM_PRND_Tx_SignWithAESOMAC(
        pSignData->poBlackBoxContext,
        pSignData->handleCI,
        f_pbData,
        f_cbData,
        pbResult ) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _Prnd_Transmitter_Update_Expiration(
    __in_opt                              DRM_VOID                                 *f_pOEMContext,
    __in                                  DRM_PRND_TRANSMITTER_SESSION_CONTEXT     *f_pSession,
    __in                                  DRM_XMR_GLOBAL_REQUIREMENTS              *f_poGlobalPolicies )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRMFILETIME ftCurrent      = {0};

    ChkArg( f_pSession         != NULL );
    ChkArg( f_poGlobalPolicies != NULL );

    Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ftCurrent );     /* returns void */

    /* Convert Expire-After-First-Play to standard expiration license */
    if( f_poGlobalPolicies->ExpirationAfterUse.fValid )
    {
        DRM_DWORD dwBeginDate = 0;
        DRM_DWORD dwEndDate   = 0;

        FILE_TIME_TO_DWORD( ftCurrent, dwBeginDate );

        /* Special case where Expire-After-First-Play is short, we change it to pre-defined value */
        if( f_poGlobalPolicies->ExpirationAfterUse.dwValue <= DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY )
        {
            ChkDR( DRM_DWordAdd( dwBeginDate, DRM_PRND_LICENSE_REBIND_MIN_EXPIRE_AFTER_FIRST_PLAY, &dwEndDate ) );
        }
        else
        {
            ChkDR( DRM_DWordAdd( dwBeginDate, f_poGlobalPolicies->ExpirationAfterUse.dwValue, &dwEndDate ) );
        }

        if( f_poGlobalPolicies->Expiration.fValid )
        {
            /* Keep the more restrictive end date and leave the begin date as is */
            f_poGlobalPolicies->Expiration.dwEndDate = MIN( f_poGlobalPolicies->Expiration.dwEndDate, dwEndDate );
        }
        else
        {
            f_poGlobalPolicies->Expiration.fValid = TRUE;

            /* Use registration time as begin Date */
            FILE_TIME_TO_DWORD(
                    f_pSession->ftSuccessfulProximity,
                    f_poGlobalPolicies->Expiration.dwBeginDate );
            f_poGlobalPolicies->Expiration.dwEndDate = dwEndDate;
        }

        f_poGlobalPolicies->ExpirationAfterUse.fValid = FALSE;
    }

    if( f_poGlobalPolicies->Expiration.fValid )
    {
        /* Adjust the expiration restriction based on any clock drift between Tx and Rx. */
        if( f_poGlobalPolicies->Expiration.dwBeginDate >= f_pSession->dwSubtractFromBeginDate )
        {
            ChkDR( DRM_DWordSub(
                        f_poGlobalPolicies->Expiration.dwBeginDate,
                        f_pSession->dwSubtractFromBeginDate,
                       &f_poGlobalPolicies->Expiration.dwBeginDate ) );
        }
        if( f_poGlobalPolicies->Expiration.dwEndDate <= MAX_UNSIGNED_TYPE(DRM_DWORD) - f_pSession->dwAddToEndDate )
        {
            ChkDR( DRM_DWordAdd(
                        f_poGlobalPolicies->Expiration.dwEndDate,
                        f_pSession->dwAddToEndDate,
                       &f_poGlobalPolicies->Expiration.dwEndDate ) );
        }
    }

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _Prnd_Transmitter_Remove_AppID(
    __in                DRM_XMR_PLAYBACK_RIGHTS     *f_poPlaybackPolicies )
{
    DRM_RESULT              dr                     = DRM_SUCCESS;
    DRM_XMR_UNKNOWN_OBJECT *pUnkObject             = NULL;
    DRM_XMR_UNKNOWN_OBJECT *pUnkObjectPrev         = NULL;

    ChkArg( f_poPlaybackPolicies != NULL );

    /*
    ** Remove the App ID list, if present.
    ** If the Tx is not an approved app, Bind would have failed before we reached this point.
    ** If the Tx is an approved app, it's approved for PRND-Transmit as well as local playback.
    ** The Rx does not need to be an approved app.
    */
    for( pUnkObject = f_poPlaybackPolicies->pUnknownObjects;
         pUnkObject != NULL && pUnkObject->fValid;
         pUnkObject = pUnkObject->pNext )
    {
        if( pUnkObject->wType == XMR_OBJECT_TYPE_APPLICATION_ID_LIST )
        {
            /*
            ** The pUnknownObjects linked list objects are allocated off of oPackStack which itself is
            ** allocated off of part of pSession->pbWorkingSpace.  We should not free them here.
            */
            if( pUnkObjectPrev == NULL )
            {
                /* We're removing the first object in the list. Point the first object to its child. */
                f_poPlaybackPolicies->pUnknownObjects = f_poPlaybackPolicies->pUnknownObjects->pNext;
            }
            else
            {
                /* We're NOT removing the first object in the list. Point the previous object to its child. */
                pUnkObjectPrev->pNext = pUnkObjectPrev->pNext->pNext;
            }
        }
        else
        {
            /* We didn't remove an object, so update the previous object to be the current object. */
            pUnkObjectPrev = pUnkObject;
        }
    }

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _Prnd_Transmitter_Remove_Invalid_PlayEnabler(
    __in            DRM_XMR_UNKNOWN_CONTAINER           *f_poUnkContainer )
{
    DRM_RESULT                           dr                     = DRM_SUCCESS;
    DRM_XMR_UNKNOWN_CONTAINER           *pUnkContainerPrev      = NULL;
    const DRM_XMR_UNKNOWN_OBJECT        *pUnkObject             = NULL;

    while( f_poUnkContainer != NULL && f_poUnkContainer->fValid )
    {
        DRM_BOOL fUpdatePrev = TRUE;

        /* Only check for PLAY_ENABLER Container */
        if( f_poUnkContainer->wType == XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER )
        {
            for( pUnkObject =  f_poUnkContainer->pObject;
                 pUnkObject != NULL && pUnkObject->fValid;
                 pUnkObject =  pUnkObject->pNext )
            {
                DRMASSERT( pUnkObject->cbData == SIZEOF( DRM_GUID ) );

                /* Only look XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT */
                if( pUnkObject->wType != XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT )
                {
                    continue;
                }

                /* Only remove PlayEnabler that's not UNKNOWN_OUTPUT nor CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT */
                if(    IDENTICAL_GUIDS(  pUnkObject->pbBuffer + pUnkObject->ibData,
                                        &DRM_PLAYENABLER_UNKNOWN_OUTPUT )
                    || IDENTICAL_GUIDS(  pUnkObject->pbBuffer + pUnkObject->ibData,
                                        &DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT ) )
                {
                    continue;
                }

                /*
                ** We don't need to free the UnknownContainer because it's allocated on the oPackStack
                */
                if( pUnkContainerPrev == NULL )
                {
                    /*
                    ** We're removing the first container which is a struct in the DRM_XMR_PLAYBACK_RIGHTS struct.
                    ** Therefore, we will just copy the next container
                    */
                    if( f_poUnkContainer->pNext != NULL )
                    {
                        OEM_SECURE_MEMCPY(
                                f_poUnkContainer,
                                f_poUnkContainer->pNext,
                                SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                        /* We don't need to update the f_poUnkContainer because it's already updated with above memcpy */
                        fUpdatePrev = FALSE;
                    }
                    else
                    {
                        /*
                         ** There's only one container in the list and it needs to be removed.
                         ** Therefore, memzero that container
                         */
                        OEM_SECURE_MEMSET(
                                f_poUnkContainer,
                                0,
                                SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );
                    }
                }
                else
                {
                    /* We're NOT removing the first container in the list. Just point the previous object to its child */
                    pUnkContainerPrev->pNext = pUnkContainerPrev->pNext->pNext;
                }

                /* We found the invalid playenabler, so break out of the loop */
                break;
            }
        }
        else
        {
            /* We didn't remove a container, so update the previous container to be the current container */
            pUnkContainerPrev = f_poUnkContainer;
        }

        if( fUpdatePrev )
        {
            f_poUnkContainer = f_poUnkContainer->pNext;
        }
    }

    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_PrepareLicensesForTransmit(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext )
{
    DRM_RESULT                                dr                        = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL                 *poAppContextInternal      = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                                 idx                       = 0;
    DRM_PRND_TRANSMITTER_SESSION_CONTEXT     *pSession                  = NULL;
    DRM_DWORD                                 cbRequiredWorkingSpace    = 0;
    DRM_BYTE                                 *pbRemainingWorkingSpace   = NULL;
    DRM_DWORD                                 cbRemainingWorkingSpace   = 0;
    DRM_XMR_LICENSE                          *pReboundLicense           = NULL;
    OEM_CRYPTO_HANDLE                         handleCI                  = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD                                 cbNewLicenseBuffer        = 0;
    DRM_BYTE                                 *pbNewLicenseBuffer        = NULL;
    DRM_BYTE                                 *pbUnpackStack             = NULL;
    DRM_DWORD                                 cbUnpackStack             = 0;
    DRM_BYTE                                 *pbPackStack               = NULL;
    DRM_DWORD                                 cbPackStack               = 0;
    DRM_DWORD                                 dwResult                  = 0;
	DRM_BYTE					 		      rgbEncKeys[ECC_P256_CIPHERTEXT_SIZE_IN_BYTES + DRM_AES_KEYSIZE_128] = {0};

    TRACE(( "Drm_Prnd_Transmitter_PrepareLicensesForTransmit Entered" ));

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    pSession = &poAppContextInternal->poPrndTransmitterContext->oPrndTransmitterSessionContext;

    ChkBOOL( pSession->ePrndProximityDetectionState == DRM_PRND_PROXIMITY_DETECTION_STATE_VERIFIED, DRM_E_DEVICE_NOT_REGISTERED );

    ChkBOOL( poAppContextInternal->fBindThenCommit, DRM_E_LICENSE_NOT_BOUND );

    for( idx = 0; idx < poAppContextInternal->dwChainDepth; idx++ )
    {
        const DRM_XMR_LICENSE *pNextLicense = &poAppContextInternal->licenseXMR[idx];
        cbNewLicenseBuffer = MAX( cbNewLicenseBuffer, pNextLicense->cbXMRLic );
    }

    /*
    ** We make the new license buffer twice the size of the largest existing license to ensure that
    ** there is space for the XMR changes we are making to the new (rebound) license.
    */
    ChkDR( DRM_DWordMult( cbNewLicenseBuffer, 2, &cbNewLicenseBuffer ) );

    /*
    ** The working space buffer contains:
    ** 1. A single DRM_XMR_LICENSE structure.
    ** 2. A rebound license buffer.
    ** 3. The stack used for DRM_XMR_UnpackLicense.
    **      We use DRM_XMR_MAX_UNPACK_ALLOC_FACTOR as documented by DRM_XMR_UnpackLicense.
    ** 4. The stack space buffer passed to DRM_XMR_StartLicense.
    **      We use DRM_XMR_MAX_UNPACK_ALLOC_FACTOR again since it will be more than enough.
    */
    ChkDR( DRM_DWordAdd( SIZEOF( DRM_XMR_LICENSE ), cbNewLicenseBuffer, &cbRequiredWorkingSpace ) );
    ChkDR( DRM_DWordMult( cbNewLicenseBuffer, DRM_XMR_MAX_UNPACK_ALLOC_FACTOR, &dwResult ) );
    ChkDR( DRM_DWordAdd( cbRequiredWorkingSpace, dwResult, &cbRequiredWorkingSpace ) );
    ChkDR( DRM_DWordAdd( cbRequiredWorkingSpace, dwResult, &cbRequiredWorkingSpace ) );

    if( pSession->cbWorkingSpace <  cbRequiredWorkingSpace
     || pSession->pbWorkingSpace == NULL )
    {
        SAFE_OEM_FREE( pSession->pbWorkingSpace );
        pSession->cbWorkingSpace = 0;
        ChkMem( pSession->pbWorkingSpace = (DRM_BYTE*)Oem_MemAlloc( cbRequiredWorkingSpace ) );
        pSession->cbWorkingSpace = cbRequiredWorkingSpace;
    }
    OEM_SECURE_ZERO_MEMORY( pSession->pbWorkingSpace, pSession->cbWorkingSpace );

    pbRemainingWorkingSpace =  pSession->pbWorkingSpace;
    cbRemainingWorkingSpace =  pSession->cbWorkingSpace;

    pReboundLicense = ( DRM_XMR_LICENSE* )pbRemainingWorkingSpace;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbRemainingWorkingSpace, SIZEOF( DRM_XMR_LICENSE ), (DRM_DWORD_PTR*)&pbRemainingWorkingSpace ) );
    ChkDR( DRM_DWordSub( cbRemainingWorkingSpace, SIZEOF( DRM_XMR_LICENSE ), &cbRemainingWorkingSpace ) );

    pbNewLicenseBuffer = pbRemainingWorkingSpace;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbRemainingWorkingSpace, cbNewLicenseBuffer, (DRM_DWORD_PTR*)&pbRemainingWorkingSpace ) );
    ChkDR( DRM_DWordSub( cbRemainingWorkingSpace, cbNewLicenseBuffer, &cbRemainingWorkingSpace ) );

    /* Use half the remaining working space for the stack for DRM_XMR_UnpackLicense (UnpackStack).  The other half will go to DRM_XMR_StartLicense (PackStack). */
    pbUnpackStack = pbRemainingWorkingSpace;
    cbUnpackStack = cbRemainingWorkingSpace / 2;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbRemainingWorkingSpace, cbUnpackStack, (DRM_DWORD_PTR*)&pbRemainingWorkingSpace ) );
    ChkDR( DRM_DWordSub( cbRemainingWorkingSpace, cbUnpackStack, &cbRemainingWorkingSpace ) );

    pbPackStack = pbRemainingWorkingSpace;
    cbPackStack = cbRemainingWorkingSpace;
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbRemainingWorkingSpace, cbPackStack, (DRM_DWORD_PTR*)&pbRemainingWorkingSpace ) );
    ChkDR( DRM_DWordSub( cbRemainingWorkingSpace, cbPackStack, &cbRemainingWorkingSpace ) );

    for( idx = 0; idx < poAppContextInternal->dwChainDepth; idx++ )
    {
        DRM_DWORD              cbNewLicense = 0;
        DRM_BYTE              *pbResult     = NULL;
        const DRM_XMR_LICENSE *pNextLicense = &poAppContextInternal->licenseXMR[idx];

        ChkBOOL( pNextLicense->pbXMRLic != NULL,                                DRM_E_LICENSE_NOT_BOUND );
        ChkBOOL( pNextLicense->cbXMRLic  > 0,                                   DRM_E_LICENSE_NOT_BOUND );
        ChkBOOL( pNextLicense->containerOuter.fValid,                           DRM_E_LICENSE_NOT_BOUND );
        ChkBOOL( pNextLicense->containerOuter.containerKeys.fValid,             DRM_E_LICENSE_NOT_BOUND );
        ChkBOOL( pNextLicense->containerOuter.containerGlobalPolicies.fValid,   DRM_E_LICENSE_NOT_BOUND );

        if( XMR_IS_SECURITY_LEVEL_VALID( pNextLicense ) )
        {
            /* We can't rebind to a lower security level certificate. */
            ChkBOOL( pSession->dwReceiverSecurityLevel >= (DRM_DWORD)pNextLicense->containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel,
                DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
        }

        ChkBOOL( XMR_IS_KEY_MATERIAL_VALID( pNextLicense ), DRM_E_RIGHTS_NOT_AVAILABLE );

        /* We can't transmit a license originating in the PRND store because that would create a repeater. */
        ChkBOOL( !poAppContextInternal->oFFLicense.rgfLicenseFromPRND[idx], DRM_E_PRND_CANNOT_REBIND_PRND_RECEIVED_LICENSE );

        if( XMR_IS_RIGHTS_VALID( pNextLicense ) && ( pNextLicense->containerOuter.containerGlobalPolicies.Rights.wValue & XMR_RIGHTS_CANNOT_PERSIST ) )
        {
            ChkBOOL( !poAppContextInternal->oFFLicense.rgfLicenseFromEST[idx], DRM_E_PRND_CANNOT_REBIND_PRND_RECEIVED_LICENSE );
        }
        
        if( pNextLicense->containerOuter.containerKeys.UplinkKid.fValid
         || pNextLicense->containerOuter.containerKeys.UplinkX.fValid )
        {
            /* License is bound to parent license: just add it to the PRND store. */
            ChkDR( DRM_DWordSub( cbNewLicenseBuffer, SIZEOF( DRM_DWORD ), &cbNewLicense ) ); /* Space for priority */
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbNewLicenseBuffer, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)&pbResult ) ); /* Space for priority */
            ChkBOOL( pNextLicense->cbXMRLic <= cbNewLicense, DRM_E_OUTOFMEMORY );

            OEM_SECURE_MEMCPY(
                pbResult,
                pNextLicense->pbXMRLic,
                pNextLicense->cbXMRLic );

            ChkDR( DRM_LST_AddLicense(
                &pSession->oLSTContextPRNDStore,
                pNextLicense->cbXMRLic, /* Size WITHOUT priority */
                pbNewLicenseBuffer,     /* Buffer WITH priority */
                &poAppContextInternal->oFFLicense.rgkid[idx],
                &poAppContextInternal->oFFLicense.rglid[idx],
                0 ) );
        }
        else
        {
            /* License is bound to Transmitter: rebind it to Receiver before adding it to the PRND store. */
            DRM_STACK_ALLOCATOR_CONTEXT          oPackStack             = {0};
            DRM_XMR_ECC_DEVICE_KEY              *pEccDeviceKey          = NULL;
            DRM_XMR_CONTENT_KEY                 *pContentKey            = NULL;
            DRM_XMR_BUILDER_CONTEXT              oXmrBuilderCtx         = {0};
            DRM_LID                              oNewLid                = {0};
            Prnd_Transmitter_SignLicense_Data    oSignData              = {0};
            CIPHERTEXT_P256                      oEncryptedAESKeyPair   = {0};
            DRMFILETIME                          ftCurrent              = {0};

            ChkDR( Oem_Random_GetBytes( poAppContextInternal->pOEMContext, (DRM_BYTE*)&oNewLid, SIZEOF( oNewLid ) ) );

            ChkDR( DRM_STK_Init( &oPackStack, pbPackStack, cbPackStack ) );

            ChkDR( DRM_XMR_UnpackLicense(
                pNextLicense->pbXMRLic,
                pNextLicense->cbXMRLic,
                &oPackStack,
                pReboundLicense ) );

            pContentKey   = &pReboundLicense->containerOuter.containerKeys.ContentKey;
            pEccDeviceKey = &pReboundLicense->containerOuter.containerKeys.ECCKey;

            ChkBOOL( XMR_IS_GLOBAL_POLICIES_VALID( pReboundLicense )
                  && XMR_IS_KEY_MATERIAL_VALID( pReboundLicense )
                  && pContentKey->fValid, DRM_E_RIGHTS_NOT_AVAILABLE );

            /*
            ** Rebind the license to the receiver.
            */
            ChkDR( DRM_PRND_Tx_RebindLicenseKeysToNewECCKey(
                &poAppContextInternal->oBlackBoxContext,
                &poAppContextInternal->oRevContext,
                &poAppContextInternal->oFFLicense.pBindingInfo[idx],
                &pSession->oECC256PubKeyContentEncryptReceiver,
                pSession->pbReceiverCertificate,
                pSession->cbReceiverCertificate,
                pSession->pbReceiverCertificateCRL,
                pSession->cbReceiverCertificateCRL,
                &pSession->SessionID,
                &handleCI,
                &oEncryptedAESKeyPair ) );

            pContentKey->iEncryptedKey = 0;
            if( XMR_IS_AUX_KEY_VALID( pNextLicense ) )
            {
                /* A scalable root license requires more work. */
                const DRM_XMR_CONTENT_KEY   *pNextLicenseContentKey = &pNextLicense->containerOuter.containerKeys.ContentKey;

                /* Verify scalable license properties. */
                AssertChkArg( XMR_IS_AUX_KEY_VALID( pReboundLicense ) );
                ChkBOOL( pNextLicenseContentKey->cbEncryptedKey == SIZEOF(rgbEncKeys), DRM_E_INVALID_LICENSE );
                ChkBOOL( pNextLicenseContentKey->wKeyEncryptionCipherType == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ, DRM_E_INVALID_LICENSE );

                /* In addition to the rebound CI/CK... */
                OEM_SECURE_MEMCPY( rgbEncKeys, &oEncryptedAESKeyPair, ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

                /*
                ** ...we have the secondary key which is protected with CK.
                ** Since CK remains unchanged (just rebound), we can copy the secondary key directly from the source license.
                */
                OEM_SECURE_MEMCPY(
                    &rgbEncKeys[ECC_P256_CIPHERTEXT_SIZE_IN_BYTES],
                    &pNextLicenseContentKey->pbEncryptedKeyBuffer[pNextLicenseContentKey->iEncryptedKey + ECC_P256_CIPHERTEXT_SIZE_IN_BYTES],
                    DRM_AES_KEYSIZE_128 );

                /*
                ** Deal with a quirk of the XMR license builder/parser:
                ** The aux key entry location DWORDs are in:
                **  Native format on input (DRM_XMR_Serialize_AuxKey uses DWORD_TO_NETWORKBYTES).
                **  Big-endian format on output (DRM_XMR_Parse_AuxKey directly casts to DRM_XMR_AUX_KEY_ENTRY*).
                ** The places where the aux key entry locations are used (in key derivation)
                **  explicitly deal with this by calling NETWORKBYTES_TO_DWORD as needed.
                ** So, if our native architecture is little-endian,
                **  we have to convert each location DWORD from big-endian (parsed)
                **  to little-endian (native) before calling the XMR builder or
                **  our XMR license will not correctly have them in network byte order.
                */
#if TARGET_LITTLE_ENDIAN
                {
                    DRM_DWORD        iAux       = 0;
                    DRM_XMR_AUX_KEY *pAuxKey    = NULL;
                    pAuxKey = &pReboundLicense->containerOuter.containerKeys.AuxKey;
                    AssertChkArg( pAuxKey != NULL );
                    AssertChkArg( pAuxKey->cEntries == 0 || pAuxKey->pEntries != NULL );
                    for( iAux = 0; iAux < pAuxKey->cEntries; iAux++ )
                    {
                        REVERSE_BYTES_DWORD( pAuxKey->pEntries[iAux].dwLocation );
                    }
                }
#endif /* TARGET_LITTLE_ENDIAN */

                pContentKey->pbEncryptedKeyBuffer = rgbEncKeys;
                pContentKey->cbEncryptedKey       = SIZEOF( rgbEncKeys );
                ChkDR( DRM_DWordToWord( XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ, &pContentKey->wKeyEncryptionCipherType ) );
            }
            else
            {
                pContentKey->pbEncryptedKeyBuffer = (DRM_BYTE*)&oEncryptedAESKeyPair;
                pContentKey->cbEncryptedKey       = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;
                ChkDR( DRM_DWordToWord( XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256, &pContentKey->wKeyEncryptionCipherType ) );
            }

            /* Point the XMR license to the new pubkey. Remove any optimized content key. */
            pEccDeviceKey->fValid           = TRUE;
            pEccDeviceKey->wEccCurveType    = XMR_ECC_CURVE_TYPE_P256;
            pEccDeviceKey->cbKeyData        = SIZEOF( pSession->oECC256PubKeyContentEncryptReceiver.PublicKey );
            pEccDeviceKey->pbKeyData        = (DRM_BYTE*)&pSession->oECC256PubKeyContentEncryptReceiver.PublicKey;
            pEccDeviceKey->iKeyData         = 0;
            pReboundLicense->containerOuter.containerKeys.OptimizedContentKey.fValid = FALSE;

            /* Update the Issue Date to the current date-time. */
            pReboundLicense->containerOuter.containerGlobalPolicies.IssueDate.fValid  = TRUE;
            Oem_Clock_GetSystemTimeAsFileTime( poAppContextInternal->pOEMContext, &ftCurrent );     /* returns void */
            FILE_TIME_TO_DWORD( ftCurrent, pReboundLicense->containerOuter.containerGlobalPolicies.IssueDate.dwValue );

            ChkDR( _Prnd_Transmitter_Update_Expiration(
                poAppContextInternal->pOEMContext,
                pSession,
               &pReboundLicense->containerOuter.containerGlobalPolicies ) );

            /* Remove MID, DomainID, and all copy rights/restrictions. */
            pReboundLicense->containerOuter.containerGlobalPolicies.Metering.fValid           = FALSE;
            pReboundLicense->containerOuter.containerGlobalPolicies.DomainID.fValid           = FALSE;
            pReboundLicense->containerOuter.containerCopyPolicies.fValid                      = FALSE;

            /* Add the CANNOT_PERSIST setting. */
            if( pReboundLicense->containerOuter.containerGlobalPolicies.Rights.fValid )
            {
                pReboundLicense->containerOuter.containerGlobalPolicies.Rights.wValue         =
                    (DRM_WORD)( pReboundLicense->containerOuter.containerGlobalPolicies.Rights.wValue | XMR_RIGHTS_CANNOT_PERSIST );
            }
            else
            {
                pReboundLicense->containerOuter.containerGlobalPolicies.Rights.fValid         = TRUE;
                pReboundLicense->containerOuter.containerGlobalPolicies.Rights.wValue         = XMR_RIGHTS_CANNOT_PERSIST;
            }

            ChkDR( _Prnd_Transmitter_Remove_AppID( &pReboundLicense->containerOuter.containerPlaybackPolicies ) );
            ChkDR( _Prnd_Transmitter_Remove_Invalid_PlayEnabler( &pReboundLicense->containerOuter.containerPlaybackPolicies.UnknownContainer ) );

            /* Build and sign the new license */
            ChkDR( DRM_XMR_StartLicense(
                 pbUnpackStack,
                 cbUnpackStack,
                 &oNewLid,
                 XMR_VERSION_3,
                 &oXmrBuilderCtx ) );

            ChkDR( DRM_XMR_AddObject(
                &oXmrBuilderCtx,
                XMR_OBJECT_TYPE_OUTER_CONTAINER,
                &pReboundLicense->containerOuter ) );

            oSignData.poBlackBoxContext = &poAppContextInternal->oBlackBoxContext;
            oSignData.handleCI          = handleCI;
            ChkDR( DRM_DWordSub( cbNewLicenseBuffer, SIZEOF( DRM_DWORD ), &cbNewLicense ) ); /* Space for priority */

            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbNewLicenseBuffer, SIZEOF( DRM_DWORD ), (DRM_DWORD_PTR*)&pbResult ) ); /* Space for priority */

            dr = DRM_XMR_FinishLicense(
                &oXmrBuilderCtx,
                &oSignData,
                _Prnd_Transmitter_SignLicense,
                pbResult,
                &cbNewLicense );
            if( dr == DRM_E_BUFFERTOOSMALL )
            {
                DRMASSERT( FALSE );         /* We should always have allocated enough space. */
                dr = DRM_E_OUTOFMEMORY;
            }
            ChkDR( dr );

            /* License is now bound to receiver: add it to the PRND store. */
            ChkDR( DRM_LST_AddLicense(
                &pSession->oLSTContextPRNDStore,
                cbNewLicense,           /* Size WITHOUT priority */
                pbNewLicenseBuffer,     /* Buffer WITH priority */
                &poAppContextInternal->oFFLicense.rgkid[idx],
                &oNewLid,
                0 ) );

            DRM_BBX_SafeReleaseKey( &poAppContextInternal->oBlackBoxContext, &handleCI );
        }
    }

ErrorExit:

    if( poAppContextInternal != NULL )
    {
        DRM_BBX_SafeReleaseKey( &poAppContextInternal->oBlackBoxContext, &handleCI );
    }

    ChkECC( ECC_Drm_Prnd_Transmitter_PrepareLicensesForTransmit, dr );
    TRACE(( "Drm_Prnd_Transmitter_PrepareLicensesForTransmit Exited {dr = 0x%x}", dr ));
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Prnd_Transmitter_IsSessionStillValid(
    __in                                                              DRM_APP_CONTEXT                                  *f_poAppContext,
    __out                                                             DRM_BOOL                                         *f_pfSessionStillValid )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal   = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext        != NULL );
    ChkArg( f_pfSessionStillValid != NULL );

    ChkArg( poAppContextInternal->poPrndTransmitterContext != NULL );
    _RefreshPrndTransmitterContextFromAppContext( f_poAppContext, poAppContextInternal->poPrndTransmitterContext );

    ChkDR( DRM_PRND_Transmitter_IsSessionStillValid(
        poAppContextInternal->poPrndTransmitterContext,
        f_pfSessionStillValid ) );

ErrorExit:
    ChkECC( ECC_Drm_Prnd_Transmitter_IsSessionStillValid, dr );
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;

