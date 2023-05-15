/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "HttpService.h"

#include "HttpServiceMetrics.h"
#include "HttpServiceRequest.h"


#include <nrdbase/Log.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/AsciiTable.h>

#include <unistd.h>

using namespace std;

NRDP_DEFINE_OBJECTCOUNT(HttpServiceRequestRecord);
NRDP_DEFINE_OBJECTCOUNT(HttpServiceConnectionRecord);
NRDP_DEFINE_OBJECTCOUNT(HttpServiceServerRecord);

namespace netflix
{


class HttpService::RequestRecord
{
public:
    //Constructor.
    RequestRecord(uint32_t id);

    //Destructor.
    ~RequestRecord();

    uint32_t id() const { return mId; }

    //The HTTP service request pointer.
    HttpServiceRequestPtr mServiceRequestPtr;

    //The timeout duration.
    AseTimeVal mTimeoutDuration;

    //The cancelled flag.
    bool mCancelledFlag;

    //The response headers.
    HttpResponseHeaders mResponseHeaders;

    //The response body.
    DataBuffer mResponseBody;

    //The async HTTP request for this service request.
    IAsyncHttpRequestPtr mAsyncHttpRequestPtr;

    //The fresh request flag.
    bool mFreshFlag;

    //The length of the response body that has been received.
    llong mReceivedResponseBodyLength;

    //The pipelining request flag.
    bool mPipeliningRequestFlag;

    //The number of sent HTTP pipelining requests before this request.
    uint32_t mSentHttpPipeliningRequestCount;

    //The number of completed HTTP pipelining requests before this request.
    uint32_t mCompletedHttpPipeliningRequestCount;

    //The HTTP service metrics.
    HttpServiceMetrics mHttpServiceMetrics;

private:

    // The request's ID.
    uint32_t mId;
};


/********************************************************************************************************************/

class HttpService::ConnectionRecord
{
public:
    //Constructor.
    ConnectionRecord();

    //Destructor.
    virtual ~ConnectionRecord();

public:
    //The async HTTP connection.
    IAsyncHttpConnectionPtr mAsyncHttpConnectionPtr;

    //The outstanding request record list.
    HttpService::RequestList mOutstandingRequestList;

	//The number of attempted HTTP requests
    uint32_t mAttemptedHttpRequests;

    //The number of sent HTTP pipelining requests.
    uint32_t mSentHttpPipeliningRequestCount;

    //The number of completed HTTP pipelining requests.
    uint32_t mCompletedHttpPipeliningRequestCount;

    //The next connect time.
    AseTimeVal mNextConnectTime;

    //The flag indicates that the connection is just established.
    bool mJustConnectedFlag;

    int mId;
};


/********************************************************************************************************************/

class HttpService::ServerRecord
{
public:
    //Constructor.
    ServerRecord();

    //Destructor.
    virtual ~ServerRecord();

    HttpService::ServerKey key() const { return HttpService::ServerKey(mServerPort, mServerName); }

public:
    //The HTTP server name.
    std::string mServerName;

    //The HTTP server port.
    uint16_t mServerPort;

    //The HTTP connection record vector.
    HttpService::ConnectionVector mConnectionVector;

    //The total number of requests.
    uint32_t mTotalRequestCount;

    //The pending request record map.
    HttpService::RequestMap mPendingRequestMap;

    //The outstanding request record map.
    HttpService::RequestMap mOutstandingRequestMap;

    //The consecutive connect failure count.
    uint32_t mConsecutiveConnectFailureCount;

    //The last connection failure code.
    int32_t mLastConnectFailureCode;

    //The consecutive pipelining failure count.
    uint32_t mConsecutivePipeliningFailureCount;

    //The http connection timeout.
    AseTimeVal mHttpConnectionTimeout;

    //The http stall timeout.
    AseTimeVal mHttpStallTimeout;

