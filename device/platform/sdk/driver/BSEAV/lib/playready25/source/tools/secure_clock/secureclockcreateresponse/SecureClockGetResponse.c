/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmwindowsenv.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <drmxmlparser.h>
#include <drmtoolsutils.h>
#include <drmutf.h>
#include <drmsecureclock.h>
#include <drmsecureclockconstants.h>
#include <drmconstants.h>

#pragma comment( lib, "wininet.lib" )

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_ANSI_APICALL, "Ignore prefast warning about ANSI functions for this entire file.")

#define MAX_FILENAME 1023
#define MAX_URL_SIZE 1024
#define SC_PROTOCOL_STRING "drm_clock_acquire_redirect: "

static DRM_RESULT _GetSecureClockURL(
    IN  const DRM_CHAR   *f_pszURL,
    IN        HINTERNET   f_hSession,
    OUT       DRM_CHAR  **f_ppszSCURL )
{
    HINTERNET  hConnect  = NULL;
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BYTE   rgbQuery[ MAX_URL_SIZE ] = { 0 };
    DRM_DWORD  cbQuery   = MAX_URL_SIZE;
    DRM_DWORD  cchSCURL  = 0;
    DRM_CHAR  *pbCurrent = NULL;
    DRM_DWORD  cbCurrent = 0;
    DRM_DWORD  cbRead    = 0;

    ChkArg( f_ppszSCURL != NULL );

    hConnect = InternetOpenUrlA( f_hSession,
                                 f_pszURL,
                                 NULL, /* no headers */
                                 0,    /* no headers */
                                 INTERNET_FLAG_PRAGMA_NOCACHE, /* no caching */
                                 0 /* no context */ );

    ChkWR( hConnect != NULL );

    /*
    ** Query for the content length from the HTTP header of the
    ** server response. Content length indicates the size of
    ** HTTP data that follows the HTTP header in the server response.
    */
    cbQuery = MAX_URL_SIZE;
    ChkWR( HttpQueryInfoA( hConnect,
                           HTTP_QUERY_CONTENT_LENGTH,
                           rgbQuery,
                           (LPDWORD)&cbQuery,
                           NULL ) );

    ChkDR( DRMCRT_AtoDWORD( (DRM_CHAR*)&rgbQuery[0], MAX_URL_SIZE, 10, &cchSCURL ) );

    /*
    ** The response is not neccesarily null-terminated. Allocate a larger buffer
    ** and ensure that it is null terminated.
    */
    ChkMem( *f_ppszSCURL = ( DRM_CHAR * )Oem_MemAlloc( cchSCURL + SIZEOF( DRM_CHAR ) ) );
    ZEROMEM( *f_ppszSCURL, cchSCURL );
    PUT_CHAR( *f_ppszSCURL, cchSCURL, '\0' );

    /*
    ** Read the HTTP data of the server response into the
    ** buffer just allocated.
    */
    pbCurrent = *f_ppszSCURL;
    cbCurrent = cchSCURL;
    while ( cbCurrent > 0 )
    {
        ChkWR( InternetReadFile( hConnect,
                                 pbCurrent,
                                 cbCurrent,
                                 ( LPDWORD )&cbRead ) );
        if ( cbRead == 0 )
        {
            break;
        }
        pbCurrent += cbRead;
        cbCurrent -= cbRead;
    }

    /*
    ** Make sure that the reasonable url length was read
    */
    ChkBOOL( cchSCURL - cbCurrent > 4, DRM_E_FAIL );
    ChkBOOL( cchSCURL > DRMCRT_strlen( SC_PROTOCOL_STRING ), DRM_E_FAIL );

    /*
    ** Skip hardcoded server string from response- get pure url
    ** Response looks like: "drm_clock_acquire_redirect: http://services.wmdrm.windowsmedia.com/SecureClock/?Time"
    */
    DRM_BYT_MoveBytes( *f_ppszSCURL, 0, *f_ppszSCURL, DRMCRT_strlen( SC_PROTOCOL_STRING ), cchSCURL - DRMCRT_strlen( SC_PROTOCOL_STRING ) );
    PUT_CHAR( *f_ppszSCURL, cchSCURL - DRMCRT_strlen( SC_PROTOCOL_STRING ), '\0' );
ErrorExit:

    if (DRM_FAILED( dr ) )
    {
        if( f_ppszSCURL != NULL )
        {
            SAFE_OEM_FREE( *f_ppszSCURL );
            *f_ppszSCURL = NULL;
        }
    }
    if ( hConnect != NULL )
    {
        InternetCloseHandle( hConnect );
    }
    return dr;
}

