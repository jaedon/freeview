/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmutilitieslite.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DateLessThan(
    IN const DRMFILETIME* f_pFileTime1,
    IN const DRMFILETIME* f_pFileTime2)
{
    DRM_UINT64 u641;
    DRM_UINT64 u642;

    FILETIME_TO_UI64( *f_pFileTime1, u641 );
    FILETIME_TO_UI64( *f_pFileTime2, u642 );
    return DRM_UI64Les( u641, u642);
}


static DRM_NO_INLINE DRM_BOOL HexStringToDword(
    IN const DRM_WCHAR *f_pwsz,
       OUT   DRM_DWORD *f_pdwValue,
    IN       DRM_LONG   f_cDigits)
{
    DRM_LONG iDigit;
    DRM_BOOL fOK = TRUE;
    DRM_DWORD dwValue = 0;

    *f_pdwValue = 0;

    for (iDigit = 0;
         iDigit < f_cDigits;
         iDigit++)
    {
        dwValue <<= 4;
        if( NATIVE_WCHAR(f_pwsz[iDigit]) >= NATIVE_WCHAR(g_wch0)
         && NATIVE_WCHAR(f_pwsz[iDigit]) <= NATIVE_WCHAR(g_wch9))
        {
            dwValue += NATIVE_WCHAR(f_pwsz[iDigit]) - NATIVE_WCHAR(g_wch0);
        }
        else if( NATIVE_WCHAR(f_pwsz[iDigit]) >= NATIVE_WCHAR(g_wchA)
              && NATIVE_WCHAR(f_pwsz[iDigit]) <= NATIVE_WCHAR(g_wchF) )
        {

            dwValue += NATIVE_WCHAR(f_pwsz[iDigit]) - NATIVE_WCHAR(g_wchA) + 10;
        }
        else if( NATIVE_WCHAR(f_pwsz[iDigit]) >= NATIVE_WCHAR(g_wcha)
              && NATIVE_WCHAR(f_pwsz[iDigit]) <= NATIVE_WCHAR(g_wchf) )
        {
            dwValue += NATIVE_WCHAR(f_pwsz[iDigit]) - NATIVE_WCHAR(g_wcha) + 10;
        }
        else
        {
            /* Reverse the shift operation if all the range checks failed */
            dwValue >>= 4;
            fOK = FALSE;
        }
    }

    MEMCPY( f_pdwValue, &dwValue, SIZEOF( dwValue ) );

    return fOK;
}

/*****************************************************************************
** Function: DRM_UTL_GetVersionAsString
**
** Synopsis: express n version binary as a base10 dotted UNICODE string.
**
** Arguments:
** [f_rgbVersion]   -- version as binary
** [f_wszVerString] -- output buffer
**
** Returns string length
**
** Notes:    Output is NUL-terminated
*****************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_UTL_GetVersionAsString(
    __in_bcount( VERSION_LEN )         const DRM_BYTE  f_rgbVersion   [__CB_DECL(VERSION_LEN)],
    __out_ecount( VER_STRING_MAX_LEN )       DRM_WCHAR f_wszVerString [VER_STRING_MAX_LEN] )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_LONG   ich     = 0;
    DRM_LONG   ib      = 0;
    DRM_DWORD  cchTemp = 0;

    /*
    ** (VER_STRING_MAX_LEN - 4) / VERSION_LEN :
    ** This is 4 characters for the 3 periods and the NULL terminator.
    ** We divide by VERSION_LEN because we have to perform the operation VERSION_LEN times
    ** and give an equal buffer size to each operation
    **
    ** ich <= (VER_STRING_MAX_LEN-(VER_STRING_MAX_LEN/VERSION_LEN)) :
    ** This is to ensure that there is enough room for the last segment of the version
    */

    for (ib = 0; (ib < VERSION_LEN) && (ich <= (VER_STRING_MAX_LEN-(VER_STRING_MAX_LEN/VERSION_LEN))); ib++)
    {
        ChkDR( DRM_UTL_NumberToString( GET_BYTE(f_rgbVersion, ib),
                                       &f_wszVerString[ich],
                                       (VER_STRING_MAX_LEN - 4) / VERSION_LEN + 1,
                                       0, 10, &cchTemp) );
        ich += cchTemp;

        if (ich < VER_STRING_MAX_LEN)
        {
            f_wszVerString [ich++] = g_wchPeriod;
        }
        else
        {
            ich = VER_STRING_MAX_LEN;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
    }

    /* Check to see if we fell out of the loop because of the second loop condition */
    if ((ib < VERSION_LEN) && (ich > (VER_STRING_MAX_LEN-(VER_STRING_MAX_LEN/VERSION_LEN))))
    {
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    /* Overwrite that last g_wchPeriod with the NULL terminator */
    f_wszVerString [ich-1] = g_wchNull;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRM_UTL_GetVersionFromStringAsDWORD
**
** Synopsis:    Parses the version string which is in the format of A.B.C.D where
**              each letter represents a byte of data.  The dot separated numbers
**              are converted into a DWORD where A is the highest order bits, B is
**              the next highest order bits, and so on.
**
** Arguments:   f_pwszVersion  - input string in A.B.C.D format
**              f_cchVersion   - length of f_pwszVersion string
**              f_pdwVersion   - return value to store the constructed DWORD in.
**
** Returns string length
**
** Notes:    Output is NULL-terminated
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_GetVersionFromStringAsDWORD(
   __in_ecount( f_cchVersion ) const DRM_WCHAR  *f_pwszVersion,
   __in                              DRM_DWORD   f_cchVersion,
   __inout                           DRM_DWORD  *f_pdwVersion )
{
          DRM_RESULT dr                = DRM_SUCCESS;
          DRM_DWORD  dwIndex           = 0;
          DRM_DWORD  dwCurrent         = 0;
          DRM_DWORD  dwShiftAmount     = 24;
    const DRM_WCHAR* pwszStart         = NULL;
    const DRM_WCHAR* pwszEnd           = NULL;
          DRM_DWORD  cchRemaining      = 0;
          DRM_DWORD  cchFromStartToEnd = 0;

    ChkArg( f_pwszVersion != NULL );
    ChkArg( f_pdwVersion  != NULL );

    /*
    **  The version string is in the format of A.B.C.D where
    **  each letter represents a byte of data.  The dot separted numbers
    **  are converted into a DWORD where A is the highest order bits, B is
    **  the next highest order bits, and so on.
    */
    *f_pdwVersion = 0;
    pwszStart = f_pwszVersion;
    cchRemaining = f_cchVersion;

    while ((dwIndex < 4) && (cchRemaining > 0))
    {
        pwszEnd = DRMCRT_wcsnstr( pwszStart,
                                  cchRemaining,
                                  &g_wchPeriod,
                                  1 );

        if (pwszEnd == NULL)
        {
            cchFromStartToEnd = cchRemaining;
        }
        else
        {
            cchFromStartToEnd = ( pwszEnd - pwszStart );
        }

        ChkDR( DRMCRT_wcsntol( pwszStart,
                               cchFromStartToEnd,
                               (DRM_LONG*)&dwCurrent ) );

        if (dwCurrent <= 255)
        {
            *f_pdwVersion = ( *f_pdwVersion + ( dwCurrent << dwShiftAmount ) );
        }
        else
        {
            ChkDR( DRM_E_INVALIDARG );
        }

        dwShiftAmount = dwShiftAmount - 8;

        if (pwszEnd == NULL)
        {
            cchRemaining = 0;
        }
        else
        {
            dwIndex++;
            pwszStart = ++pwszEnd;
            cchRemaining = ( f_cchVersion - ( pwszStart - f_pwszVersion ) );
        }
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRM_UTL_GetVersionFromStringAsDWORDA
**
** Synopsis:    Parses the version string which is in the format of A.B.C.D where
**              each letter represents a byte of data.  The dot separated numbers
**              are converted into a DWORD where A is the highest order bits, B is
**              the next highest order bits, and so on.
**
** Arguments:   f_pszVersion   - input string in A.B.C.D format
**              f_cchVersion   - length of f_pwszVersion string
**              f_pdwVersion   - return value to store the constructed DWORD in.
**
** Returns string length
**
** Notes:    Output is NULL-terminated
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_GetVersionFromStringAsDWORDA(
   __in_ecount( f_cchVersion ) const DRM_CHAR  *f_pszVersion,
   __in                              DRM_DWORD  f_cchVersion,
   __inout                           DRM_DWORD *f_pdwVersion )
{
          DRM_RESULT dr                = DRM_SUCCESS;
          DRM_DWORD  dwIndex           = 0;
          DRM_DWORD  dwCurrent         = 0;
          DRM_DWORD  dwShiftAmount     = 24;
    const DRM_CHAR* pszStart           = NULL;
    const DRM_CHAR* pszEnd             = NULL;
          DRM_DWORD  cchRemaining      = 0;
          DRM_DWORD  cchFromStartToEnd = 0;

    ChkArg( f_pszVersion != NULL );
    ChkArg( f_pdwVersion  != NULL );

    /*
    **  The version string is in the format of A.B.C.D where
    **  each letter represents a byte of data.  The dot separted numbers
    **  are converted into a DWORD where A is the highest order bits, B is
    **  the next highest order bits, and so on.
    */
    *f_pdwVersion = 0;
    pszStart = f_pszVersion;
    cchRemaining = f_cchVersion;

    while ((dwIndex < 4) && (cchRemaining > 0))
    {
        pszEnd = DRMCRT_strnstr( pszStart,
                                 cchRemaining,
                                 &g_chPeriod,
                                 1 );

        if (pszEnd == NULL)
        {
            cchFromStartToEnd = cchRemaining;
        }
        else
        {
            cchFromStartToEnd = ( pszEnd - pszStart );
        }

        ChkDR( DRMCRT_strntol( pszStart,
                               cchFromStartToEnd,
                               (DRM_LONG*)&dwCurrent ) );

        if (dwCurrent <= 255)
        {
            *f_pdwVersion = ( *f_pdwVersion + ( dwCurrent << dwShiftAmount ) );
        }
        else
        {
            ChkDR( DRM_E_INVALIDARG );
        }

        dwShiftAmount = dwShiftAmount - 8;

        if (pszEnd == NULL)
        {
            cchRemaining = 0;
        }
        else
        {
            dwIndex++;
            pszStart = ++pszEnd;
            cchRemaining = ( f_cchVersion - ( pszStart - f_pszVersion ) );
        }
    }

ErrorExit:
    return dr;
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
        Date.wYear = ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString  )) - NATIVE_WCHAR(g_wch0)) * 1000 +
                     ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+1)) - NATIVE_WCHAR(g_wch0)) * 100 +
                     ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+2)) - NATIVE_WCHAR(g_wch0)) * 10 +
                     ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+3)) - NATIVE_WCHAR(g_wch0));
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
        Date.wMonth = ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString  )) - NATIVE_WCHAR(g_wch0) ) * 10 +
                      ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+1)) - NATIVE_WCHAR(g_wch0) );
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
        Date.wDay = ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString  )) - NATIVE_WCHAR(g_wch0) ) * 10 +
                    ( NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+1)) - NATIVE_WCHAR(g_wch0) );
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
            Date.wHour =  (NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString  )) - NATIVE_WCHAR(g_wch0)) * 10
                         +(NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+1)) - NATIVE_WCHAR(g_wch0));
            f_pdstrZuluTime->pwszString += 2;
            f_pdstrZuluTime->cchString -= 2;
            /* We look for :MM only if we got HH */
            if ( f_pdstrZuluTime->cchString >3 &&
                *(f_pdstrZuluTime->pwszString) == g_wchColon &&
                DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+1)) &&
                DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+2)) )
            {
                Date.wMinute = (NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+1)) - NATIVE_WCHAR(g_wch0)) * 10
                             + (NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+2)) - NATIVE_WCHAR(g_wch0));
                f_pdstrZuluTime->pwszString += 3;
                f_pdstrZuluTime->cchString -=3;
                /* We look for :SS only if we got MM */
                if (f_pdstrZuluTime->cchString >3 &&
                    *(f_pdstrZuluTime->pwszString) == g_wchColon &&
                    DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+1)) &&
                    DRMCRT_iswdigit(*(f_pdstrZuluTime->pwszString+2)) )
                {
                    Date.wSecond = (NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+1)) - NATIVE_WCHAR(g_wch0)) * 10
                                 + (NATIVE_WCHAR(*(f_pdstrZuluTime->pwszString+2)) - NATIVE_WCHAR(g_wch0));
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

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_GetVersionFromString(
   __in_ecount( cchVersion ) const DRM_WCHAR  *pwszVersion,
   IN       DRM_DWORD   cchVersion,
   __out_ecount( VERSION_LEN ) DRM_WORD rgwVersion[VERSION_LEN] )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  uVer[VERSION_LEN];
    DRM_DWORD  iCount = 0;
    DRM_DWORD  ich = 0;

    for( iCount = 0; iCount < VERSION_LEN - 1; iCount++ )
    {
        ich = 0;
        while( ich < cchVersion )
        {
            if( pwszVersion[ich] == g_wchPeriod )
            {
                break;
            }
            ich++;
        }

        if ( ich == cchVersion)
        {
            ChkDR( CPRMEXP_INVALID_ARGUMENT );
        }

        if( pwszVersion[ich] != g_wchPeriod                                        /* Didn't find a . seperator */
         || DRM_FAILED( DRMCRT_wcsntol( pwszVersion, ich, (DRM_LONG *) &uVer [iCount] ) ) /* Couldn't extract a number */
         || uVer[iCount]     != (DRM_WORD) uVer[iCount] )                          /* Not in a valid range      */
        {
            ChkDR( CPRMEXP_INVALID_ARGUMENT );
        }

        cchVersion -= (ich+1);
        pwszVersion = pwszVersion + (ich + 1);
    }

    if( DRM_FAILED( DRMCRT_wcsntol( pwszVersion, cchVersion, (DRM_LONG *) &uVer [iCount] ) ) /* Couldn't extract a number */
     || uVer[iCount] != (DRM_WORD) uVer[iCount] )                                     /* Not in a valid range      */
    {
        ChkDR( CPRMEXP_INVALID_ARGUMENT );
    }

    for( iCount = 0; iCount < VERSION_LEN; iCount++ )
    {
        rgwVersion[iCount] = (DRM_WORD)uVer[iCount];
    }

