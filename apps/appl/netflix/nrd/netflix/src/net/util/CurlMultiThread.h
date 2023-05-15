/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef CurlMultiThread_h
#define CurlMultiThread_h

#include <map>
#include <list>

#include <curl/curl.h>
#include <openssl/ssl.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/ReadWriteLock.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/TrustStore.h>

namespace netflix {

class CurlMultiThread : public Thread
{
public:
    CurlMultiThread(ThreadConfig *);
    ~CurlMultiThread();

    void stop();

    uint32_t add(CURL *curl, void (*finishedCallback)(long, CURLcode, void *) = 0, void *userData = 0);
    void cancel(unsigned int requestId);
    bool waitFor(unsigned int requestId, const Time &max = Time());
    CURLcode perform(CURL *curl, const Time &max = Time());
    virtual void Run();

protected:
    // This is protected, so that nobody outside can call it.
    virtual void Start();

private:
    enum NodeState {
        Added = 0,
        Checking,
        Checked,
        Connecting,
        Running,
        Finished
    };

    struct Node {
        Node()
            : mUserData(NULL), mState(Added), mCancelledFlag(false),
              mWorkingCurl(NULL), mSocketHandle(CURL_SOCKET_BAD)
        {}

        void (*mFinishedCallback)(long, CURLcode, void *);
        void *mUserData;
        NodeState mState;
        bool mCancelledFlag;
        std::list<weak_ptr<ConditionVariable> > mConditionVariables;
        /** The asyc HTTP connection. */
        IAsyncHttpConnectionPtr mAsyncHttpConnectionPtr;
        CURLcode mCurlResult;
        CURL *mOriginalCurl;
        CURL *mWorkingCurl;
        int mSocketHandle;
        AseUrl mUrl;
        struct curl_slist *mDnsRecordPtr;
    };

private:
    static void performFinishedCallback(long httpStatue, CURLcode code, void *data);
    void finish(long httpStatus, Node *pNode, CURLcode result);
    CURLcode convertAseErrorCodeToCurlCode(int32_t aseErrorCode);
    void performCurlMulti(CURLM *pCurlMulti, bool &doneFlag);
    static curl_socket_t openSocketCallback(void *clientp, curlsocktype purpose, struct curl_sockaddr *address);
    static int socketOptCallback(void *clientp, curl_socket_t curlfd, curlsocktype purpose);
    static void curlShareLockCallback(CURL *easy, curl_lock_data data, curl_lock_access access, void *userp);
    static void curlShareUnlockCallback(CURL *easy, curl_lock_data data, void *userp);
private:
    unsigned int mRequestIdPool;
    bool mStartedFlag;
    bool mDone;
    CURLSH *mCurlShare;
    ReadWriteLock mCurlShareLock;
    CURLM *mHttpMulti;
    CURLM *mSslMulti;
    std::map<unsigned int, Node *> mNodes;
    Mutex mMutex;
    /** The async HTTP library. */
    IAsyncHttpClientPtr mAsyncHttpClientPtr;
    unsigned int mHttpConnectionCount;
    unsigned int mSslConnectionCount;
    Mutex mSignalByteCountMutex;
    uint32_t mSignalByteCount;
};
}

#endif
