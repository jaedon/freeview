/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMETERAPI_C
#include <drmmeterapi.h>
#include <drmmeterimp.h>
#include <drmxmlparser.h>
#include <drmcontract.h>
#include <drmsoapxmlutility.h>
#include <drmmanagerimpl.h>
#include <drmmeterconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: _IsV2MeterCert
**
** Synopsis: Function that checks whether a metering certificate is a V2
**           metering certificate.
**
** Arguments:
**
** [f_pdstrMeterCert]       -- Pointer to a DRM string that contains the
**                             certificate data to be verified.
**
** Returns:                 TRUE if it is a V2 certificate.
**                          FALSE if it is not a V2 certifcate.
**
**********************************************************************/
static DRM_BOOL _IsV2MeterCert(
    __in const DRM_CONST_STRING *f_pdstrMeterCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrListTop = EMPTY_DRM_STRING;

    ChkDR( DRM_XML_GetNode( f_pdstrMeterCert,
                            &g_dstrTagMeterCert,
                            NULL,
                            NULL,
                            0,
                            NULL,
                            &dstrListTop ) );

ErrorExit:

    return DRM_SUCCEEDED( dr );
}

/*********************************************************************
**
** Function: _IsV2MeterResponse
**
** Synopsis: Function that checks whether a meter data response is a V2
**           meter data response.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the
**                             response from the meter server.
** [f_cbResponse]           -- Size (number of bytes) of the response
**                             from the meter server.
**
** Returns:                 TRUE if it is a V2 response.
**                          FALSE if it is not a V2 response.
**
**********************************************************************/
static DRM_BOOL _IsV2MeterResponse(
    __in_bcount(f_cbResponse) const DRM_BYTE *f_pbResponse,
    __in                      const DRM_DWORD f_cbResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrXML = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListTop = EMPTY_DRM_STRING;

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    DSTR_FROM_PB( &dstrXML, f_pbResponse, f_cbResponse );

    ChkDR( DRM_XML_GetNode( &dstrXML,
                            &g_dstrTagMetering,
                            NULL,
                            NULL,
                            0,
                            NULL,
                            &dstrListTop ) );

ErrorExit:

    return DRM_SUCCEEDED( dr );
}

