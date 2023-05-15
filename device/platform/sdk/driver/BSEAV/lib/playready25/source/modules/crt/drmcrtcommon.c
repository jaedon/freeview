/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMCRTCOMMON_C
#include <drmcrt.h>
#include <byteorder.h>
#include <drmconstants.h>
#include <drmerr.h>
#include <drmlastinclude.h>
/* Do not include or use drmmathsafe.h.  Doing so will cause link-time breaks in code that uses this CRT but not drmcore.lib.  */

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function:    _DRMCRT_ASCIIToByteValue
**
** Synopsis:    Converts ASCII character to numeric value.
**
** Arguments:   [f_chAscii]   : ASCII character.
**              [f_pdwVal]    : Address of byte variable for the value.

** Returns:     TRUE    - on success
**              FALSE   - if character is not numeric or hex digit.
******************************************************************************/

static DRM_BOOL _DRMCRT_ASCIIToByteValue( __in DRM_CHAR f_chAscii, __out DRM_BYTE *f_pdwVal )
{
    DRM_BOOL bVal = TRUE;

    if ( f_pdwVal == NULL )
    {
        bVal = FALSE;
        goto ErrorExit;
    }

    /* Base 10 digit */
    if ( '0' <= f_chAscii  &&  f_chAscii <= '9' )
    {
        *f_pdwVal = (DRM_DWORD)( f_chAscii - '0' );
        goto ErrorExit;
    }

    /* Capital HEX digit - A,B,C,D,E,F */
    if ( 'A' <= f_chAscii  &&  f_chAscii <= 'F' )
    {
        *f_pdwVal = (DRM_DWORD)( f_chAscii - 'A' + 10 );
        goto ErrorExit;
    }
    /* lower case hex digit - a,b,c,d,e,f */
    if ( 'a' <= f_chAscii  &&  f_chAscii <= 'f' )
    {
        *f_pdwVal = (DRM_DWORD)( f_chAscii - 'a' + 10 );
        goto ErrorExit;
    }

    /* Unknown character, return FALSE */
    bVal = FALSE;

ErrorExit :
    return bVal;
}


/*****************************************************************************
** Function:    _DRMCRT_WCHARToByteValue
**
** Synopsis:    Converts WCHAR character to numeric value.
**
** Arguments:   [f_wch]       : WCHAR character.
**              [f_dwBase]    : base for conversion. Should be 10 or 16
**              [f_pdwVal]    : Address of byte variable for the value.

** Returns:     TRUE    - on success
**              FALSE   - if character is not numeric or hex digit.
******************************************************************************/

static DRM_NO_INLINE DRM_BOOL _DRMCRT_WCHARToByteValue(
   __in const         DRM_WCHAR f_wch,
   __in const         DRM_DWORD f_dwBase,
   __out_bcount( 1 )  DRM_BYTE *f_pdwVal )
{
    /*
     * assume a non-numeric/hex value
     */
    DRM_BOOL bVal  = FALSE;

    if ( f_pdwVal == NULL || ( f_dwBase != 10 && f_dwBase != 16 ) )
    {
        /*
         * Invalid argument.
        */
        goto ErrorExit;
    }

    /* Base 10 digit */
    if ( DRMCRT_iswdigit( f_wch ) )
    {
        DRM_WORD wVal = (DRM_WORD)( NATIVE_WCHAR( f_wch ) - NATIVE_WCHAR( g_wch0 ) );
        if( wVal <= MAX_UNSIGNED_TYPE(DRM_BYTE) )
        {
            *f_pdwVal = (DRM_BYTE)wVal;
            bVal      = TRUE;
        }
        else
        {
            *f_pdwVal = MAX_UNSIGNED_TYPE( DRM_BYTE );  /* Overflow: set to max and don't set bVal = TRUE.  Don't use mathsafe which is in drmcore.lib. */
        }
        goto ErrorExit;
    }

    if ( f_dwBase == 16 )
    {
        /* Capital HEX digit - A,B,C,D,E,F */
        if (    NATIVE_WCHAR( f_wch ) >= NATIVE_WCHAR(g_wchA)
             && NATIVE_WCHAR( f_wch ) <= NATIVE_WCHAR(g_wchF) )
        {
            DRM_WORD wVal = (DRM_WORD)( NATIVE_WCHAR( f_wch ) - NATIVE_WCHAR( g_wchA ) + 10 );
            if( wVal <= MAX_UNSIGNED_TYPE(DRM_BYTE) )
            {
                *f_pdwVal = (DRM_BYTE)wVal;
                bVal      = TRUE;
            }
            else
            {
                *f_pdwVal = MAX_UNSIGNED_TYPE( DRM_BYTE );  /* Overflow: set to max and don't set bVal = TRUE.  Don't use mathsafe which is in drmcore.lib. */
            }
            goto ErrorExit;
        }
        /* lower case hex digit - a,b,c,d,e,f */
        if (    NATIVE_WCHAR( f_wch ) >= NATIVE_WCHAR(g_wcha)
             && NATIVE_WCHAR( f_wch ) <= NATIVE_WCHAR(g_wchf)  )
        {
            DRM_WORD wVal = (DRM_WORD)( NATIVE_WCHAR( f_wch ) - NATIVE_WCHAR( g_wcha ) + 10 );
            if( wVal <= MAX_UNSIGNED_TYPE(DRM_BYTE) )
            {
                *f_pdwVal = (DRM_BYTE)wVal;
                bVal      = TRUE;
            }
            else
            {
                *f_pdwVal = MAX_UNSIGNED_TYPE( DRM_BYTE );  /* Overflow: set to max and don't set bVal = TRUE.  Don't use mathsafe which is in drmcore.lib. */
            }
            goto ErrorExit;
        }
    }

ErrorExit :
    return bVal;
}

