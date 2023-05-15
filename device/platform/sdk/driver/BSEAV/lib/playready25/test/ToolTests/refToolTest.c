/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <ToolTestAPI.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

DRM_LONG g_lOffset = 0;

static const DRM_WCHAR c_rgwchTraceDat[] = {
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'c', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'a', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '\0', '\0' ) };
static DRM_CONST_STRING c_dstrTraceDat   = CREATE_DRM_STRING( c_rgwchTraceDat );

static const DRM_WCHAR c_rgwchTraceTxt[] = {
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'c', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'x', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '\0', '\0' ) };
static DRM_CONST_STRING c_dstrTraceTxt   = CREATE_DRM_STRING( c_rgwchTraceTxt );

static DRM_CHAR       *prgchMessageToTrace[]     = {
    "Basic trace message",
    "%d",
    "%s complex tracing with a hex int (0x%x) and even a wide string \"%S\"!" };
static DRM_CHAR       *prgchMessageToVerify[]    = {
    "Basic trace message",
    "1234",
    "This is some complex tracing with a hex int (0x162e) and even a wide string \"trace.dat\"!" };
#define TEST_TRACE_0 DRM_TRACE_NO_SCOPE( ( pchMessageToTrace ) )
#define TEST_TRACE_1 DRM_TRACE_NO_SCOPE( ( pchMessageToTrace, 1234 ) )
#define TEST_TRACE_2 DRM_TRACE_NO_SCOPE( ( pchMessageToTrace, "This is some", 5678, c_rgwchTraceDat ) )

