/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "HttpServiceThread.h"

#include "HttpService.h"
#include "HttpServiceMetrics.h"
#include "HttpServiceRequest.h"
#include "HttpsService.h"

#include <nrdbase/Log.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/ConditionVariable.h>

#include <unistd.h>

using namespace std;

#define ENABLE_HST_TRACE 0

#define HST_TRACE_KEY "HST_TRACE"

/** HST trace format
 *
 *  HstTraceKey Timestamp TraceType RequestNo [ConnectionNo HostName]
 *
 *  TraceType: 0 - Made the request to HST
 *             1 - Picked the request by HST thread
 *             2 - Issued the request successfully
 *             3 - Failed to issue the request
 *             4 - Finished receiving response header
 *             5 - Completed the reesponse successfully
 *             6 - Failed to complete the response
 */

namespace netflix
{


static bool requestIsHttp(uint32_t id)
{
    return ((id & HttpsService::HTTPS_REQUEST_ID_MASK) == 0);
}


/********************************************************************************************************************/

/*
 * The HTTP service thread class.
 */

//Constructor.
HttpServiceThread::HttpServiceThread(ThreadConfig * pThreadConfig)
    : Thread(pThreadConfig),
      mStartedFlag(false),
      mDoneFlag(false),
      mDumpPending(false),
      mMutex(HTTP_SERVICE_THREAD_LOCK_MUTEX, "HttpServiceThreadLockMutex"),
      mSignalByteCountMutex(HTTP_SERVICE_THREAD_SIGNAL_MUTEX, "HttpServiceThreadSignalMutex"),
      mSignalByteCount(0),
      mHttp(new HttpService),
      mHttps(new HttpsService)
{
}

//Destructor.
HttpServiceThread::~HttpServiceThread()
{
    stop();
    delete mHttp;
    delete mHttps;
}


void HttpServiceThread::scheduleDump()
{
    ScopedMutex lock(mMutex);
    mDumpPending = true;
}

//Set/Get the number of connection per server.
void HttpServiceThread::setNumberOfConnectionPerServer(uint32_t count)
{
#ifdef NRDP_HST_MAX_CONNECTIONS_PER_SERVER
    count = std::min<uint32_t>(count, NRDP_HST_MAX_CONNECTIONS_PER_SERVER);
#endif
    mHttp->mNumberOfConnectionPerServer = count;
}

uint32_t HttpServiceThread::getNumberOfConnectionPerServer() const
{
    return mHttp->mNumberOfConnectionPerServer;
}

//Set/Get the HTTP pipelining depth.
void HttpServiceThread::setHttpPipeliningDepth(uint32_t depth)
{
    mHttp->mPipeliningDepth = depth;
}

uint32_t HttpServiceThread::getHttpPipeliningDepth() const
{
    return mHttp->mPipeliningDepth;
}

//Set/Get the HTTP pipelining support threshold.
void HttpServiceThread::setHttpPipeliningSupportThreshold(uint32_t threshold)
{
    mHttp->mPipeliningSupportThreshold = threshold;
}

uint32_t HttpServiceThread::getHttpPipeliningSupportThreshold() const
{
    return mHttp->mPipeliningSupportThreshold;
}

//Set/Get the HTTP connect backoff time, in ms.
void HttpServiceThread::setHttpConnectBackoffTime(uint32_t time)
{
    mHttp->mConnectBackoffTime = time;
}

uint32_t HttpServiceThread::getHttpConnectBackoffTime() const
{
    return mHttp->mConnectBackoffTime;
}

//Set/Get the HTTP connect failure threshold.
void HttpServiceThread::setHttpConnectFailureThreshold(uint32_t threshold)
{
    mHttp->mConnectFailureThreshold = threshold;
}

uint32_t HttpServiceThread::getHttpConnectFailureThreshold() const
{
    return mHttp->mConnectFailureThreshold;
}

//Start the HTTP service thread.
int32_t HttpServiceThread::start()
{
    ScopedMutex lock(mMutex);
    return startImpl();
}

//Stop the HTTP service thread.
int32_t HttpServiceThread::stop()
{
    bool needToWait = false;

    {
        ScopedMutex lock(mMutex);

        if (mStartedFlag)
        {
            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "was stopping.");

            //Signal the thread to stop.
            mDoneFlag = true;

            mStartedFlag = false;

            sendSignalByte();

            needToWait = true;
        }
    }

    if (needToWait)
    {
        //Wait for the thread to actually stop.
        Wait();

        //Cleanup HTTP request.
        mHttp->cleanUpRequests();

        //Cleanup HTTPs request.
        mHttps->cleanUpRequests();

        mAsyncHttpClientPtr->deinit();
        mHttp->stop();
        mHttps->stop();

        NTRACE(
            TRACE_HTTP_SERVICE_THREAD,
            "stopped.");
    }