/*****************************************************************************
** Function:    DRMCRT_AtoDWORD
**
** Synopsis:    Converts ASCII string with numeric value to DWORD.
**
** Arguments:   [f_pszStringInput]   : pointer to string with data
**              [f_cchStringInput]   : count of characters in string
**              [f_base]             : base for conversion. Should be 0, 10 or 16
**              [f_pdwValue]         : address of DWORD for converted value
**
** Note:        If f_base is 0, DRMCRT_AtoDWORD looks for "0x" or "0X" to determine
**              if the base is 10 or 16 automatically.
**
** Returns:     DRM_SUCCESS                  - on success
**              DRM_E_ARITHMETIC_OVERFLOW    - if converted value does not fit into DWORD.
**              DRM_S_FALSE                  - No error occurred but no conversion was performed.
******************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRMCRT_AtoDWORD(
    __in_ecount( f_cchStringInput ) const DRM_CHAR  *f_pszStringInput,
    __in                                  DRM_DWORD  f_cchStringInput,
    __in                                  DRM_DWORD  f_base,
    __out                                 DRM_DWORD *f_pdwValue )
{   DRM_RESULT dr           = DRM_SUCCESS;
    DRM_BYTE   bNextDigit   = 0;
    DRM_DWORD  dwIndex      = 0;
    DRM_BOOL   fNegative    = FALSE;

    ChkArg( f_pszStringInput != NULL );                        /* String must be present */
    ChkArg( f_cchStringInput >  0 );                           /* At least one character */
    ChkArg( f_base == 10 || f_base == 16 || f_base == 0 );     /* Convert only decimal or hex numbers (or figure it out) */
    ChkArg( f_pdwValue != NULL );                              /* Address for output varible must be present */

    /*
    ** See if a sign was specified
    */
    if ( f_cchStringInput > 1
      && ( f_pszStringInput[0] == '+' || f_pszStringInput[0] == '-' )    )
    {
        fNegative = ( f_pszStringInput[0] == '-' );

        f_pszStringInput += 1;
        f_cchStringInput -= 1;
    }

    /*
    ** If base is 16, then the string may start with 0x or 0X. Skip it.
    */
    if ( ( f_base == 16 || f_base == 0 )
      && f_cchStringInput > 2
      && f_pszStringInput[0] == '0'
      && ( f_pszStringInput[1] == 'x' || f_pszStringInput[1] == 'X' )    )
    {
        f_pszStringInput += 2;
        f_cchStringInput -= 2;

        /*
        ** If the given base was 0 and we found 0x or 0X, set the base to 16.
        */
        if ( f_base == 0 )
        {
            f_base = 16;
        }
    }

    /*
    ** If we get here and still have base 0, assume it's base 10.
    */
    if ( f_base == 0)
    {
        f_base = 10;
    }

    *f_pdwValue = 0;
    for( dwIndex = 0;
         dwIndex < f_cchStringInput
      && _DRMCRT_ASCIIToByteValue( f_pszStringInput[dwIndex], &bNextDigit )
      && f_base > bNextDigit;
         dwIndex++ )
    {
        DRM_DWORD dwNewValue = *f_pdwValue * f_base  +  bNextDigit;

        if ( ( dwNewValue - bNextDigit ) / f_base != *f_pdwValue )
        {
            ChkDR( DRM_E_ARITHMETIC_OVERFLOW );
        }

        /* Continue with next digit */
        *f_pdwValue = dwNewValue;
    }

    /*
    ** Perform the sign conversion
    */
    if( fNegative )
    {
        *f_pdwValue = ( DRM_DWORD ) ( -1 * ( signed ) *f_pdwValue );
    }

