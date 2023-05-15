/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include <nrdbase/tr1.h>

#include <nrdbase/DumpInfo.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/Noncopyable.h>

#include <nrdnet/AseUrl.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/NetError.h>

#include <vector>
#include <map>
#include <set>

namespace netflix {

class NetworkMetrics
{
public:
    NetworkMetrics(int dns = 0, int connect = 0, int transaction = 0, int duration = 0)
        : mDNSTime(dns), mConnectTime(connect), mTransactionTime(transaction), mDuration(duration)
    {}
    int dnsTime() const { return mDNSTime; }
    int connectTime() const { return mConnectTime; }
    int transactionTime() const { return mTransactionTime; }
    int duration() const { return mDuration; }
    void setDuration(int duration) { mDuration = duration; }
    void clear() { mDNSTime = mConnectTime = mTransactionTime = mDuration = 0; }
    NrdIpAddr serverIpAddress() const { return mServerIpAddress; }
    void setServerIpAddress(const NrdIpAddr &address) { mServerIpAddress = address; }
    std::string cname() const { return mCName; }
    void setCName(const std::string &cname) { mCName = cname; }
    void setSSLVersion(const std::string &version) { mSSLVersion = version; }
    std::string sslVersion() const { return mSSLVersion; }
private:
    int mDNSTime, mConnectTime, mTransactionTime, mDuration;
    NrdIpAddr mServerIpAddress;
    std::string mCName, mSSLVersion;
};

class ResourceManager;
class ResourceRequest;
class ResourceManagerDiskCacheRequest;
class ResourceManagerHttpServiceRequest;
class Resource : public enable_shared_from_this<Resource>, public Noncopyable
{
public:
    typedef shared_ptr<Resource> SharedPtr;
    typedef weak_ptr<Resource> WeakPtr;

    enum Method {
        Method_None,
        Method_Get,
        Method_Post,
        Method_Head,
        Method_Delete
    };

    Resource();
    virtual ~Resource();

    void init(const AseUrl &url, Method method, unsigned int flags);
    inline unsigned int requestFlags() const { return mRequestFlags; }
    inline void setRequestFlags(unsigned int flags) { mRequestFlags = flags; }
    inline AseUrl url() const { return mUrls.empty() ? AseUrl() : mUrls.back(); }
    inline AseUrl baseUrl() const { return mUrls.empty() ? AseUrl() : mUrls.front(); }
    inline std::vector<AseUrl> urls() const { return mUrls; }
    inline void setUrls(const std::vector<AseUrl> &urls) { mUrls = urls; }
    inline void addUrl(const AseUrl &url) { mUrls.push_back(url); }

    inline Method method() const { return mMethod; }

    inline int httpStatusCode() const { return mHttpStatusCode; }
    inline void setHttpStatusCode(int statusCode) { mHttpStatusCode = statusCode; }
    inline NetError::ErrorCode errorCode() const { return mErrorCode; }
    inline void setErrorCode(NetError::ErrorCode errorCode) { mErrorCode = errorCode; }
    inline int nativeErrorCode() const { return mNativeErrorCode; }
    inline DataBuffer responseData() const { return mResponseData; }
    inline void setResponseData(const DataBuffer &data) { mResponseData = data; }
    inline HttpResponseHeaders responseHeaders() const { return mResponseHeaders; }
    inline shared_ptr<ResourceManager> resourceManager() const { return mResourceManager.lock(); }
    inline void setResourceManager(const shared_ptr<ResourceManager> &resourceManager) {  mResourceManager = resourceManager; }
    std::string responseHeader(const std::string &name) const;

    virtual int cost() const;

    enum State {
        State_None,
        State_Network,
        State_Network_Validated,
        State_MemoryCache,
        State_DiskCache,
        State_DiskCacheRefresh,
        State_DiskCacheRefreshPending,
        State_File,
        State_Data,
        State_SurfaceCache
    };
    static const char *stateToString(State state);
    inline int maxAge() const { return mMaxAge; }
    inline Time expiration() const
    {
        if (!mExpiration.val() && mMaxAge && Time::hasServerTime())
            mExpiration = Time::serverTime() + Time::fromSeconds(mMaxAge);

        return mExpiration;
    }
    inline Time downloaded() const { return mDownloaded; }
    inline DataBuffer cacheKey() const { return mCacheKey; }
    inline void setCacheKey(const DataBuffer &cacheKey) { mCacheKey = cacheKey; }

    inline bool isExpired() const
    {
        ScopedMutex lock(mutex());
        if (!Time::hasServerTime() || !(mCompleteState & Complete))
            return false;
        // Resources do not expire if they don't have an expiration set
        const Time exp = expiration();
        return exp.val() && Time::serverTime() > exp;
    }

    inline NetworkMetrics networkMetrics() const { return mNetworkMetrics; }
    inline void setNetworkMetrics(const NetworkMetrics &metrics) { mNetworkMetrics = metrics; }

    bool addRequest(const shared_ptr<ResourceRequest> &request);
    virtual std::string describe() const;
    bool isComplete() const { ScopedMutex lock(mutex()); return mCompleteState & Complete; }

    virtual bool shouldSuspend() const { return false; }

    int requestCount() const { ScopedMutex lock(mutex()); return mRequests.size(); }
private:
    void wait();
    void abort();
    Mutex &mutex() const;
    ConditionVariable &condition();
    bool removeRequest(const shared_ptr<ResourceRequest> &request);
    void sync();
    unsigned int mRequestFlags;
    std::vector<AseUrl> mUrls;
    Method mMethod;

    int mHttpStatusCode, mRedirectCount, mNativeErrorCode;
    uint32_t mHSTRequestId;
    NetError::ErrorCode mErrorCode;
    enum {
        Complete = 0x1,
        PendingComplete = 0x2,
        AddToCache = 0x4
    };
    unsigned mCompleteState;

    DataBuffer mResponseData, mCacheKey;
    mutable Time mExpiration;
    Time mDownloaded;
    int mMaxAge;
    HttpResponseHeaders mResponseHeaders;
    NetworkMetrics mNetworkMetrics;
    std::set<shared_ptr<ResourceRequest> > mRequests;
    weak_ptr<ResourceManagerDiskCacheRequest> mDiskCacheRequest;
    weak_ptr<ResourceManager> mResourceManager;
    weak_ptr<ResourceManagerHttpServiceRequest> mNetworkRequest;

    friend class ResourceManager;
    friend class ResourceRequest;
    friend class ResourceManagerHttpServiceRequest;
    friend class ResourceManagerDiskCacheRequest;
};
} // namespace netflix::net

#endif
