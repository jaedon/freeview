/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASYNC_HTTP_SOCKET_CLIENT_H
#define ASYNC_HTTP_SOCKET_CLIENT_H

#include <list>
#include <vector>
#include <set>
#include <sys/select.h>

#include <nrdbase/tr1.h>
#include <nrdnet/DnsManager.h>
#include <nrdnet/IpConnectivityManager.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/TrafficShaper.h>

#include "AsyncHttpTraceRoutePlugin.h"
#include "AsyncHttpSocketConnection.h"

namespace netflix {

class AsyncHttpSocketClient : public IAsyncHttpClient
                            , public IpConnectivityListener
                            , public enable_shared_from_this<AsyncHttpSocketClient>
{
public:
    typedef shared_ptr<AsyncHttpSocketClient> AsyncHttpSocketClientPtr;

public:
    /** Constructor. */
    AsyncHttpSocketClient();

    /** Destructor. */
    virtual ~AsyncHttpSocketClient();

    /** IAsyncHttpClient functions. */

    /** Initialize the client.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_SOCKET_ERROR socket error
     *          or other negative error codes.
     */
    virtual int32_t init();

    /** Deinitialize the client.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          or other negative error codes.
     */
    virtual int32_t deinit();

    virtual void setConnectionReuse(bool enable);

    /** Set the extra HTTP request headers.
     *
     *  @param httpHeaders [In] The additional HTTP headers.
     */
    virtual void setExtraRequestHeaders( const HttpHeaders& httpHeaders );

    /** Get the extra HTTP request headers.
     *
     *  @return The extra HTTP request headers.
     */
    virtual const HttpHeaders& getExtraRequestHeaders() const;

    /** Set the trace route plugin.
     */
    virtual void setTraceRoutePlugin( AsyncHttpTraceRoutePluginPtr pTraceRoutePlugin );

    /** Open an HTTP connection.
     *
     *  @param hostName [In] The HTTP server host name.
     *  @param portNumber [In] The HTTP server port number.
     *  @param httpConnectionTimeout [In] The HTTP connection timeout.
     *  @param httpStallTimeout [In] The HTTP stall timeout.
     *  @param socketReceiverBuffer [in] The socket receive buffer size or zero for default
     *  @param pTraceListener [In] The connection trace listener.
     *  @param pAsyncHttpConnection [Out] An IAsyncHttpConnection pointer.
     *
     *  @return 0 on success
     *          -1 unknown error
     *          or other negative error codes.
     */
    virtual int32_t openConnection(
                        std::string const& hostName,
                        uint16_t portNumber,
                        const AseTimeVal& httpConnectionTimeout,
                        const AseTimeVal& httpStallTimeout,
                        uint32_t socketReceiverBuffer,
                        IHttpConnectionTraceListener* pTraceListener,
                        bool minimumRecvMode,
                        IAsyncHttpConnectionPtr& pAsyncHttpConnection );

    /** Close an HTTP connection.
     *
     *  @param pAsyncHttpConnection [In] The IAsyncHttpConnection pointer.
     *  @param bAllowReuse [in] true if reuse of this connection is allowed
     *  @param reason [in] Connection close reason
     *
     *  @return AS_NO_ERROR on success.
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_NOT_FOUND The connection is not found.
     *          or other negative error codes.
     */
    virtual int32_t closeConnection(
                        IAsyncHttpConnectionPtr const& pAsyncHttpConnection,
                        bool bAllowReuse = true,
                        int32_t reason = AS_CONNECTION_CLOSED  );

    /** Get the connection failure code */
    virtual int32_t getConnectionFailureCode() const;

    /** Get the low level failure code */
    virtual int32_t getLowLevelFailureCode() const;

    /** Wake up the HTTP client from select().
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          other negative error codes.
     */
    virtual int32_t wake();

    /** Send a signal byte from the external to the async HTTP library. */
    virtual int32_t sendSignalByteExternal();

    /** Receive the signal byte from the internal of the async HTTP library. */
    virtual int32_t receiveSignalByteInternal();

    /** Set the external FD sets.
     *
     *  @param maxFd [In] The maximum FD number.
     *  @param pReadFdSet [In] The read FD set.
     *  @param pWriteFdSet [In] The write FD set.
     */
    virtual void setExternalFdSets(
                        int maxFd,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet );

    /** Wait for events on connections or wake up signal.
     *
     *  @param selectedConnections [Out] A list of connections on which events happen.
     *  @param timeOut [In] The timeout value, AseTimeVal::INFINITE for blocking indefinitely.
     *
     *  @return SELECT_WAKE_AND_CONNECT on both wake up signal and connection event.
     *          SELECT_CONNECT on connection events
     *          SELECT_WAKE on wake up signal
     *          AS_NO_ERROR on timeout
     *          AS_UNKNOWN_ERROR unknown error
     *          or negative error code.
     */
    virtual int32_t select(
                        std::vector<IAsyncHttpConnectionPtr>& selectedConnections,
                        const AseTimeVal& timeout );


    /** AsyncHttpSocketClient specific functions. */

    /** Set the IP connectivity manager. */
    virtual void setIpConnectivityManager( IpConnectivityManager* pIpConnectivityManager );

    /** Get the IP connectivity manager. */
    virtual IpConnectivityManager* getIpConnectivityManager();

    /** Set the global DNS cache. */
    virtual void setGlobalDnsCache( IDnsCache* pGlobalDnsCache );

    /** Set the DNS trace listener. */
    virtual void setDnsTraceListener( IDnsTraceListener* pDnsTraceListener );

    /** IpConnectivityManager functions */
    virtual void trafficShaperConfigChanged();

    /** Functions used by connection objects. */

    /** Peek an ID for a new connection. */
    uint32_t peekConnectionId();

    /** Allocate an ID for a new connection. */
    uint32_t allocateConnectionId();

    /** Trace route an IP address. */
    void traceRouteIpAddress( const NrdIpAddr& ipAddress );

    void setMaxContentLength(llong maxContentLength)
    {
        mMaxContentLength = maxContentLength;
    }

    llong maxContentLength() const
    {
        return mMaxContentLength;
    }

    void trafficShaperConsume( uint32_t bytes );

    bool trafficShaperWouldSleep();

protected:
    /** The type definition for the connection list. */
    typedef std::list<AsyncHttpSocketConnectionPtr> ConnectionList;

    /** The type definition for the plugin list. */
    typedef std::list<AsyncHttpBasePluginPtr> PluginList;

    /** The type definition for the DNS request list. */
    typedef std::list<DnsManager::RequestPtr> DnsRequestList;

protected:

#ifdef NRDP_HAS_IPV6
    /** Open IPv6/v4 connection. */
    int32_t openIpV6V4Connection(
                        const std::string& hostName,
                        AsyncHttpSocketConnectionPtr pHttpConnection );
#else
    /** Open IPv4 only connection. */
    int32_t openIpV4OnlyConnection(
                        const std::string& hostName,
                        AsyncHttpSocketConnectionPtr pHttpConnection );
#endif

    /** Reset the traffic shaper based on new configuration */
    void resetTrafficShaper();

protected:
    /** The extra HTTP request headers. */
    HttpHeaders mExtraRequestHeaders;

    /** The plugin list. */
    PluginList mPluginList;

    /** The trace route plugin. */
    AsyncHttpTraceRoutePluginPtr mTraceRoutePluginPtr;

    /** The traffic shaper */
    TrafficShaperPtr    mTrafficShaperPtr;

    /** Flag for change in traffic shaper configuration */
    bool    mTrafficShaperConfigChanged;

    /** The connection list. */
    ConnectionList mConnectionList;

    /** The DNS lookup client. */
    DnsManager::ClientPtr mClient;

    /** The DNS lookup requests. */
    DnsRequestList mRequests;

    /** The socket pair for inter-thread communication. This is the mechanism to
     *  wake up the select() call. socket_pair[0] faces outside, socket_pair[1] faces
     *  inside.
     */
    int mSocketPair[2];

    /** The maximum external FD number, -1 if no external FD sets exist. */
    int mMaxExternalFd;

    /** The external FD sets. */
    fd_set mExternalReadFdSet;
    fd_set mExternalWriteFdSet;

    /** The FD sets for the select() call. */
    fd_set mReadFdSet;
    fd_set mWriteFdSet;

    /** The connection failure code. */
    int32_t mConnectionFailureCode;

    /** The low level failure code. */
    int32_t mLowLevelFailureCode;

    /** The default IP connectivity manager. */
    IpConnectivityManager mDefaultIpConnectivityManager;

    /** The current IP connectivity manager. */
    IpConnectivityManager* mIpConnectivityManagerPtr;

    /** The global DNS cache. */
    IDnsCache* mGlobalDnsCachePtr;

    /** The DNS trace listener. */
    IDnsTraceListener* mDnsTraceListenerPtr;

    /** The connection ID pool. */
    uint32_t mConnectionIdPool;

    bool mConnectionReuse;

    llong mMaxContentLength;
};

typedef AsyncHttpSocketClient::AsyncHttpSocketClientPtr AsyncHttpSocketClientPtr;

inline void AsyncHttpSocketClient::trafficShaperConsume( uint32_t bytes )
{
    if ( mTrafficShaperPtr )
        mTrafficShaperPtr->addBytes( bytes );
}

inline bool AsyncHttpSocketClient::trafficShaperWouldSleep()
{
    return mTrafficShaperPtr ? mTrafficShaperPtr->wouldSleep() : false;
}



}
#endif //ASYNC_HTTP_LIB_SOCKET_H
