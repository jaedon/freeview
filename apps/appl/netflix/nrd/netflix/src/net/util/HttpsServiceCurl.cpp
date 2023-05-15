/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "HttpsService.h"

#include "HttpServiceMetrics.h"
#include "HttpServiceRequest.h"

#include <unistd.h>
#include <nrdbase/Log.h>
#include <nrdbase/ObjectCount.h>
#include <curl/curl.h>

using namespace std;

NRDP_DEFINE_OBJECTCOUNT(HttpsServiceNode);

namespace netflix
{
//Curl callbacks.
static curl_socket_t curlSocketOpenCallback(void * clientp,
                                            curlsocktype purpose,
                                            struct curl_sockaddr * address);

static int curlSocketOptCallback(void * clientp,
                                 curl_socket_t curlfd,
                                 curlsocktype purpose);

#ifdef NRDP_HAS_TRACING
static int curlDebugCallback(CURL * easy,
                             curl_infotype type,
                             char * data,
                             size_t size,
                             void * userp);
#endif

static size_t curlDataCallback(void * data,
                               size_t size,
                               size_t nmemb,
                               void * userdata);

static size_t curlHeaderCallback(void * dataPtr,
                                 size_t size,
                                 size_t nmemb,
                                 void * userdata);

static CURLcode curlSslContextCallback(CURL * curl,
                                       void * sslCtx,
                                       void * data);

static int curlSslVerifyCallback(int validSig,
                                 X509_STORE_CTX * ctx);

#if defined(NRDP_LIMIT_SOCKET_COUNT)
static int const HTTPS_CONNECTIONS_MAX = 1;
#else
static int const HTTPS_CONNECTIONS_MAX = 6;
#endif

// User data stored in the SSL context
static int sslExDataIndex_;
static int startedRefCount_ = 0;

class HttpsService::Impl
{
public:
    //The CURL multi handle for HTTPs.
    CURLM * multiHandle_;
};

class HttpsService::Node
{
public:
    //The node states.
    typedef enum
    {
        CHECKING = 0,
        CHECKED,
        CONNECTING,
        RUNNING
    } State;

public:
    //Constructor.
    Node(uint32_t id);

    //Destructor.
    virtual ~Node();

    uint32_t id() const { return mId; }

    //Initialize the working curl handle.
    bool initWorkingCurlHandle();

    //Deinitialize the working curl handle.
    void deinitWorkingCurlHandle();

public:

    //The HTTP service request pointer.
    HttpServiceRequestPtr mServiceRequestPtr;

    //The trust store.
    TrustStorePtr mTrustStorePtr;

    //The certificate verification function.
    VerifyCertStatusFunc mVerifyCertStatusFuncPtr;

    NrdIpAddr mServerIpAddress;
    //The SSL verify peer flag.
    bool mSslVerifyPeerFlag;

    //The response headers.
    HttpResponseHeaders mResponseHeaders;

    //The response body.
    DataBuffer mResponseBody;

    //The state.
    State mState;
    bool mPaused;

    //The async HTTP connection.
    IAsyncHttpConnectionPtr mAsyncHttpConnectionPtr;

    //The working curl handle.
    CURL * mWorkingCurlHandle;

    //The request headers slist.
    struct curl_slist * mRequestHeaderSlist;

    //The fake DNS record to prime libcurl DNS cache.
    struct curl_slist * mDnsRecordPtr;

    //The socket handle.
    int mSocketHandle;

    //The transaction start time.
    AseTimeVal mTransactionStartTime;

    //The HTTP service metrics.
    HttpServiceMetrics mHttpServiceMetrics;

private:

