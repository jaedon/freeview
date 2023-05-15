/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright nd other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "CurlMultiThread.h"
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Log.h>
#include <nrdnet/IpConnectivityManager.h>
#include <nrdnet/IDnsCache.h>
#include <nrdnet/NrdIpAddr.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

using namespace netflix;

#define HTTP_CONNECTIONS_MAX 10
#define SSL_CONNECTIONS_MAX 6

CurlMultiThread::CurlMultiThread(ThreadConfig *config)
    : Thread(config),
      mRequestIdPool(1),
      mStartedFlag(false),
      mDone(false),
      mCurlShare(0),
      mCurlShareLock(UNTRACKED_MUTEX, "CurlMultiThread"),
      mHttpMulti(0),
      mSslMulti(0),
      mMutex(CURLMULTITHREAD_MUTEX, "CurlMultiThread"),
      mHttpConnectionCount(0),
      mSslConnectionCount(0),
      mSignalByteCountMutex(CURLMULTITHREAD_SIGNAL_BYTE_MUTEX, "CurlMultiThreadByteCount"),
      mSignalByteCount(0)
{
}

CurlMultiThread::~CurlMultiThread()
{
    stop();
}

void CurlMultiThread::stop()
{
    bool startedFlag;

    {
        ScopedMutex lock(mMutex);
        if (mDone)
            return;
        startedFlag = mStartedFlag;
        mDone = true;
    }

    NTRACE(TRACE_CURL_MULTI_THREAD, "shutting down, started flag: %d", startedFlag);

    if (startedFlag) {
        assert(mAsyncHttpClientPtr);

        {
            ScopedMutex signalByteLock(mSignalByteCountMutex);
            if (mSignalByteCount == 0) {
                mAsyncHttpClientPtr->wake();

                mSignalByteCount++;
            }
        }

        Wait();

        std::map<unsigned int, Node *>::iterator itrNode;

        itrNode = mNodes.begin();

        while (itrNode != mNodes.end()) {
            NTRACE(TRACE_CURL_MULTI_THREAD, "cleaning up request: %u", itrNode->first);

            if (itrNode->second->mAsyncHttpConnectionPtr) {
                mAsyncHttpClientPtr->closeConnection(itrNode->second->mAsyncHttpConnectionPtr);
            }

            if (itrNode->second->mWorkingCurl) {
                curl_easy_cleanup(itrNode->second->mWorkingCurl);

                itrNode->second->mWorkingCurl = NULL;
            }

            if (itrNode->second->mSocketHandle != -1) {
                ::close(itrNode->second->mSocketHandle);
            }

            if (itrNode->second->mDnsRecordPtr) {
                curl_slist_free_all(itrNode->second->mDnsRecordPtr);
            }

            delete itrNode->second;

            itrNode++;
        }

        mAsyncHttpClientPtr->deinit();
    }
}

uint32_t CurlMultiThread::add(CURL *curl, void (*finishedCallback)(long, CURLcode, void *), void *userData)
{
    assert(curl);

    ScopedMutex lock(mMutex);

    if (!mStartedFlag) {
        int32_t retVal;

        retVal = constructAsyncHttpClient(mAsyncHttpClientPtr, IpConnectivityManager::instance(), IDnsCache::instance(), NULL, NULL);

        if (retVal == AS_NO_ERROR) {
            retVal = mAsyncHttpClientPtr->init();

            if (retVal == AS_NO_ERROR) {
                Thread::Start();
                Thread::WaitRunning();
                mStartedFlag = true;
                NTRACE(TRACE_CURL_MULTI_THREAD, "Started.");
            } else {
                mAsyncHttpClientPtr.reset();
            }
        }
    }

    unsigned int requestId = 0;

    if (mStartedFlag) {
        requestId = mRequestIdPool++;
        assert(!mDone);
        Node *pNode = new Node;
        assert(pNode);
        char *pUrl = NULL;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &pUrl);
        NTRACE(TRACE_CURL_MULTI_THREAD, "added a request, request ID: %u CURL: %p (%s)", requestId, curl, pUrl);
        pNode->mState = Added;
        pNode->mFinishedCallback = finishedCallback;
        pNode->mUserData = userData;
        pNode->mOriginalCurl = curl;
        pNode->mUrl = pUrl;
        pNode->mDnsRecordPtr = NULL;

#if LIBCURL_VERSION_NUM >= 0x071d00
        if (pNode->mUrl.IsValid()) {
            std::string hostPortAddrStr = pNode->mUrl.GetHost();
            char tempPortNumber[32];
            snprintf(tempPortNumber, 32, ":%d", pNode->mUrl.GetPortNumber());
            hostPortAddrStr += tempPortNumber;
            hostPortAddrStr += ":127.0.0.1";
            pNode->mDnsRecordPtr = curl_slist_append(pNode->mDnsRecordPtr, hostPortAddrStr.c_str());
            NTRACE(TRACE_CURL_MULTI_THREAD, "set the fake DNS record 0: %s", hostPortAddrStr.c_str());
        }
#endif

        mNodes[requestId] = pNode;

        {
            ScopedMutex signalByteLock(mSignalByteCountMutex);
            if (mSignalByteCount == 0) {
                mAsyncHttpClientPtr->wake();

                mSignalByteCount++;
            }
        }
    }

    return requestId;
}

