/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmviewrights.h>
#include <drmlicreason.h>
#include <drmdeletedstore.h>
#include <drmantirollbackclock.h>
#include <drmcopyopl.h>
#include <drmthumbnail.h>
#include <drmwmdrm.h>
#include <drmutilitieslite.h>
#include <drmviewrightsconstants.h>

ENTER_PK_NAMESPACE_CODE;

/* Is the date a Negative Infinity representation? */
static DRMINLINE DRM_BOOL _NegInfDate( IN const DRMFILETIME *f_pft )
{
    return ( f_pft->dwLowDateTime  == 0
          && f_pft->dwHighDateTime == 0 );
}

/* Is the date a Positive Infinity representation? */
static DRMINLINE DRM_BOOL _InfDate( IN const DRMFILETIME *f_pft )
{
    return ( f_pft->dwLowDateTime  == 0xFFFFFFFF
          && f_pft->dwHighDateTime == 0xFFFFFFFF );
}

/* Initilize a date to a Negative Infinity representation */
static DRMINLINE DRM_VOID _InitNegInfDate(
    OUT DRMFILETIME *f_pDate )
{
    ZEROMEM( f_pDate, SIZEOF( DRMFILETIME ) );
}

/* Initilize a date to a Positive Infinity representation */
static DRMINLINE DRM_VOID _InitInfDate(
    OUT DRMFILETIME *f_pDate )
{
    MEMSET( f_pDate, 0xFF, SIZEOF( DRMFILETIME ) );
}

/* Is the date an already expired? */
static DRMINLINE DRM_BOOL _IsExpired( IN DRM_VOID *pOEMContext, IN const DRMFILETIME *f_pft )
{
    if( _InfDate( f_pft ) )
    {
        return FALSE;
    }
    else
    {
        DRMFILETIME ftNow;

        Oem_Clock_GetSystemTimeAsFileTime( pOEMContext, &ftNow );

        return ( DRM_UTL_DateLessThan( f_pft, &ftNow ) );
    }
}

/* Does today fall between Begin and End (inclusive)? */
static DRMINLINE DRM_BOOL _IsDateUsable(
    IN       DRM_VOID    *pOEMContext,
    IN const DRMFILETIME *f_pftBegin,
    IN const DRMFILETIME *f_pftEnd )
{
    if( _NegInfDate( f_pftBegin )
     && _InfDate(    f_pftEnd ) )
    {
        return TRUE;
    }
    else
    {
        DRMFILETIME ftNow;

        Oem_Clock_GetSystemTimeAsFileTime( pOEMContext, &ftNow );

        /* Use !(b<a) to achive a<=b (so that the check is inclusive) */
        return (  !DRM_UTL_DateLessThan( &ftNow,     f_pftBegin )
               && !DRM_UTL_DateLessThan(  f_pftEnd, &ftNow ) );
    }
}

/* Do 2 sets of begin/end dates overlap each other? */
static DRMINLINE DRM_BOOL _OverlappingDates(
    IN const DRMFILETIME *f_pftBeginDate1,
    IN const DRMFILETIME *f_pftEndDate1,
    IN const DRMFILETIME *f_pftBeginDate2,
    IN const DRMFILETIME *f_pftEndDate2 )
{
    return !( DRM_UTL_DateLessThan( f_pftEndDate2, f_pftBeginDate1 )
           || DRM_UTL_DateLessThan( f_pftEndDate1, f_pftBeginDate2 ) );
}

/* Read out BegDate and EndDate from State Data */
static DRMINLINE DRM_VOID _ReadDates(
    IN const DRM_LICENSE_STATE_DATA    *f_pstatedata,
       OUT   DRMFILETIME               *f_pstateBegDate,
       OUT   DRMFILETIME               *f_pstateEndDate )
{
    /* Initialize stateBegDate and stateEndDate. */
    _InitNegInfDate( f_pstateBegDate );
    _InitInfDate(    f_pstateEndDate );

    /* Handle dates */
    if( f_pstatedata->dwCategory == DRM_LICENSE_STATE_FROM
     || f_pstatedata->dwCategory == DRM_LICENSE_STATE_COUNT_FROM )
    {
        *f_pstateBegDate = f_pstatedata->datetime[0];
    }
    else if( f_pstatedata->dwCategory == DRM_LICENSE_STATE_FROM_UNTIL
          || f_pstatedata->dwCategory == DRM_LICENSE_STATE_COUNT_FROM_UNTIL )
    {
        *f_pstateBegDate = f_pstatedata->datetime[0];
        *f_pstateEndDate = f_pstatedata->datetime[1];
    }
    else if( f_pstatedata->dwCategory == DRM_LICENSE_STATE_UNTIL
          || f_pstatedata->dwCategory == DRM_LICENSE_STATE_COUNT_UNTIL )
    {
        *f_pstateEndDate = f_pstatedata->datetime[0];
    }
}

/*****************************************************************************/

static DRM_RESULT _GetSecStateAttr(
    IN       DRM_SECSTORE_CONTEXT *f_contextSST,
    IN const DRM_CONST_STRING     *f_pdstrAttribute,
       OUT   DRM_DWORD            *f_pdwValue,
       OUT   DRMFILETIME          *f_pft )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TOKEN      token;

    dr = DRM_SST_GetTokenValue( f_contextSST, f_pdstrAttribute, &token );
    if( DRM_FAILED(dr)
     || dr == DRM_S_FALSE )
    {
        /* Unable to retrieve the value. */
        ChkDR( CPRMEXP_RETRIEVAL_FAILURE );
    }

    switch(token.TokenType)
    {
    case TOKEN_LONG:
        if( f_pdwValue != NULL )
        {
            *f_pdwValue = token.val.lValue;
        }
        else
        {
            dr = DRM_S_FALSE; /* We are not passing any data back */
        }
        break;
    case TOKEN_DATETIME:
        if( f_pft != NULL )
        {
            UI64_TO_FILETIME( token.val.u64DateTime, *f_pft );
        }
        else
        {
            dr = DRM_S_FALSE; /* We are not passing any data back */
        }
        break;
    default:
        dr = DRM_S_FALSE; /* We don't retrieve any string data for license view. */
    }

ErrorExit:
    return dr;
}

/*****************************************************************************/

static DRM_VOID _SetExpiryCategory(
    IN OUT   DRM_LICENSE_STATE_DATA    *f_pstatedata,
    IN const DRMFILETIME               *f_pftBeginDate,
    IN const DRMFILETIME               *f_pftEndDate )
{
    DRM_DWORD dwExpiryType = ( ( !_NegInfDate( f_pftBeginDate ) ) << 1 ) + !_InfDate( f_pftEndDate );

    switch( dwExpiryType )
    {
    case 0:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_UNLIM;
        f_pstatedata->dwNumDates    = 0;
        f_pstatedata->dwNumCounts   = 0;
        f_pstatedata->dwCount[0]    = 0;
        f_pstatedata->dwCount[1]    = 0;
        /*
        ** If only basic vagueness is set, we can remove it
        ** because this license is now unlimited type.
        ** But, if other vagueness like OPL or SAP is set, then we do nothing
        ** because even an unlimited type license can fail to be usable
        ** for those types of reasons (and is thus still vague).
        */
        if( f_pstatedata->dwVague == DRM_ASD_VAGUE )
        {
            f_pstatedata->dwVague   = 0;
        }
        break;
    case 1:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_UNTIL;
        f_pstatedata->dwNumDates    = 1;
        MEMCPY( &f_pstatedata->datetime[0], f_pftEndDate,   SIZEOF( DRMFILETIME ) );
        break;
    case 2:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_FROM;
        f_pstatedata->dwNumDates    = 1;
        MEMCPY( &f_pstatedata->datetime[0], f_pftBeginDate, SIZEOF( DRMFILETIME ) );
        break;
    case 3:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_FROM_UNTIL;
        f_pstatedata->dwNumDates    = 2;
        MEMCPY( &f_pstatedata->datetime[0], f_pftBeginDate, SIZEOF( DRMFILETIME ) );
        MEMCPY( &f_pstatedata->datetime[1], f_pftEndDate,   SIZEOF( DRMFILETIME ) );
        break;
    }
}

/*****************************************************************************/

static DRM_VOID _SetCountedExpiryCategory(
    IN OUT   DRM_LICENSE_STATE_DATA    *f_pstatedata,
    IN const DRMFILETIME               *f_pftBeginDate,
    IN const DRMFILETIME               *f_pftEndDate )
{
    DRM_DWORD dwExpiryType = ( ( !_NegInfDate( f_pftBeginDate ) ) << 1 ) + !_InfDate( f_pftEndDate );

    switch( dwExpiryType )
    {
    case 0:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_COUNT;
        f_pstatedata->dwNumDates    = 0;
        break;
    case 1:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_COUNT_UNTIL;
        f_pstatedata->dwNumDates    = 1;
        MEMCPY( &f_pstatedata->datetime[0], f_pftEndDate,   SIZEOF( DRMFILETIME ) );
        break;
    case 2:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_COUNT_FROM;
        f_pstatedata->dwNumDates    = 1;
        MEMCPY( &f_pstatedata->datetime[0], f_pftBeginDate, SIZEOF( DRMFILETIME ) );
        break;
    case 3:
        f_pstatedata->dwCategory    = DRM_LICENSE_STATE_COUNT_FROM_UNTIL;
        f_pstatedata->dwNumDates    = 2;
        MEMCPY( &f_pstatedata->datetime[0], f_pftBeginDate, SIZEOF( DRMFILETIME ) );
        MEMCPY( &f_pstatedata->datetime[1], f_pftEndDate,   SIZEOF( DRMFILETIME ) );
        break;
    }
}

/*****************************************************************************/