/**********************************************************************
**
** Function:    Drm_MeterCert_InitEnum
**
** Synopsis:    Initializes an enumeration context to enumerate through all metering certificates stored
**
** Arguments:   [f_pcontextMGR]           -- Opaque DRM Application context initialized by a call to Drm_Initialize
**              [f_pcontextMeterCertEnum] -- Meter cert enumeration context to be initialized
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_InitEnum(
    __in   DRM_APP_CONTEXT     *f_pcontextMGR,
    __out  DRM_METERCERT_ENUM  *f_pcontextMeterCertEnum )
{
    DRM_APP_CONTEXT_INTERNAL     *pcontext = (DRM_APP_CONTEXT_INTERNAL *)f_pcontextMGR;
    DRM_RESULT                    dr       = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_InitEnum);

    ChkArg( f_pcontextMGR != NULL
         && f_pcontextMeterCertEnum != NULL );

    ChkDR( DRM_MCS_InitEnum( &pcontext->oMeterCertContext, f_pcontextMeterCertEnum ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    ChkECC( ECC_Drm_MeterCert_InitEnum, dr );
    return dr;
}

/**********************************************************************
**
** Function:    Drm_MeterCert_EnumNext
**
** Synopsis:    Enumerates to the next metering cert in the metering cert store
**
** Arguments:   [f_pMeterCertEnumContext] -- Meter cert enumeration context
**              [f_pmid]                  -- Buffer to hold the meter ID of the enumerated meter cert
**              [f_pwszLAINFO]            -- Buffer to hold the enumerated LAINFO
**              [f_pcchLAINFO]            -- On input specifies the number of wide characters available in the buffer,
**                                           on output specifies the number of characters in the returned LAINFO
**              [f_pfIsV2]                -- Returns whether the server pointed to by the LAINFO URL is a V2 or V4 server (ie WMDRM-PD or PlayReady )
**              [f_pbMeterCert]           -- Buffer to hold the enumerated meter cert
**              [f_pcbMeterCert]          -- On input specifies the number of bytes available in the buffer,
**                                           on output specifies the number of bytes in the returned meter cert
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_EnumNext(
    __in                                DRM_METERCERT_ENUM  *f_pcontextMeterCertEnum,
    __inout                             DRM_MID             *f_pmid,
    __out_ecount_opt( *f_pcchLAINFO )   DRM_WCHAR           *f_pwszLAINFO,
    __out_opt                           DRM_DWORD           *f_pcchLAINFO,
    __out_opt                           DRM_BOOL            *f_pfIsV2,
    __out_bcount_opt( *f_pcbMeterCert ) DRM_BYTE            *f_pbMeterCert,
    __out_opt                           DRM_DWORD           *f_pcbMeterCert )
{
    DRM_RESULT                    dr       = DRM_SUCCESS;
    DRM_DST_SLOT_HINT             slotHint = {0};
    DRM_METERCERT_ENUM_PRIVATE   *pcontextPriv = ( DRM_METERCERT_ENUM_PRIVATE* )f_pcontextMeterCertEnum;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_EnumNext);

    ChkArg( f_pcontextMeterCertEnum != NULL
        && f_pmid != NULL );

    /* If their last call to enum next failed, likely due to a buffer too small error,
    ** then we shouldn't go to the next slot
    */
    if( !pcontextPriv->fSkipEnumNext )
    {
        ChkDR( DRM_MCS_EnumNext( ( DRM_METERCERT_ENUM* )f_pcontextMeterCertEnum, f_pmid, &slotHint ) );
    }

    dr = DRM_MCS_GetMeterCertByMID( ( DRM_METERCERT_CONTEXT* )pcontextPriv->pcontextMeterCert,
                                    f_pmid,
                                    f_pwszLAINFO,
                                    f_pcchLAINFO,
                                    f_pfIsV2,
                                    f_pbMeterCert,
                                    f_pcbMeterCert );

    if ( DRM_FAILED( dr ) )
    {
        pcontextPriv->fSkipEnumNext = TRUE;
        ChkDR( dr );
    }
    pcontextPriv->fSkipEnumNext = FALSE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    ChkECC( ECC_Drm_MeterCert_EnumNext, dr );
    return dr;
}