    int mId;
};


/********************************************************************************************************************/

uint32_t HttpService::mIdPool = 1;

HttpService::HttpService()
    : mPipeliningDepth(10)
    , mPipeliningSupportThreshold(6)
    , mConnectBackoffTime(20)
    , mConnectFailureThreshold(3)
    , mNumberOfConnectionPerServer(3)
    , mListener(NULL)
    , mMutex(HTTP_SERVICE_MUTEX, "HttpService")
{
}

void HttpService::cleanUpRequests()
{
    for (ServerMap::iterator s = mServerMap.begin(); s != mServerMap.end(); ++s)
    {
        ConnectionVector & connections = s->second->mConnectionVector;
        for (ConnectionVector::iterator r = connections.begin(); r != connections.end(); ++r)
        {
            ConnectionRecordPtr record = *r;
            if (record->mAsyncHttpConnectionPtr)
            {
                mAsyncHttpClientPtr->closeConnection(record->mAsyncHttpConnectionPtr);
                record->mAsyncHttpConnectionPtr.reset();
            }
        }
    }

    mServerMap.clear();
}

AseTimeVal HttpService::findEarliestConnectTime(ullong max, AseTimeVal now)
{
    mEarliestConnectTime = AseTimeVal::INFINITE;

    for (ServerMap::iterator s = mServerMap.begin(); s != mServerMap.end(); ++s)
    {
        ConnectionVector const & connections = s->second->mConnectionVector;
        for (ConnectionVector::const_iterator c = connections.begin(); c != connections.end(); ++c)
        {
            AseTimeVal nextTime = (*c)->mNextConnectTime;
            if (nextTime < mEarliestConnectTime)
            {
                mEarliestConnectTime = nextTime;
            }
        }
    }

    return std::min(std::max(mEarliestConnectTime - now, AseTimeVal::ZERO), AseTimeVal::fromMS(max));
}

bool HttpService::removePendingRequest(uint32_t id)
{
    ScopedMutex lock(mMutex);

    //Check if the request is still in the new request map.
    RequestMap::iterator i = mNewRequestMap.find(id);

    if (i != mNewRequestMap.end())
    {
        mNewRequestMap.erase(i);
        return true;
    }
    else
    {
        return false;
    }
}

bool HttpService::isPending(uint32_t id) const
{
    ScopedMutex lock(mMutex);

    return (mNewRequestMap.find(id) != mNewRequestMap.end());
}

bool HttpService::isActive(uint32_t id, AseUrl const & url) const
{
    //Find the server record for the request.
    HttpService::ServerKey key(url);

    ServerMap::const_iterator pRecord = mServerMap.find(key);
    if (pRecord != mServerMap.end())
    {
        HttpService::ServerRecordPtr serverRecord = pRecord->second;
        if (serverRecord)
        {
            //Check if this request is pending in the server record.
            if (serverRecord->mPendingRequestMap.find(id) != serverRecord->mPendingRequestMap.end() ||
                serverRecord->mOutstandingRequestMap.find(id) != serverRecord->mOutstandingRequestMap.end())
            {
                return true;
            }
        }
    }
    return false;
}


bool HttpService::start(HttpServiceListener * listener, IAsyncHttpClientPtr client)
{
    mListener = listener;
    mAsyncHttpClientPtr = client;
    return true;
}

void HttpService::stop()
{
    mAsyncHttpClientPtr.reset();
}

static const Time sStarted = Time::mono();
static inline std::string formatTime(const AseTimeVal &time)
{
    if (time == AseTimeVal::ZERO) {
        return "zero";
    } else if (time == AseTimeVal::INFINITE) {
        return "infinite";
    }
    const Time t = Time::fromMS(time.ms());
    const Time diff = (t - sStarted);
    char buf[32];
    snprintf(buf, sizeof(buf),
             "%02lld:%02lld:%02lld.%03lld",
             diff.ms() / MilliSecondsPerHour,
             (diff.ms() % MilliSecondsPerHour) / MilliSecondsPerMinute,
             (diff.ms() % MilliSecondsPerMinute) / MilliSecondsPerSecond,
             diff.ms() % MilliSecondsPerSecond);
    return buf;
}

void HttpService::dump() const
{
    const char *stateNames[] = {
        "LOOKING_UP_STATE",   /**< The DNS lookup is in progress. */
        "OPENING_STATE",          /**< The connection is being opened. */
        "IDLE_STATE",             /**< The connection has been opened and there is no request in progress. */
        "ACTIVE_STATE",           /**< The connection has been opened and there is at least one request in
                                     progress. */
        "CLOSED_STATE",           /**< The connection has been closed. */
        "FAILED_STATE"            /**< The connection has failed. */
    };

    std::ostringstream out;
    out << "Servers:\n";
    bool first = true;
    for (ServerMap::const_iterator server = mServerMap.begin(); server != mServerMap.end(); ++server) {
        if (first) {
            first = false;
        } else {
            out << '\n';
        }
        {
            AsciiTable table(9, "Server", "Reqs", "Failures", "Last failure", "PL failures", "Conn to", "Stall to", "Pointer", "Id");
            table.addRow(server->first.toString(),
                         server->second->mTotalRequestCount,
                         server->second->mConsecutiveConnectFailureCount,
                         server->second->mLastConnectFailureCode,
                         server->second->mConsecutivePipeliningFailureCount,
                         server->second->mHttpConnectionTimeout.ms(),
                         server->second->mHttpStallTimeout.ms(),
                         server->second.get(),
                         server->second->mId);
            out << table.toString();
        }

        if (!server->second->mConnectionVector.empty()) {
            AsciiTable table(18, "Outstanding", "PL Sent", "PL completed", "Next connect", "DidOpen",
                             "State", "Ip", "Bufsiz", "Start", "Connecting", "Connected", "Failed", "Failure", "LL", "RecPtr", "RecId", "Ptr", "Id");
            for (ConnectionVector::const_iterator connection = server->second->mConnectionVector.begin();
                 connection != server->second->mConnectionVector.end(); ++connection) {
                if ((*connection)->mAsyncHttpConnectionPtr) {
                    table.addFormat("%zu,%u,%u,%s,%d,%s,%s,%u,%s,%s,%s,%s,%d,%d,%p,%d,%p,%d",
                                    (*connection)->mOutstandingRequestList.size(),
                                    (*connection)->mSentHttpPipeliningRequestCount,
                                    (*connection)->mCompletedHttpPipeliningRequestCount,
                                    formatTime((*connection)->mNextConnectTime).c_str(),
                                    (*connection)->mAsyncHttpConnectionPtr->didOpen(),
                                    stateNames[(*connection)->mAsyncHttpConnectionPtr->getState()],
                                    (*connection)->mAsyncHttpConnectionPtr->getServerIpAddress().getStrAddr().c_str(),
                                    (*connection)->mAsyncHttpConnectionPtr->getSocketReceiveBufferSize(),
                                    formatTime((*connection)->mAsyncHttpConnectionPtr->getStartTime()).c_str(),
                                    formatTime((*connection)->mAsyncHttpConnectionPtr->getConnectingTime()).c_str(),
                                    formatTime((*connection)->mAsyncHttpConnectionPtr->getConnectedTime()).c_str(),
                                    formatTime((*connection)->mAsyncHttpConnectionPtr->getFailedTime()).c_str(),
                                    (*connection)->mAsyncHttpConnectionPtr->getConnectionFailureCode(),
                                    (*connection)->mAsyncHttpConnectionPtr->getLowLevelFailureCode(),
                                    connection->get(),
                                    (*connection)->mId,
                                    (*connection)->mAsyncHttpConnectionPtr.get(),
                                    (*connection)->mAsyncHttpConnectionPtr->getId());

                } else {
                    table.addFormat("%zu,%u,%u,%s,%d,%s,%s,%u,%s,%s,%s,%s,%d,%d,%p,%d",
                                    (*connection)->mOutstandingRequestList.size(),
                                    (*connection)->mSentHttpPipeliningRequestCount,
                                    (*connection)->mCompletedHttpPipeliningRequestCount,
                                    formatTime((*connection)->mNextConnectTime).c_str(),
                                    false, "", "", 0, "", "", "", "", -1, -1,
                                    connection->get(),
                                    (*connection)->mId);
                }
            }
            out << table.toString();
        }
        struct {
            const char *name;
            const RequestMap *map;
        } const requestMaps[] = {
            { "Pending", &server->second->mPendingRequestMap },
            { "Outstanding", &server->second->mOutstandingRequestMap }
        };
        for (size_t i=0; i<sizeof(requestMaps) / sizeof(requestMaps[0]); ++i) {
            const RequestMap &map = *requestMaps[i].map;
            if (!map.empty()) {
                AsciiTable table(4, "Url", "Id", "Body", "Received");
                for (RequestMap::const_iterator request = map.begin(); request != map.end(); ++request) {
                    table.addRow(request->second->mServiceRequestPtr->mRequestUrl.str(),
                                 request->first, request->second->mServiceRequestPtr->mRequestBody.size(),
                                 request->second->mReceivedResponseBodyLength);
                }
                out << table.toString();
            }
        }
    }

    Log::warn(TRACE_HTTP_SERVICE_THREAD, "%s", out.str().c_str());
}

uint32_t HttpService::addRequest(HttpServiceRequestPtr pServiceRequest)
{
    //Create a request record.
    RequestRecordPtr pRequestRecord(new RequestRecord(allocateId()));

    //Initialize the request record.
    pRequestRecord->mServiceRequestPtr = pServiceRequest;
    pRequestRecord->mTimeoutDuration = pServiceRequest->mTimeout;
    NRDP_OBJECTCOUNT_DESCRIBE(pRequestRecord->mResponseBody, std::string("HTTPSERVICEBODY:")+pRequestRecord->mServiceRequestPtr->mRequestUrl.str());

    //Add to new request map.
    {
        ScopedMutex lock(mMutex);
        mNewRequestMap[pRequestRecord->id()] = pRequestRecord;
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "added an HTTP %s request %u %s, timeout: %llu/%llu/%llums.",
        IAsyncHttpRequest::requestMethodString(pServiceRequest->mRequestMethod),
        pRequestRecord->id(),
        pRequestRecord->mServiceRequestPtr->mRequestUrl.str().c_str(),
        pServiceRequest->mTimeout.ms(),
        pServiceRequest->mConnectTimeout.ms(),
        pServiceRequest->mStallTimeout.ms());

    return pRequestRecord->id();
}

//Cancel an HTTP request.
void HttpService::cancelRequest(
    uint32_t requestId,
    const AseUrl & requestUrl)
{
    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "was cancelling an HTTP request: %u, %s",
        requestId,
        requestUrl.str().c_str());

