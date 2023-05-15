/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdlib.h>
#include <vector>
#include <sstream>

#include <nrdnet/IpConnectivityManager.h>

#include <nrdbase/Log.h>
#include <nrdbase/tr1.h>

#include "HttpPipeliningSupportDetector.h"
#include "IPersistentStore.h"

using namespace std;

using namespace netflix;
using namespace netflix::ase;

/**
 * @class
 * @brief
 */

/** Constructor. */
HttpPipeliningSupportDetector::HttpPipeliningSupportDetector(IPersistentStorePtr pStorePtr, IAseConfigParameterPtr pAseConfigParameter)
                                                : mAseConfigParameterPtr( pAseConfigParameter ),
                                                  mPersistentStorePtr( pStorePtr ),
                                                  mTestIdPool( 0 ),
                                                  mMutex( HTTPPIPELINING_MUTEX, "HttpPipeliningSupportDetector" ),
                                                  mAbortedFlag( false ),
                                                  mRunningFlag( false )
{
    if ( mAseConfigParameterPtr )
    {
        initParameters();
    }
}

void HttpPipeliningSupportDetector::setConfigParameter(IAseConfigParameterPtr pAseConfigParameter)
{
    mAseConfigParameterPtr = pAseConfigParameter;
    initParameters();
}

void HttpPipeliningSupportDetector::initParameters()
{

    mPipelineDetectionRetryInterval =
        mAseConfigParameterPtr->getParameterAs<uint32_t>("pipelineDetectionRetryInterval", 7 * 24 * 3600 );  // one week
    mPipelineDetectionRefreshInterval =
        mAseConfigParameterPtr->getParameterAs<uint32_t>("pipelineDetectionRefreshInterval", 24 * 3600); // one day
    mMaxNumPlaybackFalures =
        mAseConfigParameterPtr->getParameterAs<uint32_t>("maxNumConsecutivePlaybackFailures", 3);

}

/** Destructor. */
HttpPipeliningSupportDetector::~HttpPipeliningSupportDetector()
{
}

int32_t HttpPipeliningSupportDetector::getPipelineState(bool& supportPipeLine)
{
    AseErrorCode retCode = AS_NO_ERROR;
    bool runTest = false;
    AseTimeVal now = AseTimeVal::now();
    ullong prevTestTime;

    do
    {
        string playbackHistory =
            mPersistentStorePtr->loadValue( IPersistentStore::NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES);
        uint32_t numPlaybackFailures = fromString<uint32_t>( playbackHistory, 0 );
        if ( numPlaybackFailures >= mMaxNumPlaybackFalures )
        {
            supportPipeLine = false;
            updateDetectionResult(0, now);
            break;
        }
        // load previous test result
        string testResult =
            mPersistentStorePtr->loadValue( IPersistentStore::PIPELINE_DETECTION_RESULTS );
        NTRACE(TRACE_MEDIACONTROL, "previous results: %s", testResult.c_str());
        if ( testResult != "" )
        {
            istringstream istream;
            istream.imbue(std::locale::classic());
            istream.str(testResult);
            char colon;
            uint32_t result;
            istream >> result >> colon >> prevTestTime;
            if ( result )
            {
                supportPipeLine = true;
                NTRACE(TRACE_MEDIACONTROL, "previous result: %d", result);
            }
            else
            {
                supportPipeLine = false;
                NTRACE(TRACE_MEDIACONTROL, "previous result: %d", result);
            }
        }
        else
        {
            // No previous detection result
            runTest = true;
            break;
        }

        if ( supportPipeLine )
        {
            if (prevTestTime + mPipelineDetectionRefreshInterval < now.seconds())
            {
                // previous test result expired
                runTest = true;
            }
        }
        else
        {
            if ( prevTestTime + mPipelineDetectionRetryInterval < now.seconds() )
            {
                runTest = true;
            }
        }

    } while (false);


    if ( runTest )
        retCode = AS_AGAIN;


    NTRACE(TRACE_MEDIACONTROL, "getPipelineState: support pipeline: %u, run test: %u",
                            supportPipeLine, runTest );
    return retCode;
}

