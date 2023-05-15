/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmstrsafe.h>
#include <drmutilities.h>

ENTER_PK_NAMESPACE_CODE;

static DRM_RESULT DRM_CALL _DrmStringLengthWorkerW(
    __in_ecount(f_cchMax) const DRM_WCHAR *f_pwsz,
    __in                        DRM_DWORD  f_cchMax,
    __out_opt                   DRM_DWORD *f_pcchLength)
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cchMaxPrev   = f_cchMax;

    while ((f_cchMax != 0) && (*f_pwsz != g_wchNull))
    {
        f_pwsz++;
        f_cchMax--;
    }

    /* Check if the string is longer than cchMax */
    ChkArg( f_cchMax != 0 );

ErrorExit:

    if (f_pcchLength != NULL)
    {
        if (DRM_SUCCEEDED(dr))
        {
            *f_pcchLength = cchMaxPrev - f_cchMax;
        }
        else
        {
            *f_pcchLength = 0;
        }
    }

    return dr;
}

static DRM_RESULT DRM_CALL _DrmStringLengthWorkerA(
    __in_z              const DRM_CHAR  *f_psz,
    __in                      DRM_DWORD  f_cchMax,
    __out_opt                 DRM_DWORD *f_pcchLength)
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cchMaxPrev   = f_cchMax;
    DRM_DWORD  ib           = 0;

    while (f_cchMax && (GET_BYTE(f_psz, ib) != '\0'))
    {
        ib++;
        f_cchMax--;
    }

    /* Check if the string is longer than cchMax */
    ChkArg( f_cchMax != 0 );