    // The request's ID.
    uint32_t mId;
};

uint32_t HttpsService::mIdPool = HTTPS_REQUEST_ID_MASK | 1;

HttpsService::HttpsService()
    : mConnectionCount(0)
    , mListener(NULL)
    , mMutex(HTTPS_SERVICE_MUTEX, "HttpsService")
    , mImpl(new Impl)
{
}

HttpsService::~HttpsService()
{
    delete mImpl;
}

void HttpsService::cleanUpRequests()
{
    for (NodeMap::iterator n = mNodeMap.begin(); n != mNodeMap.end(); ++n)
    {
        if (n->second->mAsyncHttpConnectionPtr)
        {
            mAsyncHttpClientPtr->closeConnection(n->second->mAsyncHttpConnectionPtr);
            n->second->mAsyncHttpConnectionPtr.reset();
        }
    }

    mNodeMap.clear();
}

bool HttpsService::start(HttpsServiceListener * listener, IAsyncHttpClientPtr client)
{
    mListener = listener;
    mAsyncHttpClientPtr = client;
    mImpl->multiHandle_ = curl_multi_init();
    if (mImpl->multiHandle_ == NULL)
        return false;

    if (startedRefCount_ == 0)
    {
        sslExDataIndex_ = SSL_get_ex_new_index(
            0,
            const_cast<char *>("HttpServiceThread_ssl_ex_index"),
            0,
            0,
            0);
    }

    ++startedRefCount_;
    return true;
}

void HttpsService::stop()
{
    if (mImpl->multiHandle_)
    {
        curl_multi_cleanup(mImpl->multiHandle_);
    }

    //Cleanup HTTPs handles.
    mImpl->multiHandle_ = NULL;
    mAsyncHttpClientPtr.reset();
}

void HttpsService::configure() const
{
    CURLMcode curlMultiCode;

    assert(mImpl->multiHandle_);

    curlMultiCode = curl_multi_setopt(mImpl->multiHandle_,
                                      CURLMOPT_MAXCONNECTS,
                                      HTTPS_CONNECTIONS_MAX);
    (void)curlMultiCode;

    assert(curlMultiCode == CURLM_OK);

    curlMultiCode = curl_multi_setopt(mImpl->multiHandle_,
                                      CURLMOPT_PIPELINING,
                                      0);

    assert(curlMultiCode == CURLM_OK);
}



long HttpsService::getTimeout() const
{
    CURLMcode rv;
    long timeout;
    rv = curl_multi_timeout(mImpl->multiHandle_, &timeout);
    (void)rv;

    //Make the timeout at most 1 second.
    if ((timeout < 0) || (timeout >= 1000))
    {
        timeout = 1000;
    }

    assert(rv == CURLM_OK);
    return timeout;
}


bool HttpsService::removePendingRequest(uint32_t id)
{
    ScopedMutex lock(mMutex);

    //Check if the request is still in the new request map.
    NodeMap::iterator i = mNewNodeMap.find(id);

    if (i != mNewNodeMap.end())
    {
        mNewNodeMap.erase(i);
        return true;
    }
    else
    {
        return false;
    }
}

bool HttpsService::isPending(uint32_t id) const
{
    ScopedMutex lock(mMutex);

    return (mNewNodeMap.find(id) != mNewNodeMap.end());
}


bool HttpsService::isActive(uint32_t id) const
{
    return (mNodeMap.find(id) != mNodeMap.end());
}


bool HttpsService::registerConnection(Node * pNode)
{
    if (!pNode->initWorkingCurlHandle())
    {
        return false;
    }

    curl_multi_add_handle(mImpl->multiHandle_, pNode->mWorkingCurlHandle);
    return true;
}

uint32_t HttpsService::addRequest(HttpServiceRequestPtr pServiceRequest,
                                  TrustStorePtr pTrustStore,
                                  VerifyCertStatusFunc pVerifyCertStatusFunc,
                                  bool sslVerifyPeerFlag)
{
    //Create a request node.
    NodePtr pNode(new Node(allocateId()));
    NRDP_OBJECTCOUNT_DESCRIBE(pNode->mResponseBody, std::string("HTTPSERVICEBODY:")+pServiceRequest->mRequestUrl.str());

    pNode->mServiceRequestPtr =  pServiceRequest;
    pNode->mTrustStorePtr = pTrustStore;
    pNode->mVerifyCertStatusFuncPtr = pVerifyCertStatusFunc;
    pNode->mSslVerifyPeerFlag = sslVerifyPeerFlag;

    //Create the request header slist.
    if (!(pServiceRequest->mRequestHeaders.empty()))
    {
        HttpHeaders::iterator itrHttpHeader;

        itrHttpHeader = pServiceRequest->mRequestHeaders.begin();

        while (itrHttpHeader != pServiceRequest->mRequestHeaders.end())
        {
            if ((!(itrHttpHeader->first.empty()))
                && (!(itrHttpHeader->second.empty())))
            {
                string oneHeader = itrHttpHeader->first
                    + ": "
                    + itrHttpHeader->second;

                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "added one HTTP header %s",
                    oneHeader.c_str());

                pNode->mRequestHeaderSlist = curl_slist_append(
                    pNode->mRequestHeaderSlist,
                    oneHeader.c_str());
            }

            itrHttpHeader++;
        }
    }

#if LIBCURL_VERSION_NUM >= 0x071d00
    //Work out the fake DNS record.
    string hostPortAddrStr = pServiceRequest->mRequestUrl.GetHost();

    char tempPortNumber[32];