bool HttpPipeliningSupportDetector::runCheckPipelineSupport(AseUrl const& testUrl)
{
    NTRACE(TRACE_MEDIACONTROL, "runCheckPipelineSupport: %s", testUrl.str().c_str());
    bool supportPipeLine = false;
    AseTimeVal now = AseTimeVal::now();

    AseErrorCode retCode = getPipelineState(supportPipeLine);

    if ( retCode == AS_AGAIN )
        supportPipeLine = runPipelineSupportTest(testUrl);

    if ( supportPipeLine )
    {
        updateDetectionResult(1, now);
        NTRACE(TRACE_MEDIACONTROL, "pipelineSupported: true");
    }
    else
    {
        updateDetectionResult(0, now);
        NTRACE(TRACE_MEDIACONTROL, "pipelining is not supported for this player");
    }

    return supportPipeLine;
}

/** Add a pipelining detection test. */
int32_t HttpPipeliningSupportDetector::addTest(
                                            const AseUrl& url,
                                            const AseTimeVal& timeout,
                                            TestRecordPtr& pTestRecord )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    if (!mRunningFlag)
    {
        ullong testId = mTestIdPool++;

        //Create a test record.
        pTestRecord = TestRecordPtr( new TestRecord );

        if (pTestRecord)
        {
            pTestRecord->mId = testId;
            pTestRecord->mUrl = url;
            pTestRecord->mTimeout = timeout;

            //Create an internal test record.
            InternalTestRecord* pInternalTestRecord = new InternalTestRecord;

            if (pInternalTestRecord)
            {
                pInternalTestRecord->mState = InternalTestRecord::CONNECTING_STATE;
                pInternalTestRecord->mTestRecordPtr = pTestRecord;
                pInternalTestRecord->mTimeout = AseTimeVal::INFINITE;
                pInternalTestRecord->mReceivedDataLength = 0;

                mInternalTestRecordMap[ testId ] = pInternalTestRecord;

                retVal = AS_NO_ERROR;
            }
            else
            {
                pTestRecord.reset();

                retVal = AS_NOT_ENOUGH_MEMORY;
            }

        }
        else
        {
            retVal = AS_NOT_ENOUGH_MEMORY;
        }
    }
    else
    {
        retVal = AS_NOT_ALlOWED;
    }

    return retVal;
}

