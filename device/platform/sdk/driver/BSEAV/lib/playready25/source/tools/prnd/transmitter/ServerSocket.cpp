/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <ServerSocket.h>
#include <strsafe.h>
#include <FileLogger.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CServerSocket::m_ServerSocketCount = 0;

CServerSocket::CServerSocket()
{
    // Use m_hIPv4Socket = 0 as a flag that the socket is closed.
    m_hIPv4Socket = INVALID_SOCKET;
    m_hIPv6Socket = INVALID_SOCKET;

    m_bSelected = FALSE;

    // Use m_LocalPort = -1 to indicate the the server is not bound
    // to any port;
    m_LocalPort = -1;
    m_wszLastError[0] = 0;

    m_ServerSocketCount++;
    PRNDLogMessage( "CServerSocket::CServerSocket - m_ServerSocketCount =  %d", m_ServerSocketCount );

    InitializeCriticalSection( &m_csLock );

    CSocket::Initialize();
}

CServerSocket :: ~CServerSocket()
{
    m_ServerSocketCount--;
    PRNDLogMessage( "CServerSocket::~CServerSocket - m_ServerSocketCount =  %d", m_ServerSocketCount );

    if( m_hIPv4Socket != INVALID_SOCKET )
    {
        closesocket( m_hIPv4Socket );
        m_hIPv4Socket = INVALID_SOCKET;
    }

    if( m_hIPv6Socket != INVALID_SOCKET )
    {
        closesocket( m_hIPv6Socket );
        m_hIPv6Socket = INVALID_SOCKET;
    }

    DeleteCriticalSection( &m_csLock );
}

HRESULT CServerSocket::Serve( int maxClients, int SockType, int port )
{
    PRNDLogFunctionEntry();
    HRESULT            hr = S_OK;
    SOCKADDR_IN        sockAddrIn;
    SOCKADDR_IN6       sockAddrIn6;
    SOCKADDR_STORAGE  *pSockAddr;
    int                sockAddrLen;
    int                iResult;

    ZeroMemory( &sockAddrIn, sizeof( SOCKADDR_IN ) );
    ZeroMemory( &sockAddrIn6, sizeof( SOCKADDR_IN6 ) );
    IN6ADDR_SETANY( &sockAddrIn6 );

    PRNDLogMessage( "Creating IPv4 socket..." );
    m_hIPv4Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( m_hIPv4Socket == INVALID_SOCKET )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        PRNDLogMessage( "Failed to create ipv4 socket: hr=0x%08X", hr );
        ChkHR( hr );
    }

    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_addr.s_addr = INADDR_ANY;
    sockAddrIn.sin_port = htons( ( unsigned short )port );
    pSockAddr = ( SOCKADDR_STORAGE * ) &sockAddrIn;
    sockAddrLen = sizeof( SOCKADDR_IN );

    PRNDLogMessage( "Binding to IPv4 socket.." );
    iResult = bind( m_hIPv4Socket, ( sockaddr * )( pSockAddr ), sockAddrLen );
    if( SOCKET_ERROR == iResult ) 
    {
        hr = WSA_GET_LAST_ERROR_HR();
        PRNDLogMessage( "Failed to bind ipv4 socket to port %d: hr=0x%08X", port, hr );
        ::closesocket( m_hIPv4Socket );
        m_hIPv4Socket = INVALID_SOCKET;
        ChkHR( hr );
    }

    PRNDLogMessage( "Listening on IPv4 ..." );
    iResult = listen( m_hIPv4Socket, maxClients );
    if( SOCKET_ERROR == iResult ) 
    {
        hr = WSA_GET_LAST_ERROR_HR();
        PRNDLogMessage(  "Failed to make ipv4 socket listen to port %d: hr=0x%08X", port, hr );
        ::closesocket( m_hIPv4Socket );
        m_hIPv4Socket = INVALID_SOCKET;
        ChkHR( hr );
    }

    PRNDLogMessage( "Creating IPv6 socket..." );
    m_hIPv6Socket = socket( AF_INET6, SOCK_STREAM, 0 );
    if( m_hIPv6Socket == INVALID_SOCKET ) 
    {
        hr = WSA_GET_LAST_ERROR_HR();
        if( hr == HRESULT_FROM_WIN32( WSAEAFNOSUPPORT ) )
        {   // ipv6 is optional on XP
            PRNDLogMessage( " ipv6 not supported" );
            hr = S_OK;
        }
        else
        {
            PRNDLogMessage(  "Failed to create ipv6 socket: hr=0x%08X", hr );
            ChkHR( hr );
        }
    }

    if( m_hIPv6Socket != INVALID_SOCKET )
    {
        sockAddrIn6.sin6_family = AF_INET6;
        sockAddrIn6.sin6_port = htons( ( unsigned short )port );
        pSockAddr = ( SOCKADDR_STORAGE * ) &sockAddrIn6;
        sockAddrLen = sizeof( SOCKADDR_IN6 );

        PRNDLogMessage( "Binding to IPv6 socket..." );
        iResult = bind( m_hIPv6Socket, ( sockaddr * )( pSockAddr ), sockAddrLen );
        if( SOCKET_ERROR == iResult ) 
        {
            hr = WSA_GET_LAST_ERROR_HR();
            PRNDLogMessage(  "Failed to bind ipv6 socket to port %d: hr=0x%08X", port, hr );
            ::closesocket( m_hIPv6Socket );
            m_hIPv6Socket = INVALID_SOCKET;
            ChkHR( hr );
        }

        PRNDLogMessage( "Listening on IPv6 ..." );
        iResult = listen( m_hIPv6Socket, maxClients );
        if( SOCKET_ERROR == iResult ) 
        {
            hr = WSA_GET_LAST_ERROR_HR();
            PRNDLogMessage(  "Failed to make ipv6 socket listen to port %d: hr=0x%08X", port, hr );
            ::closesocket( m_hIPv6Socket );
            m_hIPv6Socket = INVALID_SOCKET;
            ChkHR( hr );
        }
    }

    m_LocalPort = port;
    m_SocketType = SockType;

