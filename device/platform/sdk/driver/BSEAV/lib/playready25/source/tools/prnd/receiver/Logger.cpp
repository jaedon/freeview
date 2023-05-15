/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <Logger.h>

#define ERRMSGBUFFERSIZE                256

void PRNDLogHexDump( BYTE *pbData, DWORD cbDataSize, bool bDisplayChars )
{
    return;
}

void PRNDLogMessage(  const char *szFormat, ... )
{
    va_list va;
    
    va_start( va, szFormat );
    vprintf( szFormat, va );
    va_end( va );
}

HRESULT PRNDTestDumpToFile( char *szFile, BYTE *pbData, DWORD cbDataSize )
{
    return DRM_E_NOTIMPL;
}

void PRNDLogErrorMessage( HRESULT hrError )
{
    DRM_DWORD   ret     = 0; 
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
            PRNDLogMessage( ( const char* )pBuffer );
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
        PRNDLogMessage( ( const char* )pBuffer );
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