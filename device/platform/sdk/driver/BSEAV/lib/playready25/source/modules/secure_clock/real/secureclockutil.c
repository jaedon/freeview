/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SECURECLOCKUTIL_C
#include <drmsecureclock.h>
#include <drmchain.h>
#include <drmlicreason.h>
#include <drmutilitieslite.h>
#include <drmantirollbackclock.h>
#include <drmsecureclockconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CLK_IsSecureClockSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CLK_IsSecureClockUnsupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CLK_IsSecureClockExternSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CLK_IsSecureClockExternUnsupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_VOID DRM_CALL DRM_LIC_LoadSecureTimeData(
	__in_opt DRM_VOID 					 *f_pOEMContext,
    __inout  DRM_FFLICENSE               *f_pFFLicense,
    __in     DRM_DST                     *f_pDatastore )
{
    /*
    ** Intentionally use hr, not dr, here, and don't use Chk* macros.
    ** This function doesn't return an error by design because
    ** the caller does not want to handle any errors.
    */
    DRM_RESULT hr = DRM_SUCCESS;

    if( f_pFFLicense != NULL
     && !f_pFFLicense->pLicEval->fSSecureTimeDataLoaded )
    {
        f_pFFLicense->pLicEval->fSSecureTimeDataLoaded = TRUE;

        hr = DRM_SST_CreateGlobalStorePassword( f_pOEMContext, f_pFFLicense->rgbLicensePassword,
                                  ( DRM_BYTE * )f_pFFLicense->pLicEval->pcontextBBX );
        if( hr == DRM_SUCCESS )
        {
            DRM_DWORD               dwSecureData  = 0;
            DRM_SECSTORE_CLOCK_DATA oClkData      = { 0 };

            ZEROMEM( &oClkData, SIZEOF( oClkData ) );
            dwSecureData = SIZEOF( oClkData );

            hr = DRM_SST_GetData( &( f_pFFLicense->pSecStoreLicense[f_pFFLicense->dwChainDepth] ),
                                  &g_idSData,
                                  &g_idSDataPrev,
                                   f_pFFLicense->rgbLicensePassword,
                                   SECURE_STORE_GLOBAL_DATA,
                                   f_pDatastore,
                                  ( DRM_BYTE * ) &oClkData,
                                  &dwSecureData );

            if( hr == DRM_SUCCESS )
            {
                _PrepareSecureClockDataForReading( &oClkData );

                /*
                ** Now Test the Values
                */
                if ( ( oClkData.flag&CLK_LK_GP_VALID ) && ( oClkData.flag&CLK_IN_GP_VALID ) )
                {
                    f_pFFLicense->pLicEval->fSecureTimeDataValid = TRUE;
                    if ( oClkData.fInGracePeriod )
                    {
                        f_pFFLicense->pLicEval->fInGracePeriod = TRUE;
                    }
                    f_pFFLicense->pLicEval->u64GPStartTime = oClkData.LastKnownGracePeriodStartTime;
                }
            }
        }
    }

    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_LIC_CheckGracePeriod(
    __in     DRM_FFLICENSE               *f_pFFLicense )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pFFLicense != NULL );

    if ( f_pFFLicense->pLicEval->eTimeBasedState == LICEVAL_MACHINEDATETIMEREFERENCED
     && f_pFFLicense->pLicEval->fInGracePeriod )
    {
        f_pFFLicense->pLicEval->lReasonForFail = LR_LICENSE_CLOCK_NOT_SET;

        ChkDR( DRM_E_RIGHTS_NOT_AVAILABLE );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_LEVL_CheckGracePeriodXMR(
    __inout       DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in          DRM_UINT64                    f_ui64CurrentDateTime,
    __in    const DRM_XMR_LICENSE              *f_pXMR,
    __out         DRM_BOOL                     *f_pfGracePeriodExceeded )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;

    ChkArg( f_pContextLicEval       != NULL );
    ChkArg( f_pXMR                  != NULL );
    ChkArg( f_pfGracePeriodExceeded != NULL );

    /*
    ** If the license specifies a grace period,
    */
    if ( XMR_IS_GRACE_PERIOD_VALID( f_pXMR ) )
    {
        /*
        ** Indicate that the license has processed the grace period
        */
        f_pContextLicEval->eTimeBasedState = LICEVAL_GRACEPERIODREFERENCED;

        /*
        ** If we're in the grace period, make sure we're within the limit allowed by the license
        */
        if ( f_pContextLicEval->fInGracePeriod )
        {
            /*
            ** Override current datetime with antirollback-clock datetime (if supported)
            */
            ChkDR( DRM_ARCLK_LEVL_GetMachineDatetime( f_pContextLicEval, &f_ui64CurrentDateTime ) );

            /*
            ** Can only exceed grace period if current time is after GP Start Time
            */
            if ( DRM_UI64Les( f_pContextLicEval->u64GPStartTime, f_ui64CurrentDateTime )
              && DRM_I64ToUI32( DRM_I64Div( DRM_UI2I64( DRM_UI64Sub( f_ui64CurrentDateTime, f_pContextLicEval->u64GPStartTime ) ), DRM_I64( C_TICS_PER_SECOND ) ) ) >= f_pXMR->containerOuter.containerGlobalPolicies.GracePeriod.dwValue )
            {
                *f_pfGracePeriodExceeded = TRUE;
                goto ErrorExit;
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_CLK_VariableSecureTimeGetorSet(
    __in    const DRM_CONST_STRING  *pdstrToken,
    __in          TOKEN             *pNewValue,
    __inout       TOKEN             *pResult,
    __inout       DRM_VOID          *pvOpaqueData,
    __in          DRM_BOOL           fSet )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LICEVAL_CONTEXT* pLicEval = (DRM_LICEVAL_CONTEXT*) pvOpaqueData;


    if( fSet )
    {
        ChkDR(DRM_E_CPRMEXP_UPDATE_UNSUPPORTED);
    }

    if( pLicEval == NULL )
    {
        ChkDR(DRM_E_CPRMEXP_RETRIEVAL_FAILURE);
    }

    if( !pLicEval->fSecureTimeDataValid )
    {
        ChkDR(DRM_E_CPRMEXP_RETRIEVAL_FAILURE);
    }

    /* fInGracePeriod variable*/
    /*Set the flag in lic Eval context if this function was ever called, to know that if Grace period expression exists in license.
     * It is called before validation of variables, but it is fine since the function will error out if variables doesn't match.
    */
    pLicEval->eTimeBasedState = LICEVAL_GRACEPERIODREFERENCED;

    if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarInGracePeriod, pdstrToken ) )
    {
        pResult->TokenType  = TOKEN_LONG;
        pResult->val.lValue = (DRM_LONG)pLicEval->fInGracePeriod;
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarGpStartTime, pdstrToken ) )
    {
        pResult->TokenType  = TOKEN_DATETIME;
        MEMCPY(&pResult->val.u64DateTime, &pLicEval->u64GPStartTime, SIZEOF( pResult->val.u64DateTime ) ) ;
    }
    else
    {
        ChkDR(DRM_E_CPRMEXP_RETRIEVAL_FAILURE);
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;