    //Find the server record for the request.

    ServerMap::iterator itrServerRecord;

    RequestMap::iterator itrRequestRecord;
    const ServerKey key(requestUrl);

    itrServerRecord = mServerMap.find(key);

    if (itrServerRecord != mServerMap.end())
    {
        //Check if this request is pending in the server record.

        itrRequestRecord = itrServerRecord->second->mPendingRequestMap.find(
            requestId);

        if (itrRequestRecord != itrServerRecord->second->mPendingRequestMap.end())
        {
            //Found the request in the pending map.
            itrServerRecord->second->mPendingRequestMap.erase(itrRequestRecord);

            assert(itrServerRecord->second->mTotalRequestCount > 0);

            itrServerRecord->second->mTotalRequestCount--;

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "cancelled the request %u from pending map, total request count: %u.",
            requestId,
                itrServerRecord->second->mTotalRequestCount);
    }
        else
        {
    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "did not find the request %u in the pending map.",
        requestId);

            //Try to find the request in the outstanding request map.
            itrRequestRecord = itrServerRecord->second->mOutstandingRequestMap.find(
                                   requestId);
            //Found the request.
            if (itrRequestRecord != itrServerRecord->second->mOutstandingRequestMap.end())
            {
                itrRequestRecord->second->mCancelledFlag = true;

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "set the request %u to cancelled state.",
            requestId);
    }
            else
            {
    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "did not find the request %u in the outstanding map.",
        requestId);
}
        }
    }
    else
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "did not find the server record for the request: %u",
            requestId);
    }

    return;
}

//Process the HTTP server connections.

void HttpService::processServerConnections(std::map<void *, std::vector<IAsyncHttpConnectionPtr> > const & servers,
                                           AseTimeVal const & timeNow)
{
    for (std::map<void *, std::vector<IAsyncHttpConnectionPtr> >::const_iterator i = servers.begin();
         i != servers.end();
         ++i)
    {
        processServerConnections(
            timeNow,
            (HttpService::ServerRecord *)(i->first),
            i->second);
    }
}


void HttpService::processServerConnections(
    const AseTimeVal & timeNow,
    ServerRecord * pServerRecord,
    vector<IAsyncHttpConnectionPtr> const & serverConnections)
{
    vector<IAsyncHttpConnectionPtr>::const_iterator itrAsyncHttpConnection;

    ConnectionRecord * pConnectionRecord;

    itrAsyncHttpConnection = serverConnections.begin();

    while (itrAsyncHttpConnection != serverConnections.end())
    {
        //Get the server and connection record for the connection.
        pConnectionRecord = (ConnectionRecord *)((*itrAsyncHttpConnection)->getAuxUserData());

        IAsyncHttpConnection::Event httpConnectionEvent;

        httpConnectionEvent = (*itrAsyncHttpConnection)->retrieveOutstandingEvent();

        if (httpConnectionEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT)
        {
            //Process the HTTP connection.
            processConnection(
                timeNow,
                pServerRecord,
                pConnectionRecord);
        }
        else if (httpConnectionEvent == IAsyncHttpConnection::RESPONSE_AVAILABLE_EVENT)
        {
            //Process the HTTP response.
            processResponse(
                timeNow,
                pServerRecord,
                pConnectionRecord);
        }

        itrAsyncHttpConnection++;
    }

    //Check if there are enough consecutive connect failures to this server.
    if (pServerRecord->mConsecutiveConnectFailureCount >= mConnectFailureThreshold)
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "finished the server %s because of too many consecutive connect failures #2, %u %u",
            pServerRecord->mServerName.c_str(),
            pServerRecord->mConsecutiveConnectFailureCount,
            mConnectFailureThreshold);

        finishServer(timeNow, pServerRecord);

            mServerMap.erase(pServerRecord->key());
        }
    else
    {
        //If there are no more requests and no connections are connected, finish the server.
        if (pServerRecord->mTotalRequestCount == 0)
        {
            ConnectionVector::iterator itrConnectionRecord;

            itrConnectionRecord = pServerRecord->mConnectionVector.begin();

            while (itrConnectionRecord != pServerRecord->mConnectionVector.end())
            {
                if ((*itrConnectionRecord)->mAsyncHttpConnectionPtr)
                {
                    break;
                }

                itrConnectionRecord++;
            }

            if (itrConnectionRecord == pServerRecord->mConnectionVector.end())
            {
                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "removed the server %s because of no requests and no connections.",
                    pServerRecord->mServerName.c_str());

                    mServerMap.erase(pServerRecord->key());
            }
        }
    }
}

//Process the HTTP connection.
void HttpService::processConnection(
    const AseTimeVal & timeNow,
    ServerRecord * pServerRecord,
    ConnectionRecord * pConnectionRecord)
{
    if (pConnectionRecord->mAsyncHttpConnectionPtr->getState() == IAsyncHttpConnection::IDLE_STATE)
    {
        //The connection is established.
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "opened the HTTP connection, connection ID: %u.",
            pConnectionRecord->mAsyncHttpConnectionPtr->getHttpId());

        pConnectionRecord->mSentHttpPipeliningRequestCount = 0;
        pConnectionRecord->mCompletedHttpPipeliningRequestCount = 0;
        pConnectionRecord->mNextConnectTime = AseTimeVal::INFINITE;
        pConnectionRecord->mJustConnectedFlag = true;
    }
    else
    {
        //The connection is failed.
        processFailure(
            timeNow,
            pServerRecord,
            pConnectionRecord);
    }
}