static DRM_RESULT _GetV2LicenseInfo(
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN const DRM_CONST_STRING        *f_pdstrLicense,
       OUT   LicenseInfo             *f_plicenseInfo,
    IN const DRM_CONST_STRING        *f_pdstrAction,
    IN       DRM_SECSTORE_CONTEXT    *f_pcontextSST )
{
    DRM_RESULT              dr                  = DRM_SUCCESS;
    DRM_CONST_STRING        dstrCountString     = EMPTY_DRM_STRING;
    const DRM_CONST_STRING *pdstrCountString    = NULL;
    DRM_WCHAR               rgwchCountString[100];
    DRM_DWORD               dwDeleted           = 0;
    DRM_BOOL                fCheckState         = TRUE;
    DRM_BOOL                fLicenseRequiresSST = TRUE;

    ZEROMEM( f_plicenseInfo, SIZEOF( LicenseInfo ) );
    _InitInfDate( &f_plicenseInfo->endDate );
    f_plicenseInfo->dwCategory = LIC_TYPE_EXPIRY; /* Treat all licenses as expiry license for now.  */

    /* RETRIEVE ATTRIBUTES ONE BY ONE AND SET THE FIELDS. */

    /* Check for license deleted */
    if (  f_pcontextLQ->pLicEval->fLicenseIsXMR )
    {
        ChkDR( DRM_XMR_RequiresSST( f_pcontextLQ->pLicEval->plicenseXMR, &fLicenseRequiresSST ) );
        if ( !fLicenseRequiresSST )
        {
            fCheckState = FALSE;
        }
    }

    if ( fCheckState )
    {
        if( ( DRM_SUCCESS == _GetSecStateAttr( f_pcontextSST, &g_dstrDRM_LS_DELETED_ATTR, &dwDeleted, NULL ) )
         && ( dwDeleted != 0 ) )
        {
            /* This license is deleted - we can't get any info about it */
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }
    }

    ChkDR( DRM_ARCLK_ASD_CheckRollback( f_pcontextLQ->pLicEval ) );
    if( dr == DRM_S_FALSE )
    {
        /*
        ** Clock rollback occurred
        */
        goto ErrorExit;
    }

    if ( fCheckState )
    {
        /* First retrieve end expiry date.  */
        if( _GetSecStateAttr( f_pcontextSST, &g_dstrDRM_LS_ENDDATE_ATTR, NULL, &f_plicenseInfo->endDate ) != DRM_SUCCESS )
        {
            if( f_pcontextLQ->fCurrentLicenseIsXML )
            {
                ( void )DRM_WMDRM_LEVL_ParseV2License( f_pdstrLicense, &g_dstrDRM_LS_ENDDATE_ATTR, NULL, &f_plicenseInfo->endDate );
            }
            else if( XMR_IS_EXPIRATION_VALID( &f_pcontextLQ->licXMR )
                  && f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Expiration.dwEndDate != XMR_UNLIMITED )
            {
                CREATE_FILE_TIME( f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Expiration.dwEndDate, f_plicenseInfo->endDate );
            }
        }

        if( _IsExpired( f_pcontextLQ->pBBContext->pOEMContext, &f_plicenseInfo->endDate ) )
        {
            f_plicenseInfo->dwCategory = LIC_TYPE_IGNORE;
            goto ErrorExit;
        }

        /* Next retrieve begin expiry date.     */
        if( _GetSecStateAttr( f_pcontextSST, &g_dstrDRM_LS_BEGDATE_ATTR, NULL, &f_plicenseInfo->begDate ) != DRM_SUCCESS )
        {
            if( f_pcontextLQ->fCurrentLicenseIsXML )
            {
                ( void )DRM_WMDRM_LEVL_ParseV2License( f_pdstrLicense, &g_dstrDRM_LS_BEGDATE_ATTR, NULL, &f_plicenseInfo->begDate );
            }
            else if( XMR_IS_EXPIRATION_VALID( &f_pcontextLQ->licXMR )
                  && f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Expiration.dwBeginDate != 0 )
            {
                CREATE_FILE_TIME( f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Expiration.dwBeginDate, f_plicenseInfo->begDate );
            }
        }

        /*
        ** Next retrieve expiration after first use. For this type of license,
        ** we will not do parsing for XML. We expect the data to be in secure store.
        ** For XMR, we have to parse.
        */
        if( ( _GetSecStateAttr( f_pcontextSST,
                               &g_dstrDRM_LS_EXPIRATION_AFTER_FIRSTUSE_ATTR,
                               &f_plicenseInfo->dwHours, NULL ) == DRM_SUCCESS )
         || ( !f_pcontextLQ->fCurrentLicenseIsXML
           && XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID(&f_pcontextLQ->licXMR) ) )
        {
            DRMFILETIME ft;

            if( !f_pcontextLQ->fCurrentLicenseIsXML
              && XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID(&f_pcontextLQ->licXMR) )
            {
                f_plicenseInfo->dwHours = f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwValue / ( 60 * 60 );
            }

            /*
            ** It is expiration after first use type of license.
            ** See if FirstUseDate is available in secure store. If it is, it is activated.
            ** If it does not exist, then it is not enabled yet.
            */
            if( _GetSecStateAttr( f_pcontextSST, &g_dstrDRM_LS_FIRSTUSE_ATTR, NULL, &ft ) == DRM_SUCCESS )
            {
                DRM_UINT64 u64;

                FILETIME_TO_UI64( ft, u64 );

                /*
                ** It is enabled. Treat the license has expiring license.
                ** Compute the expiry date according to hours allowed by the license.
                */
                f_plicenseInfo->dwCategory = LIC_TYPE_EXPIRY;
                if( !f_pcontextLQ->fCurrentLicenseIsXML
                  && XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID(&f_pcontextLQ->licXMR) )
                {
                    /* XMR license is in seconds, use the value directly to get a more accurate date */
                    u64 = DRM_UI64Add( DRM_UI64Mul( DRM_UI64( 10000000 ),
                                                    DRM_UI64( f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwValue ) ),
                                       u64 );
                }
                else
                {
                    /* XML license is in hours */
                    u64 = DRM_UI64Add( DRM_UI64Mul( DRM_UI64( 10000000 ),
                                                    DRM_UI64( 60 * 60 * f_plicenseInfo->dwHours ) ),
                                       u64 );
                }
                UI64_TO_FILETIME( u64, ft );

                /* Set the expiry as min of this and the any end expiry specified in license. */
                if( DRM_UTL_DateLessThan( &ft, &f_plicenseInfo->endDate ) )
                {
                    f_plicenseInfo->endDate = ft;
                }
            }
            else
            {
                /* If it is an expire after first use licenses we should just return
                   now so we don't override this type with counts that aren't usable */
                f_plicenseInfo->dwCategory = LIC_TYPE_EXPIRATION_AFTER_FIRST_USE;
                goto ErrorExit;
            }
        }

        if( !f_pcontextLQ->fCurrentLicenseIsXML
           && XMR_IS_EXPIRATION_AFTER_STORE_VALID(&f_pcontextLQ->licXMR) )
        {
            DRMFILETIME ft;

            if( _GetSecStateAttr( f_pcontextSST, &g_dstrDRM_LS_FIRSTSTORE_ATTR, NULL, &ft ) == DRM_SUCCESS )
            {
                DRM_UINT64 u64;

                FILETIME_TO_UI64( ft, u64 );

                /*
                ** It is enabled. Treat the license has expiring license.
                ** Compute the expiry date according to hours allowed by the license.
                */
                f_plicenseInfo->dwCategory = LIC_TYPE_EXPIRY;
                u64 = DRM_UI64Add( DRM_UI64Mul( DRM_UI64( 10000000 ),
                                                DRM_UI64( f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.ExpirationAfterStore.dwValue ) ),
                                   u64 );
                UI64_TO_FILETIME( u64, ft );

                /* Set the expiry as min of this and the any end expiry specified in license. */
                if( DRM_UTL_DateLessThan( &ft, &f_plicenseInfo->endDate ) )
                {
                    f_plicenseInfo->endDate = ft;
                }
            }
        }

        /*
        ** Next retrieve count. If it exists, we should treat it as counted expiry license.
        ** Remaining count is never parsed from the license.
        */
        dr = DRM_S_FALSE;

        if( DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_COPY_TO_CD ) )
        {
            /* wmrm sdk uses BurnToCDCount for this right instead of Print.redbookCount. So special case this too.         */
            pdstrCountString = &g_dstrDRM_LS_BURNTOCDCOUNT_ATTR;
        }
        else if( DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_PLAYBACK )
              || DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY ) )
        {
            pdstrCountString = &g_dstrDRM_LS_PLAYCOUNT_ATTR;
        }
        else
        {
            if( NO_OF( rgwchCountString ) - g_dstrDRM_LS_COUNT_ATTR.cchString >= f_pdstrAction->cchString
             && NO_OF( rgwchCountString )                                      > g_dstrDRM_LS_COUNT_ATTR.cchString )
            {
                ChkDR( DRM_STR_StringCchCopyNW( rgwchCountString,                            NO_OF(rgwchCountString),                            f_pdstrAction->pwszString,          f_pdstrAction->cchString ) );
                ChkDR( DRM_STR_StringCchCopyNW( rgwchCountString + f_pdstrAction->cchString, NO_OF(rgwchCountString) - f_pdstrAction->cchString, g_dstrDRM_LS_COUNT_ATTR.pwszString, g_dstrDRM_LS_COUNT_ATTR.cchString ) );
                dstrCountString.pwszString = rgwchCountString;
                dstrCountString.cchString  = f_pdstrAction->cchString
                                           + g_dstrDRM_LS_COUNT_ATTR.cchString;
                pdstrCountString           = &dstrCountString;
            }
        }

        if( pdstrCountString != NULL )
        {
            dr = _GetSecStateAttr( f_pcontextSST, pdstrCountString, &f_plicenseInfo->dwCount, NULL);
        }

        if( dr == DRM_SUCCESS)
        {
            /* We got the count. To play safe, make sure the count is non-zero. If zero. ignore this license. */
            if( f_plicenseInfo->dwCount == 0 )
            {
                f_plicenseInfo->dwCategory = LIC_TYPE_IGNORE;
                goto ErrorExit;
            }
            else
            {
                f_plicenseInfo->dwCategory = LIC_TYPE_COUNTED_EXPIRY;
            }
        }

        if( f_plicenseInfo->dwCategory == LIC_TYPE_COUNTED_EXPIRY )
        {
            /* Retrieve the Start Count. */

            if( DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_COPY_TO_CD ) )
            {
                /* WMRM SDK uses BeginBurnToCDCount for this right instead of BeginPrint.redbookCount. So special case this too.         */
                pdstrCountString = &g_dstrDRM_LS_BEGINBURNTOCDCOUNT_ATTR;
            }
            else if( DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_PLAYBACK ) )
            {
                pdstrCountString = &g_dstrDRM_LS_BEGINPLAYCOUNT_ATTR;
            }
            /* construct a "Begin" + action + "Count" string */
            else
            {
                DRM_DWORD cch = g_dstrDRM_LS_COUNT_ATTR.cchString + g_dstrDRM_LS_BEGIN_ATTR.cchString;

                ChkOverflow( cch, g_dstrDRM_LS_COUNT_ATTR.cchString );

                if( NO_OF( rgwchCountString ) - cch >= f_pdstrAction->cchString
                 && NO_OF( rgwchCountString )        > cch )
                {
                    /* Begin */
                    ChkDR( DRM_STR_StringCchCopyNW(rgwchCountString, NO_OF(rgwchCountString), g_dstrDRM_LS_BEGIN_ATTR.pwszString, g_dstrDRM_LS_BEGIN_ATTR.cchString ) );
                    cch = g_dstrDRM_LS_BEGIN_ATTR.cchString;

                    /* Action */
                    ChkDR( DRM_STR_StringCchCopyNW( rgwchCountString + cch, NO_OF(rgwchCountString) - cch, f_pdstrAction->pwszString, f_pdstrAction->cchString ) );
                    cch += f_pdstrAction->cchString;

                    /* Count */
                    ChkDR( DRM_STR_StringCchCopyNW( rgwchCountString + cch, NO_OF(rgwchCountString) - cch, g_dstrDRM_LS_COUNT_ATTR.pwszString, g_dstrDRM_LS_COUNT_ATTR.cchString ) );
                    dstrCountString.pwszString = rgwchCountString;
                    dstrCountString.cchString  = g_dstrDRM_LS_BEGIN_ATTR.cchString + f_pdstrAction->cchString + g_dstrDRM_LS_COUNT_ATTR.cchString;
                    pdstrCountString           = &dstrCountString;
                }
            }

            dr = _GetSecStateAttr( f_pcontextSST, pdstrCountString, &f_plicenseInfo->dwStartCount, NULL );

            if( dr != DRM_SUCCESS && f_pcontextLQ->fCurrentLicenseIsXML )
            {
                /*
                ** XMR license does not have start count
                */
                ChkDR( DRM_WMDRM_LEVL_ParseV2License( f_pdstrLicense, pdstrCountString, &f_plicenseInfo->dwStartCount, NULL ));
            }
        }
    }
    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
}

static DRM_BOOL _ConvertLicenseInfoToStateData(
    IN const LicenseInfo               *f_plicenseinfo,
       OUT   DRM_LICENSE_STATE_DATA    *f_pstatedata )
{
    DRM_BOOL fUseState = TRUE;
    switch( f_plicenseinfo->dwCategory )
    {
    case LIC_TYPE_EXPIRY:
        _SetExpiryCategory( f_pstatedata,
                           &f_plicenseinfo->begDate,
                           &f_plicenseinfo->endDate );
        break;
    case LIC_TYPE_COUNTED_EXPIRY:
        _SetCountedExpiryCategory( f_pstatedata,
                                  &f_plicenseinfo->begDate,
                                  &f_plicenseinfo->endDate );

        /*
        ** We return both counts.
        ** Since the spec does not require to send start counts,
        ** set dwNumCounts to 1.
        */
        f_pstatedata->dwNumCounts = 1;
        f_pstatedata->dwCount[0]  = f_plicenseinfo->dwCount;
        f_pstatedata->dwCount[1]  = f_plicenseinfo->dwStartCount;
        break;
    case LIC_TYPE_EXPIRATION_AFTER_FIRST_USE:
        /*
        ** The license has never been used.
        ** It is still LIC_TYPE_EXPIRATION_AFTER_FIRST_USE type.
        ** If it had been used, it would have been changed to
        ** one of the other types.
        */

        /* Set the expiry values. */
        _SetExpiryCategory( f_pstatedata,
                           &f_plicenseinfo->begDate,
                           &f_plicenseinfo->endDate );

        f_pstatedata->dwCategory  = DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE;
        f_pstatedata->dwNumCounts = 1;
        f_pstatedata->dwCount[0]  = f_plicenseinfo->dwHours;
        break;
    default:
        /*
        ** Either unknown type of license or unsupported or expired.
        ** Ignore this license.
        */
        fUseState = FALSE;
        break;
    }

    if( fUseState )
    {
        if( f_plicenseinfo->fHasOPLs )
        {
            f_pstatedata->dwVague |= DRM_ASD_OPL;
        }
        if( f_plicenseinfo->fRequiresSAP )
        {
            f_pstatedata->dwVague |= DRM_ASD_SAP;
        }
        if( f_pstatedata->dwVague )
        {
            f_pstatedata->dwVague |= DRM_ASD_VAGUE;
        }
    }

    if( f_plicenseinfo->fHasMUExtensibleRestrictions )
    {
        f_pstatedata->dwVague |= DRM_ASD_MU_EXT_REST;
    }

    return fUseState;
}

static DRM_VOID _AggregateStateDataAndStateData(
    IN       DRM_VOID               *pOEMContext,
    IN OUT   DRM_LICENSE_STATE_DATA *f_pstatedata1,
    IN const DRM_LICENSE_STATE_DATA *f_pstatedata2,
    IN       DRM_BOOL                fFirstHasPriority)
{
    /* Aggregated Date variables */
    DRMFILETIME state1BegDate;
    DRMFILETIME state1EndDate;
    DRMFILETIME state2BegDate;
    DRMFILETIME state2EndDate;

    /* If we aggregate successfully, we must aggregate vague */
    DRM_DWORD dwAggregatedVague =
        f_pstatedata1->dwVague | f_pstatedata2->dwVague;

    /*
    ** This variable indicates whether we should we use
    ** f_statedata1 or f_statedata2 when we can't aggregate.
    */
    DRM_BOOL fUseFirst;

    switch( f_pstatedata1->dwCategory )
    {
    case DRM_LICENSE_STATE_NORIGHT:
    case DRM_LICENSE_STATE_UNLIM:
    case DRM_LICENSE_STATE_COUNT:
    case DRM_LICENSE_STATE_FROM:
    case DRM_LICENSE_STATE_UNTIL:
    case DRM_LICENSE_STATE_FROM_UNTIL:
    case DRM_LICENSE_STATE_COUNT_FROM:
    case DRM_LICENSE_STATE_COUNT_UNTIL:
    case DRM_LICENSE_STATE_COUNT_FROM_UNTIL:
    case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE:
        break;
    default:
        /* Unrecognized category, just use the other one */
        fUseFirst = FALSE;
        goto CanNotAggregate;
    }

    switch( f_pstatedata2->dwCategory )
    {
    case DRM_LICENSE_STATE_NORIGHT:
    case DRM_LICENSE_STATE_UNLIM:
    case DRM_LICENSE_STATE_COUNT:
    case DRM_LICENSE_STATE_FROM:
    case DRM_LICENSE_STATE_UNTIL:
    case DRM_LICENSE_STATE_FROM_UNTIL:
    case DRM_LICENSE_STATE_COUNT_FROM:
    case DRM_LICENSE_STATE_COUNT_UNTIL:
    case DRM_LICENSE_STATE_COUNT_FROM_UNTIL:
    case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE:
        break;
    default:
        /* Unrecognized category, just use the other one */
        fUseFirst = TRUE;
        goto CanNotAggregate;
    }

    if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_UNLIM
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_NORIGHT )
    {
        /*
        ** It is already set to unlim or the new aggregation doesn't
        ** provide any new rights.
        ** Aggregation should not be called in this case. But, play safe.
        */
        goto ErrorExit;
    }

    if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_NORIGHT
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_UNLIM )
    {
        /*
        ** In this case the new statedata is always better than the old.
        ** No need to aggregate - just copy.
        */
        MEMCPY( f_pstatedata1, f_pstatedata2, SIZEOF( *f_pstatedata1 ) );
        goto ErrorExit;
    }

    if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_COUNT
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_COUNT )
    {
        if( f_pstatedata1->dwCategory == f_pstatedata2->dwCategory )
        {
            /* If both are counts, aggregate by taking the sum */
            f_pstatedata1->dwCount[0] += f_pstatedata2->dwCount[0];
            f_pstatedata1->dwCount[1] += f_pstatedata2->dwCount[1];

            /* Make sure to aggregate vague */
            goto AggregateVague;
        }
        else
        {
            /* If only one is count, can't aggregate */
            fUseFirst = fFirstHasPriority
                     || (f_pstatedata1->dwCategory == DRM_LICENSE_STATE_COUNT);
            goto CanNotAggregate;
        }
    }

    if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE )
    {
        if( f_pstatedata1->dwCategory == f_pstatedata2->dwCategory )
        {
            /* If both are firstuse, aggregate by taking the max */
            f_pstatedata1->dwCount[0] = max( f_pstatedata1->dwCount[0],
                                             f_pstatedata2->dwCount[1] );

            /* Make sure to aggregate vague */
            goto AggregateVague;
        }
        else
        {
            /* If only one is firstuse, can't aggregate */
            fUseFirst = fFirstHasPriority
                     || (f_pstatedata1->dwCategory == DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE);
            goto CanNotAggregate;
        }
    }

    /*
    ** At this point, all that should be left are:
    ** DRM_LICENSE_STATE_FROM
    ** DRM_LICENSE_STATE_UNTIL
    ** DRM_LICENSE_STATE_FROM_UNTIL
    ** DRM_LICENSE_STATE_COUNT_FROM
    ** DRM_LICENSE_STATE_COUNT_UNTIL
    ** DRM_LICENSE_STATE_COUNT_FROM_UNTIL
    */

    /* Read in dates from the state data. */
    _ReadDates( f_pstatedata1, &state1BegDate, &state1EndDate );
    _ReadDates( f_pstatedata2, &state2BegDate, &state2EndDate );

    if( ( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_FROM
       || f_pstatedata1->dwCategory == DRM_LICENSE_STATE_UNTIL
       || f_pstatedata1->dwCategory == DRM_LICENSE_STATE_FROM_UNTIL )
     && ( f_pstatedata2->dwCategory == DRM_LICENSE_STATE_FROM
       || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_UNTIL
       || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_FROM_UNTIL ) )
    {
        /*
        ** We can aggregate two states that are date-ranges only.
        ** But, only if the date ranges overlap.
        */
        if( _OverlappingDates( &state1BegDate,
                               &state1EndDate,
                               &state2BegDate,
                               &state2EndDate ) )
        {
            /* Aggregate overlapping dates */
            /* Set the BegDate to the min BegDate */
            if( DRM_UTL_DateLessThan( &state2BegDate,
                                      &state1BegDate ) )
            {
                state1BegDate = state2BegDate;
            }
            /* Set the EndDate to the max EndDate */
            if( DRM_UTL_DateLessThan( &state1EndDate,
                                      &state2EndDate ) )
            {
                state1EndDate = state2EndDate;
            }
            _SetExpiryCategory( f_pstatedata1,
                               &state1BegDate,
                               &state1EndDate );

            /* Make sure to aggregate vague */
            goto AggregateVague;
        }
        /* Fall through if they don't overlap */
    }

    /*
    ** At this point, at least one state is:
    ** DRM_LICENSE_STATE_COUNT_FROM
    ** DRM_LICENSE_STATE_COUNT_UNTIL
    ** DRM_LICENSE_STATE_COUNT_FROM_UNTIL
    ** OR we have two expiry states that have non-overlapping date ranges.
    ** These can't be aggregated.
    ** So, we're going to use whichever one is currently usable.
    */
    if( ( _IsDateUsable( pOEMContext, &state1BegDate, &state1EndDate ) && _IsDateUsable( pOEMContext, &state2BegDate, &state2EndDate ) )
     || ( !_IsDateUsable( pOEMContext, &state1BegDate, &state1EndDate ) && !_IsDateUsable( pOEMContext, &state2BegDate, &state2EndDate ) ) )
    {
        /*
        ** Both or neither are usable.
        ** Just use the one that has priority or starts soonest.
        ** In case of a tie, we want to use first.
        ** So, we use !(state2<state1) which is same as (state1<=state2)
        */
        fUseFirst = fFirstHasPriority
                 || !DRM_UTL_DateLessThan( &state2BegDate, &state1BegDate );
    }
    else
    {
        /* Exactly one is usable.  Just use that one. */
        fUseFirst = _IsDateUsable( pOEMContext, &state1BegDate, &state1EndDate );
    }
    /* Either way, we couldn't actually aggregate */
    goto CanNotAggregate;