    return AS_NO_ERROR;
}

//Add an HTTP request.
uint32_t HttpServiceThread::addHttpRequest(HttpServiceRequestPtr pServiceRequest)
{
    uint32_t requestId = 0;

    ScopedMutex lock(mMutex);

    //Start the service thread if it has not been started.
    if (!mStartedFlag)
    {
        if (startImpl() < 0)
        {
            return 0;   // 0 is an invalid request id
        }
    }

    if (mStartedFlag && (pServiceRequest->mRequestUrl.IsValid()))
    {
        requestId = mHttp->addRequest(pServiceRequest);
        sendSignalByte();
    }

    return requestId;
}

//Add an HTTPs request.
uint32_t HttpServiceThread::addHttpsRequest(
    HttpServiceRequestPtr pServiceRequest,
    TrustStorePtr pTrustStore,
    VerifyCertStatusFunc pVerifyCertStatusFunc,
    bool sslVerifyPeerFlag)
{
    uint32_t requestId = 0;

    ScopedMutex lock(mMutex);

    //Start the service thread if it has not been started.
    if (!mStartedFlag)
    {
        if (startImpl() < 0)
        {
            return 0;   // 0 is an invalid request id
        }
    }

    if (mStartedFlag && (pServiceRequest->mRequestUrl.IsValid()))
    {
        requestId = mHttps->addRequest(pServiceRequest, pTrustStore, pVerifyCertStatusFunc, sslVerifyPeerFlag);
        sendSignalByte();
    }

    return requestId;
}

//Cancel a request.
void HttpServiceThread::cancelRequest(
    uint32_t requestId,
    const AseUrl & requestUrl)
{

    ScopedMutex lock(mMutex);

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "was cancelling a request: 0x%08x, %s",
        requestId,
        requestUrl.str().c_str());

    if (mStartedFlag)
    {
        assert(requestId != 0);

        bool foundFlag = false;

        if (requestIsHttp(requestId))
        {
            //Cancel an HTTP request immediately if it is still pending.
            foundFlag = mHttp->removePendingRequest(requestId);
        }
        else
        {
            //Cancel an HTTPs request immediately if it is still pending.
            foundFlag = mHttps->removePendingRequest(requestId);
        }

        // If it is not pending, then it must be canceled asynchronously
        if (!foundFlag)
        {
            WorkingEntry workingEntry;

            workingEntry.mRequestId = requestId;
            workingEntry.mRequestUrl = requestUrl;
            mCancelledRequests.push_back( workingEntry);

            sendSignalByte();
        }
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "cancelled the request: 0x%08x",
        requestId);
}

//Wait for a request to finish.
bool HttpServiceThread::waitForRequest(
    uint32_t requestId,
    const AseUrl & requestUrl,
    const Time & waitTime)
{
    ScopedMutex lock(mMutex);

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "was waiting for a request: 0x%08x, %s",
        requestId,
        requestUrl.str().c_str());

    bool retVal = false;

    if (mStartedFlag)
    {
        assert(requestId != 0);

        bool foundFlag = false;

        WorkingMap::iterator itrWaitingEntry;

        itrWaitingEntry = mNewWaitingMap.find(requestId);

        if (itrWaitingEntry != mNewWaitingMap.end())
        {
            foundFlag = true;
        }
        else
        {
            itrWaitingEntry = mProcessedWaitingMap.find(requestId);

            if (itrWaitingEntry != mProcessedWaitingMap.end())
            {
                foundFlag = true;
            }
        }

        if (!foundFlag)
        {
            WorkingEntry waitingEntry;

            waitingEntry.mRequestId = requestId;
            waitingEntry.mRequestUrl = requestUrl;

            itrWaitingEntry = mNewWaitingMap.insert(
                                  mNewWaitingMap.begin(),
                                  make_pair(requestId, waitingEntry));
        }

        shared_ptr<ConditionVariable> signal(new ConditionVariable);

        itrWaitingEntry->second.mSignalList.push_back(signal);

        sendSignalByte();

        if (signal->wait(&mMutex, waitTime) == NFErr_OK)
        {
            retVal = true;
        }
        else
        {
            NTRACE(
                TRACE_HTTP_SERVICE_THREAD,
                "waited for a request: 0x%08x, timeout.",
                requestId);
        }
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "waited the request: 0x%08x.",
        requestId);

    return retVal;
}

