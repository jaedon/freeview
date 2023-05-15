/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef WINCE_TEST
#include <drmwindowsenv.h>
#include <tchar.h>
#include <winbase.h>
#endif

#include <drmcommon.h>
#include <drmutilities.h>
#include <logger.h>
#include <tOEMIMP.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

#define XML_HEADER         "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
#define XSLT_REFERENCE     "<?xml-stylesheet type=\"text/xsl\" href=\"result-transform.xsl\"?>\n\n"

/*
** Define the "no buffering" mode value for setvbuf(), if it is missing (as is the case for WinCE 5.0 builds)
*/
#ifndef _IONBF
#define _IONBF 0x0004
#endif

static DRM_BOOL _fLoggerEnabled = FALSE;

DRM_BOOL g_fSilenceTestChkLogger = FALSE;

static const DRM_CHAR * const s_pszDispositionStrings[3] = { "PASS", "FAIL", "SKIP" };


void DRM_CALL Log( __in const char *szTag, __in const char *szMessage, ... )
{
    va_list va;
    va_start(va, szMessage);
    vLog( szTag, szMessage, va);
    va_end(va);
}

/*
** for wince version and ANSI version
*/
static FILE *fpXmlSummary = NULL,
            *fpLog        = NULL;
char g_szExeNameWithExtension[256];