//Process the HTTP response.
void HttpService::processResponse(
    const AseTimeVal & timeNow,
    ServerRecord * pServerRecord,
    ConnectionRecord * pConnectionRecord)
{
    IAsyncHttpRequestPtr pAsyncHttpRequest;

    IAsyncHttpRequest::State asyncRequestState;

    RequestRecord * pRequestRecord;

    //Get the triggering request.
    pAsyncHttpRequest = pConnectionRecord->mAsyncHttpConnectionPtr->getTriggeringRequest();

    assert(!pConnectionRecord->mOutstandingRequestList.empty());

    pRequestRecord = pConnectionRecord->mOutstandingRequestList.front().get();

    assert(pAsyncHttpRequest->getId() == pRequestRecord->mAsyncHttpRequestPtr->getId());

    asyncRequestState = pAsyncHttpRequest->getState();

    if (asyncRequestState >= IAsyncHttpRequest::RECEIVING_BODY_STATE)
    {
        int32_t retVal = AS_NO_ERROR;

        if (!pRequestRecord->mCancelledFlag)
        {
            if (pRequestRecord->mFreshFlag)
            {
                //Get the response headers.
                pRequestRecord->mResponseHeaders = pAsyncHttpRequest->getResponseHeaders();

                pRequestRecord->mFreshFlag = false;

#if ENABLE_HST_TRACE
                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "%s %llu %d %u %u %s",
                    HST_TRACE_KEY,
                    (timeNow -  mStartTime).ms(),
                    4,
                    pRequestRecord->id(),
                    pConnectionRecord->mAsyncHttpConnectionPtr->getHttpId(),
                    pServerRecord->mServerName.c_str());
#endif
            }

            if (pAsyncHttpRequest->getResponseBodyLength() != 0)
            {
                uint32_t bytesToReceive = 0;

                if (pAsyncHttpRequest->getResponseBodyLength() > 0)
                {
                    //Check if we have reserved the data buffer space.
                    if (pRequestRecord->mResponseBody.capacity() == 0)
                    {
                        //Reserve the buffer space since the content length is known.
                        pRequestRecord->mResponseBody.reserve(
                            (int) pAsyncHttpRequest->getResponseBodyLength());
                    }

                    bytesToReceive = pRequestRecord->mResponseBody.capacity()
                                     - pRequestRecord->mReceivedResponseBodyLength;

                    assert(bytesToReceive > 0);
                }
                else if (pAsyncHttpRequest->getResponseBodyLength() < 0)
                {
                    //The response body length is not known. Receive in 4K chunks.
                    if ((pRequestRecord->mResponseBody.capacity()
                         - pRequestRecord->mResponseBody.size()) < 4096)
                    {
                        pRequestRecord->mResponseBody.reserve(
                            pRequestRecord->mResponseBody.capacity() + 4096);
                    }

                    bytesToReceive = pRequestRecord->mResponseBody.capacity()
                                     - pRequestRecord->mReceivedResponseBodyLength;

                    assert(bytesToReceive > 0);
                }

                //Get the response body.
                retVal = pAsyncHttpRequest->getResponseBody(
                             pRequestRecord->mResponseBody.data()
                             + pRequestRecord->mReceivedResponseBodyLength,
                             bytesToReceive);

                if ((retVal == AS_NO_ERROR) || (retVal == 1))
                {
                    pRequestRecord->mReceivedResponseBodyLength += bytesToReceive;

                    pRequestRecord->mResponseBody.setUsed(
                        pRequestRecord->mReceivedResponseBodyLength);

                    //If the request has finished, provide back the headers and response body.
                    if (retVal == 1)
                    {
                        pRequestRecord->mResponseBody.reserve(pRequestRecord->mResponseBody.size());

                        pRequestRecord->mServiceRequestPtr->provideResponseHeaders(
                            pRequestRecord->mResponseHeaders);

                        pRequestRecord->mServiceRequestPtr->provideResponseBody(
                            pRequestRecord->mResponseBody);
                    }
                }
            }
            else
            {
                pRequestRecord->mServiceRequestPtr->provideResponseHeaders(
                    pRequestRecord->mResponseHeaders);

                retVal = 1;
            }
        }
        else
        {
            uint32_t bytesDiscarded;

            //This is a request that has been cancelled. Discard the resposne body.
            retVal = pAsyncHttpRequest->discardResponseBody(bytesDiscarded);
        }

        if (retVal == AS_NO_ERROR)
        {
            //no-op
        }
        else if (retVal == 1)
        {
            pRequestRecord->mHttpServiceMetrics.mHttpTransactionTime =
                (uint32_t)(timeNow - pRequestRecord->mAsyncHttpRequestPtr->getFirstByteSentTime()).ms();

            //The request has finished.
            finishHttpRequest(
                timeNow,
                pRequestRecord,
                AS_NO_ERROR,
                pAsyncHttpRequest->getResponseCode());

            if (pRequestRecord->mPipeliningRequestFlag)
            {
                assert(pConnectionRecord->mSentHttpPipeliningRequestCount > 0);

                pConnectionRecord->mCompletedHttpPipeliningRequestCount++;
            }

            if (pServerRecord->mConsecutivePipeliningFailureCount > 0)
            {
                if ((pConnectionRecord->mSentHttpPipeliningRequestCount
                     >= mPipeliningSupportThreshold)
                    && (pConnectionRecord->mCompletedHttpPipeliningRequestCount
                        >= mPipeliningSupportThreshold))
                {
                    NTRACE(
                        TRACE_HTTP_SERVICE_THREAD,
                        "reset the server %s consecutive pipelining failure count.",
                        pServerRecord->mServerName.c_str());

                    pServerRecord->mConsecutivePipeliningFailureCount = 0;
                }
            }

            pServerRecord->mOutstandingRequestMap.erase(pRequestRecord->id());

            assert(pServerRecord->mTotalRequestCount > 0);

            pServerRecord->mTotalRequestCount--;

            if (pServerRecord->mConsecutiveConnectFailureCount > 0)
            {
                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "reset the server %s consecutive connect failure count.",
                    pServerRecord->mServerName.c_str());

                //Reset the server failure count.
                pServerRecord->mConsecutiveConnectFailureCount = 0;
            }

            pConnectionRecord->mOutstandingRequestList.pop_front();
        }
        else
        {
            //The connection is failed.
            processFailure(
                timeNow,
                pServerRecord,
                pConnectionRecord);
        }
    }
}