ErrorExit:
    if( FAILED( hr ))
    {
        if( m_hIPv4Socket != INVALID_SOCKET )
        {
            closesocket( m_hIPv4Socket );
            m_hIPv4Socket = INVALID_SOCKET;
        }

        if( m_hIPv6Socket != INVALID_SOCKET )
        {
            closesocket( m_hIPv6Socket );
            m_hIPv6Socket = INVALID_SOCKET;
        }
    }

    PRNDLogFunctionExitWithResult( hr );
    return hr;
}

BOOL CServerSocket::IsOpen()  const
{
    return ( m_hIPv4Socket != INVALID_SOCKET || m_hIPv6Socket != INVALID_SOCKET );
}

static int times_accepted = 0;

HRESULT CServerSocket::Accept( CSocket **ppSocket )
{
    PRNDLogFunctionEntry();
    HRESULT          hr            = S_OK;
    int              sizeOfFD      = 0;
    size_t           size          = sizeof( SOCKADDR_STORAGE );
    SOCKET           hClientSocket = INVALID_SOCKET;
    SOCKADDR_STORAGE addr;
    timeval          tv;
    
    tv.tv_sec = 15;
    tv.tv_usec = 0;

    if( NULL == ppSocket ) return E_POINTER;

    EnterCriticalSection( &m_csLock );

    if( m_hIPv4Socket == INVALID_SOCKET && m_hIPv6Socket == INVALID_SOCKET )
    {
        PRNDLogMessage(  "m_hIPv4Socket == INVALID_SOCKET && m_hIPv6Socket == INVALID_SOCKET in %s line %d", __FILE__, __LINE__ );
        ChkHR( HRESULT_FROM_WIN32( WSAENOTSOCK )); //socket has been shutdown
    }

    if( !m_bSelected )
    {
        FD_ZERO( &m_fdAccept );
        if( m_hIPv4Socket != INVALID_SOCKET )
        {
            FD_SET( m_hIPv4Socket, &m_fdAccept );
            sizeOfFD++;
        }
        if( m_hIPv6Socket != INVALID_SOCKET )
        {
            FD_SET( m_hIPv6Socket, &m_fdAccept );
            sizeOfFD++;
        }
        if( sizeOfFD == 0 )
        {
            PRNDLogMessage(  "sizeOfFD = %d because m_hIPv4Socket == INVALID_SOCKET && m_hIPv6Socket == INVALID_SOCKET in %s line %d", sizeOfFD,  __FILE__, __LINE__ );
            ChkHR( E_UNEXPECTED ); //socket has been shutdown
        }
        for( DWORD  i =0; i < m_fdAccept.fd_count;i++ )
        {
            PRNDLogMessage( "m_fdAccept.fd_count [%d] = %p ", i , m_fdAccept.fd_array[i] );
        }
        m_bSelected = TRUE;
    }

    //test if socket is closed
    if( m_hIPv4Socket == INVALID_SOCKET && m_hIPv6Socket == INVALID_SOCKET )
    {
        PRNDLogMessage(  "m_hIPv4Socket == INVALID_SOCKET && m_hIPv6Socket == INVALID_SOCKET in %s line %d", __FILE__, __LINE__ );
        ChkHR( HRESULT_FROM_WIN32( WSAENOTSOCK )); //socket has been shutdown
    }

    if( m_hIPv4Socket != INVALID_SOCKET && FD_ISSET( m_hIPv4Socket, &m_fdAccept ))
    {
        PRNDLogMessage( "Accepting IPv4 connection.." );
        hClientSocket = ::accept( m_hIPv4Socket, ( sockaddr * ) &addr, ( int* ) &size );
    }
    else if( m_hIPv6Socket != INVALID_SOCKET && FD_ISSET( m_hIPv6Socket, &m_fdAccept ))
    {
        PRNDLogMessage( "Accepting IPv6 connection.." );
        hClientSocket = ::accept( m_hIPv6Socket, ( sockaddr * ) &addr, ( int* ) &size );
    }
    else
    {
        hr = E_UNEXPECTED;
        ChkHR( hr );
    }

    times_accepted++;
    PRNDLogMessage(  "times_accepted =  %d", times_accepted );

    if( INVALID_SOCKET == hClientSocket )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        ChkHR( hr );
    }

    // Return a new CSocket for this connection.
    *ppSocket = new CSocket( hClientSocket );

ErrorExit:
    LeaveCriticalSection( &m_csLock );
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}

int CServerSocket::GetLocalPort() const
{
    return m_LocalPort;
}

void CServerSocket::Close()
{
    PRNDLogFunctionEntry();
    // Closing a CServerSocket requires a bit of a hack - the server socket
    // will most probably be blocked on ::accept() ( see above ), and
    // we need to unblock it.  However, we cannot use the WSACancelBlockingCall()
    // function, because it is obsolete.  So, we just connect to it.
    EnterCriticalSection( &m_csLock );
    ::closesocket( m_hIPv4Socket );
    m_hIPv4Socket = INVALID_SOCKET;

    ::closesocket( m_hIPv6Socket );
    m_hIPv6Socket = INVALID_SOCKET;
    CSocket dummy( m_SocketType );
    dummy.Connect( "127.0.0.1", m_LocalPort );
    dummy.Close();

    m_bSelected = FALSE;
    LeaveCriticalSection( &m_csLock );
    PRNDLogFunctionExit();
}

const WCHAR* CServerSocket::GetLastError()
{
    return m_wszLastError;
}