ErrorExit:

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_StringToGuid(
    IN const DRM_CONST_STRING *pdstrString,
    OUT      DRM_GUID         *pGuid)
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  dwValue = 0;
    DRM_LONG ib;

    ChkArg(pdstrString != NULL
        && pGuid       != NULL
        && pdstrString->pwszString != NULL
        && pdstrString->cchString  == DRM_GUID_STRING_LEN); /* 38 characters in a proper Guid String - including { } */

    ChkArg(pdstrString->pwszString [0]  == g_wchOpenCurly
        && pdstrString->pwszString [37] == g_wchCloseCurly); /* First and last char must be { & } */

    /* Validate the delimitar characters */

    if (pdstrString->pwszString [9]  != g_wchMinus
     || pdstrString->pwszString [14] != g_wchMinus
     || pdstrString->pwszString [19] != g_wchMinus
     || pdstrString->pwszString [24] != g_wchMinus)
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    /* Convert pieces to BYTES */

    if (! HexStringToDword(pdstrString->pwszString + 1, &pGuid->Data1, 8))
    {
        ChkDR(DRM_E_INVALIDARG);
    }
    if (! HexStringToDword(pdstrString->pwszString + 10, &dwValue, 4))
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    pGuid->Data2 = (DRM_SHORT) dwValue;

    if (! HexStringToDword(pdstrString->pwszString + 15, &dwValue, 4))
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    pGuid->Data3 = (DRM_SHORT) dwValue;

    if (! HexStringToDword(pdstrString->pwszString + 20, &dwValue, 2))
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    PUT_BYTE(pGuid->Data4, 0, (DRM_BYTE) dwValue);

    if (! HexStringToDword(pdstrString->pwszString + 22, &dwValue, 2))
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    PUT_BYTE(pGuid->Data4, 1, (DRM_BYTE) dwValue);

    for (ib = 2; ib < 8; ib++)
    {
        if (! HexStringToDword(pdstrString->pwszString  + (((ib - 1) * 2) + 23), &dwValue, 2))
        {
            ChkDR(DRM_E_INVALIDARG);
        }

        PUT_BYTE(pGuid->Data4,ib, (DRM_BYTE) dwValue);
    }

    FIX_ENDIAN_DWORD( pGuid->Data1 );
    FIX_ENDIAN_WORD(  pGuid->Data2 );
    FIX_ENDIAN_WORD(  pGuid->Data3 );

    /* We have validated the string && now the bits are converted. */
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DASSSTRStringsEqual(
    __in_ecount( f_pdasstr0->m_ich + f_pdasstr0->m_cch ) const DRM_CHAR *f_pszBase0,
    const DRM_SUBSTRING *f_pdasstr0,
    __in_ecount( f_pdasstr1->m_ich + f_pdasstr1->m_cch ) const DRM_CHAR *f_pszBase1,
    const DRM_SUBSTRING *f_pdasstr1)
{
    return (f_pdasstr0->m_cch == f_pdasstr1->m_cch
        &&  DRM_BYT_CompareBytes(f_pszBase0,
                                 f_pdasstr0->m_ich,
                                 f_pszBase1,
                                 f_pdasstr1->m_ich,
                                 f_pdasstr1->m_cch) == 0);
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_GuidToString
(
    IN                                    const  DRM_GUID  *pGuid,
        __out_ecount(DRM_GUID_STRING_LEN)        DRM_WCHAR  rgwszString[DRM_GUID_STRING_LEN]
)
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  dwCount   = 0;
    DRM_DWORD  cchString = 0;
    /* OACR complains if datais written directly to rgwszString.
    ** Need local string of larger size
    */
    DRM_WCHAR  rgwszLocalString[DRM_GUID_STRING_LEN + 1] = {0};

    ChkArg(  pGuid != NULL && rgwszString != NULL );


    /* Buffer is large enough, we put the data.*/
    rgwszLocalString[cchString++] = g_wchOpenCurly;

    /* First DWORD */
    ChkDR( DRM_UTL_NumberToString( pGuid->Data1, rgwszLocalString + cchString, NO_OF(rgwszLocalString) - cchString, DRM_MAX_CCH_BASE16_DWORD_STRING, 16, NULL ) );
    cchString += 8;
    /* - */
    rgwszLocalString[cchString] = g_wchMinus;
    cchString += 1;

    /* WORD 2 */
    ChkDR( DRM_UTL_NumberToString( pGuid->Data2, rgwszLocalString + cchString, NO_OF(rgwszLocalString) - cchString, DRM_MAX_CCH_BASE16_WORD_STRING, 16, NULL ) );
    cchString += 4;
    /* - */
    rgwszLocalString[cchString] = g_wchMinus;
    cchString += 1;

    /* WORD 3 */
    ChkDR( DRM_UTL_NumberToString( pGuid->Data3, rgwszLocalString + cchString, NO_OF(rgwszLocalString) - cchString, DRM_MAX_CCH_BASE16_WORD_STRING, 16, NULL ) );
    cchString += 4;
    /* - */
    rgwszLocalString[cchString] = g_wchMinus;
    cchString += 1;

    /* byte array 2 */
    for ( dwCount = 0;  dwCount < 2;  dwCount++ )
    {
        DRM_BYTE bVal = GET_BYTE( pGuid->Data4, dwCount );
        ChkDR( DRM_UTL_NumberToString( bVal, rgwszLocalString + cchString, NO_OF(rgwszLocalString) - cchString, 2, 16, NULL ) );
        cchString += 2;
    }
    /* - */
    rgwszLocalString[cchString] = g_wchMinus;
    cchString += 1;

    /* byte array 6 */
    for ( dwCount = 2;  dwCount < 8;  dwCount++ )
    {
        DRM_BYTE bVal = GET_BYTE( pGuid->Data4, dwCount );
        ChkDR( DRM_UTL_NumberToString( bVal, rgwszLocalString + cchString, NO_OF(rgwszLocalString) - cchString, DRM_MAX_CCH_BASE16_BYTE_STRING, 16, NULL ) );
        cchString += 2;
    }

    /* } */
    rgwszLocalString[cchString] = g_wchCloseCurly;
    cchString += 1;

    /* Copy data to original string
    */
    MEMCPY( rgwszString, rgwszLocalString, cchString * SIZEOF( DRM_WCHAR ) );

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_StringToNumber(
    __in      DRM_CONST_STRING *pdstrString,
    __out     DRM_LONG         *plValue,
    __out_opt DRM_DWORD        *pcchValue )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_DWORD   dwBase       = 10;
    DRM_DWORD   dwValue      = 0;

    ChkArg( pdstrString != NULL );
    ChkArg( plValue     != NULL );

    if( ( pdstrString->cchString > 1 )
     && ( pdstrString->pwszString[0] == g_wch0 )
     && ( ( pdstrString->pwszString[1] == g_wchx )
       || ( pdstrString->pwszString[1] == g_wchX ) ) )
    {
        dwBase = 16;
    }

    dr = DRMCRT_WtoDWORD(
        pdstrString->pwszString,
        pdstrString->cchString,
        dwBase,
        &dwValue,
        pcchValue );
    if( dr == DRM_S_FALSE )
    {
        /*
        ** If DRMCRT_WtoDWORD doesn't find any digits, then it returns DRM_S_FALSE.
        ** Map it to DRM_E_INVALIDARG because we want this function to fail in that case.
        */
        dr = DRM_E_INVALIDARG;
    }
    ChkDR( dr );
    *plValue = (DRM_LONG)( dwValue );

    /*
    ** DRMCRT_WtoDWORD() performed an unsigned conversion.  Now
    ** that the value has been cast back to a signed long make sure
    ** that the sign is correct.  If not, return an overflow error
    **/
    if ( pdstrString->pwszString[0] == g_wchMinus )
    {
        if ( *plValue > 0 )
        {
            ChkDR( CPRMEXP_OVERFLOW );
        }
    }
    else
    {
        if ( *plValue < 0 )
        {
            ChkDR( CPRMEXP_OVERFLOW );
        }
    }


ErrorExit:
    return dr;
}