CanNotAggregate:
    if( !fUseFirst )
    {
        MEMCPY( f_pstatedata1, f_pstatedata2, SIZEOF( *f_pstatedata1 ) );
    }
    f_pstatedata1->dwVague |= DRM_ASD_VAGUE;
    goto ErrorExit;

AggregateVague:
    /* We were able to aggregate - make sure to aggregate vague as well */
    f_pstatedata1->dwVague |= dwAggregatedVague;
    goto ErrorExit;

ErrorExit:
    /* We finished aggregating as best we could */

    /* If any vague is set, make sure the base vague is set too */
    if( f_pstatedata1->dwVague )
    {
        f_pstatedata1->dwVague |= DRM_ASD_VAGUE;
    }

    return;
}

DRMINLINE static DRM_VOID _AggregateStateDataAndStateDataMultiple(
    IN       DRM_VOID               *f_pOEMContext,
    IN OUT   DRM_LICENSE_STATE_DATA *f_rgstatedata1,
    IN const DRM_LICENSE_STATE_DATA *f_rgstatedata2,
    IN       DRM_DWORD               f_cstatedata )
{
    for( ; f_cstatedata > 0; )
    {
        f_cstatedata--;
        _AggregateStateDataAndStateData( f_pOEMContext,
                                         &f_rgstatedata1[f_cstatedata],
                                         &f_rgstatedata2[f_cstatedata],
                                          TRUE );
    }

    return;
}

static DRM_VOID _AggregateStateDataAndStateDataChain(
    IN OUT   DRM_LICENSE_STATE_DATA    *f_pstatedata1,
    IN const DRM_LICENSE_STATE_DATA    *f_pstatedata2 )
{
    DRMFILETIME state1BegDate;       /* Beg Date of aggregated data in f_pstatedata-> */
    DRMFILETIME state1EndDate;       /* End Date of aggregated data in f_pstatedata-> */

    DRMFILETIME state2BegDate;       /* Beg Date of aggregated data in f_pstatedata-> */
    DRMFILETIME state2EndDate;       /* End Date of aggregated data in f_pstatedata-> */

    if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_NORIGHT )
    {
        /* Already know there are no rights.  Just return */
        return;
    }

    if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_UNLIM
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_NORIGHT )
    {
        /* It's easy to tell that state1 has more rights than state2.  Assign
           state1 to the value of state2 and we are done. */
        MEMCPY( f_pstatedata1, f_pstatedata2, SIZEOF( *f_pstatedata2 ) );
        return;
    }

    if( f_pstatedata2->dwCategory == DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE )
    {
        if( f_pstatedata1->dwCategory != DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE )
        {
            /*
            ** There is no way to aggregate this type with other types of licenses
            ** return information on the counted licenses and set to vague
            */
            f_pstatedata1->dwVague   |= DRM_ASD_VAGUE;
        }
        else
        {
            f_pstatedata1->dwCount[0] = min( f_pstatedata1->dwCount[0],
                                             f_pstatedata2->dwCount[0] );
        }

        f_pstatedata1->dwVague |= f_pstatedata2->dwVague & DRM_ASD_OPL;
        f_pstatedata1->dwVague |= f_pstatedata2->dwVague & DRM_ASD_SAP;
        return;
    }

    /* Handle the counts first */
    if( f_pstatedata2->dwCategory == DRM_LICENSE_STATE_COUNT
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_COUNT_FROM
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_COUNT_UNTIL
     || f_pstatedata2->dwCategory == DRM_LICENSE_STATE_COUNT_FROM_UNTIL )
    {
        f_pstatedata1->dwNumCounts = 1;
        if( f_pstatedata1->dwCategory == DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE )
        {
            /*
            ** There is no way to aggregate this type with counted licenses
            ** return information on the counted licenses and set to vague
            */
            f_pstatedata1->dwCount[0] = f_pstatedata2->dwCount[0];
            f_pstatedata1->dwVague   |= DRM_ASD_VAGUE;
        }
        else
        {
            f_pstatedata1->dwCount[0] = min( f_pstatedata1->dwCount[0],
                                             f_pstatedata2->dwCount[0] );
        }
    }

    /* handle the dates */
    _ReadDates( f_pstatedata1, &state1BegDate, &state1EndDate );
    _ReadDates( f_pstatedata2, &state2BegDate, &state2EndDate );

    if( _OverlappingDates( &state1BegDate,
                           &state1EndDate,
                           &state2BegDate,
                           &state2EndDate ) )
    {
        /* Set the BegDate to the max BegDate */
        if( DRM_UTL_DateLessThan( &state1BegDate, &state2BegDate ) )
        {
            state1BegDate = state2BegDate;
        }
        /* Set the EndDate to the min EndDate */
        if( DRM_UTL_DateLessThan( &state2EndDate, &state1EndDate ) )
        {
            state1EndDate = state2EndDate;
        }
    }
    else
    {
        /* no overlapped date, no right */
        f_pstatedata1->dwCategory = DRM_LICENSE_STATE_NORIGHT;
        goto ErrorExit;
    }

    /* Now update the category since we have changed lots of fields */
    if( f_pstatedata1->dwNumCounts > 0 )
    {
        _SetCountedExpiryCategory( f_pstatedata1,
                                  &state1BegDate,
                                  &state1EndDate );
    }
    else
    {
        _SetExpiryCategory( f_pstatedata1,
                           &state1BegDate,
                           &state1EndDate );
    }

    f_pstatedata1->dwVague |= f_pstatedata2->dwVague & DRM_ASD_OPL;
    f_pstatedata1->dwVague |= f_pstatedata2->dwVague & DRM_ASD_SAP;

ErrorExit:
    return;
}

DRMINLINE static DRM_VOID _AggregateStateDataAndStateDataChainMultiple(
    IN OUT   DRM_LICENSE_STATE_DATA    *f_rgstatedata1,
    IN const DRM_LICENSE_STATE_DATA    *f_rgstatedata2,
    IN       DRM_DWORD                  f_cstatedata )
{
    for( ; f_cstatedata > 0; )
    {
        f_cstatedata--;
        _AggregateStateDataAndStateDataChain( &f_rgstatedata1[f_cstatedata],
                                              &f_rgstatedata2[f_cstatedata] );
        if( f_rgstatedata1[f_cstatedata].dwVague != 0 )
        {
            /* If any value in the Vague field is set always set the low bit */
            f_rgstatedata1[f_cstatedata].dwVague |= DRM_ASD_VAGUE;
        }
    }

    return;
}

static DRM_VOID _AggregateStateDataAndLicenseInfo(
    IN       DRM_VOID               *pOEMContext,
    IN OUT   DRM_LICENSE_STATE_DATA *f_pstatedata,
    IN const LicenseInfo            *f_plicenseInfo )
{
    DRM_LICENSE_STATE_DATA    nextstatedata;
    ZEROMEM( &nextstatedata, SIZEOF( DRM_LICENSE_STATE_DATA    ) );
    if( _ConvertLicenseInfoToStateData( f_plicenseInfo,
                                       &nextstatedata ) )
    {
        _AggregateStateDataAndStateData( pOEMContext,
                                         f_pstatedata,
                                        &nextstatedata,
                                         FALSE );
    }

    return;
}

static DRM_VOID _AggregateStateDataAndLicenseInfoMultiple(
    IN       DRM_VOID               *f_pOEMContext,
    IN OUT   DRM_LICENSE_STATE_DATA *f_pstatedata,
    IN const LicenseInfo            *f_plicenseInfo,
    IN       DRM_DWORD               f_cstatedata )
{
    for( ; f_cstatedata > 0; )
    {
        f_cstatedata--;
        _AggregateStateDataAndLicenseInfo( f_pOEMContext,
                                           &f_pstatedata[f_cstatedata],
                                           &f_plicenseInfo[f_cstatedata] );
    }

    return;
}

/*****************************************************************************
** Function :   _IsLicenseExpired
** Synopsis :   Check if the V2 license has expired
** Arguments :
**    f_pdstrLicense: buffer containing the license
**    f_pcontextSST:  Secure store context
** Returns :    TRUE if expired
** Notes :
** Algorithm :
*/
static DRM_BOOL _IsLicenseExpired(
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN       DRM_SECSTORE_CONTEXT    *f_pcontextSST )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRMFILETIME endDate             = {0};
    DRM_BOOL    fExpired            = FALSE;
    DRM_BOOL    fLicenseRequiresSST = TRUE;

    if (  f_pcontextLQ->pLicEval->fLicenseIsXMR )
    {
        ChkDR( DRM_XMR_RequiresSST( f_pcontextLQ->pLicEval->plicenseXMR, &fLicenseRequiresSST ) );
    }

    if ( fLicenseRequiresSST )
    {
        _InitInfDate( &endDate );

        /* retrieve end expiry date.  */
        if( _GetSecStateAttr( f_pcontextSST, &g_dstrDRM_LS_ENDDATE_ATTR, NULL, &endDate ) != DRM_SUCCESS )
        {
            if( f_pcontextLQ->fCurrentLicenseIsXML )
            {
                ( void )DRM_WMDRM_LEVL_ParseV2License( &f_pcontextLQ->pLicEval->dstrContentLicense, &g_dstrDRM_LS_ENDDATE_ATTR, NULL, &endDate );
            }
            else if( XMR_IS_EXPIRATION_VALID(&f_pcontextLQ->licXMR)
                  && f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Expiration.dwEndDate != XMR_UNLIMITED )
            {
                CREATE_FILE_TIME( f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Expiration.dwEndDate, endDate );
            }
        }

        fExpired = _IsExpired(f_pcontextLQ->pBBContext->pOEMContext, &endDate);
    }

ErrorExit:
    return fExpired;
}