static DRM_RESULT _GetSCResponse(
    __in                                   const DRM_CHAR   *f_pszSCURL,
    __in                                         HINTERNET   f_hSession,
    __in_ecount(f_cbChallenge)                   DRM_BYTE   *f_pbChallenge,
    __in                                         DRM_DWORD   f_cbChallenge,
    __deref_out_ecount_opt(*f_pcbResponse)       DRM_BYTE  **f_ppbResponse,
    __out                                        DRM_DWORD  *f_pcbResponse )
{
    HINTERNET  hConnect  = NULL;
    HINTERNET  hHttpFile = NULL;
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BYTE  *pbCurrent = NULL;
    DRM_DWORD  cbCurrent = 0;
    DRM_DWORD  cbRead    = 0;
    DRM_BYTE   rgbQuery   [ MAX_URL_SIZE ] = { 0 };
    DRM_DWORD  cbQuery   = MAX_URL_SIZE;
    DRM_CHAR   rgchHostName[ MAX_URL_SIZE ] = { 0 };
    URL_COMPONENTSA oURLComponents;

    oURLComponents.dwStructSize = SIZEOF( oURLComponents );

   /* The following elements determine which components are displayed */
    oURLComponents.dwSchemeLength    = 1;
    oURLComponents.dwHostNameLength  = 1;
    oURLComponents.dwUserNameLength  = 1;
    oURLComponents.dwPasswordLength  = 1;
    oURLComponents.dwUrlPathLength   = 1;
    oURLComponents.dwExtraInfoLength = 1;

    oURLComponents.lpszScheme     = NULL;
    oURLComponents.lpszHostName   = NULL;
    oURLComponents.lpszUserName   = NULL;
    oURLComponents.lpszPassword   = NULL;
    oURLComponents.lpszUrlPath    = NULL;
    oURLComponents.lpszExtraInfo  = NULL;

    ChkWR( InternetCrackUrlA( f_pszSCURL,
                             0,
                             0,
                            &oURLComponents ) );

    ZEROMEM( rgchHostName, MAX_URL_SIZE );
    ChkArg( oURLComponents.lpszHostName != NULL );
    DRM_BYT_CopyBytes( rgchHostName, 0, oURLComponents.lpszHostName, 0, oURLComponents.dwHostNameLength );

    hConnect = InternetConnectA( f_hSession,
                                 rgchHostName,
                                 oURLComponents.nPort,
                                 NULL,
                                 NULL,
                                 INTERNET_SERVICE_HTTP,
                                 0,
                                 0 );
    ChkWR( hConnect != NULL );

    hHttpFile = HttpOpenRequestA( hConnect,
                                  "POST",
                                  oURLComponents.lpszUrlPath,
                                  NULL,
                                  NULL,
                                  0,
                                  INTERNET_FLAG_DONT_CACHE,
                                  0 );
    ChkWR( hHttpFile != NULL );

    /* Send the HTTP request to the server. */
    ChkWR( HttpSendRequestA( hHttpFile,
                             NULL,
                             0,
                             f_pbChallenge,
                             f_cbChallenge ) );

    cbQuery = MAX_URL_SIZE;
    ChkWR( HttpQueryInfoA( hHttpFile,
                           HTTP_QUERY_CONTENT_LENGTH,
                           rgbQuery,
                           (LPDWORD)&cbQuery,
                           NULL ) );

    *f_pcbResponse = (DRM_DWORD)atol(( const DRM_CHAR * )&rgbQuery[0]);
    ChkMem( *f_ppbResponse = ( DRM_BYTE * )Oem_MemAlloc( *f_pcbResponse ) );

    ZEROMEM( *f_ppbResponse, cbQuery );

    /*
    ** Read the HTTP data of the server response into the
    ** buffer just allocated.
    */
    pbCurrent = *f_ppbResponse;
    cbCurrent = *f_pcbResponse;
    while ( cbCurrent > 0 )
    {
        ChkWR( InternetReadFile( hHttpFile,
                                 pbCurrent,
                                 cbCurrent,
                                 ( LPDWORD )&cbRead ) );
        if ( cbRead == 0 )
        {
            break;
        }
        else
        {
            pbCurrent += cbRead;
            cbCurrent -= cbRead;
        }
    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
       if( f_ppbResponse != NULL )
       {
           SAFE_OEM_FREE( *f_ppbResponse );
           *f_ppbResponse = NULL;
       }
    }

    if ( hHttpFile != NULL )
    {
        InternetCloseHandle( hHttpFile );
    }
    if ( hConnect != NULL )
    {
        InternetCloseHandle( hConnect );
    }

    return dr;
}