void CurlMultiThread::cancel(unsigned int requestId)
{
    Node *pNode;
    NTRACE(TRACE_CURL_MULTI_THREAD, "cancelled a request: %u, locking mutex", requestId);
    bool freshRequest = false;
    {
        ScopedMutex lock(mMutex);
        if (mDone) {
            return;
        }

        std::map<unsigned int, Node *>::iterator itrNode;
        itrNode = mNodes.find(requestId);
        if (itrNode == mNodes.end()) {
            NTRACE(TRACE_CURL_MULTI_THREAD, "cancelled a request %u, but it is not found", requestId);
            return;
        }

        pNode = itrNode->second;
        NTRACE(TRACE_CURL_MULTI_THREAD, "cancelled a request: %u, state: %d", requestId, pNode->mState);
        if (pNode->mState == Added) {
            mNodes.erase(itrNode);
            freshRequest = true;
        } else if (pNode->mState != Finished) {
            pNode->mCancelledFlag = true;
            shared_ptr<ConditionVariable> cond(new ConditionVariable);
            pNode->mConditionVariables.push_back(cond);
            {
                ScopedMutex signalByteLock(mSignalByteCountMutex);
                if (mSignalByteCount == 0) {
                    mAsyncHttpClientPtr->wake();
                    mSignalByteCount++;
                }
            }

            cond->wait(&mMutex);
        }
    }

    NTRACE(TRACE_CURL_MULTI_THREAD, "cancelled a request: %u, done.", requestId);

    if (freshRequest) {
        finish(0, pNode, CURLE_ABORTED_BY_CALLBACK);

        if (pNode->mDnsRecordPtr) {
            curl_slist_free_all(pNode->mDnsRecordPtr);
        }

        delete pNode;
    }
}

bool CurlMultiThread::waitFor(unsigned int requestId, const Time &max)
{
    NTRACE(TRACE_CURL_MULTI_THREAD, "waited for a request: %u, locking mutex", requestId);
    {
        ScopedMutex lock(mMutex);
        assert(!mDone);
        NTRACE(TRACE_CURL_MULTI_THREAD, "waited for a request: %u, locked mutex", requestId);
        std::map<unsigned int, Node *>::iterator itrNode;
        itrNode = mNodes.find(requestId);
        if (itrNode == mNodes.end()) {
            NTRACE(TRACE_CURL_MULTI_THREAD, "waited for a request: %u, not found.", requestId);
            return false;
        }

        shared_ptr<ConditionVariable> cond(new ConditionVariable);
        itrNode->second->mConditionVariables.push_back(cond);
        if (cond->wait(&mMutex, max) != NFErr_OK) {
            NTRACE(TRACE_CURL_MULTI_THREAD, "waited for a request: %u, timeout.", requestId);
            return false;
        }
    }

    NTRACE(TRACE_CURL_MULTI_THREAD, "waited for a request: %u, done.", requestId);

    return true;
}

