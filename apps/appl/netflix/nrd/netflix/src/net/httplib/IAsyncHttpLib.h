/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IASYNC_HTTP_LIB_H
#define IASYNC_HTTP_LIB_H

#include <map>
#include <list>
#include <vector>
#include <string>

#include <sys/select.h>

#include <nrdbase/tr1.h>
#include <nrdbase/StringUtils.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/IpConnectivityManager.h>
#include <nrdnet/IDnsCache.h>
#include <nrdnet/TraceRecords.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/Variant.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/IAsyncHttpListeners.h>
#include <nrdnet/HttpResponseHeaders.h>

namespace netflix {

/** The type definition for HTTP headers. */
struct StrCaseComparisonFunc
{
    bool operator()(const std::string &strOne, const std::string &strTwo) const
    {
        return strcasecmp(strOne.c_str(), strTwo.c_str()) < 0;
    }
};

typedef std::map<std::string, std::string, StrCaseComparisonFunc> HttpHeaders;

class TrafficShaper;
typedef shared_ptr<TrafficShaper> TrafficShaperPtr;

static inline Variant httpHeadersToVariant(const HttpHeaders &headers)
{
    Variant ret;
    for (HttpHeaders::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        ret[it->first] = it->second;
    }
    return ret;
}

/**
 * @class
 * @brief
 */
class IAsyncHttpRequest
{
public:
    typedef shared_ptr<IAsyncHttpRequest> IAsyncHttpRequestPtr;

    /**
     * @enum Method
     *
     * HTTP request method.
     */
    typedef enum
    {
        GET_METHOD = 0,     /**< HTTP GET. */
        HEAD_METHOD,        /**< HTTP HEAD. */
        POST_METHOD,        /**< HTTP POST. */
        DELETE_METHOD       /**< HTTP DELETE. */
    } Method;

    typedef enum
    {
        HTTP_1_0 = 0,           /**< HTTP 1.0. */
        HTTP_1_1                /**< HTTP 1.1. */
    } Version;

    /**
     * @enum State
     *
     * HTTP request states.
     */
    typedef enum
    {
        CREATED_STATE = 0,          /**< The request is created. */
        SENDING_LINE_HEADERS_STATE, /**< The request line and headers are being sent out. */
        SENDING_BODY_STATE,         /**< The request body is being sent out. */
        WAITING_STATE,              /**< The request has been sent out, waiting for response. */
        RECEIVING_STATUS_STATE,     /**< The response status line is being received. */
        RECEIVING_HEADERS_STATE,    /**< The response headers are being received. */
        RECEIVING_BODY_STATE,       /**< The response status line and headers have been received,
                                       the body, if any, is being received. */
        COMPLETED_STATE             /**< The complete response has been received. */
    } State;

public:
    /** Constructor. */
    IAsyncHttpRequest()
    {
    }

    /** Destructor. */
    virtual ~IAsyncHttpRequest()
    {
    }

    /** Get the ID. */
    virtual uint32_t getId() const = 0;

    /** Get the request URL. */
    virtual AseUrl const& getRequestUrl() const = 0;

    /** Get the method. */
    virtual Method getMethod() const = 0;

    /** Get the version. */
    virtual Version getVersion() const = 0;

    /** Get the state. */
    virtual State getState() const = 0;

    /** Get the progress of the request. The progress is defined as the percentage
     *  of the received body length over the requested data length. In the case
     *  where the request data length is unknown, the progress is also unknown.
     *
     *  @return 0 ~ 100
     *          -1 if unknown.
     */
    virtual int32_t getProgress() const = 0 ;

    /** Set the user data which is some opaque data which the caller wants to associate with the request. */
    virtual void setUserData( void* pData ) = 0;

    /** Get the user data. */
    virtual void* getUserData() const = 0;

    /** Get the response HTTP version, only valid when state >= RECEIVING_HEADERS_STATE.
     *
     *  @param major [Out] The HTTP major version.
     *  @param minor [Out] The HTTP minor version.
     */
    virtual void getResponseHttpVersion( uint32_t& major, uint32_t& minor ) = 0;

    /** Get the response code, only valid when state >= RECEIVING_HEADERS_STATE */
    virtual uint32_t getResponseCode() const = 0;

    /** Get the value of a particular response header, only valid when state >= RECEIVING_BODY_STATE */
    virtual std::string getResponseHeaderValue( std::string const& name ) const = 0;

    /** Get all the response headers, only valid when state >= RECEIVING_BODY_STATE */
    virtual HttpResponseHeaders getResponseHeaders() const = 0;