    snprintf(
        tempPortNumber,
        32,
        ":%d",
        pServiceRequest->mRequestUrl.GetPortNumber());

    hostPortAddrStr += tempPortNumber;

    hostPortAddrStr += ":127.0.0.1";

    pNode->mDnsRecordPtr = curl_slist_append(
        pNode->mDnsRecordPtr,
        hostPortAddrStr.c_str());

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "set the fake DNS record: %s",
        hostPortAddrStr.c_str());
#endif

    {
        ScopedMutex lock(mMutex);
        mNewNodeMap[pNode->id()] = pNode;
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "added an HTTPs request %u %s.",
        pNode->id(),
        pNode->mServiceRequestPtr->mRequestUrl.str().c_str());

    return pNode->id();
}


//Cancel an HTTPs request.
void HttpsService::cancelRequest(
    uint32_t requestId,
    const AseUrl & requestUrl,
    std::vector<IAsyncHttpConnectionPtr> & selectedConnections)
{
    NRDP_UNUSED(requestUrl);
    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "was cancelling an HTTPs request: %u, %s",
        requestId,
        requestUrl.str().c_str());

    //Find the node record for the request.
    NodeMap::iterator itrNode;

    itrNode = mNodeMap.find(requestId);

    if (itrNode != mNodeMap.end())
    {
        if (itrNode->second->mAsyncHttpConnectionPtr)
        {
            //Remove the connection from the selected connection list.
            std::vector<IAsyncHttpConnectionPtr>::iterator itrConnection;

            itrConnection = selectedConnections.begin();

            while (itrConnection != selectedConnections.end())
            {
                if ((*itrConnection).get() == itrNode->second->mAsyncHttpConnectionPtr.get())
                {
                    selectedConnections.erase(itrConnection);

                    break;
                }

                itrConnection++;
            }

            mAsyncHttpClientPtr->closeConnection(
                itrNode->second->mAsyncHttpConnectionPtr);

            itrNode->second->mAsyncHttpConnectionPtr.reset();

        }

        if (itrNode->second->mWorkingCurlHandle)
        {
            curl_multi_remove_handle(
                mImpl->multiHandle_,
                itrNode->second->mWorkingCurlHandle);
        }

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "cancelled the HTTPs request: %u",
            requestId);

        //Destructor will clean up other members.
        mNodeMap.erase(itrNode);

        assert(mConnectionCount > 0);

        mConnectionCount--;
    }
    else
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "did not find the request %u in the node list.",
            requestId);
    }

    return;
}

void HttpsService::processConnections(std::vector<IAsyncHttpConnectionPtr> const & connections,
                                      const AseTimeVal & timeNow)
{
    for (std::vector<IAsyncHttpConnectionPtr>::const_iterator i = connections.begin(); i != connections.end(); ++i)
    {
        processConnection(timeNow, (Node *)((*i)->getUserData()));
    }
}

//Process the HTTPs connectioin.
void HttpsService::processConnection(
    const AseTimeVal & timeNow,
    Node * pNode)
{
    if (pNode->mAsyncHttpConnectionPtr->getState() == IAsyncHttpConnection::IDLE_STATE)
    {
        //The HTTPs connection is established.
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "opened the HTTPs connection, connection ID: %u.",
            pNode->mAsyncHttpConnectionPtr->getHttpId());

        pNode->mHttpServiceMetrics.mDnsLookupTime =
            (uint32_t)(pNode->mAsyncHttpConnectionPtr->getConnectingTime() - pNode->mAsyncHttpConnectionPtr->getStartTime()).ms();

        pNode->mHttpServiceMetrics.mHttpConnectTime =
            (uint32_t)(pNode->mAsyncHttpConnectionPtr->getConnectedTime() - pNode->mAsyncHttpConnectionPtr->getConnectingTime()).ms();

        pNode->mSocketHandle = pNode->mAsyncHttpConnectionPtr->transferSocketHandle();

        mAsyncHttpClientPtr->closeConnection(pNode->mAsyncHttpConnectionPtr);

        pNode->mHttpServiceMetrics.mServerIpAddress = pNode->mServerIpAddress = pNode->mAsyncHttpConnectionPtr->getServerIpAddress();
        pNode->mHttpServiceMetrics.mCName = pNode->mAsyncHttpConnectionPtr->getCName();

        pNode->mAsyncHttpConnectionPtr.reset();

        pNode->mTransactionStartTime = timeNow;

        pNode->mState = Node::RUNNING;

        if (!registerConnection(pNode))
        {
            finishRequest(
                pNode,
                AS_INIT_FAILURE,
                0);
        }
    }
    else
    {
        int32_t connectionFailureCode = pNode->mAsyncHttpConnectionPtr->getConnectionFailureCode();

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "failed to open the HTTPs connection, connection ID: %u, failure: %d.",
            pNode->mAsyncHttpConnectionPtr->getHttpId(),
            connectionFailureCode);

        mAsyncHttpClientPtr->closeConnection(
            pNode->mAsyncHttpConnectionPtr);

        finishRequest(
            pNode,
            connectionFailureCode,
            0);
    }
}

