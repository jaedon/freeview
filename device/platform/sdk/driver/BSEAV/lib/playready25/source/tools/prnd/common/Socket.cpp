/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <string.h>
#include <strsafe.h>
#include <crtdbg.h>

#include <Socket.h>
#include <Logger.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CSocket::s_fInitAttempted           = FALSE;
BOOL CSocket::s_fInitialized             = FALSE;
char CSocket::s_szHostName[ NI_MAXHOST ] = "";

int CSocket::m_SocketCount        = 0;
int CSocket::m_SocketConnectCount = 0;

CSocket::CSocket( int iSockType )
    : m_iSockType( iSockType )
{
    m_SocketCount++;
    Reset();
    Initialize();
}

CSocket::CSocket( SOCKET hSocket )
{
    SOCKADDR DestAddr;
    int      cbDestAddr           = sizeof( DestAddr );
    char     szServ[ NI_MAXSERV ] = { '\0' };
    DWORD    dwSockType           = 0;
    int      cbSockType           = sizeof( dwSockType );

    Reset();
    Initialize();

    m_hSocket = hSocket;
    m_SocketCount++;

    //
    // get the addr of the destination, if this socket is connected
    //
    if( SOCKET_ERROR != getpeername( hSocket, &DestAddr, &cbDestAddr ) )
    {
        if( AF_INET6 == DestAddr.sa_family )
        {
            m_RemotePort = ( int ) ntohs( ( (LPSOCKADDR_IN6 ) &DestAddr )->sin6_port );
        }
        else if( AF_INET == DestAddr.sa_family )
        {
            m_RemotePort = ( int ) ntohs( ( (LPSOCKADDR_IN ) &DestAddr )->sin_port );
        }

        getnameinfo( &DestAddr, cbDestAddr, m_szDestHost, NI_MAXHOST, szServ, NI_MAXSERV, NI_NUMERICHOST );
        CopyMemory( &m_DestAddr, &DestAddr, sizeof( DestAddr ) );
        m_cbDestAddr = ( DWORD )cbDestAddr;

        m_fBound = TRUE;
        m_fConnected = TRUE;
    }

    // get the type of the socket
    if( 0 == getsockopt( hSocket, SOL_SOCKET, SO_TYPE, ( char * ) &dwSockType, &cbSockType ) )
    {
        m_iSockType = ( int ) dwSockType;
    }
    else
    {
        _ASSERTE( !"Cannot determine the socket type" );
    }
}

CSocket::~CSocket()
{
    m_SocketCount--;
    Close();
}