//The start() implementation. Returns AS_NO_ERROR if success
int32_t HttpServiceThread::startImpl()
{
    if (mDoneFlag || mStartedFlag)
    {
        return AS_NO_ERROR;
    }

    int32_t retVal;

    //Construct the async HTTP client.
    retVal = constructAsyncHttpClient(
                    mAsyncHttpClientPtr,
                    IpConnectivityManager::instance(),
                    IDnsCache::instance(),
                    NULL,
                    NULL,
                    false,
                    1024 * 1024 * 50);

    if (retVal != AS_NO_ERROR)
    {
        return retVal;
    }

    //Initialize the async HTTP client.
    retVal = mAsyncHttpClientPtr->init();

    if (retVal != AS_NO_ERROR)
    {
        mAsyncHttpClientPtr.reset();
        return retVal;
    }

    // Initialize HTTP and HTTPS service
    if (!mHttps->start(this, mAsyncHttpClientPtr) ||
        !mHttp->start(this, mAsyncHttpClientPtr))
    {
        mHttp->stop();
        mHttps->stop();
        mAsyncHttpClientPtr.reset();
        return AS_INIT_FAILURE;
    }

    //Start the thread.
    mStartedFlag = true;
    Thread::Start();
    Thread::WaitRunning();

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "started, with configuration parameters: %u %u %u %u %u.",
        mHttp->mNumberOfConnectionPerServer,
        mHttp->mPipeliningDepth,
        mHttp->mPipeliningSupportThreshold,
        mHttp->mConnectBackoffTime,
        mHttp->mConnectFailureThreshold);

    return AS_NO_ERROR;
}

//Send the signal byte to wake up the thread.
void HttpServiceThread::sendSignalByte()
{
    ScopedMutex lock(mSignalByteCountMutex);

    if (mSignalByteCount == 0)
    {
        mAsyncHttpClientPtr->wake();

        mSignalByteCount++;
    }
}

//The thread procedure.
void HttpServiceThread::Run()
{
    AseTimeVal timeNow = AseTimeVal::now();

    mStartTime = timeNow;

    mHttps->configure();

    while (mStartedFlag)
    {
        PERFORMANCE_MARKER_SCOPED("hst.loop");

        int retVal;
        vector<IAsyncHttpConnectionPtr> selectedConnections;

        //Perform HTTPs operations. (does not block)
        mHttps->service(selectedConnections, timeNow);

        // Perform HTTP operations (blocks)
        {
            PERFORMANCE_MARKER_SCOPED("hst.loop.service");
            retVal = mHttp->service(selectedConnections, computeSelectTimeout());
        }

        if (!mStartedFlag)
        {
            break;
        }

        timeNow = AseTimeVal::now();

        {
            //Process the cancelled requests, this has to be called the first thing
            //after the above select().
            PERFORMANCE_MARKER_SCOPED("hst.loop.cancelled");
            processCancelledEntries(selectedConnections);
        }

        {
            //Process the request waiting entries.
            PERFORMANCE_MARKER_SCOPED("hst.loop.waiting");
            processWaitingEntries();
        }

        //Process selected connections.
        if ((retVal == IAsyncHttpClient::SELECT_CONNECT) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT))
        {
            PERFORMANCE_MARKER_SCOPED("hst.loop.selected");
            processSelectedConnections(
                timeNow,
                selectedConnections);
        }

        //Process signals.
        if ((retVal == IAsyncHttpClient::SELECT_WAKE) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT))
        {
            {
                ScopedMutex signalByteLock(mSignalByteCountMutex);

                assert(mSignalByteCount > 0);

                mSignalByteCount--;
            }

            PERFORMANCE_MARKER_SCOPED("hst.loop.signals");
            retVal = mAsyncHttpClientPtr->receiveSignalByteInternal();

            assert(retVal == AS_NO_ERROR);
        }

        {
            //Process HTTP requests.
            PERFORMANCE_MARKER_SCOPED("hst.loop.http");
            mHttp->processNewRequests(timeNow);
        }
        {
            //Process HTTPs requests.
            PERFORMANCE_MARKER_SCOPED("hst.loop.https");
            mHttps->processNewRequests(timeNow);
        }
        bool dump = false;
        std::vector<HttpService::ServerRecordPtr> finishedServers;
        {
            //Process timeout.
            PERFORMANCE_MARKER_SCOPED("hst.loop.timeout");
            finishedServers = mHttp->processTimeout(timeNow);
            std::swap(mDumpPending, dump);
        }
        for (std::vector<HttpService::ServerRecordPtr>::const_iterator it = finishedServers.begin(); it != finishedServers.end(); ++it) {
            mHttp->finishServer(timeNow, it->get());
        }

        if (dump)
            mHttp->dump();
    }

}

//Compute the select timeout.
AseTimeVal HttpServiceThread::computeSelectTimeout()
{
    AseTimeVal timeNow = AseTimeVal::now();

    mSelectTimeout = AseTimeVal::INFINITE;

    //Find out the HTTPs multi handle timeout.
    long httpsMaxTime = mHttps->getTimeout();

    //Find out the earliest connect time.
    mSelectTimeout = mHttp->findEarliestConnectTime(httpsMaxTime, timeNow);

    return mSelectTimeout;
}

