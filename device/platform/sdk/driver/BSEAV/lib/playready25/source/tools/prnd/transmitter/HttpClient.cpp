/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <HttpClient.h>
#include <FileLogger.h>
#include <HelperFunctions.h>
#include <FuzzFunctions.h>

#define  HTTP_RESPONSE_MAX_SIZE          4096
#define  HTTP_BURST_WAIT_TIME            5000   //milliseconds

#define  MAX_CACHE_PACKET_COUNT          1000   //24MB
#define  BUFFERING_STATUS_LOG_DURATION   30     //sec

using namespace std;

CHttpClient::CHttpClient( std::string strHttpPath )
{
    m_strHttpPath = strHttpPath;
    m_hWorkerThread = INVALID_HANDLE_VALUE;
    m_bShutdown     = false;

    m_dwLastTimeDroppedPacketLogged = 0;
    m_dwLastTimeCacheEmptyLogged    = 0;
    
}

CHttpClient::~CHttpClient()
{
    PRNDLogFunctionEntry();
    
    InitiateShutdown();
    if( m_hWorkerThread != INVALID_HANDLE_VALUE )
    {
        WaitForSingleObject( m_hWorkerThread, INFINITE ); 
        CLOSE_HANDLE( m_hWorkerThread );
    }
    
    PRNDLogFunctionExit();
}

string CHttpClient::GetResponseHeader()
{
    return m_strResponseHeaders;
}

void CHttpClient::InitiateShutdown()
{
    PRNDLogFunctionEntry();
    CDRMAutoLock lock( &m_lock );

    m_bShutdown = true;
    PRNDLogFunctionExit();
}

bool CHttpClient::ShutdownInitiated()
{
    CDRMAutoLock lock( &m_lock );

    return m_bShutdown;
}

HRESULT CHttpClient::ReadResponse()
{
    PRNDLogFunctionEntry();
    HRESULT dr                       = DRM_SUCCESS;
    bool    bResponseHeadersFound    = false;
    char   *pbResponse               = NULL;
    int     cbResponseSize           = 0;

    ChkMem( pbResponse = new char[ HTTP_RESPONSE_MAX_SIZE ] );

    while( cbResponseSize < HTTP_RESPONSE_MAX_SIZE )
    {
        char   *pbBuffer     = pbResponse + cbResponseSize;
        int     cbBufferSize = HTTP_RESPONSE_MAX_SIZE - cbResponseSize;
        
        ChkDR( m_pSocket->RecvBurst( pbBuffer, &cbBufferSize ) );
        cbResponseSize += cbBufferSize;

        pbBuffer[ cbBufferSize ] = 0;
        char* pbEnd = strstr( pbBuffer, "\r\n\r\n" );
        if( NULL != pbEnd )
        {
            bResponseHeadersFound = true;
            break;
        }

        ChkDR( m_pSocket->WaitForOperation( CSocket::OP_RECV, HTTP_BURST_WAIT_TIME, NULL ) );        
    }
    
    PRNDLogHexDump( ( BYTE* )pbResponse, ( DWORD )cbResponseSize, true );
    if( bResponseHeadersFound )
    {
        m_strResponseHeaders.assign( pbResponse, ( unsigned int )cbResponseSize );
    }

ErrorExit:
    DELETE_OBJECT_ARRAY( pbResponse );
    
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}