CURLcode CurlMultiThread::perform(CURL *curl, const Time &max)
{
    CURLcode result = CURLE_FAILED_INIT;
    unsigned int requestId;
    requestId = add(curl, performFinishedCallback, &result);
    if (requestId > 0) {
        if (!waitFor(requestId, max)) {
            cancel(requestId);
        }
    }

    return result;
}

void CurlMultiThread::performFinishedCallback(long, CURLcode code, void *data) { *((CURLcode *)data) = code; }

void CurlMultiThread::Run()
{
    assert(mAsyncHttpClientPtr);
    mCurlShare = curl_share_init();
    if (mCurlShare) {
        curl_share_setopt(mCurlShare, CURLSHOPT_LOCKFUNC, curlShareLockCallback);
        curl_share_setopt(mCurlShare, CURLSHOPT_UNLOCKFUNC, curlShareUnlockCallback);
        curl_share_setopt(mCurlShare, CURLSHOPT_USERDATA, this);
        curl_share_setopt(mCurlShare, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    }

    CURLMcode code;
    mHttpMulti = curl_multi_init();
    curl_multi_setopt(mHttpMulti, CURLMOPT_MAXCONNECTS, HTTP_CONNECTIONS_MAX);
    curl_multi_setopt(mHttpMulti, CURLMOPT_PIPELINING, 0);
    mSslMulti = curl_multi_init();
    curl_multi_setopt(mSslMulti, CURLMOPT_MAXCONNECTS, SSL_CONNECTIONS_MAX);
    curl_multi_setopt(mSslMulti, CURLMOPT_PIPELINING, 0);

    while (true) {
        PERFORMANCE_MARKER_SCOPED("curlmultithread.loop");
        bool done = false;
        performCurlMulti(mHttpMulti, done);
        if (done) {
            break;
        }

        performCurlMulti(mSslMulti, done);

        if (done) {
            break;
        }

        fd_set read, write, error;

        FD_ZERO(&read);
        FD_ZERO(&write);
        FD_ZERO(&error);

        AseTimeVal timeout;

        long httpMaxTime = -1;
        long sslMaxTime = -1;

        curl_multi_timeout(mHttpMulti, &httpMaxTime);
        curl_multi_timeout(mSslMulti, &sslMaxTime);

        if ((httpMaxTime >= 0) && (sslMaxTime >= 0)) {
            if (httpMaxTime < sslMaxTime) {
                timeout = AseTimeVal::fromMS(httpMaxTime);
            } else {
                timeout = AseTimeVal::fromMS(sslMaxTime);
            }
        } else if (httpMaxTime >= 0) {
            timeout = AseTimeVal::fromMS(httpMaxTime);
        } else if (sslMaxTime >= 0) {
            timeout = AseTimeVal::fromMS(sslMaxTime);
        } else {
            timeout = AseTimeVal::fromMS(1000);
        }

        int httpMaxFd = -1;
        int sslMaxFd = -1;

        code = curl_multi_fdset(mHttpMulti, &read, &write, &error, &httpMaxFd);

        if (code != CURLM_OK) {
            Log::error(TRACE_CURL_MULTI_THREAD, "Got curl error from HTTP curl_multi_fdset %d", code);
        }

        code = curl_multi_fdset(mSslMulti, &read, &write, &error, &sslMaxFd);

        if (code != CURLM_OK) {
            Log::error(TRACE_CURL_MULTI_THREAD, "Got curl error from SSL curl_multi_fdset %d", code);
        }

        int32_t maxFd;

        maxFd = httpMaxFd;

        if (sslMaxFd > maxFd) {
            maxFd = sslMaxFd;
        }

        mAsyncHttpClientPtr->setExternalFdSets(maxFd, &read, &write);

        int retVal;
        std::vector<IAsyncHttpConnectionPtr> selectedHttpConnections;

        retVal = mAsyncHttpClientPtr->select(selectedHttpConnections, timeout);

        if ((retVal == IAsyncHttpClient::SELECT_CONNECT) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT)) {
            std::vector<IAsyncHttpConnectionPtr>::iterator itrAsyncHttpConnection;

            IAsyncHttpConnection::Event outstandingEvent;

            for (itrAsyncHttpConnection = selectedHttpConnections.begin();
                 itrAsyncHttpConnection != selectedHttpConnections.end();
                 ++itrAsyncHttpConnection) {
                outstandingEvent = (*itrAsyncHttpConnection)->retrieveOutstandingEvent();

                assert(outstandingEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT);
                NRDP_UNUSED(outstandingEvent);

                void *ptr = (*itrAsyncHttpConnection)->getUserData();
                const uintptr_t val = reinterpret_cast<uintptr_t>(ptr);
                unsigned int requestId = static_cast<unsigned int>(val);
                CURLcode curlCode = CURLE_OK;
                std::map<unsigned int, Node *>::iterator it;
                Node *pNode = NULL;
                {
                    ScopedMutex lock(mMutex);
                    it = mNodes.find(requestId);
                    assert(it != mNodes.end());
                    pNode = it->second;
                    if ((*itrAsyncHttpConnection)->getState() == IAsyncHttpConnection::IDLE_STATE) {
                        NTRACE(TRACE_CURL_MULTI_THREAD,
                               "Request: %u successfully opened the connection from %s %d to %s.",
                               requestId,
                               (*itrAsyncHttpConnection)->getClientIpAddress().getStrAddr().c_str(),
                               (*itrAsyncHttpConnection)->getClientPortNumber(),
                               (*itrAsyncHttpConnection)->getServerIpAddress().getStrAddr().c_str());

                        pNode->mSocketHandle = (*itrAsyncHttpConnection)->transferSocketHandle();
                        mAsyncHttpClientPtr->closeConnection(*itrAsyncHttpConnection);
                        pNode->mAsyncHttpConnectionPtr.reset();
                        pNode->mWorkingCurl = curl_easy_duphandle(pNode->mOriginalCurl);
#if LIBCURL_VERSION_NUM >= 0x071d00
                        curl_easy_setopt(pNode->mWorkingCurl, CURLOPT_RESOLVE, pNode->mDnsRecordPtr);
#endif
                        if (pNode->mUrl.IsHttps()) {
                            curl_multi_add_handle(mSslMulti, pNode->mWorkingCurl);
                        } else {
                            curl_multi_add_handle(mHttpMulti, pNode->mWorkingCurl);
                        }

                        pNode->mState = Running;
                        pNode = NULL;
                    } else {
                        NTRACE(TRACE_CURL_MULTI_THREAD,
                               "Request: %u failed to open the connection %d.",
                               requestId,
                               (*itrAsyncHttpConnection)->getConnectionFailureCode());

                        curlCode = convertAseErrorCodeToCurlCode((*itrAsyncHttpConnection)->getConnectionFailureCode());

                        if (pNode->mUrl.IsHttps()) {
                            assert(mSslConnectionCount >= 1);
                            --mSslConnectionCount;
                        } else {
                            assert(mHttpConnectionCount >= 1);
                            --mHttpConnectionCount;
                        }

                        mAsyncHttpClientPtr->closeConnection(*itrAsyncHttpConnection);
                        pNode->mAsyncHttpConnectionPtr.reset();
                        pNode->mState = Finished;
                    }
                }

                if (pNode) {
                    if (pNode->mFinishedCallback) {
                        NTRACE(TRACE_CURL_MULTI_THREAD, "Request: %u calling the call back 1", requestId);
                        pNode->mFinishedCallback(0, curlCode, pNode->mUserData);
                        NTRACE(TRACE_CURL_MULTI_THREAD, "Request: %u called the call back 1", requestId);
                    }

                    {
                        ScopedMutex lock(mMutex);
                        for (std::list<weak_ptr<ConditionVariable> >::iterator i = pNode->mConditionVariables.begin();
                             i != pNode->mConditionVariables.end(); ++i) {
                            if (shared_ptr<ConditionVariable> condition = (*i).lock()) {
                                condition->signal();
                            }
                        }

                        NTRACE(TRACE_CURL_MULTI_THREAD, "Request: %u cleaned up the node, 1", requestId);

                        if (pNode->mDnsRecordPtr) {
                            curl_slist_free_all(pNode->mDnsRecordPtr);
                        }

                        delete pNode;

                        mNodes.erase(it);
                    }
                }
            }
        }

        if ((retVal == IAsyncHttpClient::SELECT_WAKE) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT)) {
            {
                ScopedMutex signalByteLock(mSignalByteCountMutex);
                assert(mSignalByteCount > 0);
                mSignalByteCount--;
            }

            retVal = mAsyncHttpClientPtr->receiveSignalByteInternal();

            if (retVal != AS_NO_ERROR) {
                Log::error(TRACE_CURL_MULTI_THREAD, "Can't read from pipe: %s", strerror(errno));
            }
        }
        {
            std::map<unsigned int, Node *> finishedNodes;

            {
                ScopedMutex lock(mMutex);

                if (mDone) {
                    break;
                }

                for (std::map<unsigned int, Node *>::iterator it = mNodes.begin(); it != mNodes.end();) {
                    Node *pNode = it->second;

                    switch (pNode->mState) {
                    case Added: {
                        if (pNode->mUrl.IsHttps()) {
                            if (mSslConnectionCount >= SSL_CONNECTIONS_MAX) {
                                ++it;
                                break;
                            }
                        } else {
                            if (mHttpConnectionCount >= HTTP_CONNECTIONS_MAX) {
                                ++it;
                                break;
                            }
                        }

                        // curl_easy_setopt(
                        //             pNode->mOriginalCurl,
                        //             CURLOPT_VERBOSE,
                        //             1 );

                        curl_easy_setopt(pNode->mOriginalCurl, CURLOPT_PROXY, "");
                        curl_easy_setopt(pNode->mOriginalCurl, CURLOPT_OPENSOCKETFUNCTION, CurlMultiThread::openSocketCallback);
                        curl_easy_setopt(pNode->mOriginalCurl, CURLOPT_OPENSOCKETDATA, it->second);
                        curl_easy_setopt(pNode->mOriginalCurl, CURLOPT_SOCKOPTFUNCTION, CurlMultiThread::socketOptCallback);
                        pNode->mWorkingCurl = curl_easy_duphandle(pNode->mOriginalCurl);

#if LIBCURL_VERSION_NUM >= 0x071d00
                        curl_easy_setopt(pNode->mWorkingCurl, CURLOPT_RESOLVE, pNode->mDnsRecordPtr);
#endif
                        if (mCurlShare) {
                            curl_easy_setopt(pNode->mWorkingCurl, CURLOPT_SHARE, mCurlShare);
                        }

                        NTRACE(TRACE_CURL_MULTI_THREAD,
                               "Request: %u CURL: %p added a new request, working CURL: %p, URL: %s",
                               it->first,
                               pNode->mOriginalCurl,
                               pNode->mWorkingCurl,
                               pNode->mUrl.str().c_str());

                        if (pNode->mUrl.IsHttps()) {
                            ++mSslConnectionCount;
                            curl_multi_add_handle(mSslMulti, pNode->mWorkingCurl);
                        } else {
                            ++mHttpConnectionCount;
                            curl_multi_add_handle(mHttpMulti, pNode->mWorkingCurl);
                        }

                        pNode->mState = Checking;

                        it++;

                        break;
                    }

                    case Connecting: {
                        if (pNode->mCancelledFlag) {
                            if (pNode->mUrl.IsHttps()) {
                                assert(mSslConnectionCount >= 1);
                                --mSslConnectionCount;
                            } else {
                                assert(mHttpConnectionCount >= 1);
                                --mHttpConnectionCount;
                            }

                            mAsyncHttpClientPtr->closeConnection(pNode->mAsyncHttpConnectionPtr);

                            pNode->mAsyncHttpConnectionPtr.reset();

                            pNode->mCurlResult = CURLE_ABORTED_BY_CALLBACK;

                            pNode->mState = Finished;

                            finishedNodes[it->first] = it->second;

                            mNodes.erase(it++);
                        } else {
                            it++;
                        }

                        break;
                    }

                    case Checking:
                    case Running: {
                        if (pNode->mCancelledFlag) {
                            if (pNode->mUrl.IsHttps()) {
                                assert(mSslConnectionCount >= 1);
                                --mSslConnectionCount;

                                curl_multi_remove_handle(mSslMulti, pNode->mWorkingCurl);
                            } else {
                                assert(mHttpConnectionCount >= 1);
                                --mHttpConnectionCount;

                                curl_multi_remove_handle(mHttpMulti, pNode->mWorkingCurl);
                            }

                            curl_easy_cleanup(pNode->mWorkingCurl);

                            pNode->mWorkingCurl = NULL;

                            pNode->mCurlResult = CURLE_ABORTED_BY_CALLBACK;

                            pNode->mState = Finished;

                            finishedNodes[it->first] = it->second;

                            if (pNode->mSocketHandle != CURL_SOCKET_BAD) {
                                ::close(pNode->mSocketHandle);
                            }

                            mNodes.erase(it++);
                        } else {
                            it++;
                        }

                        break;
                    }

                    case Checked:
                    default: {
                        it++;
                    }
                    }
                }
            }

            for (std::map<unsigned int, Node *>::iterator it = finishedNodes.begin(); it != finishedNodes.end(); it++) {
                NTRACE(TRACE_CURL_MULTI_THREAD,
                       "Request: %u CURL: %p aborted the request, %s\n",
                       it->first,
                       it->second->mOriginalCurl,
                       curl_easy_strerror(it->second->mCurlResult));

                finish(0, it->second, it->second->mCurlResult);

                if (it->second->mDnsRecordPtr) {
                    curl_slist_free_all(it->second->mDnsRecordPtr);
                }

                delete it->second;
            }
        }
    }

    CURL *easyDumy = curl_easy_init();

    curl_multi_add_handle(mHttpMulti, easyDumy);

    curl_multi_cleanup(mHttpMulti);
    mHttpMulti = 0;

    curl_easy_cleanup(easyDumy);

    easyDumy = curl_easy_init();

    curl_multi_add_handle(mSslMulti, easyDumy);

    curl_multi_cleanup(mSslMulti);
    mSslMulti = 0;

    curl_easy_cleanup(easyDumy);

    if (mCurlShare) {
        curl_share_cleanup(mCurlShare);

        mCurlShare = NULL;
    }
}

