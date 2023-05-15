/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmerror.h>
#include <drmutf.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmconstants.h>
#include <drmmathsafe.h>

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S ErrorCode \n", pwszAppName);
}

/******************************************************************************
**
*******************************************************************************
*/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR** argv )
{
    DRM_RESULT       dr                  = DRM_SUCCESS;
    DRM_RESULT       drLookup            = DRM_E_NOT_FOUND;
    DRM_ANSI_STRING  dastrErrorCode      = EMPTY_DRM_STRING;
    const DRM_CHAR  *pszErrorDescription = NULL;
    const DRM_CHAR  *pszErrorName        = NULL;
    DRM_DWORD        cch                 = 0;

    PrintCopyrightInfo( "DRM Error Code Lookup Application" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    ChkArg( argv != NULL );
    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen(argv[1]), &cch ) );
    dr = DRM_STR_UTF16toUTF8( argv[1],
                              cch,
                              dastrErrorCode.pszString,
                              0,
                             &dastrErrorCode.cchString );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( dastrErrorCode.pszString = ( DRM_CHAR * ) Oem_MemAlloc( dastrErrorCode.cchString * SIZEOF( DRM_CHAR ) ) );
        dr = DRM_SUCCESS;
    }
    ChkDR(dr);

    ChkDR( DRM_STR_UTF16toUTF8( argv[1],
                                cch,
                                dastrErrorCode.pszString,
                                0,
                               &dastrErrorCode.cchString ) );

    dastrErrorCode.cchString -= 1; /* lose the Null Terminater from the count */

    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI function.")
    ChkDR( DRM_ERR_GetErrorCodeFromStringA( dastrErrorCode.pszString,
                                            dastrErrorCode.cchString,
                                           &drLookup ) );
    PREFAST_POP

    pszErrorName = DRM_ERR_GetErrorNameFromCode( drLookup, &pszErrorDescription );

    printf( "Error Code:  %#X\nError Name:  %s\nDescription: %s", drLookup, pszErrorName, pszErrorDescription );

ErrorExit:
    SAFE_OEM_FREE( dastrErrorCode.pszString );
    return dr;

_PrintUsage:
    PrintUsage(argv == NULL ? &g_wchNull : argv[0]);
    return dr;
}