HRESULT CHttpClient::ReadResponseBody()
{
    PRNDLogFunctionEntry();
    HRESULT dr                     = DRM_SUCCESS;
    int     cbPacketSize           = TS_PACKET_SIZE;

    while( true )
    {
        vector<BYTE>    vbPacket( TS_PACKET_SIZE );
        
        if( ShutdownInitiated() )
        {
            break;
        }
        ChkDR( m_pSocket->Recv( ( char* )&vbPacket[0], &cbPacketSize, 0 ) );
        AddPacket( vbPacket );
    }
    
ErrorExit:
    InitiateShutdown();

    if( m_pSocket )
    {
        m_pSocket->Close();
        DELETE_OBJECT( m_pSocket );
    }

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

void CHttpClient::AddPacket( vector<BYTE> &vbPacket )
{
    CDRMAutoLock lock( &m_lock );
    DWORD dwNow = GetCurrentTimeInSeconds();

    if( m_qPackets.size() > MAX_CACHE_PACKET_COUNT )
    {
        if( dwNow - m_dwLastTimeDroppedPacketLogged > BUFFERING_STATUS_LOG_DURATION )
        {
            PRNDLogMessage( "CHttpClient::AddPacket() -- Dropping packet from live input stream due to cache full" );
            m_dwLastTimeDroppedPacketLogged = dwNow;
        }
    }
    else
    {
        if( dwNow - m_dwLastTimeDroppedPacketLogged > BUFFERING_STATUS_LOG_DURATION )
        {
            PRNDLogMessage( "CHttpClient::AddPacket() -- Adding packet from live input stream..." );
            m_dwLastTimeDroppedPacketLogged = dwNow;
#if 0            
            FuzzTSPacket( vbPacket );
            DuplicateTSPacket( vbPacket );
#endif 
        }
        m_qPackets.push_back( std::move( vbPacket ) );
    }
}

vector<BYTE> CHttpClient::GetPacket()
{
    CDRMAutoLock lock( &m_lock );
    vector<BYTE> vbPacket;

    if( m_qPackets.empty() )
    {
        DWORD dwNow = GetCurrentTimeInSeconds();
        if( dwNow - m_dwLastTimeCacheEmptyLogged > BUFFERING_STATUS_LOG_DURATION )
        {
            PRNDLogMessage( "CHttpClient::GetPacket() -- Cache is empty" );
            m_dwLastTimeCacheEmptyLogged = dwNow;
        }
    }
    else
    {
        vbPacket = m_qPackets.front();
        m_qPackets.pop_front();
    }
    return vbPacket;
}

HRESULT CHttpClient::ParseHostNameAndPort()
{
    PRNDLogFunctionEntry();
    HRESULT dr  = DRM_SUCCESS;
    
    vector<string>  vWords;
    string str = m_strHttpPath.substr( string( "http://" ).size() );
    str = str.substr( 0, str.find( "/" ) );

    PRNDLogMessage( "Parsing %s", str.c_str() );
    string_split( str, ":", vWords );
    
    if( vWords.size() == 1 )
    {
        m_strHostName = vWords[0];
    }
    else if( vWords.size() == 2 )
    {
        m_strHostName   = vWords[0];
        m_strPort       = vWords[1];
    }
    else
    {
        ChkDR( E_FAIL );
    }
    PRNDLogMessage( "Host name  =  %s", m_strHostName.c_str() );
    PRNDLogMessage( "Port       = %s", m_strPort.c_str() );
       
ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpClient::MakeRequest()
{
    PRNDLogFunctionEntry();

    HRESULT     dr                          = DRM_SUCCESS;
    string      strRequestHeaders;
    string      strHost = m_strHostName;

    if( !m_strPort.empty() )
    {
        strHost = strHost + ":" + m_strPort;
    }
    strRequestHeaders   = "GET ";
    strRequestHeaders  += m_strHttpPath +" HTTP/1.1\r\n";
    strRequestHeaders  += "Host: " + strHost + "\r\n";
    strRequestHeaders  += "Connection: Keep-Alive\r\n";
    strRequestHeaders  += "\r\n";
    
    PRNDLogHexDump( ( BYTE* )strRequestHeaders.c_str(), ( DWORD ) strRequestHeaders.length(), true );
    ChkDR( m_pSocket->Send( ( char* )strRequestHeaders.c_str(), ( int )strRequestHeaders.length() ) );

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

HRESULT CHttpClient::ReadHttpStream()
{
    PRNDLogFunctionEntry();

    HRESULT     dr      = DRM_SUCCESS;
    CSocket    *pSocket = NULL;

    ChkDR( ParseHostNameAndPort() );
    ChkMem( pSocket = new CSocket( SOCK_STREAM ) );
    m_pSocket = pSocket;

    ChkDR( pSocket->Connect( m_strHostName.c_str(), std::stoi( m_strPort ) ) );

    ChkDR( MakeRequest() );
    ChkDR( ReadResponse() );


ErrorExit:

    PRNDLogFunctionExitWithResult( dr );
    return dr;
}

DWORD WINAPI CHttpClient::ReadResponseBodyCallback( __in LPVOID lpParam )
{
    PRNDLogFunctionEntry();

    HRESULT             dr          = S_OK;
    CHttpClient        *pHttpClient = ( CHttpClient * )lpParam;

    if( pHttpClient != nullptr )
    {
        pHttpClient->ReadResponseBody();
    }
    
    PRNDLogFunctionExitWithResult( dr );
    return ( DWORD )dr;
}


HRESULT CHttpClient::ReadResponseBodyAsync()
{
    PRNDLogFunctionEntry();

    HRESULT  dr         = DRM_SUCCESS;
    HANDLE   hThread    = INVALID_HANDLE_VALUE;

    hThread = CreateThread( NULL, 0, CHttpClient::ReadResponseBodyCallback, this, 0, NULL );
    if( hThread == NULL )
    {
        dr = HRESULT_FROM_WIN32( GetLastError());
        goto ErrorExit;
    }
    
    m_hWorkerThread = hThread;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}


DWORD WINAPI CHttpClient::ReadHttpStreamCallback( __in LPVOID lpParam )
{
    PRNDLogFunctionEntry();

    HRESULT             dr            = S_OK;
    CHttpClient        *pHttpClient   = ( CHttpClient * )lpParam;

    if( pHttpClient != nullptr )
    {
        pHttpClient->ReadHttpStream();
    }
    
    PRNDLogFunctionExitWithResult( dr );
    return ( DWORD )dr;
}


HRESULT CHttpClient::ReadHttpStreamAsync()
{
    PRNDLogFunctionEntry();

    HRESULT  dr         = DRM_SUCCESS;
    HANDLE   hThread    = INVALID_HANDLE_VALUE;

    hThread = CreateThread( NULL, 0, CHttpClient::ReadHttpStreamCallback, this, 0, NULL );
    if( hThread == NULL )
    {
        dr = HRESULT_FROM_WIN32( GetLastError());
        goto ErrorExit;
    }
    
    m_hWorkerThread = hThread;

ErrorExit:
    PRNDLogFunctionExitWithResult( dr );
    return dr;
}