/*****************************************************************************
** Function :   _QueryRightForCandoAction
** Synopsis :   try perform all action individually on this license
** Arguments :
** Returns :    DRM_SUCCESS if action is satisfied
**              DRM_S_FALSE if not
** Notes :      Available actions are: "CanCopy", "CanPlay", "CanBurn" and "CanCreateThumbnailImage".
**              f_ppdstrActionUsed still gets set if the license might have
**              allowed the action but is expired, out of counts, etc,
**              but does not get set if the license could never support it.
** Algorithm :
*/
static DRM_RESULT _QueryRightForCandoAction(
    IN const DRM_CONST_STRING        *f_dstrAction,
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN OUT   DRM_DST                 *f_pDatastore,
       OUT   DRM_BOOL                *f_pfUpdateWithPMExpiry,
       OUT   DRM_CONST_STRING const **f_ppdstrActionUsed )
{
    DRM_RESULT dr             = DRM_E_FAIL;
    DRM_BOOL   fCanPerform    = FALSE;
    DRM_BOOL   fActionExisted = FALSE;
    DRM_BOOL   fKeepPrevious  = FALSE;

    ChkArg( f_pcontextLQ != NULL );

    *f_ppdstrActionUsed = NULL;

    /* CanCopy */
    if( DRM_UTL_DSTRStringsEqual( f_dstrAction, &g_dstrWMDRM_RIGHT_COPY ) )
    {
        /*  g_dstrWMDRM_RIGHT_COPY
        */
        dr = DRM_LEVL_PerformOperations( f_pcontextLQ->pLicEval,
                                         DRM_LICENSE_EVAL_ACTION,
                                         DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                        &g_dstrWMDRM_RIGHT_COPY,
                                        &fCanPerform,
                                        &fActionExisted,
                                         f_pDatastore,
                                         NULL,
                                         NULL );
        if( dr == DRM_SUCCESS )
        {
            *f_ppdstrActionUsed = &g_dstrWMDRM_RIGHT_COPY;

            if( fCanPerform )
            {
                /* Success! */
                goto ErrorExit;
            }
            /* Fall through */
        }

        /*
        ** If we succeed for copy, we return copy as the action used
        */
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    /* CanPlay */
    if( DRM_UTL_DSTRStringsEqual( f_dstrAction, &g_dstrWMDRM_RIGHT_PLAYBACK ) )
    {
        /*  g_dstrWMDRM_RIGHT_PLAYBACK
            g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY
        */
        dr = DRM_LEVL_PerformOperations( f_pcontextLQ->pLicEval,
                                         DRM_LICENSE_EVAL_ACTION,
                                         DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                        &g_dstrWMDRM_RIGHT_PLAYBACK,
                                        &fCanPerform,
                                        &fActionExisted,
                                         f_pDatastore,
                                         NULL,
                                         NULL );
        if( dr == DRM_SUCCESS )
        {
            *f_ppdstrActionUsed = &g_dstrWMDRM_RIGHT_PLAYBACK;
            fKeepPrevious = TRUE;
            if( fCanPerform )
            {
                /* Success! */
                goto ErrorExit;
            }
            /* Fall through - Need to try COLLABORATIVE_PLAY */
        }

        dr = DRM_LEVL_PerformOperations( f_pcontextLQ->pLicEval,
                                         DRM_LICENSE_EVAL_ACTION,
                                         DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                        &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY,
                                        &fCanPerform,
                                        &fActionExisted,
                                         f_pDatastore,
                                         NULL,
                                         NULL );
        if( dr == DRM_SUCCESS )
        {
            if( fCanPerform )
            {
                *f_ppdstrActionUsed = &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY;
                dr = DRM_SUCCESS;
                goto ErrorExit;
            }
            else if( !fKeepPrevious )
            {
                *f_ppdstrActionUsed = &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY;
            }
        }

        /*
        ** If we succeed for playback, we return playback as the action used
        ** unless COLLABORATIVE_PLAY succeeds && can be performed.
        ** In other words, if both playback and COLLABORATIVE_PLAY succeed
        ** but either both can be performed or neither can be performed,
        ** then we favor playback.
        ** In all other cases, we favor the one closest to success and can
        ** be performed.
        */
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    /* CanCreateThumbnailImage */
    if ( DRM_THUMBNAIL_IsThumbnailSupported()
      && DRM_UTL_DSTRStringsEqual(f_dstrAction, &g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE ) )
    {
        /*  g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE
        */
        dr = DRM_LEVL_PerformOperations(f_pcontextLQ->pLicEval,
                                        DRM_LICENSE_EVAL_ACTION,
                                        DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                        &g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE,
                                        &fCanPerform,
                                        &fActionExisted,
                                        f_pDatastore,
                                        NULL,
                                        NULL);
        if ( dr == DRM_SUCCESS )
        {

            //  'Create Thumbnail Image' right is an implied right which maps to Play right when using licenses.
            //  Switch the Action string if CreateThumbnailImage detected

            *f_ppdstrActionUsed = &g_dstrWMDRM_RIGHT_PLAYBACK;

            if( fCanPerform )
            {
                /* Success! */
                goto ErrorExit;
            }
        }

        dr = DRM_S_FALSE;

        goto ErrorExit;
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function :   _UseRightForCandoAction
** Synopsis :   Try to use the given right for action
** Arguments :
** Returns :    DRM_SUCCESS if right can be used for action
**              DRM_S_FALSE otherwise
** Notes :
** Algorithm :
*/
static DRM_RESULT _UseRightForCandoAction(
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN const DRM_CONST_STRING        *f_dstrAction,
    IN const DRM_CONST_STRING        *f_pdstrRightUsed,
    IN       DRM_BOOL                 f_fUpdateWithPMExpiry,
    IN       DRMFILETIME             *f_pft,
       OUT   DRM_DWORD               *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrDummy         = EMPTY_DRM_STRING;
    DRM_BOOL         fLicReqMetering   = FALSE;
    DRM_BOOL         fLicTimeBased     = FALSE;
    DRM_DWORD        dwLicMinReqAppSec = 0;
    LicenseInfo      licInfo           = { LIC_TYPE_IGNORE, 0 };

    /* The right is allowed. Let us now check if the license can be used on this machine. */
    if( _GetV2LicenseInfo( f_pcontextLQ,
                          &f_pcontextLQ->pLicEval->dstrContentLicense,
                          &licInfo,
                           f_pdstrRightUsed,
                           f_pcontextLQ->pbLIDSecStore) == DRM_SUCCESS )
    {
        if( f_pcontextLQ->fCurrentLicenseIsXML )
        {
            /* XMR license does not have SAP */
            if( DRM_WMDRM_LEVL_HasSAP( &f_pcontextLQ->pLicEval->dstrContentLicense ) )
            {
                licInfo.fRequiresSAP = TRUE;
            }
        }

        switch (licInfo.dwCategory)
        {
            case LIC_TYPE_EXPIRY:
                if( _IsExpired(f_pcontextLQ->pBBContext->pOEMContext, &licInfo.endDate) )     /* endDate > now ? */
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_EXPIRED;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }
                if( !_IsExpired(f_pcontextLQ->pBBContext->pOEMContext, &licInfo.begDate) )     /* now < begDate ? */
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_NOT_STARTED;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }
                break;

            case LIC_TYPE_IGNORE:
                if( _IsExpired(f_pcontextLQ->pBBContext->pOEMContext, &licInfo.endDate) )     /* endDate > now ? */
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_EXPIRED;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }
                else if( licInfo.dwCount == 0 )
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_EXHAUSTED;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }
                break;

            default:
                break;
        }
    }
    else
    {
        /* If we couldn't get the License Info, then just set NO_RIGHT */
        *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_NO_RIGHT;
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    if( DRM_COPYOPL_IsCopyOplSupported() )
    {
        /* get OPL restriction */
        if( f_pcontextLQ->fCurrentLicenseIsXML )
        {
            dr = DRM_WMDRM_LIC_GetCopyOPLRestriction(  f_pcontextLQ,
                                                       f_dstrAction,
                                                       f_pdstrRightUsed,
                                                      &licInfo.fHasOPLs,
                                                       f_pdwResult );
            if( dr != DRM_SUCCESS )
            {
                dr = DRM_S_FALSE;
                goto ErrorExit;
            }
        }
        else
        {
            if( DRM_UTL_DSTRStringsEqual( f_dstrAction, &g_dstrWMDRM_RIGHT_COPY ) )
            {
                if( XMR_IS_COPYPROTECTIONLEVEL_VALID( &f_pcontextLQ->licXMR ) )
                {
                    licInfo.fHasOPLs = TRUE;
                    dr = DRM_COPYOPL_ASD_ExamineOutputLevelsXMR( f_pcontextLQ, f_pdwResult );
                    if( dr != DRM_SUCCESS )
                    {
                        dr = DRM_S_FALSE;
                        goto ErrorExit;
                    }
                }
            }
            else if( DRM_UTL_DSTRStringsEqual( f_dstrAction, &g_dstrWMDRM_RIGHT_PLAYBACK ) )
            {
                if( XMR_IS_OPL_VALID( &f_pcontextLQ->licXMR ) )
                {
                    licInfo.fHasOPLs = TRUE;
                }
            }
        }
    }

    DRMASSERT( dr == DRM_SUCCESS );

    fLicTimeBased = !_InfDate( &licInfo.endDate );

    if( f_pcontextLQ->fCurrentLicenseIsXML )
    {
        /*
        ** get metering info
        */
        fLicReqMetering = DRM_WMDRM_LIC_GetAttribute( &f_pcontextLQ->pLicEval->dstrContentLicense,
                                                       NULL,
                                                       DRM_LICENSE_ATTRIB_METERING_ID,
                                                       NULL,
                                                      &dstrDummy,
                                                       0 ) == DRM_SUCCESS;

        /*
        ** get min appsec required
        */
        DRM_WMDRM_LEVL_ParseV2License( &f_pcontextLQ->pLicEval->dstrContentLicense,
                                       &g_dstrDRM_LS_APPSEC_ATTR,
                                       &dwLicMinReqAppSec,
                                        NULL );

    }
    else
    {
        /*
        ** get metering info
        */
        if( XMR_IS_METERING_VALID( &f_pcontextLQ->licXMR ) )
        {
            fLicReqMetering = f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.Metering.fValid;
        }

        /*
        ** get min appsec required
        */
        if( XMR_IS_MINIMUM_ENVIRONMENT_VALID( &f_pcontextLQ->licXMR ) )
        {
            dwLicMinReqAppSec = f_pcontextLQ->licXMR.containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel;
        }
    }

    /* CanCopy */
    if( DRM_UTL_DSTRStringsEqual( f_dstrAction, &g_dstrWMDRM_RIGHT_COPY ) )
    {
        if( DRM_UTL_DSTRStringsEqual( f_pdstrRightUsed, &g_dstrWMDRM_RIGHT_COPY ) )
        {
            if( f_pcontextLQ->fIsWMDRMPDDevice )   /* is WMDRM-PD device */
            {
                /* check appsec */
                if( f_pcontextLQ->cacheDevCert.appSec < dwLicMinReqAppSec )
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_APPSEC_TOO_LOW;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }

                /* check WMDRM-PD feature in devcert */
                if( fLicReqMetering
                 && (f_pcontextLQ->cacheDevCert.dwFeatureFlags & DRM_FEATURE_METERING) == 0 )
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_NO_METERING_SUPPORT;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }

                if( fLicTimeBased
                 && (f_pcontextLQ->cacheDevCert.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK) == 0
                 && (f_pcontextLQ->cacheDevCert.dwFeatureFlags & DRM_FEATURE_ANTI_ROLLBACK_CLOCK) == 0 )
                {
                    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_NO_CLOCK_SUPPORT;
                    dr = DRM_S_FALSE;
                    goto ErrorExit;
                }
            }

            dr = DRM_SUCCESS;
        }
        else
        {
            dr = DRM_S_FALSE;
        }
        goto ErrorExit;
    }

    /* CanPlay */
    if( DRM_UTL_DSTRStringsEqual( f_dstrAction, &g_dstrWMDRM_RIGHT_PLAYBACK ) )
    {
        if( DRM_UTL_DSTRStringsEqual(f_pdstrRightUsed, &g_dstrWMDRM_RIGHT_PLAYBACK)
         || DRM_UTL_DSTRStringsEqual(f_pdstrRightUsed, &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY) )
        {
            /* check environment variables */
            if( fLicReqMetering  &&  !f_pcontextLQ->fPCSupportMetering )
            {
                *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_NO_METERING_SUPPORT;
                dr = DRM_S_FALSE;
                goto ErrorExit;
            }

            dr = DRM_SUCCESS;
        }

        goto ErrorExit;
    }

ErrorExit:
    return dr;
}


static DRM_VOID _AddLicResultCache(
    IN OUT       DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN           DRM_LID                 *f_pLID,
    IN           DRM_BOOL                 f_fHasParentLink,
    IN           DRM_KID                 *f_pparentKID,
    IN     const DRM_DWORD               *f_rgLicResult,
    IN           DRM_DWORD                f_dwUplinkVector,
    IN           LicenseInfo             *f_rgLicInfo )
{
    DRM_DWORD iAction = DRM_MAX_RIGHTS_SUPPORTED;

    if( f_pcontextLQ->cCacheSlots == 0 )
    {
        return;
    }

    MEMCPY(&(f_pcontextLQ->rgCacheSlots[f_pcontextLQ->iCurrSlot].cachedLID), f_pLID, SIZEOF(DRM_LID));
    if( f_fHasParentLink )
    {
        MEMCPY(&(f_pcontextLQ->rgCacheSlots[f_pcontextLQ->iCurrSlot].parentKID), f_pparentKID, SIZEOF(DRM_KID));
    }
    else
    {
        MEMSET(&(f_pcontextLQ->rgCacheSlots[f_pcontextLQ->iCurrSlot].parentKID), 0, SIZEOF(DRM_KID));
    }

    f_pcontextLQ->rgCacheSlots[f_pcontextLQ->iCurrSlot].dwUplinkVector = f_dwUplinkVector;

    if (iAction > 0)
    {
        do  /* This loop iterates backwards from (DRM_MAX_RIGHTS_SUPPORTED-1) through 0. */
        {
            iAction--;

            if( f_rgLicResult != NULL )
            {
                f_pcontextLQ->rgCacheSlots[f_pcontextLQ->iCurrSlot].dwResult[iAction] = f_rgLicResult[iAction];
            }
            if( f_rgLicInfo != NULL )
            {
                f_pcontextLQ->rgCacheSlots[f_pcontextLQ->iCurrSlot].licInfo[iAction]  = f_rgLicInfo[iAction];
            }
        } while (iAction > 0);
    }

    f_pcontextLQ->iCurrSlot = (f_pcontextLQ->iCurrSlot + 1) % f_pcontextLQ->cCacheSlots;
}


static DRM_BOOL _GetLicResultCache(
    IN OUT DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN     DRM_LID                 *f_pLID,
       OUT DRM_BOOL                *f_pfHasParentLink,
       OUT DRM_KID                 *f_pparentKID,
       OUT DRM_DWORD               *f_rgLicResult,
       OUT DRM_DWORD               *f_pdwUplinkVector,
       OUT LicenseInfo             *f_rgLicInfo )
{
    DRM_DWORD iSlot   = 0;
    DRM_DWORD iAction = 0;
    DRM_BOOL  fResult = FALSE;

    for (iSlot=0; iSlot<f_pcontextLQ->cCacheSlots; iSlot++)
    {
        if( MEMCMP(&(f_pcontextLQ->rgCacheSlots[iSlot].cachedLID), f_pLID, SIZEOF(DRM_LID)) == 0 )
        {
            /* copy cached result */
            if( f_pdwUplinkVector != NULL )
            {
                *f_pdwUplinkVector = f_pcontextLQ->rgCacheSlots[iSlot].dwUplinkVector;
            }

            iAction = DRM_MAX_RIGHTS_SUPPORTED;
            if (iAction > 0)
            {
                do  /* This loop iterates backwards from (DRM_MAX_RIGHTS_SUPPORTED-1) through 0. */
                {
                    iAction--;
                    if( f_rgLicResult != NULL )
                    {
                        f_rgLicResult[iAction] = f_pcontextLQ->rgCacheSlots[iSlot].dwResult[iAction];
                    }
                    if( f_rgLicInfo != NULL )
                    {
                        f_rgLicInfo[iAction]   = f_pcontextLQ->rgCacheSlots[iSlot].licInfo[iAction];
                    }
                } while (iAction > 0);
            }

            /* copy cached parentKID */
            for ( iAction = 0; iAction < SIZEOF(DRM_KID); iAction++ )
            {
                if( f_pcontextLQ->rgCacheSlots[iSlot].parentKID.rgb[iAction] != 0 )
                {
                    break;
                }
            }
            if( iAction == SIZEOF(DRM_KID) )   /* the cached parentKID is NULL */
            {
                *f_pfHasParentLink = FALSE;
            }
            else
            {
                MEMCPY(f_pparentKID, &(f_pcontextLQ->rgCacheSlots[iSlot].parentKID), SIZEOF(DRM_KID));
            }

            fResult = TRUE;
            break;
        }
    }

    return fResult;
}



