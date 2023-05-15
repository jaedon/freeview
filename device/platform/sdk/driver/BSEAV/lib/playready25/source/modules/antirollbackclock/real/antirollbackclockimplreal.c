/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_ANTIROLLBACKCLOCKIMPLREAL_C
#include <drmantirollbackclock.h>
#include <drmlicreason.h>
#include <drmdomainstore.h>
#include <drmwmdrm.h>
#include <drmantirollbackclockconstants.h>
#include <drmdomainapi.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define C_SECONDS_IN_ROLLBACK_GRACE_PERIOD 30

/******************************************************************************
**
** Function: DRM_ARCLK_LEVL_OverrideMachineDateTimeWithSavedValue
**
** Synopsis: Overrides the machine datetime with the value stored
**           in the secure store if it is set and
**           is greater by at least the grace period.
**
** Arguments:
**      [f_pcontextLicEval]      : Specifies the liceval context to use
**      [f_pui64MachineDateTime] : Returns the machine date/time
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_OverrideMachineDateTimeWithSavedValue(
    __inout     DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __out       DRM_UINT64          *f_pui64MachineDateTime )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr    = DRM_SUCCESS;
    TOKEN       token = {0};

    ChkArg( f_pContextLicEval      != NULL );
    ChkArg( f_pui64MachineDateTime != NULL );

    if( DRM_SUCCESS == DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTGlobal, &g_dstrExprVarSavedDateTime, &token ) )
    {
        const DRM_UINT64 u64Zero = DRM_UI64LITERAL( 0, 0 );

        /* Compare for the grace period */
        if( !DRM_UI64Eql( *f_pui64MachineDateTime, u64Zero )
         && DRM_UI64Les( *f_pui64MachineDateTime, token.val.u64DateTime ) )
        {
            DRM_UINT64 u64Diff = DRM_UI64Sub( token.val.u64DateTime, *f_pui64MachineDateTime );

            if( DRM_UI64Les( u64Diff, DRM_UI64( C_SECONDS_IN_ROLLBACK_GRACE_PERIOD * C_TICS_PER_SECOND ) ) )
            {
                *f_pui64MachineDateTime = token.val.u64DateTime;
            }
        }
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: DRM_ARCLK_LEVL_GetMachineDatetime
**
** Synopsis: Get the "machine.datetime" for the antirollback clock
**
** Arguments:
**      [f_pcontextLicEval]      : Specifies the liceval context to use
**      [f_pui64MachineDateTime] : Returns the machine date/time
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_GetMachineDatetime(
    __inout DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __out   DRM_UINT64          *f_pui64MachineDateTime )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr               = DRM_SUCCESS;

    ChkArg( f_pContextLicEval    != NULL );
    ChkArg( f_pui64MachineDateTime != NULL );

    ChkDR( DRM_LEVL_GetMachineDatetime( f_pContextLicEval, f_pui64MachineDateTime ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_ASD_CheckRollback(
    __inout DRM_LICEVAL_CONTEXT *f_pContextLicEval )
{
    DRM_RESULT              dr               = DRM_SUCCESS;
    DRM_BOOL                fCheckRollback   = FALSE;

    if( f_pContextLicEval->fLicenseIsXMR )
    {
        if( XMR_HAS_TIME_BASED_RESTRICTIONS( f_pContextLicEval->plicenseXMR ) )
        {
            /*
            ** Always check rollback for XMR licenses with
            ** time-based restrictions
            */
            fCheckRollback = TRUE;
        }
    }
    else
    {
        if( DRM_WMDRM_LIC_ShouldCheckClockRollback( &f_pContextLicEval->dstrContentLicense ) )
        {
            fCheckRollback = TRUE;
        }
    }

    fCheckRollback = fCheckRollback && !f_pContextLicEval->fLicenseFromPRND;    /* Ignore anti-rollback clock checks on PRND licenses. SST entries not created. */

    if( fCheckRollback )
    {
        DRM_UINT64       u64MachineDateTime;
        TOKEN            token       = {0};
        const DRM_UINT64 u64Zero     = DRM_UI64LITERAL( 0, 0 );

        /* Get machine.datetime */
        dr = DRM_ARCLK_LEVL_GetMachineDatetime( f_pContextLicEval, &u64MachineDateTime );
        if( DRM_FAILED( dr ) )
        {
            /* A failure is unexpected */
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }

        if( DRM_UI64Eql( u64MachineDateTime, u64Zero ) )
        {
            /*
            ** Datetimes should always be non-zero.
            ** This is because zero represents Jan 1, 1601.
            ** Treat zero as a clock rollback case.
            */
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }

        /* Get secstate.OldSaveTime */
        ZEROMEM( &token, SIZEOF( token ) );
        dr = DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                   &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                   &token );

        if( DRM_FAILED( dr ) )
        {
            /* A failure is unexpected */
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }

        if( f_pContextLicEval->fLicenseIsXMR && dr == DRM_S_FALSE )
        {
            /* Should always be able to get a value for XMR licenses */
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }

        /*
        ** For XML licenses, though, we might not get a value
        ** So, only check the actual rollback state if we got a value
        */
        if ( dr == DRM_SUCCESS )
        {
            if( token.TokenType != TOKEN_DATETIME )
            {
                /* Should always get a datetime value */
                dr = DRM_S_FALSE;
                goto ErrorExit;
            }

            /* if ( u64MachineDateTime < token.val.u64DateTime ) */
            if ( DRM_UI64Les( u64MachineDateTime,
                              token.val.u64DateTime ) )
            {
                /* Rollback state - Disable license */
                dr = DRM_S_FALSE;
                goto ErrorExit;
            }
        }
        else
        {
            dr = DRM_SUCCESS;
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_DisableOnRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR,
    __out         DRM_BOOL            *f_pfDisabled )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr                 = DRM_SUCCESS;
    DRM_UINT64              u64MachineDateTime = { 0 };
    TOKEN                   token              = { 0 };

    ChkArg( f_pContextLicEval != NULL );
    ChkArg( f_pXMR != NULL );
    ChkArg( f_pfDisabled != NULL );
    *f_pfDisabled = FALSE;

    /*
    ** If the license should be disabled on clock rollback
    ** require that "machine.datetime >= secstate.OldSaveTime"
    ** Basically, don't let the license be used before it was saved
    ** or before the clock rollback time
    */
    if( XMR_HAS_TIME_BASED_RESTRICTIONS( f_pXMR )
     && f_pContextLicEval->pcontextSSTLicense != NULL
     && !f_pContextLicEval->fLicenseFromPRND )          /* Ignore anti-rollback clock checks on PRND licenses. SST entries not created. */
    {
        const DRM_UINT64 u64Zero = DRM_UI64LITERAL( 0, 0 );

        /*
        ** Get machine.datetime
        */
        ChkDR( DRM_ARCLK_LEVL_GetMachineDatetime( f_pContextLicEval, &u64MachineDateTime ) );

        if( DRM_UI64Eql( u64MachineDateTime, u64Zero ) )
        {
            /*
            ** Datetimes should always be non-zero.
            ** This is because zero represents Jan 1, 1601.
            ** Treat zero as a clock rollback case.
            */
            TRACE(("Clock rollback detected in DRM_LEVL_PerformOperationsXMR."));
            f_pContextLicEval->lReasonForFail = LR_LICENSE_TIME_CHECK_FAILURE;
            *f_pfDisabled = TRUE;
        }
        else
        {
            /*
            ** Get secstate.OldSaveTime
            */
            ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                         &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                         &token ) );

            /*
            ** If secure store entry token does not contain oldsavetime attribute for this license ( possible if we have upgraded from Emerald ),
            ** continue without reporting any error
            */
            if ( token.TokenType == TOKEN_DATETIME
              && DRM_UI64Les( u64MachineDateTime, token.val.u64DateTime ) )
            {
                TRACE(("Clock rollback detected in DRM_LEVL_PerformOperationsXMR."));
                f_pContextLicEval->lReasonForFail = LR_LICENSE_TIME_CHECK_FAILURE;
                *f_pfDisabled = TRUE;
            }
            else if( XMR_IS_DOMAIN_ID_VALID( f_pXMR ) )
            {
                ChkDR( DRM_DOM_ARCLK_LEVL_DisableOnRollback( f_pContextLicEval, f_pXMR, u64MachineDateTime, f_pfDisabled ) );
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_SaveLicenseStoreTime(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr                 = DRM_SUCCESS;
    DRM_UINT64              u64MachineDateTime = { 0 };
    TOKEN                   token              = { 0 };

    AssertChkArg( !f_pContextLicEval->fLicenseFromPRND );   /* Invalid to call for PRND licenses. */

    /*
    ** If the license should be disabled on clock rollback
    ** evaluate "!exists( secstate.OldSaveTime )?secstate.OldSaveTime=machine.datetime:0;"
    ** Basically, save the time the license was first saved
    */
    if ( XMR_HAS_TIME_BASED_RESTRICTIONS( f_pXMR ) )
    {
        /*
        ** Get secstate.OldSaveTime
        */
        ZEROMEM( &token, SIZEOF( token ) );
        ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                      &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                      &token ) );
        /*
        ** Only set it if it doesn't already exist
        */
        if( dr == DRM_S_FALSE )
        {
            /*
            ** Get machine.datetime
            */
            ChkDR( DRM_ARCLK_LEVL_GetMachineDatetime( f_pContextLicEval, &u64MachineDateTime ) );

            /*
            ** Set the time this license was saved
            */
            ZEROMEM( &token, SIZEOF( token ) );
            token.val.u64DateTime = u64MachineDateTime;
            token.TokenType = TOKEN_DATETIME;
            ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                          &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                          &token ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_ReportClockRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr                 = DRM_SUCCESS;
    DRM_UINT64              u64MachineDateTime = { 0 };
    TOKEN                   token              = { 0 };
    TOKEN                   tSaved             = { 0 };

    AssertChkArg( !f_pContextLicEval->fLicenseFromPRND );   /* Invalid to call for PRND licenses. */

    /*
    ** If the license should be disabled on clock rollback
    ** Set the license save time to a newer last know good time
    */
    if ( XMR_HAS_TIME_BASED_RESTRICTIONS( f_pXMR ) )
    {
        /*
        ** Get secstate.OldSaveTime
        */
        ZEROMEM( &token, SIZEOF( token ) );
        ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                      &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                      &token ) );

        if ( dr == DRM_S_FALSE  || token.TokenType != TOKEN_DATETIME )
        {
            /*
            ** If the oldsavetime attribute doesn't exists
            ** ( possible if we upgraded from emerald )
            ** create one now
            */

            /*
            ** Get machine.datetime
            */
            ChkDR( DRM_ARCLK_LEVL_GetMachineDatetime( f_pContextLicEval, &u64MachineDateTime ) );

            /*
            ** Set the time this license was saved
            */
            ZEROMEM( &token, SIZEOF( token ) );
            token.val.u64DateTime = u64MachineDateTime;
            token.TokenType = TOKEN_DATETIME;
            ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                          &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                          &token ) );
        }

        /*
        ** Get secstate.global.saveddatetime
        */
        ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTGlobal,
                                      &g_dstrExprVarSavedDateTime,
                                      &tSaved ) );
        ChkBOOL( dr != DRM_S_FALSE  && token.TokenType == TOKEN_DATETIME, DRM_E_CPRMEXP_RETRIEVAL_FAILURE );

        if ( DRM_UI64Les( token.val.u64DateTime, tSaved.val.u64DateTime ) )
        {
            /*
            ** Set the time this license was rolled back
            */
            ZEROMEM( &token, SIZEOF( token ) );
            token.val.u64DateTime = tSaved.val.u64DateTime;
            token.TokenType = TOKEN_DATETIME;
            ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                          &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                          &token ) );
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_ARCLK_DOMST_GetDomainSavedDateTime
**
** Synopsis: Gets the domain saved datetime for the given account ID.
**
** Arguments:
**
** [f_rgbPasswordSST]       -- Domain Store Secure Store Password generated by
**                             DRM_ARCLK_DOMST_CreateDomainStorePassword.
** [f_pOEMContext]          -- Pointer to an optional OEM context structure.
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain.
**                             It cannot be NULL.
** [f_puiDomainSavedDateTime]
**                          -- Set to the value of the saved datetime for the
**                             given account ID.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __out_opt                                DRM_UINT64              *f_puiDomainSavedDateTime )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( DRM_DOM_ARCLK_GetDomainSavedDateTime( f_rgbPasswordSST,
                                                 f_pOEMContext,
                                                 f_poDomainStoreContext,
                                                 f_poAccountID,
                                                 f_puiDomainSavedDateTime ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_ARCLK_DOMST_ClockRollbackDetected
**
** Synopsis: Updates saved datetimes for all domains after clock rollback.
**           Call this function when clock rollback is detected.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [ui64GlobalSavedTime]    -- Global saved datetime
**                             i.e. last known good datetime before rollback.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_ClockRollbackDetected(
    __in  DRM_BB_CONTEXT          *f_poBBXContext,
    __in  DRM_UINT64               ui64GlobalSavedTime )
{
    return DRM_DOM_ARCLK_ClockRollbackDetected( f_poBBXContext, ui64GlobalSavedTime );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_CreateDomainStorePassword(
    __in                                const DRM_GUID         *f_poAccountID,
    __in                                      DRM_BB_CONTEXT   *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE          f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    return DRM_DOM_ARCLK_CreateDomainStorePassword( f_poAccountID, f_poBBXContext, f_rgbPasswordSST );
}

/******************************************************************************
**
** Function: DRM_ARCLK_LIC_CheckClockRollback
**
** Synopsis: Check for clock rollback
**
** Arguments:
**      [f_pLicEval]        : Specifies the liceval context to use
**      [f_pLicStoreXML]    : Pointer to the XML license store (optional, may be NULL)
**      [f_pLicStoreXMR]    : Pointer to the XMR license store (optional, may be NULL)
**      [f_pLicEnum]        : Pointer to the license store enum context
**      [f_pbBuffer]        : Buffer used internally to store licenses
**      [f_cbBuffer]        : Specifies the size in bytes of f_pbBuffer
**      [f_pDatastore]      : Specifies the DST context
**      [f_poStack]         : Pointer to a stack allocator context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters is NULL
**      DRM_E_OUTOFMEMORY
**          The buffer provided is too small
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LIC_CheckClockRollback(
    __in                    DRM_LICEVAL_CONTEXT         *f_pLicEval,
    __in                    DRM_LICSTORE_CONTEXT        *f_pLicStoreXML,
    __in                    DRM_LICSTORE_CONTEXT        *f_pLicStoreXMR,
    __in                    DRM_LICSTOREENUM_CONTEXT    *f_pLicEnum,
    __in_ecount(f_cbBuffer) DRM_BYTE                    *f_pbBuffer,
    __in                    DRM_DWORD                    f_cbBuffer,
    __in                    DRM_DST                     *f_pDatastore,
    __in                    DRM_STACK_ALLOCATOR_CONTEXT *f_poStack )
{
    DRM_RESULT dr                   = DRM_SUCCESS;
    DRM_BOOL   fClockRolledBack     = TRUE;
    DRM_BOOL   fEvalTmp             = FALSE;
    DRM_BOOL   fResetSavedDateTime  = FALSE;
    DRM_KID    oKID                 = { 0 };
    DRM_LID    oLID                 = { 0 };
    DRM_DWORD  i                    = 0;
    DRM_BYTE   rgbPassword [__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };
    DRM_XMR_LICENSE oLicXMR         = { 0 };

    ChkArg( f_pLicEval    != NULL );
    ChkArg( f_pLicEnum    != NULL );
    ChkArg( f_pDatastore  != NULL );
    ChkArg( f_pbBuffer    != NULL );
    ChkArg( f_poStack     != NULL );

    {
        TOKEN tokSavedDateTime;
        ChkDR( DRM_SST_GetTokenValue( f_pLicEval->pcontextSSTGlobal, &g_dstrExprVarSavedDateTime, &tokSavedDateTime ) );
        if( dr == DRM_S_FALSE )
        {
            /*
            ** This error code indicates the the global clock rollback value in the secure store
            ** doesn't exist, which would be the case if this is a fresh DST file. Let's rollback
            ** anyway, because there shouldn't be any licenses if it's fresh and if there are then
            ** someone is tampering.
            */
            dr = DRM_SUCCESS;
            fClockRolledBack = TRUE;
        }
        else
        {
            /*
            ** Clock is rolled back if machine datetime < saved datetime
            */
            DRM_UINT64 ui64MachineDateTime = DRM_UI64LITERAL( 0, 0 );
            ChkDR( DRM_LEVL_GetMachineDatetime( f_pLicEval, &ui64MachineDateTime ) );
            fClockRolledBack = DRM_UI64Les( ui64MachineDateTime, tokSavedDateTime.val.u64DateTime );
        }
    }

    if( fClockRolledBack )
    {
        TOKEN tSaved = { 0 };

        for ( i = 0; i < 2; i++ )
        {
            DRM_LICSTORE_CONTEXT *pLicStore = ( i == 0 ) ? f_pLicStoreXML : f_pLicStoreXMR;

            if ( pLicStore != NULL )
            {
                /*
                ** The clock set string in this case will set the saved date time to the current time
                ** If this code path isn't executed the expression logic will only set the clock to a time
                ** forward from what the current saved date time is
                */
                fResetSavedDateTime = TRUE;

                /*
                ** There was a failure or the clock was roll back.  In either case we report that as
                ** clock rollback
                */
                ChkDR( DRM_LST_InitEnum( pLicStore, NULL, FALSE, f_pLicEnum ) );

                while( TRUE ) /* Loop will break when DRM_E_NOMORE is hit */
                {
                    DRM_DWORD cbLicSize        = 0;
                    DRM_DST_SLOT_HINT slotHint = { 0 };

                    cbLicSize = f_cbBuffer;
                    dr = DRM_LST_EnumNext( f_pLicEnum, &oKID, &oLID, &slotHint, &cbLicSize );

                    if ( dr == DRM_E_NOMORE )
                    {
                        dr = DRM_SUCCESS;
                        break;
                    }
                    ChkDR( dr );

                    /*
                    ** We could fail because the buffer isn't big enough.
                    */
                    ChkBOOL( cbLicSize <= f_cbBuffer, DRM_E_OUTOFMEMORY );

                    ChkDR( DRM_LST_GetLicense( pLicStore,
                                              &oKID,
                                              &oLID,
                                              &slotHint,
                                               f_pbBuffer,
                                              &cbLicSize ) );

                    DSTR_FROM_PB( &( f_pLicEval->dstrContentLicense ), f_pbBuffer, cbLicSize );
                    f_pLicEval->fUseCachedAttribs = FALSE;

                    MEMCPY( &f_pLicEval->LID, &oLID, SIZEOF( DRM_LID ) );

                    ChkDRContinue( DRM_BBX_HashValue( f_pLicEval->LID.rgb,
                                                      SIZEOF( f_pLicEval->LID.rgb ),
                                                      rgbPassword,
                                                      f_pLicEval->pcontextBBX ) );

                    ChkDRContinue( DRM_SST_OpenKeyTokens( f_pLicEval->pcontextSSTLicense,
                                                         &f_pLicEval->LID,
                                                          NULL,
                                                          rgbPassword,
                                                          0,
                                                          SECURE_STORE_LICENSE_DATA,
                                                          f_pDatastore ) );

                    f_pLicEval->fGlobalSecStoreWritable = FALSE;
                    f_pLicEval->dwFlags                 = LICEVAL_OPEN_CHECK_SETTINGS;
                    f_pLicEval->fReserved               = 0;
                    f_pLicEval->fLicenseIsXMR           = ( pLicStore == f_pLicStoreXMR );
                    f_pLicEval->fDeleteLicense          = FALSE;

                    if ( f_pLicEval->fLicenseIsXMR )
                    {
                        f_pLicEval->plicenseXMR = &oLicXMR;

                        ChkDR( DRM_XMR_UnpackLicense( f_pbBuffer,
                                                      cbLicSize,
                                                      f_poStack,
                                                      f_pLicEval->plicenseXMR ) );
                    }

                    dr = DRM_LEVL_PerformOperations( f_pLicEval,
                                                     DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK,
                                                     DRM_LICENSE_EVAL_DONE_WITH_OPERATION,
                                                     NULL,
                                                    &fEvalTmp,
                                                     NULL,
                                                     f_pDatastore,
                                                     NULL,
                                                     NULL );

                    if ( f_pLicEval->fDeleteLicense )
                    {
                        ( void ) DRM_LST_EnumDelete( f_pLicEnum );
                    }

                    DRM_SST_CloseKey( f_pLicEval->pcontextSSTLicense, f_pDatastore, NULL, NULL );
                }
            }
        }

        ChkDR( DRM_SST_GetTokenValue( f_pLicEval->pcontextSSTGlobal,
                                     &g_dstrExprVarSavedDateTime,
                                     &tSaved ) );

        /*
        ** For domains, only initiate clock rollback if there's actually a saveddatetime
        */
        if( dr != DRM_S_FALSE )
        {
            ChkBOOL( tSaved.TokenType == TOKEN_DATETIME, DRM_E_CPRMEXP_RETRIEVAL_FAILURE );

            ChkDR( DRM_ARCLK_DOMST_ClockRollbackDetected( f_pLicEval->pcontextBBX,
                                                          tSaved.val.u64DateTime ) );
        }
        else
        {
            dr = DRM_SUCCESS;
        }
    }

    /*
    ** update the saveddatetime -- ignore errors here
    */
    f_pLicEval->fGlobalSecStoreWritable = TRUE;
    {
        if( fResetSavedDateTime )
        {
            (void)DRM_ARCLK_LIC_ResetClockRollback( f_pLicEval );
        }
        else
        {
            /*
            ** if saved datetime < machine datetime { saved datetime = machine datetime }
            */
            TOKEN tokSavedDateTime;
            DRM_UINT64 ui64MachineDateTime = DRM_UI64LITERAL( 0, 0 );

            if( DRM_SUCCEEDED( DRM_LEVL_GetMachineDatetime( f_pLicEval, &ui64MachineDateTime ) ) )
            {
                dr = DRM_SST_GetTokenValue( f_pLicEval->pcontextSSTGlobal, &g_dstrExprVarSavedDateTime, &tokSavedDateTime );
                if( dr == DRM_S_FALSE || DRM_UI64Les( tokSavedDateTime.val.u64DateTime, ui64MachineDateTime ) )
                {
                    tokSavedDateTime.TokenType       = TOKEN_DATETIME;
                    tokSavedDateTime.val.u64DateTime = ui64MachineDateTime;
                    (void)DRM_SST_SetTokenValue( f_pLicEval->pcontextSSTGlobal, &g_dstrExprVarSavedDateTime, &tokSavedDateTime );
                }
                dr = DRM_SUCCESS;
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LIC_ResetClockRollback(
    __in DRM_LICEVAL_CONTEXT       *f_pLicEval )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_UINT64  ui64MachineDateTime = DRM_UI64LITERAL( 0, 0 );

    ChkArg( f_pLicEval != NULL );

    /* Write it in Anti Rollback Store. It must already be in writable state */
    f_pLicEval->fGlobalSecStoreWritable = TRUE;

    /*
    ** saved datetime = machine datetime
    */
    if( DRM_SUCCEEDED( DRM_LEVL_GetMachineDatetime( f_pLicEval, &ui64MachineDateTime ) ) )
    {
        TOKEN tokSavedDateTime;
        tokSavedDateTime.TokenType       = TOKEN_DATETIME;
        tokSavedDateTime.val.u64DateTime = ui64MachineDateTime;
        (void)DRM_SST_SetTokenValue( f_pLicEval->pcontextSSTGlobal, &g_dstrExprVarSavedDateTime, &tokSavedDateTime );
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