static DRM_CHAR _ByteValueToASCII( DRM_BYTE f_bVal )
{
    DRM_CHAR chReturnValue = '?';

    /* Do not have valid ASCII representation for values more that 36. Return '?' as default */
    if ( f_bVal >= 36 )
    {
        goto ErrorExit;
    }

    /* Base 10 digit */
    if ( f_bVal < 10 )
    {
        chReturnValue = '0' + f_bVal;
    }
    else
    {
        /* Hex Symbol */
        chReturnValue = 'A' - 10 + f_bVal;
    }

ErrorExit :
    return chReturnValue;
}

/*****************************************************************************
** Function: DRM_UTL_NumberToString
**
** Synopsis: express an unsigned long as a UNICODE string.
**
** Arguments:
** [f_dwValue]      -- value to translate
** [f_pwszBuffer]   -- output buffer
** [f_cchBuffer]    -- output buffer size
** [f_dwPadZeros]   -- pad the buffer with 0s to specified length, or 0 for no padding
** [f_dwRadix]      -- use specified radix value, 10 for decimal
** [*f_pcchRLength] -- optional pointer to DWORD containing output size,
**                     if the value is NULL then result size is not returned
**
** Notes:    - Output is NUL-terminated
**           - Padding is from left side. Example: 1 -> 001, 0 -> 00
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_NumberToString(
    __in                          DRM_DWORD  f_dwValue,
    __out_ecount_z( f_cchBuffer ) DRM_WCHAR *f_pwszBuffer,
    __in                          DRM_DWORD  f_cchBuffer,
    __in                          DRM_DWORD  f_dwPadZeros,
    __in                          DRM_DWORD  f_dwRadix,
    __out_opt                     DRM_DWORD *f_pcchRLength )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_WCHAR *pwszBuffer = f_pwszBuffer + (f_cchBuffer  - 1);   /* last digit at end of buffer */
    DRM_DWORD  cchOut     = 0;

    /* Check that the input buffer exists, large enough for data and NUL-terminator, and radix is valid */
    ChkArg( f_dwRadix > 1 && f_dwRadix < 36 );
    ChkArg( f_dwPadZeros < f_cchBuffer );
    ChkArg( f_pwszBuffer != NULL && f_cchBuffer > 1 );

    if ( f_pcchRLength != NULL )
    {
        *f_pcchRLength = 0;
    }

    /* Set NUL-terminators */
    *f_pwszBuffer = g_wchNull;
    *pwszBuffer-- = g_wchNull;
    cchOut++;

    if( f_dwValue == 0 )
    {
        *pwszBuffer-- = g_wch0;
        cchOut++;
    }
    else
    {
        while( f_dwValue != 0 )
        {
            if ( pwszBuffer < f_pwszBuffer )
            {
               ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            /* store the digit */
            *pwszBuffer-- = WCHAR_CAST(_ByteValueToASCII((DRM_BYTE)(f_dwValue % f_dwRadix)));
            cchOut++;

            /* reduce number */
            f_dwValue /= f_dwRadix;
        }
    }

    if( f_dwPadZeros > 0 )
    {
        while( pwszBuffer >= f_pwszBuffer && cchOut <= f_dwPadZeros )
        {
            /* add zeros before the number*/
            *pwszBuffer-- = g_wch0;
            cchOut++;
        }
    }

    if ( f_pcchRLength != NULL )
    {
        /* Exclude NUL-terminator */
        *f_pcchRLength = (cchOut - 1);
    }

    /* Shift string to begin offset */
    DRM_BYT_MoveBytes(f_pwszBuffer, 0, pwszBuffer, 1, cchOut * SIZEOF (DRM_WCHAR));

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRM_UTL_NumberToStringA
**
** Synopsis: express an unsigned long as an ANSI string.
**
** Arguments:
** [f_dwValue]      -- value to translate
** [f_pszBuffer]    -- output buffer
** [f_cchBuffer]    -- output buffer size
** [f_dwPadZeros]   -- pad the buffer with 0s to specified length, or 0 for no padding
** [f_dwRadix]      -- use specified radix value, 10 for decimal
** [*f_pcchRLength] -- optional pointer to DWORD containing output size,
**                     if the value is NULL then result size is not returned
**
** Notes:    - Output is NUL-terminated
**           - Padding is from left side. Example: 1 -> 001, 0 -> 00
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_NumberToStringA(
    __in                          DRM_DWORD  f_dwValue,
    __out_ecount_z( f_cchBuffer ) DRM_CHAR  *f_pszBuffer,
    __in                          DRM_DWORD  f_cchBuffer,
    __in                          DRM_DWORD  f_dwPadZeros,
    __in                          DRM_DWORD  f_dwRadix,
    __out_opt                     DRM_DWORD *f_pcchRLength )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_CHAR  *pszBuffer  = f_pszBuffer + (f_cchBuffer  - 1);   /* last digit at end of buffer */
    DRM_DWORD  cchOut     = 0;

    /* Check that the input buffer exists, large enough for data and NUL-terminator, and radix is valid */
    ChkArg( f_dwRadix > 1 && f_dwRadix < 36 );
    ChkArg( f_dwPadZeros < f_cchBuffer );
    ChkArg( f_pszBuffer != NULL && f_cchBuffer > 1 );

    if ( f_pcchRLength != NULL )
    {
        *f_pcchRLength = 0;
    }

    /* Set NUL-terminators */
    *f_pszBuffer = '\0';
    *pszBuffer-- = '\0';
    cchOut++;

    if( f_dwValue == 0 )
    {
        *pszBuffer-- = '0';
        cchOut++;
    }
    else
    {
        while( f_dwValue != 0 )
        {
            if ( pszBuffer < f_pszBuffer )
            {
               ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            /* store the digit */
            *pszBuffer-- = _ByteValueToASCII((DRM_BYTE)(f_dwValue % f_dwRadix));
            cchOut++;

            /* reduce number */
            f_dwValue /= f_dwRadix;
        }
    }

    if( f_dwPadZeros > 0 )
    {
        while( pszBuffer >= f_pszBuffer && cchOut <= f_dwPadZeros )
        {
            /* add zeros before the number*/
            *pszBuffer-- = '0';
            cchOut++;
        }
    }

    if ( f_pcchRLength != NULL )
    {
        /* Exclude NUL-terminator */
        *f_pcchRLength = (cchOut - 1);
    }

    /* Shift string to begin offset */
    DRM_BYT_MoveBytes(f_pszBuffer, 0, pszBuffer, 1, cchOut);

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    PKEncryptLarge
**
** Synopsis:    Encrypt large amoutn of data (using PKEncrypt with large data is slow, this is fast)
**
** Arguments:   [f_ppubkey]        -- Caller supplied public key to encrypt with
**              [f_pbClear]        -- Array of bytes to encrypt
**              [f_cbClear]        -- Length ob f_pbClear in bytes
**              [f_pbCipher]       -- Buffer to hold encrypted data
**              [f_cbKeySymmetric] -- Desired length of internal symmertic key to be created
**              [f_pcontextCRYP]   -- A DRM_CRYPTO_CONTEXT so PKCrypto operations can be performed.
**
** Notes:       out must be of length f_cbClear + DRM_ECC160_CIPHERTEXT_LEN
**              Internally a rc4 symmetric key will be created to encrypt the content (because it's fast) &
**              that symmertic key will be encrypted with the PUBKEY.
**              In place encryption is possible if f_pbCipher equals to f_pbClear.
***********************************************************************/


/**********************************************************************
** Function:    DRM_UTL_StringInsertBlankSubString
**
** Synopsis:    Insert blanks to a target string at specific insertion point
**
** Arguments:   [f_pdstrTarget]  -- target string
**              [f_ichInsertion] -- insertion position of target string: index of f_pwsz.
**              [f_cch]          -- # of blank char to insert
**
** Notes:
** - Caller is responsible to make sure there is enough room in target string.
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_StringInsertBlankSubString(
    IN OUT DRM_STRING *f_pdstrTarget,
    IN     DRM_DWORD   f_ichInsertion,
    IN     DRM_DWORD   f_cch)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchToMove     = 0;
    DRM_DWORD  ich           = 0;

    ChkDRMString(f_pdstrTarget);
    ChkArg(f_ichInsertion <= f_pdstrTarget->cchString);

    cchToMove = f_pdstrTarget->cchString - f_ichInsertion;

    MEMMOVE(&f_pdstrTarget->pwszString[f_ichInsertion+f_cch],
            &f_pdstrTarget->pwszString[f_ichInsertion],
            cchToMove*SIZEOF(DRM_WCHAR));

    /* fill the hole in the blanks */
    for (ich=0; ich<f_cch; ich++)
    {
        f_pdstrTarget->pwszString[f_ichInsertion+ich] = g_wchSpace;
    }

    /* adjust destination string length */
    f_pdstrTarget->cchString += f_cch;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    DRM_UTL_StringInsertBlankSubStringA
**
** Synopsis:    Insert blanks to a target string at specific insertion point.
**
** Arguments:
**
** [f_pdastrTarget]              -- Pointer to a target DRM ANSI string.
** [f_ichInsertion]              -- Insertion position of target string.
** [f_cch]                       -- Number of blank characters to insert.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Notes:                   Caller is responsible to make sure there is enough
**                          room in target string.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_StringInsertBlankSubStringA(
    IN OUT DRM_ANSI_STRING *f_pdastrTarget,
    IN DRM_DWORD f_ichInsertion,
    IN DRM_DWORD f_cch )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchToMove = 0;
    DRM_DWORD ich = 0;

    ChkDRMANSIString( f_pdastrTarget );
    ChkArg( f_ichInsertion <= f_pdastrTarget->cchString );

    cchToMove = f_pdastrTarget->cchString - f_ichInsertion;

    MEMMOVE( &f_pdastrTarget->pszString[ f_ichInsertion + f_cch ],
             &f_pdastrTarget->pszString[ f_ichInsertion ],
             cchToMove );

    /* Fill the hole with blanks. */
    for ( ich = 0; ich < f_cch; ich++ )
    {
        PUT_CHAR( f_pdastrTarget->pszString, f_ichInsertion + ich, ' ' );
    }

    /* Adjust the length of the target string. */
    f_pdastrTarget->cchString += f_cch;

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_UTL_StringInsertSubString
**
** Synopsis:    Insert a substring to a target string at specific insertion point
**
** Arguments:   [f_pdstrTarget]  -- target string
**              [f_ichInsertion] -- insertion position of target string: index of f_pwsz.
**              [f_pdstrSub]     -- sub string to be inserted
**
** Notes:
** - Caller is responsible to make sure there is enough room in target string.
***********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_UTL_StringInsertSubString(
    IN OUT   DRM_STRING       *f_pdstrTarget,
    IN       DRM_DWORD         f_ichInsertion,
    IN const DRM_CONST_STRING *f_pdstrSub)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDRMString(f_pdstrTarget);
    ChkDRMString(f_pdstrSub);
    ChkArg(f_ichInsertion <= f_pdstrTarget->cchString);

    ChkDR(DRM_UTL_StringInsertBlankSubString(f_pdstrTarget, f_ichInsertion, f_pdstrSub->cchString));

    /* copy the source string to target point */
    MEMCPY( (DRM_BYTE *) (f_pdstrTarget->pwszString + f_ichInsertion),
            PB_DSTR(f_pdstrSub),
            CB_DSTR(f_pdstrSub));

ErrorExit:
    return dr;
}



/**********************************************************************
** Function:    DRM_UTL_StringRemoveSubString
** Synopsis:    remove a substring from the target string
** Arguments:   [f_pdstrSource] -- Source string
**              [f_pdstrSub]    -- A substring within f_pdstrSource
** Notes:
** - Caller is responsible for sufficient buffer in destination string
***********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_UTL_StringRemoveSubString(
    IN OUT   DRM_STRING       *f_pdstrSource,
    IN const DRM_CONST_STRING *f_pdstrSub)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchToMove = 0;
    DRM_DWORD  ich       = 0;
    DRM_WCHAR *pwsz       = NULL;

    ChkDRMString(f_pdstrSource);
    if ( f_pdstrSub             == NULL
      || f_pdstrSub->pwszString == NULL
      || f_pdstrSub->cchString  == 0 )
    {
        goto ErrorExit;
    }

    /* make sure f_pdstrSub is within f_pdstrSource */
    ChkArg(f_pdstrSource->pwszString <= f_pdstrSub->pwszString);

    ChkArg((f_pdstrSub->pwszString    + f_pdstrSub->cchString)
        <= (f_pdstrSource->pwszString + f_pdstrSource->cchString));

    cchToMove = f_pdstrSource->cchString
              - f_pdstrSub->cchString
              - (DRM_DWORD)(f_pdstrSub->pwszString
              - f_pdstrSource->pwszString);

    /* move the rest of the string to cover the hole.
    ** alternative way: use memmove().
    */

    pwsz = (DRM_WCHAR *) f_pdstrSub->pwszString;

    for (ich = cchToMove;
         ich > 0;
         ich--)
    {
        *pwsz = pwsz [f_pdstrSub->cchString];
        pwsz++;
    }

    /* adjust string length */
    f_pdstrSource->cchString -= f_pdstrSub->cchString;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function : DRM_UTL_DSTRStringsEqual
**            DRM_UTL_DASTRStringsEqual
**
** Synopsis: case-sensitive comparison of two DRM_CONST_STRINGs or
**           DRM_ANSI_CONST_STRINGs
**
** Arguments:
** [f_pdstr1]
** [f_pdstr2] -- strings to compare
**
** Return: TRUE if strings are same length and identical
**
** Note: No argument checking; make sure you pass valid strings
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DSTRStringsEqual(
    const DRM_CONST_STRING *pdstr1,
    const DRM_CONST_STRING *pdstr2)
{
    return (pdstr1->cchString == pdstr2->cchString
        &&  DRMCRT_wcsncmp(pdstr1->pwszString, pdstr2->pwszString, pdstr1->cchString) == 0);
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DASTRStringsEqual(
    const DRM_ANSI_CONST_STRING *f_pdastr1,
    const DRM_ANSI_CONST_STRING *f_pdastr2)
{
    return (f_pdastr1->cchString == f_pdastr2->cchString
        &&  DRMCRT_strncmp(f_pdastr1->pszString, f_pdastr2->pszString, f_pdastr1->cchString) == 0);
}

/*****************************************************************************
** Function : DRM_UTL_DASSTRStringsEqual
**
** Synopsis: case-sensitive comparison of an ANSI substring with a
**           DRM_ANSI_CONST_STRING
** Arguments:
** [f_pszBase] -- base of buffer into which f_pdasstr is an offset
** [f_pdasstr] -- substring of f_pszBase to compare
** [f_pdastr]  -- constant or literal string to compare
**
** Return: TRUE if strings are same length and identical
**
** Note: No argument checking; make sure you pass valid strings
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DASSTRStringsEqual(
    __in_ecount( f_pdasstr->m_ich + f_pdasstr->m_cch ) const DRM_CHAR *f_pszBase,
    const DRM_SUBSTRING         *f_pdasstr,
    const DRM_ANSI_CONST_STRING *f_pdastr)
{
    return (f_pdasstr->m_cch  == f_pdastr->cchString
        &&  DRM_BYT_CompareBytes(f_pszBase,
                                 f_pdasstr->m_ich,
                                 f_pdastr->pszString,
                                 0,
                                 f_pdasstr->m_cch) == 0);
}

/*****************************************************************************
** Function : DRM_UTL_DASSTRStrings_CI_Equal
**
** Synopsis: case-INsensitive comparison of an ANSI substring with a
**           DRM_ANSI_CONST_STRING
** Arguments:
** [f_pszBase] -- base of buffer into which f_pdasstr is an offset
** [f_pdasstr] -- substring of f_pszBase to compare
** [f_pdastr]  -- constant or literal string to compare
**
** Return: TRUE if strings are same length and identical
**
** Note: No argument checking; make sure you pass valid strings
*****************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DASSTRStrings_CI_Equal(
    __in_ecount( f_pdasstr->m_ich + f_pdasstr->m_cch ) const DRM_CHAR      *f_pszBase,
    const DRM_SUBSTRING *f_pdasstr,
    const DRM_ANSI_CONST_STRING *f_pdastr )
{
    if( f_pdasstr->m_cch != f_pdastr->cchString )
    {
        return FALSE;
    }
    else
    {
        DRM_DWORD i;

        for( i=0; i<f_pdasstr->m_cch; i++ )
        {
            if( DRMCRT_towlower( WCHAR_CAST( GET_BYTE( f_pszBase, f_pdasstr->m_ich + i ) ) )
             != DRMCRT_towlower( WCHAR_CAST( GET_BYTE( f_pdastr->pszString, i ) ) ) )
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/**********************************************************************
** Function:    DRM_UTL_EnsureDataAlignment
**
** Synopsis:    return a pointer aligned on the specified data boundary
**
** Arguments:   [f_pbOriginal] -- original pointer, potentially misaligned
**              [f_cbOriginal] -- maximum size of f_pbOriginal
**              [f_ppbAligned] -- filled in with adjusted pointer
**              [f_pcbAligned] -- adjusted size, <= f_cbOriginal
**              [f_cbDataType] -- SIZEOF data type to align
**
** Notes:       address is moved forward from the original to the next
**              aligned boundary
***********************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_EnsureDataAlignment(
    __in_bcount( f_cbOriginal )IN  const DRM_BYTE *f_pbOriginal,
    IN  const DRM_DWORD   f_cbOriginal,
    __deref_out_bcount( *f_pcbAligned ) DRM_BYTE **f_ppbAligned,
    OUT       DRM_DWORD  *f_pcbAligned,
    IN  const DRM_DWORD   f_cbDataType,
    OUT       DRM_DWORD  *f_pcbAdjustment)
{
    DRM_RESULT      dr         = DRM_SUCCESS;
    DRM_DWORD_PTR   pvOriginal = (DRM_DWORD_PTR) f_pbOriginal;
    DRM_DWORD       cbScrap    = 0;
    DRM_DWORD       cbAdjust   = 0;

    ChkArg (f_pbOriginal != NULL
        &&  f_cbOriginal  > 0
        &&  f_ppbAligned != NULL
        &&  f_pcbAligned != NULL
        &&  f_cbDataType >= 1);

    /* e.g. for a DWORD on an 4n + 1 address, cbScrap = 1 */

    cbScrap  = (DRM_DWORD) (pvOriginal % (f_cbDataType / CB_NATIVE_BYTE));

    /* e.g. for a DWORD on an 4n + 1 address, cbAdjust = 3 */

    if (cbScrap > 0)
    {
        cbAdjust = (DRM_DWORD) ((f_cbDataType / CB_NATIVE_BYTE) - cbScrap);
    }

    /* check to see if it's already aligned */

    if (cbScrap == 0)
    {
        *f_ppbAligned = (DRM_BYTE*)f_pbOriginal;
        *f_pcbAligned = f_cbOriginal;
    }
    else
    {
        /* verify that an adjusted pointer won't straddle the boundary */

        if ((f_cbDataType + cbAdjust) < f_cbOriginal)
        {
            *f_ppbAligned = (DRM_BYTE *) (pvOriginal) + (DRM_WORD) cbAdjust;
            *f_pcbAligned =  f_cbOriginal - cbAdjust;
        }
        else
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
    }

    if (f_pcbAdjustment != NULL)
    {
        *f_pcbAdjustment = cbAdjust;
    }

ErrorExit:
    return dr;
} /* DRM_UTL_EnsureDataAlignment */

/*********************************************************************
**
**  Function:  DRM_UTL_DecodeKID
**
**  Synopsis:  Decode a KID value.  In general it just performs a Base64 decode but
**             also takes into account the possibility that a header or license
**             may technically have an invalid KID in it (wrong size) and we need
**             to work with it correctly.
**
**  Arguments:
**     [f_pdstrKID] -- String representation of the KID
**     [f_pkid]     -- Binary KID will be copied to this out buffer.
**
**  Notes:     KIDs should really be 24 characters long when encoded but
**             some licenses and content have these strings incorrectly set
**             so this function compensates for that possiblity.
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_DecodeKID(
    IN const DRM_CONST_STRING *f_pdstrKID,
       OUT   DRM_KID          *f_pkid )
{
    DRM_RESULT          dr      = DRM_SUCCESS;
    DRM_CONST_STRING    dstrKID = EMPTY_DRM_STRING;
    DRM_DWORD           cb      = SIZEOF(DRM_KID);
    DRM_WCHAR           wszKID[ CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1 ];
    DRM_DWORD           cchKID  = NO_OF( wszKID ) - 1 ;


    /* If count of characters in the string is less than cchKID ( 24 )
    ** The we prepend the caracters in the string with '*'
    */


    if ( f_pdstrKID->cchString <= cchKID )
    {
        DRM_DWORD cchDiff  = cchKID - f_pdstrKID->cchString;
        dstrKID.pwszString = wszKID,
        dstrKID.cchString  = cchKID;

        for( cb = 0; cb < cchDiff; cb++ )
        {
            wszKID[cb] = g_wchAsterisk;
        }

        ChkDR( DRM_STR_StringCchCopyNW( wszKID + cchDiff,
                                   NO_OF( wszKID ) - cchDiff,
                                   f_pdstrKID->pwszString,
                                   f_pdstrKID->cchString ) );

        cb = SIZEOF( DRM_KID );
        dr = DRM_B64_DecodeW( &dstrKID, &cb, (DRM_BYTE*)f_pkid, 0 );
    }
    else
    {
        dstrKID.pwszString = f_pdstrKID->pwszString,
        dstrKID.cchString  = f_pdstrKID->cchString;
    }


    if (   DRM_FAILED( dr )
        || f_pdstrKID->cchString > cchKID )
    {
        /*
        **  Bad KID - Let's just take the first 16 bytes from original ANSI KID
        */
        DRM_DWORD   iKID = 0;

        /*
        **  Now convert it ansi - Note we cannot use DRM_UTL_DemoteUNICODEtoASCII
        **  here because we are writing to DRM_KID directly instead of a string
        **  So we do not want the trailing '\0'
        **  We need to remember that dstrKID.pwszString may have prepended with '*'
        **  We copy last 16 characters from dstrKID.pwszString to ensure that all
        **  significant characters from f_pdstrKID were used.
        */
        while( iKID < DRM_ID_SIZE )
        {
            /*
            **  Test if string was prepended with '*'. Then take last 16 characters to preserve data in f_pdstrKID.
            **  Otherwize take first 16.
            */

            if ( f_pdstrKID->cchString <= NO_OF( wszKID ) - 1 )
            {
                PUT_BYTE( f_pkid->rgb, iKID, (DRM_CHAR)NATIVE_WCHAR(dstrKID.pwszString[iKID + (dstrKID.cchString - DRM_ID_SIZE)]) );
            }
            else
            {
                PUT_BYTE( f_pkid->rgb, iKID, (DRM_CHAR)NATIVE_WCHAR(dstrKID.pwszString[iKID]) );
            }
            iKID++;
        }
        dr = DRM_SUCCESS;
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRM_UTL_PromoteASCIItoUNICODE
**
** Synopsis: copies an ASCII ANSI string to wide-ASCII
**
** Arguments:
** [f_pszBase]  -- Base pointer to ANSI original string
** [f_pdasstr]  -- Pointer to substring specification (within f_pszBase)
** [f_pdstrOut] -- UNICODE copy
*****************************************************************************/

/* This function exists to allow for SAL annotation of DRM_STRING elements */
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL _promoteASCIItoUNICODE(
    const                DRM_CHAR      *f_pszBase,
    const                DRM_SUBSTRING *f_pdasstr,
    __out_ecount(f_cch)  DRM_WCHAR     *f_pwszString,
    const                DRM_DWORD      f_cch)
{
    DRM_DWORD ich = 0;

    for (ich = 0; ich < f_cch; ich++)
    {
        f_pwszString [ich] = WCHAR_CAST(GET_CHAR(f_pszBase, f_pdasstr->m_ich + ich));
    }

}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_UTL_PromoteASCIItoUNICODE(
    __in_ecount( f_pdasstr->m_ich + f_pdasstr->m_cch ) const DRM_CHAR      *f_pszBase,
    const DRM_SUBSTRING *f_pdasstr,
          DRM_STRING    *f_pdstrOut)
{
    DRM_DWORD cch = min(f_pdasstr->m_cch, f_pdstrOut->cchString);

    _promoteASCIItoUNICODE(f_pszBase, f_pdasstr, f_pdstrOut->pwszString, cch);

    f_pdstrOut->cchString = cch;
}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_UTL_DemoteUNICODEtoASCII(
    __in_ecount(cchMax) const DRM_WCHAR *pwszFileName,
    __out_ecount(cchMax) DRM_CHAR  *pszFileName,
                         DRM_DWORD  cchMax )
{
    DRM_DWORD ich = 0;

    while( cchMax             > 0
        && pwszFileName[ich] != g_wchNull )
    {
        PUT_BYTE( pszFileName, ich, (DRM_CHAR)NATIVE_WCHAR(pwszFileName[ich]) );
        cchMax--;
        ich++;
    }

    if( cchMax > 0 )
    {
        /* NULL terminate if there is room */
        PUT_BYTE(pszFileName, ich, '\0');
    }
}


DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DSTRSearch(
    IN const DRM_CONST_STRING *f_pdstrString,
    IN const DRM_CONST_STRING *f_pdstrSubString,
       OUT   DRM_CONST_STRING *f_pdstrFoundString )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_CONST_STRING dstrString    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSubString = EMPTY_DRM_STRING;

    /*
    ** NOTE:  All dr's in this function are meaningless.
    ** They will be masked before return into a TRUE/FALSE
    ** return value.
    */
    ChkArg( f_pdstrFoundString != NULL );

    ChkDRMString( f_pdstrString );
    ChkDRMString( f_pdstrSubString );

    dstrString    = *f_pdstrString;
    dstrSubString = *f_pdstrSubString;

    while( dstrString.cchString >= dstrSubString.cchString )
    {
        DRM_DWORD ich = 0;

        while( ich                        <  dstrSubString.cchString
            && dstrString.pwszString[ich] == dstrSubString.pwszString[ich] )
        {
            ich++;
        }

        if( ich == dstrSubString.cchString )
        {
            /*
            ** We looped through all the characters in dstrSubString
            ** so there must have been a match
            */

            *f_pdstrFoundString = dstrString;
            goto ErrorExit;
        }

        dstrString.cchString--;
        dstrString.pwszString++;
    }

    dr = DRM_E_FAIL;

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        return FALSE;
    }
    return TRUE;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_DSTRSearchReverse(
    IN const DRM_CONST_STRING *f_pdstrString,
    IN const DRM_CONST_STRING *f_pdstrSubString,
       OUT   DRM_CONST_STRING *f_pdstrFoundString )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_CONST_STRING dstrString    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSubString = EMPTY_DRM_STRING;

    /*
    ** NOTE:  All dr's in this function are meaningless.
    ** They will be masked before return into a TRUE/FALSE
    ** return value.
    */

    ChkArg( f_pdstrFoundString != NULL );

    ChkDRMString( f_pdstrString );
    ChkDRMString( f_pdstrSubString );

    dstrString    = *f_pdstrString;
    dstrSubString = *f_pdstrSubString;

    if( dstrSubString.cchString == 0 )
    {
        *f_pdstrFoundString = dstrString;
        goto ErrorExit;
    }

    if( dstrString.cchString < dstrSubString.cchString )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    dstrString.pwszString += (f_pdstrString->cchString - dstrSubString.cchString);
    dstrString.cchString   =  dstrSubString.cchString;

    while( dstrString.cchString <= f_pdstrString->cchString )
    {
        DRM_DWORD ich = 0;

        while( ich                        <  dstrSubString.cchString
            && dstrString.pwszString[ich] == dstrSubString.pwszString[ich] )
        {
            ich++;
        }

        if( ich == dstrSubString.cchString )
        {
            /*
            ** We looped through all the characters in dstrSubString
            ** so there must have been a match
            */
            *f_pdstrFoundString = dstrString;
            goto ErrorExit;
        }

        dstrString.cchString++;
        dstrString.pwszString--;
    }

    dr = DRM_E_FAIL;

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        return FALSE;
    }
    return TRUE;
}