void CurlMultiThread::Start() {}

void CurlMultiThread::finish(long httpStatus, Node *pNode, CURLcode result)
{

    if (pNode->mFinishedCallback) {
        pNode->mFinishedCallback(httpStatus, result, pNode->mUserData);
    }

    for (std::list<weak_ptr<ConditionVariable> >::iterator i = pNode->mConditionVariables.begin(); i != pNode->mConditionVariables.end();
         ++i) {
        if (shared_ptr<ConditionVariable> condition = (*i).lock()) {
            condition->signal();
        }
    }
}

CURLcode CurlMultiThread::convertAseErrorCodeToCurlCode(int32_t aseErrorCode)
{
    CURLcode retCode;

    switch (aseErrorCode) {
    case AS_SOCKET_ERROR:
    case AS_SELECT_ERROR: {
        retCode = CURLE_FAILED_INIT;
        break;
    }

    case AS_DNS_ERROR:
    case AS_DNS_TIMEOUT:
    case AS_DNS_NOT_FOUND: {
        retCode = CURLE_COULDNT_RESOLVE_HOST;
        break;
    }

    case AS_CONNECTION_NOT_OPEN:
    case AS_CONNECTION_ERROR:
    case AS_CONNECTION_TIMEOUT:
    case AS_CONNECTION_REFUSED:
    case AS_CONNECTION_NET_UNREACHABLE:
    case AS_CONNECTION_RESET:
    case AS_CONNECTION_CLOSED: {
        retCode = CURLE_COULDNT_CONNECT;
        break;
    }

    case AS_DNS_NO_DATA:
    case AS_DNS_QUERY_MALFORMED:
    case AS_DNS_SERVER_FAILURE:
    case AS_DNS_NOT_IMPLEMENTED:
    case AS_DNS_QUERY_REFUSED:
    case AS_DNS_BAD_NAME:
    case AS_DNS_CONNECTION_REFUSED:
    case AS_DNS_NO_MEMORY:
    case AS_DNS_CHANNEL_DESTROYED:
    case AS_DNS_RESPONSE_MALFORMED: {
        retCode = CURLE_COULDNT_RESOLVE_HOST;
        break;
    }

    case AS_CONNECTION_INVALID_DESTINATION:
    default: {
        retCode = CURLE_COULDNT_CONNECT;
        break;
    }
    }

    return retCode;
}