HRESULT CSocket::SetTTL( UINT nHopCount )
{
    PRNDLogFunctionEntry();
    HRESULT hr = S_OK;

    if( INVALID_SOCKET == m_hSocket )
    {
        PRNDLogMessage( "Socket not initialized" );
        ChkHR( E_FAIL );
    }
    
    if( 0 != setsockopt( m_hSocket, IPPROTO_IP, IP_TTL,  ( const char* ) &nHopCount,  sizeof( nHopCount ) ) )
    {
        ChkHR( WSA_GET_LAST_ERROR_HR() );
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}

void CSocket::Reset()
{
    PRNDLogFunctionEntry();

    // a hSocket equals to INVALID_SOCKET is a flag that this socket is no longer open.
    m_hSocket = INVALID_SOCKET;

    // a remote port of -1 means that the socket is not connected.
    m_RemotePort = -1;

    // a local port of -1 means that the socket is not connected.
    m_LocalPort = -1;

    m_szDestHost[0] = 0;

    ZeroMemory( &m_DestAddr, sizeof( m_DestAddr ) );
    m_cbDestAddr = 0;

    m_fBound = FALSE;
    m_fConnected = FALSE;
    PRNDLogFunctionExit();
}


HRESULT CSocket::OpenSocket(  USHORT af )
{
    PRNDLogFunctionEntry();
    HRESULT hr = S_OK;

    if( IsOpen() )
    {
        PRNDLogFunctionExitWithResult( hr );
        return S_OK;
    }

    // we only can do TCP or UDP
    if( SOCK_STREAM != m_iSockType && SOCK_DGRAM != m_iSockType )
    {
        hr = E_INVALIDARG;
        PRNDLogMessage( "Invalid socket type: %d", m_iSockType );
        ChkHR( hr );
    }
    else
    {
        m_hSocket = ::socket( af, m_iSockType, 0 );
        if( INVALID_SOCKET == m_hSocket )
        {
            hr = WSA_GET_LAST_ERROR_HR();
            PRNDLogMessage(  "Failed to create a socket: hr=0x%08X", hr );
            Error();
            ChkHR( hr );
        }
    }

ErrorExit:
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}


HRESULT CSocket::Connect(
    __in const SOCKADDR * pDestAddr,
    __in DWORD cbDestAddr )
{
    PRNDLogFunctionEntry();
    HRESULT hr = S_OK;
    int     iResult;
    char    szServ[ NI_MAXSERV ];

    if( IsConnected() )
    {
        hr = HRESULT_FROM_WIN32( WSAEISCONN );
        PRNDLogMessage( "Already connected. Call Close first! hr=0x%08X", hr );
        ChkHR( hr );
    }

    hr = OpenSocket( pDestAddr->sa_family );
    if( FAILED( hr ) )
    {
        ChkHR( hr );
    }

    m_SocketConnectCount++;
    PRNDLogMessage( "CSocket::Connect client - m_SocketConnectCount =  %d", m_SocketConnectCount );

    // Try to connect the socket.
    iResult = ::connect( m_hSocket, pDestAddr, ( int )cbDestAddr );
    if( SOCKET_ERROR == iResult )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        PRNDLogMessage( "Failed to connect to the server: hr=0x%08X", hr );
        Error();
        ChkHR( hr );
    }

    // Store destination info
    iResult = getnameinfo( pDestAddr, ( int )cbDestAddr, m_szDestHost, NI_MAXHOST, szServ, NI_MAXSERV, 0 );
    if( SOCKET_ERROR == iResult )
    {
        m_szDestHost[ 0 ] = 0;
    }
    m_cbDestAddr = cbDestAddr;

    CopyMemory( &m_DestAddr, pDestAddr, m_cbDestAddr );

    if( AF_INET6 == pDestAddr->sa_family )
    {
        m_RemotePort = ( int ) ntohs( ( ( LPSOCKADDR_IN6 ) pDestAddr )->sin6_port );
    }
    else if( AF_INET == pDestAddr->sa_family )
    {
        m_RemotePort = ( int ) ntohs( ( ( LPSOCKADDR_IN ) pDestAddr )->sin_port );
    }

    m_fBound = TRUE;
    m_fConnected = TRUE;

ErrorExit:
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}


HRESULT CSocket::Connect( const char * szHost, int port )
{
    PRNDLogFunctionEntry();
    HRESULT          hr        = S_OK;
    struct addrinfo *addrInfo;
    struct addrinfo *addrInfo2 = NULL;
    char             szPortNumber[100];
    int              iResult;

    if( IsConnected() )
    {
        hr = HRESULT_FROM_WIN32( WSAEISCONN );
        PRNDLogMessage( "Already connected. Call Close first! hr=0x%08X", hr );
        ChkHR( hr );
    }

    // Find the address of the requested host.
    hr = StringCchPrintfA( szPortNumber, 99, "%d", port );
    if( FAILED( hr ) ) ChkHR( hr );

    iResult = ::getaddrinfo( szHost, szPortNumber, NULL, &addrInfo );
    if( 0 != iResult )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        PRNDLogMessage(  "Failed to get the host address info: hr=0x%08X", hr );
        Error();
        ChkHR( hr );
    }

    // Keep addrinfo to free it later
    addrInfo2 = addrInfo;

    // Make a socket to ensure for a valid remote addr family
    while( NULL != addrInfo )
    {
        m_hSocket = ::socket( addrInfo->ai_family, m_iSockType, 0 );
        if( INVALID_SOCKET != m_hSocket )
        {
            break;
        }
        addrInfo = addrInfo->ai_next;
    }

    if( INVALID_SOCKET == m_hSocket )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        PRNDLogMessage( "Failed to create a socket: hr=0x%08X", hr );
        Error();
        ChkHR( hr );
    }
    else
    {
        closesocket( m_hSocket );
        m_hSocket = INVALID_SOCKET;
    }
    
    if( NULL != addrInfo )
    {
        ChkHR( Connect( addrInfo->ai_addr, ( DWORD ) addrInfo->ai_addrlen ) );
    }