/** Run all the detection tests. */
int32_t HttpPipeliningSupportDetector::run( std::list<TestRecordPtr>& completedTestList )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    completedTestList.clear();

    InternalTestRecordMap::iterator internalTestRecordItr;

    InternalTestRecord* pInternalTestRecord;

    std::list<TestRecordPtr>::iterator testRecordItr;

    do
    {
        mMutex.lock();

        if ((!mRunningFlag) && (!mAbortedFlag))
        {
            //Initialize the socket based async HTTP client.

            retVal = constructAsyncHttpClient(
                                mAsyncHttpClientPtr,
                                IpConnectivityManager::instance() );

            if (retVal != AS_NO_ERROR)
            {
                mMutex.unlock();

                break;
            }

            retVal = mAsyncHttpClientPtr->init();

            if (retVal != AS_NO_ERROR)
            {
                mAsyncHttpClientPtr.reset();

                mMutex.unlock();

                break;
            }

            //Establish the connection for each test.

            internalTestRecordItr = mInternalTestRecordMap.begin();

            while (internalTestRecordItr != mInternalTestRecordMap.end())
            {
                pInternalTestRecord = internalTestRecordItr->second;

                uint16_t portNumber = 80;

                if (pInternalTestRecord->mTestRecordPtr->mUrl.HasPort())
                {
                    portNumber = strtoul(
                                    pInternalTestRecord->mTestRecordPtr->mUrl.GetPort().c_str(),
                                    NULL,
                                    10 );
                }

                retVal = mAsyncHttpClientPtr->openConnection(
                                                pInternalTestRecord->mTestRecordPtr->mUrl.GetHost(),
                                                portNumber,
                                                AseTimeVal::ZERO,
                                                AseTimeVal::ZERO,
                                                0 /* use default socket buffer size */,
                                                NULL,
                                                false,
                                                pInternalTestRecord->mAsyncHttpConnectionPtr );

                if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
                {
                    pInternalTestRecord->mAsyncHttpConnectionPtr->setUserData(
                                                        (void*)(uintptr_t) internalTestRecordItr->first );
                }
                else
                {
                    pInternalTestRecord->mTestRecordPtr->mPipeliningSupportResult
                                                                = PIPELINING_SUPPORT_MAYBE;

                    pInternalTestRecord->mTestRecordPtr->mReasonCode = retVal;

                    completedTestList.push_back( pInternalTestRecord->mTestRecordPtr );
                }

                internalTestRecordItr++;
            }

            testRecordItr = completedTestList.begin();

            while (testRecordItr != completedTestList.end())
            {
                delete mInternalTestRecordMap[ (*testRecordItr)->mId ];

                mInternalTestRecordMap.erase( (*testRecordItr)->mId );

                testRecordItr++;
            }

            mRunningFlag = true;
        }

        if (mAbortedFlag)
        {
            mMutex.unlock();

            retVal = AS_INTERRUPTED;

            break;
        }

        mMutex.unlock();

        if (!completedTestList.empty())
        {
            if (!mInternalTestRecordMap.empty())
            {
                retVal = AS_AGAIN;
            }
            else
            {
                retVal = AS_NO_ERROR;
            }

            break;
        }
        else if (mInternalTestRecordMap.empty())
        {
            retVal = AS_NO_ERROR;

            break;
        }

        AseTimeVal timeNow = AseTimeVal::now();

        AseTimeVal selectTimeout = AseTimeVal::INFINITE;

        //Work out the select timeout.
        internalTestRecordItr = mInternalTestRecordMap.begin();

        while (internalTestRecordItr != mInternalTestRecordMap.end())
        {
            pInternalTestRecord = internalTestRecordItr->second;

            if (pInternalTestRecord->mState != InternalTestRecord::CONNECTING_STATE)
            {
                if (pInternalTestRecord->mTimeout > timeNow)
                {
                    if (selectTimeout > (pInternalTestRecord->mTimeout - timeNow))
                    {
                        selectTimeout = pInternalTestRecord->mTimeout - timeNow;
                    }
                }
                else
                {
                    selectTimeout = AseTimeVal::ZERO;

                    break;
                }
            }

            internalTestRecordItr++;
        }

        std::vector<IAsyncHttpConnectionPtr> selectedConnections;

        retVal = mAsyncHttpClientPtr->select( selectedConnections, selectTimeout );

        timeNow = AseTimeVal::now();

        if ((retVal == IAsyncHttpClient::SELECT_WAKE) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT))
        {
            retVal = AS_INTERRUPTED;

            break;
        }

        if (retVal == IAsyncHttpClient::SELECT_CONNECT)
        {
            std::vector<IAsyncHttpConnectionPtr>::iterator asyncHttpConnectionItr;

            asyncHttpConnectionItr = selectedConnections.begin();

            while (asyncHttpConnectionItr != selectedConnections.end())
            {
                processHttpConnection(
                            completedTestList,
                            *asyncHttpConnectionItr,
                            timeNow );

                //Get the test ID.
                asyncHttpConnectionItr++;
            }

            testRecordItr = completedTestList.begin();

            while (testRecordItr != completedTestList.end())
            {
                delete mInternalTestRecordMap[ (*testRecordItr)->mId ];

                mInternalTestRecordMap.erase( (*testRecordItr)->mId );

                testRecordItr++;
            }
        }

        processTimeout( completedTestList, timeNow );
    }
    while (true);

    return retVal;
}

