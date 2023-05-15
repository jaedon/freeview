/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include <nrdbase/Log.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Version.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdbase/StdInt.h>
#include <sys/socket.h>
#include <string>
#include <vector>

DECLARE_TRACEAREA(WEBSOCKET);

#if defined(BUILD_DEBUG)
#define WSTRACE(...) NTRACE(TRACE_WEBSOCKET, __VA_ARGS__)
#else
#define WSTRACE(...)
#endif

#if 0
#define WSVERBOSE(...) NTRACE(TRACE_WEBSOCKET, __VA_ARGS__)
#else
#define WSVERBOSE(...)
#endif

#define WSINFO(...) Log::info(TRACE_WEBSOCKET, __VA_ARGS__)
#define WSWARN(...) Log::warn(TRACE_WEBSOCKET, __VA_ARGS__)
#define WSERROR(...) Log::error(TRACE_WEBSOCKET, __VA_ARGS__)

namespace netflix {
class IWebSocketHandler;

namespace websocket {

typedef int socket;

enum MessageType
{
    TextMessage,
    BinaryMessage
};

enum ResultType
{
    Continue            =  1,
    Success             =  0,
    NoMemory            = -1,
    InvalidCall         = -2,
    InvalidParameter    = -3,
    NetworkError        = -4,
    ProtocolError       = -5,
    InvalidData         = -6,
    NoHandler           = -7
};

struct ConnectionInfo
{
    std::string                 mUrl;
    std::string                 mHost;
    std::string                 mOrigin;
    std::string                 mClient;
    std::string                 mRequestKey;
    std::vector<std::string>    mSubprotocols;
    socket                      mSocket;
};

struct ConnectionConfig
{
    ullong    mRecvBufferSize;
    ullong    mRecvLimit;
    ullong    mSendLimit;
};

class IConnection
{
public:
    IConnection(socket const sock,
                const std::string &url,
                const std::string &host,
                const std::string &client,
                const std::string &subprotocol,
                IWebSocketHandler *pHandler);
    virtual ~IConnection();

    // This is the interface by which messages are sent.
    virtual ResultType  sendTextMessage(const void *pData, ullong length) = 0;
    virtual ResultType  sendBinaryMessage(const void *pData, ullong length) = 0;
    virtual ResultType  sendMessage(MessageType type, const void *pData, ullong length) = 0;
    virtual ResultType  beginMessage(MessageType type, const void *pData, ullong length) = 0;
    virtual ResultType  continueMessage(const void *pData, ullong length) = 0;
    virtual ResultType  endMessage(const void *pData, ullong length) = 0;

    // Called when the connection is to process input
    virtual ResultType  process() = 0;

    // Status
    virtual bool        isOpen() const = 0;
    virtual bool        isClosed() const = 0;
    virtual ResultType  closeConnection() = 0;

    // Information
    std::string         getHost() const;
    std::string         getClient() const;
    std::string         getUrl() const;
    std::string         getSubprotocol() const;
    socket              getSocket() const { return mSocket; }
    bool                closeSocket();

protected:

    socket              mSocket;
    std::string         mHost;
    std::string         mClient;
    std::string         mUrl;
    std::string         mSubprotocol;
    IWebSocketHandler * mpHandler;
};

/** Return the necessary HTTP headers to perform a WebSocket handshake with a server
 *  @param key [In] 16 random bytes to use as the key
 *  @param subprotocols [In] subprotocols sought, comma seperated
 *  @param headers [Out] HTTP headers to use in the handshake request
 */
ResultType buildClientHandshake(const std::vector<uint8_t> &key,
                                const std::string &subprotocols,
                                HttpHeaders &headers);

/** Validate the HTTP headers and perform the WebSocket handshake
 *  @param socket [In] The socket on which to perform a handshake and communicate
 *  @param url [In] The url on which the handshake was requested
 *  @param subprotocol [In] subprotocol specified in the handshake
 *  @param headers [In] HTTP headers of the response
 *  @param config [In] Configuration for the connection
 *  @param pHandler [In] handler for this connection
 */
ResultType handshakeResponse(socket sock,
                             const std::string &url,
                             const std::string &subprotocols,
                             const std::vector<uint8_t> &key,
                             const HttpResponseHeaders &headers,
                             const std::string &localHost,
                             const std::string &remoteHost,
                             const ConnectionConfig &config,
                             IWebSocketHandler *pHandler);

/** Validate the HTTP headers and perform the WebSocket handshake
 *  @param sock [In] The socket on which the handshake was recvd
 *  @param url [In] The url on which the handshake was requested
 *  @param httpHeaders [In] HTTP header of request
 *  @param client [In] The address of the remote client connecting
 *  @oaram connectionInfo [Out] Information about the connection
 *  @return Success on success
 *          ProtocolError on invalid HTTP headers/values
 */
ResultType handshake(socket sock,
                     const std::string &url,
                     const HttpHeaders &requestHeaders,
                     const std::string &client,
                     websocket::ConnectionInfo &connectionInfo);

/** Initialize a ConnectionConfig with default values
 * @param config [Out] The configuration to initialize
 */
void defaultConfig(websocket::ConnectionConfig &config);

/** Accept a WebSocket connection (server)
 *  @param connectionInfo [In] The connection information acquired from handshake()
 *  @param subprotocol [In] Which subprotocol is being accepted
 *  @param config [In] Configuration for the connection
 *  @param pHandler [Out] The handler for this connection
 *  @return Success on success
 *          NoMemory on out of memory
 *          NetworkError
*/
ResultType acceptConnection(const websocket::ConnectionInfo &connectionInfo,
                            const std::string &subprotocol,
                            const websocket::ConnectionConfig &config,
                            IWebSocketHandler *pHandler);

/** Reject a WebSocket connection (server)
 *  @param connectionInfo [In] The connectionifnormation acqured from handshake()
 *  @return Success on success
 *          NetworkError
 */
ResultType rejectConnection(const websocket::ConnectionInfo &connectionInfo);

}

class IWebSocketHandler
{
public:

    IWebSocketHandler();
    virtual ~IWebSocketHandler();

    // These methods to be implemented by derived
    // classes for responding to messages and events
    virtual void onOpen(const std::string &url, const std::string &protocol) = 0;
    virtual void onMessageBegin(websocket::MessageType type) = 0;
    virtual void onData(const void *pData, ullong length) = 0;
    virtual void onMessageEnd() = 0;
    virtual void onError() = 0;
    virtual void onClose(int code, const std::string &reason) = 0;

    // These methods to be implemented by derived classes
    // for signalling that there is data pending to be sent
    virtual bool pendingSend() const = 0;
    // and as a signal to send.
    virtual void onSend() = 0;

    shared_ptr<websocket::IConnection> mpConnection;
};

} // namespace netflix::net

#endif // __WEBSOCKET_H__
