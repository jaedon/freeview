/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMLICEVALREAL_C
#include <drmlicreason.h>
#include <drmantirollbackclock.h>
#include <drmcontentrevocation.h>
#include <drmsymopt.h>
#include <drmutilities.h>
#include <drmwmdrm.h>
#include <drmwmdrmconstants.h>
#include <drmxmlparser.h>
#include <drmapprevocation.h>
#include <drmcopyopl.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function :   _IsCachedEvent
**
** Synopsis :   Check whether we already have this event cached, and find the
**              index of the entry in cache, if found
**
** Arguments :
**      [f_pdstrEvent]    : Event string
**      [f_pdstrType]     : Type string if it is a "ONACTION" event
**      [f_pCachedEvents] : Buffer of cached events
**      [f_cCachedEvents] : Length of cached events buffer
**      [f_piEvent]       : Returns index of entry in cache. If event is not found
**                          in cache, this returns -1
**
** Returns :
**      TRUE
**          Found event in cache
**      FALSE
**          Did not find event in cache
**
******************************************************************************/
static DRM_NO_INLINE DRM_BOOL DRM_CALL _IsCachedEvent(
    __in const DRM_CONST_STRING *f_pdstrEvent,
    __in const DRM_CONST_STRING *f_pdstrType,
    __in const DRM_CACHED_EVENT *f_pCachedEvents,
    __in       DRM_DWORD         f_cCachedEvents,
    __out      DRM_LONG         *f_piEvent ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL DRM_CALL _IsCachedEvent(
    __in const DRM_CONST_STRING *f_pdstrEvent,
    __in const DRM_CONST_STRING *f_pdstrType,
    __in const DRM_CACHED_EVENT *f_pCachedEvents,
    __in       DRM_DWORD         f_cCachedEvents,
    __out      DRM_LONG         *f_piEvent )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_DWORD  iEvent = 0;
    DRM_BOOL   fFound = FALSE;

    DRMASSERT( f_piEvent != NULL );

    if ( f_piEvent == NULL )
    {
        return FALSE;
    }

    *f_piEvent  = -1;

    if ( DRM_UTL_DSTRStringsEqual( f_pdstrEvent, &g_dstrLicEvalOnAction ) )
    {
        /*
        ** We might have already parsed this event
        */
        for( ; iEvent < f_cCachedEvents && !fFound; iEvent++ )
        {
            if ( DRM_UTL_DSTRStringsEqual( f_pdstrType, &f_pCachedEvents[iEvent].dstrType ) )
            {
                *f_piEvent = (DRM_LONG)iEvent;
                fFound = TRUE;
            }
        }
    }

    return fFound;
}

