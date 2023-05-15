/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASYNC_HTTP_SOCKET_CONNECTION_H
#define ASYNC_HTTP_SOCKET_CONNECTION_H

#include <list>
#include <sys/select.h>

#include <nrdnet/NrdIpAddr.h>
#include <nrdnet/IpConnectivityManager.h>
#include <nrdbase/CircularBuffer.h>

#include <nrdnet/AseTimeVal.h>
#include <nrdnet/IAsyncHttpLib.h>
#include "AsyncHttpSocketRequest.h"

namespace netflix {

#define SENDING_STAGING_BUFFER_SIZE 2048
#define RECEIVING_STAGING_BUFFER_SIZE 1024

class AsyncHttpSocketClient;

/* @class
 * @brief
 */
class AsyncHttpSocketConnection : public IAsyncHttpConnection
{
public:
    typedef shared_ptr<AsyncHttpSocketConnection> AsyncHttpSocketConnectionPtr;

public:
    /** Constructor.
     *
     *  @param pAsyncHttpSocketClient [In] The parent HTTP client, note this is not a shared
     *                                     pointer because we need to break cyclic dependency.
     *  @param id [In] The ID of the connection.
     *  @param pTraceListener [In] The connection trace listener.
     */
    AsyncHttpSocketConnection(
                    AsyncHttpSocketClient* pAsyncHttpSocketClient,
                    uint32_t id,
                    const std::string& destinationHostName,
                    IHttpConnectionTraceListener* pTraceListener );

    /** Destructor. */
    virtual ~AsyncHttpSocketConnection();

    /** Set the IP connectivity policy.
     *
     *  @param ipConnectivityPolicy [In] The IP connectivity policy.
     */
    virtual void setIpConnectivityPolicy(
                        IpConnectivityManager::Policy ipConnectivityPolicy );

    /** Notify the DNS lookup result.
     *
     *  @param timeNow [In] The current time.
     *  @param statusCode [In] The DNS lookup result status.
     *  @param destinationIpAddress [In] The destination IP address if the lookup is successful.
     *
     *  @return AS_NO_ERROR if all DNS lookup results are known. The client started connecting.
     *          AS_IN_PROGRESS when more DNS lookup results are expected.
     */
    virtual int32_t notifyDnsLookupResult(
                        const AseTimeVal& timeNow,
                        int32_t statusCode,
                        const NrdIpAddr& destinationIpAddr,
                        const std::string &cname);

    /** Notify the DNS lookup result.
     *
     *  @param timeNow [In] The current time.
     *  @param statusCode [In] The DNS lookup result status.
     *  @param destinationIpAddressList [In] The destination IP address list if the lookup is successful.
     *
     *  @return AS_NO_ERROR if all DNS lookup results are known. The client started connecting.
     *          AS_IN_PROGRESS when more DNS lookup results are expected.
     */
    virtual int32_t notifyDnsLookupResult(
                        const AseTimeVal& timeNow,
                        int32_t statusCode,
                        const std::vector<NrdIpAddr>& destinationIpAddrList,
                        const std::string &cname);

    /** Set the destination port number.
     *
     *  @param destinationPortNumber [In] The destination port number, in network byte order.
     */
    virtual void setDestinationPortNumber( uint16_t destinationPortNumber );

    /** Get the destination port number.
     *
     *  @return The destination port number, in network byte order.
     */
    virtual uint16_t getDestinationPortNumber() const;

    /** Set the secondary TCP connection delay.
     *
     *  @param delay [In] The delay.
     */
    virtual void setSecondaryTcpConnectionDelay( const AseTimeVal& delay );

    /** Set the fast TCP connection timeout delay.
     *
     *  @param delay [In] The delay.
     */
    virtual void setFastTcpTimeoutDelay( const AseTimeVal& delay );

    /** Set the fast TCP connection timeout max count.
     *
     *  @param delay [In] The delay.
     */
    virtual void setFastTcpTimeoutMaxCount( uint32_t maxCount );

    /** Set the HTTP connection timeout value.
     *
     *  @param timeout [In] The timeout.
     */
    virtual void setHttpConnectionTimeout( const AseTimeVal& timeout );