/*
**  Add encoding-decoding strings in pairs
*/

const DRM_WCHAR g_rgwchEncodedAmpersand[]               = { ONE_WCHAR('&', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR(';', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchEncodedQuote[]                   = { ONE_WCHAR('&', '\0'), ONE_WCHAR('q', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR(';', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchEncodedLesserThan[]              = { ONE_WCHAR('&', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR(';', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchEncodedGreaterThan[]             = { ONE_WCHAR('&', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR(';', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchEncodedCarriageReturnNewLine[]   = { ONE_WCHAR('\n', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchDecodedCarriageReturnNewLine[]   = { ONE_WCHAR('\r', '\0'), ONE_WCHAR('\n', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchEncodedCarriageReturn[]          = { ONE_WCHAR('\n', '\0'), ONE_WCHAR('\0', '\0')};
const DRM_WCHAR g_rgwchDecodedCarriageReturn[]          = { ONE_WCHAR('\r', '\0'), ONE_WCHAR('\0', '\0')};

const DRM_CONST_STRING g_dstrEncodedAmpersand               = CREATE_DRM_STRING( g_rgwchEncodedAmpersand );
const DRM_CONST_STRING g_dstrEncodedQuote                   = CREATE_DRM_STRING( g_rgwchEncodedQuote );
const DRM_CONST_STRING g_dstrEncodedLesserThan              = CREATE_DRM_STRING( g_rgwchEncodedLesserThan );
const DRM_CONST_STRING g_dstrEncodedGreaterThan             = CREATE_DRM_STRING( g_rgwchEncodedGreaterThan );
const DRM_CONST_STRING g_dstrEncodedCarriageReturnNewLine   = CREATE_DRM_STRING( g_rgwchEncodedCarriageReturnNewLine );
const DRM_CONST_STRING g_dstrDecodedCarriageReturnNewLine   = CREATE_DRM_STRING( g_rgwchDecodedCarriageReturnNewLine );
const DRM_CONST_STRING g_dstrEncodedCarriageReturn          = CREATE_DRM_STRING( g_rgwchEncodedCarriageReturn );
const DRM_CONST_STRING g_dstrDecodedCarriageReturn          = CREATE_DRM_STRING( g_rgwchDecodedCarriageReturn );

const DRM_CHAR g_rgchEncodedAmpersand[]             = "&amp;";
const DRM_CHAR g_rgchEncodedQuote[]                 = "&quot;";
const DRM_CHAR g_rgchEncodedLesserThan[]            = "&lt;";
const DRM_CHAR g_rgchEncodedGreaterThan[]           = "&gt;";
const DRM_CHAR g_rgchEncodedCarriageReturnNewLine[] = "\n";
const DRM_CHAR g_rgchDecodedCarriageReturnNewLine[] = "\r\n";
const DRM_CHAR g_rgchEncodedCarriageReturn[]        = "\n";
const DRM_CHAR g_rgchDecodedCarriageReturn[]        = "\r";

const DRM_ANSI_CONST_STRING g_dastrEncodedAmpersand             = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedAmpersand, 5 );
const DRM_ANSI_CONST_STRING g_dastrEncodedQuote                 = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedQuote, 6 );
const DRM_ANSI_CONST_STRING g_dastrEncodedLesserThan            = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedLesserThan, 4 );
const DRM_ANSI_CONST_STRING g_dastrEncodedGreaterThan           = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedGreaterThan, 4 );
const DRM_ANSI_CONST_STRING g_dastrEncodedCarriageReturnNewLine = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedCarriageReturnNewLine, 1 );
const DRM_ANSI_CONST_STRING g_dastrDecodedCarriageReturnNewLine = CREATE_DRM_ANSI_STRING_EX( g_rgchDecodedCarriageReturnNewLine, 2 );
const DRM_ANSI_CONST_STRING g_dastrEncodedCarriageReturn        = CREATE_DRM_ANSI_STRING_EX( g_rgchEncodedCarriageReturn, 1 );
const DRM_ANSI_CONST_STRING g_dastrDecodedCarriageReturn        = CREATE_DRM_ANSI_STRING_EX( g_rgchDecodedCarriageReturn, 1 );

typedef struct tagXMLEncodeMapping {
    const DRM_BOOL           fEncodeOnly;
    const DRM_CONST_STRING  *pdstrEncoding;
    const DRM_WCHAR          wchDecodedCharacter;   /* only used if !fEncodeOnly */
    const DRM_CONST_STRING  *pdstrDecodedString;    /* only used if fEncodeOnly */
} XMLEncodeMapping;

typedef struct tagXMLEncodeMappingUA {
    const DRM_BOOL                fEncodeOnly;
    const DRM_CONST_STRING       *pdstrEncoding;
    const DRM_CHAR                chDecodedCharacter;    /* only used if !fEncodeOnly */
    const DRM_ANSI_CONST_STRING  *pdastrDecodedString;   /* only used if fEncodeOnly */
} XMLEncodeMappingUA;

typedef struct tagXMLEncodeMappingA {
    const DRM_BOOL                fEncodeOnly;
    const DRM_ANSI_CONST_STRING  *pdastrEncoding;
    const DRM_CHAR                chDecodedCharacter;   /* only used if !fEncodeOnly */
    const DRM_ANSI_CONST_STRING  *pdastrDecodedString;  /* only used if fEncodeOnly */
} XMLEncodeMappingA;

static const XMLEncodeMapping   g_rgXMLEncodeMapping[] =
{
    { FALSE, &g_dstrEncodedAmpersand,             WCHAR_CAST( '&' ),  NULL },
    { FALSE, &g_dstrEncodedQuote,                 WCHAR_CAST( '\"' ), NULL },
    { FALSE, &g_dstrEncodedLesserThan,            WCHAR_CAST( '<' ),  NULL },
    { FALSE, &g_dstrEncodedGreaterThan,           WCHAR_CAST( '>' ),  NULL },
    { TRUE,  &g_dstrEncodedCarriageReturnNewLine, WCHAR_CAST( '\0' ), &g_dstrDecodedCarriageReturnNewLine },
    { TRUE,  &g_dstrEncodedCarriageReturn,        WCHAR_CAST( '\0' ), &g_dstrDecodedCarriageReturn }
};

static const XMLEncodeMappingUA g_rgXMLEncodeMappingUA[] =
{
    { FALSE, &g_dstrEncodedAmpersand,             '&',  NULL },
    { FALSE, &g_dstrEncodedQuote,                 '\"', NULL },
    { FALSE, &g_dstrEncodedLesserThan,            '<',  NULL },
    { FALSE, &g_dstrEncodedGreaterThan,           '>',  NULL },
    { TRUE,  &g_dstrEncodedCarriageReturnNewLine, '\0', &g_dastrDecodedCarriageReturnNewLine },
    { TRUE,  &g_dstrEncodedCarriageReturn,        '\0', &g_dastrDecodedCarriageReturn }
};

static const XMLEncodeMappingA g_rgXMLEncodeMappingA[] =
{
    { FALSE, &g_dastrEncodedAmpersand,             '&',  NULL },
    { FALSE, &g_dastrEncodedQuote,                 '\"', NULL },
    { FALSE, &g_dastrEncodedLesserThan,            '<',  NULL },
    { FALSE, &g_dastrEncodedGreaterThan,           '>',  NULL },
    { TRUE,  &g_dastrEncodedCarriageReturnNewLine, '\0', &g_dastrDecodedCarriageReturnNewLine },
    { TRUE,  &g_dastrEncodedCarriageReturn,        '\0', &g_dastrDecodedCarriageReturn }
};

/******************************************************************************
**
** Function :   DRM_UTL_XMLDecode
**
** Synopsis :   Decode special characters like "&amp;" to plain old"&"
**
** Arguments :  f_pwszEncoded   : Encoded string
**              f_cchEncoded    : Length of encoded string
**              f_pwszDecoded   : Buffer for decoded string; if NULL, the
**                                length of buffer required is returned and
**                                errcode is DRM_E_BUFFERTOOSMALL
**              f_pcchDecoded   : Length of buffer for decoded string. After
**                                returning, it contains length of decoded buffer,
**                                or length of buffer required.
**
**
** Returns :    DRM_E_BUFFERTOOSMALL, if buffer for decoded string is not long
**              enough
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_XMLDecode(
    __in_ecount( f_cchEncoded ) const DRM_WCHAR *f_pwszEncoded,
    IN          DRM_DWORD    f_cchEncoded,
    __out_ecount_opt(*f_pcchDecoded) DRM_WCHAR *f_pwszDecoded,
    IN  OUT     DRM_DWORD   *f_pcchDecoded )
{
    DRM_RESULT      dr                  = DRM_SUCCESS;
    DRM_DWORD       iEncoded            = 0;
    DRM_DWORD       iDecoded            = 0;
    DRM_DWORD       iEncoding           = 0;
    DRM_BOOL        fFoundEncodedChar   = FALSE;

    ChkArg( f_pwszEncoded != NULL && f_pcchDecoded != NULL );

    while ( iEncoded < f_cchEncoded )
    {
        /*
        **  Check whether the next character is an encoded character
        */
        fFoundEncodedChar = FALSE;
        for ( iEncoding = 0;
              iEncoding < NO_OF( g_rgXMLEncodeMapping );
              iEncoding ++ )
        {
            if ( !g_rgXMLEncodeMapping[iEncoding].fEncodeOnly
              && ( f_cchEncoded >= iEncoded + g_rgXMLEncodeMapping[iEncoding].pdstrEncoding->cchString )
              && DRMCRT_wcsncmp( f_pwszEncoded + iEncoded,
                                 g_rgXMLEncodeMapping[iEncoding].pdstrEncoding->pwszString,
                                 g_rgXMLEncodeMapping[iEncoding].pdstrEncoding->cchString ) == 0 )
            {
                if ( f_pwszDecoded != NULL && iDecoded < (*f_pcchDecoded) )
                {
                    f_pwszDecoded[ iDecoded ] =   g_rgXMLEncodeMapping[iEncoding].wchDecodedCharacter;
                }
                iEncoded += g_rgXMLEncodeMapping[iEncoding].pdstrEncoding->cchString;
                fFoundEncodedChar = TRUE;
                break;
            }
        }

        if ( !fFoundEncodedChar )
        {
            if ( f_pwszDecoded != NULL && iDecoded < (*f_pcchDecoded) )
            {
                f_pwszDecoded[ iDecoded ] = f_pwszEncoded[ iEncoded ];
            }
            iEncoded++;
        }

        iDecoded++;
    }

    if ( iDecoded > *f_pcchDecoded || f_pwszDecoded == NULL )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    *f_pcchDecoded = iDecoded;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_UTL_XMLDecodeUA
**
** Synopsis :   Convert special characters in a Unicode URL string according
**              to XML decoding standard. The output is a UTF8/ANSI string.
**              For example, "&amp;" will be converted to "&".
**
** Arguments :
** [f_pwchEncoded]          - Pointer to a buffer that contains the source URL string.
** [f_cchEncoded]           - Size (number of wide characters) of the source URL string.
** [f_pchDecoded]           - Pointer to a buffer to receive the decoded URL string.
**                            If it is NULL, the function returns an error and the
**                            size of the required buffer is stored in *f_pcchDecoded.
** [f_pcchDecoded]          - Pointer to a variable that contains the size (in characters
**                            of the decode buffer during input and receives the
**                            size (in characterss) of the decoded string during output.
**
** Returns:                 DRM_SUCCESS on success
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the decode buffer is too small.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_XMLDecodeUA(
    __in_ecount( f_cchEncoded ) const DRM_WCHAR *f_pwchEncoded,
    IN DRM_DWORD f_cchEncoded,
    __out_ecount_opt(*f_pcchDecoded) DRM_CHAR *f_pchDecoded,
    IN OUT DRM_DWORD *f_pcchDecoded )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iEncoded = 0;
    DRM_DWORD iDecoded = 0;
    DRM_DWORD iEncoding = 0;
    DRM_BOOL fFoundEncodedChar = FALSE;
    DRM_WCHAR wchTmp = 0;

    ChkArg( f_pwchEncoded != NULL && f_pcchDecoded != NULL );

    while ( iEncoded < f_cchEncoded )
    {
        fFoundEncodedChar = FALSE;

        /*
        ** Check whether the next character is an encoded character.
        */
        for ( iEncoding = 0;
              iEncoding < NO_OF( g_rgXMLEncodeMappingUA );
              iEncoding ++ )
        {
            if( !g_rgXMLEncodeMappingUA[iEncoding].fEncodeOnly
             && ( f_cchEncoded >= iEncoded + g_rgXMLEncodeMappingUA[ iEncoding ].pdstrEncoding->cchString )
             && DRMCRT_wcsncmp( f_pwchEncoded + iEncoded,
                                g_rgXMLEncodeMappingUA[ iEncoding ].pdstrEncoding->pwszString,
                                g_rgXMLEncodeMappingUA[ iEncoding ].pdstrEncoding->cchString ) == 0 )
            {
                if ( f_pchDecoded != NULL && iDecoded < *f_pcchDecoded )
                {
                    f_pchDecoded[ iDecoded ] = g_rgXMLEncodeMappingUA[ iEncoding ].chDecodedCharacter;
                }

                iEncoded += g_rgXMLEncodeMappingUA[ iEncoding ].pdstrEncoding->cchString;

                fFoundEncodedChar = TRUE;

                break;
            }
        }

        if ( !fFoundEncodedChar )
        {
            if ( f_pchDecoded != NULL && iDecoded < *f_pcchDecoded )
            {
                wchTmp = f_pwchEncoded[ iEncoded ];
                FIX_ENDIAN_WORD( wchTmp );

                f_pchDecoded[ iDecoded ] = ( DRM_CHAR )wchTmp;
            }

            iEncoded++;
        }

        iDecoded++;
    }

    if ( iDecoded > *f_pcchDecoded || f_pchDecoded == NULL )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pcchDecoded = iDecoded;

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function :   DRM_UTL_XMLDecodeA
**
** Synopsis :   Convert special characters in an ANSI/UTF8 URL string according
**              to XML decoding standard. The output is a UTF8/ANSI string.
**              For example, "&amp;" will be converted to "&".
**
** Arguments :
** [f_pchEncoded]           - Pointer to a buffer that contains the source URL string.
** [f_cchEncoded]           - Size (number of characters) of the source URL string.
** [f_pchDecoded]           - Pointer to a buffer to receive the decoded URL string.
**                            If it is NULL, the function returns an error and the
**                            size of the required buffer is stored in *f_pcchDecoded.
** [f_pcchDecoded]          - Pointer to a variable that contains the size (in characters
**                            of the decode buffer during input and receives the
**                            size (in characterss) of the decoded string during output.
**
** Returns:                 DRM_SUCCESS on success
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the decode buffer is too small.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_XMLDecodeA(
    __in_ecount( f_cchEncoded ) const DRM_CHAR *f_pchEncoded,
    IN DRM_DWORD f_cchEncoded,
    __out_ecount_opt(*f_pcchDecoded) DRM_CHAR *f_pchDecoded,
    IN OUT DRM_DWORD *f_pcchDecoded )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iEncoded = 0;
    DRM_DWORD iDecoded = 0;
    DRM_DWORD iEncoding = 0;
    DRM_BOOL fFoundEncodedChar = FALSE;

    ChkArg( f_pchEncoded != NULL && f_pcchDecoded != NULL );

    while ( iEncoded < f_cchEncoded )
    {
        fFoundEncodedChar = FALSE;

        /*
        ** Check whether the next character is an encoded character.
        */
        for ( iEncoding = 0;
              iEncoding < NO_OF( g_rgXMLEncodeMappingA );
              iEncoding ++ )
        {
            if( !g_rgXMLEncodeMappingA[iEncoding].fEncodeOnly
             && ( f_cchEncoded >= iEncoded + g_rgXMLEncodeMappingA[ iEncoding ].pdastrEncoding->cchString )
             && MEMCMP( f_pchEncoded + iEncoded,
                        g_rgXMLEncodeMappingA[ iEncoding ].pdastrEncoding->pszString,
                        g_rgXMLEncodeMappingA[ iEncoding ].pdastrEncoding->cchString ) == 0 )
            {
                if ( f_pchDecoded != NULL && iDecoded < *f_pcchDecoded )
                {
                    f_pchDecoded[ iDecoded ] = g_rgXMLEncodeMappingA[ iEncoding ].chDecodedCharacter;
                }

                iEncoded += g_rgXMLEncodeMappingA[ iEncoding ].pdastrEncoding->cchString;

                fFoundEncodedChar = TRUE;

                break;
            }
        }

        if ( !fFoundEncodedChar )
        {
            if ( f_pchDecoded != NULL && iDecoded < *f_pcchDecoded )
            {
                f_pchDecoded[ iDecoded ] = f_pchEncoded[ iEncoded ];
            }

            iEncoded++;
        }

        iDecoded++;
    }

    if ( iDecoded > *f_pcchDecoded || f_pchDecoded == NULL )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pcchDecoded = iDecoded;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_UTL_XMLEncodeA
**
** Synopsis: Function that does XML encoding of a UTF8/ANSI string.
**
** Arguments:
**
** [f_pchDecoded]           -- Pointer to a buffer that contains the string
**                             to be XML encoded.
** [f_cchDecoded]           -- Size (number of characters) of the input string.
** [f_fAttributeText]       -- Flag indicating whether the text to be encoded is
**                             in the attribute of a XML tag. If f_fAttributeText
**                             is FALSE, '"' does not need to be encoded.
** [f_pchEncoded]           -- Pointer to a buffer to receive the XML encoded
**                             string. If it is NULL, the required size will
**                             be stored in *f_pcchEncoded.
** [f_pcchEncoded]          -- Pointer to a variable that contains the size
**                             of buffer to receive the XML encoded string
**                             during input and received the actual size of
**                             XML encoded string during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to receive
**                          the XML encoded string is NULL or not big enough.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_XMLEncodeA(
    __in_ecount( f_cchDecoded ) const DRM_CHAR *f_pchDecoded,
    __in DRM_DWORD f_cchDecoded,
    __in DRM_BOOL f_fAttributeText,
    __out_ecount_opt(*f_pcchEncoded) DRM_CHAR *f_pchEncoded,
    __inout DRM_DWORD *f_pcchEncoded )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0, j = 0, k = 0;
    DRM_DWORD cchEncoded = 0;
    DRM_CHAR *pchEncodedCurr = f_pchEncoded;
    DRM_BOOL fBufferTooSmall = FALSE;
    DRM_DWORD cchInput = 0;
    DRM_DWORD cchMapping = 0;
    DRM_WCHAR wchTmp = 0;

    ChkArg( f_pchDecoded != NULL && f_pcchEncoded != NULL );

    if ( f_pchEncoded == NULL && f_cchDecoded > 0 )
    {
        fBufferTooSmall = TRUE;
    }

    cchInput = *f_pcchEncoded;

    while ( i < f_cchDecoded )
    {
        /* Do not encode '"' if the string is not going to be used as attribute text. */
        if ( f_fAttributeText || f_pchDecoded[ i ] != '\"' )
        {
            for ( j = 0; j < NO_OF( g_rgXMLEncodeMappingUA ); j++ )
            {
                if( g_rgXMLEncodeMappingUA[ j ].fEncodeOnly )
                {
                    if( ( f_cchDecoded >= i + g_rgXMLEncodeMappingUA[ j ].pdastrDecodedString->cchString )
                     && MEMCMP( f_pchDecoded + i,
                                g_rgXMLEncodeMappingUA[ j ].pdastrDecodedString->pszString,
                                g_rgXMLEncodeMappingUA[ j ].pdastrDecodedString->cchString ) == 0 )
                    {
                        break;
                    }
                }
                else if ( f_pchDecoded[ i ] == g_rgXMLEncodeMappingUA[ j ].chDecodedCharacter )
                {
                    break;
                }
            }
        }
        else
        {
            j = NO_OF( g_rgXMLEncodeMappingUA );
        }

        if ( j < NO_OF( g_rgXMLEncodeMappingUA ) )
        {
            /* Found a mapping. */

            cchMapping = g_rgXMLEncodeMappingUA[ j ].pdstrEncoding->cchString;

            cchEncoded += cchMapping;

            if ( f_pchEncoded != NULL )
            {
                if ( cchEncoded <= *f_pcchEncoded )
                {
                    for ( k = 0; k < cchMapping; k++ )
                    {
                        if ( pchEncodedCurr < f_pchEncoded + cchInput )
                        {
                            wchTmp = g_rgXMLEncodeMappingUA[ j ].pdstrEncoding->pwszString[ k ];
                            FIX_ENDIAN_WORD( wchTmp );

                            *pchEncodedCurr = ( DRM_CHAR )wchTmp;

                            pchEncodedCurr++;
                        }
                    }
                }
                else
                {
                    fBufferTooSmall = TRUE;
                }
            }

            i += g_rgXMLEncodeMappingUA[ j ].fEncodeOnly ? g_rgXMLEncodeMappingUA[ j ].pdastrDecodedString->cchString : 1;
        }
        else
        {
            cchEncoded++;

            if ( f_pchEncoded != NULL )
            {
                if ( cchEncoded <= *f_pcchEncoded )
                {
                    if ( pchEncodedCurr < f_pchEncoded + cchInput )
                    {
                        *pchEncodedCurr = f_pchDecoded[ i ];

                        pchEncodedCurr++;
                    }
                }
                else
                {
                    fBufferTooSmall = TRUE;
                }
            }

            i++;
        }
    }

    *f_pcchEncoded = cchEncoded;

    if ( fBufferTooSmall )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_UTL_XMLEncode
**
** Synopsis: Function that does XML encoding of a Unicode string.
**
** Arguments:
**
** [f_pdstrToEncode]        -- Contains the string
**                             to be XML encoded.
** [f_fAttributeText]       -- Flag indicating whether the text to be encoded is
**                             in the attribute of a XML tag. If f_fAttributeText
**                             is FALSE, '"' does not need to be encoded.
** [f_pdstrEncoded]         -- Pointer to a DRM_STRING to receive the XML encoded
**                             string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to receive
**                          the XML encoded string is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_XMLEncode(
    __in  const   DRM_CONST_STRING    *f_pdstrToEncode,
    __in          DRM_BOOL             f_fAttributeText,
    __out         DRM_STRING          *f_pdstrEncoded)
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_WCHAR *pwszSrc = NULL;
    DRM_DWORD cchToCopy = 0;
    DRM_WCHAR *pwszDest = NULL;
    DRM_DWORD cchDest = 0;
    DRM_DWORD i = 0;

    ChkDRMString(f_pdstrToEncode);
    ChkDRMString(f_pdstrEncoded);

    pwszSrc = f_pdstrToEncode->pwszString;
    cchToCopy = f_pdstrToEncode->cchString;
    cchDest = f_pdstrEncoded->cchString;
    pwszDest = f_pdstrEncoded->pwszString;

    while (cchDest && cchToCopy && (*pwszSrc != g_wchNull))
    {
        DRM_BOOL fWasEscaped = FALSE;

        for(i = 0; i < NO_OF( g_rgXMLEncodeMapping ); i++)
        {
            if( g_rgXMLEncodeMapping[ i ].fEncodeOnly )
            {
                if( ( cchToCopy >= g_rgXMLEncodeMapping[ i ].pdstrDecodedString->cchString )
                 && DRMCRT_wcsncmp( pwszSrc,
                                    g_rgXMLEncodeMapping[ i ].pdstrDecodedString->pwszString,
                                    g_rgXMLEncodeMapping[ i ].pdstrDecodedString->cchString ) == 0 )
                {
                    DRM_DWORD dwLen = 0;
                    fWasEscaped = TRUE;

                    dwLen = min(cchDest, g_rgXMLEncodeMapping[i].pdstrEncoding->cchString);
                    MEMCPY(pwszDest, g_rgXMLEncodeMapping[i].pdstrEncoding->pwszString, dwLen * SIZEOF( DRM_WCHAR ));
                    cchDest -= dwLen;
                    pwszSrc += g_rgXMLEncodeMapping[ i ].pdstrDecodedString->cchString;
                    pwszDest += dwLen;
                    cchToCopy -= g_rgXMLEncodeMapping[ i ].pdstrDecodedString->cchString;
                }
            }
            else if( *pwszSrc == g_rgXMLEncodeMapping[i].wchDecodedCharacter
                  /* Do not encode '"' if the string is not going to be used as attribute text. */
                  && ( f_fAttributeText || *pwszSrc != '\"' ) )
            {
                DRM_DWORD dwLen = 0;
                fWasEscaped = TRUE;

                dwLen = min(cchDest, g_rgXMLEncodeMapping[i].pdstrEncoding->cchString);
                MEMCPY(pwszDest, g_rgXMLEncodeMapping[i].pdstrEncoding->pwszString, dwLen * SIZEOF( DRM_WCHAR ));
                cchDest -= dwLen;
                pwszSrc++;
                pwszDest += dwLen;
                cchToCopy--;
                break;
            }
        }

        if(!fWasEscaped)
        {
            *pwszDest++ = *pwszSrc++;
            cchDest--;
            cchToCopy--;
        }
    }

    if (cchDest == 0)
    {
        /* We are going to truncate f_pwszDest */
        pwszDest--;
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *pwszDest = g_wchNull;

    f_pdstrEncoded->cchString -= cchDest;


 ErrorExit:
    return dr;
}
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_XOR(
    __inout_bcount(cb) DRM_BYTE  *pbLHS,
    __in_bcount(cb) const DRM_BYTE  *pbRHS,
    IN                   DRM_DWORD  cb )
{
    DRM_DWORD i;
    for( i = 0; i < cb; i++ )
    {
        PUT_BYTE(pbLHS, i, GET_BYTE(pbLHS, i) ^ GET_BYTE(pbRHS,i) );
    }
}

DRM_API DRM_RESULT DRM_CALL DRM_UTL_ReadGUID(
   __in_bcount( cbData ) const DRM_BYTE *pbData,
   IN  const    DRM_DWORD cbData,
   IN           DRM_DWORD ibGuidOffset,
   OUT          DRM_GUID *pDrmGuid )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( pbData != NULL );
    ChkArg( pDrmGuid != NULL );

    ChkOverflow( cbData, ibGuidOffset );

    ChkBOOL( cbData - ibGuidOffset >= DRM_GUID_LEN, DRM_E_BUFFERTOOSMALL );

    /*  Convert field by field */
    LITTLEENDIAN_BYTES_TO_DWORD( pDrmGuid->Data1, pbData, ibGuidOffset );
    ibGuidOffset += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_DWORD( pDrmGuid->Data2,  pbData, ibGuidOffset );
    ibGuidOffset += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_WORD( pDrmGuid->Data3, pbData, ibGuidOffset );
    ibGuidOffset += SIZEOF( DRM_WORD );

    /*  Copy last 8 bytes */
    DRM_BYT_CopyBytes( pDrmGuid->Data4, 0, pbData, ibGuidOffset, 8 );

ErrorExit :
    return dr;
}

DRM_API DRM_BOOL DRM_CALL DRM_UTL_AreGUIDEqual( const DRM_GUID *pDrmGuid1, const DRM_GUID *pDrmGuid2 )
{
    if( pDrmGuid1 == NULL || pDrmGuid2 == NULL )
        return FALSE;
    else
        return 0 == MEMCMP( pDrmGuid1, pDrmGuid2, SIZEOF( DRM_GUID ) );
}

/**********************************************************************
** Function:    DRM_UTL_IsZeros
** Synopsis:    check if the given byte array contains all zero bytes.
** Arguments:   [pbBuff] -- byte array to be checked
**              [cbBuff[ -- size of pbBuff
** Returns:     TRUE if all zeros
** Notes:
***********************************************************************
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_IsZeros(
    __in_bcount( cbBuff )const DRM_BYTE *pbBuff,
    IN DRM_DWORD cbBuff )
{
    DRM_DWORD i = 0;

    for ( i = 0; i < cbBuff; i++)
    {
        if (GET_BYTE(pbBuff, i) != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}



/*********************************************************************
**
**  Function:  DRM_UTL_LShift
**
**  Synopsis:  Left-shift on a byte buffer (not done in place)
**
**  Arguments:
**     f_pbIn  :  Input buffer
**     f_pbOut :  Output (result) buffer
**     f_cb    :  Byte count to shift
**
**  Returns: DRM_SUCCESS
**              Success
**           DRM_E_INVALIDARG
**              One of the pointer arguments was NULL
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_LShift(
    __in_bcount( f_cb ) const DRM_BYTE *f_pbIn,
    __out_ecount(f_cb) DRM_BYTE *f_pbOut,
    IN                 DRM_DWORD f_cb )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;
    DRM_BYTE   b  = 0;

    ChkArg( f_pbIn != NULL
         && f_pbOut != NULL );

    for( i = 0; i < f_cb; i++ )
    {
        b = GET_BYTE( f_pbIn, i ) << 1;

        if( i < f_cb - 1 )
        {
            PUT_BYTE( f_pbOut, i, b | ( GET_BYTE( f_pbIn, i + 1 ) >> 7 ) );
        }
        else
        {
            PUT_BYTE( f_pbOut, i, b );
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_UTL_RShift
**
**  Synopsis:  Right-shift on a byte buffer (not done in place)
**
**  Arguments:
**     f_pbIn  :  Original buffer
**     f_pbOut :  Output (result) buffer
**     f_cb    :  Byte count to shift
**
**  Returns: DRM_SUCCESS
**              Success
**           DRM_E_INVALIDARG
**              One of the pointer arguments was NULL
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_RShift(
    __in_bcount( f_cb ) const DRM_BYTE *f_pbIn,
    __out_ecount(f_cb) DRM_BYTE *f_pbOut,
    IN                 DRM_DWORD f_cb )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;
    DRM_BYTE   b  = 0;

    ChkArg( f_pbIn != NULL
         && f_pbOut != NULL );

    for( i = 0; i < f_cb; i++ )
    {
        b = GET_BYTE( f_pbIn, i ) >> 1;

        if( i > 0 )
        {
            PUT_BYTE( f_pbOut, i, b | ( GET_BYTE( f_pbIn, i - 1 ) << 7 ) );
        }
        else
        {
            PUT_BYTE( f_pbOut, i, b );
        }
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_MultiplyDWORDs
**
** Synopsis :   Multiplies two DWORDs while checking for arithmetic overflows
**
** Parameters:
**      [f_dwFirst]   : Specifies the first DWORD to multiply
**      [f_dwSecond]  : Specifies the second DWORD to multiply
**      [f_pdwResult] : Returns the result of the multiplication
**
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pdwResult was NULL
**      DRM_E_ARITHMETIC_OVERFLOW
**          An overflow occurred during multiplication
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_MultiplyDWORDs(
    IN      DRM_DWORD   f_dwFirst,
    IN      DRM_DWORD   f_dwSecond,
        OUT DRM_DWORD  *f_pdwResult )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_UINT64 ui64Result = DRM_UI64Mul( DRM_UI64( f_dwFirst ), DRM_UI64( f_dwSecond ) );

    ChkArg( f_pdwResult != NULL );

    *f_pdwResult = 0;
    ChkBOOL( DRM_UI64High32( ui64Result ) == 0, DRM_E_ARITHMETIC_OVERFLOW );
    *f_pdwResult = DRM_UI64Low32( ui64Result );

ErrorExit:
    return dr;
}



EXIT_PK_NAMESPACE_CODE;