//Process HTTPs requests.
void HttpsService::processNewRequests(
    const AseTimeVal & timeNow)
{
    while (mConnectionCount < HTTPS_CONNECTIONS_MAX)
    {
        NodePtr pNode;

        //Get a new HTTPs request node.
        {
            ScopedMutex lock(mMutex);

            if (mNewNodeMap.empty())
            {
                break;
            }

            pNode = mNewNodeMap.begin()->second;
            mNewNodeMap.erase(mNewNodeMap.begin());
        }

        pNode->mTransactionStartTime = timeNow;

        pNode->mState = Node::CHECKING;

        mNodeMap[pNode->id()] = pNode;
        mConnectionCount++;

        if (!registerConnection(pNode.get()))
        {
            finishRequest(
                pNode.get(),
                AS_INIT_FAILURE,
                0);
        }
    }
}

//Perform HTTPs operations.
void HttpsService::service(std::vector<IAsyncHttpConnectionPtr> & selectedConnections, const AseTimeVal & timeNow)
{
    (void)selectedConnections;
    CURLMcode curlMultiCode;

    int runningHandles = 0;

    do
    {
        curlMultiCode = curl_multi_perform(
            mImpl->multiHandle_,
            &runningHandles);

    }
    while (curlMultiCode == CURLM_CALL_MULTI_PERFORM);

    if (curlMultiCode != CURLM_OK)
    {
        Log::error(
            TRACE_HTTP_SERVICE_THREAD,
            "curl_multi_perform returned error: %d",
            curlMultiCode);
    }

    while (true) {
        int messages = 0;
        CURLMsg *pMsg = curl_multi_info_read(mImpl->multiHandle_, &messages);
        if (!pMsg)
            break;
        if (pMsg->msg == CURLMSG_DONE)
        {
            CURL * pWorkingHandle = pMsg->easy_handle;

            CURLcode curlEasyCode = pMsg->data.result;

            //Get the pointer to the node.
            char * pPrivate = NULL;

            Node * pNode;

            curl_easy_getinfo(
                pWorkingHandle,
                CURLINFO_PRIVATE,
                &pPrivate);

            assert(pPrivate);

            pNode = (Node *) pPrivate;

            long httpStatusCode = 0;

            if (curl_easy_getinfo(
                    pWorkingHandle,
                    CURLINFO_RESPONSE_CODE,
                    &httpStatusCode) != CURLE_OK)
            {
                httpStatusCode = 400;
            }

            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "libcurl completed the transaction, request: %u, curl code: %d,"
                "curl status: %s, HTTP response status: %ld",
                pNode->id(),
                curlEasyCode,
                curl_easy_strerror(curlEasyCode),
                httpStatusCode);

            assert(timeNow >= pNode->mTransactionStartTime);

            pNode->mHttpServiceMetrics.mHttpTransactionTime = (uint32_t)(timeNow - pNode->mTransactionStartTime).ms();

            assert((pNode->mState == Node::CHECKING)
                   || (pNode->mState == Node::CHECKED)
                   || (pNode->mState == Node::RUNNING));

            curlMultiCode = curl_multi_remove_handle(mImpl->multiHandle_, pNode->mWorkingCurlHandle);
            if ( curlMultiCode != CURLM_OK){
                Log::error(
                    TRACE_HTTP_SERVICE_THREAD,
                    "curl_multi_remove_handle returned error: %d (%s)",
                    curlMultiCode, curl_multi_strerror(curlMultiCode));
            }

            pNode->deinitWorkingCurlHandle();

            if (pNode->mState == Node::CHECKED)
            {
                //libcurl tells us it needs a new connection.
                int32_t retVal;

                retVal = mAsyncHttpClientPtr->openConnection(
                    pNode->mServiceRequestPtr->mRequestUrl.GetHost(),
                    pNode->mServiceRequestPtr->mRequestUrl.GetPortNumber(),
                    pNode->mServiceRequestPtr->mConnectTimeout,
                    pNode->mServiceRequestPtr->mStallTimeout,
                    32 * 1024,
                    NULL,
                    false,
                    pNode->mAsyncHttpConnectionPtr);

                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "started to open HTTPs connection to %s %d, connection ID: %d, ret: %d",
                    pNode->mServiceRequestPtr->mRequestUrl.GetHost().c_str(),
                    pNode->mServiceRequestPtr->mRequestUrl.GetPortNumber(),
                    ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS)) ?
                    (int32_t) pNode->mAsyncHttpConnectionPtr->getHttpId() : -1,
                    retVal);

                if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
                {
                    pNode->mState = Node::CONNECTING;

                    pNode->mAsyncHttpConnectionPtr->setUserData(pNode);
                }
                else
                {
                    //Failed to open the connection.
                    finishRequest(
                        pNode,
                        curlEasyCode,
                        httpStatusCode);
                }
            }
            else
            {
                //The transaction has actually failed.
                finishRequest(
                    pNode,
                    curlEasyCode,
                    httpStatusCode);
            }
        }
    }

    fd_set readFd;
    fd_set writeFd;
    fd_set errorFd;
    int httpsMaxFd = -1;

    FD_ZERO(&readFd);
    FD_ZERO(&writeFd);
    FD_ZERO(&errorFd);

    CURLMcode rv;
    rv = curl_multi_fdset(mImpl->multiHandle_, &readFd, &writeFd, &errorFd, &httpsMaxFd);
    assert(rv == CURLM_OK);
    NRDP_UNUSED(rv);

    //Work out the external FD sets.
    mAsyncHttpClientPtr->setExternalFdSets(httpsMaxFd, &readFd, &writeFd);
}