ErrorExit:
    // Free addrinfo
    if( NULL != addrInfo2 )
    {
        freeaddrinfo( addrInfo2 );
    }
    PRNDLogFunctionExitWithResult( hr );

    return hr;
}


void CSocket::Error()
{
    closesocket( m_hSocket );
    Reset();
}


BOOL CSocket::IsOpen() const
{
    return ( m_hSocket != INVALID_SOCKET );
}


BOOL CSocket::IsConnected() const
{
    return m_fConnected;
}

BOOL CSocket::IsBound() const
{
    return m_fConnected || m_fBound;
}

HRESULT CSocket::Close()
{
    HRESULT hr = S_OK;

    if( IsOpen() )
    {
        int iResult = closesocket( m_hSocket );
        m_SocketConnectCount--;
        PRNDLogMessage( "CSocket::Close()  - m_SocketConnectCount =  %d", m_SocketConnectCount );

        if( 0 != iResult )
        {
            hr = WSA_GET_LAST_ERROR_HR();
        }

        Reset();
    }

    return hr;
}


HRESULT CSocket::WaitForOperation(
    IN  DWORD  dwOpFlags,
    IN  DWORD  dwTimeout,
    OUT DWORD *pdwAvailableOps )
{
    PRNDLogFunctionEntry();
    HRESULT  hr = S_OK;
    fd_set   fd_read;
    fd_set   fd_write;
    fd_set  *pfd_read = NULL;
    fd_set  *pfd_write = NULL;
    TIMEVAL  timeout;
    TIMEVAL *pTimeout;
    int      iResult;

    if( INVALID_SOCKET == m_hSocket )
    {
        ChkHR( HRESULT_FROM_WIN32( WSAENOTSOCK ) );
    }

    FD_ZERO( &fd_read );
    FD_ZERO( &fd_write );

    // FD_SET contains a constant conditional. Since it's a system header, we'll suppress the warning.
#pragma warning(push)
#pragma warning(disable:4127)
    FD_SET( m_hSocket, &fd_read );
    FD_SET( m_hSocket, &fd_write );
#pragma warning(pop)

    if( INFINITE == dwTimeout )
    {
        pTimeout = NULL;
        PRNDLogMessage( "Infinite timeout" );
    }
    else
    {
        PRNDLogMessage( "Timeout = %d", dwTimeout );
        timeout.tv_sec = ( long )( dwTimeout / 1000 );
        timeout.tv_usec = ( long )( ( dwTimeout % 1000 ) * 1000 );
        pTimeout = &timeout;
    }

    if( OP_RECV & dwOpFlags )
    {
        pfd_read = &fd_read;
    }
    else if( OP_SEND & dwOpFlags )
    {
        pfd_write = &fd_write;
    }

    iResult = ::select( 0, pfd_read, pfd_write, NULL, pTimeout );

    if( 0 == iResult )
    {
        ChkHR( HRESULT_FROM_WIN32( WAIT_TIMEOUT ) );
    }
    else if( SOCKET_ERROR == iResult )
    {
        ChkHR( WSA_GET_LAST_ERROR_HR() );
    }
    else if( NULL != pdwAvailableOps )
    {
        *pdwAvailableOps = 0;

        if( FD_ISSET( m_hSocket, &fd_read ) )
        {
            *pdwAvailableOps |= OP_RECV;
        }
        if( FD_ISSET( m_hSocket, &fd_write ) )
        {
            *pdwAvailableOps |= OP_SEND;
        }
    }
ErrorExit:
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}