/*****************************************************************************
** Function :   _LoadNextLicense
** Synopsis :   Load next license to memory
** Arguments :
** Returns :    DRM_SUCCESS if next license can be loaded
**              DRM_S_FALSE otherwise
** Notes :
** Algorithm :
*/
static DRM_RESULT _LoadNextLicense(
    IN OUT DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN OUT DRM_DST                 *f_pDatastore,
    IN     DRM_DWORD                f_dwChainDepth,
       OUT DRM_BOOL                *f_pFromCache,
       OUT DRM_LID                 *f_pLID,
       OUT DRM_BOOL                *f_pfHasParentLink,
       OUT DRM_KID                 *f_pparentKID,
       OUT DRM_DWORD               *f_rgLicResult,
       OUT DRM_DWORD               *f_pdwUplinkVector,
       OUT LicenseInfo             *f_rgLiceInfo )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  cbSize = f_pcontextLQ->cbBuffer;
    DRM_KID    kid;
    DRM_BYTE   rgbPassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_CONST_STRING dstrUplinkKID = EMPTY_DRM_STRING;
    DRM_DST_SLOT_HINT slotHint = {0};

    /* get next license LID */
    ChkDR(DRM_LST_EnumNext(f_pcontextLQ->rgpLicQueryContext[f_dwChainDepth], &kid, f_pLID, &slotHint, &cbSize));

    /* try load licinfo of "f_pLID" from cache if not LEAF license */
    if( f_dwChainDepth > 0 )
    {
        if( _GetLicResultCache(f_pcontextLQ, f_pLID, f_pfHasParentLink, f_pparentKID, f_rgLicResult, f_pdwUplinkVector, f_rgLiceInfo) )
        {
            *f_pFromCache = TRUE;
            goto ErrorExit;
        }
    }

    /*
    ** Reset the stack by setting the stack top to 0, effectively clear everything on the stack.
    **
    ** Waning: Any existing data on the stack will be lost due to the reset of the stack stop.
    **         The caller should make sure that any existing data on the stack can be safely
    **         discarded.
    */
    f_pcontextLQ->stack.nStackTop = 0;
    f_pcontextLQ->pbCurrentLicenseBuff = NULL;

    ChkDR(DRM_STK_Alloc( &f_pcontextLQ->stack, cbSize, (DRM_VOID **)&f_pcontextLQ->pbCurrentLicenseBuff));


    /* load next license in memory */
    ChkDR(DRM_LST_GetLicense(f_pcontextLQ->pActiveLicStore, &kid, f_pLID, &slotHint, f_pcontextLQ->pbCurrentLicenseBuff, &cbSize));

    if( f_pcontextLQ->fCurrentLicenseIsXML )
    {
        /* setup DSTR for XML license */
        DSTR_FROM_PB(&f_pcontextLQ->pLicEval->dstrContentLicense, f_pcontextLQ->pbCurrentLicenseBuff, cbSize);
    }
    else
    {
        /*
        ** crack the XMR license
        */
        ChkDR( DRM_XMR_UnpackLicense( f_pcontextLQ->pbCurrentLicenseBuff,
                                      cbSize,
                                     &f_pcontextLQ->stack,
                                     &f_pcontextLQ->licXMR ) );
    }

    f_pcontextLQ->pLicEval->fUseCachedAttribs = FALSE;
    MEMCPY( f_pcontextLQ->pLicEval->LID.rgb, f_pLID->rgb, SIZEOF(f_pLID->rgb) );

    /* Talk to blackbox to get the secure store password for this license */

    ChkDR(DRM_SST_CreateLicenseStatePassword(f_pLID, rgbPassword, f_pcontextLQ->pBBContext));
    dr = DRM_SST_OpenKeyTokens( f_pcontextLQ->pbLIDSecStore,
                               &f_pcontextLQ->pLicEval->LID,
                                NULL,
                                rgbPassword,
                                0,
                                SECURE_STORE_LICENSE_DATA,
                                f_pDatastore );

    /* get parent uplink KID, if any */
    *f_pfHasParentLink = FALSE;
    if( f_pcontextLQ->fCurrentLicenseIsXML )
    {
        if( DRM_WMDRM_LIC_GetAttribute( &f_pcontextLQ->pLicEval->dstrContentLicense,
                                         NULL,
                                         DRM_LICENSE_ATTRIB_CHAINEDKID,
                                         NULL,
                                        &dstrUplinkKID,
                                         0) == DRM_SUCCESS )
        {
            *f_pfHasParentLink = TRUE;
            DRM_UTL_DecodeKID( &dstrUplinkKID, f_pparentKID );
        }
    }
    else
    {
        if( XMR_IS_UPLINK_KID_VALID( &f_pcontextLQ->licXMR ) )
        {
            *f_pfHasParentLink = TRUE;
            DRM_BYT_CopyBytes( f_pparentKID->rgb,
                               0,
                               f_pcontextLQ->licXMR.containerOuter.containerKeys.UplinkKid.pbguidUplinkKID,
                               f_pcontextLQ->licXMR.containerOuter.containerKeys.UplinkKid.iguidUplinkKID,
                               SIZEOF( f_pparentKID->rgb ) );
        }
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function :   _IsLicenseSelectable
** Synopsis :   Check if the license is selectable / valid
** Arguments :
** Returns :    DRM_SUCCESS if license can be selected
**              DRM_S_FALSE otherwise
** Notes :
** Algorithm :
*/
static DRM_RESULT _IsLicenseSelectable(
    IN OUT DRM_LICSTOREENUM_CONTEXT *f_pcontextLIC,
    IN OUT DRM_VIEW_RIGHTS_CONTEXT  *f_pcontextLQ,
    IN OUT DRM_DST                  *f_pDatastore,
    IN     DRM_BOOL                  f_fDeleteExpiredLicenses,
       OUT DRM_BOOL                 *f_pLicenseDeleted )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BOOL   fSelect = FALSE;

    f_pcontextLQ->pLicEval->pcontextSSTLicense      = f_pcontextLQ->pbLIDSecStore;
    f_pcontextLQ->pLicEval->pcontextSSTGlobal       = f_pcontextLQ->pbGlobalSecStore;
    f_pcontextLQ->pLicEval->fGlobalSecStoreWritable = FALSE;
    f_pcontextLQ->pLicEval->dwFlags                 = LICEVAL_VERIFY_IGNORE_VERIFICATION;
    f_pcontextLQ->pLicEval->fReserved               = 0;
    DRM_LEVL_PerformOperations( f_pcontextLQ->pLicEval,
                                DRM_LICENSE_EVAL_SELECT,
                                DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                NULL,
                               &fSelect,
                                NULL,
                                f_pDatastore,
                                NULL,
                                NULL );

    if( fSelect )
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    /* skip this license if cannot be selected, expired, etc */
    if( f_fDeleteExpiredLicenses )
    {
        /*
        **  We need to make sure that the reason for failure is set correctly.
        **  This is a workaround to compensate for the fact that v9
        **  licenses do not set the reason correctly sometimes.
        */
        if( f_pcontextLQ->pLicEval->lReasonForFail == 0 )
        {
            /* No reason given by license. Try parsing to get the reason, if we can. */
            (void) DRM_WMDRM_LEVL_GetLicenseReasonForUnusable( f_pcontextLQ->pLicEval,
                                                              &f_pcontextLQ->pLicEval->lReasonForFail );
        }

        if( f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_EXPIRED
          || f_pcontextLQ->pLicEval->fDeleteLicense )
        {
            /* License is expired. Let's delete it. */
            (void) DRM_LST_EnumDeleteAndSaveDate( f_pcontextLIC, DRM_DELETEDSTORE_IsDeletedStoreSupported() );
            *f_pLicenseDeleted = TRUE;
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }
    }

    if( f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_EXPIRED
     || f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_NOSAP
     || f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_NOSVP
     || f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_NOWDM
     || f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_NEED_UPGRADE_MSSAP
     || f_pcontextLQ->pLicEval->lReasonForFail == LR_LICENSE_INVALID_APPLICATION )
    {
        dr = DRM_S_FALSE;
    }

ErrorExit:
    return dr;
}


/* return FALSE if the license cannot be used */
static DRM_BOOL _EvalCurrLicforCandoActions(
    IN OUT                          DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN OUT                          DRM_DST                 *f_pDatastore,
    IN                              DRM_DWORD                f_cActions,
    __in_ecount(f_cActions)   const DRM_CONST_STRING        *f_rgpdstrAction[],
    IN                              DRM_BOOL                 f_fHasParentLink,
    __in_ecount(f_cActions)   const DRM_DWORD                f_rgAggrResults[],
    __out_ecount(f_cActions)        DRM_DWORD                f_rgLicResult[],
       OUT                          DRM_DWORD               *f_pdwUpSearchVector )
{
          DRM_RESULT        dr                  = DRM_SUCCESS;
          DRM_BOOL          fCanDo              = FALSE;
          DRM_DWORD         iAction             = f_cActions;
    const DRM_CONST_STRING *pdstrRightUsed      = NULL;
          DRM_BOOL          fUpdateWithPMExpiry = FALSE;

    *f_pdwUpSearchVector = 0;

    /* get right for each action */
    if (iAction > 0)
    {
        do  /* This loop iterates backwards from (f_cActions-1) through 0. */
        {
            DRM_BOOL    fNoRight  = FALSE;
            DRMFILETIME ft;

            iAction--;

            /* this action already has right, skip to next */
            if( f_rgAggrResults[iAction] == DRM_LICQUERY_SATISFIED )
            {
                continue;
            }

            /* see if we have right for this action requested */
            dr = _QueryRightForCandoAction( f_rgpdstrAction[iAction],
                                            f_pcontextLQ,
                                            f_pDatastore,
                                           &fUpdateWithPMExpiry,
                                           &pdstrRightUsed );
            if( DRM_FAILED( dr ) || ( pdstrRightUsed == NULL ) )
            {
                f_rgLicResult[iAction] |= DRM_LICQUERY_NOT_ENABLED_NO_RIGHT;
                continue;   /* no possible right for action, try next */
            }
            /*
            ** If _QueryRightForCandoAction returns DRM_S_FALSE and still
            ** sets pdstrRightUsed, then the license might have
            ** allowed the action but is expired, out of counts, etc.
            ** There's no way we're going to return DRM_LICQUERY_SATISFIED
            ** in this case, but we're hoping to return something more detailed
            ** than just DRM_LICQUERY_NOT_ENABLED_NO_RIGHT.
            ** So, set fNoRight to indicate that we should fall back to
            ** DRM_LICQUERY_NOT_ENABLED_NO_RIGHT even if everything else succeeds.
            */
            if( dr == DRM_S_FALSE )
            {
                fNoRight = TRUE;
            }

            /*
            ** The right is or was allowed.
            ** Let us now check if the license can be used on this machine.
            */
            dr = _UseRightForCandoAction( f_pcontextLQ,
                                          f_rgpdstrAction[iAction],
                                          pdstrRightUsed,
                                          fUpdateWithPMExpiry,
                                         &ft,
                                         &f_rgLicResult[iAction] );
            if( dr != DRM_SUCCESS)
            {
                continue;
            }

            if( fNoRight )
            {
                /*
                ** Well, _UseRightForCandoAction didn't give us better info
                ** about why we couldn't use the right, so fall back to
                ** DRM_LICQUERY_NOT_ENABLED_NO_RIGHT.
                */
                f_rgLicResult[iAction] |= DRM_LICQUERY_NOT_ENABLED_NO_RIGHT;
                continue;
            }

            /* everything is fine. Check if this is end of chain */
            f_rgLicResult[iAction] = DRM_LICQUERY_SATISFIED;
            if( f_fHasParentLink )
            {
                *f_pdwUpSearchVector |= (1 << iAction);  /* include this for uplink (parent) search */
            }

            fCanDo = TRUE;
        }while (iAction > 0);
    }
    return fCanDo;
}