//Process the HTTP failure.
void HttpService::processFailure(
    const AseTimeVal & timeNow,
    ServerRecord * pServerRecord,
    ConnectionRecord * pConnectionRecord)
{
    //The connection failed.
    if (pConnectionRecord->mAsyncHttpConnectionPtr->didOpen())
    {
        //The connection was actually established, then failed.

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "lost the HTTP connection, connection ID: %u, failure: %d.",
            pConnectionRecord->mAsyncHttpConnectionPtr->getHttpId(),
            pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode());

        RequestList::iterator itrRequestRecord;

        itrRequestRecord = pConnectionRecord->mOutstandingRequestList.begin();

        //For the first request, check the pipelining support.
        if (itrRequestRecord != pConnectionRecord->mOutstandingRequestList.end())
        {
            //Clear the headers and data that may have been received.
            (*itrRequestRecord)->mResponseHeaders.clear();
            (*itrRequestRecord)->mResponseBody.clear();
            (*itrRequestRecord)->mReceivedResponseBodyLength = 0;

            if (((*itrRequestRecord)->mSentHttpPipeliningRequestCount
                 >= mPipeliningSupportThreshold)
                && ((*itrRequestRecord)->mCompletedHttpPipeliningRequestCount
                    < mPipeliningSupportThreshold))
            {
                pServerRecord->mConsecutivePipeliningFailureCount++;

                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "updated the server %s consecutive pipelining failure count %u.",
                    pServerRecord->mServerName.c_str(),
                    pServerRecord->mConsecutivePipeliningFailureCount);

                if (pServerRecord->mConsecutivePipeliningFailureCount >= (2 * mNumberOfConnectionPerServer))
                {
                    //Disable HTTP pipelining by setting the pipelining depth to 1.
                    mPipeliningDepth = 1;

                    NTRACE(
                        TRACE_HTTP_SERVICE_THREAD,
                        "deemed HTTP pipelining not supported %u %u %u.",
                        (*itrRequestRecord)->mSentHttpPipeliningRequestCount,
                        (*itrRequestRecord)->mCompletedHttpPipeliningRequestCount,
                        mPipeliningSupportThreshold);
                }
            }

            pServerRecord->mLastConnectFailureCode = pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode();

            pServerRecord->mConsecutiveConnectFailureCount++;

            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "updated the server %s consecutive connect failure count %u "
                "because it lost the connection (%d).",
                pServerRecord->mServerName.c_str(),
                pServerRecord->mConsecutiveConnectFailureCount, pServerRecord->mLastConnectFailureCode);
        }

        while (itrRequestRecord != pConnectionRecord->mOutstandingRequestList.end())
        {
#if ENABLE_HST_TRACE
            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "%s %llu %d %u",
                HST_TRACE_KEY,
                (timeNow - mStartTime).ms(),
                6,
                (*itrRequestRecord)->id());
#endif

            pServerRecord->mOutstandingRequestMap.erase((*itrRequestRecord)->id());

            if (!(*itrRequestRecord)->mCancelledFlag)
            {
                if ((itrRequestRecord == pConnectionRecord->mOutstandingRequestList.begin())
                    && ((pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode() == AS_HTTP_MESSAGE_LENGTH_ERROR)
                        || (pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode() == AS_HTTP_PROTOCOL_ERROR)))
                {
                    Log::error(
                        TRACE_HTTP_SERVICE_THREAD,
                        "Got protocol error or invalid message length %d %s",
                        pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode(),
                        (*itrRequestRecord)->mServiceRequestPtr->mRequestUrl.str().c_str());

                    finishHttpRequest(
                        timeNow,
                        itrRequestRecord->get(),
                        pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode(),
                        0);
                }
                else
                {
                    const int32_t error = pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode();
                    if (error != AS_HTTP_TRANSACTION_TIMEOUT || (*itrRequestRecord)->mAsyncHttpRequestPtr->getTimeoutTime() > timeNow)
                    {
                        pServerRecord->mPendingRequestMap[(*itrRequestRecord)->id()] = (*itrRequestRecord);

                        NTRACE(
                            TRACE_HTTP_SERVICE_THREAD,
                            "returned the HTTP request %u to pending.",
                            (*itrRequestRecord)->id());
                    }
                    else
                    {
                        NTRACE(
                            TRACE_HTTP_SERVICE_THREAD,
                            "Did not retry request %u due to AS_HTTP_TRANSACTION_TIMEOUT.",
                            (*itrRequestRecord)->id());
                        finishHttpRequest(
                            timeNow,
                            itrRequestRecord->get(),
                            error,
                            0);

                    }
                    //Clear the async HTTP request.
                    (*itrRequestRecord)->mAsyncHttpRequestPtr.reset();
                    (*itrRequestRecord)->mPipeliningRequestFlag = false;
                }
            }
            else
            {
                pServerRecord->mTotalRequestCount--;
            }

            itrRequestRecord++;
        }

        pConnectionRecord->mOutstandingRequestList.clear();
    }
    else
    {
        //The connection failed at connect time, update the count.
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "failed to open the HTTP connection, connection ID: %u, failure: %d.",
            pConnectionRecord->mAsyncHttpConnectionPtr->getHttpId(),
            pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode());

        pServerRecord->mLastConnectFailureCode = pConnectionRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode();

        pServerRecord->mConsecutiveConnectFailureCount++;

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "updated the server %s consecutive connect failure count %u "
            "because it failed to connect #2.",
            pServerRecord->mServerName.c_str(),
            pServerRecord->mConsecutiveConnectFailureCount);
    }

    mAsyncHttpClientPtr->closeConnection(
        pConnectionRecord->mAsyncHttpConnectionPtr);

    pConnectionRecord->mAsyncHttpConnectionPtr.reset();

    //Reopen the connection later if there are still requests.
    if (pServerRecord->mTotalRequestCount > 0)
    {
        pConnectionRecord->mNextConnectTime = timeNow
                                              + AseTimeVal::fromMS(mConnectBackoffTime);
    }
    else
    {
        pConnectionRecord->mNextConnectTime = AseTimeVal::INFINITE;
    }
}