void CurlMultiThread::performCurlMulti(CURLM *pCurlMulti, bool &doneFlag)
{
    CURLMcode code;
    int stillRunning = 0;

    do {
        code = curl_multi_perform(pCurlMulti, &stillRunning);

        {
            ScopedMutex lock(mMutex);

            doneFlag = mDone;

            if (doneFlag) {
                break;
            }
        }

    } while (code == CURLM_CALL_MULTI_PERFORM);

    if (doneFlag) {
        return;
    }

    if (code != CURLM_OK) {
        Log::error(TRACE_CURL_MULTI_THREAD, "Got curl error from curl_multi_perform %d", code);
    }

    while (true) {
        int msgsInQueue = 0;
        CURLMsg *msg = curl_multi_info_read(pCurlMulti, &msgsInQueue);
        if (!msg)
            break;
        if (msg->msg == CURLMSG_DONE) {
            CURL *curl = msg->easy_handle;

            CURLcode result = msg->data.result;

            std::map<unsigned int, Node *>::iterator it;

            long httpStatusCode = 0;

            Node *pNode = NULL;

            {
                ScopedMutex lock(mMutex);

                doneFlag = mDone;

                if (doneFlag) {
                    break;
                }

                it = mNodes.begin();

                while (it != mNodes.end()) {
                    if (curl == it->second->mWorkingCurl) {
                        break;
                    }

                    it++;
                }

                assert(it != mNodes.end());

                CURLcode infoCode;

                infoCode = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

                if (infoCode != CURLE_OK) {
                    httpStatusCode = 400;
                }

                NTRACE(TRACE_CURL_MULTI_THREAD,
                       "Request: %u completed the transaction %s, working CURL: %p, infoCode: %d HTTP response status: %ld",
                       it->first,
                       curl_easy_strerror(result),
                       curl,
                       infoCode,
                       httpStatusCode);

                pNode = it->second;

                assert((pNode->mState == Checking) || (pNode->mState == Checked) || (pNode->mState == Running));

                curl_multi_remove_handle(pCurlMulti, pNode->mWorkingCurl);

                if (pNode->mState == Checked) {
                    // assert( result == CURLE_COULDNT_CONNECT );

                    // This is the only case we signaled that we need a new connection.
                    if (pNode->mUrl.IsValid()) {
                        int32_t retVal;

                        uint32_t socketReceiveBufferSize = 0;

                        if (pNode->mUrl.IsHttps()) {
                            socketReceiveBufferSize = 32 * 1024;
                        } else {
                            socketReceiveBufferSize = 64 * 1024;
                        }

                        retVal = mAsyncHttpClientPtr->openConnection(pNode->mUrl.GetHost(),
                                                                     pNode->mUrl.GetPortNumber(),
                                                                     AseTimeVal::ZERO,
                                                                     AseTimeVal::ZERO,
                                                                     socketReceiveBufferSize,
                                                                     NULL,
                                                                     false,
                                                                     pNode->mAsyncHttpConnectionPtr);

                        NTRACE(TRACE_CURL_MULTI_THREAD,
                               "CURL: %u started to open connection to %s %d, ret: %d",
                               it->first,
                               pNode->mUrl.str().c_str(),
                               pNode->mUrl.GetPortNumber(),
                               retVal);

                        if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS)) {
                            pNode->mAsyncHttpConnectionPtr->setUserData((void *)(uintptr_t)it->first);

                            pNode->mState = Connecting;

                            curl_easy_cleanup(pNode->mWorkingCurl);

                            pNode->mWorkingCurl = NULL;

                            pNode = NULL;
                        } else {
                            result = convertAseErrorCodeToCurlCode(retVal);
                        }
                    } else {
                        result = CURLE_URL_MALFORMAT;
                    }
                } else {
                    // There are actual libcurl returned status.

                    if (pNode->mSocketHandle != CURL_SOCKET_BAD) {
                        ::close(pNode->mSocketHandle);

                        pNode->mSocketHandle = CURL_SOCKET_BAD;
                    }
                }

                if (pNode) {
                    if (pNode->mUrl.IsHttps()) {
                        assert(mSslConnectionCount >= 1);
                        --mSslConnectionCount;
                    } else {
                        assert(mHttpConnectionCount >= 1);
                        --mHttpConnectionCount;
                    }

                    curl_easy_cleanup(curl);

                    pNode->mState = Finished;
                }
            }

            if (pNode) {
                if (pNode->mFinishedCallback) {
                    NTRACE(TRACE_CURL_MULTI_THREAD, "Request: %u CURL: %p calling the call back", it->first, pNode->mOriginalCurl);

                    if (result == CURLE_OPERATION_TIMEDOUT) {
                        Log::error(TRACE_CURL_MULTI_THREAD, "CurlMultiThread HTTP request timeout, url: %s.", pNode->mUrl.str().c_str());
                    }

                    pNode->mFinishedCallback(httpStatusCode, result, pNode->mUserData);

                    NTRACE(TRACE_CURL_MULTI_THREAD, "Request: %u CURL: %p called the call back", it->first, pNode->mOriginalCurl);
                }

                {

                    ScopedMutex lock(mMutex);

                    for (std::list<weak_ptr<ConditionVariable> >::iterator i = pNode->mConditionVariables.begin();
                         i != pNode->mConditionVariables.end();
                         ++i) {
                        if (shared_ptr<ConditionVariable> condition = (*i).lock()) {
                            condition->signal();
                        }
                    }

                    NTRACE(TRACE_CURL_MULTI_THREAD, "Request: %u CURL: %p cleaned up the node", it->first, pNode->mOriginalCurl);

                    if (pNode->mDnsRecordPtr) {
                        curl_slist_free_all(pNode->mDnsRecordPtr);
                    }

                    delete pNode;

                    mNodes.erase(it);
                }
            }
        }
    }
}