ErrorExit :
    if( DRM_SUCCEEDED( dr ) && dwIndex == 0 )
    {
        dr = DRM_S_FALSE;
    }
    return dr;
}

/*****************************************************************************
** Function:    DRMCRT_WtoDWORD
**
** Synopsis:    Converts WCHAR string with numeric value to DWORD.
**
** Arguments:   [f_pwszStringInput]  : pointer to string with data
**              [f_cchStringInput]   : count of characters in string
**              [f_dwBase]           : base for conversion. Should be 0, 10 or 16
**              [f_pdwValue]         : address of DWORD for converted value
**              [f_pcchValue]        : (optional) Returns the number of characters
**                                     consumed while parsing the string.
**
** Note:        If f_dwBase is 0, DRMCRT_WtoDWORD looks for "0x" or "0X" to determine
**              if the base is 10 or 16 automatically.
**
** Returns:     DRM_SUCCESS                  - on success
**              DRM_E_ARITHMETIC_OVERFLOW    - if converted value does not fit into DWORD.
**              DRM_S_FALSE                  - No error occurred but no conversion was performed.
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRMCRT_WtoDWORD(
    __in_ecount( f_cchStringInput ) const DRM_WCHAR *f_pwszStringInput,
    __in                                  DRM_DWORD  f_cchStringInput,
    __in                                  DRM_DWORD  f_dwBase,
    __out                                 DRM_DWORD *f_pdwValue,
    __out_opt                             DRM_DWORD *f_pcchValue )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_BYTE   bNextDigit   = 0;
    DRM_DWORD  dwIndex      = 0;
    DRM_DWORD  dwNewValue   = 0;
    DRM_BOOL   fNegative    = FALSE;
    DRM_DWORD  cchValue     = 0;

    ChkArg( f_pwszStringInput != NULL );                          /* String must be present */
    ChkArg( f_cchStringInput >  0 );                              /* At least one character */
    ChkArg( f_dwBase == 10 || f_dwBase == 16 || f_dwBase == 0 );  /* Convert only decimal or hex numbers */
    ChkArg( f_pdwValue  != NULL );                                /* Address for output varible must be present */
    if( f_pcchValue != NULL )
    {
        *f_pcchValue = 0;
    }

    /*
    ** See if a sign was specified
    */
    if ( f_cchStringInput > 1
      && ( f_pwszStringInput[0] == g_wchPlus || f_pwszStringInput[0] == g_wchMinus )    )
    {
        fNegative = ( f_pwszStringInput[0] == g_wchMinus );

        f_pwszStringInput += 1;
        f_cchStringInput -= 1;
        cchValue += 1;
    }

    /*
    ** If base is 16, then the string may start with 0x or 0X. Skip it.
    */
    if ( ( f_dwBase == 16 || f_dwBase == 0 )
      && f_cchStringInput > 2
      && f_pwszStringInput[0] == g_wch0
      && ( f_pwszStringInput[1] == g_wchx || f_pwszStringInput[1] == g_wchX )    )
    {
        f_pwszStringInput += 2;
        f_cchStringInput -= 2;
        cchValue += 2;
        if ( f_dwBase == 0 )
        {
            f_dwBase = 16;
        }
    }

    /*
    ** If we get here and still have base 0, assume it's base 10.
    */
    if ( f_dwBase == 0)
    {
        f_dwBase = 10;
    }

    *f_pdwValue = 0;
    for( dwIndex = 0;
         dwIndex < f_cchStringInput
      && _DRMCRT_WCHARToByteValue( f_pwszStringInput[dwIndex], f_dwBase, &bNextDigit )
      && f_dwBase > bNextDigit;
         dwIndex++ )
    {
        dwNewValue = *f_pdwValue * f_dwBase  +  bNextDigit;

        if ( ( dwNewValue - bNextDigit ) / f_dwBase != *f_pdwValue )
        {
            /*
            ** Must be overflow. Flag it for error.
            */
            dr = DRM_E_ARITHMETIC_OVERFLOW;

            /*
            ** Continue to process the integer to consume the rest of the digits.
            ** This ensures that *f_pcchValue includes all digits.
            */
        }

        /*
        ** Continue with next digit
        */
        *f_pdwValue = dwNewValue;
        cchValue += 1;
    }

    /*
    ** Fail if an overflow was detected.
    */
    ChkDR( dr );

    /*
    ** Perform the sign conversion
    */
    if( fNegative )
    {
        *f_pdwValue = ( DRM_DWORD ) ( -1 * ( signed ) *f_pdwValue );
    }

