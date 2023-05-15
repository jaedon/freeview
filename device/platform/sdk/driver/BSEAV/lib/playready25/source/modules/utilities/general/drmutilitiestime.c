/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMUTILITIESGENERAL_C
#include <drmutilitieslite.h>
#include <drmconstants.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DateLessThan(
    __in const DRMFILETIME* f_pFileTime1,
    __in const DRMFILETIME* f_pFileTime2)
{
    DRM_UINT64 u641;
    DRM_UINT64 u642;

    FILETIME_TO_UI64( *f_pFileTime1, u641 );
    FILETIME_TO_UI64( *f_pFileTime2, u642 );
    return DRM_UI64Les( u641, u642);
}

DRM_API DRM_BOOL DRM_CALL DRM_UTL_IsTimeoutExceeded(
    __in_opt    DRM_VOID    *f_pOEMContext,
    __in        DRMFILETIME *f_pftStartTime,
    __in const  DRM_DWORD    f_dwTimeoutSeconds )
{
    DRMFILETIME ft;
    DRM_UINT64  ui64Start;
    DRM_UINT64  ui64Now;

    DRMASSERT( f_pftStartTime != NULL );
    __analysis_assume( f_pftStartTime != NULL );

    Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ft );

    /*
    ** Convert start time to quadword seconds
    */
    FILETIME_TO_UI64( *f_pftStartTime, ui64Start );
    ui64Start = DRM_UI64Div( ui64Start, DRM_UI64( C_TICS_PER_SECOND ) );

    /*
    ** Convert current time to quadword seconds
    */
    FILETIME_TO_UI64( ft, ui64Now );
    ui64Now = DRM_UI64Div( ui64Now, DRM_UI64( C_TICS_PER_SECOND ) );

    /*
    ** Return FALSE if start is in future
    */
    if( DRM_UI64Les( ui64Now, ui64Start ) )
    {
        return FALSE;
    }

    /*
    ** Return TRUE if the timeout value is less than the elapsed time (now - start)
    */
    if( DRM_UI64Les( DRM_UI64( f_dwTimeoutSeconds ), DRM_UI64Sub( ui64Now, ui64Start ) ) )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