/******************************************************************************
**
** Function: _Eval
**
** Synopsis: Evaluate an expression
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to use
**      [f_pdstrLIData]     : Specifies the LI Data string
**      [f_pdstrEvent]      : Speifies event string data
**      [f_pdstrAttrValue]  : Specifies attribute data
**      [f_fCondition]      : Specifies whether to verify a condition or action
**      [f_pfResult]        : Returns the result of the action
**      [f_pfExisted]       : Returns whether the action existed
**
** Returns:
**      DRM_SUCCESS
**          Success. If an action fails, may still return DRM_SUCCESS as long
**          as the error was not fatal.
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      DRM_E_XMLNOTFOUND
**          XML data error occurred
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _Eval(
    __inout        DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __inout        DRM_CONST_STRING    *f_pdstrLIData,
    __in     const DRM_CONST_STRING    *f_pdstrEvent,
    __in     const DRM_CONST_STRING    *f_pdstrAttrValue,
    __in           DRM_BOOL             f_fCondition,
    __out_opt      DRM_BOOL            *f_pfResult,
    __out_opt      DRM_BOOL            *f_pfExisted ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT DRM_CALL _Eval(
    __inout        DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __inout        DRM_CONST_STRING    *f_pdstrLIData,
    __in     const DRM_CONST_STRING    *f_pdstrEvent,
    __in     const DRM_CONST_STRING    *f_pdstrAttrValue,
    __in           DRM_BOOL             f_fCondition,
    __out_opt      DRM_BOOL            *f_pfResult,
    __out_opt      DRM_BOOL            *f_pfExisted )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT        dr       = DRM_SUCCESS;
    DRM_LONG          iEvents  = 0;
    DRM_CONST_STRING  dstrExpr = EMPTY_DRM_STRING;
    TOKEN             tResult  = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICEVAL, PERF_FUNC__Eval );

    ChkArg( f_pdstrEvent != NULL );

    if ( f_pfResult != NULL )
    {
        /*
        ** Set the default value for missing event or condition
        */
        if ( DRM_UTL_DSTRStringsEqual( f_pdstrEvent, &g_dstrLicEvalOnAction ) )
        {
            *f_pfResult = FALSE;   /* An action is missing => that right is not allowed. */
        }
        else
        {
            *f_pfResult = TRUE;    /* For other events, the default is to assume true. */
        }
    }
    else
    {
        ChkArg( !f_fCondition );
    }

    if ( f_pContextLicEval->fUseCachedAttribs
      && _IsCachedEvent( f_pdstrEvent,
                         f_pdstrAttrValue,
                         f_pContextLicEval->rgCachedEvents,
                         f_pContextLicEval->cCachedEvents,
                        &iEvents ) )
    {
        if ( f_fCondition )
        {
            ASSIGN_DRM_STRING( dstrExpr,
                               f_pContextLicEval->rgCachedEvents[iEvents].dstrCondition );
        }
        else
        {
            ASSIGN_DRM_STRING( dstrExpr, f_pContextLicEval->rgCachedEvents[iEvents].dstrAction );
        }
        if ( dstrExpr.cchString == 0 )
        {
            dr = DRM_E_XMLNOTFOUND;
        }
    }
    else
    {
        if ( f_fCondition )
        {
            dr = DRM_WMDRM_LIC_GetEvent( &( f_pContextLicEval->dstrContentLicense ),
                                          f_pdstrEvent,
                                          f_pdstrAttrValue,
                                          f_pdstrLIData,
                                         &dstrExpr,
                                          NULL,
                                          NULL );
        }
        else
        {
            dr = DRM_WMDRM_LIC_GetEvent( &( f_pContextLicEval->dstrContentLicense ),
                                          f_pdstrEvent,
                                          f_pdstrAttrValue,
                                          f_pdstrLIData,
                                          NULL,
                                         &dstrExpr,
                                          NULL );
        }
    }
    if ( DRM_FAILED( dr ) )
    {
        if ( f_pfExisted != NULL )
        {
            *f_pfExisted = FALSE;
        }
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    if ( dstrExpr.cchString == 0 )
    {
        if ( f_pfResult != NULL )
        {
            *f_pfResult = TRUE; /* A missing CONDITION or missing expression in CONDITION. */
        }
        if ( f_pfExisted != NULL )
        {
            *f_pfExisted = FALSE;
        }
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    if ( f_pfExisted != NULL )
    {
        *f_pfExisted = TRUE;
    }

    dr = DRM_WMDRM_EXPR_EvaluateExpression( &dstrExpr, &f_pContextLicEval->contextEXPR, &tResult );

    if ( !f_fCondition )
    {
        /*
        ** It's an action.  Eat the errors, unless we're a chk build in which case let's fail.
        ** An action failing doesn't mean we shouldn't let people play their content, but we should
        ** catch this failure when testing to ensure scripts are working properly
        */
        DRMASSERT( DRM_SUCCEEDED( dr ) );

        dr = DRM_SUCCESS;
        if ( f_pfResult != NULL )
        {
            *f_pfResult = TRUE;
        }
    }
    else
    {
        ChkDR( dr );
        /*
        ** It's a condition
        */
        if ( tResult.TokenType == TOKEN_LONG && f_pfResult != NULL )
        {
            *f_pfResult = ( tResult.val.lValue != 0 );
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function: DRM_WMDRM_LEVL_PerformOperationsXML
**
** Synopsis: Do an operation on an XML license
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to use
**      [f_eOperation]      : Specifies the operation to be perfomed
**      [f_eOperationState] :
**      [f_pdstrAction]     : Specifies action data for DRM_LICENSE_EVAL_ACTION operation
**      [f_pfPerform]       : Returns whether the action has been performed
**      [f_pfActionExisted] : Returns whether the action existed
**      [f_pDatastore]      : Specifies the DST context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      LICEVAL_LICENSE_NOT_SUPPLIED
**          The license content string is NULL
**      LICEVAL_INVALID_LICENSE
**          The specified license is invalid
**      LICEVAL_LICENSE_REVOKED
**          The specified license has been revoked
**      LICEVAL_KID_MISMATCH
**          KID mismatch with header
**      DRM_E_CH_UNABLE_TO_VERIFY
**          Unable to verify the signature of the content header
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LEVL_PerformOperationsXML(
    __inout         DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in            eDRM_LICEVAL_OPERATIONS       f_eOperation,
    __in            eDRM_LICEVAL_OPERATION_STATE  f_eOperationState,
    __in_opt  const DRM_CONST_STRING             *f_pdstrAction,
    __out_opt       DRM_BOOL                     *f_pfPerform,
    __out_opt       DRM_BOOL                     *f_pfActionExisted,
    __in      const DRM_DST                      *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT        dr            = DRM_SUCCESS;
    DRM_LONG          lResult       = 0;
    DRM_CONST_STRING  dstrAction    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrLIData    = EMPTY_DRM_STRING;
    DRM_BOOL          fCondition    = TRUE;
    DRM_CONST_STRING  dstrKID       = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICEVAL, PERF_FUNC__PerformOperationsXML );

    /*
    ** The other pointers in the context structure *may* not be needed so don't fail just yet for those
    */
    ChkArg( f_pContextLicEval              != NULL );
    ChkArg( f_pContextLicEval->pcontextBBX != NULL );
    ChkArg( f_pDatastore                   != NULL );

    switch( f_eOperationState )
    {
    case DRM_LICENSE_EVAL_CAN_DO_OPERATION:
        fCondition = TRUE;
        break;
    case DRM_LICENSE_EVAL_DONE_WITH_OPERATION:
        fCondition = FALSE;
        break;
    default:
        ChkArg( FALSE );
    }

    if ( f_pfPerform != NULL )
    {
        *f_pfPerform = FALSE;
    }
    else
    {
        ChkArg( !fCondition );
    }

    /*
    ** Some flag we don't recognize was given.
    */
    ChkArg( ( f_pContextLicEval->dwFlags &
            ~( LICEVAL_VERIFY_IGNORE_VERIFICATION
             | LICEVAL_VERIFY_CERT_WITH_EXPIRYCHECK
             | LICEVAL_VERIFY_CERT_WITHOUT_EXPIRYCHECK
             | LICEVAL_VERIFY_SIGNATURE ) ) == 0 );

    /*
    ** If you are checking the cert chain you must also check the signature
    */
    ChkArg( ( f_pContextLicEval->dwFlags & LICEVAL_VERIFY_SIGNATURE )                != 0
         || ( f_pContextLicEval->dwFlags & LICEVAL_VERIFY_CERT_WITH_EXPIRYCHECK )    == 0
         || ( f_pContextLicEval->dwFlags & LICEVAL_VERIFY_CERT_WITHOUT_EXPIRYCHECK ) == 0 );

    if ( !f_pContextLicEval->fReserved )
    {
        ChkBOOL( f_pContextLicEval->dstrContentLicense.pwszString != NULL,
                 DRM_E_LICEVAL_LICENSE_NOT_SUPPLIED );

        if ( ( f_pContextLicEval->dwFlags & LICEVAL_VERIFY_IGNORE_VERIFICATION ) == 0 )
        {
            /*
            **  Some XML optimizations can be done here as well. Ignoring for
            **  now since the XML operations in here are relatively inexpensive.
            */
            ChkDR( DRM_WMDRM_LIC_VerifySignature( &f_pContextLicEval->dstrContentLicense,
                                                   f_pContextLicEval->pcontextBBX,
                                                  &lResult ) );
            ChkBOOL( lResult != 0, DRM_E_INVALID_LICENSE );

            if( !DRM_SYMOPT_IsSymOptSupported() )
            {
                /*
                ** During storing, we normally should check expiry of license server certificates.
                */
                if ( f_pContextLicEval->dwFlags & LICEVAL_VERIFY_CERT_WITH_EXPIRYCHECK )
                {
                    ChkDR( DRM_WMDRM_LIC_VerifyCertChain( &f_pContextLicEval->dstrContentLicense,
                                                           TRUE,
                                                           f_pContextLicEval,
                                                          &lResult ) );
                    ChkBOOL( lResult != 0, DRM_E_INVALID_LICENSE );
                }
                else if ( f_pContextLicEval->dwFlags & LICEVAL_VERIFY_CERT_WITHOUT_EXPIRYCHECK )
                {
                    ChkDR( DRM_WMDRM_LIC_VerifyCertChain( &f_pContextLicEval->dstrContentLicense,
                                                           FALSE,
                                                           f_pContextLicEval,
                                                          &lResult ) );
                    ChkBOOL( lResult != 0, DRM_E_INVALID_LICENSE );
                }
            }
        }

        /*
        ** Process content revocation lists if any from this license.
        */
        dr = DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation( f_pContextLicEval, f_pDatastore );

        /*
        ** Treat as failure and revoke this license.
        ** Set a value in the ExprControl
        */
        ChkBOOL( DRM_SUCCEEDED( dr )
              && !DRM_CONTENTREVOCATION_LEVL_IsLicenseRevoked( f_pContextLicEval, f_pDatastore ), DRM_E_LICEVAL_LICENSE_REVOKED );

        f_pContextLicEval->fReserved = TRUE;
    }

    if ( !f_pContextLicEval->fUseCachedAttribs )
    {
        ChkDR( DRM_WMDRM_LIC_GetAttribute( &( f_pContextLicEval->dstrContentLicense ),
                                            NULL,
                                            DRM_LICENSE_ATTRIB_LID,
                                           &dstrLIData,
                                           &dstrAction,
                                            0 ) );
        ChkDR( DRM_UTL_StringToLittleEndianGuid( &dstrAction, ( DRM_GUID* )f_pContextLicEval->LID.rgb ) );
        ChkDR( DRM_WMDRM_LIC_GetAttribute( &( f_pContextLicEval->dstrContentLicense ),
                                            NULL,
                                            DRM_LICENSE_ATTRIB_KID,
                                           &dstrLIData,
                                           &dstrKID,
                                            0 ) );
        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &f_pContextLicEval->KID ) );
    }

    /*
    ** Actually can do action stuff here.
    */
    switch( f_eOperation )
    {
        case DRM_LICENSE_EVAL_SELECT:
            ChkDR( DRM_CONTENTREVOCATION_LEVL_VerifyHeaderSignature(
                f_pContextLicEval,
                f_eOperationState,
                &dstrKID,
                &dstrLIData ) );

            dstrAction.cchString  = g_dstrLicEvalOnSelect.cchString;
            dstrAction.pwszString = g_dstrLicEvalOnSelect.pwszString;
            break;
        case DRM_LICENSE_EVAL_DELETE:
            dstrAction.cchString  = g_dstrLicEvalOnDelete.cchString;
            dstrAction.pwszString = g_dstrLicEvalOnDelete.pwszString;
            break;
        case DRM_LICENSE_EVAL_STORE:
            fCondition = FALSE;
            dstrAction.cchString  = g_dstrLicEvalOnStore.cchString;
            dstrAction.pwszString = g_dstrLicEvalOnStore.pwszString;
            break;
        case DRM_LICENSE_EVAL_ACTION:
            ChkArg( f_pdstrAction != NULL );
            dstrAction.cchString  = g_dstrLicEvalOnAction.cchString;
            dstrAction.pwszString = g_dstrLicEvalOnAction.pwszString;
            break;
        case DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK:
            if( DRM_ARCLK_IsAntirollbackClockSupported() )
            {
                ChkArg( !fCondition );
                ASSIGN_DRM_STRING( dstrAction, g_dstrLicEvalOnClockRollback );
                break;
            }
            /*
            ** Fall through to default case
            */
            __fallthrough;
        default:
            ChkArg( FALSE );
    }

    f_pContextLicEval->contextEXPR.GetVariable  = DRM_WMDRM_EXPR_GlobalGetVariable;
    f_pContextLicEval->contextEXPR.SetVariable  = DRM_WMDRM_EXPR_GlobalSetVariable;
    f_pContextLicEval->contextEXPR.pvOpaqueData = f_pContextLicEval;

    MEMCPY( &f_pContextLicEval->contextEXPR.KID,
            &f_pContextLicEval->KID,
             SIZEOF( DRM_KID ) );
    MEMCPY( &f_pContextLicEval->contextEXPR.LID,
            &f_pContextLicEval->LID,
             SIZEOF( DRM_LID ) );

    ChkDR( _Eval( f_pContextLicEval,
                 &dstrLIData,
                 &dstrAction,
                  f_pdstrAction,
                  fCondition,
                  f_pfPerform,
                  f_pfActionExisted ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function: DRM_WMDRM_LEVL_GetLicenseReasonForUnusable
**
** Synopsis: Parse the license again to get the reason for failure.
**
** Arguments:
**      [f_pcontextLicEval]     : Specifies the liceval context
**      [f_plReasonForUnusable] : Returns the reason for failure
**
** Returns :
**      DRM_SUCCESS
**          Succeeded
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**
** Notes:
**      This is a workaround to compensate for the fact that v9
**      licenses do not always set the failure reason correctly
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LEVL_GetLicenseReasonForUnusable(
    __in  const DRM_LICEVAL_CONTEXT   *f_pcontextLicEval,
    __out       DRM_LONG              *f_plReasonForUnusable )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   dwAppSec = 0;
    DRMFILETIME fileTime = {0};
    DRMFILETIME currDate = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICEVAL,
                               PERF_FUNC_DRM_WMDRM_LEVL_GetLicenseReasonForUnusable );

    ChkArg( f_pcontextLicEval     != NULL );
    ChkArg( f_plReasonForUnusable != NULL );

    *f_plReasonForUnusable = 0;

    Oem_Clock_GetSystemTimeAsFileTime( f_pcontextLicEval->pcontextBBX->pOEMContext, &currDate );

    dr = DRM_WMDRM_LEVL_ParseV2License( &f_pcontextLicEval->dstrContentLicense,
                                        &g_dstrDRM_LS_BEGDATE_ATTR,
                                         NULL,
                                        &fileTime );

    if ( dr == DRM_SUCCESS
      && DRM_UTL_DateLessThan( &currDate, &fileTime ) )
    {
        *f_plReasonForUnusable = LR_LICENSE_NOTENABLED;
    }
    else
    {
        dr = DRM_WMDRM_LEVL_ParseV2License( &f_pcontextLicEval->dstrContentLicense,
                                            &g_dstrDRM_LS_ENDDATE_ATTR,
                                             NULL,
                                            &fileTime );

        if ( dr == DRM_SUCCESS
          && DRM_UTL_DateLessThan( &fileTime, &currDate ) )
        {
            *f_plReasonForUnusable = LR_LICENSE_EXPIRED;
        }
        else
        {
            dr = DRM_WMDRM_LEVL_ParseV2License( &f_pcontextLicEval->dstrContentLicense,
                                                &g_dstrDRM_LS_APPSEC_ATTR,
                                                &dwAppSec,
                                                 NULL );
            if ( dr == DRM_SUCCESS
              && ( ( f_pcontextLicEval->fLegacyAppInfoValid
                  && f_pcontextLicEval->certinfoAppLegacy.appSec < dwAppSec )
                 || f_pcontextLicEval->certinfoSDK.appSec < dwAppSec ) )
            {
                *f_plReasonForUnusable = LR_LICENSE_APPSECLOW;
            }
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LEVL_ParseV2License(
    __in      const DRM_CONST_STRING *f_pdstrLicense,
    __in      const DRM_CONST_STRING *f_pdstrAttrName,
    __out_opt       DRM_DWORD        *f_pdwValue,
    __out_opt       DRMFILETIME      *f_pfiletime )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_CONST_STRING dstrExpression = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLIData     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue      = EMPTY_DRM_STRING;
    TOKEN            token          = {0};

    ChkDRMString( f_pdstrLicense );
    ChkDRMString( f_pdstrAttrName );

    if( f_pfiletime != NULL
     && ( f_pdstrAttrName == &g_dstrDRM_LS_BEGDATE_ATTR
       || f_pdstrAttrName == &g_dstrDRM_LS_ENDDATE_ATTR ) )
    {
        const DRM_CONST_STRING *pdstrSubString = NULL;

        /* Retrieve the condition for the ONSELECT event. */
        ChkDR( DRM_WMDRM_LIC_GetEvent( f_pdstrLicense,
                                      &g_dstrLicEvalOnSelect,
                                       NULL,
                                      &dstrLIData,
                                      &dstrExpression,
                                       NULL,
                                       NULL ) );
        dr = DRM_S_FALSE;

        if( f_pdstrAttrName == &g_dstrDRM_LS_BEGDATE_ATTR )
        {
            pdstrSubString = &g_dstrDRM_LS_MACHINE_DATETIME_GE;
        }
        else if( f_pdstrAttrName == &g_dstrDRM_LS_ENDDATE_ATTR )
        {
            pdstrSubString = &g_dstrDRM_LS_MACHINE_DATETIME_LE;
        }

        /*
        ** Search for last occurence of pwszSearch in bstrExpr. Due to disable clock & deletelicesne
        ** functionalities, it is possible that there is more than one such string. The last one
        ** is the correct one that is used for determining whether license is selectable or not.
        */
        if( DRM_UTL_DSTRSearchReverse( &dstrExpression, pdstrSubString, &dstrValue ) )
        {
            /*
            ** If the date string was found we know there are at least 21 characters,
            ** and we push past all those and look at the date immediately after,
            ** skipping whitespace
            */

            if( dstrValue.cchString > g_dstrDRM_LS_MACHINE_DATETIME_GE.cchString
             && 1                   < g_dstrDRM_LS_MACHINE_DATETIME_GE.cchString ) /* There must be at least a '#' character */
            {
                /* Advance to the # */
                dstrValue.pwszString += g_dstrDRM_LS_MACHINE_DATETIME_GE.cchString - 1;
                dstrValue.cchString  -= g_dstrDRM_LS_MACHINE_DATETIME_GE.cchString - 1;

                while( dstrValue.cchString > 0
                    && DRMCRT_iswspace( *(dstrValue.pwszString) ) )
                {
                    dstrValue.pwszString++;
                    dstrValue.cchString--;
                }

                if( DRM_SUCCEEDED( DRM_WMDRM_EXPR_ExtractDateToken( &dstrValue, &token ) ) )
                {
                    UI64_TO_FILETIME( token.val.u64DateTime, *f_pfiletime );
                    dr = DRM_SUCCESS;
                }
            }
        }
    }
    else if( f_pdwValue      !=  NULL
          && f_pdstrAttrName == &g_dstrDRM_LS_APPSEC_ATTR )
    {
        /*
        ** Need to get min app sec needed by the license.
        ** Retrieve the condition for the ONSELECT event.
        */

        ChkDR( DRM_WMDRM_LIC_GetEvent( f_pdstrLicense,
                                      &g_dstrLicEvalOnSelect,
                                       NULL,
                                      &dstrLIData,
                                      &dstrExpression,
                                       NULL,
                                       NULL ) );
        dr = DRM_S_FALSE;

        if( DRM_UTL_DSTRSearchReverse( &dstrExpression, &g_dstrDRM_LS_APP_MINSECLEVEL_GE, &dstrValue ) )
        {
            /*
            ** If the date string was found we know there are at least 21 characters,
            ** and we push past all those and look at the date immediately after,
            ** skipping whitespace
            */

            if( dstrValue.cchString > g_dstrDRM_LS_APP_MINSECLEVEL_GE.cchString )
            {
                /* Advance past the string */
                dstrValue.pwszString += g_dstrDRM_LS_APP_MINSECLEVEL_GE.cchString;
                dstrValue.cchString  -= g_dstrDRM_LS_APP_MINSECLEVEL_GE.cchString;

                while( dstrValue.cchString > 0
                    && DRMCRT_iswspace( *(dstrValue.pwszString) ) )
                {
                    dstrValue.pwszString++;
                    dstrValue.cchString--;
                }

                if( DRM_SUCCEEDED( DRM_WMDRM_EXPR_ExtractLongToken( &dstrValue, &token ) ) )
                {
                    *f_pdwValue = (DRM_DWORD)token.val.lValue;
                    dr          = DRM_SUCCESS;
                }
            }
        }
    }
    else if( f_pdwValue      !=  NULL
          && f_pdstrAttrName == &g_dstrDRM_LS_SAP_ATTR )
    {
        /*
        ** Need to get min app sec needed by the license.
        ** Retrieve the condition for the ONSELECT event.
        */
        *f_pdwValue = 0;
        ChkDR( DRM_WMDRM_LIC_GetEvent( f_pdstrLicense,
                                      &g_dstrLicEvalOnSelect,
                                       NULL,
                                      &dstrLIData,
                                      &dstrExpression,
                                       NULL,
                                       NULL ) );
        dr = DRM_S_FALSE;

        if( DRM_UTL_DSTRSearch( &dstrExpression, &g_dstrDRMK_PREFIX, &dstrValue ) )
        {
            /*
            ** If drmk was found in the onselect statement then we know SAP
            ** is requiered
            */
            *f_pdwValue = 1;
        }
    }
    else if( f_pdwValue != NULL
          && DRM_UTL_DSTRSearch( f_pdstrAttrName, &g_dstrDRM_LS_BEGIN_ATTR, &dstrValue )
          && DRM_UTL_DSTRSearch( f_pdstrAttrName, &g_dstrDRM_LS_COUNT_ATTR, &dstrValue ) )
    {
        /* We will look for this in ONSTORE action part. */
        ChkDR( DRM_WMDRM_LIC_GetEvent( f_pdstrLicense,
                                      &g_dstrLicEvalOnStore,
                                       NULL,
                                      &dstrLIData,
                                       NULL,
                                      &dstrExpression,
                                       NULL ) );

        dr = DRM_S_FALSE;

        if( DRM_UTL_DSTRSearch( &dstrExpression, f_pdstrAttrName, &dstrValue ) )
        {
            DRM_CONST_STRING dstr = EMPTY_DRM_STRING;

            if( DRM_UTL_DSTRSearch( &dstrValue, &g_dstrEqual, &dstr) )
            {
                /* Move past the "=" */
                dstr.pwszString += 1;
                dstr.cchString  -= 1;

                while( dstrValue.cchString > 0
                    && DRMCRT_iswspace( *(dstrValue.pwszString) ) )
                {
                    dstrValue.pwszString++;
                    dstrValue.cchString--;
                }

                ChkDR( DRM_WMDRM_EXPR_ExtractLongToken( &dstr, &token ) );
                *f_pdwValue = (DRM_DWORD)token.val.lValue;
            }
        }
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        dr = DRM_S_FALSE;
    }
    return dr;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_LEVL_HasSAP(
    __in      const DRM_CONST_STRING *f_pdstrLicense )
{
    DRM_DWORD cbSize = 0;
    (void)DRM_WMDRM_LEVL_ParseV2License( f_pdstrLicense,
                                         &g_dstrDRM_LS_SAP_ATTR,
                                         &cbSize,
                                         NULL );

    return cbSize > 0;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_CheckSymmOptStatus(
    __in      const  DRM_CONST_STRING   *f_pdstrLicense,
    __out            DRM_SYMMOPT_STATUS *f_eStatus,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNode,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNodeData,
    __out_opt        DRM_GUID           *f_pCkVerifiedGUID )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_CONST_STRING dstrLicData = EMPTY_DRM_STRING;

    ChkArg( f_pdstrLicense != NULL );
    ChkArg( f_eStatus      != NULL );

    *f_eStatus = DRM_SYMMOPT_STATUS_NONE;

    if ( f_pdstrSymmSigNode != NULL )
    {
        f_pdstrSymmSigNode->cchString  = 0;
        f_pdstrSymmSigNode->pwszString = NULL;
    }
    if ( f_pdstrSymmSigNodeData != NULL )
    {
        f_pdstrSymmSigNodeData->cchString  = 0;
        f_pdstrSymmSigNodeData->pwszString = NULL;
    }

    /*
    ** Try to find a SYMSIGNATURE node
    */
    if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( f_pdstrLicense, &g_dstrLicensorInfoNode, NULL, NULL, &dstrLicData, NULL, g_wchForwardSlash ) ) )
    {
        DRM_CONST_STRING dstrSymSigNode = EMPTY_DRM_STRING;

        if( DRM_SUCCEEDED( DRM_XML_GetSubNode( &dstrLicData,
                                               &g_dstrTagSymSig,
                                                NULL,
                                                NULL,
                                                0,
                                               &dstrSymSigNode,
                                                f_pdstrSymmSigNodeData,
                                                1 ) ) )
        {
            DRM_CONST_STRING dstrVerifiedCk = EMPTY_DRM_STRING;

            if ( f_pdstrSymmSigNode != NULL )
            {
                *f_pdstrSymmSigNode = dstrSymSigNode;
            }

            /*
            ** Try to find an "ID" (verifiedCK) attribute from the "SYMSIGNATURE" tag.
            */
            if ( DRM_SUCCEEDED( DRM_XML_GetNodeAttribute( &dstrSymSigNode,
                                                          &g_dstrAttributeVerifiedCK,
                                                          &dstrVerifiedCk ) ) )
            {
                DRM_GUID oVerifiedCKGuid = EMPTY_DRM_GUID;

                ChkDR( DRM_UTL_StringToLittleEndianGuid( &dstrVerifiedCk, &oVerifiedCKGuid ) );
                *f_eStatus = GET_CKVERIFIED_BIT( oVerifiedCKGuid )? DRM_SYMMOPT_STATUS_COMPLETE : DRM_SYMMOPT_STATUS_PARTIAL;

                if ( f_pCkVerifiedGUID != NULL )
                {
                    MEMCPY( f_pCkVerifiedGUID, &oVerifiedCKGuid, SIZEOF( DRM_GUID ) );
                }
            }
            else
            {
                *f_eStatus = DRM_SYMMOPT_STATUS_PARTIAL_NOTAG;
            }
        }
    }

ErrorExit:
    return dr;
}

#define CCH_SYMVAL_TAG  ( 2  + g_dstrTagSymValue.cchString             /* <SYMVALUE> */                                               \
                             + CCH_BASE64_EQUIV( 20 )                  /* B64-encoded symmetrically encrypted content key.        */  \
                                                                       /* This value will be updated during processing            */  \
                                                                       /* as it could be different sizes depending on the license */  \
                         + 3 + g_dstrTagSymValue.cchString)            /* </SYMVALUE> */                                              \

static DRM_RESULT DRM_CALL _UpdateLicenseWithSymmetricSignature(
    __inout        DRM_STRING          *f_pdstrLicense,
    __in     const DRM_CONST_STRING    *f_pdstrData,
    __in           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_opt const DRM_SLK             *f_pslk,
    __in           DRM_BOOL             f_fVerifiedCk )
{
    const DRM_CONST_STRING  *pdstrLicense       = (const DRM_CONST_STRING*) f_pdstrLicense;
    DRM_CONST_STRING         dstrSignatureNode  = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrSignature      = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrPubKey         = EMPTY_DRM_STRING;
    DRM_WCHAR               *pwszInsertionPoint = NULL;
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_SYMMOPT_STATUS       eStatus            = DRM_SYMMOPT_STATUS_NONE;
    DRM_LONG                 lResult            = 0;
    DRM_GUID                 ckVerifiedGuid     = EMPTY_DRM_GUID;
    DRM_BYTE                 rgbSymmSig[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_DWORD                cch                = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICPARSER, PERF_FUNC__UpdateLicenseWithSymmetricSignature);

    /*
    ** 1. If a sym signature already exists and a slk is passed in, try to verify the signature using the slk.
    ** otherwise assume the license already contains a signature (sym or asym) associated with the device
    ** key and tries the verify the signature using the device key.
    ** 2. If the above verification passes, try to create/update a sym signature (associated with the
    ** device key).
    */

    if( f_pslk != NULL
     && DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( pdstrLicense,
                                                 &g_dstrXPathSymSig,
                                                 NULL,
                                                 NULL,
                                                 &dstrSignatureNode,
                                                 &dstrSignature,
                                                 g_wchForwardSlash ) ) )
    {
        cch = SIZEOF( rgbSymmSig );
        ChkDR( DRM_B64_DecodeW( &dstrSignature, &cch, rgbSymmSig, 0 ) );

        if( cch != DRM_SHA1_DIGEST_LEN
         || DRM_FAILED( DRM_BBX_SymmetricVerify( f_pcontextLEVL->pcontextBBX,
                                                 NULL,
                                                 PB_DSTR( f_pdstrData ),
                                                 CB_DSTR( f_pdstrData ),
                                                 NULL,
                                                 0,
                                                 f_pslk,
                                                 rgbSymmSig ) ) )
        {
            ChkDR( DRM_E_INVALID_LICENSE );
        }

        /*
        ** Treat the SLK verified license as a partially sym verified license without tag
        ** so the code after this "if" statement can add a "verifiedck" attribute
        **  to the <SYMSIGNATURE> tag.
        */
        eStatus = DRM_SYMMOPT_STATUS_PARTIAL_NOTAG;
    }
    else
    {
        /*
        ** Check for the level of symmetric optimization already being applied to the license.
        */
        ChkDR( DRM_SYMOPT_LIC_CheckSymmOptStatus( pdstrLicense, &eStatus, &dstrSignatureNode, &dstrSignature, &ckVerifiedGuid ) );

        if ( eStatus == DRM_SYMMOPT_STATUS_COMPLETE
          || ( eStatus == DRM_SYMMOPT_STATUS_PARTIAL && !f_fVerifiedCk ) )
        {
            /* We should not receive a license with verifiedck attribute through ILA with a SLK. */
            ChkBOOL( f_pslk == NULL, DRM_E_INVALID_LICENSE );
            goto ErrorExit;
        }

        /*
        ** Verify existing signature and cert chain
        */
        ChkDR( DRM_WMDRM_LIC_VerifySignature( pdstrLicense, f_pcontextLEVL->pcontextBBX, &lResult ) );
        if( lResult != 1 )
        {
            ChkDR( DRM_E_INVALID_LICENSE );
        }
        ChkDR( DRM_WMDRM_LIC_VerifyCertChain( pdstrLicense, TRUE, f_pcontextLEVL, &lResult ) );
        if( lResult != 1 )
        {
            ChkDR( DRM_E_INVALID_LICENSE );
        }
    }

    /*
    ** No symm opts at all - make room for the symmetric signature node
    */
    if ( eStatus == DRM_SYMMOPT_STATUS_NONE )
    {
        /* We are inserting immediatly after the closing tag of the DATA node */
        pwszInsertionPoint  = ( DRM_WCHAR* ) ( f_pdstrData->pwszString + f_pdstrData->cchString );
        ChkDR( DRM_UTL_StringInsertBlankSubString( f_pdstrLicense,
                                     ( DRM_DWORD )( f_pdstrData->pwszString - f_pdstrLicense->pwszString ) + f_pdstrData->cchString,
                                                   CCH_SYMSIG_TAG ) );

        /* Generate new random GUID. */
        ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&ckVerifiedGuid, SIZEOF( DRM_GUID ) ) );
    }
    /*
    ** A sym signature node already exists: resize if necessary.
    */
    else
    {
        DRM_DWORD cchDelta = 0;

        if ( eStatus == DRM_SYMMOPT_STATUS_PARTIAL_NOTAG )
        {
            /*
            ** The current <SYMSIGNATURE> tag does not have the "verifiedck" attribute,
            ** allocate enough space for the attribute.
            */
            cchDelta += CCH_SYMSIG_TAG_VERIFIEDCK;

            /* Generate new random GUID. */
            ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&ckVerifiedGuid, SIZEOF( DRM_GUID ) ) );
        }

        if ( DRM_FAILED( DRM_XML_GetNodeAttribute( &dstrSignatureNode,
                                                   &g_dstrAttributePubkey,
                                                   &dstrPubKey ) ) )
        {
            /*
            ** The current <SYMSIGNATURE> tag does not have the "pubkey" attribute,
            ** allocate enough space for the attribute.
            */
            cchDelta += CCH_SYMSIG_TAG_PUB_KEY;
        }

        if ( cchDelta > 0 )
        {
            ChkDR( DRM_UTL_StringInsertBlankSubString( f_pdstrLicense,
                                        ( DRM_DWORD )( dstrSignature.pwszString - f_pdstrLicense->pwszString ),
                                                       cchDelta ) );
        }

        pwszInsertionPoint = ( DRM_WCHAR * )dstrSignatureNode.pwszString;
    }

    /* Turn on/off the verifiedCk bit. */
    SET_CKVERIFIED_BIT( ckVerifiedGuid, f_fVerifiedCk );

    /*
    ** Create the signed buffer: Data section of license concatenated with the value of verifiedck
    */
    ChkDR( DRM_BBX_SymmetricSign( f_pcontextLEVL->pcontextBBX,
                                  PB_DSTR( f_pdstrData ),
                                  CB_DSTR( f_pdstrData ),
                                  ( DRM_BYTE * )&ckVerifiedGuid,
                                  SIZEOF( DRM_GUID ),
                                  rgbSymmSig ) );

    ChkDR( DRM_UTL_BuildXMLSymmSignature( &f_pcontextLEVL->pcontextBBX->cachedCertValues.pubkeyCurrent,
                                          rgbSymmSig,
                                          pwszInsertionPoint,
                                          CCH_SYMSIG_TAG,
                                          &ckVerifiedGuid ) );

