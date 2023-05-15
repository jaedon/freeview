/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* Most of the internal data of the resource (contents, headers, expiration etc)
 * are immutable from the point that they're accessible from other threads than
 * the network thread. This is why there is no mutex protection around the
 * setters and getters */

#include "Resource.h"
#include "ResourceRequest.h"
#include "ResourceManager.h"
#include "DiskCache.h"
#include <nrdbase/StringTokenizer.h>
#include <nrdnet/HttpServiceThread.h>
#include <nrdbase/ObjectCount.h>

using namespace netflix;

struct ResourceCount : public netflix::ObjectCount::Record
{
public:
    inline ResourceCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    virtual size_t cost(void *object) const { return static_cast<Resource*>(object)->cost(); }
    virtual std::string describe(void *object) const { return static_cast<Resource*>(object)->url().str(); }
};

NRDP_DEFINE_OBJECTCOUNT_RECORD(ResourceCount, Resource);

Resource::Resource()
    : mRequestFlags(0), mMethod(Method_None), mHttpStatusCode(0),
      mRedirectCount(0), mNativeErrorCode(AS_NO_ERROR), mHSTRequestId(0),
      mErrorCode(NetErrorCode::NETWORKERROR), mCompleteState(0),
      mDownloaded(Time::mono()), mMaxAge(0)
{
    NRDP_OBJECTCOUNT_REF(Resource, this);
}

Resource::~Resource()
{
    NTRACE(TRACE_RESOURCEMANAGER, "Resource destroyed %s %p", url().c_str(), this);
    NRDP_OBJECTCOUNT_DEREF(Resource, this);
    {
        ScopedMutex lock(ResourceManager::sMutex);
        abort();
    }
}

void Resource::init(const AseUrl &url, Method method, unsigned int flags)
{
    assert(mMethod == Method_None);
    assert(method != Method_None);
    mMethod = method;
    mUrls.push_back(url);
    mRequestFlags = flags;
}

void Resource::abort()
{
    assert(MutexStack::locked(ResourceManager::sMutex.name()));

    if (mCompleteState & (Complete|PendingComplete))
        return;
    ResourceManager::SharedPtr resourceManager = mResourceManager.lock();
    if (!resourceManager)
        return;
    NTRACE(TRACE_RESOURCEMANAGER, "Resource::abort %s id %d complete 0x%x", url().c_str(), mHSTRequestId, mCompleteState);
    if (mHSTRequestId && resourceManager->mHttpServiceThread)
        resourceManager->mHttpServiceThread->cancelRequest(mHSTRequestId, mUrls.back());
    resourceManager->remove(mCacheKey);
    mHSTRequestId = 0;
    shared_ptr<ResourceManagerDiskCacheRequest> diskCacheRequest = mDiskCacheRequest.lock();
    shared_ptr<ResourceManagerHttpServiceRequest> networkRequest = mNetworkRequest.lock();
    mDiskCacheRequest.reset();
    mNetworkRequest.reset();
    mCompleteState = Complete;
    mErrorCode = NetErrorCode::ABORTED;

    if (diskCacheRequest)
        ResourceManager::abortDiskCacheRequest(diskCacheRequest);
    if (networkRequest) {
        resourceManager->removeNetworkRequest(networkRequest);
    }
}

std::string Resource::describe() const
{
    ScopedMutex lock(ResourceManager::sMutex);
    std::ostringstream s;
    s << "size:" << prettyBytes(mResponseData.size()) << " " << url();
    const DataBuffer cacheKey = ResourceManager::prettyCacheKey(mCacheKey);
    if (cacheKey != url().str())
        s << " key:" << cacheKey.c_str();
    return s.str();
}

std::string Resource::responseHeader(const std::string &name) const
{
    return ResourceManager::findResponseHeader(name, mResponseHeaders);
}

bool Resource::addRequest(const ResourceRequest::SharedPtr &request)
{
    {
        ScopedMutex lock(ResourceManager::sMutex);
        if (!(mCompleteState & Complete)) {
            mRequests.insert(request);
            return false;
        }
    }
    resourceManager()->instrument(request, shared_from_this(), url(), State_MemoryCache);
    resourceManager()->requestFinished(request, shared_from_this(), State_MemoryCache);
    request->onComplete(shared_from_this(), State_MemoryCache);
    return true;
}

bool Resource::removeRequest(const shared_ptr<ResourceRequest> &request)
{
    assert(MutexStack::locked(ResourceManager::sMutex.name()));
    NTRACE(TRACE_RESOURCEMANAGER, "Resource::removeRequest %s from %zu", url().c_str(), mRequests.size());
    const bool ret = mRequests.erase(request);
    if (ret && mRequests.empty()) {
        abort();
    }
    return ret;
}

int Resource::cost() const
{
    int result = sizeof(Resource);
    for (HttpResponseHeaders::const_iterator it = mResponseHeaders.begin(); it != mResponseHeaders.end(); ++it)
        result += it->header.length() + it->value.length();
    result += mCacheKey.capacity();
    result += mResponseData.capacity();
    return result;
}

void Resource::wait()
{
    NTRACE(TRACE_RESOURCEMANAGER, "Resource wait %s %p", url().c_str(), this);
    ScopedMutex lock(ResourceManager::sMutex);
    while (!(mCompleteState & Complete)) {
        ResourceManager::sCondition.wait(&ResourceManager::sMutex);
    }
    NTRACE(TRACE_RESOURCEMANAGER, "Resource wait %s finished %p", url().c_str(), this);
}

const char *Resource::stateToString(State state)
{
    switch (state) {
    case State_None:
        return "none";
    case State_Network:
        return "network";
    case State_MemoryCache:
        return "memory";
    case State_DiskCacheRefreshPending:
    case State_Network_Validated:
    case State_DiskCache:
        return "disk";
    case State_DiskCacheRefresh:
        return "refresh";
    case State_File:
        return "file";
    case State_Data:
        return "data";
    case State_SurfaceCache:
        return "surfacecache";
    }
    return "";
}

Mutex & Resource::mutex() const
{
    return ResourceManager::sMutex;
}

ConditionVariable &Resource::condition()
{
    return ResourceManager::sCondition;
}