/*********************************************************************
**
** Function: _SecureClockNetClient
**
** Synopsis: Function that sends a secure clock challenge packet
**           to the secure clock server and receives a response packet
**           from the server.
**           This function uses Windows Internet API library to do
**           the operations. It can be configured to use different
**           proxy settings.
**
** Arguments:
**
** [f_pbChallenge]          -- Pointer to buffer that contains a challenge
**                             packet to be sent to the server.
** [f_cbChallenge]          -- Size of the buffer that contains a challenge
**                             packet to be sent to the server.
** [f_ppbResponse]          -- Pointer to pointer to a buffer that receives
**                             a response packet from the server.
** [f_ppbResponse]          -- Pointer to a variable that reveives the size of
**                             a response packet from the server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if any HTTP error happens.
**
**********************************************************************/
static DRM_RESULT _SecureClockNetClient(
    IN  const DRM_CHAR   *f_pszURL,
    IN        DRM_BYTE   *f_pbChallenge,
    IN        DRM_DWORD   f_cbChallenge,
    OUT       DRM_BYTE  **f_ppbResponse,
    OUT       DRM_DWORD  *f_pcbResponse )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    HINTERNET  hSession = NULL;
    DRM_CHAR  *pszSCURL = NULL;

    ChkArg( f_pszURL != NULL );
    ChkArg( f_pbChallenge != NULL );
    ChkArg( f_cbChallenge > 0 );
    ChkArg( f_ppbResponse != NULL );
    ChkArg( f_pcbResponse != NULL );

    /* Use proxy settings configured in the Internet Explorer. */
    hSession = InternetOpenA( "Secure Clock Response Tool",
                              PRE_CONFIG_INTERNET_ACCESS,
                              "",
                              INTERNET_INVALID_PORT_NUMBER,
                              0 );
    ChkWR( hSession != NULL );

    ChkDR( _GetSecureClockURL( f_pszURL, hSession, &pszSCURL ) );

    ChkDR( _GetSCResponse( pszSCURL,
                           hSession,
                           f_pbChallenge,
                           f_cbChallenge,
                           f_ppbResponse,
                           f_pcbResponse ) );

ErrorExit:

    SAFE_OEM_FREE( pszSCURL );
    if ( hSession != NULL )
    {
        InternetCloseHandle( hSession );
    }
    return dr;
}


