/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#define DRM_BUILDING_STARTUP_C

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <drmcrt.h>
#include <drmutf.h>
#include <drmcmdlnpars.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <oemcommon.h>

#include <drmlastinclude.h>

/*****************************************************************************
** Function: main
**
** Synopsis: This function is called during startup if the executable was
**           compiled for ANSI character sets.
**
** Arguments:
**           argc               -- Number of arguments.
**           argv               -- Array of pointers to argument strings.
**
** Returns:
**           Integer value to return to the shell.
*****************************************************************************/
int DRM_CCALL main(
    int argc,
    __in_ecount(argc) char** argv )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    int        i            = 0;
    int        iRet         = 0;
    DRM_DWORD  cchConverted = 0;
    DRM_DWORD  cchNeeded    = 0;
    wchar_t  **ppwszArgv    = NULL;
    wchar_t   *wszArg       = NULL;

    /*
    ** Create a wide-version of the argv array.
    */
    ChkOverflow( (DRM_DWORD)argc * sizeof( wchar_t * ), (DRM_DWORD)argc );
    ChkMem( ppwszArgv = (wchar_t **)malloc( argc * sizeof( wchar_t * ) ) );

    ZEROMEM( (void *)ppwszArgv, argc * sizeof( wchar_t * ) );

    for ( i = 0; i < argc; i++ )
    {
        /*
        ** Get the number of wide-char elements needed for the expanded string.
        ** Negative 1 is the documented error value.
        */
        dr = DRM_STR_UTF8toUTF16( argv[i], 0, DRMCRT_strlen( argv[i] ), NULL, &cchNeeded );
        ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_INVALIDARG );

        /*
        ** Increment the count to account for the terminating NULL.
        */
        ChkOverflow( ( cchNeeded + 1 ), cchNeeded );
        cchNeeded += 1;

        /*
        ** Since this is the ANSI OEM layer (and a command-line startup lib),
        ** malloc is available. There is no reason to use SIZEOF(...) as
        ** DRM_Main doesn't take a closely-packed memory buffer of chars, but
        ** rather an array, packed at the C-compiler's discretion. Furthermore,
        ** we ensure Oem_MemAlloc won't be called until the OEM layer is
        ** initialized.
        */
        ChkOverflow( cchNeeded * sizeof( wchar_t ), cchNeeded );
        ChkMem( wszArg = (wchar_t *)malloc( cchNeeded * sizeof( wchar_t ) ) );
        ZEROMEM( wszArg, cchNeeded * sizeof( wchar_t ) );

        cchConverted = cchNeeded;
        ChkDR( DRM_STR_UTF8toUTF16( argv[i], 0, DRMCRT_strlen( argv[i] ), wszArg, &cchConverted ) );

        if( cchConverted >= cchNeeded )
        {
            /*
            ** This could only happen if the string changed ... which cannot
            ** happen.
            */
            ChkDR( DRM_E_LOGICERR );
        }

        ppwszArgv[i] = wszArg;
    }

    /*
    ** Invoke the DRM_Main function now that everything has been converted.
    */
    iRet = (int) DRM_Main( (DRM_LONG)argc , (DRM_WCHAR **)ppwszArgv );

ErrorExit:

    if ( ppwszArgv != NULL )
    {
        for ( i = 0; i < argc; i++ )
        {
            if ( ppwszArgv[i] != NULL )
            {
                free( ppwszArgv[i] );
            }
        }

        free( ppwszArgv );
    }

    if ( DRM_FAILED( dr ) )
    {
        iRet = 0;
    }

    return iRet;
}


/*****************************************************************************
** Function: wmain
**
** Synopsis: This function is called during startup if the executable was
**           compiled for UNICODE character sets.
**
** Arguments:
**           argc               -- Number of arguments.
**           argv               -- Array of pointers to argument strings.
**
** Returns:
**           Integer value to return to the shell.
*****************************************************************************/
int DRM_CCALL wmain(
    int argc,
    __in_ecount(argc) wchar_t** argv )
{
    return (int) DRM_Main( (DRM_LONG) argc, (DRM_WCHAR**) argv );
}

