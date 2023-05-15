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
#include <drmtoolsutils.h>
#include <drmsecureclock.h>
#include <drmtoolsinit.h>

#define EMBEDDED_WITH_NO_PARAMS 0

#if EMBEDDED_WITH_NO_PARAMS

DRM_LONG  g_argc = 3;

DRM_WCHAR g_Arg1[] = { ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('k', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR g_Arg2[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('j', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR g_Arg3[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('j', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR *g_argv[] = { g_Arg1, g_Arg2, g_Arg3};
#endif

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf( "Syntax: %S [-?] -r:ResponseFile -s:DataStoreFile\n", pwszAppName );
}


static DRM_RESULT ProcessResponse( DRM_CONST_STRING *f_dstrFileResponse,
                             DRM_CONST_STRING *f_dstrFileDataStore )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_BYTE        *pbData         = NULL;
    OEM_FILEHDL      hfileResponse  = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD        cbWritten      = 0;
    DRM_RESULT       drResponse     = DRM_SUCCESS;
    DRM_DWORD        cbFile         = 0;
    DRM_APP_CONTEXT *pcontextMGR    = NULL;
    DRM_BYTE        *pbOpaqueBuffer = NULL;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_dstrFileDataStore, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pcontextMGR ) );

    hfileResponse = Oem_File_Open( NULL, f_dstrFileResponse->pwszString,
                                   OEM_GENERIC_READ,
                                   OEM_FILE_SHARE_READ,
                                   OEM_OPEN_EXISTING,
                                   OEM_ATTRIBUTE_NORMAL );

    if( hfileResponse == OEM_INVALID_HANDLE_VALUE )
    {
        printf( "Input file not opened\n" );
        ChkDR( DRM_E_FAIL );
    }

    Oem_File_GetSize( hfileResponse, &cbFile );

    Oem_File_SetFilePointer( hfileResponse, 0, OEM_FILE_BEGIN, NULL );
    ChkMem( pbData = (DRM_BYTE*) Oem_MemAlloc(cbFile) );

    if( !Oem_File_Read( hfileResponse, pbData, cbFile, &cbWritten )
      || cbWritten != cbFile)
    {
        ChkDR( DRM_E_FILEREADERROR );
    }

    dr = Drm_SecureClock_ProcessResponse( pcontextMGR,
                                          pbData,
                                          cbFile,
                                         &drResponse );

    if ( drResponse != DRM_SUCCESS )
    {
        printf( "Response contains error %d\n", drResponse );
    }
    if ( dr != DRM_SUCCESS )
    {
        printf( "Error Processing response %d\n", dr );
    }
    else
    {
        /* if dr is SUCCESS then report drResponse */
        dr = drResponse;
    }

ErrorExit:
    Oem_File_Close( hfileResponse );
    Oem_MemFree( pbData );

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pcontextMGR );
    return dr;
}

DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_CONST_STRING dstrResponseFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrStoreName    = EMPTY_DRM_STRING;
    DRM_LONG   i   = 0;
    DRM_RESULT dr  = DRM_SUCCESS;

    PrintCopyrightInfo( "Process DRM Devcert Response tool" );

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
        DRM_WCHAR        wchOption        = 0;
        DRM_CONST_STRING dstrParam        = EMPTY_DRM_STRING;

        if( !DRM_CMD_ParseCmdLine( argv[i], &wchOption, &dstrParam, NULL ) )
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
            switch( DRMCRT_towlower(wchOption) )
            {
            case WCHAR_CAST('r'):
                ASSIGN_DRM_STRING( dstrResponseFile, dstrParam );
                break;

            case WCHAR_CAST('s'):
                ASSIGN_DRM_STRING( dstrStoreName, dstrParam );
                break;

            case WCHAR_CAST( '?' ):
                goto _PrintUsage;

            default:
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
    }

    if( dstrResponseFile.cchString == 0 || dstrStoreName.cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }


    ChkDR( ProcessResponse( &dstrResponseFile, &dstrStoreName ) );

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