//Finish an HTTPs request.
void HttpsService::finishRequest(
    Node * pNode,
    int32_t curlErrorCode,
    long httpStatusCode)
{
    if (curlErrorCode == CURLE_OK)
    {
        pNode->mResponseBody.reserve(pNode->mResponseBody.size());

        pNode->mServiceRequestPtr->provideResponseHeaders(pNode->mResponseHeaders);

        pNode->mServiceRequestPtr->provideResponseBody(pNode->mResponseBody);
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "was calling the finish callback %u.",
        pNode->id());

    pNode->mServiceRequestPtr->finish(
        curlErrorCode,
        httpStatusCode,
        &(pNode->mHttpServiceMetrics));

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "called the finish callback %u %d %ld %u %u %u.",
        pNode->id(),
        curlErrorCode,
        httpStatusCode,
        pNode->mHttpServiceMetrics.mDnsLookupTime,
        pNode->mHttpServiceMetrics.mHttpConnectTime,
        pNode->mHttpServiceMetrics.mHttpTransactionTime);

    mListener->onHttpsRequestFinished(pNode->id());

    //Remove the node from the map.
    mNodeMap.erase(pNode->id());

    //Decrement the HTTPs connection count.
    assert(mConnectionCount > 0);

    mConnectionCount--;

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "finished the HTTPs request.");
}

uint32_t HttpsService::allocateId()
{
    // Note: this could be a problem if there are two simultaneous instances -- jjb 10/31/13
    uint32_t id = mIdPool++;
    if (mIdPool == (HTTPS_REQUEST_ID_MASK | 0x7fffffff))
    {
        mIdPool = HTTPS_REQUEST_ID_MASK | 1;
    }

    return id;
}

//Curl callbacks.
curl_socket_t curlSocketOpenCallback(
    void * clientp,
    curlsocktype /*purpose*/,
    struct curl_sockaddr * /*address*/)
{
    curl_socket_t sockFd;

    HttpsService::Node * pNode = (HttpsService::Node *) clientp;

    if (pNode->mState == HttpsService::Node::CHECKING)
    {
        sockFd = CURL_SOCKET_BAD;

        pNode->mState = HttpsService::Node::CHECKED;

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "signaled a new connection is needed for request: %u",
            pNode->id());
    }
    else
    {
        sockFd = pNode->mSocketHandle;

        pNode->mSocketHandle = CURL_SOCKET_BAD;

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "transferred a connected socket: %d to libcurl for request: %u.",
            sockFd,
            pNode->id());
    }

    return sockFd;
}

static int curlSocketOptCallback(
    void * /*clientp*/,
    curl_socket_t /*curlfd*/,
    curlsocktype /*purpose*/)
{
    return CURL_SOCKOPT_ALREADY_CONNECTED;
}