    /** Set the HTTP stall timeout value.
     *
     *  @param timeout [In] The timeout.
     */
    virtual void setHttpStallTimeout( const AseTimeVal& timeout );

    /** Set the socket receive buffer size.
     *
     *  @param bufferSize [In] The socket receive buffer size, in byte.
     */
    virtual void setSocketReceiveBufferSize( uint32_t bufferSize );

    /** Set the socket reattempt timeouts
     *
     * @param initial [in] Initial delay after all IP addresses have been tried before retrying
     * @param max [in] Max delay after exponential backoff
     */
    void setSocketReattemptDelays( AseTimeVal const& initial, AseTimeVal const& max );

    /** Open the TCP connection. This function opens the socket and tries to connect to the
     *  destination.
     *
     *  @param timeNow [In] The current time.
     *
     *  @return AS_NO_ERROR on success, the connection is established or in progress.
     *          Other error codes if the connection is failed.
     */
    virtual int32_t open( const AseTimeVal& timeNow );

    /** Close the TCP connection.
     *
     *  @param reason [in] The close reason
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error.
     */
    virtual int32_t close( int32_t reason = AS_CONNECTION_CLOSED );

    /** Get the socket handle.
     *
     *  @return The socket handle.
     */
    virtual int getSocketHandle() const;


    void setOutstandingEvent(Event event);

    /** Check if there is any outstanding event. */
    bool hasOutstandingEvent();

    /** Signal the connection failure. */
    void signalConnectionFailure(
                    int32_t connectionFailureCode,
                    int32_t lowLevelFailureCode );

    /** Signal that the response body is completed. */
    void signalResponseBodyCompleted( AsyncHttpSocketRequest* pRequest );

    /** Reset the receiving buffer. */
    void resetReceivingBuffer();

    /** Send out some requests.
     *
     *  @param timeNow [In] The current time.
     *
     *  @return AS_NO_ERROR
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_IN_PROGRESS sending pending
     *          AS_CONNECTION_ERROR connection error
     */
    virtual int32_t sendRequests( const AseTimeVal& timeNow );

    /** Prepare the data for calling select(). This function is called before select()
     *  to fill up the FD sets and when the connection is interested in a timeout signal.
     *
     *  @param timeNow [In] The current time.
     *  @param selectingFd [Out] The FD to select.
     *  @param pReadFdSet [Out] The read FD set.
     *  @param pWriteFdSet [Out] The write FD set.
     *  @param timeout [Out] The time the connection is interested in a timeout signal.
     */
    virtual void prepareSelect(
                        const AseTimeVal& timeNow,
                        int& selectingFd,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet,
                        AseTimeVal& timeout );

    /** Process the select events. This function is called to process events returned
     *  from select().
     *
     *  @param timeNow [In] The current time.
     *  @param pReadFdSet [In] The read FD set.
     *  @param pWriteFdSet [In] The write FD set.
     */
    virtual void processSelect(
                        const AseTimeVal& timeNow,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet );

    /** Receive data from the socket.
     *
     *  @param pBuffer [In] The buffer to receive data.
     *  @param bufferLength ]In] The length of the buffer.
     *  @param flags [In] The flags.
     *
     *  @return The OS recv() return values.
     */
    virtual int32_t receive(
                        unsigned char* pBuffer,
                        uint32_t bufferLength,
                        int32_t flags );

    /** IAsyncHttpConnection functions */

    /** Get the TCP ID. */
    virtual uint32_t getId() const;

    /** Get the HTTP ID. */
    virtual uint32_t getHttpId() const;

    /** Retrieve the outstanding event. */
    virtual Event retrieveOutstandingEvent();

    /** Get the current state */
    virtual State getState() const;

    /** Get the current state and how long the connection has been in the state. */
    virtual State getState( AseTimeVal& duration ) const;

    /** Return true if the connection ever reached IDLE_STATE */
    virtual bool didOpen() const;

    /** Get the client IP address, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
    virtual NrdIpAddr getClientIpAddress() const;

    /** Get the client port number, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
    virtual uint16_t getClientPortNumber() const;

    /** Get the server name. */
    virtual const std::string& getServerName() const;