/** Abort the detection tests. */
int32_t HttpPipeliningSupportDetector::abort()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    mMutex.lock();

    if (!mAbortedFlag)
    {
        if (mRunningFlag)
        {
            retVal = mAsyncHttpClientPtr->wake();
        }
        else
        {
            retVal = AS_NO_ERROR;
        }

        mAbortedFlag = true;
    }
    else
    {
        retVal = AS_INVALID_VALUE;
    }

    mMutex.unlock();

    return retVal;
}


/** Cleanup the detection test. */
void HttpPipeliningSupportDetector::cleanup()
{
    mMutex.lock();

    if (!mInternalTestRecordMap.empty())
    {
        InternalTestRecordMap::iterator internalTestRecordItr;

        internalTestRecordItr = mInternalTestRecordMap.begin();

        while (internalTestRecordItr != mInternalTestRecordMap.end())
        {
            delete internalTestRecordItr->second;

            internalTestRecordItr++;
        }

        mInternalTestRecordMap.clear();
    }

    if (mAsyncHttpClientPtr)
    {
        mAsyncHttpClientPtr->deinit();

        mAsyncHttpClientPtr.reset();
    }

    mRunningFlag = false;
    mAbortedFlag = false;

    mMutex.unlock();

    return;
}

void HttpPipeliningSupportDetector::processHttpConnection(
                                        std::list<TestRecordPtr>& completedTestList,
                                        IAsyncHttpConnectionPtr pAsyncHttpConnection,
                                        const AseTimeVal& timeNow )
{
    int32_t retVal;

    void* ptr = pAsyncHttpConnection->getUserData();

    uintptr_t testId = (uintptr_t) ptr;

    InternalTestRecord* pInternalTestRecord = mInternalTestRecordMap[ testId ];

    TestRecordPtr pTestRecord = pInternalTestRecord->mTestRecordPtr;

    switch (pInternalTestRecord->mState)
    {
        case InternalTestRecord::CONNECTING_STATE:
        {
            IAsyncHttpConnection::Event httpConnectionEvent;

            httpConnectionEvent = pInternalTestRecord
                                        ->mAsyncHttpConnectionPtr
                                        ->retrieveOutstandingEvent();

            assert( httpConnectionEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT);
            NRDP_UNUSED(httpConnectionEvent);

            if (pInternalTestRecord->mAsyncHttpConnectionPtr->getState()
                                                    == IAsyncHttpConnection::IDLE_STATE)
            {
                //Make up a range header.
                HttpHeaders headers;

                headers[ "Range" ] = "bytes=0-0";

                //Send out back-to-back probe requests.
                do
                {
                    IAsyncHttpRequestPtr pAsyncHttpRequest;

                    //Send probe one.
                    retVal = pInternalTestRecord->mAsyncHttpConnectionPtr->issueGetRequest(
                                                                            pInternalTestRecord->mTestRecordPtr->mUrl,
                                                                            headers,
                                                                            IHttpRequestTraceListenerPtr(),
                                                                            pAsyncHttpRequest );

                    if (retVal != AS_NO_ERROR)
                    {
                        break;
                    }

                    //Send probe two.
                    retVal = pInternalTestRecord->mAsyncHttpConnectionPtr->issueGetRequest(
                                                                            pInternalTestRecord->mTestRecordPtr->mUrl,
                                                                            headers,
                                                                            IHttpRequestTraceListenerPtr(),
                                                                            pAsyncHttpRequest );
                } while (false);

                if (retVal == AS_NO_ERROR)
                {
                    pInternalTestRecord->mTimeout = pInternalTestRecord->mTestRecordPtr->mTimeout + timeNow;

                    pInternalTestRecord->mState =
                                        InternalTestRecord::WAITING_FOR_PROBE_ONE_RESPONSE_STATE;
                }
                else
                {
                    completeTest(
                        completedTestList,
                        pTestRecord,
                        PIPELINING_SUPPORT_MAYBE,
                        retVal,
                        pInternalTestRecord->mAsyncHttpConnectionPtr );
                }
            }
            else
            {
                completeTest(
                    completedTestList,
                    pTestRecord,
                    PIPELINING_SUPPORT_MAYBE,
                    pInternalTestRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode(),
                    pInternalTestRecord->mAsyncHttpConnectionPtr );
            }

            break;
        }

        case InternalTestRecord::WAITING_FOR_PROBE_ONE_RESPONSE_STATE:
        {
            IAsyncHttpConnection::Event httpConnectionEvent;

            httpConnectionEvent = pInternalTestRecord
                                        ->mAsyncHttpConnectionPtr
                                        ->retrieveOutstandingEvent();

            if (httpConnectionEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT)
            {
                completeTest(
                    completedTestList,
                    pTestRecord,
                    PIPELINING_SUPPORT_MAYBE,
                    pInternalTestRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode(),
                    pInternalTestRecord->mAsyncHttpConnectionPtr );
            }
            else if (httpConnectionEvent == IAsyncHttpConnection::RESPONSE_AVAILABLE_EVENT)
            {
                IAsyncHttpRequestPtr pAsyncHttpRequest;

                IAsyncHttpRequest::State requestState;

                pAsyncHttpRequest = pInternalTestRecord->mAsyncHttpConnectionPtr->getTriggeringRequest();

                //Check the request state.
                requestState = pAsyncHttpRequest->getState();

                if ((requestState == IAsyncHttpRequest::RECEIVING_BODY_STATE)
                    || (requestState == IAsyncHttpRequest::COMPLETED_STATE))
                {
                    uint32_t httpMajorVersion;
                    uint32_t httpMinorVersion;

                    pAsyncHttpRequest->getResponseHttpVersion(
                                                        httpMajorVersion,
                                                        httpMinorVersion );

                    if ((httpMajorVersion == 1) && (httpMinorVersion == 1))
                    {
                        //Check the response code.
                        if ((pAsyncHttpRequest->getResponseCode() == 200)
                            ||  (pAsyncHttpRequest->getResponseCode() == 206))
                        {
                            uint32_t bufferSize;

                            bufferSize = HTTP_PIPELINING_SUPPORT_DETECTOR_BUFFER_SIZE;

                            retVal =  pAsyncHttpRequest->getResponseBody(
                                                                pInternalTestRecord->mReceivingDataBuffer,
                                                                bufferSize );

                            if (retVal == 1)
                            {
                                pInternalTestRecord->mReceivedResponseCode
                                            = pAsyncHttpRequest->getResponseCode();

                                pInternalTestRecord->mState
                                            = InternalTestRecord::WAITING_FOR_PROBE_TWO_RESPONSE_STATE;
                            }
                            else if (retVal != AS_NO_ERROR)
                            {
                                completeTest(
                                    completedTestList,
                                    pTestRecord,
                                    PIPELINING_SUPPORT_MAYBE,
                                    retVal,
                                    pInternalTestRecord->mAsyncHttpConnectionPtr );
                            }
                        }
                        else if ((pAsyncHttpRequest->getResponseCode() >= 300)
                                && (pAsyncHttpRequest->getResponseCode() <= 499))
                        {
                            if (pAsyncHttpRequest->getContentLength() == 0)
                            {
                                pInternalTestRecord->mReceivedResponseCode
                                            = pAsyncHttpRequest->getResponseCode();

                                pInternalTestRecord->mState
                                            = InternalTestRecord::WAITING_FOR_PROBE_TWO_RESPONSE_STATE;
                            }
                            else
                            {
                                //This response does have a body.

                                uint32_t bufferSize;

                                bufferSize = HTTP_PIPELINING_SUPPORT_DETECTOR_BUFFER_SIZE;

                                retVal =  pAsyncHttpRequest->getResponseBody(
                                                                    pInternalTestRecord->mReceivingDataBuffer,
                                                                    bufferSize );

                                if (retVal == 1)
                                {
                                    pInternalTestRecord->mReceivedResponseCode
                                                = pAsyncHttpRequest->getResponseCode();

                                    pInternalTestRecord->mState
                                                = InternalTestRecord::WAITING_FOR_PROBE_TWO_RESPONSE_STATE;

                                }
                                else if (retVal != AS_NO_ERROR)
                                {
                                    completeTest(
                                        completedTestList,
                                        pTestRecord,
                                        PIPELINING_SUPPORT_MAYBE,
                                        retVal,
                                        pInternalTestRecord->mAsyncHttpConnectionPtr );
                                }
                            }
                        }
                        else if ((pAsyncHttpRequest->getResponseCode() >= 500)
                                && (pAsyncHttpRequest->getResponseCode() <= 599))
                        {
                            completeTest(
                                completedTestList,
                                pTestRecord,
                                PIPELINING_SUPPORT_NO,
                                AS_HTTP_RESPONSE_5XX,
                                pInternalTestRecord->mAsyncHttpConnectionPtr );
                        }
                        else
                        {
                            completeTest(
                                completedTestList,
                                pTestRecord,
                                PIPELINING_SUPPORT_NO,
                                AS_HTTP_PROTOCOL_ERROR,
                                pInternalTestRecord->mAsyncHttpConnectionPtr );
                        }
                    }
                    else
                    {
                        completeTest(
                            completedTestList,
                            pTestRecord,
                            PIPELINING_SUPPORT_NO,
                            AS_HTTP_PROTOCOL_ERROR,
                            pInternalTestRecord->mAsyncHttpConnectionPtr );
                    }
                }
            }

            break;
        }

        case InternalTestRecord::WAITING_FOR_PROBE_TWO_RESPONSE_STATE:
        {
            IAsyncHttpConnection::Event httpConnectionEvent;

            httpConnectionEvent = pInternalTestRecord
                                        ->mAsyncHttpConnectionPtr
                                        ->retrieveOutstandingEvent();

            if (httpConnectionEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT)
            {
                completeTest(
                    completedTestList,
                    pTestRecord,
                    PIPELINING_SUPPORT_MAYBE,
                    pInternalTestRecord->mAsyncHttpConnectionPtr->getConnectionFailureCode(),
                    pInternalTestRecord->mAsyncHttpConnectionPtr );
            }
            else if (httpConnectionEvent == IAsyncHttpConnection::RESPONSE_AVAILABLE_EVENT)
            {
                IAsyncHttpRequestPtr pAsyncHttpRequest;

                IAsyncHttpRequest::State requestState;

                pAsyncHttpRequest = pInternalTestRecord->mAsyncHttpConnectionPtr->getTriggeringRequest();

                //Check the request state.
                requestState = pAsyncHttpRequest->getState();

                if ((requestState == IAsyncHttpRequest::RECEIVING_BODY_STATE)
                    || (requestState == IAsyncHttpRequest::COMPLETED_STATE))
                {
                    //Check the response code.
                    if ((pAsyncHttpRequest->getResponseCode() == 200)
                        || (pAsyncHttpRequest->getResponseCode() == 206))
                    {
                        uint32_t bufferSize;

                        bufferSize = HTTP_PIPELINING_SUPPORT_DETECTOR_BUFFER_SIZE;

                        retVal =  pAsyncHttpRequest->getResponseBody(
                                                            pInternalTestRecord->mReceivingDataBuffer,
                                                            bufferSize );

                        if (retVal == 1)
                        {
                            if (pInternalTestRecord->mReceivedResponseCode
                                            == pAsyncHttpRequest->getResponseCode())
                            {
                                completeTest(
                                    completedTestList,
                                    pTestRecord,
                                    PIPELINING_SUPPORT_YES,
                                    AS_NO_ERROR,
                                    pInternalTestRecord->mAsyncHttpConnectionPtr );
                            }
                            else
                            {
                                completeTest(
                                    completedTestList,
                                    pTestRecord,
                                    PIPELINING_SUPPORT_NO,
                                    AS_INVALID_VALUE,
                                    pInternalTestRecord->mAsyncHttpConnectionPtr );
                            }
                        }
                        else if (retVal != AS_NO_ERROR)
                        {
                            completeTest(
                                completedTestList,
                                pTestRecord,
                                PIPELINING_SUPPORT_MAYBE,
                                retVal,
                                pInternalTestRecord->mAsyncHttpConnectionPtr );
                        }
                    }
                    else if ((pAsyncHttpRequest->getResponseCode() >= 300)
                            && (pAsyncHttpRequest->getResponseCode() <= 499))
                    {
                        if (pAsyncHttpRequest->getContentLength() == 0)
                        {
                            if (pInternalTestRecord->mReceivedResponseCode
                                            == pAsyncHttpRequest->getResponseCode())
                            {
                                completeTest(
                                    completedTestList,
                                    pTestRecord,
                                    PIPELINING_SUPPORT_YES,
                                    AS_NO_ERROR,
                                    pInternalTestRecord->mAsyncHttpConnectionPtr );
                            }
                            else
                            {
                                completeTest(
                                    completedTestList,
                                    pTestRecord,
                                    PIPELINING_SUPPORT_NO,
                                    AS_INVALID_VALUE,
                                    pInternalTestRecord->mAsyncHttpConnectionPtr );
                            }
                        }
                        else
                        {
                            //This response does have a body.

                            uint32_t bufferSize;

                            bufferSize = HTTP_PIPELINING_SUPPORT_DETECTOR_BUFFER_SIZE;

                            retVal =  pAsyncHttpRequest->getResponseBody(
                                                                pInternalTestRecord->mReceivingDataBuffer,
                                                                bufferSize );

                            if (retVal == 1)
                            {
                                if (pInternalTestRecord->mReceivedResponseCode
                                                == pAsyncHttpRequest->getResponseCode())
                                {
                                    completeTest(
                                        completedTestList,
                                        pTestRecord,
                                        PIPELINING_SUPPORT_YES,
                                        AS_NO_ERROR,
                                        pInternalTestRecord->mAsyncHttpConnectionPtr );
                                }
                                else
                                {
                                    completeTest(
                                        completedTestList,
                                        pTestRecord,
                                        PIPELINING_SUPPORT_NO,
                                        AS_INVALID_VALUE,
                                        pInternalTestRecord->mAsyncHttpConnectionPtr );
                                }
                            }
                            else if (retVal != AS_NO_ERROR)
                            {
                                completeTest(
                        completedTestList,
                                    pTestRecord,
                                    PIPELINING_SUPPORT_MAYBE,
                                    retVal,
                                    pInternalTestRecord->mAsyncHttpConnectionPtr );
                            }
                        }
                    }
                    else if ((pAsyncHttpRequest->getResponseCode() >= 500)
                            && (pAsyncHttpRequest->getResponseCode() <= 599))
                    {
                        completeTest(
                            completedTestList,
                            pTestRecord,
                            PIPELINING_SUPPORT_NO,
                            AS_HTTP_RESPONSE_5XX,
                            pInternalTestRecord->mAsyncHttpConnectionPtr );
                    }
                    else
                    {
                        completeTest(
                            completedTestList,
                            pTestRecord,
                            PIPELINING_SUPPORT_NO,
                            AS_HTTP_PROTOCOL_ERROR,
                            pInternalTestRecord->mAsyncHttpConnectionPtr );
                    }
                }
            }

            break;
        }
    }

    return;
}