//Process HTTP requests.
void HttpService::processNewRequests(
    const AseTimeVal & timeNow)
{
    RequestMap newRequestMap;
    {
        ScopedMutex lock(mMutex);
        newRequestMap.swap(mNewRequestMap);
    }

    //Put new requests to the server's pending map.
    RequestMap::iterator itrRequestRecord;

    itrRequestRecord = newRequestMap.begin();

    while (itrRequestRecord != newRequestMap.end())
    {
        //Check if there is already a server record for the request.
        ServerMap::iterator itrServerRecord;

        ServerRecordPtr pServerRecord;

        const ServerKey key(itrRequestRecord->second->mServiceRequestPtr->mRequestUrl);

        itrServerRecord = mServerMap.find(key);

        if (itrServerRecord == mServerMap.end())
        {
            //Create the server record.
            pServerRecord.reset(new ServerRecord);
            pServerRecord->mHttpConnectionTimeout = itrRequestRecord->second->mServiceRequestPtr->mConnectTimeout;
            pServerRecord->mHttpStallTimeout = itrRequestRecord->second->mServiceRequestPtr->mStallTimeout;

            assert(pServerRecord);

            //Create the connection records.
            uint32_t count = 0;

            while (count < mNumberOfConnectionPerServer)
            {
                ConnectionRecordPtr pConnectionRecord(new ConnectionRecord);

                assert(pConnectionRecord);

                pServerRecord->mConnectionVector.push_back(pConnectionRecord);

                count++;
            }

            pServerRecord->mServerName = key.host;
            pServerRecord->mServerPort = key.port;

            mServerMap[key] = pServerRecord;
        }
        else
        {
            pServerRecord = itrServerRecord->second;

            if (pServerRecord->mTotalRequestCount == 0)
            {
                ConnectionVector::iterator itrConnectionRecord;

                itrConnectionRecord = pServerRecord->mConnectionVector.begin();

                while (itrConnectionRecord != pServerRecord->mConnectionVector.end())
                {
                    if (!((*itrConnectionRecord)->mAsyncHttpConnectionPtr)
                        && ((*itrConnectionRecord)->mNextConnectTime == AseTimeVal::INFINITE))
                    {
                        (*itrConnectionRecord)->mNextConnectTime = AseTimeVal::ZERO;
                    }

                    itrConnectionRecord++;
                }
            }
        }

#if ENABLE_HST_TRACE
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "%s %llu %d %u",
            HST_TRACE_KEY,
            (timeNow - mStartTime).ms(),
            1,
            itrRequestRecord->second->id());
#endif

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "processed the added HTTP request %u.",
            itrRequestRecord->second->id());

        pServerRecord->mPendingRequestMap[itrRequestRecord->first] = itrRequestRecord->second;

        pServerRecord->mTotalRequestCount++;

        itrRequestRecord++;
    }

    //Schedule HTTP requests for each server.
    ServerMap::iterator itrServerRecord;

    itrServerRecord = mServerMap.begin();

    while (itrServerRecord != mServerMap.end())
    {
        scheduleHttpRequests(
            timeNow,
            itrServerRecord->second.get());

        itrServerRecord++;
    }
}

//Schedule HTTP requests for a serer.
void HttpService::scheduleHttpRequests(
    const AseTimeVal & timeNow,
    ServerRecord * pServerRecord)
{
    RequestMap::iterator itrRequestRecord;
    RequestMap::iterator itrTemp;

    ConnectionVector::iterator itrConnectionRecord;
    ConnectionRecord * pConnectionRecord;

    int32_t retVal;

    itrRequestRecord = pServerRecord->mPendingRequestMap.begin();

    while (itrRequestRecord != pServerRecord->mPendingRequestMap.end())
    {
        //Find the connection that is
        //(1) is connected.
        //(2) is below the pipelining depth.
        //(3) has the least number of outstanding request.
        pConnectionRecord = NULL;

        itrConnectionRecord = pServerRecord->mConnectionVector.begin();

        while (itrConnectionRecord != pServerRecord->mConnectionVector.end())
        {
            if ((*itrConnectionRecord)->mAsyncHttpConnectionPtr)
            {
                if ((*itrConnectionRecord)->mAsyncHttpConnectionPtr->getState()
                    == IAsyncHttpConnection::IDLE_STATE)
                {
                    assert((*itrConnectionRecord)->mOutstandingRequestList.empty());

                    pConnectionRecord = itrConnectionRecord->get();

                    break;
                }
                else if ((*itrConnectionRecord)->mAsyncHttpConnectionPtr->getState()
                         == IAsyncHttpConnection::ACTIVE_STATE)
                {
                    if ((*itrConnectionRecord)->mOutstandingRequestList.size() < mPipeliningDepth)
                    {
                        if (!pConnectionRecord)
                        {
                            pConnectionRecord = itrConnectionRecord->get();
                        }
                        else if (pConnectionRecord->mOutstandingRequestList.size()
                                 > (*itrConnectionRecord)->mOutstandingRequestList.size())

                        {
                            pConnectionRecord = itrConnectionRecord->get();
                        }
                        else
                        {
                            NTRACE(TRACE_HTTP_SERVICE_THREAD, "Not sending. Too many outstanding requests in the connection (%zu/%zu) for %s:%d (%d)",
                                   pConnectionRecord->mOutstandingRequestList.size(),
                                   (*itrConnectionRecord)->mOutstandingRequestList.size(),
                                   pServerRecord->mServerName.c_str(),
                                   pServerRecord->mServerPort,
                                   itrRequestRecord->first);
                        }
                    }
                    else
                    {
                        NTRACE(TRACE_HTTP_SERVICE_THREAD, "Not sending. Too many outstanding requests (%zu/%d) for %s:%d (%d)",
                               (*itrConnectionRecord)->mOutstandingRequestList.size(),
                               mPipeliningDepth,
                               pServerRecord->mServerName.c_str(),
                               pServerRecord->mServerPort,
                               itrRequestRecord->first);
                    }
                }
                else
                {
                    NTRACE(TRACE_HTTP_SERVICE_THREAD, "Not sending. State is %d for %s:%d (%d)",
                           (*itrConnectionRecord)->mAsyncHttpConnectionPtr->getState(),
                           pServerRecord->mServerName.c_str(),
                           pServerRecord->mServerPort,
                           itrRequestRecord->first);
                }
            }
            else
            {
                NTRACE(TRACE_HTTP_SERVICE_THREAD, "Not sending. no mAsyncHttpConnectionPtr for %s:%d (%d)",
                       pServerRecord->mServerName.c_str(),
                       pServerRecord->mServerPort,
                       itrRequestRecord->first);
            }

            itrConnectionRecord++;
        }

        if (pConnectionRecord)
        {
            assert(!itrRequestRecord->second->mAsyncHttpRequestPtr);

            //Send the request.
            retVal = pConnectionRecord->mAsyncHttpConnectionPtr->issueRequest(
                         itrRequestRecord->second->mServiceRequestPtr->mRequestMethod,
                         IAsyncHttpRequest::HTTP_1_1,
                         itrRequestRecord->second->mServiceRequestPtr->mRequestUrl,
                         itrRequestRecord->second->mServiceRequestPtr->mRequestHeaders,
                         itrRequestRecord->second->mServiceRequestPtr->mRequestBody.data(),
                         itrRequestRecord->second->mServiceRequestPtr->mRequestBody.size(),
                         itrRequestRecord->second->mTimeoutDuration,
                         IHttpRequestTraceListenerPtr(),
                         itrRequestRecord->second->mAsyncHttpRequestPtr);

#if ENABLE_HST_TRACE
            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "%s %llu %d %u %u %s",
                HST_TRACE_KEY,
                (timeNow -  mStartTime).ms(),
                (retVal == AS_NO_ERROR) ? 2 : 3,
                itrRequestRecord->second->id(),
                pConnectionRecord->mAsyncHttpConnectionPtr->getHttpId(),
                pServerRecord->mServerName.c_str());
#endif

            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "sent the HTTP request %u on connection %u, ret: %d.",
                itrRequestRecord->first,
                pConnectionRecord->mAsyncHttpConnectionPtr->getHttpId(),
                retVal);

            if (retVal == AS_NO_ERROR)
            {
                //Check if we just sent the first request over this connection.
                if (pConnectionRecord->mJustConnectedFlag)
                {
                    itrRequestRecord->second->mHttpServiceMetrics.mDnsLookupTime =
                        (uint32_t)(pConnectionRecord->mAsyncHttpConnectionPtr->getConnectingTime() - pConnectionRecord->mAsyncHttpConnectionPtr->getStartTime()).ms();

                    itrRequestRecord->second->mHttpServiceMetrics.mHttpConnectTime =
                        (uint32_t)(pConnectionRecord->mAsyncHttpConnectionPtr->getConnectedTime() - pConnectionRecord->mAsyncHttpConnectionPtr->getConnectingTime()).ms();

                    pConnectionRecord->mJustConnectedFlag = false;
                }
                else
                {
                    itrRequestRecord->second->mHttpServiceMetrics.mDnsLookupTime = 0;

                    itrRequestRecord->second->mHttpServiceMetrics.mHttpConnectTime = 0;
                }

                //Sent the request, initialize some variables.
                itrRequestRecord->second->mFreshFlag = true;

                assert(itrRequestRecord->second->mResponseHeaders.empty());
                assert(itrRequestRecord->second->mResponseBody.empty());
                assert(itrRequestRecord->second->mReceivedResponseBodyLength == 0);

                itrRequestRecord->second->mSentHttpPipeliningRequestCount = pConnectionRecord->mSentHttpPipeliningRequestCount;
                itrRequestRecord->second->mCompletedHttpPipeliningRequestCount = pConnectionRecord->mCompletedHttpPipeliningRequestCount;

                if (!(pConnectionRecord->mOutstandingRequestList.empty()))
                {
                    //This is a pipelining request.
                    itrRequestRecord->second->mPipeliningRequestFlag = true;

                    pConnectionRecord->mSentHttpPipeliningRequestCount++;
                }

                //Move the request to the outstanding map.
                pServerRecord->mOutstandingRequestMap[itrRequestRecord->first] = itrRequestRecord->second;

                //Move the request to the connection's outstanding list.
                pConnectionRecord->mOutstandingRequestList.push_back(itrRequestRecord->second);
                ++pConnectionRecord->mAttemptedHttpRequests;
                itrRequestRecord->second->mHttpServiceMetrics.mServerIpAddress = pConnectionRecord->mAsyncHttpConnectionPtr->getServerIpAddress();
                itrRequestRecord->second->mHttpServiceMetrics.mCName = pConnectionRecord->mAsyncHttpConnectionPtr->getCName();

                itrTemp = itrRequestRecord++;

                pServerRecord->mPendingRequestMap.erase(itrTemp);
            }
            else
            {
                //This is in case the request is first one sent on the connection,
                //it is not in the outstanding list.
                if (pConnectionRecord->mOutstandingRequestList.empty())
                {
                    pServerRecord->mConsecutiveConnectFailureCount++;

                    NTRACE(
                        TRACE_HTTP_SERVICE_THREAD,
                        "updated the server %s consecutive connect failure count %u "
                        "because it failed to send.",
                        pServerRecord->mServerName.c_str(),
                        pServerRecord->mConsecutiveConnectFailureCount);
                }

                //Failed to send the request.
                processFailure(
                    timeNow,
                    pServerRecord,
                    pConnectionRecord);
            }
        }
        else
        {
            //There is no connection to send the request.
            break;
        }
    }
}