HRESULT CSocket::Send( const char *pbBuffer, int cbBuffer )
{
    HRESULT hr = S_OK;

    ChkArgHR( cbBuffer );

    if( NULL == pbBuffer )
    {
        hr = E_INVALIDARG;
        ChkHR( hr );
    }

    // check if the socket is connected
    if( ! IsConnected() )
    {
        hr = HRESULT_FROM_WIN32( WSAENOTCONN );
        PRNDLogMessage( "Socket is not connected. Call Connect first!", hr );
        ChkHR( hr );
    }

    // number of bytes that have been sent.
    int count = 0;

    do {
        // Try to send all data that remains.
        int len = ::send( m_hSocket, pbBuffer + count, cbBuffer - count, 0 );

        // Complain if we encounter an error.
        if( len == SOCKET_ERROR  )
        {
            hr = WSA_GET_LAST_ERROR_HR();
            PRNDLogMessage( "Error sending data through the socket: 0x%X", hr );
            Error();
            ChkHR( hr );
        }
        count += len;
    } while( count < cbBuffer );

ErrorExit:
    return hr;
}


HRESULT CSocket::SendTo( const char *pbBuffer, int cbBuffer, const sockaddr *pDestAddr, int cbDestAddr )
{
    PRNDLogFunctionEntry();
    HRESULT hr         = S_OK;
    int     iSentTotal = 0;

    if( NULL == pDestAddr || 0 == cbDestAddr )
    {
        hr = E_INVALIDARG;
        PRNDLogMessage( "A valid destination addr is required." );
        ChkHR( hr );
    }

    if( 0 == cbBuffer )
    {
        ChkHR( hr );
    }

    if( NULL == pbBuffer )
    {
        hr = E_INVALIDARG;
        PRNDLogMessage(  "A valid buffer is required." );
        ChkHR( hr );
    }

        hr = OpenSocket( pDestAddr->sa_family );
        if( FAILED( hr ) )
        {
            ChkHR( hr );
        }

    if( IsConnected() )
    {
        // check if the connected destination matches the given one
        if(    ( DWORD )cbDestAddr != m_cbDestAddr
             || 0 != memcmp( &m_DestAddr, pDestAddr, ( size_t )cbDestAddr ) )
        {
            hr = E_UNEXPECTED;
            PRNDLogMessage( "This socket is connected to a different destination. Call Close first!" );
            ChkHR( hr );
        }
    }

    // now send the buffer to destination
    iSentTotal = 0;
    do
    {
        int iSent = ::sendto( m_hSocket, pbBuffer + iSentTotal, cbBuffer - iSentTotal, 0, pDestAddr, cbDestAddr );

        if( SOCKET_ERROR == iSent )
        {
            hr = WSA_GET_LAST_ERROR_HR();
            PRNDLogMessage( "::sento failed: hr=0x%08X", hr );
            ChkHR( hr );
        }

        iSentTotal += iSent;
    }
    while( iSentTotal < cbBuffer );

    m_fBound = TRUE;

ErrorExit:
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}


HRESULT CSocket::RecvBurst( char *pData, int *pcbData )
{
    PRNDLogFunctionEntry();
    HRESULT hr  = S_OK;
    int     len = 0;

    ChkArgHR( pcbData );
    ChkArgHR( pData );
    ChkArgHR( *pcbData );

    // check if the socket is bound
    if( ! IsBound() )
    {
        hr = E_UNEXPECTED;
        PRNDLogMessage( "Socket is not bound. Call Connect or SendTo first!", hr );
        ChkHR( hr );
    }

    len = recv( m_hSocket, pData, *pcbData, 0 );
    if( SOCKET_ERROR == len || 0 == len )
    {
        // Complain if we encounter an error.  While a returned value of 0 is not
        // technically an error code, but rather means "the connection was gracefully
        // closed" by the other end, it still means that we won't be able to receive all
        // the data, and that is why we consider it an error situation.
        hr = WSA_GET_LAST_ERROR_HR();
        if( SUCCEEDED( hr ) )
        {
            hr = HRESULT_FROM_WIN32( WSAECONNABORTED );
        }
        PRNDLogMessage(  "::recv failed: hr=0x%08X", hr );
        ChkHR( hr );
    }
    PRNDLogMessage( "Received byte count = %d", len );

    *pcbData   = len;

ErrorExit:
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}


