/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmtoolsinit.h>
#include <drmconstants.h>

#if DRM_SUPPORT_PROFILING
#include <drmprofile.h>
#endif


#if EMBEDDED_WITH_NO_PARAMS

DRM_LONG  g_argc = 4;

/* player.exe */
DRM_WCHAR g_Arg1[] = { ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};

/* 1k1s.pkg */
DRM_WCHAR g_Arg2[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('w', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('k', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('p', '\0'), ONE_WCHAR('k', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('\0', '\0')};

/* 1k1s.out */
DRM_WCHAR g_Arg3[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('w', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('k', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('o', '\0'), ONE_WCHAR('u', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};

/* sample.hds */
DRM_WCHAR g_Arg4[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('w', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0', '\0')};
DRM_WCHAR *g_argv[] = { g_Arg1, g_Arg2, g_Arg3, g_Arg4 };

#endif /* EMBEDDED_WITH_NO_PARAMS */


/******************************************************************************
** Handle callbacks from Drm_Reader_Bind
*******************************************************************************
*/
DRM_RESULT DRM_CALL BindCallback(
    __in const DRM_VOID                 *f_pvPolicyCallbackData,
    __in       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in const DRM_VOID                 *f_pv )
{
    return DRMTOOLS_PrintPolicyCallback( f_pvPolicyCallbackData, f_dwCallbackType );
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static DRM_RESULT
DoPlay ( const DRM_CONST_STRING *pdstrContentFilename,
         const DRM_CONST_STRING *pdstrOutputFileName,
         const DRM_CONST_STRING *pdstrDataStoreFile,
         const DRM_CONST_STRING *pdstrAction,
               DRM_BOOL fDoCommit )
{
    DRM_RESULT                   dr                  = DRM_SUCCESS;
    DRM_DWORD                    dw;
    const DRM_CONST_STRING      *apdcsRights [1]     = { NULL };
    DRM_APP_CONTEXT             *poManagerContext    = NULL;
    DRM_BYTE                    *pbOpaqueBuffer      = NULL;
    DRM_DECRYPT_CONTEXT          oDecryptContext;
    DRM_BOOL                     fDecryptInitialized = FALSE;
    DRM_LICENSE_STATE_DATA       statedatavales[NO_OF(apdcsRights)];
    DRM_DWORD                    cbWritten           = 0;
    DRM_BYTE                    *pbXML               = NULL;
    DRM_BYTE                     rgbBuffer[__CB_DECL(4096)];
    OEM_FILEHDL                  Outfp               = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD                    cbRead              = 0;
    DRM_WCHAR                   *pszFile             = NULL;
    DRM_BYTE                    *pbRevocationBuffer  = NULL;
    DRM_ENVELOPED_FILE_CONTEXT   oEnvFile;
    DRM_BOOL                     fEnvFileOpen        = FALSE;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, pdstrDataStoreFile, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poManagerContext ) );

    pszFile = (DRM_WCHAR *) pbOpaqueBuffer;

    apdcsRights[0] = pdstrAction;

    ChkDR( DRM_STR_StringCchCopyNW( pszFile,
                                   SIZEOF( DRM_APP_CONTEXT ) / SIZEOF( DRM_WCHAR ),
                                   pdstrOutputFileName->pwszString,
                                   pdstrOutputFileName->cchString ) );

    Outfp = Oem_File_Open (NULL,
                           pszFile,
                           OEM_GENERIC_WRITE,
                           OEM_FILE_SHARE_NONE,
                           OEM_CREATE_ALWAYS,
                           OEM_ATTRIBUTE_NORMAL);

    if( OEM_INVALID_HANDLE_VALUE == Outfp )
    {
        printf( "Output file not opened\n" );
        ChkDR( DRM_E_FAIL );
    }

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        ChkMem( pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );
        ChkDR( Drm_Revocation_SetBuffer( poManagerContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    }

    ChkDR( Drm_Envelope_Open( poManagerContext, NULL, pdstrContentFilename->pwszString, &oEnvFile) );
    fEnvFileOpen = TRUE;

    ChkDR( Drm_LicenseQuery_GetState( poManagerContext, apdcsRights, NO_OF( apdcsRights ), statedatavales, (DRMPFNPOLICYCALLBACK)BindCallback, NULL ) );

    printf( ( "  Content rights information:\n" ) );

    for( dw = 0; dw < NO_OF( apdcsRights ); dw++ )
    {
        printf( ( "    %ws:\n" ), apdcsRights[dw]->pwszString );
        DRMTOOLS_PrintLicenseStateData( ("      "), &(statedatavales[ dw ]) );
    }
    printf("\r\n");

    ChkDR( Drm_Reader_Bind (poManagerContext,
                          apdcsRights,
                          NO_OF (apdcsRights),
                          (DRMPFNPOLICYCALLBACK)BindCallback,
                          NULL,
                          &oDecryptContext ) );

    fDecryptInitialized = TRUE;

    ChkDR( Drm_Envelope_InitializeRead( &oEnvFile, &oDecryptContext ) );

    if( fDoCommit )
    {
        /* Decrypt and play content */
        ChkDR( Drm_Reader_Commit( poManagerContext, NULL, NULL ) );
    }

    do
    {
        ChkDR( Drm_Envelope_Read( &oEnvFile, rgbBuffer, SIZEOF( rgbBuffer ), &cbRead ) );

        if ( !Oem_File_Write(Outfp, rgbBuffer, cbRead, &cbWritten) || cbWritten != cbRead )
        {
            printf( "Failed to write output data 0x%x\n", dr);
            ChkDR( DRM_E_FAIL );
        }
    } while( cbRead == SIZEOF( rgbBuffer ) );

ErrorExit:
    if( fDecryptInitialized )
    {
        Drm_Reader_Close( &oDecryptContext );
    }

    if (Outfp != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close (Outfp);
    }

    SAFE_OEM_FREE( pbXML );

    if( fEnvFileOpen )
    {
        Drm_Envelope_Close( &oEnvFile );
    }

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poManagerContext );

    SAFE_OEM_FREE( pbRevocationBuffer );

    DRMTOOLS_PrintErrorCode( dr );

    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-?] -c:ContentFile -o:OutputFile -s:StoreName [-n (skips calling Drm_Reader_Commit)] [-t (flags the secure clock as being set)] [-a:Action (default is Play)] [-m:NumSeconds (changes the device time before playing)] \n", pwszAppName);
}

/******************************************************************************
**
*******************************************************************************
*/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR** argv )
{
    DRM_CONST_STRING dstrStoreName   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrContentFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutputFile  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAction      = g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_LONG         lTimeOffset     = 0;
    DRMSYSTEMTIME    oSysTime        = { 0 };
    DRMFILETIME      oFileTime       = { 0 };
    DRM_UINT64       ui64Time        = DRM_UI64LITERAL(0, 0);
    DRM_BOOL         fDoCommit       = TRUE;
    DRM_LONG         i               = 0;
    DRM_RESULT       dr              = DRM_SUCCESS;

    PrintCopyrightInfo( "Sample DRM player application" );

#if EMBEDDED_WITH_NO_PARAMS
    argc = g_argc;
    argv = g_argv;
#endif

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    ChkArg( argv != NULL );
    for( i = 1; i<argc; i++ )
    {
        DRM_WCHAR wchOptionChar;
        DRM_CONST_STRING dstrParam;
        if ( !DRM_CMD_ParseCmdLine(argv[i], &wchOptionChar, &dstrParam, NULL) )
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
            wchOptionChar = DRMCRT_towlower( wchOptionChar );
            switch( wchOptionChar )
            {
            case WCHAR_CAST('c'):
                dstrContentFile.pwszString = dstrParam.pwszString;
                dstrContentFile.cchString  = dstrParam.cchString;
                break;
            case WCHAR_CAST('o'):
                dstrOutputFile.pwszString = dstrParam.pwszString;
                dstrOutputFile.cchString  = dstrParam.cchString;
                break;
            case WCHAR_CAST('s'):
                dstrStoreName.pwszString = dstrParam.pwszString;
                dstrStoreName.cchString  = dstrParam.cchString;
                break;
            case WCHAR_CAST('n'):
                fDoCommit = FALSE;
                break;
            case WCHAR_CAST('t'):
                Oem_Clock_SetResetState( NULL, FALSE );
                break;
            case WCHAR_CAST('a'):
                dstrAction.pwszString = dstrParam.pwszString;
                dstrAction.cchString  = dstrParam.cchString;
                break;
            case WCHAR_CAST('m'):
                ChkDR( DRMCRT_wcsntol( dstrParam.pwszString, dstrParam.cchString, &lTimeOffset ) );
                break;
            case WCHAR_CAST('?'):
                goto _PrintUsage;
            default:
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
    }

    if( !dstrContentFile.cchString || !dstrOutputFile.cchString || !dstrStoreName.cchString )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    /*
    ** Change the device time before playing
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &oFileTime );

    FILETIME_TO_UI64( oFileTime, ui64Time );
    if( lTimeOffset > 0 )
    {
        ui64Time = DRM_UI64Add( ui64Time, DRM_UI64Mul( DRM_UI64( lTimeOffset ), DRM_UI64( 10000000L ) ) );
    }
    else
    {
        ui64Time = DRM_UI64Sub( ui64Time, DRM_UI64Mul( DRM_UI64( -lTimeOffset ), DRM_UI64( 10000000L ) ) );
    }
    UI64_TO_FILETIME( ui64Time, oFileTime );

    ChkArg( Oem_Clock_FileTimeToSystemTime( &oFileTime, &oSysTime ) );
    Oem_Clock_SetSystemTime( NULL, &oSysTime );

#if DRM_SUPPORT_PROFILING
    ChkDR( DRM_Start_Profiling(NULL,NULL) );
#endif

    ChkDR( DoPlay( &dstrContentFile, &dstrOutputFile, &dstrStoreName, &dstrAction, fDoCommit ) );

#if DRM_SUPPORT_PROFILING
    ChkDR( DRM_Stop_Profiling() );
#endif
ErrorExit:

    /*
    ** Change the device time back
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &oFileTime );

    FILETIME_TO_UI64( oFileTime, ui64Time );
    if( lTimeOffset > 0 )
    {
        ui64Time = DRM_UI64Sub( ui64Time, DRM_UI64Mul( DRM_UI64( lTimeOffset ), DRM_UI64( 10000000L ) ) );
    }
    else
    {
        ui64Time = DRM_UI64Add( ui64Time, DRM_UI64Mul( DRM_UI64( -lTimeOffset ), DRM_UI64( 10000000L ) ) );
    }
    UI64_TO_FILETIME( ui64Time, oFileTime );

    if ( DRM_SUCCEEDED( Oem_Clock_FileTimeToSystemTime( &oFileTime, &oSysTime ) ) )
    {
        Oem_Clock_SetSystemTime( NULL, &oSysTime );
    }

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
    PrintUsage(argv == NULL ? &g_wchNull : argv[0]);
    return dr;
}