//Process timeout.
std::vector<HttpService::ServerRecordPtr> HttpService::processTimeout(const AseTimeVal & timeNow)
{
    std::vector<ServerRecordPtr> ret;
    int32_t retVal;

#if !defined(NRDP_LIMIT_SOCKET_COUNT)
    if (timeNow >= earliestConnectTime())
#endif
    {
        ServerMap::iterator itrServerRecord;

        itrServerRecord = mServerMap.begin();

        //For each server.
        while (itrServerRecord != mServerMap.end())
        {
            ConnectionVector::iterator itrConnectionRecord;

            itrConnectionRecord = itrServerRecord->second->mConnectionVector.begin();

            //For each connection.
            while (itrConnectionRecord != itrServerRecord->second->mConnectionVector.end())
            {
                //Check if it is time to open a connection.
                if (timeNow > (*itrConnectionRecord)->mNextConnectTime)
                {
                    assert(!(*itrConnectionRecord)->mAsyncHttpConnectionPtr);

                    //Start to open the connection.
                    retVal = mAsyncHttpClientPtr->openConnection(
                                 itrServerRecord->second->mServerName,
                                 itrServerRecord->second->mServerPort,
                                 itrServerRecord->second->mHttpConnectionTimeout,
                                 itrServerRecord->second->mHttpStallTimeout,
                                 64 * 1024,
                                 NULL,
                                 false,
                                 (*itrConnectionRecord)->mAsyncHttpConnectionPtr);

                    NTRACE(
                        TRACE_HTTP_SERVICE_THREAD,
                        "started to open HTTP connection to %s %d, connection ID: %d, ret: %d",
                        itrServerRecord->second->mServerName.c_str(),
                        itrServerRecord->second->mServerPort,
                        ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS)) ?
                        (int32_t)(*itrConnectionRecord)->mAsyncHttpConnectionPtr->getHttpId() : -1,
                        retVal);

                    if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
                    {
                        (*itrConnectionRecord)->mAsyncHttpConnectionPtr->setUserData(
                            itrServerRecord->second.get());

                        (*itrConnectionRecord)->mAsyncHttpConnectionPtr->setAuxUserData(
                            (*itrConnectionRecord).get());

                        (*itrConnectionRecord)->mNextConnectTime = AseTimeVal::INFINITE;
                    }
                    else
                    {
                        //Failed to open the connection.
                        itrServerRecord->second->mLastConnectFailureCode = retVal;

                        //Update the consecutive connect failure count.
                        itrServerRecord->second->mConsecutiveConnectFailureCount++;

                        NTRACE(
                            TRACE_HTTP_SERVICE_THREAD,
                            "updated the server %s:%d consecutive connect failure count %u "
                            "because it failed to connect #1.",
                            itrServerRecord->first.host.c_str(),
                            itrServerRecord->first.port,
                            itrServerRecord->second->mConsecutiveConnectFailureCount);

                        //Set the next connect time.
                        (*itrConnectionRecord)->mNextConnectTime = timeNow
                                + AseTimeVal::fromMS(mConnectBackoffTime);
                    }
                }

                itrConnectionRecord++;
            }

            //Check if there are enough consecutive connect failures to this server.
            if (itrServerRecord->second->mConsecutiveConnectFailureCount >= mConnectFailureThreshold)
            {
                NTRACE(
                    TRACE_HTTP_SERVICE_THREAD,
                    "finished the server %s:%d because of too many consecutive connect failures #1, %u %u",
                    itrServerRecord->first.host.c_str(),
                    itrServerRecord->first.port,
                    itrServerRecord->second->mConsecutiveConnectFailureCount,
                    mConnectFailureThreshold);
                ret.push_back(itrServerRecord->second);

                mServerMap.erase(itrServerRecord++);
            }
            else
            {
#if defined(NRDP_LIMIT_SOCKET_COUNT)
                bool close = true;
                for (HttpService::ConnectionVector::const_iterator it = itrServerRecord->second->mConnectionVector.begin();
                     it != itrServerRecord->second->mConnectionVector.end(); ++it) {
                    if (!(*it)->mOutstandingRequestList.empty() || (!(*it)->mAttemptedHttpRequests)) {
                        close = false;
                        break;
                    }
                }

                if (close) {
                    ret.push_back(itrServerRecord->second);
                    mServerMap.erase(itrServerRecord++);
                } else {
                    itrServerRecord++;
                }
#else
                    itrServerRecord++;
#endif
            }
        }
    }
    return ret;
}