#ifdef NRDP_HAS_TRACING
static int curlDebugCallback(
    CURL * /*easy*/,
    curl_infotype type,
    char * data,
    size_t size,
    void * /*userp*/)
{
    NRDP_UNUSED(data);
    NRDP_UNUSED(size);

    switch (type)
    {
    case CURLINFO_TEXT:
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "[CURL-INFO] %s",
            string(data, size).c_str());

        break;
    }

    case CURLINFO_HEADER_IN:
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "[RESP-HEADER] %s",
            string(data, size).c_str());

        break;
    }

    case CURLINFO_HEADER_OUT:
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "[REQ-HEADER] %s",
            string(data, size).c_str());

        break;
    }

    case CURLINFO_DATA_IN:
    {
        /*
          NTRACE(
          TRACE_HTTP_SERVICE_THREAD,
          "[DATA-IN] %s",
          string( data, size ).c_str() );
        */
        break;
    }
    case CURLINFO_DATA_OUT:
    {
        /*
          NTRACE(
          TRACE_HTTP_SERVICE_THREAD,
          "[DATA-OUT] %s",
          string( data, size ).c_str() );
        */
        break;
    }

    default:
        // Don't know what to do.
        break;
    }

    // All done.
    return 0;
}
#endif

int curlProgressCallback(void *data, double, double, double, double)
{
    HttpsService::Node * pNode = (HttpsService::Node *) data;
#ifdef CURL_WRITEFUNC_PAUSE
    if(pNode->mPaused && !pNode->mServiceRequestPtr->shouldSuspend()) {
        pNode->mPaused = false;
        curl_easy_pause(pNode->mWorkingCurlHandle, CURLPAUSE_CONT);
    }
#endif
    return 0;
}


size_t curlDataCallback(
    void * data,
    size_t size,
    size_t nmemb,
    void * userdata)
{
    HttpsService::Node * pNode = (HttpsService::Node *) userdata;
#ifdef CURL_WRITEFUNC_PAUSE
    if(pNode->mPaused || pNode->mServiceRequestPtr->shouldSuspend()) {
        pNode->mPaused = true;
        return CURL_WRITEFUNC_PAUSE;
    }
#endif

    const size_t len = (size * nmemb);

    pNode->mResponseBody.append((const unsigned char *) data, len);
    return len;
}

size_t curlHeaderCallback(
    void * dataPtr,
    size_t size,
    size_t nmemb,
    void * userdata)
{
    HttpsService::Node * pNode = (HttpsService::Node *) userdata;

    const char * data = reinterpret_cast<const char *>(dataPtr);
    const size_t len = (size * nmemb);
    if (len && data && *data)
    {
        size_t colon = 0;
        while (colon < len && data[colon] != ':')
        {
            ++colon;
        }
        if (colon != len)
        {
            size_t valueStart = colon + 1;
            while (valueStart < len && std::isspace((unsigned char)data[valueStart]))
            {
                ++valueStart;
            }

            size_t valueEnd = len - 1;
            while (valueEnd >= valueStart && std::isspace((unsigned char)data[valueEnd]))
            {
                --valueEnd;
            }

            pNode->mResponseHeaders.push_back(std::make_pair(std::string(data, colon),
                                              valueStart <= valueEnd ? std::string(data + valueStart, valueEnd - valueStart + 1) : std::string()));

            enum { ContentLengthLength = 14 };
            if (colon == ContentLengthLength && !pNode->mResponseBody.capacity()
                && !strncasecmp(data, "Content-Length", ContentLengthLength))
            {
                const unsigned long contentLength = strtoul(data + valueStart, 0, 10);
                if (contentLength > 0)
                {
                    pNode->mResponseBody.reserve(static_cast<int>(contentLength));
                }
            }

        }
    }
    return len;
}

CURLcode curlSslContextCallback(
    CURL * /*curl*/,
    void * sslCtx,
    void * data)
{
    SSL_CTX * ctx = static_cast<SSL_CTX *>(sslCtx);

    static const char *supportedCiphers = "ECDH+AESGCM:DH+AESGCM:RSA+AESGCM:ECDH+AES128:DH+AES:ECDH+AES256:DH+AES256:ECDH+3DES:DH+3DES:RSA+AES:RSA+3DES:!aNULL:!MD5:!DSS";
    int result = SSL_CTX_set_cipher_list(ctx, supportedCiphers);
    assert(result);
    (void)result;

    HttpsService::Node * pNode = (HttpsService::Node *) data;

    std::vector<X509 *>::const_iterator itrX509;

    itrX509 = pNode->mTrustStorePtr->x509s.begin();

    while (itrX509 != pNode->mTrustStorePtr->x509s.end())
    {
        X509_STORE_add_cert(ctx->cert_store, (*itrX509));

        itrX509++;
    }

    SSL_CTX_set_ex_data(ctx, sslExDataIndex_, pNode);

    X509_VERIFY_PARAM_set_time(ctx->param, Time::serverTime().seconds());

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, curlSslVerifyCallback);

    enum { SSLOpts = (SSL_OP_ALL |
                      SSL_OP_NO_TLSv1 |
                      SSL_OP_NO_SSLv2 |
                      SSL_OP_NO_SSLv3 |
                      SSL_OP_CIPHER_SERVER_PREFERENCE) };
    result = SSL_CTX_set_options(ctx, SSLOpts);
    assert(result);
    (void)result;

    return CURLE_OK;
}

