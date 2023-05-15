/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PKTestNameSpace.h>
#include <drmmathsafe.h>

USING_PKTEST_NAMESPACE;

/*****************************************************************************
** Function: CompareDSTRToWCHAR
**
** Synopsis: Does a compare of a DRM_CONST_STRING with a WCHAR.
**
** Arguments:   f_dstrLhs : pointer to DRM_CONST_STRING.
**              f_pwszRhs : pointer to the WCHAR string.
**
** Returns:
**          DRM_SUCCESS if the same, DRM_S_FALSE if not the same
*****************************************************************************/
DRM_RESULT DRM_CALL CompareDSTRToWCHAR( __in const DRM_CONST_STRING *f_dstrLhs, __in const DRM_WCHAR *f_pwszRhs)
{
    DRM_RESULT dr = DRM_S_FALSE;

    ChkArg( f_dstrLhs != NULL );
    ChkArg( f_pwszRhs != NULL );

    if( f_dstrLhs->cchString != DRMCRT_wcslen( f_pwszRhs ) )
    {
        /* Do not use ChkBOOL because it logs and this is not an error condition */
        goto ErrorExit;
    }

    if( DRMCRT_wcsncmp(f_dstrLhs->pwszString, f_pwszRhs, f_dstrLhs->cchString) != 0 )
    {
        /* Do not use ChkBOOL because it logs and this is not an error condition */
        goto ErrorExit;
    }

    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRMCRT_WtoWORD
**
** Synopsis: Converts DRM_WCHAR string to a DRM_WORD value.
**
** Arguments:   f_pwszStringInput : the string to convert.
**              f_cchStringInput  : the number of characters to convert
**              f_base            : the base of string
**              f_pwValue         : where to put the DRM_WORD value
**              f_pcchValue       : the actual number of characters converted.
**
** Returns:
**          DRM_SUCCESS if OK
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRMCRT_WtoWORD(
        __in_ecount( f_cchStringInput ) const DRM_WCHAR *f_pwszStringInput,
        __in                                  DRM_DWORD  f_cchStringInput,
        __in                                  DRM_DWORD  f_base,
        __out                                 DRM_WORD  *f_pwValue,
        __out_opt                             DRM_DWORD *f_pcchValue )
{
    DRM_DWORD  dwTemp = 0;
    DRM_RESULT dr     = DRM_SUCCESS;

    ChkArg( f_pwszStringInput != NULL );
    ChkArg( f_pwValue != NULL );

    ChkDR( DRMCRT_WtoDWORD( f_pwszStringInput, f_cchStringInput, f_base, &dwTemp, f_pcchValue ) );
    *f_pwValue = (DRM_WORD)dwTemp;
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: TST_UTL_wcsistr
**
** Synopsis: Finds a string in another string using case insensitive search.
**
** Arguments:   f_pwszStr        : The string to search in.
**              f_pwszStrSearch  : The string to search for.
**
**Returns:
**          pointer to the string found.  NULL if not found
*****************************************************************************/
const DRM_WCHAR * DRM_CALL TST_UTL_wcsistr(__in const DRM_WCHAR *f_pwszStr, __in const DRM_WCHAR *f_pwszStrSearch)
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    const DRM_WCHAR *pwszRetStr  = NULL;
    DRM_DWORD        dwLenStr    = 0;
    DRM_DWORD        dwLenSearch = 0;
    DRM_DWORD        index       = 0;

    ChkArg( f_pwszStr != NULL );
    ChkArg( f_pwszStrSearch != NULL );

    dwLenStr = (DRM_DWORD)DRMCRT_wcslen( f_pwszStr );
    dwLenSearch = (DRM_DWORD)DRMCRT_wcslen( f_pwszStrSearch );

    ChkArg( dwLenStr != 0 );
    ChkArg( dwLenSearch != 0 );

    if( dwLenSearch > dwLenStr )
    {
        /* Do not use ChkBOOL because it logs and this is not an error condition */
        goto ErrorExit;
    }

    for( index = 0; index < dwLenStr - dwLenSearch + 1; index++)
    {
        if( DRMCRT_wcsnicmp( &f_pwszStr[index], f_pwszStrSearch, dwLenSearch ) == 0)
        {
            pwszRetStr = &f_pwszStr[index];
            break;
        }
    }

ErrorExit:
    return pwszRetStr;
}

/*****************************************************************************
** Function: TST_UTL_wcsncat
**
** Synopsis: concatenates two NULL terminated strings upto a maximum number
**
** Arguments:   f_pwszTarget  : The string to concatenate to.
**              f_pwszSource  : The string to copy from.
**              f_cchLen      : The maximum number of characters to add
**
** Returns:
**          DRM_SUCCESS if no problems
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_wcsncat( __out DRM_WCHAR *f_pwszTarget, __in const DRM_WCHAR *f_pwszSource, __in DRM_DWORD f_cchLen )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    ChkArg( f_pwszTarget != NULL );
    ChkArg( f_pwszSource != NULL );

    while( *f_pwszTarget != 0 )
    {
        f_pwszTarget++;
    }

    while( f_cchLen > 0 && *f_pwszSource != 0 )
    {
        *f_pwszTarget = *f_pwszSource;
        f_cchLen--;
        f_pwszTarget++;
        f_pwszSource++;
    }
    *f_pwszTarget = 0;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: TST_UTL_wcsncpy
**
** Synopsis: Copies a given number of characters from a NULL terminated string
**           to another string.
**
** Arguments:   f_pwszTarget  : The string to copy to.
**              f_pwszSource  : The string to copy from.
**              f_cchLen      : The maximum number of characters to copy
**
** Returns:
**          DRM_SUCCESS if no problems
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_wcsncpy( __out DRM_WCHAR *f_pwszTarget, __in const DRM_WCHAR *f_pwszSource, __in DRM_DWORD f_cchLen )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( f_pwszTarget != NULL );

    *f_pwszTarget = 0;
    dr = TST_UTL_wcsncat( f_pwszTarget, f_pwszSource, f_cchLen );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function: CopyDRMString
**
** Synopsis: Makes a hard copy of a DRM_CONST_STRING.
**           This function allocates the target string buffer using Oem_MemAlloc.
**           It will need to be freed using SAFE_OEM_FREE or FREEDRMSTRING
**
** Arguments:   f_pdstrTarget : The string to copy to.  Must be freed
**              f_pwszSource  : The string to copy from.
**
** Returns:
**          DRM_SUCCESS if no problems
*****************************************************************************/
DRM_RESULT DRM_CALL CopyDRMString(  __out DRM_CONST_STRING      *f_pdstrTarget,
                                    __in const DRM_CONST_STRING *f_pdstrSource )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  len = 0;

    len = f_pdstrSource->cchString * SIZEOF( DRM_WCHAR );

    if ( len > 0 && NULL != f_pdstrSource->pwszString )
    {
        f_pdstrTarget->pwszString = ( const DRM_WCHAR* )Oem_MemAlloc( len + SIZEOF( DRM_WCHAR ) );  /* allocate size + 1. */
        ChkMem( f_pdstrTarget->pwszString );
        DRMCRT_memset( ( void* )f_pdstrTarget->pwszString, 0, len + SIZEOF( DRM_WCHAR ) );  /* clear size + 1 */
        f_pdstrTarget->cchString = f_pdstrSource->cchString;

        DRMCRT_memcpy( ( DRM_WCHAR* )f_pdstrTarget->pwszString, f_pdstrSource->pwszString, len );
    }
ErrorExit:

    return dr;
}

