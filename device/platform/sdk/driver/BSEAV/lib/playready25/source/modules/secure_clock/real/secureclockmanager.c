/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SECURECLOCKMANAGER_C
#include <drmsecureclock.h>
#include <drmsecureclockconstants.h>
#include <drmcontract.h>
#include <devicedevcert.h>
#include <drmmanagerimpl.h>
#include <drmwmdrm.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_CLK_CheckSecureClock(
    __in DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal )
{
    DRM_RESULT            dr                   = DRM_SUCCESS;
    DRM_SECSTORE_CONTEXT *poSecureStoreContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_CheckSecureClock );

    /* Result is first DRM_SHA1_DIGEST_LEN bytes of rgbDRMLicense. */
    ChkDR( DRM_SST_CreateGlobalStorePassword(f_poAppContextInternal->pOEMContext,
            f_poAppContextInternal->pbDRMLicense,
            ( DRM_BYTE * )&f_poAppContextInternal->oBlackBoxContext ) );

    /* As only DRM_SHA1_DIGEST_LEN bytes are getting used for password,
       other bytes can be used for DRM_SECSTORE_CONTEXT. */
    poSecureStoreContext =
        ( DRM_SECSTORE_CONTEXT * )( f_poAppContextInternal->pbDRMLicense +
        __CB_DECL( DRM_SHA1_DIGEST_LEN ) );

    ChkDR( DRM_CLK_InitCheckSecureClock(  f_poAppContextInternal->pbDRMLicense,
                                         &f_poAppContextInternal->oBlackBoxContext.CryptoContext,
                                         &f_poAppContextInternal->oDatastoreHDS,
                                         &f_poAppContextInternal->fClockSet,
                                          poSecureStoreContext,
                                         &f_poAppContextInternal->oLicEvalContext,
                                          f_poAppContextInternal->pOEMContext ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_ProcessResponseImpl(
    __in                        DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    __in                        DRM_DWORD        f_cbResponse,
    __out                       DRM_RESULT      *f_pResult )
{
    DRM_RESULT                dr                        = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal      = ( DRM_APP_CONTEXT_INTERNAL* )f_poAppContext;
    DRM_SECSTORE_CONTEXT     *poSecureStoreContext      = NULL;
    const PUBKEY             *ppubkeySecureClockServer  = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_Drm_SecureClock_ProcessResponseImpl );

    ChkArg(  f_poAppContext != NULL
          && f_pbResponse != NULL
          && f_cbResponse != 0
          && f_pResult != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if ( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags &
           DRM_FEATURE_SECURE_CLOCK ) == 0 )
    {
        ChkDR( DRM_E_CLK_NOT_SUPPORTED );
    }

    /* Result is first DRM_SHA1_DIGEST_LEN bytes of rgbDRMLicense. */
    ChkDR( DRM_SST_CreateGlobalStorePassword( poAppContextInternal->pOEMContext, poAppContextInternal->pbDRMLicense,
                                              ( DRM_BYTE * )&poAppContextInternal->oBlackBoxContext ) );

   /*
   ** As only DRM_SHA1_DIGEST_LEN bytes are getting used for password,
   ** other bytes can be used for DRM_SECSTORE_CONTEXT.
   */
    poSecureStoreContext =
        ( DRM_SECSTORE_CONTEXT * )( poAppContextInternal->pbDRMLicense +
        __CB_DECL( DRM_SHA1_DIGEST_LEN ) );

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    /* Explicitly make the global secure store writeabele. */
    poAppContextInternal->oLicEvalContext.fGlobalSecStoreWritable = TRUE;

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ppubkeySecureClockServer = &poAppContextInternal->oBlackBoxContext.cachedCertValues.pubkeySecureClockServer;
    }
    else
    {
        ppubkeySecureClockServer = &g_pubkeyDefaultPlayReadySecureClockServer;
    }

    /* Call DRM_CLK_ProcessResponse(). */
    ChkDR( DRM_CLK_ProcessResponse(  ppubkeySecureClockServer,
                                     f_pbResponse,
                                     f_cbResponse,
                                     poAppContextInternal->pbDRMLicense,
                                     f_pResult,
                                    &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                    &poAppContextInternal->oDatastoreHDS,
                                     poSecureStoreContext,
                                    &poAppContextInternal->oLicEvalContext,
                                     poAppContextInternal->pOEMContext ) );

    /* After successful processing response, Set DRM App context Clock flag. */
    poAppContextInternal->fClockSet = TRUE;

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_SecureClock_ProcessResponse, dr );

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_GenerateChallengeImpl(
    __in                                DRM_APP_CONTEXT *f_poAppContext,
    __out_ecount_opt( *f_pcchURL )      DRM_WCHAR       *f_pwszURL,
    __inout_opt                         DRM_DWORD       *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE        *f_pbChallenge,
    __inout                             DRM_DWORD       *f_pcbChallenge )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_CONST_STRING          dstrURL              = EMPTY_DRM_STRING;
    const DRM_CONST_STRING   *pdstrURL             = NULL;
    DRM_CONST_STRING          dstrDeviceCert       = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_Drm_SecureClock_GenerateChallengeImpl );

    ChkArg(  f_poAppContext != NULL
          && f_pcbChallenge != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if ( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags &
           DRM_FEATURE_SECURE_CLOCK ) == 0 )
    {
        ChkDR( DRM_E_CLK_NOT_SUPPORTED );
    }

    DSTR_FROM_PB( &dstrDeviceCert,
                   poAppContextInternal->pbDRMLicense,
                   poAppContextInternal->cbDRMLicense );

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( DRM_DDC_GetXMLDeviceCertificate( poAppContextInternal->pOEMContext,
                                                poAppContextInternal->pKeyFileContext,
                                                ( DRM_STRING * )&dstrDeviceCert,
                                                0,
                                                &poAppContextInternal->oBlackBoxContext.CryptoContext ) );

        ChkDR( DRM_DCP_GetAttribute( NULL,
                                     &dstrDeviceCert,
                                     DRM_DEVCERT_SECURECLOCKURL,
                                     NULL,
                                     &dstrURL ) );

        pdstrURL = &dstrURL;
    }
    else
    {
        pdstrURL = &g_dstrDefaultPlayReadySecureClockServerUrl;
    }

    /* Result is first DRM_SHA1_DIGEST_LEN bytes of oSecStoreGlobalContext. */
    ChkDR( DRM_SST_CreateGlobalStorePassword( poAppContextInternal->pOEMContext, ( DRM_BYTE * )&poAppContextInternal->oSecStoreGlobalContext,
                                              ( DRM_BYTE * )&poAppContextInternal->oBlackBoxContext ) );

   dr = DRM_CLK_CreateChallenge(  pdstrURL,
                                  ( const DRM_BYTE * )&poAppContextInternal->oSecStoreGlobalContext,
                                 &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                 &poAppContextInternal->oDatastoreHDS,
                                 &poAppContextInternal->rgcontextSST[ 0 ],
                                  poAppContextInternal->pOEMContext,
                                  f_pwszURL,
                                  f_pcchURL,
                                  f_pbChallenge,
                                  f_pcbChallenge );