    /** Get the resovled server IP address, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
    virtual NrdIpAddr getServerIpAddress() const;

    virtual std::string getCName() const;

    /** Get the secondary TCP connection delay. */
    virtual const AseTimeVal& getSecondaryTcpConnectionDelay() const;

    /** Get the fast TCP connection timeout delay. */
    virtual const AseTimeVal& getFastTcpTimeoutDelay() const;

    /** Get the fast TCP connection timeout max count. */
    virtual uint32_t getFastTcpTimeoutMaxCount() const;

    /** Get the HTTP connection timeout value. */
    virtual const AseTimeVal& getHttpConnectionTimeout() const;

    /** Get the HTTP stall timeout value. */
    virtual const AseTimeVal& getHttpStallTimeout() const;

    /** Get the socket receive buffer size.
     *
     *  @return The socket receive buffer size.
     */
    virtual uint32_t getSocketReceiveBufferSize() const;

    /** Get the actual socket receive buffer size.
     *
     *  @return The actual socket receive buffer size.
     */
    virtual uint32_t getActualSocketReceiveBufferSize() const;

    /** Get the time the client starts the connection. */
    virtual const AseTimeVal& getStartTime() const;

    /** Get the time the client attempts to connect to the server. */
    virtual const AseTimeVal& getConnectingTime() const;

    /** Get the time the client finishes connecting to the server. */
    virtual const AseTimeVal& getConnectedTime() const;

    /** Get the time the client failes to connect to the server. */
    virtual const AseTimeVal& getFailedTime() const;

    /** Set the user data which is some opaque data which the caller wants
     *  to associate with the connection. */
    virtual void setUserData( void* pData );

    /** Get the user data. */
    virtual void* getUserData() const;

    /** Set the auxiliary user data. */
    virtual void setAuxUserData( void* pData );

    /** Get the axuiliary user data. */
    virtual void* getAuxUserData() const;

    /** Get the connection failure code */
    virtual int32_t getConnectionFailureCode() const;

    /** Get the low level failure code */
    virtual int32_t getLowLevelFailureCode() const;

    /** Do not recv any more than the bare minimum of the HTTP response **/
    void setMinimumRecvMode(bool enable);

    /** Issue an HTTP get request.
     *
     *  @param url [In] The request URL.
     *  @param headers [In] The request headers in name/value pairs.
     *  @param pTraceListener [In] The HTTP request trace listener.
     *  @param pAsyncHttpRequest [Out] An IAsyncHttpRequest pointer.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          or other negative error codes.
     */
    virtual int32_t issueGetRequest(
                        AseUrl const& url,
                        HttpHeaders const& headers,
                        IHttpRequestTraceListenerPtr pTraceListener,
                        IAsyncHttpRequestPtr& pAsyncHttpRequest );

    /** Issue an HTTP request.
     *
     *  The caller is responsible for making sure that the request body
     *  data is around until the request is finsiehd (succeed or failed).
     *
     *  @param method [In] The request method.
     *  @param version [In] The request version.
     *  @param url [In] The request URL.
     *  @param headers [In] The request headers in name/value pairs.
     *  @param pBodyData [In] The request body data.
     *  @param bodyLength [In] The request body length.
     *  @param timeoutDuration [In] The timeout duration.
     *  @param pTraceListener [In] The HTTP request trace listener.
     *  @param pAsyncHttpRequest [Out] An IAsyncHttpRequest pointer.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          or other negative error codes.
     */
    virtual int32_t issueRequest(
                        IAsyncHttpRequest::Method method,
                        IAsyncHttpRequest::Version version,
                        AseUrl const& url,
                        HttpHeaders const& headers,
                        unsigned char* pBodyData,
                        uint32_t bodyLength,
                        const AseTimeVal& timeoutDuration,
                        IHttpRequestTraceListenerPtr pTraceListener,
                        IAsyncHttpRequestPtr& pAsyncHttpRequest );

    /** Cancel an HTTP request.
     *
     *  @param pAsyncHttpRequest [In] The IAsyncHttpRequest pointer.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_NOT_FOUND The request is not found
     *          AS_IN_PROGRESS The reqeust is in progresss, cannot be cancelled.
     *          or other negative error codes.
     */
    virtual int32_t cancelRequest(
                        IAsyncHttpRequestPtr const& pAsyncHttpRequest );