    /** Get the chunked transfer flag. */
    virtual bool getChunkedTransferFlag() const = 0;

    /** Get the content length. */
    virtual llong getContentLength() const = 0;

    /** Get the response body length. */
    virtual llong getResponseBodyLength() const = 0;

    /** Get the response body, only valid when state >= RECEIVING_BODY_STATE.
     *
     *  This function may not return the whole body. It is possible that this function returns 0 or 1,
     *  but the returned buffer length is 0. This may happen, for example, in the case of chunked transfer
     *  as shown below.
     *
     *  B
     *  First chunk      ----> getResponseBody() returns 0 with buffer length set to 11
     *  C
     *  Second chunk     ----> getResponseBody() returns 0 with buffer length set to 12
     *  0                ----> getResponseBody() returns 0 with buffer length set to 0
     *  MD5: 0xabcdefg   ----> getResponseBody() returns 1 with buffer length set to 0
     *
     *  If this function returns a connection failure error, the caller should close
     *  the connection and assume all the outstanding requests are lost.
     *
     *  @param pBuffer [In] The buffer to store the response body.
     *  @param bufferLength [In/Out] When input, it is the capacity of the buffer.
     *                               When outout, it is the size of response body read.
     *
     *  @return 1 on success and the complete body has been read
     *          AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_CONNECTION_ERROR connection failures
     *          or other negative error codes.
     */
    virtual int32_t getResponseBody( unsigned char* pBuffer, uint32_t& bufferLength ) = 0;

    /** As getResponseBody except that the body data is discarded
     *  @param bytesDiscarded [in/out] On entry, if limit = true, the max number of bytes to discard
     *                                 on exit, the number of bytes discarded
     *  @param limit [in] If true, at most bytesDiscarded bytes are discarded, otherwise all the
     *                    received body data so far will be discarded
     */
    virtual int32_t discardResponseBody( uint32_t& bytesDiscarded, bool const limit = false ) = 0;

    /** Get the time the client sends the first byte. */
    virtual const AseTimeVal& getFirstByteSentTime() const = 0;

    /** Get the time the client receives the first byte. */
    virtual const AseTimeVal& getFirstByteReceivedTime() const = 0;

    /** Get the transfer timeout time. */
    virtual const AseTimeVal& getTimeoutTime() const = 0;

public:

    /** Get the method as a string */
    std::string getMethodString() const	{ return requestMethodString(getMethod()); }

    /** Returns the given method as a string */
    static char const * requestMethodString(Method method)
    {
        switch (method) {
        case GET_METHOD: return "GET";
        case POST_METHOD: return "POST";
        case HEAD_METHOD: return "HEAD";
        case DELETE_METHOD: return "DELETE";
        }
        assert(0);
        return 0;
    }


};

typedef IAsyncHttpRequest::IAsyncHttpRequestPtr IAsyncHttpRequestPtr;

/* @class
 * @brief
 *
 *  Timeline of a connection.
 *                                                / Connected
 *                         / Connecting           |
 *                         |                      |
 *   / Start               |                      |
 *   |                     |                      |
 *   *---------------------*----------------------*
 *   |                     |                      |
 *   +-- DNS lookup time --+-- TCP connect time --+
 */
class IAsyncHttpConnection
{
public:
    typedef shared_ptr<IAsyncHttpConnection> IAsyncHttpConnectionPtr;

    /**
     * @enum Event
     *
     * HTTP connection events.
     */
    typedef enum
    {
        NO_EVENT = 0,               /**< No event. */
        STATE_CHANGE_EVENT,         /**< The connection state is changed. The connection goes from
                                       OPENING_STATE to IDLE_STATE, or goes to CLOSED_STATE or
                                       FAILED_STATE.*/
        RESPONSE_AVAILABLE_EVENT    /**< The response of some HTTP request is available. The response
                                       is either completed or the body is available for reading.*/
    } Event;

    /**
     * @enum State
     *
     * HTTP connection states.
     */
    typedef enum
    {
        LOOKING_UP_STATE = 0,   /**< The DNS lookup is in progress. */
        OPENING_STATE,          /**< The connection is being opened. */
        IDLE_STATE,             /**< The connection has been opened and there is no request in progress. */
        ACTIVE_STATE,           /**< The connection has been opened and there is at least one request in
                                   progress. */
        CLOSED_STATE,           /**< The connection has been closed. */
        FAILED_STATE            /**< The connection has failed. */
    } State;

public:
    /** Constructor. */
    IAsyncHttpConnection()
    {
    }