//Process the cancelled entries.
void HttpServiceThread::processCancelledEntries(
    std::vector<IAsyncHttpConnectionPtr>& selectedConnections)
{
    ScopedMutex lock(mMutex);


    for (CancelledRequestList::iterator i = mCancelledRequests.begin(); i != mCancelledRequests.end(); ++i)
    {
        uint32_t id = i->mRequestId;
        AseUrl const & url = i->mRequestUrl;
        if (requestIsHttp(id))
        {
            //Cancel an HTTP request.
            mHttp->cancelRequest(id, url);
        }
        else
        {
            //Cancel an HTTPs request.
            mHttps->cancelRequest(id, url, selectedConnections);
        }
    }

    mCancelledRequests.clear();

}
//Process the waiting entries.
void HttpServiceThread::processWaitingEntries()
{
    ScopedMutex lock(mMutex);

    //Check each entry in the new waiting map, if there is a request can be found,
    //move the entry to the processed waiting map. Otherwise, signal here.

    bool foundFlag = false;

    WorkingMap::iterator itrWorkingEntry;

    itrWorkingEntry = mNewWaitingMap.begin();

    while (itrWorkingEntry != mNewWaitingMap.end())
    {
        foundFlag = false;

        WorkingEntry & working = itrWorkingEntry->second;
        uint32_t id = working.mRequestId;

        if (requestIsHttp(id))
        {
            //Try to find the new waiting entry in request maps.

            if (mHttp->isPending(id) || mHttp->isActive(id, working.mRequestUrl))
            {
                foundFlag = true;
            }
        }
        else
        {
            if (mHttps->isPending(id) || mHttps->isActive(id))
            {
                foundFlag = true;
            }
        }

        if (foundFlag)
        {
            //If the new waiting entry is found, move it to the processed map.
            mProcessedWaitingMap[itrWorkingEntry->first] = working;
        }
        else
        {
            //The new waiting entry is not found, signal here.
            working.signal();
        }

        itrWorkingEntry++;
    }

    mNewWaitingMap.clear();
}

void HttpServiceThread::onHttpRequestFinished(int requestId)
{
    WorkingMap::iterator entry = mProcessedWaitingMap.find(requestId);

    if (entry != mProcessedWaitingMap.end())
    {
        entry->second.signal();

        mProcessedWaitingMap.erase(entry);
    }
}

void HttpServiceThread::onHttpsRequestFinished(int requestId)
{
    WorkingMap::iterator entry = mProcessedWaitingMap.find(requestId);

    if (entry != mProcessedWaitingMap.end())
    {
        entry->second.signal();

        mProcessedWaitingMap.erase(entry);
    }
}

//Process the selected connectioins.
void HttpServiceThread::processSelectedConnections(
    const AseTimeVal & timeNow,
    vector<IAsyncHttpConnectionPtr>& selectedConnections)
{
    map<void *, vector<IAsyncHttpConnectionPtr> > serverConnectionVectorMap;

    vector<IAsyncHttpConnectionPtr> httpsConnections;

    vector<IAsyncHttpConnectionPtr>::iterator itrAsyncHttpConnection;

    itrAsyncHttpConnection = selectedConnections.begin();

    while (itrAsyncHttpConnection != selectedConnections.end())
    {
        //Separate HTTP and HTTPs connections. The HTTP connections have both user data and aux
        //data set. The HTTPs connections only have user data set.
        if (((*itrAsyncHttpConnection)->getUserData()) &&
            ((*itrAsyncHttpConnection)->getAuxUserData()))
        {
            //Group the selected connections based on the server.
            serverConnectionVectorMap[(*itrAsyncHttpConnection)->getUserData()].push_back(
                *itrAsyncHttpConnection);
        }
        else if ((*itrAsyncHttpConnection)->getUserData())
        {
            //Set HTTPs connection to the vector.
            httpsConnections.push_back(*itrAsyncHttpConnection);
        }
        else
        {
            assert(false);
        }

        itrAsyncHttpConnection++;
    }

    //Process HTTP server connections.
    mHttp->processServerConnections(serverConnectionVectorMap, timeNow);

    //Process HTTPs connection.
    mHttps->processConnections(httpsConnections, timeNow);
}

//Signal the working entry.
void HttpServiceThread::WorkingEntry::signal()
{
    for (ConditionVariableList::iterator i = mSignalList.begin(); i != mSignalList.end(); ++i)
    {
        shared_ptr<ConditionVariable> signal = i->lock();
        if (signal)
        {
            signal->signal();
        }
    }

    NTRACE(
        TRACE_HTTP_SERVICE_THREAD,
        "signaled the request: 0x%08x, %s",
        mRequestId,
        mRequestUrl.str().c_str());
}

}
