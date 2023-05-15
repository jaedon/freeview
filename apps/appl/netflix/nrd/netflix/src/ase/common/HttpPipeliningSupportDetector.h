/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTP_PIPELINING_SUPPORT_DETECTOR_H
#define HTTP_PIPELINING_SUPPORT_DETECTOR_H

#include <list>
#include <map>

#include <nrdbase/Mutex.h>
#include <nrdnet/AseUrl.h>

#include <nrdase/IAseConfigParameter.h>
#include <nrdase/IPersistentStore.h>
#include <nrdbase/AseErrorCodes.h>
#include "AseCommonUtil.h"
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/IAsyncHttpLib.h>

namespace netflix {
namespace ase {

#define HTTP_PIPELINING_SUPPORT_DETECTOR_BUFFER_SIZE 1024

/**
 * @class
 * @brief
 */
class HttpPipeliningSupportDetector
{
public:

    typedef shared_ptr<HttpPipeliningSupportDetector> HttpPipeliningSupportDetectorPtr;

    /** Pipelining support result. */
    typedef enum
    {
        PIPELINING_SUPPORT_NO = 0,
        PIPELINING_SUPPORT_MAYBE,
        PIPELINING_SUPPORT_YES
    } PipeliningSupportResult;

    /** Test record. */
    class TestRecord
    {
    public:
        typedef shared_ptr<TestRecord> TestRecordPtr;

    public:
        /** The test record Id. */
        uint32_t mId;

        /** The URL. */
        AseUrl mUrl;

        /** The timeout. */
        AseTimeVal mTimeout;

        /** Pipelining support result. */
        PipeliningSupportResult mPipeliningSupportResult;

        /** Auxiliary reason code. */
        int32_t mReasonCode;
    };

    typedef TestRecord::TestRecordPtr TestRecordPtr;

public:
    /** Constructor. */
    HttpPipeliningSupportDetector(IPersistentStorePtr pStorePtr,
                                  IAseConfigParameterPtr pAseConfigParameter = IAseConfigParameterPtr());

    void setConfigParameter(IAseConfigParameterPtr pAseConfigParameter);

    /** Destructor. */
    virtual ~HttpPipeliningSupportDetector();


    /** check http pipeline support
     * @return true if pipeline supported
     */
    bool runCheckPipelineSupport(AseUrl const& testUrl);

    /** Get the pipeling state from persistent store
     *  @param [out] supportPipeline The pipelinging state
     *  @return AS_NO_ERROR if history is fresh
     *          AS_AGAIN is the test should be run again
     */
    int32_t getPipelineState(bool& supportPipeline);

    /** Abort the detection tests. This function is to be called from
     *  a different thread.
     *
     *  @return AS_NO_ERROR if the abort is successful
     *          AS_INVALID_VALUE if no tests are running.
     *          AS_UNKNOWN_ERROR unknown error
     */
    int32_t abort();


protected:
    /** Internal test record */
    class InternalTestRecord
    {
    public:
        typedef enum
        {
            CONNECTING_STATE = 0,
            WAITING_FOR_PROBE_ONE_RESPONSE_STATE,
            WAITING_FOR_PROBE_TWO_RESPONSE_STATE
        } State;

    public:
        /** The state. */
        State mState;

        /** The external test record. */
        TestRecordPtr mTestRecordPtr;

        /** The timeout in wall clock. */
        AseTimeVal mTimeout;

        /** The asyc HTTP connection. */
        IAsyncHttpConnectionPtr mAsyncHttpConnectionPtr;

        /** The received response code. */
        uint32_t mReceivedResponseCode;

        /** The received data length. */
        llong mReceivedDataLength;

        /** The receiving data buffer, this is only needed under the condition that the
         *  received data is to be consumed here rather than given back to the URL request
         *  manager. An example of such condition is URL redirects.
         */
        unsigned char mReceivingDataBuffer[HTTP_PIPELINING_SUPPORT_DETECTOR_BUFFER_SIZE];
    };

    /** Internal test record map. */
    typedef std::map<uint32_t, InternalTestRecord*> InternalTestRecordMap;

protected:
    void processHttpConnection(
                    std::list<TestRecordPtr>& completedTestList,
                    IAsyncHttpConnectionPtr pAsyncHttpConnection,
                    const AseTimeVal& timeNow );

    void processTimeout(
                    std::list<TestRecordPtr>& completedTestList,
                    const AseTimeVal& timeNow );

    void completeTest(
                    std::list<TestRecordPtr>& completedTestList,
                    TestRecordPtr pTestRecord,
                    PipeliningSupportResult pipeliningSupportResult,
                    int32_t reasonCode,
                    IAsyncHttpConnectionPtr pAsyncHttpConnection );


    /** Add a pipelining detection test.
     *
     *  @param url [In] The reqeust URL.
     *  @param timeout [In] The timeout for the test.
     *  @param pTestRecord [Out] The test record.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_NOT_ENOUGH_MEMORY if there is not enough memory
     *                               for the test.
     */
    int32_t addTest(
                const AseUrl& url,
                const AseTimeVal& timeout,
                TestRecordPtr& pTestRecord );

    /** Run all the detection tests. The function returns whenever one
     *  or multiple tests are completed. The return values indicates
     *  if there are still tests in progress, in that case this function
     *  should be called again until the return value indicates all the
     *  tests are completed.
     *
     *  @param completedTestList [Out] The completed tests.
     *
     *  @return AS_NO_ERROR if all the tests are completed
     *          AS_AGAIN if there are tests in progress
     *          AS_INTERRUPTED if the tests are interrupted.
     *          AS_UNKNOWN_ERROR unknown error
     */
    int32_t run( std::list<TestRecordPtr>& completedTestList );

    /** Cleanup the detector. */
    void cleanup();

    bool runPipelineSupportTest(AseUrl const& testUrl);
    void updateDetectionResult(uint32_t result, AseTimeVal const& testTime);

    void initParameters();

protected:
    /** The pointer to the configuration parameter. */
    IAseConfigParameterPtr mAseConfigParameterPtr;

    IPersistentStorePtr mPersistentStorePtr;

    /** The async HTTP library. */
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    /** The internal test record map. */
    InternalTestRecordMap mInternalTestRecordMap;

    /** The test ID pool. */
    uint32_t mTestIdPool;

    /** The mutex for protecting the HTTP library initialization. */
    Mutex mMutex;

    /** The abort flag. */
    volatile bool mAbortedFlag;

    /** The running flag. */
    volatile bool mRunningFlag;


    // The detection retry interval if the previous result is false, in seconds
    uint32_t                    mPipelineDetectionRetryInterval;
    // The detection refresh interval if the previous result is true, in seconds
    uint32_t                    mPipelineDetectionRefreshInterval;
    // The maximum number of consecutive playback failures tolerated
    uint32_t                    mMaxNumPlaybackFalures;

};

typedef HttpPipeliningSupportDetector::HttpPipeliningSupportDetectorPtr HttpPipeliningSupportDetectorPtr;

}}
#endif //HTTP_PIPELINING_SUPPORT_DETECTOR_H