    /** Get all issued requests.
     *
     *  @return a list of issed requests. */
    virtual std::list<IAsyncHttpRequestPtr> getAllIssuedRequests() const;

    /** Get the triggering request.
     *
     *  This function is normally called after a RESPONSE_AVAILABLE_EVENT is observed on an
     *  HTTP connection to find out the triggering HTTP request. Various HTTP
     *  request functions can then be called depending on the state of the HTTP request.
     *  For example, if the state of the HTTP request is RECEIVING_BODY_STATE,
     *  IAsyncHttpRequest::getResponseBody() can be called to read the HTTP response
     *  body.
     *
     *  @return The HTTP request that triggers the RESPONSE_AVAILABLE_EVENT.
     */
    virtual IAsyncHttpRequestPtr getTriggeringRequest();

    /** Pause the receiving on this connection.
     *
     *  @return AS_NO_ERROR if succeed, otherwise error codes.
     */
    virtual int32_t pauseReceiving();

    /** Resume the receiving on this connection.
     *
     *  @return AS_NO_ERROR if succeed, otherwise error codes.
     */
    virtual int32_t resumeReceiving();

    /** Transfer the socket handle to the caller. After this function is called,
     *  the caller is responsible for closing the socket. The caller still needs
     *  to free the connection object by calling IAsyncHttpClient::closeConnection().
     */
    virtual int32_t transferSocketHandle();

    /** reset AsyncHttpSocketClient* **/
    void setAsyncHttpSocketClient(AsyncHttpSocketClient* pAsyncHttpSocketClient);

    /** reset IHttpConnectionTraceListener* **/
    void setConnectionTraceListener(IHttpConnectionTraceListener* pTraceListener);

    /* set function for connection reuse. If true, do not throw away socket,
     * and reuse already connected socket for same server and port*/
    void setConnectionReuse(bool enable)
    {
        mConnectionReuse = enable;
    }

    void setMaxContentLength(llong maxContentLength)
    {
        mMaxContentLength = maxContentLength;
    }

    llong maxContentLength() const
    {
        return mMaxContentLength;
    }

 protected:

    class SocketAttempt;
    typedef shared_ptr<SocketAttempt> SocketAttemptPtr;

    /** Open a socket using the next address for a protocol
     *  @param protocol [in] The IP protocol version
     *  @param fastTimeoutDelay [in] The delay before re-attempting the connection
     *  @param lowLevelFailureCode [out] The low level failure code for the last attempt
     *                                   when there are not addresses remaining to try
     */
    int32_t openNextSocket( NrdIpAddr::Version protocol,
                            uint32_t sequence,
                            AseTimeVal const& fastTimeoutDelay );

    /** Check the available addresses against the connectivity policy,
     *  adjusting the connectivity policy if necessary
     *
     *  @result true if there is a connectivity policy compatble with the available addresses
     */
    bool checkConnectivityPolicy();

    /** Perform tasks associated with a socket becomeing sucessfully connected */
    void socketConnected( SocketAttemptPtr pSocketAttempt );

    /** Clean up after asynchronous failure of socket open */
    void socketFailed( SocketAttemptPtr pSocketAttempt );

    /** Test if we should reattempt immediately, or after delay */
    bool shouldWaitForNextSocket( NrdIpAddr::Version protocol );

    /** Parse response status line.
     *
     *  @param invalidResponse [Out] The flag indicates that the response in invalid.
     */
    virtual void parseResponseStatus( int32_t& responseStatus );

    /** Parse response headers.
     *
     *  @param invalidResponse [Out] The flag indicates that the response in invalid.
     */
    virtual void parseResponseHeaders( int32_t& responseStatus );

    /** Parse chunked response body - chunk size. */
    virtual void parseChunkSize( int32_t& responseStatus );

    /** Parse chunked response body - chunk data body. */
    virtual void parseChunkDataBody( int32_t& responseStatus );