int curlSslVerifyCallback(
    int validSig,
    X509_STORE_CTX * ctx)
{
    SSL * ssl = (SSL *) X509_STORE_CTX_get_ex_data(
        ctx,
        SSL_get_ex_data_X509_STORE_CTX_idx());

    HttpsService::Node * pNode = (HttpsService::Node *)(SSL_CTX_get_ex_data(ssl->initial_ctx, sslExDataIndex_));

    int ret = pNode->mVerifyCertStatusFuncPtr(validSig, ctx,
                                              pNode ? pNode->mServiceRequestPtr.get() : 0,
                                              pNode ? &pNode->mServerIpAddress : 0);

    return ret;
}

#if 0
// No longer used. Here for reference only.

//Convert CURL code to ASE error code.
int32_t convertCurlCodeToAseErrorCode(
    CURLcode curlCode)
{
    int32_t aseErrorCode = AS_UNKNOWN_ERROR;

    switch (curlCode)
    {
        case CURLE_OK:
            aseErrorCode = AS_NO_ERROR;
            break;

        case CURLE_COULDNT_RESOLVE_PROXY:
        case CURLE_COULDNT_RESOLVE_HOST:
            aseErrorCode = AS_DNS_ERROR;
            break;

        case CURLE_COULDNT_CONNECT:
            aseErrorCode = AS_HTTP_CONNECTION_ERROR;
            break;


        case CURLE_OPERATION_TIMEDOUT:
            aseErrorCode = AS_TIMEOUT;
            break;

        case CURLE_SEND_ERROR:
        case CURLE_RECV_ERROR:
        case CURLE_GOT_NOTHING:
        case CURLE_PARTIAL_FILE:
        case CURLE_UNSUPPORTED_PROTOCOL:
        case CURLE_URL_MALFORMAT:
        case CURLE_BAD_FUNCTION_ARGUMENT:
            aseErrorCode = AS_HTTP_CONNECTION_ERROR;
            break;

        case CURLE_PEER_FAILED_VERIFICATION:
        case CURLE_SSL_CACERT:
        case CURLE_SSL_ISSUER_ERROR:
        case CURLE_SSL_CONNECT_ERROR:
        case CURLE_SSL_CIPHER:
        case CURLE_SSL_ENGINE_NOTFOUND:
        case CURLE_SSL_ENGINE_SETFAILED:
        case CURLE_SSL_CERTPROBLEM:
        case CURLE_SSL_ENGINE_INITFAILED:
        case CURLE_SSL_CACERT_BADFILE:
            aseErrorCode = AS_SSL_ERROR;
            break;

        default:
            aseErrorCode = AS_UNKNOWN_ERROR;
    }

    return aseErrorCode;
}

#endif
/**
 * The HTTPs request node class.
 */
HttpsService::Node::Node(uint32_t id) : mState(CHECKING), mPaused(false),
    mWorkingCurlHandle(NULL),
    mRequestHeaderSlist(NULL),
    mDnsRecordPtr(NULL),
    mSocketHandle(CURL_SOCKET_BAD),
    mId(id)
{
    NRDP_OBJECTCOUNT_REF(HttpsServiceNode, this);
}

//Destructor.
HttpsService::Node::~Node()
{
    NRDP_OBJECTCOUNT_DEREF(HttpsServiceNode, this);
    if (mWorkingCurlHandle)
    {
        curl_easy_cleanup(mWorkingCurlHandle);
    }

    if (mRequestHeaderSlist)
    {
        curl_slist_free_all(mRequestHeaderSlist);
    }

    if (mDnsRecordPtr)
    {
        curl_slist_free_all(mDnsRecordPtr);
    }

    if (mSocketHandle != CURL_SOCKET_BAD)
    {
        ::close(mSocketHandle);

        mSocketHandle = CURL_SOCKET_BAD;
    }
}