ErrorExit:

     DRM_PROFILING_LEAVE_SCOPE;

     return dr;
}

static DRM_RESULT DRM_CALL _UpdateLicenseWithSymmetricKey(
    __inout        DRM_STRING          *f_pdstrLicense,
    __in     const DRM_CONST_STRING    *f_pdstrData,
    __in           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_opt const DRM_SLK             *f_pslk )
{
    DRM_RESULT              dr                 = DRM_SUCCESS;
    DRM_CONST_STRING        dstrString         = EMPTY_DRM_STRING;
    const DRM_CONST_STRING *pdstrLicense       = (const DRM_CONST_STRING*) f_pdstrLicense;
    DRM_DWORD               cch                = 0;
    DRM_DWORD               cbSymmKey          = 0;
    DRM_DWORD               cbSymmKeyOrig      = 0;
    DRM_DWORD               cbSymmKeyDelta     = 0;
    DRM_WCHAR              *pwszInsertionPoint = NULL;
    DRM_BINDING_INFO        bindinfo           = { 0 };
    DRM_DWORD               cbKey              = SIZEOF( bindinfo.m_BindingInfo.m_XML.m_rgbContentKey );

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICPARSER, PERF_FUNC__UpdateLicenseWithSymmetricKey);

    bindinfo.m_fXMRLicense = FALSE;

    /*
    **   Using the blackbox create a symmetrically decryptable content key for the license
    **   1.  if a symvalue exists and slk is not NULL, extract it with the current slk and rebind
    **   2.  if a symvalue exists and slk IS NULL, do not rebind (already bound to current slk)
    **   3.  else asymmetrically extract and rebind.
    */

    /* Get the PUBKEY this license is bound to */
    ChkDR( DRM_WMDRM_LIC_GetEnablingBits( (DRM_CONST_STRING*) f_pdstrLicense,
                                          0,
                                         &bindinfo.m_BindingInfo.m_XML.m_dwAlgorithm,
                                         &bindinfo.m_BindingInfo.m_XML.m_oPublKey,
                                          bindinfo.m_BindingInfo.m_XML.m_rgbContentKey,
                                         &cbKey,
                                         &bindinfo.m_BindingInfo.m_XML.m_oLsPublKey,
                                          bindinfo.m_BindingInfo.m_XML.m_rgbSignature,
                                          NULL,
                                         &f_pcontextLEVL->pcontextBBX->CryptoContext ) );

    if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( pdstrLicense,
                                                &g_dstrXPathSymValue,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                &dstrString,
                                                 g_wchForwardSlash ) ) )
    {
        /*
        ** The license already has a symmetrically encrypted content key, which is bound to the current bbx
        */
        if ( f_pslk == NULL )
        {
            goto ErrorExit;
        }

        cbSymmKey = SIZEOF( bindinfo.m_BindingInfo.m_XML.m_rgbContentKey );
        ChkDR( DRM_B64_DecodeW( &dstrString, &cbSymmKey, bindinfo.m_BindingInfo.m_XML.m_rgbContentKey, 0 ) );

        /*
        **  DRM_B64_DecodeW set cbSymmKey=the number of bytes copied into dstrString (i.e. the size of the key)
        **  Store the size of the key in cbSymmKeyOrig.  If DRM_BBX_RebindSymmetricKey changes the size of
        **  the key then, on return, cbSymmKey will have that new size.
        */
        cbSymmKeyOrig = cbSymmKey;

        ChkDR( DRM_BBX_RebindSymmetricKeyXML( f_pcontextLEVL->pcontextBBX,
                                              f_pslk,
                                             &bindinfo.m_BindingInfo.m_XML,
                                              cbSymmKey ) );

        /*
        **  Is the key larger after DRM_BBX_RebindSymmetricKey() than it was before?
        **  If yes, then determine how much larger the key is now so that the
        **  license string can be padded with enough space for the new larger key.
        */
        if ( cbSymmKey > cbSymmKeyOrig )
        {
            cbSymmKeyDelta = cbSymmKey - cbSymmKeyOrig;
        }

        /* Set the insertion point to just at the begining of the <SYMVALUE tag */
        pwszInsertionPoint = (DRM_WCHAR*) (dstrString.pwszString - ( 2 + g_dstrTagSymValue.cchString ));
    }
    else
    {
        /* Convert the asymmetric key to a symmetric key.  This happens in place. */
        ChkDR( DRM_BBX_AsymmetricToSymmetricKeyXML( f_pcontextLEVL->pcontextBBX,
                                                    &bindinfo,
                                                    &cbSymmKey ) );

        cch = (CCH_SYMVAL_TAG - CCH_BASE64_EQUIV( 20 )) + CCH_BASE64_EQUIV(cbSymmKey);

        /* We are inserting immediatly before the closing tag of the LICENSORINFO tag */
        pwszInsertionPoint  = (DRM_WCHAR*) (f_pdstrData->pwszString + f_pdstrData->cchString);
        ChkDR( DRM_UTL_StringInsertBlankSubString( f_pdstrLicense,
                                       (DRM_DWORD)(f_pdstrData->pwszString - f_pdstrLicense->pwszString ) + f_pdstrData->cchString,
                                                   cch ) );
    }

    /* Create the symmetric encrypted content key node and data */
    *pwszInsertionPoint = g_wchLessThan;
     pwszInsertionPoint++;
    ChkDR( DRM_STR_StringCchCopyNW( pwszInsertionPoint, g_dstrTagSymValue.cchString + 1, g_dstrTagSymValue.pwszString, g_dstrTagSymValue.cchString ) );
     pwszInsertionPoint += g_dstrTagSymValue.cchString;
    *pwszInsertionPoint = g_wchGreaterThan;
     pwszInsertionPoint++;

     if ( cbSymmKeyDelta > 0 )
     {
        /*
        ** If the key is larger after being rebound then add some blank spaces
        ** as a space holder so that the new larger key will fit correctly in
        ** the string.
        */
        cch = CCH_BASE64_EQUIV( cbSymmKey ) - CCH_BASE64_EQUIV( cbSymmKeyOrig );

        ChkDR( DRM_UTL_StringInsertBlankSubString( f_pdstrLicense,
                                       (DRM_DWORD)(pwszInsertionPoint - f_pdstrLicense->pwszString ),
                                                   cch ) );
     }

    cch = CCH_BASE64_EQUIV( cbSymmKey );
    ChkDR( DRM_B64_EncodeW( bindinfo.m_BindingInfo.m_XML.m_rgbContentKey,
                            cbSymmKey,
                            pwszInsertionPoint,
                           &cch,
                            0 ) );

     pwszInsertionPoint += cch;
    *pwszInsertionPoint  = g_wchLessThan;
     pwszInsertionPoint += 1;
    *pwszInsertionPoint  = g_wchForwardSlash;
     pwszInsertionPoint += 1;

    ChkDR( DRM_STR_StringCchCopyNW( pwszInsertionPoint,  g_dstrTagSymValue.cchString + 1, g_dstrTagSymValue.pwszString, g_dstrTagSymValue.cchString ) );
     pwszInsertionPoint += g_dstrTagSymValue.cchString;
    *pwszInsertionPoint  = g_wchGreaterThan;
     pwszInsertionPoint += 1;