    /** Parse chunked response body - chunk data end. */
    virtual void parseChunkDataEnd( int32_t& responseStatus );

    /** Parse chunked response body - chunk trailer. */
    virtual void parseChunkTrailer( int32_t& responseStatus );

    /** Parse chunked response body. */
    virtual void parseChunkedResponseBody( int32_t& responseStatus );

    /** Parse response body.
     *
     *  @param invalidResponse [Out] The flag indicates that the response in invalid.
     */
    virtual void parseResponseBody( int32_t& responseStatus );

    /** Parse some responses. */
    virtual void parseResponses();

    /** Receive some responses.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     */
    virtual int32_t receiveResponses();

    /** Transit to a new state */
    void transit( State state );

    /** Set up the next reqeust to receive response. */
    void setupNextReceivingRequest();

protected:
    /** The type definition for the request list, this is in the order that the
     *  requests where created.
     */
    typedef std::list<AsyncHttpSocketRequestPtr> RequestList;

    /** Class representing an attempt to open a socket
     *
     * We may have multiple TCP establishments in progress simultaneously. We will start a new
     * on after a 200ms timeout, or whenever an existing one fails. We'll cycle through protocols and
     * through the IP addresses provided for each protocol.
     */

    class SocketAttempt
    {
        public:
            enum State
            {
                INIT = 0,
                CONNECTING = 1,
                CONNECTED = 2,
                CLOSED = 3,
                FAILED = 4
            };

            SocketAttempt( AsyncHttpSocketConnection* pAsyncHttpSocketConnection );

            virtual ~SocketAttempt();

            /** Attempt to open the connection */
            int32_t open(   uint32_t            id,
                            uint32_t            httpId,
                            NrdIpAddr const&    ipAddress,
                            uint16_t            port,
                            uint32_t            sequence,
                            AseTimeVal const&   fastTimeoutDelay,
                            uint32_t            socketReceiveBufferSize,
                            AseTimeVal const&   dnsLookupTime );

            /** Close the connection */
            int32_t close( int32_t reason = AS_CONNECTION_CLOSED );

            /** Process this connection after select */
            int32_t processSelect( AseTimeVal const& timeNow );

            /** Clear the fast timeout */
            void clearFastTimeout();

            /** Transfer the socket handle ( state == CONNECTTED ) */
            int transferSocketHandle();

            /** Return the current state */
            State getState() const;

            /** Test socket is connecting */
            bool connecting() const;

            /** Test if the socket is connected */
            bool connected() const;

            /** Test if the socket if failed */
            bool failed() const;

            /** Get the TCP connection id */
            uint32_t getId() const;

            /** Get the destination IP address ( state >= CONNECTING ) */
            NrdIpAddr const& getDestinationIpAddress() const;

            /** Get the destination port number ( state >= CONNECTING ) */
            uint16_t getDestinationPortNumber() const;

            /** Get the sequence number */
            uint32_t getSequence() const;

            /** Get the protocol version */
            NrdIpAddr::Version getProtocolVersion() const;

            /** Returns the socket handle ( state >= CONNECTING )*/
            int getOsSocketHandle() const;

            /** Returns the fast timeout delay ( state >= CONNECTING ) */
            AseTimeVal const& getFastTimeoutDelay() const;

            /** Returns the fast timeout time (absolute) ( state >= CONNECTING ) */
            AseTimeVal getFastTimeout() const;

            /** Retrn the actual socket receive buffer size ( state >= CONNECTING ) */
            uint32_t getSocketReceiveBufferSize() const;

            /** Returns the tcp connection start time ( state >= CONNECTING ) */
            AseTimeVal const& getTcpConnectingTime() const;

            /** Returns the source ip address ( state >= CONNECTING ) */
            NrdIpAddr const& getSourceIpAddress() const;

            /** Returns the source port ( state >= CONNECTING ) */
            uint16_t getSourcePort() const;

            /** Returns the tcp connection time ( state == CONNECTED ) */
            AseTimeVal const& getTcpConnectedTime() const;

            /** Returns the tcp failure time ( state == FAILED ) */
            AseTimeVal const& getTcpConnectFailureTime() const;

            /** Returns the connection failure code ( state == CONNECTED ) */
            int32_t getFailureCode() const;