ErrorExit:

    ChkECC( ECC_Drm_SecureClock_GenerateChallenge, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_CLK_Drm_SecureClock_GetValueImpl(
    __in                                          DRM_APP_CONTEXT *f_poAppContext,
    __in_ecount_opt( *f_pcchSecureTime )          DRM_WCHAR       *f_pwszSecureTime,
    __inout_opt                                   DRM_DWORD       *f_pcchSecureTime,
    __in_opt                                      DRM_DWORD       *f_pdwFlags,
    __in_bcount_opt( *f_pcbSecureTimeWithStatus ) DRM_BYTE        *f_pbSecureTimeWithStatus,
    __inout                                       DRM_DWORD       *f_pcbSecureTimeWithStatus )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_SECSTORE_CONTEXT     *poSecureStoreContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_DRM_CLK_Drm_SecureClock_GetValueImpl );

    ChkArg( f_poAppContext != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if ( ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags &
           DRM_FEATURE_SECURE_CLOCK ) == 0 )
    {
        ChkDR( DRM_E_CLK_NOT_SUPPORTED );
    }

    /* Result is first DRM_SHA1_DIGEST_LEN bytes of rgbDRMLicense. */
    ChkDR( DRM_SST_CreateGlobalStorePassword(poAppContextInternal->pOEMContext,
            poAppContextInternal->pbDRMLicense,
            ( DRM_BYTE * )&poAppContextInternal->oBlackBoxContext ) );

    /*
    ** As only DRM_SHA1_DIGEST_LEN bytes are getting used for password,
    ** other bytes can be used for DRM_SECSTORE_CONTEXT.
    */
    poSecureStoreContext =
        ( DRM_SECSTORE_CONTEXT * )( poAppContextInternal->pbDRMLicense +
        __CB_DECL( DRM_SHA1_DIGEST_LEN ) );

    /* Call DRM_CLK_GetSecureClock(). */
    dr = DRM_CLK_GetSecureClock(  f_pwszSecureTime,
                                  f_pcchSecureTime,
                                  f_pdwFlags,
                                  f_pbSecureTimeWithStatus,
                                  f_pcbSecureTimeWithStatus,
                                  poAppContextInternal->pbDRMLicense,
                                 &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                 &poAppContextInternal->oDatastoreHDS,
                                  poSecureStoreContext,
                                  poAppContextInternal->pOEMContext );

ErrorExit:

    ChkECC( ECC_Drm_SecureClock_GetValue, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


EXIT_PK_NAMESPACE_CODE;

