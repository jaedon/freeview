/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#include <socket.h>

#include <windows.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// ServerSocket
//
// ServerSocket is the server side of a socket connection.  Whenever a new
// connection is made, it returns a new Socket object to serve that
// connection.
/////////////////////////////////////////////////////////////////////////////
class CServerSocket
{
public:
    CServerSocket();
    virtual ~CServerSocket();

    static int m_ServerSocketCount;

    const WCHAR *GetLastError();
    void Close();

    ///////////////////////////////////////////////////////////////////////////
    // Manipulators
    ///////////////////////////////////////////////////////////////////////////

    // Setup this socket to accept connections on the given port.
    // Return false on a failure.
    HRESULT Serve( int maxClients, int SockType, int port = 0 );

    // Accept an incoming connection on this socket and return a pointer
    // to a new Socket for that connection.  On error, NULL is returned
    HRESULT Accept( CSocket **ppSocket );

    ///////////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////////

    // Return true if this socket is still serving connections.
    BOOL IsOpen()  const;

    // Return the socket descriptor that this socket uses.
    // This gives away part of the representation, but it's necessary
    SOCKET GetHandle() const
    {
        return m_hIPv4Socket;
    }

    int GetLocalPort() const;

private:
    // Socket descriptor for this server connection.
    SOCKET           m_hIPv4Socket;
    SOCKET           m_hIPv6Socket;
    int              m_LocalPort;
    WCHAR            m_wszLastError[100];
    int              m_SocketType;
    CRITICAL_SECTION m_csLock;
    BOOL             m_bSelected;
    fd_set           m_fdAccept;

};