            /** Returns the low level connection failure code */
            int32_t getLowLevelFailureCode() const;

        protected:
            /** Open the socket
             *
             *  If the open fails, this function should return the failure code and populate
             *  mLowLevelFailureCode. If the open succeeds immediately return AS_NO_ERROR,
             *  if it is in progress, return AS_IN_PROGRESS
             */
            virtual int32_t openSocket() = 0;

            /** Connect the socket */
            virtual int32_t connectSocket() = 0;

            /** Get the source address in mSourceIpAddress and mSourcePortNumber */
            virtual void obtainSourceAddress() = 0;

        protected:
            AsyncHttpSocketConnection*          mAsyncHttpSocketConnectionPtr;
            State                               mState;
            uint32_t                            mId;
            uint32_t                            mHttpId;
            NrdIpAddr                           mDestinationIpAddress;
            uint16_t                            mDestinationPort;
            uint32_t                            mSequence;
            AseTimeVal                          mFastTimeoutDelay;
            AseTimeVal                          mDnsLookupTime;
            uint32_t                            mOsSocketReceiveBufferSize;
            int                                 mOsSocketHandle;
            NrdIpAddr                           mSourceIpAddress;
            uint16_t                            mSourcePortNumber;
            AseTimeVal                          mTcpConnectingTime;
            AseTimeVal                          mTcpConnectCompleteTime;
            int32_t                             mConnectionFailureCode;
            int32_t                             mLowLevelFailureCode;
    };

    class SocketAttemptIpv4 : public SocketAttempt
    {
        public:
            SocketAttemptIpv4( AsyncHttpSocketConnection* pAsyncHttpSocketConnection );

            virtual ~SocketAttemptIpv4() {}

        protected:
            virtual int32_t openSocket();

            virtual int32_t connectSocket();

            virtual void obtainSourceAddress();
    };

#ifdef NRDP_HAS_IPV6
    class SocketAttemptIpv6 : public SocketAttempt
    {
        public:
            SocketAttemptIpv6(  AsyncHttpSocketConnection* pAsyncHttpSocketConnection );

            virtual ~SocketAttemptIpv6() {}

        protected:
            virtual int32_t openSocket();

            virtual int32_t connectSocket();

            virtual void obtainSourceAddress();

    };
#endif

protected:
    /** The parent AsyncHttpSocketClient pointer */
    AsyncHttpSocketClient* mAsyncHttpSocketClientPtr;

    /** The ID of the final TCP connection. */
    uint32_t mId;

    /** The ID of this HTTP connection. */
    uint32_t mHttpId;

    /** The IP connectivity policy. */
    IpConnectivityManager::Policy mIpConnectivityPolicy;

    /** The actual source IP address. */
    NrdIpAddr mActualSourceIpAddress;

    /** The actual source port number. */
    uint16_t mActualSourcePortNumber;

    /** The destination host name. */
    std::string mDestinationHostName;

    /** The actual destination IP address used. */
    NrdIpAddr mActualDestinationIpAddress;

    std::string mCName;

    /** The destination port number. */
    uint16_t mDestinationPortNumber;

    /** The secondary TCP connection delay. */
    AseTimeVal mSecondaryTcpConnectionDelay;

    /** The TCP fast timeout delay */
    AseTimeVal mFastTimeoutDelay;

    /** The TCP fast timeout maximum socket count */
    uint32_t mFastTimeoutMaxCount;

    /** The HTTP connection timeout. */
    AseTimeVal mHttpConnectionTimeout;

    /** The HTTP stall timeout. */
    AseTimeVal mHttpStallTimeout;

    /** The socket receive buffer size. */
    uint32_t mSocketReceiveBufferSize;

    /** The actual socket receive buffer size. */
    uint32_t mActualSocketReceiveBufferSize;

    /** The outstanding event. */
    Event mOutstandingEvent;

    /** The previous state. */
    State mPreviousState;

    /** The current state. */
    State mState;

    /** The time when the connection enters the current state. */
    AseTimeVal mStateEnterTime;

    /** The time the connection is started. */
    AseTimeVal mStartTime;

