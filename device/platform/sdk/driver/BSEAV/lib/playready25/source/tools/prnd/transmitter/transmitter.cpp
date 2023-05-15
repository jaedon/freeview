/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <PRNDSessionManager.h>
#include <FileLogger.h>
#include <ServerSocket.h>
#include <TransmitterConfig.h>
#include <HelperFunctions.h>

#if DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP
#include <UPnPTransmitter.h>
#endif /* DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP */

#define MAX_IP_HOP_COUNT  3
using namespace std;


DWORD WINAPI ListenForMediaStreamingRequests( __in LPVOID lpParam )
{
    PRNDLogFunctionEntry();

    HRESULT             dr              = S_OK;
    CServerSocket      *pServerSocket   = NULL;

    ChkMem( pServerSocket = new CServerSocket() );
    ChkDR( pServerSocket->Serve( PRND_SESSION_COUNT_MAX, SOCK_STREAM, CTransmitterConfig::Instance().GetMediaStreamerPort() ) );

    while( true )
    {
        PRNDLogMessage( " " );
        PRNDLogMessage( "Waiting for MediaStream client connection.." );

        CSocket *pClientSocket = NULL;
        ChkDR( pServerSocket->Accept( &pClientSocket ) );

        PRNDLogMessage( "Creating worker thread to process MediaStream client connection.." );
        if( CreateThread( NULL, 0, CPRNDSessionManager::ProcessMediaStreamingRequestsCallback, pClientSocket, 0, NULL ) == NULL )
        {
            dr = HRESULT_FROM_WIN32( GetLastError() );
            goto ErrorExit;
        }
    }

ErrorExit:
    if( pServerSocket != NULL )
    {
        pServerSocket->Close();
        DELETE_OBJECT( pServerSocket );
    }

    PRNDLogFunctionExitWithResult( dr );
    return ( DWORD )dr;
}


int __cdecl main( int argc, char **argv )
{
    HRESULT             dr                = DRM_SUCCESS;
    CServerSocket      *pServerSocket     = NULL;
    string              strLogFile;

#if DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP
    CUPnPComContext     comContext;
    CUPnPTransmitter   *pUPnPTransmitter  = NULL;
#endif /* DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP */

    CTransmitterConfig  &config           = CTransmitterConfig::Instance();

    ChkDR( config.GetTransmitterLogFileFullPath( strLogFile ) );
    CFileLogger::Create( strLogFile.c_str() );

    ChkDR( config.SetupDrmPathGlobalVariable() );
    ChkDR( config.ParseConfigFile() );

    ChkMem( pServerSocket = new CServerSocket() );
    ChkDR( pServerSocket->Serve( PRND_SESSION_COUNT_MAX, SOCK_STREAM, config.GetTransmitterPort() ) );

    if( config.UPnPIsEnabled() )
    {
#if DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP
        std::string strResourcePath;

        ChkDR( comContext.Init() );

        ChkMem( pUPnPTransmitter = CUPnPTransmitter::MakeInstance() );

        ChkDR( config.GetTransmitterPath( strResourcePath ) );

        ChkDR( pUPnPTransmitter->Start( config.GetUPnPDeviceName().c_str(), strResourcePath.c_str() ) );
#else /* DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP */
        
        PRNDLogMessage( "UPnP is not supported on this build." );
        ChkDR( DRM_E_NOTIMPL );

#endif /* DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP */

    }

    if( CreateThread( NULL, 0, ListenForMediaStreamingRequests, NULL, 0, NULL ) == NULL )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    while( true )
    {
        PRNDLogMessage( " " );
        PRNDLogMessage( "Waiting for PRND client connection.." );

        CSocket * pClientSocket = NULL;
        ChkDR( pServerSocket->Accept( &pClientSocket ) );
        ChkDR( pClientSocket->SetTTL( MAX_IP_HOP_COUNT ) );

        PRNDLogMessage( "Creating worker thread to process PRND client connection.." );
        if( CreateThread( NULL, 0, CPRNDSessionManager::ProcessPRNDSessionMessagesCallback, pClientSocket, 0, NULL ) == NULL )
        {
            dr = HRESULT_FROM_WIN32( GetLastError() );
            goto ErrorExit;
        }
    }

ErrorExit:
    PRNDLogMessage( "Exiting the app with Dr = 0x%X", dr );
    if( FAILED( dr ) )
    {
        printf( " Transmitter exited with failure. Look at Transmitter.log file for details" );
    }
    if( pServerSocket != NULL )
    {
        pServerSocket->Close();
        DELETE_OBJECT( pServerSocket );
    }

#if DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP
    DELETE_OBJECT( pUPnPTransmitter );
#endif /* DRM_TOOLS_PRND_TRANSMITTER_USES_UPNP */
    
    CFileLogger::Close();

    return dr;
}