/*****************************************************************************
** Function :   _QueryRightForCandoActions
** Synopsis :   Find V2/V3 license for requested actions
**              - all licenses for given KID are scanned, or
**              - when all actions are satisfied.
** Arguments :
** Returns :    DRM_SUCCESS if no error, otherwise, specific error code is returned
** Notes :
** Algorithm :
**    FindRightForActions(Env,KID,cAction,rgActions[],rgResults[],rgfInclude[],dwChainDepth)
**    {
**        DRM_BOOL _rgfInclude[];
**        For each license of KID
**        {
**            if  !( license is selectable ) || !( license is not expired ) then
**                continue;
**
**            parentKID = GetParentLink();
**            For (action i to N)
**            {
**                if( rgResults[i].Satisfied ) or ( !rgfInclude[i] ) then
**                    contimue;
**
**                RightUsed = GetRightForAction()
**                if( RightUsed == NULL ) then
**                    continue;
**
**                use_right(rgAction[i], RightUsed);
**
**                if( parentKID == NULL )
**                    rgResults[i].Satisfied = TRUE;
**                else
**                    _rgfInclude[i] = TRUE;
**            }
**
**            if(_rgfInclude[] not empty) and (parentKID != NULL) then
**            {
**                // call recursively for parent license
**                FindRightForActions(Env,parentKID,cAction,rgActions[],rgResults[],_rgfInclude[i],dwChainDepth+1);
**            }
**            if(all actions are satisfied) then
**                break;
**        }
**    }
*/
static DRM_RESULT _QueryRightForCandoActions(
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN OUT   DRM_DST                 *f_pDatastore,
    IN       DRM_BOOL                 f_fDeleteExpiredLicenses,
    IN       DRM_KID                 *f_pKID,
    IN       DRM_DWORD                f_dwSearchVector,
    IN       DRM_DWORD                f_dwChainDepth,
    IN       DRM_DWORD                f_cActions,
    IN const DRM_CONST_STRING        *f_rgpdstrAction[],
       OUT   DRM_BOOL                *f_pfLicFound,
       OUT   DRM_BOOL                *f_pfRecentExpired,
       OUT   DRM_DWORD                f_rgAggrResults[] )
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_DWORD   dwUpSearchVector    = 0;
    DRM_DWORD   iAction             = 0;
    DRM_BOOL    fLicenseDeleted     = FALSE;
    DRM_BOOL    fAllDone            = FALSE;
    DRM_BOOL    fHasParentLink      = FALSE;
    DRM_BOOL    fParentLinkExists   = FALSE;
    DRM_BOOL    fFromCache          = FALSE;
    DRM_BOOL    fHasRight           = FALSE;
    DRM_KID     kidParent           = {0};
    DRM_LID     lid                 = {0};
    DRM_DWORD   rgLicResult[DRM_MAX_RIGHTS_SUPPORTED];

    ChkArg( f_pfLicFound      != NULL
         && f_pfRecentExpired != NULL
         && f_cActions        <= DRM_MAX_RIGHTS_SUPPORTED
         && f_cActions         > 0);

    /* Terminating condition: check chaining depth */
    if( f_dwChainDepth >= f_pcontextLQ->dwLicChainDepth  ||  f_dwChainDepth >= DRM_MAX_LICENSE_CHAIN_DEPTH )
    {
        iAction = f_cActions;
        do   /* This loop iterates backwards from (f_cActions-1) through 0. */
        {
            iAction--;
            if( (f_dwSearchVector & (1 << iAction)) != 0 )
            {
                f_rgAggrResults[iAction] |= DRM_LICQUERY_NOT_ENABLED_CHAIN_DEPTH_TOO_HIGH;
            }
        } while (iAction > 0);
        goto ErrorExit;   /* exceeding max license chain depth, we are done */
    }

    /* Check for recently expired non-leaf licenses */
    if( f_dwChainDepth > 0 )
    {
        ChkDR( DRM_DELETEDSTORE_CheckRecentExpired(
            f_pcontextLQ->pBBContext->pOEMContext,
            f_pDatastore,
            f_pKID,
            f_pfRecentExpired ) );
        /*
        ** A recently expired uplink license counts as "finding" a license
        ** since we want to return "expired" but not "no uplink"
        */
        *f_pfLicFound = *f_pfLicFound || *f_pfRecentExpired;
    }

    /* init enum license for given KID */
    ChkDR(DRM_LST_InitEnum(f_pcontextLQ->pActiveLicStore, f_pKID, TRUE, f_pcontextLQ->rgpLicQueryContext[f_dwChainDepth]));
    while (!fAllDone)  /* try each license equals to KID until all done */
    {
        /* init temp result and cache LID */
        iAction = f_cActions;
        do   /* This loop iterates backwards from (f_cActions-1) through 0. */
        {
            iAction--;
            rgLicResult[iAction] = DRM_LICQUERY_NOT_ENABLED;
        } while (iAction > 0);

        dr = _LoadNextLicense(f_pcontextLQ, f_pDatastore, f_dwChainDepth, &fFromCache, &lid, &fHasParentLink, &kidParent, rgLicResult, &dwUpSearchVector, NULL);
        if( dr != DRM_SUCCESS )
        {
            /* No more licenses - we're done. */
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }

        /* Found a license */
        *f_pfLicFound = TRUE;

        if( !fFromCache )  /* no cached value found */
        {
            /* Is the license selectable or expired? */
            dr = _IsLicenseSelectable(f_pcontextLQ->rgpLicQueryContext[f_dwChainDepth],f_pcontextLQ,f_pDatastore,f_fDeleteExpiredLicenses,&fLicenseDeleted);
            if( dr != DRM_SUCCESS
             || fLicenseDeleted
             || _IsLicenseExpired(f_pcontextLQ, f_pcontextLQ->pbLIDSecStore) )
            {
                iAction = f_cActions;
                do   /* This loop iterates backwards from (f_cActions-1) through 0. */
                {
                    iAction--;
                    if( (f_dwSearchVector & (1 << iAction)) != 0 )
                    {
                        f_rgAggrResults[iAction] |= DRM_LICQUERY_NOT_ENABLED_EXPIRED;
                    }
                } while (iAction > 0);
                continue;
            }

            /* get right for each action from current license */
            fHasRight = _EvalCurrLicforCandoActions( f_pcontextLQ,
                                                     f_pDatastore,
                                                     f_cActions,
                                                     f_rgpdstrAction,
                                                     fHasParentLink,     /* TRUE if parent exists */
                                                     f_rgAggrResults,
                                                     rgLicResult,
                                                    &dwUpSearchVector ); /* bit vector for corresponding action to search for parent link */

            /* cache LID and parentKID */
            if( fHasRight  &&  f_dwChainDepth > 0 )
            {
                /* add rgLicResult and rgLicInfo to cache */
                _AddLicResultCache(f_pcontextLQ, &lid, fHasParentLink, &kidParent, rgLicResult, dwUpSearchVector, NULL);
            }
        }

        /* aggregate the result from license */
        iAction = f_cActions;
        do   /* This loop iterates backwards from (f_cActions-1) through 0. */
        {
            iAction--;
            if( rgLicResult[iAction] == DRM_LICQUERY_SATISFIED  )
            {
                if( !fHasParentLink                                /* fHasParentLink == FALSE if this is the end of the chain */
                 && ( (f_dwSearchVector & (1 << iAction)) != 0 ) ) /* this bit is 'ON' when the leaf has right for action */
                {
                    /* only set this to satisfied when this license is end of the chain
                       and the leaf license has the right for action */
                    f_rgAggrResults[iAction] = DRM_LICQUERY_SATISFIED;
                }
            }
            else if( f_rgAggrResults[iAction] != DRM_LICQUERY_SATISFIED )
            {
                f_rgAggrResults[iAction] |= rgLicResult[iAction];
            }
        } while (iAction > 0);

        if( !fHasRight )
        {
            continue;
        }

        if( dwUpSearchVector != 0  &&  fHasParentLink )
        {
            /* call recursively for parent link search */
            _QueryRightForCandoActions( f_pcontextLQ,
                                        f_pDatastore,
                                        FALSE,  /* do not perform delete on parent link */
                                       &kidParent,
                                        dwUpSearchVector,      /* bit vector for corresponding action to search for parent link */
                                        f_dwChainDepth+1,
                                        f_cActions,
                                        f_rgpdstrAction,
                                       &fParentLinkExists,
                                        f_pfRecentExpired,
                                        f_rgAggrResults );

            if( !fParentLinkExists )
            {
                iAction = f_cActions;
                do   /* This loop iterates backwards from (f_cActions-1) through 0. */
                {
                    iAction--;
                    if( f_rgAggrResults[iAction] != DRM_LICQUERY_SATISFIED )
                    {
                        f_rgAggrResults[iAction] |= DRM_LICQUERY_NOT_ENABLED_NO_UPLINK;
                    }
                } while (iAction > 0);
            }
        }

        dwUpSearchVector = 0;   /* reset for next round of parent search */

        /* check if we are done */
        fAllDone = TRUE;
        iAction = f_cActions;
        do   /* This loop iterates backwards from (f_cActions-1) through 0. */
        {
            iAction--;
            if( f_rgAggrResults[iAction] != DRM_LICQUERY_SATISFIED )
            {
                fAllDone = FALSE;
                break;
            }
        } while (iAction > 0);
    }

ErrorExit:
    return dr;
}



static DRM_RESULT _QueryRightForAction(
    IN const DRM_CONST_STRING        *f_dstrAction,
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN OUT   DRM_DST                 *f_pDatastore,
    IN       DRMPFNPOLICYCALLBACK     f_pfnPolicyCallback,
    IN const DRM_VOID                *f_pvCallbackParameter,
       OUT   DRM_BOOL                *f_pfUpdateWithPMExpiry,
       OUT   DRM_CONST_STRING const **f_ppdstrActionUsed )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL   fCanPerform    = FALSE;
    DRM_BOOL   fActionExisted = FALSE;

    *f_ppdstrActionUsed = f_dstrAction;

#ifndef PDDRM_COPYOPL
#define PDDRM_COPYOPL 300
#endif /* PDDRM_COPYOPL */

    dr = DRM_LEVL_PerformOperations( f_pcontextLQ->pLicEval,
                                     DRM_LICENSE_EVAL_ACTION,
                                     DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                     f_dstrAction,
                                    &fCanPerform,
                                    &fActionExisted,
                                     f_pDatastore,
                                     f_pfnPolicyCallback,
                                     f_pvCallbackParameter );
    if( dr == DRM_SUCCESS && fCanPerform )
    {
        goto ErrorExit;
    }

ErrorExit:

    return fCanPerform? DRM_SUCCESS : DRM_S_FALSE;
}