    /** The time the connection is attempted. */
    AseTimeVal mConnectingTime;

    /** The time the connection is opened. */
    AseTimeVal mConnectedTime;

    /** The time the connectioin is failed. */
    AseTimeVal mFailedTime;

    /** The time at which the socket buffer was last knwon to be empty */
    AseTimeVal mSocketReceiveEmptyTime;

    /** The user data. */
    void* mUserDataPtr;

    /** The auxiliary user data. */
    void* mAuxUserDataPtr;

    /** The connection failure code. */
    int32_t mConnectionFailureCode;

    /** The low level failure code. */
    int32_t mLowLevelFailureCode;

    /** The actual socket handle. */
    int mSocketHandle;

    /** The request list. */
    RequestList mRequestList;

    /** The iterator to the request that is being sent out. */
    RequestList::iterator mSendingRequestItr;

    /** The iterator to the request that is being received. */
    RequestList::iterator mReceivingRequestItr;

    /** Staging buffers are to store the data to be sent to the server and
     *  the data just received from the server.
     */

    /** The staging buffer for sending. This buffer stores the request to be sent to
     *  the server.
     */
    unsigned char mSendingBuffer[SENDING_STAGING_BUFFER_SIZE];

    /** The next byte to be sent in the sending buffer. */
    unsigned char* mSendingPtr;

    /** The length of data to be sent. */
    uint32_t mSendingDataLength;

    /** The staging buffer for receiving. This buffer stores the response just
     *  received from the server.
     */
    unsigned char mReceivingBuffer[RECEIVING_STAGING_BUFFER_SIZE];

    /** The next position in the receiving buffer to be processed. */
    unsigned char* mProcessingPtr;

    /** The length of the data received. */
    uint32_t mReceivedDataLength;

    /** The working variables for parsing responses. */
    uint32_t mItemIndex;

    unsigned char* mItemPtr;

    /** The variables to store parsed status line items. */
    std::string mStatusHttpVersion;
    std::string mStatusCode;
    std::string mStatusReasonPhrase;

    /** The current chunk size. */
    uint32_t mCurrentChunkSize;

    /** Does the request have non-empty chunks ? */
    bool mHasNonEmptyChunks;

    /** Multi-purpose timers
     *   State       Purpose
     *   ===================
     *   Connecting  connection timeout.
     *   Active      stall timeout.
     */
    AseTimeVal mTimerOne;

    /** The request ID pool. */
    uint32_t mRequestIdPool;

    /** The flag indicates the receiving is paused. */
    bool mReceivingPausedFlag;

    /** The HTTP connection trace listener. */
    IHttpConnectionTraceListener* mTraceListenerPtr;

    /// The working variable for computing byte receiving statistics.
    uint32_t mIgnoredDataLength;

    /** The flag indicates that the next receive would be the first one after resume. */
    bool mFirstReceiveAfterResume;

    /** The time when the connection is paused. */
    AseTimeVal mPausedTime;

    /** The previous unread byte count in the socket receive buffer. */
    uint32_t mPreviousUnreadByteCount;

    /** The number of pending DNS lookups. */
    uint32_t mPendingDnsLookupCount;

    /** The frontmost socket attempt - keep this so that we can report overall timeout */
    SocketAttemptPtr mFrontSocketAttempt;

    //Variables for IP dual-stack connect.

#ifdef NRDP_HAS_IPV6

#define ASYNC_IP_VERSION_COUNT 2

#else

#define ASYNC_IP_VERSION_COUNT 1

#endif

    /** The destination address list. */
    std::vector<NrdIpAddr>              mDestinationIpAddressList[ASYNC_IP_VERSION_COUNT];
    std::vector<NrdIpAddr>::iterator    mDestinationIpAddressItr[ASYNC_IP_VERSION_COUNT];

    /** The current socket attempts */
    std::list<SocketAttemptPtr>         mSocketAttemptList;

    /** Reattempt times */
    AseTimeVal mSocketReattemptTime[ASYNC_IP_VERSION_COUNT];
    AseTimeVal mSocketReattemptDelay[ASYNC_IP_VERSION_COUNT];
    AseTimeVal mSocketReattemptMaxDelay;

