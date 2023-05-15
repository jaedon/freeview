/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#define WSA_GET_LAST_ERROR_HR() HRESULT_FROM_WIN32( ( unsigned long ) WSAGetLastError() )

extern HRESULT PRNDTestDumpToFile( char *szFile, BYTE *pbData, DWORD cbDataSize );
extern void PRNDLogHexDump( BYTE *pbData, DWORD cbDataSize, bool bDisplayChars );
extern void PRNDLogMessage(  const char *szFormat, ... );
extern void PRNDLogErrorMessage( HRESULT hrError );

/*
** These macros intentionally use a constant conditional.
*/
#pragma warning(disable:4127)

#define PRNDLogFunctionEntry() do {                     \
        PRNDLogMessage( "Enter %s() ", __FUNCTION__ );  \
    } while( FALSE )

#define PRNDLogFunctionExit() do {                      \
        PRNDLogMessage( "Exit %s() ", __FUNCTION__ );   \
    } while( FALSE )

#define PRNDLogFunctionExitWithResult( drResult ) do {                            \
        PRNDLogMessage( "Exit %s() with result = 0x%X", __FUNCTION__, drResult ); \
        if( FAILED( drResult ) )                                                  \
        {                                                                         \
            PRNDLogErrorMessage( drResult );                                      \
        }                                                                         \
    } while( FALSE )


#define ChkHR( expr ) do {                                                                                         \
        hr = ( expr );                                                                                             \
        if( FAILED( hr ) )                                                                                         \
        {                                                                                                          \
            PRNDLogMessage( "%s() failed with hr = 0x%X in File %s At %d ", __FUNCTION__, hr, __FILE__, __LINE__ );\
            goto ErrorExit;                                                                                        \
        }                                                                                                          \
    } while( FALSE )

#define ChkArgHR( expr ) do {                                                                                        \
        if( !( expr ) )                                                                                              \
        {                                                                                                            \
            PRNDLogMessage( "ChkArg : Invalid argument at %s in File %s At %d ", __FUNCTION__, __FILE__, __LINE__ ); \
            hr = E_INVALIDARG;                                                                                       \
            goto ErrorExit;                                                                                          \
        }                                                                                                            \
    } while( FALSE )