/* return FALSE if the license cannot be used */
static DRM_BOOL _CollectLicStateforRequestedActions(
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN OUT   DRM_DST                 *f_pDatastore,
    IN       DRM_DWORD                f_cActions,
    IN const DRM_CONST_STRING* const  f_rgpdstrAction[],
    IN       DRM_BOOL                 f_fHasParentLink,
       OUT   LicenseInfo              f_rgLicinfo[],
    IN       DRMPFNPOLICYCALLBACK     f_pfnPolicyCallback,
    IN const DRM_VOID                *f_pvCallbackParameter)
{
    DRM_DWORD               iAction             = f_cActions;
    DRM_BOOL                fHasRights          = FALSE;
    const DRM_CONST_STRING *pdstrRightUsed      = NULL;
    DRM_XMR_UNKNOWN_OBJECT *pExtRestrictLoop    = NULL;
    DRM_WORD                wRightID            = XMR_OBJECT_TYPE_INVALID;
    DRM_BOOL                fUpdateWithPMExpiry = FALSE;

    /* get right for each action */
    if (iAction > 0)
    {
        do  /* This loop iterates backwards from (f_cActions-1) through 0. */
        {
            iAction--;
            ZEROMEM( &f_rgLicinfo[iAction], SIZEOF( LicenseInfo ) );

            if( _QueryRightForAction( f_rgpdstrAction[iAction],
                                      f_pcontextLQ,
                                      f_pDatastore,
                                      f_pfnPolicyCallback,
                                      f_pvCallbackParameter,
                                     &fUpdateWithPMExpiry,
                                     &pdstrRightUsed ) != DRM_SUCCESS )
            {
                continue;   /* no right for this action, try next one */
            }

            /* The right is allowed. Let us now check if the license can be used on this machine. */
            if( _GetV2LicenseInfo( f_pcontextLQ,
                                  &f_pcontextLQ->pLicEval->dstrContentLicense,
                                  &f_rgLicinfo[iAction],
                                   pdstrRightUsed,
                                   f_pcontextLQ->pbLIDSecStore ) != DRM_SUCCESS )
            {
                continue;   /* nope! */
            }

            fHasRights = TRUE;

            if( f_pcontextLQ->fCurrentLicenseIsXML )
            {
                if( DRM_WMDRM_LIC_HasOPLRestriction( &f_pcontextLQ->pLicEval->dstrContentLicense, pdstrRightUsed ) )
                {
                    f_rgLicinfo[iAction].fHasOPLs = TRUE;
                }

                if( DRM_WMDRM_LEVL_HasSAP( &f_pcontextLQ->pLicEval->dstrContentLicense ) )
                {
                    f_rgLicinfo[iAction].fRequiresSAP = TRUE;
                }
            }
            else if( DRM_UTL_DSTRStringsEqual( f_rgpdstrAction[iAction], &g_dstrWMDRM_RIGHT_COPY ) )
            {
                wRightID = XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER;

                if( XMR_IS_COPYPROTECTIONLEVEL_VALID(&f_pcontextLQ->licXMR) )
                {
                    f_rgLicinfo[iAction].fHasOPLs = TRUE;
                }

                if( XMR_IS_COPY_VALID( &f_pcontextLQ->licXMR ) )
                {
                    pExtRestrictLoop = f_pcontextLQ->licXMR.containerOuter.containerCopyPolicies.pUnknownObjects;
                }
            }
            else if( DRM_UTL_DSTRStringsEqual( f_rgpdstrAction[iAction], &g_dstrWMDRM_RIGHT_PLAYBACK )
                  || DRM_UTL_DSTRStringsEqual( f_rgpdstrAction[iAction], &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY ) )
            {
                wRightID = XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER;

                if( XMR_IS_OPL_VALID( &f_pcontextLQ->licXMR ) )
                {
                    f_rgLicinfo[iAction].fHasOPLs = TRUE;
                }

                if( XMR_IS_PLAY_VALID( &f_pcontextLQ->licXMR ) )
                {
                    pExtRestrictLoop = f_pcontextLQ->licXMR.containerOuter.containerPlaybackPolicies.pUnknownObjects;
                }
            }
            else if( f_pcontextLQ->licXMR.containerOuter.containerUnknown.fValid )
            {
                DRM_LONG lExtObjType = 0;

                /* Check if the string they gave is a 16-bit number, if so it would be the
                ** object type of a container that may be an extensible right
                */
                if( DRM_SUCCEEDED( DRMCRT_wcsntol( f_rgpdstrAction[iAction]->pwszString, f_rgpdstrAction[iAction]->cchString, &lExtObjType )
                  && lExtObjType > 0 && lExtObjType <= MAX_UNSIGNED_TYPE( DRM_WORD ) ) )
                {
                    DRM_XMR_UNKNOWN_CONTAINER *pUnknownContainer = &f_pcontextLQ->licXMR.containerOuter.containerUnknown;

                    /* Seems to be a valid object type */

                    while( pUnknownContainer != NULL
                        && pUnknownContainer->fValid )
                    {
                        if( pUnknownContainer->wType == lExtObjType )
                        {
                            /* We found a match */
                            pExtRestrictLoop = pUnknownContainer->pObject;
                            break;
                        }

                        pUnknownContainer = pUnknownContainer->pNext;
                    }
                }

                wRightID = (DRM_WORD)lExtObjType;
            }

            while( pExtRestrictLoop != NULL
                && pExtRestrictLoop->fValid )
            {
                if( ( pExtRestrictLoop->wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
                {
                    if( ( pExtRestrictLoop->wFlags & XMR_FLAGS_MUST_UNDERSTAND ) != 0 )
                    {
                        f_rgLicinfo[iAction].fHasMUExtensibleRestrictions = TRUE;
                    }

                    if( f_pfnPolicyCallback != NULL )
                    {
                        DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT oCallback = {0};

                        /*
                        ** This is likley an extensible rights container.
                        ** Need to pass it up to the app for them to parse.
                        */
                        oCallback.wRightID = wRightID;
                        oCallback.pRestriction = pExtRestrictLoop;
                        oCallback.pXMRLicense = &f_pcontextLQ->licXMR;
                        oCallback.pContextSST = f_pcontextLQ->pbLIDSecStore;

                        f_pfnPolicyCallback( &oCallback, DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK, f_pvCallbackParameter );
                    }
                }
                pExtRestrictLoop = pExtRestrictLoop->pNext;
            }
        }while (iAction > 0);
    }

    return fHasRights;
}


static DRM_BOOL _GetRootVariance(
    IN OUT DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN     DRM_DWORD                f_dwChainDepth,
    IN     DRM_DWORD                f_cRootVariance,
    IN     DRM_KID                 *f_pkidParent,
       OUT DRM_DWORD               *f_piCurrVariance )
{
    DRM_BOOL  fResult = FALSE;
    DRM_DWORD i = 0;

    for (i=0; i<f_cRootVariance && i<DRM_MAX_CHAIN_ROOTVARIANCE_SUPPORT; i++)
    {
        if( MEMCMP(f_pkidParent, &(f_pcontextLQ->rgStack[f_dwChainDepth].rgkidRoot[i]), SIZEOF(DRM_KID)) == 0 )
        {
            *f_piCurrVariance = i;
            fResult = TRUE;
            break;
        }
    }

    if( i < DRM_MAX_CHAIN_ROOTVARIANCE_SUPPORT )
    {
        *f_piCurrVariance = i;
        fResult = TRUE;
    }

    return fResult;
}



static DRM_RESULT _AddRootVariance(
    IN OUT DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN     DRM_DWORD                f_dwChainDepth,
    IN     DRM_DWORD                f_iCurrVariance,
    IN     DRM_KID                 *f_pkidParent )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  iAction   = 0;

    ChkArg(f_iCurrVariance < DRM_MAX_CHAIN_ROOTVARIANCE_SUPPORT);
    MEMMOVE(&(f_pcontextLQ->rgStack[f_dwChainDepth].rgkidRoot[f_iCurrVariance]), f_pkidParent, SIZEOF(DRM_KID));

    ZEROMEM(f_pcontextLQ->rgStack[f_dwChainDepth].rgRootState[f_iCurrVariance], SIZEOF(DRM_LICENSE_STATE_DATA   ) * DRM_MAX_RIGHTS_SUPPORTED);
    ZEROMEM(f_pcontextLQ->rgStack[f_dwChainDepth].rgLeafState[f_iCurrVariance], SIZEOF(DRM_LICENSE_STATE_DATA   ) * DRM_MAX_RIGHTS_SUPPORTED);
    for ( iAction = 0; iAction < DRM_MAX_RIGHTS_SUPPORTED; iAction++ )
    {
        f_pcontextLQ->rgStack[f_dwChainDepth].rgRootState[f_iCurrVariance][iAction].dwCategory = DRM_LICENSE_STATE_NORIGHT;
        f_pcontextLQ->rgStack[f_dwChainDepth].rgLeafState[f_iCurrVariance][iAction].dwCategory = DRM_LICENSE_STATE_NORIGHT;
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function :   _QueryAndAggregateLicStatesForKID
** Synopsis :   Query and aggregate license state for given KID
** Arguments :
** Returns :    DRM_SUCCESS if no error, otherwise, specific error code is returned
** Notes :      - All licenses for given KID are scanned.
**              - support up to "DRM_MAX_CHAIN_ROOTVARIANCE_SUPPORT" root KID variances
**                per leaf license of same KID.
*/
static DRM_RESULT _QueryAndAggregateLicStatesForKID(
    IN       DRM_KID                  *f_pKID,
    IN       DRM_DWORD                 f_dwAggregationType,
    IN OUT   DRM_VIEW_RIGHTS_CONTEXT  *f_pcontextLQ,
    IN OUT   DRM_DST                  *f_pDatastore,
    IN       DRM_BOOL                  f_fDeleteExpiredLicenses,
    IN       DRM_DWORD                 f_dwChainDepth,
    IN       DRM_DWORD                 f_cActions,
    IN const DRM_CONST_STRING*  const  f_rgpdstrAction[],
       OUT   DRM_LICENSE_STATE_DATA    f_rgStateData[],
     IN       DRMPFNPOLICYCALLBACK     f_pfnPolicyCallback,
    IN const DRM_VOID                 *f_pvCallbackParameter)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD   iAction          = 0;
    DRM_DWORD   cRootVariance    = 0;
    DRM_DWORD   iCurrVariance    = 0;
    DRM_BOOL    fLicenseDeleted  = FALSE;
    DRM_BOOL    fHasParentLink   = FALSE;
    DRM_BOOL    fFromCache       = FALSE;
    DRM_KID     kidParent        = {0};
    DRM_LID     lid              = {0};
    DRM_BOOL    fUnlimited       = FALSE;

    /* Terminating condition for recursive call: check chaining depth
    ** up to DRM_MAX_LICENSE_CHAIN_DEPTH depth of recursive call
    */
    if( f_dwChainDepth >= f_pcontextLQ->dwLicChainDepth  ||  f_dwChainDepth >= DRM_MAX_LICENSE_CHAIN_DEPTH )
    {
        ChkDR( DRM_E_LIC_CHAIN_TOO_DEEP );  /* exceeding max license chain depth, we are done */
    }

    /* init buffer */
    for ( iAction = 0; iAction < f_cActions; iAction++ )
    {
        f_pcontextLQ->rgStack[f_dwChainDepth].rgStatePri[iAction].dwCategory = DRM_LICENSE_STATE_NORIGHT;
        ZEROMEM(&(f_pcontextLQ->rgStack[f_dwChainDepth].rgkidRoot), SIZEOF(DRM_KID) * DRM_MAX_CHAIN_ROOTVARIANCE_SUPPORT);
    }

    /* init enum license for given KID */
    ChkDR(DRM_LST_InitEnum(f_pcontextLQ->pActiveLicStore, f_pKID, TRUE, f_pcontextLQ->rgpLicQueryContext[f_dwChainDepth]));
    while ( !fUnlimited )  /* try each license equals to KID */
    {
        if( _LoadNextLicense( f_pcontextLQ,
                              f_pDatastore,
                              f_dwChainDepth,
                             &fFromCache,
                             &lid,
                             &fHasParentLink,
                             &kidParent,
                              NULL,
                              NULL,
                              f_pcontextLQ->rgLicInfo ) != DRM_SUCCESS )
        {
            dr = DRM_SUCCESS;       /* no more license */
            break;
        }

        if( !fFromCache )  /* no cached value found */
        {
            if( fHasParentLink && !_GetRootVariance(f_pcontextLQ, f_dwChainDepth, cRootVariance, &kidParent, &iCurrVariance) )
            {
                continue;   /* ignore this license if root variance is full */
            }

            /* Is the license selectable or expired? */
            dr = _IsLicenseSelectable(
                    f_pcontextLQ->rgpLicQueryContext[f_dwChainDepth],
                    f_pcontextLQ,
                    f_pDatastore,
                    f_fDeleteExpiredLicenses,
                   &fLicenseDeleted );
            if( dr != DRM_SUCCESS
             || fLicenseDeleted
             || _IsLicenseExpired( f_pcontextLQ, f_pcontextLQ->pbLIDSecStore ) )
            {
                continue;   /* not selectable or expired */
            }

            /* get right for each action from current license */
            if( !_CollectLicStateforRequestedActions( f_pcontextLQ,
                                                      f_pDatastore,
                                                      f_cActions,
                                                      f_rgpdstrAction,
                                                      fHasParentLink,
                                                      f_pcontextLQ->rgLicInfo,
                                                      f_pfnPolicyCallback,
                                                      f_pvCallbackParameter ) )
            {
                continue;   /* this license has no rights for actions */
            }

            if( f_dwChainDepth > 0  &&  iCurrVariance == cRootVariance )   /* cache LID and parentKID */
            {
                /* add rgLicResult and rgLicInfo to cache */
                _AddLicResultCache(f_pcontextLQ, &lid, fHasParentLink, &kidParent, NULL, 0, f_pcontextLQ->rgLicInfo);
            }
        }

        if( !fHasParentLink  ||  f_dwAggregationType != DRM_ASD_AGGREGATE_ROOT_LICENSES)
        {
            /* aggregate SIMPLE license */
            _AggregateStateDataAndLicenseInfoMultiple( f_pcontextLQ->pBBContext->pOEMContext,
                                                       f_rgStateData,
                                                       f_pcontextLQ->rgLicInfo,
                                                       f_cActions );

            for ( iAction = 0; iAction < f_cActions; iAction++ )
            {
                fUnlimited = TRUE;      /* Assume unlimited for now */
                if( f_rgStateData[iAction].dwCategory != DRM_LICENSE_STATE_UNLIM )
                {
                    fUnlimited = FALSE;
                    break; /* from the for loop */
                }
            }
        }
        else
        {
            if( iCurrVariance == cRootVariance )   /* this is a new root variance */
            {
                _AddRootVariance(f_pcontextLQ, f_dwChainDepth, iCurrVariance, &kidParent);
            }

            /* aggregate LEAF license */
            _AggregateStateDataAndLicenseInfoMultiple( f_pcontextLQ->pBBContext->pOEMContext,
                                                       f_pcontextLQ->rgStack[f_dwChainDepth].rgLeafState[iCurrVariance],
                                                       f_pcontextLQ->rgLicInfo,
                                                       f_cActions);

            if( iCurrVariance == cRootVariance )   /* this is a new root variance */
            {
                ++cRootVariance;

                /* call recursively for parent link search */
                _QueryAndAggregateLicStatesForKID( &kidParent,
                                                    f_dwAggregationType,
                                                    f_pcontextLQ,
                                                    f_pDatastore,
                                                    FALSE,  /* do not perform delete on parent link */
                                                    f_dwChainDepth+1,
                                                    f_cActions,
                                                    f_rgpdstrAction,
                                                    f_pcontextLQ->rgStack[f_dwChainDepth].rgRootState[iCurrVariance],
                                                    f_pfnPolicyCallback,
                                                    f_pvCallbackParameter );
            }

            /* we cannot tell if there is any unlimited license before we aggregate root to leaf */
        }
    }

    for (iCurrVariance = 0; iCurrVariance < cRootVariance; iCurrVariance++)
    {
        /* aggregate ROOT to LEAF */
        _AggregateStateDataAndStateDataChainMultiple( f_pcontextLQ->rgStack[f_dwChainDepth].rgLeafState[iCurrVariance],
                                                      f_pcontextLQ->rgStack[f_dwChainDepth].rgRootState[iCurrVariance],
                                                      f_cActions );

        /*
        ** Aggregate LEAF to SIMPLE
        ** Simple takes precedence in case of ties, so pass simple first
        */
        _AggregateStateDataAndStateDataMultiple( f_pcontextLQ->pBBContext->pOEMContext,
                                                 f_rgStateData,
                                                 f_pcontextLQ->rgStack[f_dwChainDepth].rgLeafState[iCurrVariance],
                                                 f_cActions );
    }

ErrorExit:
    return dr;
}



/*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_GetLicenseAggregateData(
    __in_ecount( f_cActionsQueried) const DRM_CONST_STRING*  const f_rgpdstrAction[], /* Array of DRM_CONST_STRING pointers */
    __out_opt                             DRM_LICENSE_STATE_DATA   f_rgStateData[],   /* array of DRM_LICENSE_STATE_DATAs */
    __in                                  DRM_DWORD                f_cActionsQueried,
    __inout_opt                           DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __inout                               DRM_DST                 *f_pDatastore,
    __in                                  DRM_BOOL                 f_fDeleteExpiredLicenses,
    __in                                  DRM_DWORD                f_dwAggregationType,
    __in_opt                              DRMPFNPOLICYCALLBACK     f_pfnPolicyCallback,
    __in_opt                        const DRM_VOID                *f_pvCallbackParameter)
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_WORD   iAction = 0;
    DRM_DWORD  i       = 0;

    ChkArg( f_rgpdstrAction   != NULL
         && f_rgStateData     != NULL
         && f_pcontextLQ      != NULL
         && f_pDatastore      != NULL
         && f_cActionsQueried  > 0
         && f_cActionsQueried <= DRM_MAX_RIGHTS_SUPPORTED );

    for( iAction = 0; iAction < f_cActionsQueried; iAction ++ )
    {
        ChkDRMString( f_rgpdstrAction[iAction] );
    }

    ZEROMEM(f_pcontextLQ->rgLicInfo,           SIZEOF(f_pcontextLQ->rgLicInfo ) );
    ZEROMEM(f_pcontextLQ->rgStack,             SIZEOF(DRM_STATE_DATA_STACK) * DRM_MAX_LICENSE_CHAIN_DEPTH);
    ZEROMEM(f_rgStateData,                     SIZEOF(DRM_LICENSE_STATE_DATA   ) * f_cActionsQueried );

    /* initialize cache to NULL. This query will not use cache for now */
    f_pcontextLQ->iCurrSlot    = 0;
    f_pcontextLQ->cCacheSlots  = 0;
    f_pcontextLQ->rgCacheSlots = NULL;
    f_pcontextLQ->dwLicChainDepth = DRM_MAX_LICENSE_CHAIN_DEPTH;

    /* init buffer */
    for ( iAction = 0; iAction < f_cActionsQueried; iAction++ )
    {
        f_rgStateData[iAction].dwCategory = DRM_LICENSE_STATE_NORIGHT;
    }

    /* do XML license store query */
    if( f_pcontextLQ->pbLicenseStoreXML != NULL )
    {
        f_pcontextLQ->fCurrentLicenseIsXML    = TRUE;
        f_pcontextLQ->pLicEval->fLicenseIsXMR = FALSE;
        f_pcontextLQ->pActiveLicStore         = f_pcontextLQ->pbLicenseStoreXML;
        f_pcontextLQ->pLicEval->plicenseXMR   = NULL;
        f_pcontextLQ->pbCurrentLicenseBuff    = NULL;

        MEMSET(&f_pcontextLQ->stack, 0, SIZEOF(DRM_STACK_ALLOCATOR_CONTEXT));
        f_pcontextLQ->stack.cbStack = f_pcontextLQ->cbBuffer;
        f_pcontextLQ->stack.pbStack = f_pcontextLQ->pbBuffer;

        _QueryAndAggregateLicStatesForKID( &f_pcontextLQ->KID,
                                            f_dwAggregationType,
                                            f_pcontextLQ,
                                            f_pDatastore,
                                            f_fDeleteExpiredLicenses,
                                            0,
                                            f_cActionsQueried,
                                            f_rgpdstrAction,
                                            f_rgStateData,
                                            f_pfnPolicyCallback,
                                            f_pvCallbackParameter );
    }

    if( f_pcontextLQ->pbLicenseStoreXMR != NULL )
    {
        f_pcontextLQ->fCurrentLicenseIsXML    = FALSE;
        f_pcontextLQ->pLicEval->fLicenseIsXMR = TRUE;
        f_pcontextLQ->pActiveLicStore         = f_pcontextLQ->pbLicenseStoreXMR;
        f_pcontextLQ->pLicEval->plicenseXMR   = &(f_pcontextLQ->licXMR);
        f_pcontextLQ->pbCurrentLicenseBuff    = NULL;

        MEMSET(&f_pcontextLQ->stack, 0, SIZEOF(DRM_STACK_ALLOCATOR_CONTEXT));
        f_pcontextLQ->stack.cbStack = f_pcontextLQ->cbBuffer;
        f_pcontextLQ->stack.pbStack = f_pcontextLQ->pbBuffer;

        _QueryAndAggregateLicStatesForKID( &f_pcontextLQ->KID,
                                            f_dwAggregationType,
                                            f_pcontextLQ,
                                            f_pDatastore,
                                            f_fDeleteExpiredLicenses,
                                            0,
                                            f_cActionsQueried,
                                            f_rgpdstrAction,
                                            f_rgStateData,
                                            f_pfnPolicyCallback,
                                            f_pvCallbackParameter );
    }


    dr = DRM_SUCCESS;
ErrorExit:

    if( f_pcontextLQ != NULL )
    {
        for (i = 0; i < DRM_MAX_LICENSE_CHAIN_DEPTH; i++)
        {
            if( f_pcontextLQ->rgpLicQueryContext[i] != NULL )
            {
                ZEROMEM( f_pcontextLQ->rgpLicQueryContext[i], SIZEOF( *(f_pcontextLQ->rgpLicQueryContext[i])) );

            }
        }
    }
    return dr;
}