    /** Reattempt sequences */
    uint32_t mSocketReattemptSequence[ASYNC_IP_VERSION_COUNT];

    /** Failure codes */
    int32_t mLastFailureCode[ASYNC_IP_VERSION_COUNT];
    int32_t mLastLowLevelFailureCode[ASYNC_IP_VERSION_COUNT];

#ifdef NRDP_HAS_IPV6

    /** The secondary TCP connect time. */
    AseTimeVal mSecondaryTcpConnectTime;

#endif

    /** Whether to restrict recv calls on the socket to exactly HTTP data and no more **/
    bool mMinimumRecvMode;

    /* if true, do not throw away socket, and reuse already connected socket for
     * same server and port*/
    bool mConnectionReuse;

    llong mMaxContentLength;
};

typedef AsyncHttpSocketConnection::AsyncHttpSocketConnectionPtr AsyncHttpSocketConnectionPtr;

inline void AsyncHttpSocketConnection::SocketAttempt::clearFastTimeout()
{
    mFastTimeoutDelay = AseTimeVal::INFINITE;
}

inline AsyncHttpSocketConnection::SocketAttempt::State
AsyncHttpSocketConnection::SocketAttempt::getState() const
{
    return mState;
}

inline bool AsyncHttpSocketConnection::SocketAttempt::connecting() const
{
    return mState == CONNECTING;
}

inline bool AsyncHttpSocketConnection::SocketAttempt::connected() const
{
    return mState == CONNECTED;
}

inline bool AsyncHttpSocketConnection::SocketAttempt::failed() const
{
    return mState == FAILED;
}

inline uint32_t AsyncHttpSocketConnection::SocketAttempt::getId() const
{
    return mId;
}

inline NrdIpAddr const& AsyncHttpSocketConnection::SocketAttempt::getDestinationIpAddress() const
{
    return mDestinationIpAddress;
}

inline uint16_t AsyncHttpSocketConnection::SocketAttempt::getDestinationPortNumber() const
{
    return mDestinationPort;
}

inline uint32_t AsyncHttpSocketConnection::SocketAttempt::getSequence() const
{
    return mSequence;
}

inline NrdIpAddr::Version AsyncHttpSocketConnection::SocketAttempt::getProtocolVersion() const
{
    return mDestinationIpAddress.getVersion();
}

inline int AsyncHttpSocketConnection::SocketAttempt::getOsSocketHandle() const
{
    return mOsSocketHandle;
}

inline AseTimeVal const& AsyncHttpSocketConnection::SocketAttempt::getFastTimeoutDelay() const
{
    return mFastTimeoutDelay;
}

inline AseTimeVal AsyncHttpSocketConnection::SocketAttempt::getFastTimeout() const
{
    return mTcpConnectingTime + mFastTimeoutDelay;
}

inline uint32_t AsyncHttpSocketConnection::SocketAttempt::getSocketReceiveBufferSize() const
{
    return mOsSocketReceiveBufferSize;
}

inline AseTimeVal const& AsyncHttpSocketConnection::SocketAttempt::getTcpConnectingTime() const
{
    return mTcpConnectingTime;
}

inline NrdIpAddr const& AsyncHttpSocketConnection::SocketAttempt::getSourceIpAddress() const
{
    return mSourceIpAddress;
}

inline uint16_t AsyncHttpSocketConnection::SocketAttempt::getSourcePort() const
{
    return mSourcePortNumber;
}

inline AseTimeVal const& AsyncHttpSocketConnection::SocketAttempt::getTcpConnectedTime() const
{
    return mTcpConnectCompleteTime;
}

inline AseTimeVal const& AsyncHttpSocketConnection::SocketAttempt::getTcpConnectFailureTime() const
{
    return mTcpConnectCompleteTime;
}

inline int32_t AsyncHttpSocketConnection::SocketAttempt::getFailureCode() const
{
    return mConnectionFailureCode;
}

inline int32_t AsyncHttpSocketConnection::SocketAttempt::getLowLevelFailureCode() const
{
    return mLowLevelFailureCode;
}

}
#endif //ASYNC_HTTP_CONNECTION_SOCKET_H