HRESULT CSocket::Recv( char *pData, int *pcbData, int flags )
{
    HRESULT hr = S_OK;

    ChkArgHR( pcbData );
    ChkArgHR( pData );
    ChkArgHR( *pcbData );

    // check if the socket is bound
    if( ! IsBound() )
    {
        hr = E_UNEXPECTED;
        PRNDLogMessage( "Socket is not bound. Call Connect or SendTo first!", hr );
        ChkHR( hr );
    }

    int cbRequested = *pcbData;
    while( cbRequested > 0 )
    {
        int iResult = recv( m_hSocket, pData, cbRequested, flags );
        if( SOCKET_ERROR == iResult || 0 == iResult )
        {
            // Complain if we encounter an error.  While a returned value of 0 is not
            // technically an error code, but rather means "the connection was gracefully
            // closed" by the other end, it still means that we won't be able to receive all
            // the data, and that is why we consider it an error situation.
            hr = WSA_GET_LAST_ERROR_HR();
            if( SUCCEEDED( hr ) )
            {
                hr = HRESULT_FROM_WIN32( WSAECONNABORTED );
            }
            PRNDLogMessage(  "::recv failed: hr=0x%08X", hr );
            ChkHR( hr );
        }
#if 0        
        PRNDLogMessage( "Received byte count = %d", iResult );
#endif
        pData       += iResult;
        cbRequested -= iResult;

    }

ErrorExit:
    return hr;
}


HRESULT CSocket::RecvFrom(
    IN     char     *pbData,
    IN OUT int      *pcbData,
    OUT    SOCKADDR *pFromAddr,
    IN OUT int      *pcbFromAddr )
{
    PRNDLogFunctionEntry();
    HRESULT hr = S_OK;
    int     cbData;

    if( NULL == pbData || NULL == pcbData )
    {
        return E_POINTER;
    }
    // check if the socket is bound
    if( !IsBound() )
    {
        hr = E_UNEXPECTED;
        PRNDLogMessage( "Socket is not bound. Call Bind or SendTo first!", hr );
        return hr;
    }

    cbData = ::recvfrom( m_hSocket, pbData, *pcbData, 0, pFromAddr, pcbFromAddr );
    if( 0 == cbData )
    {
        hr = HRESULT_FROM_WIN32( WSAECONNABORTED );
    }
    else if( SOCKET_ERROR == cbData )
    {
        hr = WSA_GET_LAST_ERROR_HR();
    }
    else
    {
        *pcbData = cbData;
    }

    PRNDLogFunctionExitWithResult( hr );
    return hr;
}


HRESULT CSocket::Bind(
    IN     WORD      wPort,
    IN     BOOL      fIPv6,
    OUT    SOCKADDR *pLocalAddr,
    IN OUT DWORD    *pcbLocalAddr )
{
    PRNDLogFunctionEntry();
    HRESULT          hr = S_OK;
    SOCKADDR_STORAGE LocalAddr;
    DWORD            cbLocalAddr;

    if( NULL != pcbLocalAddr && NULL == pLocalAddr )
    {
        return E_POINTER;
    }

    if( IsBound() )
    {
        hr = E_FAIL;
        PRNDLogMessage( "Already bound. Call Close first! hr=0x%08X", hr );
        return hr;
    }

    //
    // Determine the local addr to bind
    //
    hr = GetLocalAddress( &LocalAddr, &cbLocalAddr, fIPv6 );
    if( FAILED( hr ) )
    {
        return hr;
    }

    if( pcbLocalAddr && *pcbLocalAddr < cbLocalAddr )
    {
        return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
    }

    if( fIPv6 )
    {
        PSOCKADDR_IN6 pLocalAddr6 = ( PSOCKADDR_IN6 ) &LocalAddr;
        pLocalAddr6->sin6_port = htons( wPort );
    }
    else
    {
        PSOCKADDR_IN pLocalAddr4 = ( PSOCKADDR_IN ) &LocalAddr;
        pLocalAddr4->sin_port  = htons( wPort );
    }

    //
    // Open the socket
    //
    hr = OpenSocket( LocalAddr.ss_family );

    if( SUCCEEDED( hr ) )
    {
        //
        // Now bind to the local addr
        //
        if( SOCKET_ERROR == ::bind( m_hSocket, ( SOCKADDR * ) &LocalAddr, ( int )cbLocalAddr ) )
        {
            hr = WSA_GET_LAST_ERROR_HR();
            PRNDLogMessage( "::bind failed: hr=0x%08X", hr );
        }
        else
        {
            if( NULL != pcbLocalAddr )
            {
                CopyMemory( pLocalAddr, &LocalAddr, cbLocalAddr );
                *pcbLocalAddr = cbLocalAddr;
            }
            m_fBound = TRUE;
        }
    }

    PRNDLogFunctionExitWithResult( hr );
    return hr;
}