/******************************************************************************
**
** Function:  CreateTraceFile
**
** Synopsis:  Creates a trace file that can be verified by VerifyTraceFile once processed by tracetool.exe
**
** Arguments: [argc] : Number of arguments
**            [argv] : Ignored
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS            if everything is OK
**
******************************************************************************/
DRM_RESULT DRM_CALL CreateTraceFile( long argc, __in_ecount( argc ) char **argv )
{
#if DRM_SUPPORT_TRACING
    DRM_RESULT      dr                  = DRM_SUCCESS;
    OEM_FILEHDL     fp                  = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE        rgbRead[500]        = {0};
    DRM_DWORD       cbExpected          = 0;
    DRM_DWORD       cbToRead            = 0;
    DRM_DWORD       cbRead              = 0;
    DRM_DWORD       idx                 = 0;
    DRM_BYTE       *pb                  = NULL;

    ChkArg( NO_OF( prgchMessageToTrace) == NO_OF( prgchMessageToVerify ) );

    /* Start tracing */
    DRM_TRACE_START( NULL, &c_dstrTraceDat );

    /* Turn off all channels */
    DRM_TRACE_CHANNEL_OFF( MAX_UNSIGNED_TYPE(DRM_DWORD) );

    /* Turn on the file channel */
    DRM_TRACE_CHANNEL_ON( TRACE_FILE_CHANNEL );

    /* Trace each message */
    for( idx = 0; idx < NO_OF( prgchMessageToTrace ); idx++ )
    {
        DRM_CHAR *pchMessageToTrace = prgchMessageToTrace[idx];
        DRM_CHAR *pchMessageToVerify = prgchMessageToVerify[idx];
        switch( idx )
        {
        case 0: TEST_TRACE_0; break;
        case 1: TEST_TRACE_1; break;
        case 2: TEST_TRACE_2; break;
        default: ChkArg( FALSE ); break;
        }
        cbExpected += SIZEOF( TRACE_HEADER ) + SIZEOF( DRM_DWORD ) + DRMCRT_strlen( pchMessageToVerify );
    }

    /* Stop tracing - dev code invocation complete */
    DRM_TRACE_STOP();

    /* Now we verify what was traced */

    /* Open the trace.dat file */
    fp = Oem_File_Open(
        NULL,
        c_dstrTraceDat.pwszString,
        OEM_GENERIC_READ,
        OEM_FILE_SHARE_NONE,
        OEM_OPEN_EXISTING,
        OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILENOTFOUND );

    /* Verify the size of the trace.dat file */
    ChkBOOL( Oem_File_GetSize( fp, &cbToRead ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( cbToRead == cbExpected, DRM_E_FILE_READ_ERROR );

    /* Read the entire trace.dat file */
    ChkBOOL( Oem_File_Read( fp, rgbRead, cbToRead, &cbRead ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( cbToRead == cbRead, DRM_E_FILE_READ_ERROR );

    pb = rgbRead;
    /* Verify each message */
    for( idx = 0; idx < NO_OF( prgchMessageToVerify ); idx++ )
    {
        DRM_CHAR        *pchMessageToVerify = prgchMessageToVerify[idx];
        DRM_DWORD        cchExpected        = 0;
        DRM_DWORD        cbOffsetOf_cch     = DRM_OFFSET_OF( TRACE_PACKET, m_oBody ) + DRM_OFFSET_OF( TRACE_STRING_DATA, m_cchData );
        DRM_DWORD        cbOffsetOf_rgch    = DRM_OFFSET_OF( TRACE_PACKET, m_oBody ) + DRM_OFFSET_OF( TRACE_STRING_DATA, m_rgchData );

        /* Verify messsage length */
        cchExpected = DRMCRT_strlen( pchMessageToVerify );
        pb += cbOffsetOf_cch;   /* Ignore the header information */
        ChkBOOL( MEMCMP( pb, &cchExpected, SIZEOF(DRM_DWORD) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
        pb -= cbOffsetOf_cch;

        /* Verify messsage contents */
        pb += cbOffsetOf_rgch;  /* Ignore the header information and the length we already verified */
        ChkBOOL( MEMCMP( pb, pchMessageToVerify, cchExpected ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Move on to next message */
        pb += cchExpected;
    }

ErrorExit:
    if( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }

    return dr;
#else  /* DRM_SUPPORT_TRACING */
    return DRM_E_NOTIMPL;
#endif /* DRM_SUPPORT_TRACING */
}

/******************************************************************************
**
** Function:  VerifyTraceFile
**
** Synopsis:  Verifies a trace file that was created by CreateTraceFile and processed by tracetool.exe
**
** Arguments: [argc] : Number of arguments
**            [argv] : Ignored
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS            if everything is OK
**
******************************************************************************/
DRM_RESULT DRM_CALL VerifyTraceFile( long argc, __in_ecount( argc ) char **argv )
{
#if DRM_SUPPORT_TRACING
    DRM_RESULT      dr                  = DRM_SUCCESS;
    OEM_FILEHDL     fp                  = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE        rgbRead[500]        = {0};
    DRM_DWORD       cbToRead            = 0;
    DRM_DWORD       cbRead              = 0;
    DRM_DWORD       idx                 = 0;
    DRM_BYTE       *pb                  = NULL;

    /* Open the trace.txt file */
    fp = Oem_File_Open(
        NULL,
        c_dstrTraceTxt.pwszString,
        OEM_GENERIC_READ,
        OEM_FILE_SHARE_NONE,
        OEM_OPEN_EXISTING,
        OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILENOTFOUND );

    ChkBOOL( Oem_File_GetSize( fp, &cbToRead ), DRM_E_FILE_READ_ERROR );

    /* Read the entire trace.txt file */
    ChkBOOL( Oem_File_Read( fp, rgbRead, cbToRead, &cbRead ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( cbToRead == cbRead, DRM_E_FILE_READ_ERROR );

    pb = rgbRead;
    /* Verify each message */
    for( idx = 0; idx < NO_OF( prgchMessageToVerify ); idx++ )
    {
        DRM_CHAR    *pchMessageToVerify = prgchMessageToVerify[idx];
        DRM_DWORD    cchExpected        = 0;

        /* Make sure we didn't reach end of what we read */
        ChkBOOL( pb < rgbRead + cbRead, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Verify messsage contents */
        cchExpected = DRMCRT_strlen( pchMessageToVerify );
        ChkBOOL( MEMCMP( pb, pchMessageToVerify, cchExpected ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Move on to next message - ignore timestamp by searching for newline */
        pb += cchExpected;
        while( pb < rgbRead + cbRead && *pb != '\n' )
        {
            pb++;
        }

        /* Make sure we didn't reach end of what we read */
        ChkBOOL( pb < rgbRead + cbRead, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Skip newline - if this is the last message, it's the last character of the file.*/
        pb++;
    }

    ChkBOOL( pb == rgbRead + cbRead, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    if( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }

    return dr;
#else  /* DRM_SUPPORT_TRACING */
    return DRM_E_NOTIMPL;
#endif /* DRM_SUPPORT_TRACING */
}

/*************************************************************************
**  SetupDRMDir
**
**  Copies the files necessary to test use the test tools correctly to the drm folder.
**
*************************************************************************/
DRM_RESULT DRM_CALL SetupDRMDir(long argc, __in_ecount_opt(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( TST_OEM_CreateDirectory( TOOLTEST_DEFAULT_PATH ) );

    ChkDR( _CopyFileToDRMFolder( DEVCERT_TEMPLATE_FILE_NAME, DEVCERT_TEMPLATE_FILE_NAME) );
    /*
    ** Priv Key has to come first so that the keyfile can store the cert correctly.
    */
    ChkDR( _CopyFileToDRMFolder( PRIVATE_KEY_FILE_NAME, PRIVATE_KEY_FILE_NAME) );
    ChkDR( _CopyFileToDRMFolder( DEVCERT_FILE_NAME, DEVCERT_FILE_NAME) );

    ChkDR( _CopyFileToDRMFolder( GROUP_CERT_FILE_NAME, GROUP_CERT_FILE_NAME) );
    ChkDR( _CopyFileToDRMFolder( GC_PRIVATE_KEY_FILE_NAME, GC_PRIVATE_KEY_FILE_NAME) );
    ChkDR( _CopyFileToDRMFolder( BDEVCERT_FILE_NAME, BDEVCERT_FILE_NAME) );
    ChkDR( _CopyFileToDRMFolder( Z_PRIVATE_KEY_FILE_NAME_SIGN, Z_PRIVATE_KEY_FILE_NAME_SIGN) );
    ChkDR( _CopyFileToDRMFolder( Z_PRIVATE_KEY_FILE_NAME_ENCRYPT, Z_PRIVATE_KEY_FILE_NAME_ENCRYPT) );

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: RunPregenTool
**
** Synopsis: This test calls a specified pregenerated tool
**           withi the specified arguments.
**
** Arguments:
**  argv[0] -- tool name and argument
**
*********************************************************/
DRM_RESULT DRM_CALL RunPregenTool(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT          dr                       = DRM_E_TEST_INCOMPLETE;

    ChkArg( argc > 0 );
    ChkArg( argv[0] != NULL );

    ChkDR( TST_OEM_ExecuteShellCommand( argv[0] ) );

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: RunPregenToolEx
**
** Synopsis: This test calls a specified pregenerated tool
**           withi the specified arguments and adds "-drmpath" parameter.
**
** Arguments:
**  argv[0] -- tool name and argument
**
*********************************************************/
DRM_RESULT DRM_CALL RunPregenToolEx(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_E_TEST_INCOMPLETE;

    DRM_CHAR   szProcNameWithParams[DRM_MAX_PATH * 2 + 1] = {0};
    DRM_CHAR   szDrmPath[DRM_MAX_PATH + 1]                = {0};

    ChkArg( argc > 0 );
    ChkArg( argv[0] != NULL );

    /*
    ** Concatenate argv[0] with -drmpath:g_dstrDrmPath.pwszString
    ** so that tools can be redirected to a proper DRM folder.
    */
    ChkDR( DRM_STR_StringCchCopyNA( szProcNameWithParams,
                                    DRM_MAX_PATH * 2 + 1,
                                    argv[0],
                                    (DRM_DWORD)DRMCRT_strlen( argv[0] ) ) );

    ChkDR( DRM_STR_StringCchCatA( szProcNameWithParams,
                                  DRM_MAX_PATH*2,
                                  " -drmpath:" ) );

    DRM_UTL_DemoteUNICODEtoASCII( g_dstrDrmPath.pwszString,
                                  szDrmPath,
                                  DRM_MAX_PATH + 1 );

    Log( "Trace", "DRM path is: %s", szDrmPath );

    ChkDR( DRM_STR_StringCchCatNA( szProcNameWithParams,
                                  DRM_MAX_PATH * 2 + 1,
                                  szDrmPath,
                                  DRM_MAX_PATH + 1 ) );

    ChkDR( TST_OEM_ExecuteShellCommand( szProcNameWithParams ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function: ToolSleep
**
** Synopsis:  Sleeps a number of Milliseconds Specified by argv[0]
**
** Arguments: argv[0] - number of milliseconds to sleep
**
** Returns:   DRM_SUCCESS on success.
**            DRM_E_INVALIDARG - not enough arguments
**
**********************************************************************/
DRM_RESULT DRM_CALL ToolSleep(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  dwMillSecond = 0;

    ChkArg(argc == 1 && argv[0]);

    dwMillSecond = (DRM_DWORD)atol(argv[0]);

    tDRMSleep(dwMillSecond);

ErrorExit:

    return dr;
}


/**********************************************************************
**
** Function: Delete
**
** Synopsis: Deletes the file specified by argv[0]
**
** Arguments: argv[0] - file name
**
** Returns:   DRM_SUCCESS on success.
**            DRM_E_INVALIDARG - not enough arguments
**
**********************************************************************/
DRM_RESULT DRM_CALL Delete(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg ( argc == 1 && argv[0] );

    ChkDR( tRemoveFile( argv[0] ) );

ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function: Test_API_CheckFilePresense
**
** Synopsis: Attempts to open specified file to check that it's present
**           and can possibly be used by other tools.
**
** Arguments: argv[0] - file name
**
** Returns:   DRM_SUCCESS on success.
**            DRM_E_INVALIDARG - not enough arguments
**            DRM_E_FILE_READ_ERROR - file is not found
**********************************************************************/
DRM_RESULT DRM_CALL Test_API_CheckFilePresense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_CONST_STRING    dstrPath    = EMPTY_DRM_STRING;
    OEM_FILEHDL         hFile       = OEM_INVALID_HANDLE_VALUE;

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrPath , argv[0] ) );

    hFile = Oem_File_Open( NULL,
                           dstrPath.pwszString,
                           OEM_GENERIC_READ,
                           OEM_FILE_SHARE_READ,
                           OEM_OPEN_EXISTING,
                           OEM_ATTRIBUTE_NORMAL );

    if ( OEM_INVALID_HANDLE_VALUE == hFile )
    {
        TRACE(("Test_API_CheckFilePresense - Cannot open file %s\n", argv[0] ));
        ChkDR( DRM_E_FILE_READ_ERROR );
        goto ErrorExit;
    }
    else
    {
        TRACE(("Test_API_CheckFilePresense - Succesfully located file %s\n", argv[0] ));
    }
ErrorExit:
    if ( hFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close(hFile);
    }
    FREEDRMSTRING( dstrPath );
    return dr;
}

/**********************************************************************
**
** Function: Test_API_ChangeTime
**
** Synopsis: Set the clock time
**
** Arguments: argv[0] - Time offset in seconds
**
** Returns:   DRM_SUCCESS on success.
**            DRM_E_INVALIDARG - not enough arguments
**
**********************************************************************/
DRM_RESULT DRM_CALL TestManagerChangeTime( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_LONG   lOffset = 0;

    ChkArg( argc == 1 );
    ChkArg( argv[0] != NULL );

    lOffset = atol( argv[0] );

    tChangeSystemTime( lOffset );

#ifdef WINCE_TEST
    tDRMSleep( 2000 );
#endif

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function: Test_API_CopyDRMFiles
**
** Synopsis: To copy the DRM specific files(ex. devcert.dat and priv.dat)
**
**
** Arguments:   IN argv[0] - device certificate file name
**              IN argv[1] - Associated priv.dat file name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG for invalid arguments
**              errors returned by _CopyFileToDRMFolder
**
**********************************************************************/
DRM_RESULT DRM_CALL Test_API_CopyDeviceCertFiles
(
long argc, __in_ecount(argc) char **argv
)
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_CONST_STRING    dstrDevCertFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrPrivDatFile = EMPTY_DRM_STRING;

    if ( argc < 2 )
    {
        Log( "Trace", "\t\tFrom Test_API_CopyDeviceCertFiles: error in number of arguments." );
        ChkDR ( DRM_E_INVALIDARG );
    }

    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrDevCertFile , argv[0] ) );

    ChkDR( MakeDRMString( &dstrPrivDatFile , argv[1] ) );


    ChkDR( RemoveDRMFile(RMFILE_KEYFILE) );
    /*
    ** Priv Key has to come first so that the keyfile can store the cert correctly.
    */
    ChkDR( _CopyFileToDRMFolder( dstrPrivDatFile.pwszString , PRIVATE_KEY_FILE_NAME ) );
    ChkDR( _CopyFileToDRMFolder( dstrDevCertFile.pwszString , DEVCERT_FILE_NAME ) );
    ChkDR( _CopyFileToDRMFolder( dstrDevCertFile.pwszString , DEVCERT_TEMPLATE_FILE_NAME ) );

ErrorExit:
    FREEDRMSTRING( dstrDevCertFile );
    FREEDRMSTRING( dstrPrivDatFile );
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ToolTest)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR( RemoveDRMFile(RMFILE_KEYFILE) );
    ChkDR( tRemoveFile( LOCAL_HDS_FILE ) );
    ChkDR( SetupDRMDir(0,NULL) );

ErrorExit:
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ToolTest)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST( ToolTest )
    API_ENTRY(RunPregenTool)
    API_ENTRY(RunPregenToolEx)
    API_ENTRY(SetupDRMDir)
    API_ENTRY(ToolSleep)
    API_ENTRY(Delete)
    API_ENTRY(TestManagerChangeTime)
    API_ENTRY(Test_API_CopyDeviceCertFiles)
    API_ENTRY(Test_API_CheckFilePresense)
    API_ENTRY( CreateTraceFile )
    API_ENTRY( VerifyTraceFile )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