curl_socket_t CurlMultiThread::openSocketCallback(void *clientp, curlsocktype /*purpose*/, struct curl_sockaddr * /*address*/)
{
    curl_socket_t sockfd;

    Node *pNode = (Node *)clientp;

    if (pNode->mState == Checking) {
        sockfd = CURL_SOCKET_BAD;

        pNode->mState = Checked;

        NTRACE(TRACE_CURL_MULTI_THREAD, "signaled a new connection is needed.");
    } else {
        sockfd = pNode->mSocketHandle;

        pNode->mSocketHandle = CURL_SOCKET_BAD;

        NTRACE(TRACE_CURL_MULTI_THREAD, "transferred a connected socket: %d to libcurl.", sockfd);
    }

    return sockfd;
}

int CurlMultiThread::socketOptCallback(void * /*clientp*/, curl_socket_t /*curlfd*/, curlsocktype /*purpose*/)
{
    return CURL_SOCKOPT_ALREADY_CONNECTED;
}

void CurlMultiThread::curlShareLockCallback(CURL * /*easy*/, curl_lock_data /*data*/, curl_lock_access access, void *userp)
{
    CurlMultiThread *that = (CurlMultiThread *)userp;

    that->mCurlShareLock.lock(access == CURL_LOCK_ACCESS_SINGLE ? ReadWriteLock::Write : ReadWriteLock::Read);
}

void CurlMultiThread::curlShareUnlockCallback(CURL * /*easy*/, curl_lock_data /*data*/, void *userp)
{
    CurlMultiThread *that = (CurlMultiThread *)userp;

    that->mCurlShareLock.unlock();
}