/**********************************************************************
**
** Function:    Drm_MeterCert_Update
**
** Synopsis:    Updates a metering certificate in the metering certificate store
**
** Arguments:   [f_pcontextMGR]     -- initialized DRM Application context
**              [f_pbMeterCert]     -- meter cert to store
**              [f_cbMeterCert]     -- size of meter cert to store.
**              [f_pmid]            -- optional pointer to MID that was added/updated
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Update(
    __in                         DRM_APP_CONTEXT   *f_pcontextMGR,
    __in_bcount( f_cbMeterCert ) DRM_BYTE          *f_pbMeterCert,
    __in                         DRM_DWORD          f_cbMeterCert,
    __out_opt                    DRM_MID           *f_pmid )
{
    DRM_RESULT                    dr           = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL     *pcontext     = (DRM_APP_CONTEXT_INTERNAL*)f_pcontextMGR;
    DRM_CONST_STRING              dstrMeterCert = EMPTY_DRM_STRING;
    DRM_MID                       midMeterCert = {0};
    DRM_BOOL                      fIsV2        = TRUE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_Update);

    ChkArg( f_pcontextMGR != NULL
        && f_pbMeterCert != NULL
        && f_cbMeterCert > 0 );

    DSTR_FROM_PB( &dstrMeterCert, f_pbMeterCert, f_cbMeterCert );

    if ( _IsV2MeterCert( &dstrMeterCert ) )
    {
        /* Perform basic validation and signature checking on the meter cert. */
        ChkDR( DRM_MTR_ParseMeterCert( &dstrMeterCert,
                                       &pcontext->oLicEvalContext,
                                       NULL,
                                       NULL,
                                       &midMeterCert,
                                       NULL ) );
    }
    else
    {
        /* Try to parse the meter cert as a binary meter cert. */
        PUBKEY_P256 oServerPubKey = { 0 };

        ChkDR( DRM_MTR_ParseBinaryMeterCert( &pcontext->oBlackBoxContext,
                                             &pcontext->oDatastoreHDS,
                                             &pcontext->oSecStoreGlobalContext,
                                             &pcontext->oRevContext,
                                             pcontext->pbRevocationBuffer,
                                             pcontext->cbRevocationBuffer,
                                             f_pbMeterCert,
                                             f_cbMeterCert,
                                             &midMeterCert,
                                             NULL,
                                             NULL,
                                             &oServerPubKey ) );
        fIsV2 = FALSE;
    }

    if ( f_pmid != NULL )
    {
        MEMCPY( f_pmid, &midMeterCert, SIZEOF( DRM_MID ) );
    }

    ChkDR( DRM_MCS_UpdateMeterCert( &pcontext->oMeterCertContext, NULL, fIsV2, f_pbMeterCert, f_cbMeterCert, &midMeterCert ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    ChkECC( ECC_Drm_MeterCert_Update, dr );
    return dr;
}

/**********************************************************************
**
** Function:    Drm_MeterCert_Invalidate
**
** Synopsis:    Invalidates a metering certificate
**
** Arguments:   [f_pcontextMGR]  -- Opaque DRM Application context initialized by a call to Drm_Initialize
**              [f_pmid]         -- Meter ID of the meter cert to invalidate
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Invalidate(
    __in       DRM_APP_CONTEXT  *f_pcontextMGR,
    __in const DRM_MID          *f_pmid )
{
    DRM_RESULT                dr       = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pcontext = (DRM_APP_CONTEXT_INTERNAL*)f_pcontextMGR;

    ChkArg( f_pcontextMGR != NULL
        && f_pmid != NULL );

    ChkDR( DRM_MCS_InvalidateMeterCert( &pcontext->oMeterCertContext, f_pmid ) );

ErrorExit:
    ChkECC( ECC_Drm_MeterCert_Invalidate, dr );
    return dr;
}

/**********************************************************************
**
** Function:    Drm_MeterCert_Delete
**
** Synopsis:    Deletes a metering certificate
**
** Arguments:   [f_pcontextMGR]  -- Opaque DRM Application context initialized by a call to Drm_Initialize
**              [f_pmid]         -- Meter ID of the meter cert to delete
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the parameters are invalid or
**              any of the return codes as defined in drmresults.h
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Delete(
    __in       DRM_APP_CONTEXT  *f_pcontextMGR,
    __in const DRM_MID          *f_pmid )
{
    DRM_RESULT                dr       = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pcontext = (DRM_APP_CONTEXT_INTERNAL*)f_pcontextMGR;

    ChkArg( f_pcontextMGR != NULL
        && f_pmid != NULL );

    ChkDR( DRM_MCS_DeleteMeterCert( &pcontext->oMeterCertContext, f_pmid ) );

ErrorExit:
    ChkECC( ECC_Drm_MeterCert_Delete, dr );
    return dr;
}

/**********************************************************************
**
** Function:    Drm_Metering_GenerateChallenge
**
** Synopsis:    Generates a challenge that includes all the metering
**              information for a specific MID identified by the
**              provided metering certificate.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbMeterCert]          -- Pointer to a metering certificate that identifies
**                             the metering ID that wants to collect data.
** [f_cbMeterCert]          -- Size of the metering certificate.
** [f_pchCustomData]        -- Pointer to a buffer that contains custom data to
**                             be sent to the server. It may be NULL.
** [f_cchCustomData]        -- Size ( number of characters ) of the custom data.
** [f_pszUrl]               -- User allocated buffer to get URL in CHAR.
** [f_pcchUrl]              -- Character count of f_pszUrl. Can be NULL if URL is not required.
** [f_pbChallenge]          -- Buffer to hold the metering challenge.
** [f_pcbChallenge]         -- Size in bytes of f_pbChallenge on input. Number of bytes
**                             copied to f_pbChallenge on return or length required if the
**                             buffer was too small.
**
** Returns:                 DRM_SUCCESS on success or
**                          DRM_S_FALSE if the meter data challenge does not contains the
**                          meter data of all KIDs (SOAP based V3 protocol only).
**                          DRM_E_INVALIDARG if any of the parameters are invalid or
**                          any of the return codes as defined in drmresults.h.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DEVCERT_EXCEEDS_SIZE_LIMIT if the device cert is too big.
**                          DRM_E_NOMORE if no KID is ready to be processed
**                          (SOAP based V3 protocol only).
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Metering_GenerateChallenge(
    __in                                DRM_APP_CONTEXT   *f_poAppContext,
    __in_bcount( f_cbMeterCert ) const  DRM_BYTE          *f_pbMeterCert,
    __in                                DRM_DWORD          f_cbMeterCert,
    __in_ecount_opt( f_cchCustomData )  DRM_CHAR          *f_pchCustomData,
    __in_opt                            DRM_DWORD          f_cchCustomData,
    __out_ecount_opt( *f_pcchURL )      DRM_CHAR          *f_pchURL,
    __inout_opt                         DRM_DWORD         *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE          *f_pbChallenge,
    __inout                             DRM_DWORD         *f_pcbChallenge )
{
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *)f_poAppContext;
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_CONST_STRING          dstrMeterCert        = EMPTY_DRM_STRING;
    DRM_REVOCATIONSTORE_CONTEXT *poRevStoreContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_Drm_Metering_GenerateChallenge );

    ChkArg ( f_poAppContext    != NULL
         && f_pbMeterCert      != NULL
         && f_cbMeterCert      >  0
         && f_pcbChallenge     != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if ( !poAppContextInternal->fStoreOpened )
    {
        ChkDR ( DRM_E_DRM_NOT_INITIALIZED );
    }

    if ( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_METERING ) == 0 )
    {
        ChkDR ( DRM_E_METERING_NOT_SUPPORTED );
    }

    /*
    ** Clear the previous response fields in the app context.
    */
    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    DSTR_FROM_PB( &dstrMeterCert, f_pbMeterCert, f_cbMeterCert );

    if ( _IsV2MeterCert( &dstrMeterCert ) )
    {
        ChkDR( DRM_MTR_GenerateMeterChallenge( &poAppContextInternal->oLicEvalContext,
                                               poAppContextInternal->pbDRMLicense,
                                               poAppContextInternal->cbDRMLicense,
                                               &dstrMeterCert,
                                               ( _XMBContext* )f_pbChallenge,
                                               f_pcbChallenge,
                                               f_pchURL,
                                               f_pcchURL ) );
    }
    else
    {
        DRM_DWORD cbDeviceCert = 0;
        DRM_DWORD cbLicenseBuffer = poAppContextInternal->cbDRMLicense;
        DRM_DWORD dwAlignAdjust = 0;
        DRM_BYTE *pbStack = NULL;
        DRM_DWORD cbStack = 0;

        ChkDR( DRM_SOAPXML_GetDeviceCert( poAppContextInternal->oBlackBoxContext.pKeyFileContext,
                                          FALSE,
                                          NULL,
                                          &cbDeviceCert ) );

        ChkBOOL( cbDeviceCert < cbLicenseBuffer, DRM_E_DEVCERT_EXCEEDS_SIZE_LIMIT );

        ChkDR( DRM_SOAPXML_GetDeviceCert( poAppContextInternal->oBlackBoxContext.pKeyFileContext,
                                          FALSE,
                                          ( DRM_CHAR * )poAppContextInternal->pbDRMLicense,
                                          &cbDeviceCert ) );

        ChkBOOL( cbDeviceCert < cbLicenseBuffer, DRM_E_DEVCERT_EXCEEDS_SIZE_LIMIT );

        pbStack = poAppContextInternal->pbDRMLicense + cbDeviceCert;
        cbStack = cbLicenseBuffer - cbDeviceCert;

        /* Align the starting address of the stack buffer to DRM_DWORD. */
        dwAlignAdjust = ( SIZEOF( DRM_DWORD_PTR ) - ( ( ( DRM_DWORD_PTR )pbStack ) %
                        SIZEOF( DRM_DWORD_PTR ) ) ) % SIZEOF( DRM_DWORD_PTR );

        if ( cbStack > dwAlignAdjust )
        {
            pbStack += dwAlignAdjust;
            cbStack -= dwAlignAdjust;
        }

        if( DRM_REVOCATION_IsRevocationSupported() )
        {
            poRevStoreContext = &poAppContextInternal->oRevContext;
        }

        ChkDR( DRM_MTR_GenerateMeterDataChallenge( &poAppContextInternal->oBlackBoxContext,
                                                   &poAppContextInternal->oDatastoreHDS,
                                                   &poAppContextInternal->oSecStoreGlobalContext,
                                                   poRevStoreContext,
                                                   poAppContextInternal->pbRevocationBuffer,
                                                   poAppContextInternal->cbRevocationBuffer,
                                                   pbStack,
                                                   cbStack,
                                                   f_pbMeterCert,
                                                   f_cbMeterCert,
                                                   poAppContextInternal->pbDRMLicense,
                                                   cbDeviceCert,
                                                   NULL,
                                                   0,
                                                   f_pchURL,
                                                   f_pcchURL,
                                                   f_pbChallenge,
                                                   f_pcbChallenge ) );
    }

ErrorExit:

    ChkECC( ECC_Drm_Metering_GenerateChallenge, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Metering_ProcessResponse
**
** Synopsis:    A meter repsonse is sent to the device after a sever
**              process the challenge.  This response is handled
**              by resetting the metering data just reported.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Input buffer containing the response to process.
** [f_cbResponse]           -- Size of the response.
** [f_pfFlags]              -- Status output flags.
**
** Returns:                 DRM_SUCCESS on success or
**                          DRM_E_INVALIDARG if the argument is invalid or
**                          any return code as defined in drmresults.h.
**
** Notes:                   For V2 protocol, If DRM_METER_RESPONSE_PARTIAL is returned
**                          in f_pfFlags it indicates that the challenge/response
**                          round trip was successful but only a partial amout
**                          of the entire metering data was sent. The application
**                          should perform the entire Drm_Metering_GenerateChallenge/
**                          Drm_Metering_ProcessReponse cycle again.
**                          For SOAP based V3 protocol, f_pfFlags is used to receive the
**                          status code from the server.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Metering_ProcessResponse(
    __in                        DRM_APP_CONTEXT  *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE         *f_pbResponse,
    __in                        DRM_DWORD         f_cbResponse,
    __out                       DRM_DWORD        *f_pfFlags )
{
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *)f_poAppContext;
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 cbScratch            = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_Drm_Metering_ProcessResponse );

    ChkArg ( f_poAppContext   != NULL
         &&  f_pbResponse     != NULL
         &&  f_pfFlags        != NULL
         &&  f_cbResponse     >  0 );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if( !poAppContextInternal->fStoreOpened )
    {
        ChkDR ( DRM_E_DRM_NOT_INITIALIZED );
    }

    if ( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_METERING ) == 0 )
    {
        ChkDR ( DRM_E_METERING_NOT_SUPPORTED );
    }

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    if ( _IsV2MeterResponse( f_pbResponse, f_cbResponse ) )
    {
        dr = DRM_MTR_ProcessMeterResponse( &poAppContextInternal->oLicEvalContext,
                                            poAppContextInternal->pbDRMLicense,
                                            poAppContextInternal->cbDRMLicense,
                                            f_pbResponse,
                                            f_cbResponse,
                                            f_pfFlags );
    }
    else
    {
        DRM_REVOCATIONSTORE_CONTEXT *poRevStoreContext = NULL;
        DRM_BYTE *pbRevBuffer = NULL;
        DRM_DWORD cbRevBuffer = 0;

        cbScratch = poAppContextInternal->cbDRMLicense;

        if( DRM_REVOCATION_IsRevocationSupported() )
        {
            poRevStoreContext = poAppContextInternal->oLicEvalContext.pContextRev;
            pbRevBuffer = poAppContextInternal->oLicEvalContext.pbRevocationBuffer;
            cbRevBuffer = poAppContextInternal->oLicEvalContext.cbRevocationBuffer;
        }

        ChkDR( DRM_MTR_ProcessMeterDataResponse( &poAppContextInternal->oBlackBoxContext,
                                                 &poAppContextInternal->oDatastoreHDS,
                                                 poAppContextInternal->oLicEvalContext.pcontextSSTRevocation,
                                                 poRevStoreContext,
                                                 pbRevBuffer,
                                                 cbRevBuffer,
                                                 poAppContextInternal->pbDRMLicense,
                                                 &cbScratch,
                                                 f_pbResponse,
                                                 f_cbResponse,
                                                 &poAppContextInternal->fResponseSignatureChecked,
                                                 &poAppContextInternal->dwLastResponseProtocol,
                                                 ( DRM_RESULT * )f_pfFlags ) );
    }