/****************************************************************************/
static DRM_RESULT _GetSecureClockResponse( DRM_BYTE *pbChallenge, DRM_DWORD cbChallenge, DRM_BYTE **ppbResponse, DRM_DWORD *pcbResponse )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_CONST_STRING dstrTemp        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrURL         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrChallenge   = EMPTY_DRM_STRING;
    DRM_SUBSTRING    dasstrChallenge = EMPTY_DRM_SUBSTRING;
    DRM_BYTE        *pbTemp          = NULL;
    DRM_DWORD        cbTemp          = 0;
    DRM_CHAR         rgchURL[MAX_URL_SIZE] = { 0 };
    DRM_DWORD        cchURL = 0;

    ChkArg( pbChallenge != NULL );
    ChkArg( cbChallenge > 0 );

    ZEROMEM(rgchURL, MAX_URL_SIZE*SIZEOF(DRM_CHAR) );

    /* Decode base64 encoded challenge */
    ChkMem( pbTemp = (DRM_BYTE *) Oem_MemAlloc( cbChallenge ) );
    cbTemp  = cbChallenge;
    dasstrChallenge.m_cch = cbChallenge;
    ChkDR( DRM_B64_DecodeA( (DRM_CHAR*)pbChallenge, &dasstrChallenge, &cbTemp, pbTemp, 0 ) );

    dstrChallenge.pwszString = (const DRM_WCHAR*)pbTemp;
    dstrChallenge.cchString = cbTemp / SIZEOF(DRM_WCHAR);

    ChkDR( DRM_XML_GetNode( &dstrChallenge, &g_dstrTagDRMReqRoot, NULL, NULL, 0, NULL, &dstrTemp ) );

    ChkDR( DRM_XML_GetNode( &dstrTemp, &g_dstrTagData, NULL, NULL, 0, &dstrData, NULL ) );

    /* Read Data section */
    ChkDR( DRM_XML_GetNode( &dstrTemp, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrData ) );

    /* Get URL */
    ChkDR( DRM_XML_GetNode( &dstrData, &g_dstrTagURL, NULL, NULL, 0, NULL, &dstrURL ) );

    cchURL = MAX_URL_SIZE;
    ChkDR( DRM_STR_UTF16toUTF8( dstrURL.pwszString, dstrURL.cchString, rgchURL, 0, &cchURL ) );
    ChkDR( _SecureClockNetClient( rgchURL, pbChallenge, cbChallenge, ppbResponse, pcbResponse ) );

ErrorExit:

    SAFE_OEM_FREE( dstrChallenge.pwszString );
    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf( "Syntax: %S [-?] -l:ChallengeFile -r:OutputresponseFile \n", pwszAppName );
    printf( "        Note: Challenge file and output response file are base64 encoded" );
}

/******************************************************************************
**
*******************************************************************************
*/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_RESULT dr  = DRM_SUCCESS;

    DRM_CONST_STRING dstrChallengeFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrResponseFile  = EMPTY_DRM_STRING;

    DRM_BYTE * pbChallenge = NULL;
    DRM_DWORD  cbChallenge = 0;
    DRM_BYTE * pbResponse  = NULL;
    DRM_DWORD  cbResponse  = 0;
    DRM_LONG   i           = 0;

    PrintCopyrightInfo( "Secure Clock Response Tool" );

    if( !DRM_CLK_IsSecureClockSupported() )
    {
        printf( "This tool requires that the real secure clock library be linked to it.\r\n" );
        ChkDR( DRM_E_CLK_NOT_SUPPORTED );
    }

    if( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    ChkArg( argv != NULL );
    for( i = 1; i<argc; i++ )
    {
        DRM_WCHAR wchOption = 0;
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;
        if ( !DRM_CMD_ParseCmdLine(argv[i], &wchOption, &dstrParam, NULL) )
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
                dstrChallengeFile = dstrParam;
                break;

            case WCHAR_CAST('r'):
                dstrResponseFile = dstrParam;
                break;

            case WCHAR_CAST( '?' ):
                goto _PrintUsage;

            default:
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
    }

    if( dstrChallengeFile.cchString == 0
     || dstrResponseFile.cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }
    if( dstrResponseFile .cchString > MAX_FILENAME
     || dstrChallengeFile.cchString > MAX_FILENAME )
    {
        printf( "File name is too big. Max Filename is %d \n", MAX_FILENAME );
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ChkDR( DRMTOOLS_ReadBinaryFileData( &dstrChallengeFile, &pbChallenge, &cbChallenge ) );

    ChkDR( _GetSecureClockResponse( pbChallenge, cbChallenge, &pbResponse, &cbResponse ) );

    ChkDR( DRMTOOLS_WriteBufferToFile( &dstrResponseFile, pbResponse, cbResponse ) );

ErrorExit:

    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbResponse );

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
    PrintUsage( argv == NULL ? &g_wchNull : argv[0] );
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */
PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

