/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <FileLogger.h>
#include <lmerr.h>
#include <TransmitterConfig.h>
#include <HelperFunctions.h>

#define ERRMSGBUFFERSIZE                256
#define DISPLAY_BYTE_COUNT              16

CFileLogger *CFileLogger::s_fileLogger = NULL;
CDRMLock     CFileLogger::s_lock;

using namespace std;

void PRNDLogHexDump( BYTE *pbData, DWORD cbDataSize, bool bDisplayChars )
{
    CFileLogger::s_fileLogger->LogRowsOfRawBytes( pbData, cbDataSize, bDisplayChars );
}

void PRNDLogMessage(  const char *szFormat, ... )
{
    va_list va;
    
    va_start( va, szFormat );
    if( CFileLogger::s_fileLogger != NULL )
    {
        CFileLogger::s_fileLogger->LogMessage( szFormat, va );
    }
    va_end( va );
}

HRESULT PRNDTestDumpToFile( char *szFile, BYTE *pbData, DWORD cbDataSize )
{
    DRM_RESULT          dr                      = DRM_SUCCESS;
    char                szDumpFile[MAX_PATH]    = {0};
    string              strPath;
    
    ChkDR( CTransmitterConfig::Instance().GetTransmitterPath( strPath ) );
    strPath = append_backslash_if_not_exist( strPath );
    
    strncpy( szDumpFile, strPath.c_str(), strPath.length() );
    strncat( szDumpFile, szFile, strlen( szFile ) );

    FILE* fpFile = fopen( szDumpFile, "w+" );
    ChkBOOL( fpFile != NULL, DRM_E_FILEOPEN );

    fwrite( pbData, 1, cbDataSize, fpFile );
    fclose( fpFile );

ErrorExit:
    return dr;
}


void PRNDLogErrorMessage( HRESULT hrError )
{
    DWORD       ret     = 0; 
    HMODULE     hInst   = NULL; 
    HLOCAL      pBuffer = NULL;

    hInst = LoadLibraryA( "NETMSG.DLL" );
    if( hInst != 0 )
    { 
        ret = FormatMessageA(  
                              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
                              hInst, 
                    ( DWORD ) hrError, 
                              MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), 
                   ( LPSTR ) &pBuffer, 
                              ERRMSGBUFFERSIZE, 
                              NULL );
        if( ret != 0 )
        {
            PRNDLogMessage( ( char* )pBuffer );
            goto ErrorExit;
        }
    } 
    
    //
    // Get the message string from the system.
    //
    ret = FormatMessageA(  
                          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL,
                ( DWORD ) hrError, 
                          MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), 
               ( LPSTR ) &pBuffer,
                          ERRMSGBUFFERSIZE, 
                          NULL );
    
    if( ret != 0 )
    {
        PRNDLogMessage( ( char* )pBuffer );
    }
    
ErrorExit:
    if( hInst != NULL )
    {
        FreeLibrary( hInst );
    }
    if( pBuffer != NULL )
    {
        LocalFree( pBuffer );
    }
}

HRESULT CFileLogger::Create( const char *pszLogFile )
{
    DRM_RESULT   dr = DRM_SUCCESS;
    CDRMAutoLock lock( &s_lock );

    if( CFileLogger::s_fileLogger == NULL )
    {
        ChkMem( CFileLogger::s_fileLogger = new CFileLogger());
    }

    ChkDR( s_fileLogger->Initialize( pszLogFile ) );

ErrorExit:
    return dr;
}

void CFileLogger::Close()
{
    CDRMAutoLock lock( &s_lock );
    CFileLogger::s_fileLogger->CloseInternal();
}

CFileLogger::CFileLogger()
{
    m_fpLogFile = NULL;
}

CFileLogger::~CFileLogger()
{

}

void CFileLogger::CloseInternal()
{
    if( m_fpLogFile )
    {
        fclose( m_fpLogFile );
        m_fpLogFile = NULL;
    }
}

HRESULT CFileLogger::Initialize( const char *pszLogFile )
{
    DRM_RESULT dr = DRM_SUCCESS;

    CloseInternal();
    m_fpLogFile = fopen( pszLogFile, "w+" );
    ChkBOOL( m_fpLogFile != NULL, DRM_E_FILEOPEN );

ErrorExit:
    return dr;
}

void CFileLogger::LogRowsOfRawBytes( BYTE *pbData, DWORD cbDataSize, bool bDisplayChars )
{
    CDRMAutoLock lock( &s_lock );
    DWORD        dwDisplayByteCount = 0;
    
    for( BYTE *pbBuffer = pbData ; cbDataSize > 0; )
    {
        if( cbDataSize > DISPLAY_BYTE_COUNT )
        {
            dwDisplayByteCount = DISPLAY_BYTE_COUNT;
            cbDataSize -= DISPLAY_BYTE_COUNT;
        }
        else
        {
            dwDisplayByteCount = cbDataSize;
            cbDataSize = 0;
        }

        DisplayOneLineOfRawBytes( pbBuffer, dwDisplayByteCount, bDisplayChars );
        pbBuffer += dwDisplayByteCount;
    }
}

void CFileLogger::DisplayOneLineOfRawBytes( BYTE *pbBuffer, DWORD dwByteCountToDisplay, bool bDisplayChars )
{
    CDRMAutoLock  lock( &s_lock );
    
    DWORD   j           = 0;
    DWORD   m           = 0;
    DWORD   dwIndexA    = 0;
    char    str[100]    = {0};

    for( m = 0; m < DISPLAY_BYTE_COUNT; m++ )
    {
        if( dwIndexA == 8 )
        {
            j += sprintf_s( str+j, _countof( str )-j, "- " );
        }
        dwIndexA++;

        if( m < dwByteCountToDisplay )
        {
            j += sprintf_s( str+j, _countof( str )-j, "%02x ", pbBuffer[m] );
        }
        else
        {
            j += sprintf_s( str+j, _countof( str )-j, "__ " );
        }
    }
    j += sprintf_s( str+j, _countof( str )-j, "  " );

    if( bDisplayChars )
    {
        for( m = 0; m < DISPLAY_BYTE_COUNT; m++ )
        {
            if( m < dwByteCountToDisplay )
            {
                char c = ( char )pbBuffer[m];
                if( c == '\0' || c == '\r' || c == '\n' )
                {
                    c = '.';
                }
                j += sprintf_s( str+j, _countof( str )-j, "%c", c );
            }
            else
            {
                j += sprintf_s( str+j, _countof( str )-j, " " );
            }
        }
    }
    PRNDLogMessage( str );
}

void CFileLogger::LogMessage( const char *szFormat, va_list vaList )
{
    CDRMAutoLock  lock( &s_lock );

    SYSTEMTIME systimeUTC    = {0};
    SYSTEMTIME localTime     = {0};

    FILETIME   fileTime      = {0};
    FILETIME   fileTimeLocal = {0};

    ::GetSystemTime( &systimeUTC );
    ::SystemTimeToFileTime( &systimeUTC,&fileTime );
    ::FileTimeToLocalFileTime( &fileTime, &fileTimeLocal );
    ::FileTimeToSystemTime( &fileTimeLocal, &localTime );

    fprintf( m_fpLogFile, "%5d %hu-%02hu-%02hu %02hu:%02hu:%02hu  ",
            ::GetCurrentThreadId(),
            localTime.wYear,
            localTime.wMonth,
            localTime.wDay,
            localTime.wHour,
            localTime.wMinute,
            localTime.wSecond );

    vfprintf( m_fpLogFile, szFormat, vaList );
    fprintf( m_fpLogFile, "\n" );
    fflush( m_fpLogFile );
}