    /** Destructor. */
    virtual ~IAsyncHttpConnection()
    {
    }

    /** Get the TCP ID. */
    virtual uint32_t getId() const = 0;

    /** Get the HTTP ID. */
    virtual uint32_t getHttpId() const = 0;

    /** Retrieve the outstanding event. */
    virtual Event retrieveOutstandingEvent() = 0;

    /** Get the current state. */
    virtual State getState() const = 0;

    /** Get the current state and how long the connection has been in the state. */
    virtual State getState( AseTimeVal& duration ) const = 0;

    /** Return true if the connection ever reached IDLE_STATE */
    virtual bool didOpen() const = 0;

    /** Get the client IP address, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
    virtual NrdIpAddr getClientIpAddress() const = 0;

    /** Get the client port number, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
    virtual uint16_t getClientPortNumber() const = 0;

    /** Get the server name. */
    virtual const std::string& getServerName() const = 0;

    /** Get the resovled server IP address, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
    virtual NrdIpAddr getServerIpAddress() const = 0;

    virtual std::string getCName() const = 0;

    /** Get the secondary TCP connection delay. */
    virtual const AseTimeVal& getSecondaryTcpConnectionDelay() const = 0;

    /** Get the fast TCP connection timeout delay. */
    virtual const AseTimeVal& getFastTcpTimeoutDelay() const = 0;

    /** Get the HTTP connection timeout value. */
    virtual const AseTimeVal& getHttpConnectionTimeout() const = 0;

    /** Get the HTTP stall timeout value. */
    virtual const AseTimeVal& getHttpStallTimeout() const = 0;

    /** Get the socket receive buffer size. */
    virtual uint32_t getSocketReceiveBufferSize() const = 0;

    /** Get the actual socket receive buffer size. */
    virtual uint32_t getActualSocketReceiveBufferSize() const = 0;

    /** Get the time the client starts the connection. */
    virtual const AseTimeVal& getStartTime() const = 0;

    /** Get the time the client attempts to connect to the server. */
    virtual const AseTimeVal& getConnectingTime() const = 0;

    /** Get the time the client finishes connecting to the server. */
    virtual const AseTimeVal& getConnectedTime() const = 0;

    /** Get the time the client failes to connect to the server. */
    virtual const AseTimeVal& getFailedTime() const = 0;

    /** Set the user data which is some opaque data which the caller wants to associate
     *  with the connection. */
    virtual void setUserData( void* pData ) = 0;

    /** Get the user data. */
    virtual void* getUserData() const = 0;

    /** Set the auxiliary user data. */
    virtual void setAuxUserData( void* pData ) = 0;

    /** Get the axuiliary user data. */
    virtual void* getAuxUserData() const = 0;

    /** Get the connection failure code */
    virtual int32_t getConnectionFailureCode() const = 0;

    /** Get the low level failure code */
    virtual int32_t getLowLevelFailureCode() const = 0;

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
                        IAsyncHttpRequestPtr& pAsyncHttpRequest ) = 0;

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
                        IAsyncHttpRequestPtr& pAsyncHttpRequest ) = 0;

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
                        IAsyncHttpRequestPtr const& pAsyncHttpRequest ) = 0;

    /** Get all issued requests.
     *
     *  @return a list of issed requests. */
    virtual std::list<IAsyncHttpRequestPtr> getAllIssuedRequests() const = 0;

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
    virtual IAsyncHttpRequestPtr getTriggeringRequest() = 0;

    /** Pause the receiving on this connection.
     *
     *  @return AS_NO_ERROR if succeed, otherwise error codes.
     */
    virtual int32_t pauseReceiving() = 0;

    /** Resume the receiving on this connection.
     *
     *  @return AS_NO_ERROR if succeed, otherwise error codes.
     */
    virtual int32_t resumeReceiving() = 0;

    /** Transfer the socket handle to the caller. After this function is called,
     *  the caller is responsible for closing the socket. The caller still needs
     *  to free the connection object by calling IAsyncHttpClient::closeConnection().
     */
    virtual int32_t transferSocketHandle() = 0;
};

typedef IAsyncHttpConnection::IAsyncHttpConnectionPtr IAsyncHttpConnectionPtr;

/**
 * @class
 * @brief
 */
class IAsyncHttpClient
{
public:
    typedef shared_ptr<IAsyncHttpClient> IAsyncHttpClientPtr;

public:
    /** Constructor. */
    IAsyncHttpClient()
    {
    }

    /** Destructor. */
    virtual ~IAsyncHttpClient()
    {
    }

