/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmsecureclock.h>
#include <drmsecureclockstatus.h>
#include <drmtoolsutils.h>
#include <drmtoolsinit.h>

#define EMBEDDED_WITH_NO_PARAMS 0

#if EMBEDDED_WITH_NO_PARAMS

DRM_LONG  g_argc = 4;

DRM_WCHAR g_Arg1[] = { ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('k', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR g_Arg2[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('j', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR g_Arg3[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('j', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR g_Arg4[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('j', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR *g_argv[] = { g_Arg1, g_Arg2, g_Arg3, g_Arg4 };
#endif

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-?] -l:ChallengeOutputFile -s:DataStoreFile [-u:URLFile]\n", pwszAppName);
}


/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static DRM_RESULT GenerateChallenge(
    DRM_CONST_STRING *f_pdstrOutputFileName,
    DRM_CONST_STRING *f_pdstrFileURL,
    DRM_CONST_STRING *f_pdstrDataStoreFile)
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_APP_CONTEXT *poManagerContext   = NULL;
    DRM_BYTE        *pbOpaqueBuffer     = NULL;
    DRM_DWORD        cchChallenge       = 0;
    DRM_DWORD        cchURL             = 0;
    DRM_CHAR        *pszChallenge       = NULL;
    DRM_DWORD        cchSecTime         = 0;
    DRM_WCHAR       *pwszSecTime        = NULL;
    DRM_BYTE        *pbTimeStatus       = NULL;
    DRM_DWORD        cbTimeStatus       = 0;
    OEM_FILEHDL      Outfp              = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD        iIO                = 0;
    DRM_DWORD        dwFlag             = 0;
    DRM_WCHAR       *pwszURL            = NULL;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_pdstrDataStoreFile, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poManagerContext ) );

    /* strings from argv's will be NUL-terminated by the runtime */

    Outfp = Oem_File_Open( NULL, f_pdstrOutputFileName->pwszString,
                                  OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                                  OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                  OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );

    if( Outfp == OEM_INVALID_HANDLE_VALUE )
    {
        printf( "Output file not opened\n" );
        ChkDR( DRM_E_FAIL );
    }

    dr = Drm_SecureClock_GetValue( poManagerContext, pwszSecTime, &cchSecTime, &dwFlag, pbTimeStatus, &cbTimeStatus );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        goto ErrorExit;
    }
    ChkMem( pwszSecTime = (DRM_WCHAR*) Oem_MemAlloc( cchSecTime * SIZEOF( DRM_WCHAR ) ) );
    ChkMem( pbTimeStatus = (DRM_BYTE*) Oem_MemAlloc( cbTimeStatus ) );
    MEMSET( pwszSecTime, 'a', cchSecTime * SIZEOF( DRM_WCHAR ) );
    MEMSET( pbTimeStatus, 'b', cbTimeStatus );
    ChkDR( Drm_SecureClock_GetValue( poManagerContext, pwszSecTime, &cchSecTime, &dwFlag, pbTimeStatus, &cbTimeStatus ) );

    if ( DRM_CLK_NOT_SET == dwFlag || DRM_CLK_NEEDS_REFRESH == dwFlag )
    {
        dr = Drm_SecureClock_GenerateChallenge( poManagerContext,
                                                NULL,
                                               &cchURL,
                                                NULL,
                                               &cchChallenge );

        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            goto ErrorExit;
        }
        ChkMem( pszChallenge = (DRM_CHAR *)  Oem_MemAlloc(cchChallenge) );
        ChkMem( pwszURL      = (DRM_WCHAR *) Oem_MemAlloc(cchURL * SIZEOF (DRM_WCHAR)) );

        ChkDR( Drm_SecureClock_GenerateChallenge( poManagerContext,
                                                  pwszURL,
                                                 &cchURL,
                                                  (DRM_BYTE *) pszChallenge,
                                                 &cchChallenge));


    }

    if( !Oem_File_Write( Outfp, pszChallenge , cchChallenge, &iIO)
      || iIO != cchChallenge)
    {
        printf( "Failed to write challenge\n" );
        ChkDR( DRM_E_FAIL );
    }

    if( f_pdstrFileURL->cchString > 0 )
    {
        OEM_FILEHDL hfileURL = OEM_INVALID_HANDLE_VALUE;

        hfileURL = Oem_File_Open( NULL, f_pdstrFileURL->pwszString,
                                          OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                                          OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                          OEM_CREATE_ALWAYS,
                                          OEM_ATTRIBUTE_NORMAL );

        if( hfileURL != OEM_INVALID_HANDLE_VALUE )
        {
            DRM_DWORD cbURL = cchURL * SIZEOF (DRM_WCHAR);

            if( !Oem_File_Write(hfileURL, pwszURL, cbURL, &iIO)
              || iIO != cbURL)
            {
                printf( "Failed to write URL file\n" );
                ChkDR( DRM_E_FAIL );
            }

            Oem_File_Close( hfileURL );
        }
        else
        {
            printf( "URL file not opened\n" );
            ChkDR( DRM_E_FAIL );
        }
    }

ErrorExit:

    if( Outfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( Outfp );
    }

    if( pszChallenge != NULL )
    {
        Oem_MemFree( pszChallenge );
    }

    if( pwszSecTime != NULL )
    {
        Oem_MemFree( pwszSecTime );
    }

    if( pbTimeStatus != NULL )
    {
        Oem_MemFree( pbTimeStatus );
    }

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poManagerContext );
    return dr;
}


/******************************************************************************
**
*******************************************************************************
*/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_CONST_STRING dstrOutputFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrStoreName  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileURL    = EMPTY_DRM_STRING;
    DRM_LONG   i   = 0;
    DRM_RESULT dr  = DRM_SUCCESS;

    PrintCopyrightInfo( "Generate Secure Clock Challenge" );

    if( !DRM_CLK_IsSecureClockSupported() )
    {
        printf( "This tool requires that the real secure clock library be linked to it.\r\n" );
        ChkDR( DRM_E_CLK_NOT_SUPPORTED );
    }

#if EMBEDDED_WITH_NO_PARAMS
    argc = g_argc;
    argv = g_argv;
#endif

    if( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    for( i = 1; i < argc; i++ )
    {
        DRM_WCHAR        wchOption = 0;
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

        if (! DRM_CMD_ParseCmdLine(argv[i], &wchOption, &dstrParam, NULL) )
        {
           /* See if it's a default parameter that PK knows how to handle */
            if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[i] ) ) )
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
        else
        {
            wchOption = DRMCRT_towlower( wchOption );

            switch( wchOption )
            {
            case WCHAR_CAST('l'):
                dstrOutputFile = dstrParam;
                break;
            case WCHAR_CAST('s'):
                dstrStoreName = dstrParam;
                break;
            case WCHAR_CAST('u'):
                dstrFileURL = dstrParam;
                break;
            case WCHAR_CAST( '?' ):
                goto _PrintUsage;
            default:
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
    }

    if( dstrOutputFile.cchString == 0
     || dstrStoreName .cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ChkDR( GenerateChallenge( &dstrOutputFile, &dstrFileURL, &dstrStoreName ) );

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;

_PrintUsage:
    PrintUsage( argv[0] );
    return dr;
}

