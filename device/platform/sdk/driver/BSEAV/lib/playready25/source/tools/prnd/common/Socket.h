/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

class CSocket
{

public:
    CSocket( int iSocketType );
    virtual ~CSocket();

    // Special version of the constructor for building sockets
    // on the server side.
    CSocket( SOCKET fd );

    ///////////////////////////////////////////////////////////////////////////
    // Manipulators
    ///////////////////////////////////////////////////////////////////////////

    // Connect this socket to the given host, port
    HRESULT Connect( const char *host, int port );

    // Connect this socket to the given destination
    HRESULT Connect( IN const SOCKADDR *pDestAddr, IN DWORD cbDestAddr );

    HRESULT CSocket::SetTTL( UINT nHopCount );
    
    // Close the socket
    HRESULT Close();

    // Send the given buffer to ( connected ) destination. Blocking call.
    HRESULT Send( const char *pbBuffer, int cbBuffer );

    // Receive data from the ( bound ) destination. Blocking call.
    HRESULT Recv( /* [in] */ char *pData, /* [in, out] */ int *pcbData, int flags );

    HRESULT CSocket::RecvBurst( /* [in] */ char *pData, /* [in, out] */ int *pcbData );

    // Receive data from a destination. Blocking call.
    //
    // The RecvFrom call requires the socket to be bound to a local address which can be done in 2 ways:
    //      - Explicit call to Bind
    //      - Implicitly binding thru a SendTo call
    //
    // For connection oriented sockets ( e.g. SOCK_STREAM ) RecvFrom behaves in the same way as Recv call.
    // Note that this call will fail if the socket is connected.
    // For connectionless sockets ( SOCK_DGRAM ), either a SendTo or a Bind call has to be issued before
    // a RecvFrom call, otherwise a failure is returned.
    //
    HRESULT RecvFrom(
        IN     char     *pbData,
        IN OUT int      *pcbData,
        OUT    SOCKADDR *pFromAddr = NULL,
        OUT    int      *pcbFromAddr = NULL );


    // Send the given buffer to given destination. Blocking call.
    HRESULT SendTo( const char *pbBuffer, int cbBuffer, const sockaddr *pDestAddr, int cbDestAddr );

    // Perform an explicit bind on the given port nr. Blocking call.
    // For TCP/IP if port nr is given as zero, an unused port between 1024 and 5000 is assigned.
    // The bound local address is copied to pLocalAddr, if it is not NULL.
    //
    // Usually one doesn't need to call bind explicitly for the client sockets. See RecvFrom above
    HRESULT Bind(
        IN     WORD      wPort,
        IN     BOOL      fIPv6,
        OUT    SOCKADDR *pLocalAddr   = NULL,
        IN OUT DWORD    *pcbLocalAddr = NULL );


    ///////////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////////

    enum OPTYPE
    {
        OP_RECV = 1,
        OP_SEND = 2
    };

    // Wait for the specified operation( s ) to be available within the specified timeout period
    // Return HRESULT_FROM_WIN32( WAIT_TIMEOUT ) in case of timeout
    // When succeeded, the available operation( s ) are set in the pdwAvailableOps flag
    HRESULT WaitForOperation(
        IN DWORD dwOpFlags,
        IN DWORD dwTimeout,
        OUT DWORD * pdwAvailableOps );


    // Return true if this socket is created.
    BOOL IsOpen() const;

    // Return true if this socket is connected.
    BOOL IsConnected() const;

    // Return true if this socket is bound.
    BOOL IsBound() const;

    // Return the file descriptor that this socket uses.
    // This gives away part of the representation, but it's necessary
    SOCKET GetHandle() const
    {
        return m_hSocket;
    };

    // Returns the remote machine name to which this socket is connected.
    // If the socket is not connected or if an error occured, "" is returned.
    const char * GetRemoteHost() const;

    // Returns the remote machine address to which this socket is connected
    // If the socket is not connected a failed hr is returned
    HRESULT GetRemoteAddress(
        OUT SOCKADDR_STORAGE *pRemoteAddr,
        OUT DWORD            *pcbRemoteAddr );

    // Returns the remote port to which this socket is connected.
    // If the socket is not connected or if an error occured, -1 is returned.
    int GetRemotePort() const;

    // Returns the local port on which this socket is connected.
    // If the socket is not connected or if an error occured, -1 is returned.
    int GetLocalPort() const;

    // Returns the last error, as returned by the function WSAGetLastError().
    const WCHAR * GetLastError() const;

    int GetType() const
    {
        return m_iSockType;
    }

    static void Initialize();

private:
    static char s_szHostName[ NI_MAXHOST ];

    WCHAR m_wszLastError[501];
    // File descriptor for connection.
    SOCKET m_hSocket;

    // Socket type ( either SOCK_STREAM or SOCK_DGRAM )
    int m_iSockType;

    // TRUE if the socket is bound; FALSE otherwise
    BOOL m_fBound;

    // TRUE if socket is connected; FALSE otherwise
    BOOL m_fConnected;

    // Destination host, addr len and addr ( in case of connected sockets )
    char m_szDestHost[ NI_MAXHOST ];
    DWORD m_cbDestAddr;
    sockaddr_storage m_DestAddr;

    int m_RemotePort;
    int m_LocalPort;

    HRESULT OpenSocket(  USHORT af );

    // Sets the various values in case of an error.
    void Error();
    void Reset();

public:

    static int m_SocketCount ;
    static int m_SocketConnectCount ;

    static const char* GetLocalHostName();
    static HRESULT GetLocalAddress(
            OUT SOCKADDR_STORAGE *pSockAddr,
            OUT DWORD            *pcbSockAddr,
            IN  BOOL              fIPv6 = FALSE );

    static BOOL s_fInitAttempted;
    static BOOL s_fInitialized;
};