/*
** log to text file
*/
void DRM_CALL LogStart(long lTC, ENUM_TESTCASE_TYPE f_eType, const char *szTitle)
{
    DRMSYSTEMTIME dsystime = {0};

    printf( "\nvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv Start of Test vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
    fprintf( fpLog, "<br/>vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv Start of Test vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv<br/>");

    fprintf(fpXmlSummary, "\t<LOGRESULTS CaseNo=\"%ld\" Type=\"", lTC);
    switch( f_eType )
    {
    case eBVT:
        fputs( "BVT\" ", fpXmlSummary );
        break;
    case eFunc:
        fputs( "FUNC\" ", fpXmlSummary );
        break;
    case eDRT:
        fputs( "DRT\" ", fpXmlSummary );
        break;
    case ePerf:
        fputs( "PERF\" ", fpXmlSummary );
        break;
    case eFuzz:
        fputs( "FUZZ\" ", fpXmlSummary );
        break;
    default:
        fputs( "UNKNOWN\" ", fpXmlSummary );
        break;
    }

    Oem_Clock_GetSystemTime(NULL, &dsystime);
    fprintf( fpLog, "<h2 class=title id=%ld>%ld -- %s</h2>\n", lTC, lTC, szTitle);
    fprintf( fpLog, "<pre>\nStart test case #%ld at %d-%02d-%02d %02d:%02d:%02d\n",
                    lTC,
                    dsystime.wYear,
                    dsystime.wMonth,
                    dsystime.wDay,
                    dsystime.wHour,
                    dsystime.wMinute,
                    dsystime.wSecond);

    printf( "\nStart test case #%ld at %d-%02d-%02d %02d:%02d:%02d\n",
                lTC,
                dsystime.wYear,
                dsystime.wMonth,
                dsystime.wDay,
                dsystime.wHour,
                dsystime.wMinute,
                dsystime.wSecond);
    printf( "\tTitle: %s\n", szTitle );
}

/*
** LogBlob
**
** Writes a blob of binary data to the test Log.
**
** Arguments -
**      [f_szTag]       -- Ignored, present for compatibility with the COM Logger
**      [f_szMessage]   -- Message to write before the dump
**      [f_pbBlob]      -- Blob to write
**      [f_cbBlob]      -- How many bytes of f_pbBlob to write.
*/
void DRM_CALL LogBlob(const char* f_szTag, const char* f_szMessage, const DRM_BYTE* f_pbBlob, DRM_DWORD f_cbBlob )
{
    Log( f_szTag, f_szMessage );

    if( fpLog != NULL )
    {
        fwrite( f_pbBlob, SIZEOF(DRM_BYTE), f_cbBlob, fpLog);
        fprintf( fpLog, "\n\n" );
    }

    if ( _fLoggerEnabled )
    {
        printf("open the logfile for the byte dump\n");
    }
}

void DRM_CALL LogBlobB64(
    __out_ecount( f_cchLogBuffer )            DRM_CHAR  *f_pchLogBuffer,
    __in                                      DRM_DWORD  f_cchLogBuffer,
    __in_ecount( f_cbBlob )             const DRM_BYTE  *f_pbBlob,
    __in                                      DRM_DWORD  f_cbBlob )
{
    if( f_cbBlob == 0 )
    {
        Log( "Trace", "\tLogBlobB64: FAILED: SIZE ZERO BLOB" );
    }
    else if( f_cchLogBuffer < CCH_BASE64_EQUIV(f_cbBlob) + 1 )
    {
        Log( "Trace", "\tLogBlobB64: FAILED: LOG BUFFER TOO SMALL" );
    }
    else if( DRM_FAILED( DRM_B64_EncodeA( f_pbBlob, f_cbBlob, f_pchLogBuffer, &f_cchLogBuffer, 0 ) ) )
    {
        Log( "Trace", "\tLogBlobB64: FAILED: B64 ENCODE" );
    }
    else
    {
        f_pchLogBuffer[f_cchLogBuffer] = g_chNull;
        Log( "Trace", "\tLogBlobB64: \"%s\"", f_pchLogBuffer );
    }
}

/*
** only print out the results
*/
void DRM_CALL vLog(
    const char *szTag,
    __in_z __format_string const char *szMessage,
    va_list argptr)
{
    DRMSYSTEMTIME dsystime = {0};

    if( szMessage != NULL
     && ( !g_fSilenceTestChkLogger || strncmp( szTag, "Chk", 3 ) != 0 ) )
    {
        Oem_Clock_GetSystemTime(NULL, &dsystime);
        if( fpLog != NULL )
        {
            fprintf(fpLog, "%d>%hu-%02hu-%02hu %02hu:%02hu:%02hu\t",
                    Oem_GetCurrentThreadId(),
                    dsystime.wYear,
                    dsystime.wMonth,
                    dsystime.wDay,
                    dsystime.wHour,
                    dsystime.wMinute,
                    dsystime.wSecond);

            vfprintf( fpLog, szMessage, argptr );
            fprintf( fpLog, "\n");
        }
        if ( _fLoggerEnabled || fpLog == NULL )
        {
            printf("%d>%hu-%02hu-%02hu %02hu:%02hu:%02hu\t",
                    Oem_GetCurrentThreadId(),
                    dsystime.wYear,
                    dsystime.wMonth,
                    dsystime.wDay,
                    dsystime.wHour,
                    dsystime.wMinute,
                    dsystime.wSecond);

            vprintf(szMessage, argptr);
            printf("\n");
        }
    }
}


void DRM_CALL LogEnd(
    __in       const enum _EDISPOSITION  f_eDisposition,
    __in       const DRM_UINT64          f_qwTicks,
    __in       const DRM_DWORD           f_dwTCID,
    __in_z_opt const DRM_CHAR           *f_szTitle,
    __in_z_opt const DRM_CHAR           *f_szTrace )
{
    DRMSYSTEMTIME dsystime = {0};

    Oem_Clock_GetSystemTime( NULL, &dsystime );

    fprintf( fpLog, "</pre>\n" );

    fprintf( fpXmlSummary, "Result=\"%s\" Ticks=\"%lu\">\n", s_pszDispositionStrings[f_eDisposition], DRM_UI64Low32(f_qwTicks) );

    if( f_szTitle != NULL )
    {
        printf( "\t\tResults for test: %lu -- %s\n", f_dwTCID, f_szTitle );
        fprintf( fpXmlSummary, "\t\t<Title>%lu -- %s</Title>\n", f_dwTCID, f_szTitle );
        fprintf( fpLog, "<h3>Results for test: %lu -- %s</h3>", f_dwTCID, f_szTitle );
    }
    if( fpLog != NULL )
    {

        fprintf( fpLog, "<br/>\nTime=%lu ms\n", DRM_UI64Low32(f_qwTicks) );
        fprintf( fpLog, "<br/>\nResult=\"%s\" at \t\t%hu-%02hu-%02hu %02hu:%02hu:%02hu\n",
                         s_pszDispositionStrings[f_eDisposition],
                         dsystime.wYear,
                         dsystime.wMonth,
                         dsystime.wDay,
                         dsystime.wHour,
                         dsystime.wMinute,
                         dsystime.wSecond );
    }
    if ( _fLoggerEnabled || fpLog == NULL )
    {
        printf( "\tTime=%lu ms\n", DRM_UI64Low32(f_qwTicks) );
    }
    printf( "\tResult=\"%s\" at %hu-%02hu-%02hu %02hu:%02hu:%02hu\n",
                    s_pszDispositionStrings[f_eDisposition],
                    dsystime.wYear,
                    dsystime.wMonth,
                    dsystime.wDay,
                    dsystime.wHour,
                    dsystime.wMinute,
                    dsystime.wSecond );
    if ( _fLoggerEnabled || fpLog == NULL )
    {
        printf( "\tTRACE:\n%s\n\n", f_szTrace );
    }

    if( f_szTrace != NULL )
    {
        fprintf( fpXmlSummary, "\t\t<Trace>%s</Trace>\n", f_szTrace );
        fprintf( fpLog, "<h3>%s</h3>\n", f_szTrace );
    }

    fprintf( fpLog, "<br/>\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ End of Test ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^<br/>");
    printf( "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ End of Test ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    fputs( "\t</LOGRESULTS>\n",fpXmlSummary );
}


/*
** create the XML Result log file. If there is an existed one from previous run, delete it
**
** Arguments:
**              [f_fLoggerEnabled]  --  TRUE  means write Log call to stdout and log file
**                                        FASLE means just write to log File
**
*/
void DRM_CALL InitLogger( DRM_BOOL f_fLoggerEnabled )
{
    char  szFullPath[256]     = {0};
    char  szLogFullPath[256]  = {0};
    char  szLogFileName[256]  = {0};
    int   iLen                = 0;
    char *pszLogFileName      = NULL;
    char *pszTemp             = NULL;
    char  szPathSeparator[2]  = {0};


    szPathSeparator[0] = g_chPathSeparator;
    szPathSeparator[1] = '\0';

    _fLoggerEnabled = f_fLoggerEnabled;
    /*
    ** remove the .exe extension
    */
    iLen = (int)strlen(g_szExeNameWithExtension);
    if( strstr(g_szExeNameWithExtension,".exe")||strstr(g_szExeNameWithExtension,".xex"))
    {
        iLen = iLen - 4;
    }

    iLen = (int)min( SIZEOF( szFullPath ) - 5, iLen );

    if ( iLen < 0 || iLen > SIZEOF( szFullPath ) - 5 )
    {
        return;
    }

    MEMCPY( szFullPath, g_szExeNameWithExtension, (DRM_SIZE_T)iLen );
    szFullPath[ iLen ]     = '.';
    szFullPath[ iLen + 1 ] = 'x';
    szFullPath[ iLen + 2 ] = 'm';
    szFullPath[ iLen + 3 ] = 'l';
    szFullPath[ iLen + 4 ] = '\0';

    iLen = (int)min( SIZEOF( szLogFullPath ) - 5, iLen );
    MEMCPY( szLogFullPath, g_szExeNameWithExtension, (DRM_SIZE_T)iLen );
    szLogFullPath[ iLen ]     = '.';
    szLogFullPath[ iLen + 1 ] = 'h';
    szLogFullPath[ iLen + 2 ] = 't';
    szLogFullPath[ iLen + 3 ] = 'm';
    szLogFullPath[ iLen + 4 ] = '\0';

    pszLogFileName = szLogFullPath;
    pszTemp = pszLogFileName;

    if( pszTemp == NULL )
    {
        return;
    }

    /*
    ** Get the filename without the full path for the Log file, to store the name in the XML file.
    */
    do
    {
        pszLogFileName = pszTemp + 1;
        pszTemp = strstr( pszLogFileName, szPathSeparator );
    } while( pszTemp != NULL );
    MEMCPY( szLogFileName, pszLogFileName, min( strlen( pszLogFileName ) + 1, SIZEOF( szLogFileName ) ) );

    /*
    ** delete the old file if there is any one
    */
    fpXmlSummary = fopen(szFullPath, "w+");
    if(fpXmlSummary)
        fclose(fpXmlSummary);
    /*
    ** open XML Log for appending mode
    */
    fpXmlSummary = fopen(szFullPath, "a+");

    /*
    ** open Log File
    */
    fpLog = fopen( szLogFullPath, "w+" );
    if( fpLog != NULL )
    {
        fclose( fpLog );
    }
    fpLog = fopen( szLogFullPath, "a+" );

    /*
    ** Xbox doesn't support non buffering mode correctly. Turning this on causes empty log files to be written.
    */
#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_XBOX
    setvbuf( fpLog, NULL, _IONBF, 0 );
    setvbuf( fpXmlSummary, NULL, _IONBF, 0 );
#endif

    fputs( XML_HEADER, fpXmlSummary );
    fputs( XSLT_REFERENCE, fpXmlSummary );
    fprintf( fpXmlSummary, "<LOGSUMMARY stdout_log=\"%s\">\n", szLogFileName );
}


/*
** close the file
*/
void DRM_CALL UninitLogger()
{
    if (fpXmlSummary){
        fputs("</LOGSUMMARY>\n",fpXmlSummary);
        fclose(fpXmlSummary);
    }
    if ( fpLog )
    {
        fclose( fpLog );
    }
}

/***************************************************************************
**
** Function:    LogTestInfo
**
** Synopsis:    This function writes the base names of the expected test script
**              files and the suuported platform extensions to the xml log.
**              This lets us gather results correctly based on conditionally
**              compiled script lists.
**
**              Generated XML:
**              <testinfo>
**                  <scriptfilebases>
**                      <file>testscript1</file>
**                      <file>testscript2</file>
**                      ...
**                  </scriptfilebases>
**                  <platforms>
**                      <platform>.tc.</platform>
**                      <platform>.pc.</platform>
**                      ...
**                  </platforms>
**                  <runwhat DRT="YES|NO" BVT="YES|NO" FUNC="YES|NO" PERF="YES|NO" FUZZ="YES|NO" />
**              </testinfo>
**
**
** Arguments:
**              [f_rgoTestScriptList]       pointer to the array containing the
**                                          test script entries. The last
**                                          element MUST have a NULL file name.
**              [f_szSupportedPlatforms]    pointer to the array containing the platform
**                                          extensions of the test script files. The last
**                                          element MUST be NULL.
**              [f_eRunWhat]                what type of tests are we running
**
****************************************************************************/
void DRM_CALL LogTestInfo( const TestScriptListEntry *f_rgoTestScriptList, const char** f_szSupportedPlatforms, DRM_TEST_RUN_TYPE f_eRunWhat)
{
    fputs( "\t<testinfo>\n", fpXmlSummary );
    fputs( "\t\t<scriptfilebases>\n", fpXmlSummary );

    for( ; f_rgoTestScriptList->m_szTestScriptFileName != NULL; f_rgoTestScriptList++ )
    {
        if( ( f_rgoTestScriptList->m_pfnIsSupported != NULL )
         && !f_rgoTestScriptList->m_pfnIsSupported() )
        {
            /*
            ** If test script file not supported, continue to the next file in the list
            */
            continue;
        }

        fprintf( fpXmlSummary, "\t\t\t<file>%s</file>\n", f_rgoTestScriptList->m_szTestScriptFileName );
    }
    fputs( "\t\t</scriptfilebases>\n", fpXmlSummary );
    fputs( "\t\t<platforms>\n", fpXmlSummary );
    while( *f_szSupportedPlatforms != NULL )
    {
        fprintf( fpXmlSummary, "\t\t\t<platform>%s</platform>\n", *f_szSupportedPlatforms );
        f_szSupportedPlatforms++;
    }
    fputs( "\t\t</platforms>\n", fpXmlSummary );
    fprintf(fpXmlSummary, "\t\t<runwhat DRT=\"%s\" BVT=\"%s\" FUNC=\"%s\" PERF=\"%s\" FUZZ=\"%s\" />\n",
            ( ( f_eRunWhat & eRunDRT )  == eRunDRT  ) ? "YES" : "NO",
            ( ( f_eRunWhat & eRunBVT )  == eRunBVT  ) ? "YES" : "NO",
            ( ( f_eRunWhat & eRunFunc ) == eRunFunc ) ? "YES" : "NO",
            ( ( f_eRunWhat & eRunPerf ) == eRunPerf ) ? "YES" : "NO",
            ( ( f_eRunWhat & eRunFuzz ) == eRunFuzz ) ? "YES" : "NO" );
    fputs( "\t</testinfo>\n", fpXmlSummary );

    return;
}

EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP; /* __WARNING_USE_WIDE_API */