//Finish all the requests for a server.
void HttpService::finishServer(
    const AseTimeVal & timeNow,
    ServerRecord * pServerRecord)
{
    RequestMap::iterator itrRequestRecord;

    //Close all the connections.
    ConnectionVector::iterator itrConnectionRecord;

    itrConnectionRecord = pServerRecord->mConnectionVector.begin();

    while (itrConnectionRecord != pServerRecord->mConnectionVector.end())
    {
        //Close the connection if it is open.
        if ((*itrConnectionRecord)->mAsyncHttpConnectionPtr)
        {
            mAsyncHttpClientPtr->closeConnection((*itrConnectionRecord)->mAsyncHttpConnectionPtr);
        }

        itrConnectionRecord++;
    }

    //Finish all the outstanding requests.
    itrRequestRecord = pServerRecord->mOutstandingRequestMap.begin();

    while (itrRequestRecord != pServerRecord->mOutstandingRequestMap.end())
    {
        finishHttpRequest(
            timeNow,
            itrRequestRecord->second.get(),
            pServerRecord->mLastConnectFailureCode,
            0);

        itrRequestRecord++;
    }

    //Finish all the pending requests.
    itrRequestRecord = pServerRecord->mPendingRequestMap.begin();

    while (itrRequestRecord != pServerRecord->mPendingRequestMap.end())
    {
        finishHttpRequest(
            timeNow,
            itrRequestRecord->second.get(),
            pServerRecord->mLastConnectFailureCode,
            0,
            false);

        itrRequestRecord++;
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "finished the server %s %d.",
        pServerRecord->mServerName.c_str(),
        pServerRecord->mLastConnectFailureCode);
}


//Finish an HTTP request.
void HttpService::finishHttpRequest(
    const AseTimeVal & timeNow,
    RequestRecord * pRequestRecord,
    int32_t aseErrorCode,
    long httpStatusCode,
    bool traceFlag)
{
    if (!(pRequestRecord->mCancelledFlag))
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "was calling the finish callback %u.",
            pRequestRecord->id());

        pRequestRecord->mServiceRequestPtr->finish(
            aseErrorCode,
            httpStatusCode,
            &(pRequestRecord->mHttpServiceMetrics));

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "called the finish callback %u %d %ld %u %u %u.",
            pRequestRecord->id(),
            aseErrorCode,
            httpStatusCode,
            pRequestRecord->mHttpServiceMetrics.mDnsLookupTime,
            pRequestRecord->mHttpServiceMetrics.mHttpConnectTime,
            pRequestRecord->mHttpServiceMetrics.mHttpTransactionTime);

        //Check if the request is also in the processed waiting map.
        if (mListener)
        {
            mListener->onHttpRequestFinished(pRequestRecord->id());
        }
    }

#if ENABLE_HST_TRACE
    if (traceFlag)
    {
        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "%s %llu %d %u",
            HST_TRACE_KEY,
            (timeNow - mStartTime).ms(),
            (aseErrorCode == AS_NO_ERROR) ? 5 : 6,
            pRequestRecord->id());
    }
#else
    NRDP_UNUSED(traceFlag);
    NRDP_UNUSED(timeNow);
#endif

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "finished the HTTP request %u, cancelled: %s.",
        pRequestRecord->id(),
        pRequestRecord->mCancelledFlag ? "True" : "False");
}

int HttpService::service(std::vector<IAsyncHttpConnectionPtr> & selectedConnections, AseTimeVal const & timeout)
{
    return mAsyncHttpClientPtr->select(selectedConnections, timeout);
}

uint32_t HttpService::allocateId()
{
    // Note: this could be a problem if there are two simultaneous instances -- jjb 10/31/13
    uint32_t id = mIdPool++;
    if (mIdPool == 0x7FFFFFFF)
    {
        mIdPool = 1;
    }

    return id;
}

/********************************************************************************************************************/

/*
 * The HTTP request record class.
 */
HttpService::RequestRecord::RequestRecord(uint32_t id) : mCancelledFlag(false),
    mFreshFlag(true),
    mReceivedResponseBodyLength(0),
    mPipeliningRequestFlag(false),
    mSentHttpPipeliningRequestCount(0),
    mCompletedHttpPipeliningRequestCount(0),
    mId(id)
{
    NRDP_OBJECTCOUNT_REF(HttpServiceRequestRecord, this);
}

HttpService::RequestRecord::~RequestRecord()
{
    NRDP_OBJECTCOUNT_DEREF(HttpServiceRequestRecord, this);
}

/********************************************************************************************************************/

/*
 * The HTTP connection record class.
 */
HttpService::ConnectionRecord::ConnectionRecord()
    : mAttemptedHttpRequests(0),
      mSentHttpPipeliningRequestCount(0),
      mCompletedHttpPipeliningRequestCount(0),
      mNextConnectTime(AseTimeVal::ZERO),
      mJustConnectedFlag(false)
{
    static int sId = 0;
    mId = ++sId;
    NRDP_OBJECTCOUNT_REF(HttpServiceConnectionRecord, this);
}

HttpService::ConnectionRecord::~ConnectionRecord()
{
    NRDP_OBJECTCOUNT_DEREF(HttpServiceConnectionRecord, this);
}

/********************************************************************************************************************/

/*
 * The HTTP server record class.
 */
HttpService::ServerRecord::ServerRecord() : mTotalRequestCount(0),
    mConsecutiveConnectFailureCount(0),
    mLastConnectFailureCode(0),
    mConsecutivePipeliningFailureCount(0)
{
    static int sId = 0;
    mId = ++sId;
    NRDP_OBJECTCOUNT_REF(HttpServiceServerRecord, this);
}

HttpService::ServerRecord::~ServerRecord()
{
    NRDP_OBJECTCOUNT_DEREF(HttpServiceServerRecord, this);
}




}