const char* CSocket::GetRemoteHost() const
{
    return m_szDestHost;
}


HRESULT CSocket::GetRemoteAddress(
    OUT SOCKADDR_STORAGE *pRemoteAddr,
    OUT DWORD            *pcbRemoteAddr )
{
    if( NULL == pRemoteAddr || NULL == pcbRemoteAddr )
    {
        return E_POINTER;
    }

    if( !IsConnected() )
    {
        return HRESULT_FROM_WIN32( WSAENOTCONN );
    }

    CopyMemory( pRemoteAddr, &m_DestAddr, m_cbDestAddr );
    *pcbRemoteAddr = m_cbDestAddr;
    return S_OK;
}

int CSocket::GetLocalPort() const
{
    return m_LocalPort;
}


int CSocket::GetRemotePort() const
{
    return m_RemotePort;
}


const WCHAR* CSocket::GetLastError() const
{
    return m_wszLastError;
}


const char* CSocket::GetLocalHostName()
{
    Initialize();
    if( 0 == s_szHostName[0] )
    {
        gethostname( s_szHostName, NI_MAXHOST );
    }
    return s_szHostName;
}

void CSocket::Initialize()
{
    if( !s_fInitAttempted )
    {
        s_fInitAttempted = TRUE;
        WSADATA wsaData;

        s_fInitialized = WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) == 0;
    }
}

HRESULT CSocket::GetLocalAddress(
    OUT SOCKADDR_STORAGE *pSockAddr,
    OUT DWORD            *pcbSockAddr,
    IN  BOOL              fIPv6 )
{
    PRNDLogFunctionEntry();
    HRESULT   hr      = S_OK;
    ADDRINFO *ai      = NULL;
    ADDRINFO *ai2     = NULL;
    int       iResult = 0;
    char      szHostName[MAX_PATH] = "localhost";
    
    if( NULL == pSockAddr || NULL == pcbSockAddr )
    {
        return E_POINTER;
    }

    Initialize();

    ZeroMemory( pSockAddr, sizeof( SOCKADDR_STORAGE ) );

    iResult = gethostname( szHostName, MAX_PATH );
    if( SOCKET_ERROR == iResult )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        ChkHR( hr );
    }

    iResult = getaddrinfo( szHostName, NULL, NULL, &ai );
    if( 0 != iResult )
    {
        hr = WSA_GET_LAST_ERROR_HR();
        ChkHR( hr );
    }

    // Keep addrinfo to free it later
    ai2 = ai;

    hr = WSAEPFNOSUPPORT;
    while( NULL != ai )
    {
        if( fIPv6 && ai->ai_family == AF_INET6 )
        {
            memcpy( pSockAddr, ai->ai_addr, ( DWORD ) ai->ai_addrlen );
           *pcbSockAddr = ( DWORD ) ai->ai_addrlen;
            hr = S_OK;
            break;
        }
        else if( !fIPv6 && ai->ai_family == AF_INET )
        {
            memcpy( pSockAddr, ai->ai_addr, ( DWORD ) ai->ai_addrlen );
           *pcbSockAddr = ( DWORD ) ai->ai_addrlen;
            hr = S_OK;
            break;
        }
        ai = ai->ai_next;
    }

ErrorExit:
    if( NULL != ai2 )
    {
        freeaddrinfo( ai2 );
    }
    PRNDLogFunctionExitWithResult( hr );
    return hr;
}