void HttpPipeliningSupportDetector::processTimeout(
                                        std::list<TestRecordPtr>& completedTestList,
                                        const AseTimeVal& timeNow )
{
    std::list<TestRecordPtr> tempTestList;

    InternalTestRecordMap::iterator internalTestRecordItr;

    internalTestRecordItr = mInternalTestRecordMap.begin();

    while (internalTestRecordItr != mInternalTestRecordMap.end())
    {
        if (internalTestRecordItr->second->mState != InternalTestRecord::CONNECTING_STATE)
        {
            if (timeNow >= internalTestRecordItr->second->mTimeout)
            {
                completeTest(
                        tempTestList,
                        internalTestRecordItr->second->mTestRecordPtr,
                        PIPELINING_SUPPORT_MAYBE,
                        AS_TIMEOUT,
                        internalTestRecordItr->second->mAsyncHttpConnectionPtr );
            }
        }

        internalTestRecordItr++;
    }


    std::list<TestRecordPtr>::iterator testRecordItr;

    testRecordItr = tempTestList.begin();

    while (testRecordItr != tempTestList.end())
    {
        completedTestList.push_back( *testRecordItr );

        delete mInternalTestRecordMap[ (*testRecordItr)->mId ];

        mInternalTestRecordMap.erase( (*testRecordItr)->mId );

        testRecordItr++;
    }
}