ErrorExit:

     DRM_PROFILING_LEAVE_SCOPE;

     return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_UpdateLicenseWithSymmetricData(
    __inout        DRM_STRING          *f_pdstrLicense,
    __in           DRM_DWORD            f_cchLicenseMax,
    __in           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in           DRM_BOOL             f_fVerifiedCK,
    __in_opt const DRM_SLK             *f_pslk )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_CONST_STRING dstrEnablingBits = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrData         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSymValueNode = EMPTY_DRM_STRING;


    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICPARSER, PERF_FUNC_DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData);

    ChkArg( f_pdstrLicense != NULL );
    ChkArg( f_pcontextLEVL != NULL );
    ChkArg( f_pcontextLEVL->pcontextBBX != NULL );

    if( f_pdstrLicense->cchString + CCH_SYMSIG_TAG + CCH_SYMVAL_TAG > f_cchLicenseMax )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Steps:
    **   1.  Determine if this is a leaf, simple, or root license
    **   2a. Update root and simple licenses with a symmetric content key
    **   2b. Remove any <SYMVALUE> node from chained leaf licenses
    **   3.  Update license with a symmetric signature
    */

    /*
    ** The following XML may be added(or updated) to the license enabling bits section
    ** <SYMVALUE>B64-encoded symmetrically encrypted content key</SYMVALUE>
    ** <SYMSIGNATURE>B64-encoded symmetric signature</SYMSIGNATURE>
    */

    /* If there is an uplink kid node we don't rebind the content key */
    dr = DRM_WMDRM_LIC_GetAttribute( (const DRM_CONST_STRING*)f_pdstrLicense,
                                      NULL,
                                      DRM_LICENSE_ATTRIB_CHAINEDKID,
                                     &dstrData,
                                     &dstrEnablingBits,
                                      0);
    if( DRM_FAILED( dr ) )
    {
        /*
        ** If the license is either a root or a simple license, always symmetrically
        ** rebind the content key to the device key. dstrData points at the LICENSEORINFO node
        ** in the license, populated by the previous call to GetAttribute.
        */
        ChkDR( _UpdateLicenseWithSymmetricKey( f_pdstrLicense,
                                              &dstrData,
                                               f_pcontextLEVL,
                                               f_pslk ) );

        /*
        ** No need to verify content key since the above call of
        ** _UpdateLicenseWithSymmetricKey() has done so already.
        */
        f_fVerifiedCK = TRUE;
    }
    else
    {
        /*
        ** Leaf licenses are already symmetrically bound to the content key in the root license.
        ** In this case, remove any <SYMVALUE> node so that we will not use it erroneously in the
        ** future. This also ensures that the algorithm will be set to eMSDRM_CK for this leaf.
        */
        if( DRM_SUCCEEDED( DRM_XML_GetSubNode( (DRM_CONST_STRING *)f_pdstrLicense,
                                               &g_dstrTagSymValue,
                                                NULL,
                                                NULL,
                                                0,
                                               &dstrSymValueNode,
                                                NULL,
                                                2 ) ) )
        {
            ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrLicense, &dstrSymValueNode ) );
        }
    }

    /*
    ** Always symmetrically rebind the license signature to the device key.
    */
    ChkDR( _UpdateLicenseWithSymmetricSignature( f_pdstrLicense,
                                                 &dstrData,
                                                 f_pcontextLEVL,
                                                 f_pslk,
                                                 f_fVerifiedCK ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_VerifySymmetricSignatureIfPresent(
    __in  const DRM_CONST_STRING *f_pdstrLicense,
    __in        DRM_BB_CONTEXT   *f_pcontextBBX,
    __in        DRM_CONST_STRING *f_pdstrLIData,
    __in        DRM_CONST_STRING *f_pdstrSignature,
    __out       DRM_LONG         *f_plResult )
{
    DRM_RESULT         dr                = DRM_SUCCESS;
    DRM_CONST_STRING   dstrSignatureNode = EMPTY_DRM_STRING;
    DRM_SYMMOPT_STATUS eSymmOptStatus    = DRM_SYMMOPT_STATUS_NONE;
    DRM_GUID           ckVerifiedGUID    = EMPTY_DRM_GUID;
    DRM_DWORD          dwSize            = DRM_ECC160_SIGNATURE_LEN;

    /* Look for a SYMMETRIC signature first.  If not found then proceed to regular signature verification */
    if ( DRM_SUCCEEDED( DRM_SYMOPT_LIC_CheckSymmOptStatus( f_pdstrLicense, &eSymmOptStatus, &dstrSignatureNode, f_pdstrSignature, &ckVerifiedGUID ) ) )
    {
        if ( eSymmOptStatus > DRM_SYMMOPT_STATUS_NONE )
        {
            /* There is a symmetric signature, verify it. */
            DRM_CONST_STRING dstrEnablingBits = EMPTY_DRM_STRING;
            DRM_CONST_STRING dstrPubKey = EMPTY_DRM_STRING;
            DRM_DWORD cbPubKey = 0;
            PUBKEY oPubKey = { 0 };
            DRM_BYTE rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )];

            /* Try to get the device public key from the enabling bits node. */
            if ( DRM_SUCCEEDED( DRM_XML_GetSubNode( f_pdstrLIData,
                                                    &g_dstrTagEnablingbits,
                                                    NULL,
                                                    NULL,
                                                    0,
                                                    &dstrEnablingBits,
                                                    NULL,
                                                    1 ) ) )
            {
                ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBits,
                                                 &g_dstrTagPubkey,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 &dstrPubKey,
                                                 g_wchForwardSlash ) );
            }
            else
            {
                /* Try to find a "pubkey" attribute from the "SYMSIGNATURE" tag. */
                ChkDR( DRM_XML_GetNodeAttribute( &dstrSignatureNode,
                                                 &g_dstrAttributePubkey,
                                                 &dstrPubKey ) );
            }

            /* Decode the device public key. */
            cbPubKey = SIZEOF( PUBKEY );
            ChkDR( DRM_B64_DecodeW( &dstrPubKey, &cbPubKey, ( DRM_BYTE * )&oPubKey, 0 ) );

            dwSize = SIZEOF( rgbSymSig );
            ChkDR( DRM_B64_DecodeW( f_pdstrSignature, &dwSize, rgbSymSig, 0 ) );

            if ( eSymmOptStatus == DRM_SYMMOPT_STATUS_PARTIAL_NOTAG )
            {
                if ( DRM_SUCCEEDED( DRM_BBX_SymmetricVerify( f_pcontextBBX,
                                                             &oPubKey,
                                                             PB_DSTR( f_pdstrLIData ),
                                                             CB_DSTR( f_pdstrLIData ),
                                                             NULL,
                                                             0,
                                                             NULL,
                                                             rgbSymSig ) ) )
                {
                    /* Signature verification passed.  Skip the rest of the function */
                    *f_plResult = 1;
                    goto ErrorExit;
                }
            }
            else
            {
                if ( DRM_SUCCEEDED( DRM_BBX_SymmetricVerify( f_pcontextBBX,
                                                             &oPubKey,
                                                             PB_DSTR( f_pdstrLIData ),
                                                             CB_DSTR( f_pdstrLIData ),
                                                             ( DRM_BYTE * )&ckVerifiedGUID,
                                                             SIZEOF( DRM_GUID ),
                                                             NULL,
                                                             rgbSymSig ) ) )
                {
                    /* Signature verification passed.  Skip the rest of the function */
                    *f_plResult = 1;
                    goto ErrorExit;
                }
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_PerformAppRevocationCheck(
    __in                                      DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    __in                                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                const DRM_CONST_STRING            *f_pdstrContentLicense,
    __in                                      APPCERT                     *f_pAppCert,
    __in_bcount( f_cbRevocationBuffer )       DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                      DRM_DWORD                    f_cbRevocationBuffer,
    __inout                                   DRM_REVLIST_ID_VALUES       *f_pRevListIDs )
{
    DRM_RESULT       dr                         = DRM_SUCCESS;
    DRM_CONST_STRING dstrAttributeData          = EMPTY_DRM_STRING;
    DRM_DWORD        cbBuffer                   = 0;
    DRM_DWORD        crlVersion                 = 0;
    DRM_BOOL         fUpdatedRevocationList     = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_PerformAppRevocationCheck );

    ChkArg ( f_pContextCrypto != NULL
        &&  f_pContextRev != NULL
        &&  f_pRevListIDs  != NULL);

    dr = DRM_WMDRM_LIC_GetAttribute( f_pdstrContentLicense,
                                     &g_dstrAppRevocation,
                                     DRM_LICENSE_ATTRIB_REVOCATION,
                                     NULL,
                                     &dstrAttributeData,
                                     0 );

    if (dr == DRM_E_XMLNOTFOUND)
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    ChkDR(dr);  /* other errors */

    /* Ensure latest app crl version is up to date */
    cbBuffer = f_cbRevocationBuffer;
    crlVersion = f_pRevListIDs->wmdrmapp;
    ChkDR( DRM_RVK_GetCurrentAppRevocationList( f_pContextRev,
                                                f_pbRevocationBuffer,
                                                &cbBuffer,
                                                &crlVersion ) );
    if( crlVersion > f_pRevListIDs->wmdrmapp )
    {
        f_pRevListIDs->wmdrmapp = crlVersion;
        f_pRevListIDs->fUpdatedRevocationList = TRUE;
    }

    ChkDR(DRM_RVK_UpdateAppRevocationListW( f_pContextCrypto,
                                            f_pContextRev,
                                            f_pbRevocationBuffer,
                                            f_cbRevocationBuffer,
                                            &dstrAttributeData,
                                            f_pAppCert,
                                            f_pRevListIDs->wmdrmapp,
                                            &fUpdatedRevocationList,
                                            NULL));
    if( fUpdatedRevocationList )
    {
        f_pRevListIDs->fUpdatedRevocationList = TRUE;
    }

ErrorExit:
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        dr = DRM_E_REVOCATION_BUFFER_TOO_SMALL;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetCopyOPLRestriction(
    __inout     DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __in  const DRM_CONST_STRING        *f_pdstrAction,
    __in  const DRM_CONST_STRING        *f_pdstrRightUsed,
    __out       DRM_BOOL                *f_pfHasOPLs,
    __out       DRM_DWORD               *f_pdwResult )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING dstrRestriction   = EMPTY_DRM_STRING;

    AssertChkArg( f_pcontextLQ != NULL );
    AssertChkArg( f_pfHasOPLs  != NULL );
    AssertChkArg( DRM_COPYOPL_IsCopyOplSupported() );
    AssertChkArg( f_pcontextLQ->fCurrentLicenseIsXML );

    /* Intentionally do NOT initialize *f_pfHasOPLs = FALSE, as it may already be set */

    dr = DRM_WMDRM_LIC_GetEvent( &f_pcontextLQ->pLicEval->dstrContentLicense,
                                 &g_dstrLicEvalOnAction,
                                  f_pdstrRightUsed,
                                  NULL,
                                  NULL,
                                  NULL,
                                 &dstrRestriction );
    if( dr == DRM_SUCCESS  &&  dstrRestriction.cchString > 0 )
    {
        *f_pfHasOPLs = TRUE;

        /* examine OPL here for CanCopy */
        if( DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_COPY ) )
        {
            DRM_STACK_ALLOCATOR_CONTEXT stack = {0};

            stack.pbStack  = f_pcontextLQ->copyOPL.rgbCopyOPLBuffer;
            stack.cbStack  = SIZEOF(f_pcontextLQ->copyOPL.rgbCopyOPLBuffer);

            dr = DRM_COPYOPL_ProcessCopyOutputLevelData( &dstrRestriction, &f_pcontextLQ->copyOPL.oplCopy, &stack );
            if( dr == DRM_SUCCESS )
            {
                dr = DRM_COPYOPL_ASD_ExamineOutputLevels( f_pcontextLQ, &f_pcontextLQ->copyOPL.oplCopy, f_pdwResult );
                if( dr != DRM_SUCCESS )
                {
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }
            }
        }
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_LIC_HasOPLRestriction(
    __in const DRM_CONST_STRING *f_pdstrLicense,
    __in const DRM_CONST_STRING *f_pdstrRightUsed )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_CONST_STRING dstrRestriction = EMPTY_DRM_STRING;

    dr = DRM_WMDRM_LIC_GetEvent(  f_pdstrLicense,
                                 &g_dstrLicEvalOnAction,
                                  f_pdstrRightUsed,
                                  NULL,
                                  NULL,
                                  NULL,
                                 &dstrRestriction );
    if( ( dr == DRM_SUCCESS ) && ( dstrRestriction.cchString > 0 ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_LIC_ShouldCheckClockRollback(
    __in const DRM_CONST_STRING *f_pdstrLicense )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_CONST_STRING dstrExpr = EMPTY_DRM_STRING;

    /*
    ** For XML licenses, there are 3 possibilities:
    ** 1. No action on clock rollback
    **      In this case, this call will fail or not init dstrExpr
    ** 2. Delete on rollback
    **      If in rollback, the license is deleted and we caught it above
    **      Else, we won't get a value for OldSaveTime below and
    **            we won't actually check for rollback below
    ** 3. Disable on rollback
    **      If in rollback, we'll get a value for OldSaveTime below and
    **            we'll actually check for rollback below and
    **            we'll recognize rollback and disable the license
    **      Else, we'll get a value for OldSaveTime below and
    **            we'll actually check for rollback below but
    **            we'll recognize that rollback did not occur
    ** Therefore, it's safe to check for rollback any time
    **  this call succeeds and inits dstrExpr
    */
    dr = DRM_WMDRM_LIC_GetEvent(  f_pdstrLicense,
                                 &g_dstrLicEvalOnClockRollback,
                                  NULL,
                                  NULL,
                                  NULL,
                                 &dstrExpr,
                                  NULL );

    if( DRM_SUCCEEDED( dr ) && dstrExpr.cchString != 0 )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

EXIT_PK_NAMESPACE_CODE;