** Function: DRM_UTL_GetZuluTimeFromStringAsUI64
**
** Synopsis:    Parses a Zulu datetime format string into a UINT64 datetime value.
**              Advances f_pdstrZuluTime->pwszString by the length of the datetime string.
**              Decrements f_pdstrZuluTime->cchString by the length of the datetime string.
**
** Arguments:   f_pdstrZuluTime - input string in Zulu datetime format
**                                This value is modified!
**              f_pui64DateTime - return value to store the constructed datetime in.
**
** Notes:       Caller should NOT pass in an allocated buffer for f_pdstrZuluTime->pwszString
**              unless they are maintaining a separate pointer for freeing it!
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_GetZuluTimeFromStringAsUI64(
    __inout DRM_CONST_STRING *f_pdstrZuluTime,
    __out   DRM_UINT64       *f_pui64DateTime )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRMSYSTEMTIME Date;
    DRMFILETIME   filetime;

    ChkArg( f_pdstrZuluTime != NULL );
    ChkArg( f_pui64DateTime != NULL );
    *f_pui64DateTime = DRM_UI64HL( 0, 0 );

    ZEROMEM(&Date, SIZEOF(Date));
    if( f_pdstrZuluTime->cchString < 11 ) /* Minimum of 11 characters */
    {
        return DRM_E_INVALIDARG;
    }

    if ( *(f_pdstrZuluTime->pwszString) != g_wchPound)
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    f_pdstrZuluTime->pwszString++; /*Consume # */
    f_pdstrZuluTime->cchString--;

    if ( DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString)) &&
         DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+1)) &&
         DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+2)) &&
         DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+3)) )
    {
        /* We got YYYY             */
        Date.wYear = (DRM_WORD)( (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[0]) - NATIVE_WCHAR(g_wch0) ) * 1000 +
                                 (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[1]) - NATIVE_WCHAR(g_wch0) ) * 100 +
                                 (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[2]) - NATIVE_WCHAR(g_wch0) ) * 10 +
                                 (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[3]) - NATIVE_WCHAR(g_wch0) ) );
        f_pdstrZuluTime->pwszString += 4;
        f_pdstrZuluTime->cchString -= 4;
    }
    else
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    /* Next we should have MM */
    if ( DRMCRT_iswdigit( *(f_pdstrZuluTime->pwszString) ) && DRMCRT_iswdigit( *(f_pdstrZuluTime->pwszString+1) ) )
    {
        /* We got MM */
        Date.wMonth = (DRM_WORD)( (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[0]) - NATIVE_WCHAR(g_wch0) ) * 10 +
                                  (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[1]) - NATIVE_WCHAR(g_wch0) ) );
        f_pdstrZuluTime->pwszString += 2;
        f_pdstrZuluTime->cchString -= 2;
    }
    else
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }
    /* Next we should have DD */
    if ( DRMCRT_iswdigit( *(f_pdstrZuluTime->pwszString) ) && DRMCRT_iswdigit( *(f_pdstrZuluTime->pwszString+1) ) )
    {
        /* We got DD */
        Date.wDay = (DRM_WORD)( (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[0]) - NATIVE_WCHAR(g_wch0) ) * 10 +
                                (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[1]) - NATIVE_WCHAR(g_wch0) ) );
        f_pdstrZuluTime->pwszString += 2;
        f_pdstrZuluTime->cchString -= 2;
    }
    else
    {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }
    /* Now we expect a space or Z */
    if ( *(f_pdstrZuluTime->pwszString) != g_wchSpace && *(f_pdstrZuluTime->pwszString) != g_wchZ )
    {
        dr = DRM_E_INVALIDARG;
    }
    else
    {
        if (*(f_pdstrZuluTime->pwszString) == g_wchSpace)
        {
            f_pdstrZuluTime->pwszString++; /* If it was a space, skip. */
            f_pdstrZuluTime->cchString--;
        }
        /* Now we expect optional time fields */
        if ( f_pdstrZuluTime->cchString > 2 &&
            DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString)) &&
            DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+1)) )
        {
            /* We got HH. */
            Date.wHour =   (DRM_WORD)( (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[0]) - NATIVE_WCHAR(g_wch0) ) * 10
                                    +  (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[1]) - NATIVE_WCHAR(g_wch0) ) );
            f_pdstrZuluTime->pwszString += 2;
            f_pdstrZuluTime->cchString -= 2;
            /* We look for :MM only if we got HH */
            if ( f_pdstrZuluTime->cchString >3 &&
                *(f_pdstrZuluTime->pwszString) == g_wchColon &&
                DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+1)) &&
                DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+2)) )
            {
                Date.wMinute = (DRM_WORD)( (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[1]) - NATIVE_WCHAR(g_wch0) ) * 10
                                         + (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[2]) - NATIVE_WCHAR(g_wch0) ) );
                f_pdstrZuluTime->pwszString += 3;
                f_pdstrZuluTime->cchString -=3;
                /* We look for :SS only if we got MM */
                if (f_pdstrZuluTime->cchString >3 &&
                    *(f_pdstrZuluTime->pwszString) == g_wchColon &&
                    DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+1)) &&
                    DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+2)) )
                {
                    Date.wSecond = (DRM_WORD)( (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[1]) - NATIVE_WCHAR(g_wch0) ) * 10
                                             + (DRM_WORD)( NATIVE_WCHAR(f_pdstrZuluTime->pwszString[2]) - NATIVE_WCHAR(g_wch0) ) );
                    f_pdstrZuluTime->pwszString += 3;
                    f_pdstrZuluTime->cchString -=3;
                }
            }
        }
        /* Now we must see the closing Z# symbols */
        if ( f_pdstrZuluTime->cchString <2 || *(f_pdstrZuluTime->pwszString) != g_wchZ)
        {
            dr = DRM_E_INVALIDARG;
            goto ErrorExit;
        }
        f_pdstrZuluTime->pwszString++; /* Consume Z */
        f_pdstrZuluTime->cchString--;
        if (*(f_pdstrZuluTime->pwszString) != g_wchPound)
        {
            dr = DRM_E_INVALIDARG;
            goto ErrorExit;
        }
        f_pdstrZuluTime->pwszString++; /* Consume # */
        f_pdstrZuluTime->cchString--;
    }
    if (!Oem_Clock_SystemTimeToFileTime(&Date, &filetime))
    {
        dr = DRM_E_INVALIDARG; /* One or more fields in the date must be invalid. */
        goto ErrorExit;
    }
    FILETIME_TO_UI64( filetime, *f_pui64DateTime );
ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