void HttpPipeliningSupportDetector::completeTest(
                                        std::list<TestRecordPtr>& completedTestList,
                                        TestRecordPtr pTestRecord,
                                        PipeliningSupportResult pipeliningSupportResult,
                                        int32_t reasonCode,
                                        IAsyncHttpConnectionPtr pAsyncHttpConnection )
{
    pTestRecord->mPipeliningSupportResult = pipeliningSupportResult;
    pTestRecord->mReasonCode = reasonCode;

    mAsyncHttpClientPtr->closeConnection( pAsyncHttpConnection );

    completedTestList.push_back( pTestRecord );
}


void HttpPipeliningSupportDetector::updateDetectionResult(uint32_t result, AseTimeVal const& testTime)
{
    // update pipeline detection test time
    NTRACE(TRACE_MEDIACONTROL, "updateDetectionResult: %d", result);
    ostringstream ostream;
    ostream.imbue(std::locale::classic());
    ostream << result << ":" << testTime.seconds();
    mPersistentStorePtr->saveValue( IPersistentStore::PIPELINE_DETECTION_RESULTS, ostream.str() );
}

bool HttpPipeliningSupportDetector::runPipelineSupportTest(AseUrl const& testUrl)
{
    NTRACE(TRACE_MEDIACONTROL, "runPipelineSupportTest");
    AseErrorCode retCode = AS_NO_ERROR;

    AseTimeVal timeout = AseTimeVal::fromMS( 30 * 1000 ); // 30 seconds
    TestRecordPtr pTestRecord;
    retCode = addTest( testUrl, timeout, pTestRecord );
    if ( retCode != AS_NO_ERROR )
        return false;

    retCode = addTest( testUrl, timeout, pTestRecord );
    if ( retCode != AS_NO_ERROR )
        return false;

    list<TestRecordPtr> completedTestList;
    retCode = AS_AGAIN;
    uint32_t noCount = 0;
    uint32_t yesCount = 0;
    uint32_t maybeCount = 0;
    while (retCode == AS_AGAIN)
    {
        retCode = run( completedTestList );
        if ((retCode == AS_NO_ERROR) || (retCode == AS_AGAIN))
        {
            std::list<TestRecordPtr>::iterator testRecordItr;
            for ( testRecordItr = completedTestList.begin();
                    testRecordItr != completedTestList.end();
                    ++testRecordItr )
            {
                switch((*testRecordItr)->mPipeliningSupportResult)
                {
                    case PIPELINING_SUPPORT_NO:
                    {
                        ++noCount;
                        NTRACE(TRACE_MEDIACONTROL, "one no");
                        break;
                    }

                    case PIPELINING_SUPPORT_MAYBE:
                    {
                        ++maybeCount;
                        NTRACE(TRACE_MEDIACONTROL, "one maybe");
                        break;
                    }

                    case PIPELINING_SUPPORT_YES:
                    {
                        ++yesCount;
                        NTRACE(TRACE_MEDIACONTROL, "one yes");
                        break;
                    }
                }
            }
        }
        else
        {
            break;
        }

        if ( (noCount > 0) || (yesCount > 0) )
        {
            retCode = AS_NO_ERROR;
            break;
        }
    }

    bool supportPipeLine = true;
    if ( (noCount > 0) || (maybeCount == 2) )
    {
        supportPipeLine = false;
    }
    else
    {
        supportPipeLine = true;
    }


    cleanup();
    return supportPipeLine;
}