/*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_IsAllowed(
    __in    const DRM_CONST_STRING        *f_pdstrAction,
    __inout       DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __inout       DRM_DST                 *f_pDatastore )
{
    DRM_RESULT                   dr = DRM_E_LOGICERR;
    const DRM_CONST_STRING      *ppArray [1];
    DRM_LICENSE_STATE_DATA       rgStateData[1];

    ppArray [0] = f_pdstrAction;

    ChkArg( f_pcontextLQ != NULL );
    ChkDRMString( f_pdstrAction );

    ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData( ppArray,
                                                   rgStateData,
                                                   1,
                                                   f_pcontextLQ,
                                                   f_pDatastore,
                                                   FALSE,
                                                   DRM_ASD_AGGREGATE_ROOT_LICENSES,
                                                   NULL,
                                                   NULL ) );

    if( rgStateData[0].dwCategory != DRM_LICENSE_STATE_NORIGHT
     && rgStateData[0].dwCategory != DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        dr = DRM_S_FALSE;
    }

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        dr = DRM_S_FALSE;
    }
    return dr;
}

/******************************************************************************
** Function :   DRM_ASD_InitQuery
** Synopsis :   Init context for query rights.
** Arguments :  f_pdstrDevCert - optional, can be NULL
**              f_dwTargetOPL  - target device Output Level
**              f_pcontextLQ - output query context
** Returns :
** Notes :
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_InitCanDoQuery(
    __in        DRM_CRYPTO_CONTEXT       *f_pDrmCrypto,
    __in        DRM_LICSTORE_CONTEXT     *f_pbLicenseStoreXML,
    __in        DRM_LICSTORE_CONTEXT     *f_pbLicenseStoreXMR,     /* optional for now */
    __in_ecount( DRM_MAX_LICENSE_CHAIN_DEPTH)   DRM_LICSTOREENUM_CONTEXT *f_rgpLicQueryContext[DRM_MAX_LICENSE_CHAIN_DEPTH],
    __in        DRM_LICEVAL_CONTEXT      *f_pLicEval,
    __in        DRM_BYTE                 *f_pbBuffer,
    __in        DRM_DWORD                 f_cbBuffer,
    __in        DRM_LONG                  f_lDeviceAppSec,
    __in        DRM_BOOL                  f_fDeviceHasSerialNum,
    __in        DRM_DWORD                 f_dwDeviceCopyOPL,
    __in  const DRM_GUID                 *f_pDeviceGUID,
    __in  const DRM_CONST_STRING         *f_pdstrDevCert,
    __in        DRM_DWORD                 f_cCacheSlots,     /* must be >= 1 */
    __in        DRM_CANDO_QUERY_CACHE    *f_rgCacheSlots,
    __out       DRM_VIEW_RIGHTS_CONTEXT  *f_pcontextLQ )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;

    ChkArg(f_pDrmCrypto         != NULL
        && f_pbLicenseStoreXML  != NULL
        && f_rgpLicQueryContext != NULL
        && f_pLicEval           != NULL
        && f_pbBuffer           != NULL
        && f_pDeviceGUID        != NULL
        && f_pcontextLQ         != NULL
        && f_cCacheSlots        >= 1
        && f_rgCacheSlots       != NULL
        && f_pLicEval->pcontextSSTGlobal  != NULL
        && f_pLicEval->pcontextSSTLicense != NULL);

    MEMSET(f_pcontextLQ, 0, SIZEOF(DRM_VIEW_RIGHTS_CONTEXT));

    f_pcontextLQ->pbGlobalSecStore  = f_pLicEval->pcontextSSTGlobal;
    f_pcontextLQ->pbLIDSecStore     = f_pLicEval->pcontextSSTLicense;
    f_pcontextLQ->pbLicenseStoreXML = f_pbLicenseStoreXML;
    f_pcontextLQ->pbLicenseStoreXMR = f_pbLicenseStoreXMR;
    f_pcontextLQ->pLicEval          = f_pLicEval;
    f_pcontextLQ->pbBuffer          = f_pbBuffer;
    f_pcontextLQ->cbBuffer          = f_cbBuffer;

    for (i=0; i<DRM_MAX_LICENSE_CHAIN_DEPTH; i++)
    {
        ChkArg(f_rgpLicQueryContext[i] != NULL);
        f_pcontextLQ->rgpLicQueryContext[i] = f_rgpLicQueryContext[i];
    }

    /* parse the given device cert if not NULL */
    if( f_pdstrDevCert != NULL )
    {
        DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;

        ChkDRMString(f_pdstrDevCert);
        ChkDR(DRM_DCP_VerifyDeviceCert( NULL, f_pdstrDevCert, DRM_DCP_VERIFY_ENTIRE_DEVCERT, NULL, f_pDrmCrypto ));
        ChkDR(DRM_DCP_LoadPropertiesCache( NULL, f_pdstrDevCert, NULL, f_pDrmCrypto, &f_pcontextLQ->cacheDevCert ));

        /* get max chain depth */
        if( DRM_DCP_GetAttribute( NULL, f_pdstrDevCert, DRM_DEVCERT_MAXCHAINDEPTH, NULL, &dstrValue) == DRM_SUCCESS )
        {
            DRMCRT_wcsntol( dstrValue.pwszString, dstrValue.cchString, (DRM_LONG*)&f_pcontextLQ->dwLicChainDepth);
        }
    }
    else
    {
        f_pcontextLQ->dwLicChainDepth = DRM_MAX_LICENSE_CHAIN_DEPTH;
    }

    f_pcontextLQ->fIsWMDRMPDDevice    = (f_pdstrDevCert != NULL);
    f_pcontextLQ->lDeviceAppSec       = f_lDeviceAppSec;
    f_pcontextLQ->dwDeviceCopyOPL     = f_dwDeviceCopyOPL;
    f_pcontextLQ->deviceGUID          = *f_pDeviceGUID;
    f_pcontextLQ->fDeviceHasSerialNum = f_fDeviceHasSerialNum;

    /* initialize cache */
    f_pcontextLQ->iCurrSlot = 0;
    f_pcontextLQ->cCacheSlots = f_cCacheSlots;
    f_pcontextLQ->rgCacheSlots = f_rgCacheSlots;
    for( i=0; i<f_cCacheSlots; i++ )
    {
        MEMSET( &f_rgCacheSlots[i], 0, SIZEOF(DRM_CANDO_QUERY_CACHE) );
    }

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   DRM_VIEWRIGHTS_CandoQuery
**
** Synopsis :   Query rights for given actions
**
** Arguments :  pcontextHDS - HDS context
**              pNamespace  - Namespace identifier to be opened/created
**              eOpenMode   - eDRM_DST_CREATE_NEW / eDRM_DST_OPEN_EXISTING
**              wMaxNumChildren - Number of children if a new namespace is to
**                                be created
**              pcontextNS  - Namespace context
**
** Returns :    DRM_E_DSTNAMESPACENOTFOUND - if namespace is not found and
**                  request is only to open existing existing namespace
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_CandoQuery(
    __in                                   DRM_KID                 *f_pKID,               /* kid to query */
    __in                                   DRM_DWORD                f_cActionsQueried,    /* # of actions to query */
    __in_ecount( f_cActionsQueried ) const DRM_CONST_STRING        *f_rgpdstrAction[],    /* Array of DRM_CONST_STRING pointers */
    __inout                                DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __inout                                DRM_DST                 *f_pDatastore,
    __in                                   DRM_BOOL                 f_fDeleteExpiredLicenses,
    __out_ecount( f_cActionsQueried )      DRM_DWORD                f_rgResults[] )       /* query result */
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_WORD   iAction = 0;
    DRM_BOOL   fLicFound = FALSE;
    DRM_BOOL   fRecentExpired = FALSE;

    /* check arguments */
    ChkArg( f_pKID                      != NULL
         && f_pcontextLQ                != NULL
         && f_pcontextLQ->pBBContext    != NULL
         && f_rgpdstrAction             != NULL
         && f_pDatastore                != NULL
         && f_rgResults                 != NULL
         && f_cActionsQueried            > 0
         && f_cActionsQueried           <= DRM_MAX_RIGHTS_SUPPORTED );
    for( iAction = 0; iAction           < f_cActionsQueried; iAction ++ )
    {
        ChkDRMString( f_rgpdstrAction[iAction] );
        f_rgResults[iAction] = DRM_LICQUERY_NOT_ENABLED;    /* initialize result */
    }

    /* initialize cache */
    f_pcontextLQ->iCurrSlot = 0;
    for (iAction=0; iAction<f_pcontextLQ->cCacheSlots; iAction++)
    {
        MEMSET(&(f_pcontextLQ->rgCacheSlots[iAction]), 0, SIZEOF(DRM_CANDO_QUERY_CACHE));
    }

    /* do XML license store query */
    if( f_pcontextLQ->pbLicenseStoreXML != NULL )
    {
        f_pcontextLQ->fCurrentLicenseIsXML      = TRUE;
        f_pcontextLQ->pActiveLicStore           = f_pcontextLQ->pbLicenseStoreXML;
        f_pcontextLQ->pLicEval->plicenseXMR     = NULL;
        f_pcontextLQ->pbCurrentLicenseBuff      = NULL;
        f_pcontextLQ->pLicEval->fLicenseIsXMR   = FALSE;
        f_pcontextLQ->pLicEval->fIsReadOnlyMode = TRUE;

        MEMSET(&f_pcontextLQ->stack, 0, SIZEOF(DRM_STACK_ALLOCATOR_CONTEXT));
        f_pcontextLQ->stack.cbStack = f_pcontextLQ->cbBuffer;
        f_pcontextLQ->stack.pbStack = f_pcontextLQ->pbBuffer;

        ChkDR( _QueryRightForCandoActions( f_pcontextLQ,
                                           f_pDatastore,
                                           f_fDeleteExpiredLicenses,
                                           f_pKID,
                                           0xFFFFFFFF,  /* initial vector: include all actions */
                                           0,           /* start from LEAF */
                                           f_cActionsQueried,
                                           f_rgpdstrAction,
                                          &fLicFound,
                                          &fRecentExpired,
                                           f_rgResults ) );
        for ( iAction = 0; iAction < f_cActionsQueried; iAction ++ )
        {
            if( f_rgResults[iAction] == DRM_LICQUERY_SATISFIED )
            {
                f_rgResults[iAction] = 0;
            }
        }
    }

    /* do XMR license store query */
    if( f_pcontextLQ->pbLicenseStoreXMR != NULL )
    {
        f_pcontextLQ->fCurrentLicenseIsXML = FALSE;
        f_pcontextLQ->pActiveLicStore = f_pcontextLQ->pbLicenseStoreXMR;
        f_pcontextLQ->pLicEval->plicenseXMR = &(f_pcontextLQ->licXMR);
        f_pcontextLQ->pLicEval->fLicenseIsXMR = TRUE;
        f_pcontextLQ->pbCurrentLicenseBuff = NULL;

        MEMSET(&f_pcontextLQ->stack, 0, SIZEOF(DRM_STACK_ALLOCATOR_CONTEXT));
        f_pcontextLQ->stack.cbStack = f_pcontextLQ->cbBuffer;
        f_pcontextLQ->stack.pbStack = f_pcontextLQ->pbBuffer;

        ChkDR( _QueryRightForCandoActions( f_pcontextLQ,
                                           f_pDatastore,
                                           f_fDeleteExpiredLicenses,
                                           f_pKID,
                                           0xFFFFFFFF,  /* initial vector: include all actions */
                                           0,           /* start from LEAF */
                                           f_cActionsQueried,
                                           f_rgpdstrAction,
                                          &fLicFound,
                                          &fRecentExpired,
                                           f_rgResults ) );
        for ( iAction = 0; iAction < f_cActionsQueried; iAction ++ )
        {
            if( f_rgResults[iAction] == DRM_LICQUERY_SATISFIED )
            {
                f_rgResults[iAction] = 0;
            }
        }
    }

    /* Check for recently expired leaf licenses */
    ChkDR( DRM_DELETEDSTORE_CheckRecentExpired(
        f_pcontextLQ->pBBContext->pOEMContext,
        f_pDatastore,
        f_pKID,
        &fRecentExpired ) );

    /*
    ** A recently expired leaf license counts as "finding" a license
    ** since we want to return "expired" and not "no license"
    */
    fLicFound = fLicFound || fRecentExpired;

    for( iAction = 0; iAction < f_cActionsQueried; iAction ++ )
    {
        /*
        ** fLicFound represents whether a license for the KID queried
        ** was found (in any store).  If not, then we return "no license".
        */
        if( !fLicFound )
        {
            f_rgResults[iAction] =
                DRM_LICQUERY_NOT_ENABLED |
                DRM_LICQUERY_NOT_ENABLED_NO_LICENSE;
        }
        /*
        ** If we found a recent expired license AND we didn't
        ** find a license that allows the action, then add the expired flag.
        */
        else if( fRecentExpired && f_rgResults[iAction] != 0 )
        {
            f_rgResults[iAction] |= DRM_LICQUERY_NOT_ENABLED_EXPIRED;
        }
        /*
        ** If we get to this point with no reason set,
        ** then set the generic reason.
        */
        else if( f_rgResults[iAction] == DRM_LICQUERY_NOT_ENABLED )
        {
            f_rgResults[iAction] |= DRM_LICQUERY_NOT_ENABLED_NO_RIGHT;
        }
    }

    dr = DRM_SUCCESS;
ErrorExit:
    if( f_pcontextLQ != NULL )
    {
        for( iAction=0; iAction<DRM_MAX_LICENSE_CHAIN_DEPTH; iAction++ )
        {
            if( f_pcontextLQ->rgpLicQueryContext[iAction] != NULL )
            {
                ZEROMEM( f_pcontextLQ->rgpLicQueryContext[iAction], SIZEOF( *(f_pcontextLQ->rgpLicQueryContext[iAction]) ) );
            }
        }
    }
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