ErrorExit:
    if( DRM_SUCCEEDED( dr ) && dwIndex == 0 )
    {
        dr = DRM_S_FALSE;
    }
    if( f_pcchValue != NULL )
    {
        *f_pcchValue = cchValue;
    }
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRMCRT_strntol(
    __in_ecount( cchStringInput ) const DRM_CHAR *pszStringInput,
    __in                                DRM_DWORD cchStringInput,
    __out                               DRM_LONG *plValue )
{
    DRM_ANSI_CONST_STRING szString;
    DRM_DWORD lMaxVal    = ULONG_MAX / 10;
    DRM_DWORD lNextDigit = 0;
    DRM_DWORD lValue     = 0;
    DRM_BOOL fPositive   = TRUE;

    if( NULL == plValue ||
        NULL == pszStringInput ||
        0 == cchStringInput )
    {
        return DRM_E_INVALIDARG;
    }

    if( pszStringInput[0] == g_chMinus || pszStringInput[0] == g_chPlus )
    {
        if( pszStringInput[0] == g_chMinus )
        {
            fPositive = FALSE;
        }
        pszStringInput++;
        cchStringInput--;
    }

    szString.pszString = pszStringInput;
    szString.cchString = cchStringInput;
    *plValue = 0;

    while( szString.cchString > 0 && DRMCRT_isdigit( *( szString.pszString ) ) )
    {
        lNextDigit = (DRM_DWORD)( (*(szString.pszString) - g_ch0) );
        if( lValue < lMaxVal ||
            ( lValue == lMaxVal && lNextDigit <= ULONG_MAX % 10 ) )
        {
            lValue = lValue * 10 + lNextDigit;
        }
        else
        {
            return DRM_E_ARITHMETIC_OVERFLOW;
        }
        szString.pszString++;
        szString.cchString--;
    }

    if( lValue & 0x80000000 )
    {
        /* The high bit is set.  */
        if( lValue == 0x80000000 && !fPositive )
        {
            /* Special case */
            *plValue = (DRM_LONG)lValue;
        }
        else
        {
            /* This is fine for an unsigned long, but means we really have overflowed to return a signed long */
            return DRM_E_ARITHMETIC_OVERFLOW;
        }
    }
    else
    {
        *plValue = (DRM_LONG)lValue;
        if( !fPositive )
        {
           *plValue *= -1;
        }
    }

    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRMCRT_wcsntol(
    __in_ecount( cchStringInput ) const DRM_WCHAR *pwszStringInput,
    __in                                DRM_DWORD  cchStringInput,
    __out                               DRM_LONG  *plValue )
{
    DRM_CONST_STRING wszString;
    DRM_DWORD lMaxVal    = ULONG_MAX / 10;
    DRM_DWORD lNextDigit = 0;
    DRM_DWORD lValue     = 0;
    DRM_BOOL fPositive   = TRUE;

    if( NULL == plValue ||
        NULL == pwszStringInput ||
        0 == cchStringInput )
    {
        return DRM_E_INVALIDARG;
    }

    if( pwszStringInput[0] == g_wchMinus || pwszStringInput[0] == g_wchPlus )
    {
        if( pwszStringInput[0] == g_wchMinus )
        {
            fPositive = FALSE;
        }
        pwszStringInput++;
        cchStringInput--;
    }

    wszString.pwszString = pwszStringInput;
    wszString.cchString = cchStringInput;
    *plValue = 0;

    while( wszString.cchString > 0 && DRMCRT_iswdigit( *( wszString.pwszString ) ) )
    {

        lNextDigit = (DRM_DWORD)( (NATIVE_WCHAR(*(wszString.pwszString)) - NATIVE_WCHAR(g_wch0)) );
        if( lValue < lMaxVal ||
            ( lValue == lMaxVal && lNextDigit <= ULONG_MAX % 10 ) )
        {
            lValue = lValue * 10 + lNextDigit;
        }
        else
        {
            return DRM_E_ARITHMETIC_OVERFLOW;
        }
        wszString.pwszString++;
        wszString.cchString--;
    }

    if( lValue & 0x80000000 )
    {
        /* The high bit is set.  */
        if( lValue == 0x80000000 && !fPositive )
        {
            /* Special case */
            *plValue = (DRM_LONG)lValue;
        }
        else
        {
            /* This is fine for an unsigned long, but means we really have overflowed to return a signed long */
            return DRM_E_ARITHMETIC_OVERFLOW;
        }
    }
    else
    {
        *plValue = (DRM_LONG)lValue;
        if( !fPositive )
        {
           *plValue *= -1;
        }
    }

    return DRM_SUCCESS;
}

DRM_API DRM_CHAR* DRM_CALL DRMCRT_strnstr(
   __in_ecount( cchMaxStr )       const DRM_CHAR  *pszStr,
   __in                                 DRM_DWORD  cchMaxStr,
   __in_ecount( cchMaxStrSearch ) const DRM_CHAR  *pszStrSearch,
   __in                                 DRM_DWORD  cchMaxStrSearch
)
{
    DRM_CHAR *pszRetVal     = NULL;
    DRM_DWORD i              = 0;

    /* Check Arguments */
    if ( pszStr == NULL  ||
         pszStrSearch == NULL ||
         cchMaxStr == 0 ||
         cchMaxStrSearch == 0 )
    {
        goto ErrorExit;
    }

    /* Get the length of the string to search for */
    while( i < cchMaxStrSearch )
    {
        if( pszStrSearch[i] == g_chNull )
        {
            break;
        }
        i++;
    }
    cchMaxStrSearch = i;


    /* Now search */
    for ( ; cchMaxStrSearch <= cchMaxStr; pszStr++, cchMaxStr-- )
    {
       if ( 0 == DRMCRT_strncmp( pszStrSearch, pszStr, cchMaxStrSearch ) )
       {
           pszRetVal = (DRM_CHAR*)pszStr;
           break;
       }
    }

ErrorExit :
    return pszRetVal;
}

DRM_API DRM_WCHAR* DRM_CALL DRMCRT_wcsnstr(
   __in_ecount( cchMaxStr )       const DRM_WCHAR *pwszStr,
   __in                                 DRM_DWORD  cchMaxStr,
   __in_ecount( cchMaxStrSearch ) const DRM_WCHAR *pwszStrSearch,
   __in                                 DRM_DWORD  cchMaxStrSearch )
{
    DRM_WCHAR *pwszRetVal    = NULL;
    DRM_DWORD i              = 0;

    /* Check Arguments */
    if ( pwszStr == NULL  ||
         pwszStrSearch == NULL ||
         cchMaxStr == 0 ||
         cchMaxStrSearch == 0 )
    {
        goto ErrorExit;
    }

    /* Get the length of the string to search for */
    while( i < cchMaxStrSearch )
    {
        if( pwszStrSearch[i] == g_wchNull )
        {
            break;
        }
        i++;
    }
    cchMaxStrSearch = i;


    /* Now search */
    for ( ; cchMaxStrSearch <= cchMaxStr; pwszStr++, cchMaxStr-- )
    {
       if ( 0 == DRMCRT_wcsncmp( pwszStrSearch, pwszStr, cchMaxStrSearch ) )
       {
           pwszRetVal = (DRM_WCHAR*)pwszStr;
           break;
       }
    }

ErrorExit :
    return pwszRetVal;
}

EXIT_PK_NAMESPACE_CODE;