//Initialize the working curl handle.
bool HttpsService::Node::initWorkingCurlHandle()
{
    bool retVal = false;

    //Create the working curl handle.
    mWorkingCurlHandle = curl_easy_init();

    if (mWorkingCurlHandle)
    {

        //Initialize the working curl handle.
        if (!(mServiceRequestPtr->mRequestBody.empty()))
        {
            //Always do post if there is a request body.
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_POSTFIELDSIZE,
                mServiceRequestPtr->mRequestBody.size());

            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_COPYPOSTFIELDS,
                mServiceRequestPtr->mRequestBody.c_str());
        }
        else if (mServiceRequestPtr->mRequestMethod == IAsyncHttpRequest::POST_METHOD)
        {
            //Always do post if there is a request body.
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_POSTFIELDSIZE,
                0);

            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_POST,
                1);
        }
        else if (mServiceRequestPtr->mRequestMethod == IAsyncHttpRequest::HEAD_METHOD)
        {
            // Asked for head, but no body
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_NOBODY,
                1);
        }
        else if (mServiceRequestPtr->mRequestMethod == IAsyncHttpRequest::DELETE_METHOD)
        {
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_CUSTOMREQUEST,
                "DELETE");
        }
        else if (mServiceRequestPtr->mRequestMethod != IAsyncHttpRequest::GET_METHOD)
        {
            // Should never happen
            assert(false);
        }

        if (mRequestHeaderSlist)
        {
            mRequestHeaderSlist = curl_slist_append(mRequestHeaderSlist, "Expect:");
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_HTTPHEADER,
                mRequestHeaderSlist);
        }

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_NOSIGNAL,
            1);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_FOLLOWLOCATION,
            0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_HEADERFUNCTION,
            curlHeaderCallback);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_HEADERDATA,
            this);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_WRITEFUNCTION,
            curlDataCallback);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_WRITEDATA,
            this);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_NOPROGRESS,
            0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_PROGRESSFUNCTION,
            curlProgressCallback);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_PROGRESSDATA,
            this);

        if (mServiceRequestPtr->mTimeout > AseTimeVal::ZERO)
        {
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_TIMEOUT_MS,
                mServiceRequestPtr->mTimeout.ms());
        }

        if (mServiceRequestPtr->mConnectTimeout > AseTimeVal::ZERO)
        {
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_CONNECTTIMEOUT_MS,
                mServiceRequestPtr->mConnectTimeout.ms());
        }

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_FORBID_REUSE,
            0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_FRESH_CONNECT,
            0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_URL,
            mServiceRequestPtr->mRequestUrl.str().c_str());

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_SSL_CTX_FUNCTION,
            curlSslContextCallback);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_SSL_CTX_DATA,
            this);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_CAPATH,
            0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_CAINFO,
            0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_SSL_VERIFYHOST,
            mSslVerifyPeerFlag ? 2 : 0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_SSL_VERIFYPEER,
            mSslVerifyPeerFlag ? 1 : 0);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_ACCEPT_ENCODING,
            "");  // "" means send all available encodings

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_PROXY,
            "");

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_OPENSOCKETFUNCTION,
            curlSocketOpenCallback);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_OPENSOCKETDATA,
            this);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_SOCKOPTFUNCTION,
            curlSocketOptCallback);

        if (mDnsRecordPtr)
        {
            /*
             * Set DNS timeout forever (-1). This is to avoid libcurl(< 7.35)
             * bug that prune CURLOPT_RESOLVE entries after DNS cache timeout
             * has elapsed.
             *
             * With libcurl earlier than 7.35, below scenario can prune CURLOPT_RESOLVE
             * 1. client starts the connection and server keeps it alive
             * 2. CURLOPT_DNS_CACHE_TIMEOUT elapse (by default,
             *    libcurl's DNS cache timeout is 60sec)
             * 3. server kills the connection
             * 4. client starts a new request, finds the connection to
             *   be dead and creates a new one
             */
            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_DNS_CACHE_TIMEOUT,
                -1);

            curl_easy_setopt(
                mWorkingCurlHandle,
                CURLOPT_RESOLVE,
                mDnsRecordPtr);
        }

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_PRIVATE,
            this);

#ifdef NRDP_HAS_TRACING
        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_VERBOSE,
            1);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_DEBUGFUNCTION,
            curlDebugCallback);

        curl_easy_setopt(
            mWorkingCurlHandle,
            CURLOPT_DEBUGDATA,
            this);
#endif

        retVal = true;
    }

    return retVal;
}

//Deinitialize the working curl handle.
void HttpsService::Node::deinitWorkingCurlHandle()
{
    curl_easy_cleanup(mWorkingCurlHandle);

    mWorkingCurlHandle = NULL;
}


}