ErrorExit:

    if (f_pcchLength != NULL)
    {
        if (DRM_SUCCEEDED(dr))
        {
            *f_pcchLength = cchMaxPrev - f_cchMax;
        }
        else
        {
            *f_pcchLength = 0;
        }
    }

    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _DrmStringCopyNWorkerW(
    __out_ecount_z(f_cchDest)       DRM_WCHAR *f_pwszDest,
    __in                            DRM_DWORD  f_cchDest,
    __in_ecount(f_cchToCopy)  const DRM_WCHAR *f_pwszSrc,
    __in                            DRM_DWORD  f_cchToCopy )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pwszSrc != NULL );
    ChkArg( f_pwszDest != NULL );
    ChkArg( f_cchDest != 0 );

    while (f_cchDest && f_cchToCopy && (*f_pwszSrc != g_wchNull))
    {
        *f_pwszDest++ = *f_pwszSrc++;
        f_cchDest--;
        f_cchToCopy--;
    }

    if (f_cchDest == 0)
    {
        /* We are going to truncate f_pwszDest */
        f_pwszDest--;
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pwszDest = g_wchNull;

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DrmStringCopyWorkerW(
    __out_ecount_z(f_cchDest) DRM_WCHAR *f_pwszDest,
    __in                      DRM_DWORD  f_cchDest,
    __in_z              const DRM_WCHAR *f_pwszSrc)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_cchDest != 0 );

    while (f_cchDest && (*f_pwszSrc != g_wchNull))
    {
        *f_pwszDest++ = *f_pwszSrc++;
        f_cchDest--;
    }

    if (f_cchDest == 0)
    {
        /* we are going to truncate f_pwszDest */
        f_pwszDest--;
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pwszDest = g_wchNull;

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _DrmStringCopyWorkerA(
    __out_ecount_z(f_cchDest) DRM_CHAR *f_pszDest,
    __in                      DRM_DWORD f_cchDest,
    __in_z              const DRM_CHAR *f_pszSrc)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  ib = 0;

    ChkArg(f_cchDest != 0);

    while (f_cchDest && (GET_BYTE(f_pszSrc, ib) != '\0'))
    {
        PUT_BYTE( f_pszDest, ib, GET_BYTE(f_pszSrc, ib) );
        ib++;
        f_cchDest--;
    }

    if (f_cchDest == 0)
    {
        // we are going to truncate f_pszDest
        ib--;
        dr = DRM_E_BUFFERTOOSMALL;
    }

    PUT_BYTE( f_pszDest, ib, '\0' );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DrmStringCopyNWorkerA(
    __out_ecount_z(f_cchDest)   DRM_CHAR *f_pszDest,
    __in                        DRM_DWORD f_cchDest,
    __in_ecount(f_cchSrc) const DRM_CHAR *f_pszSrc,
    __in                        DRM_DWORD f_cchSrc)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  ib = 0;

    ChkArg(f_cchDest != 0);

    while (f_cchDest && f_cchSrc && (GET_BYTE(f_pszSrc, ib) != '\0'))
    {
        PUT_BYTE( f_pszDest, ib, GET_BYTE(f_pszSrc, ib) );
        ib++;
        f_cchDest--;
        f_cchSrc--;
    }

    if (f_cchDest == 0)
    {
        // we are going to truncate f_pszDest
        ib--;
        dr = DRM_E_BUFFERTOOSMALL;
    }

    PUT_BYTE( f_pszDest, ib, '\0' );
ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DrmStringCatNWorkerW(
    __inout_ecount_z(f_cchDest)       DRM_WCHAR *f_pwszDest,
    __in                              DRM_DWORD  f_cchDest,
    __in_ecount(f_cchToAppend) const  DRM_WCHAR *f_pwszSrc,
    __in                              DRM_DWORD  f_cchToAppend)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchDestLength = 0;

    ChkDR( _DrmStringLengthWorkerW(f_pwszDest, f_cchDest, &cchDestLength) );

    ChkDR( _DrmStringCopyNWorkerW(f_pwszDest + cchDestLength,
                        f_cchDest - cchDestLength,
                        f_pwszSrc,
                        f_cchToAppend) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DrmStringCatWorkerW(
    __inout_ecount_z(f_cchDest) DRM_WCHAR *f_pwszDest,
    __in                        DRM_DWORD  f_cchDest,
    __in_z                const DRM_WCHAR *f_pwszSrc)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchDestLength = 0;

    ChkDR( _DrmStringLengthWorkerW(f_pwszDest, f_cchDest, &cchDestLength) );

    ChkDR( _DrmStringCopyWorkerW(f_pwszDest + cchDestLength,
                          f_cchDest - cchDestLength,
                          f_pwszSrc) );
ErrorExit:
   return dr;
}

static DRM_RESULT DRM_CALL _DrmStringCatWorkerA(
    __inout_ecount_z(f_cchDest) DRM_CHAR *f_pszDest,
    __in                        DRM_DWORD f_cchDest,
    __in_z                const DRM_CHAR *f_pszSrc)
{
    DRM_RESULT dr;
    DRM_DWORD  cchDestLength;

    ChkDR( _DrmStringLengthWorkerA(f_pszDest, f_cchDest, &cchDestLength) );

    ChkOverflow( f_cchDest, f_cchDest - cchDestLength );

    ChkDR( _DrmStringCopyWorkerA(f_pszDest + cchDestLength,
                                 f_cchDest - cchDestLength,
                                 f_pszSrc) );
ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DrmStringCatNWorkerA(
    __inout_ecount_z(f_cchDest)       DRM_CHAR *f_pszDest,
    __in                              DRM_DWORD f_cchDest,
    __in_ecount(f_cchToAppend) const  DRM_CHAR *f_pszSrc,
    __in                              DRM_DWORD f_cchToAppend)
{
    DRM_RESULT dr;
    DRM_DWORD  cchDestLength;

    ChkDR( _DrmStringLengthWorkerA(f_pszDest, f_cchDest, &cchDestLength) );

    ChkOverflow( f_cchDest, f_cchDest - cchDestLength );

    ChkDR( _DrmStringCopyNWorkerA(f_pszDest + cchDestLength,
                                  f_cchDest - cchDestLength,
                                  f_pszSrc,
                                  f_cchToAppend) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchLengthW
**
** Synopsis:    This function is a replacement for strlen.
**
**              It is used to ensure that a string is not larger than a
**              given length, in characters. If that condition is met,
**              StringCchLength returns the current length of the string
**              in characters, not including the terminating null character.
**
** Arguments:   [f_pwsz]    Pointer to a buffer containing the string whose
**                          length is being checked.
**              [f_cchMax]  The maximum number of characters allowed in f_pwsz,
**                          including the terminating null character.
**                          This value cannot exceed DRM_STRSAFE_MAX_CCH.
**              [f_pcch]    Pointer to a variable of type DRM_DWORD
**                          containing the number of characters in f_pwsz,
**                          excluding the terminating null character.
**                          This value is valid only if f_pcch is not null
**                          and the function succeeds.
**
** Returns:     DRM_SUCCESS
**                  The string at f_pwsz was not null, and the length of the
**                  string (including the terminating null character) is less
**                  than or equal to f_cchMax characters.
**              DRM_E_INVALIDARG
**                  The value in f_pwsz is NULL, f_cchMax is larger than
**                  DRM_STRSAFE_MAX_CCH, or f_pwsz is longer than cchMax.
**              DRM_E_BUFFERTOOSMALL
**                  f_cchSrc is larger than f_cchDest
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchLengthW
(
    __in_z const DRM_WCHAR *f_pwsz,
    __in         DRM_DWORD  f_cchMax,
    __out_opt    DRM_DWORD *f_pcchLength )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( (f_pwsz != NULL) && (f_cchMax <= DRM_STRSAFE_MAX_CCH) );

    ChkDR( _DrmStringLengthWorkerW(f_pwsz, f_cchMax, f_pcchLength) );

ErrorExit:
    if (DRM_FAILED(dr) && f_pcchLength != NULL)
    {
        *f_pcchLength = 0;
    }
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchLengthA
**
** Synopsis:    This function is a replacement for strlen.
**
**              It is used to ensure that a string is not larger than a
**              given length, in characters. If that condition is met,
**              StringCchLength returns the current length of the string
**              in characters, not including the terminating null character.
**
** Arguments:   [f_psz]     Pointer to a buffer containing the string whose
**                          length is being checked.
**              [f_cchMax]  The maximum number of characters allowed in f_pwsz,
**                          including the terminating null character.
**                          This value cannot exceed DRM_STRSAFE_MAX_CCH.
**              [f_pcch]    Pointer to a variable of type DRM_DWORD
**                          containing the number of characters in f_pwsz,
**                          excluding the terminating null character.
**                          This value is valid only if f_pcch is not null
**                          and the function succeeds.
**
** Returns:     DRM_SUCCESS
**                  The string at f_pwsz was not null, and the length of the
**                  string (including the terminating null character) is less
**                  than or equal to f_cchMax characters.
**              DRM_E_INVALIDARG
**                  The value in f_pwsz is NULL, f_cchMax is larger than
**                  DRM_STRSAFE_MAX_CCH, or f_pwsz is longer than f_cchMax.
**              DRM_E_BUFFERTOOSMALL
**                  f_cchSrc is larger than f_cchDest
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchLengthA(
    __in_z              const DRM_CHAR  *f_psz,
    __in                      DRM_DWORD  f_cchMax,
    __out_opt                 DRM_DWORD *f_pcchLength)
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( (f_psz != NULL) && (f_cchMax <= DRM_STRSAFE_MAX_CCH) );

    ChkDR( _DrmStringLengthWorkerA(f_psz, f_cchMax, f_pcchLength) );

ErrorExit:
    if (DRM_FAILED(dr) && f_pcchLength != NULL)
    {
        *f_pcchLength = 0;
    }

    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCopyNW
**
** Synopsis:    Copies a null-terminated wide character buffer from source
**              to destination. If the source buffer is longer than the
**              destination buffer, sets the last byte of the destination
**              buffer to 0 and returns DRM_E_BUFFERTOOSMALL
**
** Arguments:   [f_pwszDest] The destination buffer
**              [f_cchDest]  The character count of the destination buffer
**              [f_pwszSrc]  The source buffer
**              [f_cchSrc]   The character count of the source buffer
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_INVALIDARG
**                  One of the byte counts is too large, or either the source
**                  or destination buffer is NULL, or the destination byte count
**                  is 0.
**              DRM_E_BUFFERTOOSMALL
**                  f_cchSrc is larger than f_cchDest
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_STR_StringCchCopyNW(
    __out_ecount_z(f_cchDest)     DRM_WCHAR  *f_pwszDest,
    __in                          DRM_DWORD   f_cchDest,
    __in_ecount( f_cchSrc ) const DRM_WCHAR  *f_pwszSrc,
    __in                          DRM_DWORD   f_cchSrc )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( (f_cchDest <= DRM_STRSAFE_MAX_CCH) && (f_cchSrc <= DRM_STRSAFE_MAX_CCH) );

    ChkArg( f_pwszDest != NULL
         && f_pwszSrc  != NULL );

    ChkDR( _DrmStringCopyNWorkerW(f_pwszDest, f_cchDest, f_pwszSrc, f_cchSrc) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCopyNA
**
** Synopsis:    Copies a null-terminated character buffer from source
**              to destination. If the source buffer is longer than the
**              destination buffer, sets the last byte of the destination
**              buffer to 0 and returns DRM_E_BUFFERTOOSMALL
**
** Arguments:   [f_pszDest]  The destination buffer
**              [f_cchDest]  The byte count of the destination buffer
**              [f_pszSrc]   The source buffer
**              [f_cchSrc]   The byte count of the source buffer
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_INVALIDARG
**                  One of the byte counts is too large, or either the source
**                  or destination buffer is NULL, or the destination byte count
**                  is 0.
**              DRM_E_BUFFERTOOSMALL
**                  f_cchSrc is larger than f_cchDest
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCopyNA(
    __out_ecount_z(f_cchDest)       DRM_CHAR *f_pszDest,
    __in                            DRM_DWORD f_cchDest,
    __in_ecount(f_cchToCopy)  const DRM_CHAR *f_pszSrc,
    __in                            DRM_DWORD f_cchToCopy)
{
    DRM_RESULT dr;

    ChkArg( (f_cchDest <= DRM_STRSAFE_MAX_CCH) );
    ChkArg( (f_cchToCopy <= DRM_STRSAFE_MAX_CCH) );

    ChkArg( f_pszDest != NULL
         && f_pszSrc  != NULL );

    ChkDR( _DrmStringCopyNWorkerA(f_pszDest, f_cchDest, f_pszSrc, f_cchToCopy) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCopyW
**
** Synopsis:    Copies a null-terminated wide character buffer from source
**              to destination. If the source buffer is longer than the
**              destination buffer, sets the last byte of the destination
**              buffer to 0 and returns DRM_E_BUFFERTOOSMALL
**
** Arguments:   [f_pwszDest] The destination buffer
**              [f_cchDest]  The byte count of the destination buffer
**              [f_pwszSrc]  The NULL terminated source buffer
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_INVALIDARG
**                  One of the byte counts is too large, or either the source
**                  or destination buffer is NULL, or the destination byte count
**                  is 0.
**              DRM_E_BUFFERTOOSMALL
**                  f_cchSrc is larger than f_cchDest
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCopyW(
    __out_ecount_z(f_cchDest) DRM_WCHAR  *f_pwszDest,
    __in                      DRM_DWORD   f_cchDest,
    __in_z              const DRM_WCHAR  *f_pwszSrc )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( (f_cchDest <= DRM_STRSAFE_MAX_CCH) );

    ChkArg( f_pwszDest != NULL
         && f_pwszSrc  != NULL );

    ChkDR( _DrmStringCopyWorkerW(f_pwszDest, f_cchDest, f_pwszSrc) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCopyA
**
** Synopsis:    Copies a null-terminated wide character buffer from source
**              to destination. If the source buffer is longer than the
**              destination buffer, sets the last byte of the destination
**              buffer to 0 and returns DRM_E_BUFFERTOOSMALL
**
** Arguments:   [f_pszDest]  The destination buffer
**              [f_cchDest]  The byte count of the destination buffer
**              [f_pszSrc]   The NULL terminated source buffer
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_INVALIDARG
**                  One of the byte counts is too large, or either the source
**                  or destination buffer is NULL, or the destination byte count
**                  is 0.
**              DRM_E_BUFFERTOOSMALL
**                  f_cchSrc is larger than f_cchDest
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCopyA(
    __out_ecount_z(f_cchDest) DRM_CHAR *f_pszDest,
    __in                      DRM_DWORD f_cchDest,
    __in_z              const DRM_CHAR *f_pszSrc)
{
    DRM_RESULT dr;

    ChkArg( (f_cchDest <= DRM_STRSAFE_MAX_CCH) );

    ChkArg( f_pszDest != NULL
         && f_pszSrc  != NULL );

    ChkDR( _DrmStringCopyWorkerA(f_pszDest, f_cchDest, f_pszSrc) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCatW
**
** Synopsis:    This function is a replacement for strcat. The size, in
**              characters, of the destination buffer is provided to the
**              function to ensure that StringCchCat does not write past
**              the end of this buffer.
**
** Arguments:   [f_pwszDest] Pointer to a buffer containing the string
**                           that f_pwszSrc is concatenated to, and which
**                           contains the entire resultant string. The
**                           string at f_pwszSrc is added to the end of the
**                           string at f_pwszDest.
**              [f_cchDest]  Size of the destination buffer, in characters.
**                           This value must equal the length of f_pwszSrc
**                           plus the length of f_pwszDest plus 1 to account
**                           for both strings and the terminating null character.
**                           The maximum number of characters allowed is
**                           DRM_STRSAFE_MAX_CCH.
**              [f_pwszSrc]  Pointer to a buffer containing the source string
**                           that is concatenated to the end of f_pwszDest. This
**                           source string must be null-terminated.
**
** Returns:     DRM_SUCCESS
**                  Source data was present, the strings were fully concatenated
**                  without truncation, and the resultant destination buffer is
**                  null-terminated.
**              DRM_E_INVALIDARG
**                  The value in f_cchDest is 0 or larger than STRSAFE_MAX_CCH, or
**                  the destination buffer is already full.
**              DRM_E_BUFFERTOOSMALL
**                  The concatenation operation failed due to insufficient buffer
**                  space. The destination buffer contains a truncated,
**                  null-terminated version of the intended result. Where
**                  truncation is acceptable, this is not necessarily a failure
**                  condition.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCatW
(   __out_ecount_z(f_cchDest) DRM_WCHAR *f_pwszDest,
    __in                      DRM_DWORD  f_cchDest,
    __in_z              const DRM_WCHAR *f_pwszSrc )

{   DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pwszDest != NULL && f_cchDest <= DRM_STRSAFE_MAX_CCH && f_pwszSrc != NULL );

    ChkDR( _DrmStringCatWorkerW(f_pwszDest, f_cchDest, f_pwszSrc) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCatA
**
** Synopsis:    This function is a replacement for strcat. The size, in
**              characters, of the destination buffer is provided to the
**              function to ensure that StringCchCat does not write past
**              the end of this buffer.
**
** Arguments:   [f_pszDest] Pointer to a buffer containing the string
**                          that f_pszSrc is concatenated to, and which
**                          contains the entire resultant string. The
**                          string at f_pszSrc is added to the end of the
**                          string at f_pszDest.
**              [f_cchDest] Size of the destination buffer, in characters.
**                          This value must equal the length of f_pszSrc
**                          plus the length of f_pszDest plus 1 to account
**                          for both strings and the terminating null character.
**                          The maximum number of characters allowed is
**                          DRM_STRSAFE_MAX_CCH.
**              [f_pszSrc]  Pointer to a buffer containing the source string
**                          that is concatenated to the end of f_pszDest. This
**                          source string must be null-terminated.
**
** Returns:     DRM_SUCCESS
**                  Source data was present, the strings were fully concatenated
**                  without truncation, and the resultant destination buffer is
**                  null-terminated.
**              DRM_E_INVALIDARG
**                  The value in f_cchDest is 0 or larger than STRSAFE_MAX_CCH, or
**                  the destination buffer is already full.
**              DRM_E_BUFFERTOOSMALL
**                  The concatenation operation failed due to insufficient buffer
**                  space. The destination buffer contains a truncated,
**                  null-terminated version of the intended result. Where
**                  truncation is acceptable, this is not necessarily a failure
**                  condition.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCatA
(   __out_ecount_z(f_cchDest) DRM_CHAR  *f_pszDest,
    __in                      DRM_DWORD  f_cchDest,
    __in_z              const DRM_CHAR  *f_pszSrc )

{   DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pszDest != NULL && f_cchDest <= DRM_STRSAFE_MAX_CCH && f_pszSrc != NULL );

    ChkDR( _DrmStringCatWorkerA(f_pszDest, f_cchDest, f_pszSrc) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCatNW
**
** Synopsis:    This function is a replacement for strcat. The size, in
**              characters, of the destination buffer is provided to the
**              function to ensure that StringCchCat does not write past
**              the end of this buffer.
**
** Arguments:   [f_pwszDest] Pointer to a buffer containing the string
**                           that f_pwszSrc is concatenated to, and which
**                           contains the entire resultant string. The
**                           string at f_pwszSrc is added to the end of the
**                           string at f_pwszDest.
**              [f_cchDest]  Size of the destination buffer, in characters.
**                           This value must equal the length of f_pwszSrc
**                           plus the length of f_pwszDest plus 1 to account
**                           for both strings and the terminating null character.
**                           The maximum number of characters allowed is
**                           DRM_STRSAFE_MAX_CCH.
**              [f_pwszSrc]  Pointer to a buffer containing the source string
**                           that is concatenated to the end of f_pwszDest. This
**                           source string must be null-terminated.
**              [f_cchToAppend] The maximum number of characters to append to f_pwszDest.
**
** Returns:     DRM_SUCCESS
**                  Source data was present, the strings were fully concatenated
**                  without truncation, and the resultant destination buffer is
**                  null-terminated.
**              DRM_E_INVALIDARG
**                  The value in f_cchDest is 0 or larger than STRSAFE_MAX_CCH, or
**                  the destination buffer is already full.
**              DRM_E_BUFFERTOOSMALL
**                  The concatenation operation failed due to insufficient buffer
**                  space. The destination buffer contains a truncated,
**                  null-terminated version of the intended result. Where
**                  truncation is acceptable, this is not necessarily a failure
**                  condition.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCatNW
(   __out_ecount_z(f_cchDest)        DRM_WCHAR *f_pwszDest,
    __in                             DRM_DWORD  f_cchDest,
    __in_ecount(f_cchToAppend) const DRM_WCHAR *f_pwszSrc,
    __in                             DRM_DWORD  f_cchToAppend )

{   DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pwszDest != NULL && f_cchDest <= DRM_STRSAFE_MAX_CCH && f_pwszSrc != NULL );

    ChkDR( _DrmStringCatNWorkerW(f_pwszDest, f_cchDest, f_pwszSrc, f_cchToAppend) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchCatNA
**
** Synopsis:    This function is a replacement for strcat. The size, in
**              characters, of the destination buffer is provided to the
**              function to ensure that StringCchCat does not write past
**              the end of this buffer.
**
** Arguments:   [f_pszDest] Pointer to a buffer containing the string
**                          that f_pszSrc is concatenated to, and which
**                          contains the entire resultant string. The
**                          string at f_pszSrc is added to the end of the
**                          string at f_pszDest.
**              [f_cchDest] Size of the destination buffer, in characters.
**                          This value must equal the length of f_pszSrc
**                          plus the length of f_pszDest plus 1 to account
**                          for both strings and the terminating null character.
**                          The maximum number of characters allowed is
**                          DRM_STRSAFE_MAX_CCH.
**              [f_pszSrc]  Pointer to a buffer containing the source string
**                          that is concatenated to the end of f_pszDest. This
**                          source string must be null-terminated.
**              [f_cchToAppend] The maximum number of characters to append
**                              to f_pwszDest.
**
** Returns:     DRM_SUCCESS
**                  Source data was present, the strings were fully concatenated
**                  without truncation, and the resultant destination buffer is
**                  null-terminated.
**              DRM_E_INVALIDARG
**                  The value in f_cchDest is 0 or larger than STRSAFE_MAX_CCH, or
**                  the destination buffer is already full.
**              DRM_E_BUFFERTOOSMALL
**                  The concatenation operation failed due to insufficient buffer
**                  space. The destination buffer contains a truncated,
**                  null-terminated version of the intended result. Where
**                  truncation is acceptable, this is not necessarily a failure
**                  condition.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchCatNA
(   __out_ecount_z(f_cchDest)        DRM_CHAR  *f_pszDest,
    __in                             DRM_DWORD  f_cchDest,
    __in_ecount(f_cchToAppend) const DRM_CHAR  *f_pszSrc,
    __in                             DRM_DWORD  f_cchToAppend )

{   DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pszDest != NULL && f_cchDest <= DRM_STRSAFE_MAX_CCH && f_pszSrc != NULL );

    ChkDR( _DrmStringCatNWorkerA(f_pszDest, f_cchDest, f_pszSrc, f_cchToAppend) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchPrintfExW_1
**
** Synopsis:    This function is a replacement for sprintf. It accepts
**              a format string and a list of arguments and returns a
**              formatted string.
**              StringCchPrintfEx adds to the functionality of
**              StringCchPrintf by returning a pointer to the end of
**              the destination string, and by returning the number of
**              characters left unused in that string.
**              This version takes only a single format argument
**
** Arguments:   [f_pwszDest] Pointer to a buffer that receives the formatted,
**                           null-terminated string created from f_pwszFormat
**                           and its arguments
**              [f_cchDest]  Size of the destination buffer, in characters.
**                           This value must equal the length of f_pwszSrc
**                           plus the length of f_pwszDest plus 1 to account
**                           for both strings and the terminating null character.
**                           The maximum number of characters allowed is
**                           DRM_STRSAFE_MAX_CCH.
**              [f_ppwszDestEnd] Address of a pointer to the end of f_pwszDest.
**                           If f_ppszDestEnd is non-NULL and data is copied
**                           into the destination buffer, this points to the
**                           terminating null character at the end of the string.
**              [f_pcchRemaining] Pointer to a variable that indicates the number
**                           of unused characters in f_pwszDest, including the
**                           terminating null character. If f_pcchRemaining is NULL,
**                           the count is not kept or returned.
**              [f_dwFlags]  For future expansion, currently should be 0.
**              [f_pwszFormat] Pointer to a buffer containing a printf-style
**                           format string. This string must be null-terminated.
**              [f_dwValue]  Argument to be inserted into f_pwszFormat
**
** Returns:     DRM_SUCCESS
**                  Source data was present, the strings were fully concatenated
**                  without truncation, and the resultant destination buffer is
**                  null-terminated.
**              DRM_E_INVALIDARG
**                  The value in f_cchDest is 0 or larger than STRSAFE_MAX_CCH, or
**                  the destination buffer is already full.
**              DRM_E_BUFFERTOOSMALL
**                  The copy operation failed due to insufficient buffer
**                  space. The destination buffer contains a truncated,
**                  null-terminated version of the intended result. Where
**                  truncation is acceptable, this is not necessarily a failure
**                  condition.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchPrintfExW_1
(   __out_ecount_z(f_cchDest)                DRM_WCHAR *f_pwszDest,
    __in                                     DRM_DWORD  f_cchDest,
    __deref_opt_out_ecount(*f_pcchRemaining) DRM_WCHAR**f_ppwszDestEnd,
    __out_opt                                DRM_DWORD *f_pcchRemaining,
    __in                                     DRM_DWORD  f_dwFlags,
    __in_z __format_string             const DRM_WCHAR *f_pwszFormat,
    __in                                     DRM_DWORD  f_dwValue )
{   DRM_RESULT dr       = DRM_SUCCESS;
    DRM_BOOL   fIsToken = FALSE;
    DRM_DWORD  dwTokensHit = 0;

    ChkArg( f_pwszDest != NULL );
    ChkArg( f_cchDest <= DRM_STRSAFE_MAX_CCH );
    ChkArg( f_pwszFormat != NULL );
    ChkArg( f_cchDest != 0 );

    while (f_cchDest && (*f_pwszFormat != g_wchNull))
    {
        if( *f_pwszFormat == WCHAR_CAST('%') )
        {
            if( !fIsToken )
            {
                fIsToken = TRUE;
                f_pwszFormat++;
                continue;
            }
            /* Double % means they're escaping a % */
            fIsToken = FALSE;
        }
        else if( fIsToken )
        {
            /* Only parse the first token */
            if( dwTokensHit < 1 )
            {
                DRM_WCHAR wcToken = *f_pwszFormat++;
                DRM_DWORD cchRet = 0;

                switch(wcToken)
                {
                    case WCHAR_CAST('d'):
                        ChkDR( DRM_UTL_NumberToString( f_dwValue, f_pwszDest, f_cchDest, 0, 10, &cchRet ) );
                        f_cchDest -= cchRet;
                        f_pwszDest += cchRet;
                        break;

                    case WCHAR_CAST('x'):
                    case WCHAR_CAST('X'):
                        ChkDR( DRM_UTL_NumberToString( f_dwValue, f_pwszDest, f_cchDest, 0, 16, &cchRet ) );
                        f_cchDest -= cchRet;
                        f_pwszDest += cchRet;
                        break;

                    default:
                        ChkArg( FALSE );
                }

                fIsToken = FALSE;
                dwTokensHit++;

                continue;
            }
            else
            {
                fIsToken = FALSE;
            }
        }

        *f_pwszDest++ = *f_pwszFormat++;
        f_cchDest--;
    }

    if (f_cchDest == 0)
    {
        /* we are going to truncate f_pwszDest */
        f_pwszDest--;
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pwszDest = g_wchNull;

    if( f_ppwszDestEnd != NULL )
    {
        *f_ppwszDestEnd = f_pwszDest;
    }

    if( f_pcchRemaining != NULL )
    {
        *f_pcchRemaining = f_cchDest;
    }

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    DRM_STR_StringCchPrintfW_1
**
** Synopsis:    This function is a replacement for sprintf. It accepts
**              a format string and a list of arguments and returns a
**              formatted string.
**              This version takes only a single format argument
**
** Arguments:   [f_pwszDest] Pointer to a buffer that receives the formatted,
**                           null-terminated string created from f_pwszFormat
**                           and its arguments
**              [f_cchDest]  Size of the destination buffer, in characters.
**                           This value must equal the length of f_pwszSrc
**                           plus the length of f_pwszDest plus 1 to account
**                           for both strings and the terminating null character.
**                           The maximum number of characters allowed is
**                           DRM_STRSAFE_MAX_CCH.
**              [f_pwszFormat] Pointer to a buffer containing a printf-style
**                           format string. This string must be null-terminated.
**              [f_dwValue]  Argument to be inserted into f_pwszFormat
**
** Returns:     DRM_SUCCESS
**                  Source data was present, the strings were fully concatenated
**                  without truncation, and the resultant destination buffer is
**                  null-terminated.
**              DRM_E_INVALIDARG
**                  The value in f_cchDest is 0 or larger than STRSAFE_MAX_CCH, or
**                  the destination buffer is already full.
**              DRM_E_BUFFERTOOSMALL
**                  The copy operation failed due to insufficient buffer
**                  space. The destination buffer contains a truncated,
**                  null-terminated version of the intended result. Where
**                  truncation is acceptable, this is not necessarily a failure
**                  condition.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchPrintfW_1
(   __out_ecount_z(f_cchDest) DRM_WCHAR       *f_pwszDest,
    __in                      DRM_DWORD        f_cchDest,
    __in __format_string      DRM_WCHAR const *f_pwszFormat,
    __in                      DRM_DWORD        f_dwValue )
{
    return DRM_STR_StringCchPrintfExW_1( f_pwszDest, f_cchDest, NULL, NULL, 0, f_pwszFormat, f_dwValue );
}

/**********************************************************************
** Function:    DRM_STR_DstrCat
**
** Synopsis:    Concatenate a DRM string struct to the target DRM string struct
**
** Arguments:   [f_pdstrTarget]  -- target string
**              [f_pdstrAdd]     -- string to be added at the end
**              [f_cchTargetMax] -- maximum number of chars in the target DRM_STRING
**
** Notes:       None.
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_DstrCat(
    __inout    DRM_STRING       *f_pdstrTarget,
    __in const DRM_CONST_STRING *f_pdstrAdd,
    __in const DRM_DWORD         f_cchTargetMax )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdstrTarget             != NULL
         && f_pdstrTarget->pwszString != NULL );

    ChkDRMString( f_pdstrAdd );

    ChkBOOL( (f_pdstrTarget->cchString + f_pdstrAdd->cchString) <= f_cchTargetMax, DRM_E_BUFFERTOOSMALL );

    ChkDR( DRM_STR_StringCchCatNW( f_pdstrTarget->pwszString,
                            f_cchTargetMax,
                            f_pdstrAdd->pwszString,
                            f_pdstrAdd->cchString ) );

    /* adjust destination string length */
    f_pdstrTarget->cchString += f_pdstrAdd->cchString;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_StringCchToLower
**
** Synopsis:    Converts DRM string to lower case DRM string
**
** Arguments:   [f_pwszToLower] -- Pointer to a buffer that contains
**                                 wide character string to convert to lowercase
**              [f_cchToLower]  -- Size of string to convert
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_StringCchToLower(
    __inout_ecount_nz(f_cchToLower) DRM_WCHAR *f_prgwchToLower,
    __in                            DRM_DWORD  f_cchToLower )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  dwCount     = 0;
    DRM_WCHAR *pwchCurrent = f_prgwchToLower;

    ChkArg( f_prgwchToLower!=NULL );

    for ( dwCount = 0; dwCount<f_cchToLower; dwCount++ )
    {
        *pwchCurrent = DRMCRT_towlower( *pwchCurrent );
        pwchCurrent++;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_STR_DstrTowLower
**
** Synopsis:    Converts DRM string to lower case DRM string
**
** Arguments:   [f_pdstrToLower] -- DRM string to convert
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_STR_DstrToLower(
    __inout DRM_STRING *f_pdstrToLower )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDRMString(f_pdstrToLower);

    ChkDR( DRM_STR_StringCchToLower( f_pdstrToLower->pwszString,
                                     f_pdstrToLower->cchString ) );

ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