    /** Initialize the client.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_SOCKET_ERROR socket error
     *          AS_DNS_ERROR dns library error
     *          or other negative error codes.
     */
    virtual int32_t init() = 0;

    /** Deinitialize the client.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          or other negative error codes.
     */
    virtual int32_t deinit() = 0;

    /** Set the extra HTTP request headers.
     *
     *  @param httpHeaders [In] The additional HTTP headers.
     */
    virtual void setExtraRequestHeaders( const HttpHeaders& httpHeaders ) = 0;

    /** Get the extra HTTP request headers.
     *
     *  @return The extra HTTP request headers.
     */
    virtual const HttpHeaders& getExtraRequestHeaders() const = 0;

    /** Open an HTTP connection.
     *
     *  @param hostName [In] The HTTP server host name.
     *  @param portNumber [In] The HTTP server port number, in host order.
     *  @param httpConnectionTimeout [In] The HTTP connection timeout.
     *  @param httpStallTimeout [In] The HTTP stall timeout.
     *  @param socketReceiverBuffer [In] The socket receive buffer size or zero for default
     *  @param pTraceListener [In] The connection trace listener.
     *  @param pAsyncHttpConnection [Out] An IAsyncHttpConnection pointer.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_IN_PROGRESS the connection is in progress
     *          or other negative error codes.
     */
    virtual int32_t openConnection(
                        std::string const& hostName,
                        uint16_t portNumber,
                        const AseTimeVal& httpConnectionTimeout,
                        const AseTimeVal& httpStallTimeout,
                        uint32_t socketReceiverBuffer,
                        IHttpConnectionTraceListener* pTraceListener,
                        bool minRecvMode,
                        IAsyncHttpConnectionPtr& pAsyncHttpConnection ) = 0;

    /** Close an HTTP connection.
     *
     *  @param pAsyncHttpConnection [In] The IAsyncHttpConnection pointer.
     *  @param bAllowReuse [in] true if reuse of this connection is allowed
     *  @param reason [in] Connection close reason
     *
     *  @return AS_NO_ERROR on success.
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_NOT_FOUND The connection is not found
     *          or other negative error codes.
     */
    virtual int32_t closeConnection(
                        IAsyncHttpConnectionPtr const& pAsyncHttpConnection,
                        bool bAllowReuse = true,
                        int32_t reason = AS_CONNECTION_CLOSED ) = 0;

    /** Get the connection failure code */
    virtual int32_t getConnectionFailureCode() const = 0;

    /** Get the low level failure code */
    virtual int32_t getLowLevelFailureCode() const = 0;

    /** Wake up the HTTP client from select().
     *
     *  @return 0 on success
     *          -1 unknown error
     *          other negative error codes.
     */
    virtual int32_t wake() = 0;

    /** Send a signal byte from the external to the async HTTP library. */
    virtual int32_t sendSignalByteExternal() = 0;

    /** Receive the signal byte from the internal of the async HTTP library. */
    virtual int32_t receiveSignalByteInternal() = 0;

    /** Set the external FD sets.
     *
     *  @param maxFd [In] The maximum FD number.
     *  @param pReadFdSet [In] The read FD set.
     *  @param pWriteFdSet [In] The write FD set.
     */
    virtual void setExternalFdSets(
                    int maxFd,
                    fd_set* pReadFdSet,
                    fd_set* pWriteFdSet ) = 0;

    /** Return conditions for select */
    enum
    {
        SELECT_WAKE = 1,
        SELECT_CONNECT = 2,
        SELECT_WAKE_AND_CONNECT = 3,
    };

    /** Wait for events on connections or wake up signal.
     *
     *  @param selectedConnections [Out] A list of connections on which events happen.
     *  @param timeout [In] The timeout value, AseTimeVal::INFINITE for blocking indefinitely.
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
                    const AseTimeVal& timeout ) = 0;
};

typedef IAsyncHttpClient::IAsyncHttpClientPtr IAsyncHttpClientPtr;

/** Construct an asynchronous HTTP client. */
int32_t constructAsyncHttpClient(
                    IAsyncHttpClientPtr& pAsyncHttpClient,
                    IpConnectivityManager* pIpConnectivityManager,
                    IDnsCache* pGlobalDnsCache = NULL,
                    IDnsTraceListener* pDnsTraceListener = NULL,
                    IRouteListener* pRouteListener = NULL,
                    bool enableConnectionReuse = false,
                    llong maxContentLength = 0);

}
#endif //IASYNC_HTTP_LIB
