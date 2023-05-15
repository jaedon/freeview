/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RESOURCEREQUEST_H
#define RESOURCEREQUEST_H

#include <nrdbase/tr1.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/DumpInfo.h>
#include <nrdbase/EventLoop.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/Noncopyable.h>

#include <nrdnet/AseUrl.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/TrustStore.h>
#include <nrdnet/Resource.h>
#include <nrdnet/DiskCache.h>

#include <vector>
#include <map>
#include <set>

struct ResourceRequestCount : public netflix::ObjectCount::Record
{
public:
    inline ResourceRequestCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~ResourceRequestCount() { }
    virtual std::string describe(void *object) const;
};

NRDP_DECLARE_OBJECTCOUNT_RECORD(ResourceRequestCount, ResourceRequest);

namespace netflix {

class ResourceRequest : public enable_shared_from_this<ResourceRequest>, public Noncopyable
{
public:
    typedef shared_ptr<ResourceRequest> SharedPtr;
    typedef weak_ptr<ResourceRequest> WeakPtr;

    ResourceRequest(const AseUrl &url, Resource::Method method=Resource::Method_Get);
    ResourceRequest(unsigned int flags, const AseUrl &url, Resource::Method method);
    virtual ~ResourceRequest()
    {
        NTRACE(TRACE_RESOURCEMANAGER, "Request destroyed %s %p", mUrl.c_str(), this);
        NRDP_OBJECTCOUNT_DEREF(ResourceRequest, this);
    }

    inline unsigned int flags() const
    {
        return mFlags;
    }
    void setFlags(unsigned int flags)
    {
        mFlags = flags;
    }
    void setFlag(unsigned int flag, bool on = true)
    {
        if (on) {
            mFlags |= flag;
        } else {
            mFlags &= ~flag;
        }
    }

    enum Flag {
        None        = 0x000000,
        Synchronous = 0x100000,
        Joined      = 0x200000
    };

    inline Resource::Method method() const
    {
        return mMethod;
    }
    inline void setRequestHeaders(const HttpHeaders &h)
    {
#ifdef NRDP_HAS_TRACING
        for (HttpHeaders::const_iterator it = h.begin(); it != h.end(); ++it)
            NTRACE(TRACE_RESOURCEMANAGER, "%s %s [%s=%s]", __FUNCTION__, url().c_str(), it->first.c_str(), it->second.c_str());
#endif
        mRequestHeaders = h;
    }
    inline HttpHeaders requestHeaders() const
    {
        return mRequestHeaders;
    }
    inline void setRequestHeader(const std::string &key, const std::string &value)
    {
        NTRACE(TRACE_RESOURCEMANAGER, "%s %s [%s=%s]", __FUNCTION__, url().c_str(), key.c_str(), value.c_str());
        if (value.empty()) {
            mRequestHeaders.erase(key);
        } else {
            mRequestHeaders[key] = value;
        }
    }
    inline bool hasRequestHeader(const std::string &key) const
    {
        HttpHeaders::const_iterator it = mRequestHeaders.find(key);
        return (it != mRequestHeaders.end());
    }
    inline std::string requestHeader(const std::string &key) const
    {
        HttpHeaders::const_iterator it = mRequestHeaders.find(key);
        if (it != mRequestHeaders.end())
            return it->second;
        return std::string();
    }
    inline const DataBuffer requestData() const
    {
        return mRequestData;
    }
    inline void setRequestData(const DataBuffer &b)
    {
        mRequestData = b;
        if (!mRequestData.isEmpty())
            mMethod = Resource::Method_Post;
    }
    inline bool hasRequestData() const
    {
        return !mRequestData.isEmpty();
    }

    inline Time timeout() const
    {
        return mTimeout;
    }
    inline void setTimeout(const Time &timeOut)
    {
        mTimeout = timeOut;
    }

    inline Time connectTimeout() const
    {
        return mConnectTimeout;
    }
    inline void setConnectTimeout(const Time &timeOut)
    {
        mConnectTimeout = timeOut;
    }

    inline Time stallTimeout() const
    {
        return mStallTimeout;
    }
    inline void setStallTimeout(const Time &timeOut)
    {
        mStallTimeout = timeOut;
    }

    inline AseUrl url() const
    {
        return mUrl;
    }
    inline AseUrl baseUrl() const
    {
        return mBaseUrl;
    }
    inline void setUrl(const AseUrl &url)
    {
        mUrl = url;
    }
    inline void setBaseUrl(const AseUrl &url)
    {
        mBaseUrl = url;
    }
    inline const DataBuffer cacheKey() const
    {
        return mCacheKey;
    }
    inline void setCacheKey(const DataBuffer &cacheKey)
    {
        mCacheKey = cacheKey;
    }
    bool abort();

    inline void setResource(const Resource::SharedPtr &resource)
    {
        mResource = resource;
    }

    inline Resource::SharedPtr resource() const
    {
        return mResource.lock();
    }
    shared_ptr<ResourceManager> resourceManager() const;
    inline void setResourceManager(const shared_ptr<ResourceManager> &manager)
    {
        mResourceManager = manager;
    }

    inline void setExtraData(const Variant &data)
    {
        mExtraData = data;
    }
    inline Variant extraData() const
    {
        return mExtraData;
    }

    virtual void onComplete(const Resource::SharedPtr &, Resource::State) { }
private:
    void startTimeoutTimer();
    void wait();
    AseUrl mUrl, mBaseUrl;
    DataBuffer mCacheKey;
    Resource::Method mMethod;
    Resource::WeakPtr mResource;
    unsigned int mFlags;
    HttpHeaders mRequestHeaders;
    DataBuffer mRequestData;
    EventLoop::Timer::WeakPtr mTimeoutTimer;
    Time mTimeout, mConnectTimeout, mStallTimeout;
    weak_ptr<ResourceManager> mResourceManager;
    Variant mExtraData;
    friend class ResourceManager;
};

} // namespace netflix::net


#endif
