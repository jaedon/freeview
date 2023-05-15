/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTP_SERVICE_THREAD_H
#define HTTP_SERVICE_THREAD_H

#include <list>
#include <map>
#include <string>
#include <vector>
#include <deque>

#include <nrdbase/tr1.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/StringFormatter.h>

#include <nrdnet/AseTimeVal.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/IAsyncHttpLib.h>

#include "HttpServiceMetrics.h"
#include "HttpsService.h"
#include "HttpService.h"

namespace netflix
{
    class ConditionVariable;


class HttpService;
class HttpsService;

/**
 * The HTTP service thread class.
 */
class HttpServiceThread : public Thread, public HttpServiceListener, public HttpsServiceListener
{
public:
    //Constructor.
    HttpServiceThread(ThreadConfig * pThreadConfig);

    //Destructor.
    virtual ~HttpServiceThread();

    void scheduleDump();

    //Set/Get the number of connection per server.
    void setNumberOfConnectionPerServer(uint32_t count);
    uint32_t getNumberOfConnectionPerServer() const;

    //Set/Get the HTTP pipelining depth.
    void setHttpPipeliningDepth(uint32_t depth);
    uint32_t getHttpPipeliningDepth() const;

    //Set/Get the HTTP pipelining support threshold.
    void setHttpPipeliningSupportThreshold(uint32_t threshold);
    uint32_t getHttpPipeliningSupportThreshold() const;

    //Set/Get the HTTP connect backoff time, in ms.
    void setHttpConnectBackoffTime(uint32_t time);
    uint32_t getHttpConnectBackoffTime() const;

    //Set/Get the HTTP connect failure threshold.
    void setHttpConnectFailureThreshold(uint32_t threshold);
    uint32_t getHttpConnectFailureThreshold() const;

    //Start the HTTP service thread.
    int32_t start();

    //Stop the HTTP service thread.
    int32_t stop();

    //Add an HTTP request.
    uint32_t addHttpRequest(HttpServiceRequestPtr pServiceRequest);

    //Add an HTTPS request.
    uint32_t addHttpsRequest(
        HttpServiceRequestPtr pServiceRequest,
        TrustStorePtr pTrustStore,
        VerifyCertStatusFunc pVerifyCertStatusFunc,
        bool sslVerifyPeerFlag);

    //Cancel a request.
    void cancelRequest(
        uint32_t requestId,
        const AseUrl & requestUrl);

    //Wait for a request to finish.
    bool waitForRequest(
        uint32_t requestId,
        const AseUrl & requestUrl,
        const Time & waitTime = Time());

protected:
    //The working entry.
	class WorkingEntry
	{
	public:
		//Type definition for condition variable list.
		typedef std::list<weak_ptr<ConditionVariable> > ConditionVariableList;

		//The request ID.
		uint32_t mRequestId;

		//The request URL.
		AseUrl mRequestUrl;

		//The list of done signals.
		ConditionVariableList mSignalList;

	public:
		//Signal the working entry.
		void signal();
	};

    //Type definition for working map.
    typedef std::map<uint32_t, WorkingEntry> WorkingMap;

	typedef std::vector<WorkingEntry> CancelledRequestList;

protected:
    //The start() implementation.
    int32_t startImpl();

    //Send the signal byte to wake up the thread.
    void sendSignalByte();

    //The thread procedure.
    virtual void Run();

    //Compute the select timeout.
    AseTimeVal computeSelectTimeout();

    //Process the cancelled entries.
    void processCancelledEntries(
        std::vector<IAsyncHttpConnectionPtr>& selectedConnections);

    //Process the waiting entries.
    void processWaitingEntries();


    //Process the selected connections.
    void processSelectedConnections(
        const AseTimeVal & timeNow,
        std::vector<IAsyncHttpConnectionPtr> & selectedConnections);

    // Called when a request is finished
    virtual void onHttpRequestFinished(int requestId);
    // Called when a request is finished
    virtual void onHttpsRequestFinished(int requestId);

protected:

	//The flag that shows if the service is started.
    bool mStartedFlag;

    //The flag that shows if the service is completed.
    bool mDoneFlag;

    //Set if someone requested a dump
    bool mDumpPending;

    //The start time.
    AseTimeVal mStartTime;

    //The mutex.
    mutable Mutex mMutex;

    //The signal byte count mutex.
    Mutex mSignalByteCountMutex;

    //The signal byte count.
    uint32_t mSignalByteCount;

    //The async HTTP library.
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    //The select timeout value.
    AseTimeVal mSelectTimeout;

    //The list of cancelled requests.
    CancelledRequestList mCancelledRequests;


    //The new waiting map.
    WorkingMap mNewWaitingMap;

    //The processed waiting map.
    WorkingMap mProcessedWaitingMap;

    HttpService * mHttp;
    HttpsService * mHttps;
};

}

#endif //HTTP_SERVICE_THREAD_H