ErrorExit:

    ChkECC( ECC_Drm_Metering_ProcessResponse, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_METERING_UpdateData
**
** Synopsis: API that updates metering data in the SST
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_ppdstrActions]        -- Array of actions performed
** [f_cActions]             -- Count of actions performed
**
** Returns:                 DRM_SUCCESS on success or
**                          DRM_E_INVALIDARG if the argument is invalid or
**                          any return code as defined in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_METERING_UpdateData(
    __in                        DRM_APP_CONTEXT   *f_poAppContext,
    __in                  const DRM_CONST_STRING **f_ppdstrActions,
    __in                        DRM_DWORD          f_cActions )
{
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *)f_poAppContext;
    DRM_RESULT                dr                   = DRM_SUCCESS;

    ChkArg ( f_poAppContext   != NULL );

    ChkDR( DRM_MTR_UpdateData( &poAppContextInternal->oFFLicense.rgmid[ poAppContextInternal->dwChainDepth ],
                                &poAppContextInternal->oFFLicense.rglid[ poAppContextInternal->dwChainDepth ],
                                &poAppContextInternal->oFFLicense.rgkid[ poAppContextInternal->dwChainDepth ],
                                f_ppdstrActions,
                                f_cActions,
                                ( DRM_METERING_CONTEXT * ) &poAppContextInternal->rgcontextSST[ poAppContextInternal->dwChainDepth ],
                                &poAppContextInternal->oDatastoreHDS,
                                &poAppContextInternal->oBlackBoxContext ) );
ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Drm_MeterCert_GenerateChallenge
**
** Synopsis: API that generates a meter certificate challenge XML to
**           be sent to the server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poMID]                -- Pointer to a meter ID (MID).
** [f_pchCustomData]        -- Pointer to a buffer that contains custom data to
**                             be sent to the server. It may be NULL.
** [f_cchCustomData]        -- Size ( number of characters ) of the custom data.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated
**                             challenge byte stream. If it is NULL, the
**                             function returns an error and the size of
**                             the required buffer is stored in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the
**                             size (in bytes) of the challenge buffer
**                             during input and receives the size (in
**                             bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_GenerateChallenge(
    __in                                    DRM_APP_CONTEXT  *f_poAppContext,
    __in                              const DRM_MID          *f_poMID,
    __in_ecount_opt( f_cchCustomData) const DRM_CHAR         *f_pchCustomData,
    __in_opt                                DRM_DWORD         f_cchCustomData,
    __out_bcount_opt( *f_pcbChallenge )     DRM_BYTE         *f_pbChallenge,
    __inout                                 DRM_DWORD        *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_GenerateChallenge );

    ChkArg( f_poAppContext != NULL );
    ChkDR( DRM_MTR_GenerateMeterCertChallenge( &poAppContextInternal->oBlackBoxContext,
                                               f_poMID,
                                               f_pchCustomData,
                                               f_cchCustomData,
                                               f_pbChallenge,
                                               f_pcbChallenge ) );

ErrorExit:

    ChkECC( ECC_Drm_MeterCert_GenerateChallenge, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: Drm_MeterCert_ProcessResponse
**
** Synopsis: API that processes a meter certificate response from the
**           server. It extracts the embedded meter certificate and
**           stores it into the meter certificate store.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the meter
**                             certificate response from the server.
** [f_cbResponse]           -- Size of the meter certificate response from
**                             the server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to an variable that receives
**                             the status code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_METERCERT_NOT_FOUND - if meter certificate
**                          was not found in response
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_ProcessResponse(
    __in                        DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    __in                        DRM_DWORD        f_cbResponse,
    __out                       DRM_RESULT      *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD cbScratch = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_ProcessResponse );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    cbScratch = poAppContextInternal->cbDRMLicense;

    ChkDR( DRM_MTR_ProcessMeterCertResponse( &poAppContextInternal->oLicEvalContext,
                                             &poAppContextInternal->oMeterCertContext,
                                             poAppContextInternal->pbDRMLicense,
                                             &cbScratch,
                                             f_pbResponse,
                                             f_cbResponse,
                                             f_pResult ) );

ErrorExit:

    ChkECC( ECC_Drm_MeterCert_ProcessResponse, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
