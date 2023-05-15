/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ResourceManager.h"
#include "DiskCache.h"

#include <nrdnet/ResourceManager.h>
#include <nrdnet/IpConnectivityManager.h>
#include <nrdnet/HttpServiceThread.h>

#include <nrdbase/Dumper.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Application.h>
#include <nrdbase/Configuration.h>

#include <openssl/sha.h>

#include <assert.h>
#include <iostream>
#include <errno.h>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>

using namespace netflix;

namespace netflix {

struct CacheData {
    CacheData() : mode(0), refresh(-1), priority(0), maxAge(0) {}

    unsigned int mode;
    int refresh, priority, maxAge;
    Time expiration;
    DataBuffer key;
};

class ResourceManagerHttpServiceRequest : public HttpServiceRequest, public enable_shared_from_this<ResourceManagerHttpServiceRequest>
{
public:
    ResourceManagerHttpServiceRequest(const Resource::SharedPtr &resource,
                                      const ResourceRequest::SharedPtr &request,
                                      const Resource::State state = Resource::State_Network);
    static int sslVerifyCallback(int status, void *ctx, HttpServiceRequest *request, const NrdIpAddr *ipAddress);

    virtual bool shouldSuspend() const {
        if (mResource)
            return mResource->shouldSuspend();
        return HttpServiceRequest::shouldSuspend();
    }
    virtual void provideResponseHeaders(const HttpResponseHeaders &responseHeaders);
    virtual void provideResponseBody(const DataBuffer& responseData);
    virtual void finish(int nativeErrorCode, long httpStatusCode, HttpServiceMetrics *pHttpServiceMetrics);

    Resource::SharedPtr resource() { return mResource; }

    int mRedirectCount;

    const Time mStarted;
    DataBuffer mResponseData, mCachedResponseData;
    HttpResponseHeaders mResponseHeaders, mCachedResponseHeaders;
    Resource::SharedPtr mResource;
    Resource::State mState;
    int mCachedHttpStatusCode;
    const unsigned int mRequestFlags;
    ResourceManager::SharedPtr mResourceManager;
};

class ResourceManagerRefreshTimer : public Application::Timer
{
public:
    ResourceManagerRefreshTimer(int seconds, const shared_ptr<ResourceManagerHttpServiceRequest> &request)
        : Application::Timer(seconds * 1000), mRequest(request)
    {
        setSingleShot(true);
    }

    virtual std::string describe() const { return "ResourceManagerRefreshTimer"; }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("resourcemanagerrefreshtimer.fired");
        mRequest->mResourceManager->sendNetworkRequest(mRequest);
    }
private:
    shared_ptr<ResourceManagerHttpServiceRequest> mRequest;
};

class ResourceManagerDiskCacheRequest : public DiskCache::Request
{
public:
    ResourceManagerDiskCacheRequest(const AseUrl &url, DiskCache::Response::Type type, const CacheData &cacheData,
                                    const Resource::SharedPtr &resource, const ResourceRequest::SharedPtr &request);
    virtual void onComplete(const DiskCache::Response::SharedPtr &response);
private:
    Resource::SharedPtr mResource;
    const ResourceRequest::SharedPtr mRequest;
    CacheData mCacheData;
    const Time mStarted;
};

Mutex ResourceManager::sMutex(RESOURCEMANAGER_MUTEX, "ResourceManager");
ConditionVariable ResourceManager::sCondition;
ResourceManager::ResourceManager()
    : mCapacity(0), mSize(0), mHits(0), mMisses(0), mExpired(0), mNoCache(0), mState(Initial)
{
    INSTRUMENTATION_INTERVAL_SWITCHED_START("resourcemanager", Variant(), 120);
}

ResourceManager::~ResourceManager()
{
    NTRACE(TRACE_RESOURCEMANAGER, "cleaning up.");
    mCacheList.deleteAll();
}

void ResourceManager::init()
{
    ScopedMutex lock(sMutex);
    mHttpServiceThread.reset(new HttpServiceThread(&THREAD_RESOURCE_HTTP));
    mHttpServiceThread->setNumberOfConnectionPerServer(IpConnectivityManager::instance()->getHstNumberOfConnectionPerServer());
    mHttpServiceThread->setHttpPipeliningDepth(IpConnectivityManager::instance()->getHstHttpPipeliningDepth());
    mHttpServiceThread->setHttpPipeliningSupportThreshold(IpConnectivityManager::instance()->getHstHttpPipeliningSupportThreshold());
    mHttpServiceThread->setHttpConnectBackoffTime(IpConnectivityManager::instance()->getHstHttpConnectBackoffTime());
    mHttpServiceThread->setHttpConnectFailureThreshold(IpConnectivityManager::instance()->getHstHttpConnectFailureThreshold());
    const uint32_t err = mHttpServiceThread->start();
    if (err != AS_NO_ERROR) {
        Log::error(TRACE_RESOURCEMANAGER, "Failed to initialize http thread %d", err);
    }
    mState = Running;
}

void ResourceManager::shutdown()
{
    {
        ScopedMutex lock(sMutex);
        if (mState == ShuttingDown || mState == Finished)
            return;
        mState = ShuttingDown;
        std::vector<Resource::SharedPtr> resources;
        resources.reserve(mCacheList.size());
        for (CacheEntry *entry = mCacheList.first(); entry; entry = entry->next) {
            if (entry->resource) {
                resources.push_back(entry->resource);
            }
        }
        for (std::vector<Resource::SharedPtr>::const_iterator it = resources.begin(); it != resources.end(); ++it) {
            (*it)->abort();
        }
    }
    if (mHttpServiceThread)
        mHttpServiceThread->stop();
    if (mDiskCache)
        mDiskCache->shutdown();

    shared_ptr<HttpServiceThread> httpServiceThread;
    shared_ptr<DiskCache> diskCache;
    {
        ScopedMutex lock(sMutex);
        mState = Finished;
        std::swap(mHttpServiceThread, httpServiceThread);
        std::swap(mDiskCache, diskCache);
    }
}

std::string ResourceManager::findResponseHeader(const std::string &name, const HttpResponseHeaders &headers)
{
    std::string result;
    for (size_t i = 0; i < headers.size(); ++i) {
        const std::pair<std::string, std::string> &header = headers[i];
        if (!strncasecmp(header.first.c_str(), name.c_str(), name.size())) {
            if (!header.second.empty()) {
                if (!result.empty() && header.first == "Cookie") {//merge
                    result += "; " + header.second;
                } else {
                    result = header.second;
                }
            }
        }
    }
    return result;
}

std::string ResourceManager::findRequestHeader(const std::string &name, const HttpHeaders &headers)
{
    const HttpHeaders::const_iterator it = headers.find(name);
    return it == headers.end() ? std::string() : it->second;
}

AseUrl ResourceManager::fixRelativeUrl(const AseUrl &base, const AseUrl &url)
{
    if (url.IsEmpty())
        return url;

    AseUrl fixed = url;
    if (!url.HasScheme()) {
        fixed.Resolve(base);
    } else if (url.GetScheme() != "data") {
        fixed.RemoveDotSegmentsFromPath();
    }

    NTRACEIF(fixed.str() != url.str(), TRACE_RESOURCEMANAGER,
             "turned incomplete URL %s relative to %s into %s", url.c_str(), base.c_str(), fixed.c_str());

    return fixed;
}


void ResourceManager::touch(CacheEntry *entry) // lock always held
{
    assert(!mCacheList.isEmpty());
    mCacheList.moveToEnd(entry);
}

ResourceManager::CacheEntry *ResourceManager::remove(CacheMap::iterator it) // lock always held
{
    assert(it != mCacheMap.end());
    CacheEntry *entry = it->second;
    NTRACES(TRACE_RESOURCEMANAGER, "ResourceManager::remove " << entry->resource->url());
    assert(mCacheMap.find(it->first) != mCacheMap.end());
    mCacheMap.erase(it);
    CacheEntry *next = entry->next;
    mCacheList.remove(entry);
    if (entry->resource && entry->resource->mCompleteState & Resource::Complete) {
        mSize -= entry->resource->cost();
    }
    delete entry;
    return next;
}

Resource::SharedPtr ResourceManager::find(const DataBuffer &cacheKey)
{
    ScopedMutex lock(sMutex);
    CacheMap::iterator it = mCacheMap.find(cacheKey);
    if (it != mCacheMap.end()) {
        Resource::SharedPtr resource = it->second->resource;
        if (resource->mCompleteState & Resource::Complete) {
            if (resource->isExpired()) {
                remove(it);
            } else {
                touch(it->second);
                INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("resourcemanager", "hits");
                return resource;
            }
        }
    }

    return Resource::SharedPtr();
}

void ResourceManager::touch(const std::vector<DataBuffer> &cacheKeys)
{
    ScopedMutex lock(sMutex);
    for (std::vector<DataBuffer>::const_iterator it = cacheKeys.begin(); it != cacheKeys.end(); ++it) {
        const DataBuffer &cacheKey = (*it);
        CacheMap::iterator cit = mCacheMap.find(cacheKey);
        if (cit != mCacheMap.end() && !(cit->second->resource->mCompleteState & Resource::Complete))
            touch(cit->second);
    }
}

void ResourceManager::touch(const DataBuffer &cacheKey)
{
    ScopedMutex lock(sMutex);
    CacheMap::iterator it = mCacheMap.find(cacheKey);
    if (it != mCacheMap.end() && it->second->resource->mCompleteState & Resource::Complete)
        touch(it->second);
}

Resource::SharedPtr ResourceManager::findResource(const AseUrl &url, const DataBuffer &cacheKey) // sMutex always held
{
    CacheMap::iterator it = mCacheMap.find(cacheKey);
    if (it == mCacheMap.end()) {
        report(Result_Miss, url);
        return Resource::SharedPtr();
    }

    CacheEntry *entry = it->second;
    assert(entry);
    assert(entry->resource);
    if (entry->resource->isExpired()) {
        remove(it);
        report(Result_Expired, url);
        return Resource::SharedPtr();
    }

    if (entry->resource->mCompleteState & Resource::Complete) {
        touch(entry);
        report(Result_Hit, url);
    } else {
        report(Result_Miss, url);
    }
    return entry->resource;
}

void ResourceManager::sendRequest(const ResourceRequest::SharedPtr &request)
{
    request->setResourceManager(shared_from_this());
    INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("gibbon.request", request.get()));
    startRequest(request);
    if (request->flags() & ResourceRequest::Synchronous) {
        request->wait();
    }
}

void ResourceManager::startRequest(const ResourceRequest::SharedPtr &request)
{
    PERFORMANCE_MARKER_SCOPED("resourcemanager.startrequest");

    NTRACE(TRACE_RESOURCEMANAGER, "Sending request %s (%p)\n", request->url().c_str(), request.get());

    const AseUrl url = request->url();
    const std::string scheme = url.GetScheme();
    if (scheme == "file") {
        handleFileRequest(url.GetPath(), request);
        return;
    } else if (scheme == "data") {
        handleDataRequest(request);
        return;
    }

    if (!request->hasRequestHeader("Accept-Encoding")) {
        if (Configuration::resourceManagerGzipCompressionEnabled()) {
            request->setRequestHeader("Accept-Encoding", "deflate, gzip");
        } else {
            request->setRequestHeader("Accept-Encoding", "deflate");
        }
    }
    if (!request->hasRequestHeader("Connection"))
        request->setRequestHeader("Connection", "Keep-Alive");

    Resource::SharedPtr resource = request->resource();
    assert(!resource || !resource->isComplete());
    shared_ptr<ResourceManagerHttpServiceRequest> hstRequest;
    if (!resource) {
        CacheData cacheData;
        cacheData.key = request->cacheKey();
        ResourceManager::parseCacheData(cacheData,
                                        request->url(),
                                        request->method(),
                                        request->requestHeaders(),
                                        request->requestData());
        NRDP_OBJECTCOUNT_DESCRIBE(cacheData.key, "RESOURCEMANAGER_CACHEKEY:"+request->url().str());
        request->setCacheKey(cacheData.key);
        bool shuttingDown;
        DiskCache::SharedPtr diskCache;
        bool createdResource = false;
        {
            ScopedMutex lock(sMutex);
            if (cacheData.mode & CacheMode_MemoryCache) {
                resource = findResource(url, cacheData.key);
                NTRACE(TRACE_RESOURCEMANAGER, "Finding resource for %s %s => %p",
                       url.c_str(), ResourceManager::prettyCacheKey(cacheData.key).c_str(),
                       resource.get());
            }
            if (!resource) {
                createdResource = true;
                resource = createResource(request);
                if (mCacheMap.find(cacheData.key) == mCacheMap.end()) {
                    // if there already is an entry in there with this cache key but
                    // someone requested the same resource with no-memory-cache we don't
                    // create a new CacheEntry
                    CacheEntry *entry = new CacheEntry;
                    entry->prev = 0;
                    entry->next = 0;
                    entry->resource = resource;
                    assert(resource);
                    entry->cacheKey = cacheData.key;
                    mCacheMap[cacheData.key] = entry;
                    mCacheList.append(entry);
                    NTRACE(TRACE_RESOURCEMANAGER, "Created cache entry for %s for joining purposes", url.c_str());
                } else {
                    NTRACE(TRACE_RESOURCEMANAGER, "Did not create cache entry for %s because it was already in the cache", url.c_str());
                }
            }
            shuttingDown = mState != Running;
            diskCache = mDiskCache;
        }
        NTRACE(TRACE_RESOURCEMANAGER, "Looking for [%s](%s) %s.\n", url.c_str(),
               ResourceManager::prettyCacheKey(cacheData.key).constData(),
               resource ? (resource->isComplete() ? "Found it in the resource cache (complete)" : "Found it in the resource cache)")
               : (cacheData.mode & CacheMode_MemoryCache ? "Did not find it in the resource cache" : "Did not want resource cache"));

        if (shuttingDown) {
            finish(resource, AS_NO_ERROR, 500, Resource::State_Network);
            return;
        }

        if (!createdResource) {
            request->setFlag(ResourceRequest::Joined);
            request->setResource(resource);
            if (!resource->addRequest(request))
                request->startTimeoutTimer();
            NTRACE(TRACE_RESOURCEMANAGER, "Joined existing request for %s cacheKey: %s (%d requests/complete %d)",
                   request->url().c_str(), ResourceManager::prettyCacheKey(cacheData.key).c_str(),
                   resource->requestCount(), resource->isComplete());
            return;
        }

        request->startTimeoutTimer();
        if (sendDiskCacheRequest(diskCache, request, resource, cacheData)) {
            return;
        }

        if (cacheData.mode & CacheMode_OnlyIfCached) {
            finish(resource, AS_NO_ERROR, 404, Resource::State_DiskCache);
            return;
        }
    } else {
        bool shuttingDown;
        {
            ScopedMutex lock(sMutex);
            shuttingDown = mState != Running;
        }
        if (shuttingDown) {
            finish(resource, AS_NO_ERROR, 500, Resource::State_Network);
            return;
        }
    }

    hstRequest.reset(new ResourceManagerHttpServiceRequest(resource, request));
    hstRequest->mResourceManager = shared_from_this();
    assert(hstRequest);
    resource->mNetworkRequest = hstRequest;
    sendNetworkRequest(hstRequest);
}

bool ResourceManager::sendDiskCacheRequest(const DiskCache::SharedPtr &diskCache,
                                           const ResourceRequest::SharedPtr &request,
                                           const Resource::SharedPtr &resource,
                                           const CacheData &cacheData)
{
    if ((cacheData.mode & (CacheMode_DiskCache|CacheMode_Fresh)) == CacheMode_DiskCache && diskCache) {
        shared_ptr<ResourceManagerDiskCacheRequest> req;
        const DiskCache::Response::Type allowedType = diskCache->allowedType(request);
        req.reset(new ResourceManagerDiskCacheRequest(request->url(), allowedType, cacheData, resource, request));
        resource->mDiskCacheRequest = req;
        if (diskCache->sendRequest(req))
            return true;
        diskCache->report(DiskCache::Miss, req->url());
    }
    return false;
}

DiskCache::SharedPtr ResourceManager::diskCache() const
{
    ScopedMutex lock(sMutex);
    return mDiskCache;
}

void ResourceManager::configureDiskCache(int capacity, int maxPending, int catalogTimer)
{
    shared_ptr<DiskCache> shutdown;
    {
        ScopedMutex lock(sMutex);
        if (mDiskCache) {
            if (!capacity) {
                std::swap(shutdown, mDiskCache);
            } else {
                mDiskCache->configure(capacity, maxPending, catalogTimer);
            }
        } else if (capacity) {
            mDiskCache = createDiskCache();
            if (mDiskCache)
                mDiskCache->init(capacity, maxPending, catalogTimer);
        }
    }
    if (shutdown)
        shutdown->shutdown();
}

std::list<Resource::SharedPtr> ResourceManager::find(const DumpInfo &dump) const
{
    ScopedMutex lock(sMutex);
    std::list<Resource::SharedPtr> result;
    int resource_count = 0;
    for (CacheEntry *e = mCacheList.first(); e; e = e->next) {
        if (dump.empty() || resource_count == dump.idx || e->resource.get() == dump.ptr ||
            (dump.match.length() && contains(e->resource->url().str(), dump.match)))
            result.push_back(e->resource);
        ++resource_count;
    }
    return result;
}

class ResourceManagerDumper : public Dumper
{
public:
    ResourceManagerDumper() : mFlags(0) { }

    void setResource(const shared_ptr<Resource> resource, unsigned int f) { mResource = resource; mFlags = f; }
    Resource::SharedPtr resource() const { return mResource; }
    unsigned int flags() const { return mFlags; }

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value);
    virtual std::vector<std::string> consolePostProcess(const Variant& values);

private:
    unsigned int mFlags;
    Resource::SharedPtr mResource;
};

std::vector<std::string> ResourceManagerDumper::consoleEntry(const unsigned int /*idx*/, const std::string& /*section*/, const Variant& value)
{
    std::vector<std::string> ret;
    const llong inflight = value["inflight"].integer();
    if (inflight) {
        ret.push_back(StringFormatter::sformat(" [%lld] [%p] inflight:%lld %s",
                                               value["idx"].integer(),
                                               value["ptr"].pointer(),
                                               inflight,
                                               value["description"].string().c_str()));
    } else {
        ret.push_back(StringFormatter::sformat(" [%lld] [%p] status:%lld expires:%llds downloaded:%lldms %s",
                                               value["idx"].integer(),
                                               value["ptr"].pointer(),
                                               value["status"].integer(),
                                               value["expiration"].integer(),
                                               value["downloaded"].integer(),
                                               value["description"].string().c_str()));
    }
    return ret;
}

std::vector<std::string> ResourceManagerDumper::consolePostProcess(const Variant& values)
{
    const double total = values["total"].dbl();
    return one(StringFormatter::sformat("Resource Cache: Hits: %s (%.1f%%) Misses: %s (%.1f%%) Expired: %s (%.1f%%) Matched: %s(%s) Size: %s/%s (%d%%)",
                                        prettyNumber(values["hits"].integer()).c_str(),
                                        total != 0. ? (values["hits"].integer() * 100. / total) : 0.,
                                        prettyNumber(values["misses"].integer()).c_str(),
                                        total != 0. ? (values["misses"].integer() * 100. / total) : 0.,
                                        prettyNumber(values["expired"].integer()).c_str(),
                                        total != 0. ? (values["expired"].integer() * 100. / total) : 0.,
                                        prettyBytes(values["matchedSize"].integer()).c_str(),
                                        prettyNumber(values["matched"].integer()).c_str(),
                                        prettyBytes(values["cacheSize"].integer()).c_str(),
                                        prettyBytes(values["capacity"].integer()).c_str(),
                                        static_cast<int>((values["totalSize"].integer() / values["capacity"].value<double>()) * 100.)));
}

shared_ptr<ResourceManagerDumper> ResourceManager::dumper(const DumpInfo& dump) const
{
    shared_ptr<ResourceManagerDumper> dumper(new ResourceManagerDumper);
    dumper->keys() << "inflight" << "idx" << "ptr" << "description" << "size" << "status" << "expiration"
                   << "downloaded" << "url" << "cacheKey";

    ScopedMutex lock(sMutex);
    int index = 0, matched = 0, matchedSize = 0;
    for (CacheEntry *e = mCacheList.first(); e; e = e->next) {
        shared_ptr<Resource> r = e->resource;
        const int size = r->cost();
        int inflight = 0;
        inflight = r->mRequests.size();
        enum { Matched, MatchedDump, NotMatched } match = NotMatched;
        if (index == dump.idx || r.get() == dump.ptr) {
            match = MatchedDump;
        } else if ((!dump.ptr && dump.idx == -1 && !dump.match.length()) || (dump.match.length() && contains(r->url().str(), dump.match))) {
            match = Matched;
        }
        if (match != NotMatched) {
            if (inflight) {
                dumper->values() << inflight << index << r.get() << r->describe()
                                 << 0 << 0 << 0 << (Time::mono() - r->downloaded()).ms()
                                 << r->url().str() << r->cacheKey();
            } else {
                dumper->values() << 0 << index << r.get() << r->describe()
                                 << r->cost() << r->httpStatusCode()
                                 << (r->expiration() - Time::serverTime()).seconds()
                                 << r->downloaded().ms() << r->url().str()
                                 << r->cacheKey();
                if (match == MatchedDump) {
                    dumper->setResource(r, dump.flags);
                }
            }
            matchedSize += size;
            ++matched;
        }
        ++index;
    }
    const double total = mHits + mMisses + mExpired;
    Dumper& d = *dumper;
    d["total"] = total;
    d["hits"] = mHits;
    d["misses"] = mMisses;
    d["expired"] = mExpired;
    d["matched"] = matched;
    d["cacheSize"] = mSize;
    d["matchedSize"] = matchedSize;
    d["capacity"] = mCapacity;

    return dumper;
}

void ResourceManager::dump(const DumpInfo &dump) const
{
    shared_ptr<ResourceManagerDumper> d = dumper(dump);
    const std::vector<std::string>& data = d->toConsole();
    for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
        Log::warn(TRACE_RESOURCEMANAGER, "%s", it->c_str());
    }
    const Resource::SharedPtr& r = d->resource();
    if (r) {
        for (size_t i = 0; i < r->mResponseHeaders.size(); ++i) {
            Log::warn(TRACE_RESOURCEMANAGER, " ResponseHeader(%zu): %s: %s", i,
                      r->mResponseHeaders[i].header.c_str(),
                      r->mResponseHeaders[i].value.c_str());
        }

        const unsigned int flags = d->flags();
        if (flags & DumpInfo::Data) {
            if (flags & DumpInfo::Raw) {
                Log::warn(TRACE_RESOURCEMANAGER, " Data\n%s", r->mResponseData.c_str());
            } else {
                Log::warn(TRACE_RESOURCEMANAGER, " Data\n%s", Log::hexDump(r->mResponseData).c_str());
            }
        }
    }
}

Variant ResourceManager::resourceInfo() const
{
    shared_ptr<ResourceManagerDumper> d = dumper();
    return d->variant();
}

static inline std::string prettyTimeout(const char *type, const AseTimeVal &timeout)
{
    assert(type);
    if (timeout != AseTimeVal::ZERO) {
        char buf[128];
        const int w = snprintf(buf, sizeof(buf), " %s: %llums", type, timeout.ms());
        return std::string(buf, w);
    }
    return std::string();
}

void ResourceManager::dumpNetwork(const DumpInfo &dump)
{
    ScopedMutex lock(sMutex);
    int i = 0, matched = 0;
    if (dump.flags & Dump_Requests) {
        for (std::set<shared_ptr<ResourceManagerHttpServiceRequest> >::const_iterator it = mNetworkRequests.begin();
             it != mNetworkRequests.end(); ++it) {
            const shared_ptr<ResourceManagerHttpServiceRequest> &req = *it;
            if (dump.empty()) {
                ++matched;
                const ullong elapsed = (Time::mono() - req->mStarted).ms();
                Log::warn(TRACE_RESOURCEMANAGER, " [%d/%p] %s [%lldms]",
                          i, it->get(), req->mRequestUrl.c_str(), elapsed);
            } else if (dump.idx == i || (dump.match.length() && contains(req->mRequestUrl.str(), dump.match)) || dump.ptr == req.get()) {
                ++matched;
                const ullong elapsed = (Time::mono() - req->mStarted).ms();
                Log::warn(TRACE_RESOURCEMANAGER, " [%d/%p] %s [%lldms]%s%s%s",
                          i, req.get(), req->mRequestUrl.c_str(), elapsed,
                          prettyTimeout("timeout", req->mTimeout).c_str(),
                          prettyTimeout("connectTimeout", req->mConnectTimeout).c_str(),
                          prettyTimeout("stallTimeout", req->mStallTimeout).c_str());
                int headerIdx = 0;
                for (HttpHeaders::const_iterator header = req->mRequestHeaders.begin(); header != req->mRequestHeaders.end(); ++header) {
                    Log::warn(TRACE_RESOURCEMANAGER, "   [%d/%zu] %s: %s",
                              ++headerIdx, req->mRequestHeaders.size(),
                              header->first.c_str(), header->second.c_str());
                }
                if (!req->mRequestBody.isEmpty()) {
                    Log::warn(TRACE_RESOURCEMANAGER, "   Body:\n%s", req->mRequestBody.c_str());
                }
            }
            ++i;
        }
        Log::warn(TRACE_RESOURCEMANAGER, "Network: (%d/%zu)", matched, mNetworkRequests.size());
    }
    if (dump.flags & Dump_HST)
        mHttpServiceThread->scheduleDump();
}

void ResourceManager::setCapacity(int s)
{
    assert(s >= 0);
    ScopedMutex lock(sMutex);
    mCapacity = s;
    purge_internal();
}

int ResourceManager::purge()
{
    ScopedMutex lock(sMutex);
    return purge_internal();
}

int ResourceManager::purge_internal() // lock always held
{
    int purged = 0;
    int purgeCount = 0;
    CacheEntry *e = mCacheList.first();
    while (e && mSize > mCapacity) {
        if (e->resource->mCompleteState & Resource::Complete) {
            const int cost = e->resource->cost();
            NTRACE(TRACE_RESOURCEMANAGER, "Purged %s (%s)",
                   e->resource->url().c_str(), ResourceManager::prettyCacheKey(e->resource->cacheKey()).constData());
            e = remove(mCacheMap.find(e->cacheKey));
            purged += cost;
            ++purgeCount;
        } else {
            e = e->next;
        }
    }
    if (purged) {
        INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("resourcemanager", "purged", purged);
        NTRACES(TRACE_RESOURCEMANAGER,
                "ResourceManager:purged " << purgeCount << "/" << mCacheMap.size() + purgeCount << " items " <<
                purged << "/" <<  mSize + purged);
    }
    return purged;
}

void ResourceManager::clear()
{
    ScopedMutex lock(sMutex);
    mCacheList.deleteAll();
    mCacheMap.clear();
    mHits = mMisses = mExpired = mSize = 0;
}

void ResourceManager::setTrustStoreData(const DataBuffer &trustStore)
{
    ScopedMutex lock(sMutex);
    mTrustStore.reset(new TrustStore(trustStore));
}

bool ResourceManager::sendNetworkRequest(const shared_ptr<ResourceManagerHttpServiceRequest> &hstRequest)
{
    uint32_t id;
    {
        ScopedMutex lock(sMutex);
        if (!mHttpServiceThread)
            return false;
        if (hstRequest->mRequestUrl.IsHttps()) {
            hstRequest->mResource->mHSTRequestId = mHttpServiceThread->addHttpsRequest(hstRequest,
                                                                                       mTrustStore,
                                                                                       ResourceManagerHttpServiceRequest::sslVerifyCallback,
                                                                                       NetConfiguration::getSSLPeerVerification());
        } else {
            hstRequest->mResource->mHSTRequestId = mHttpServiceThread->addHttpRequest(hstRequest);
        }
        id = hstRequest->mResource->mHSTRequestId;
        if (id) {
            mNetworkRequests.insert(hstRequest);
        }
    }
    NTRACE(TRACE_RESOURCEMANAGER, "ResourceManager::Sending network request %s (0x%x) -> %u\n",
           hstRequest->mRequestUrl.c_str(), hstRequest->mRequestFlags, id);

    if (!id) {
        finish(hstRequest->mResource, AS_REQUEST_CREATION_FAILURE, 400);
        return false;
    }
    return true;
}

void ResourceManager::finish(const Resource::SharedPtr &resource, int nativeErrorCode, int httpStatusCode,
                             Resource::State state, const NetworkMetrics &metrics, const HttpResponseHeaders &responseHeaders,
                             const DataBuffer &responseData)
{
    PERFORMANCE_MARKER_SCOPED("resourcemanager.finish");

#ifdef NRDP_HAS_TRACING
    const bool logResponse = (responseData.size() <= 64 * 1024
                              && !responseData.isBinary()
                              && !contains(ResourceManager::findResponseHeader("Content-Type", responseHeaders), "image"));

    NTRACE(TRACE_RESOURCEMANAGER, "Finish %s (%d) status: %d state: %s data: %d%c%s Resource: %p\n", resource->url().c_str(),
           static_cast<int>(resource->urls().size()), httpStatusCode, Resource::stateToString(state),
           responseData.size(), logResponse ? '\n' : ' ', logResponse ? responseData.c_str() : "N/A", resource.get());
#endif
    assert(state != Resource::State_MemoryCache);
    CacheData data;
    ResourceRequest::SharedPtr redirect;
    {
        ScopedMutex lock(sMutex);
        if (resource->mRequests.empty() || resource->mCompleteState) {
            NTRACE(TRACE_RESOURCEMANAGER, "%s was apparently aborted", resource->url().c_str());
            return;
        }
        ResourceRequest::SharedPtr request = *resource->mRequests.begin();
        data.key = request->cacheKey();
        data.expiration = resource->expiration();
        ResourceManager::parseCacheData(data,
                                        request->url(),
                                        request->method(),
                                        request->requestHeaders(),
                                        request->requestData(),
                                        responseHeaders);

        resource->mHSTRequestId = 0;
        resource->mDownloaded = Time::mono() - resource->mDownloaded;

        resource->mHttpStatusCode = httpStatusCode;
        const std::string sslVersion = resource->mNetworkMetrics.sslVersion();
        resource->mNetworkMetrics = metrics;
        resource->mNetworkMetrics.setSSLVersion(sslVersion);
        resource->mNetworkMetrics.setDuration(resource->mDownloaded.val());
        resource->mResponseHeaders = responseHeaders;
        resource->mResponseData = responseData;
        resource->mExpiration = data.expiration;
        resource->mMaxAge = data.maxAge;
        resource->mNativeErrorCode = nativeErrorCode;
        const NetErrorCode::Code errorCode = NetError::nativeErrorCodeToNetError(nativeErrorCode);
        if (errorCode != NetErrorCode::SUCCESS || httpStatusCode >= 400)
            data.mode = CacheMode_NoCache;
        resource->mErrorCode = errorCode;

        enum { MaxRedirects = 10 };
        if (errorCode == NetErrorCode::SUCCESS && httpStatusCode >= 300 && httpStatusCode < 400
            && httpStatusCode != 304 && resource->urls().size() < MaxRedirects) {
            const std::string location = findResponseHeader("Location", responseHeaders);
            if (!location.empty()) {
                NTRACE(TRACE_RESOURCEMANAGER, "Redirected %s => %s", resource->url().c_str(),
                       ResourceManager::fixRelativeUrl(resource->url(), location).c_str());
                const AseUrl redirectUrl = ResourceManager::fixRelativeUrl(resource->url(), location);
                resource->addUrl(redirectUrl);
                ResourceRequest::SharedPtr req;
                for (std::set<shared_ptr<ResourceRequest> >::const_iterator it = resource->mRequests.begin();
                     it != resource->mRequests.end(); ++it) {
                    ResourceRequest::SharedPtr request = *it;
                    if (!redirect)
                        redirect = request;
                    request->mUrl = redirectUrl;
                }
#ifdef NRDP_HAS_SWITCHED_INSTRUMENTATION
                // must be a redirect, end instrumentation before a new one starts
                const std::vector<AseUrl> urls = request->resource()->urls();
                assert(urls.size() >= 2);
                instrument(request, request->resource(), urls.at(urls.size() - 2), state);
                INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("gibbon.request", request.get()));
#endif
            }
        }
        if (!redirect) {
            const CacheMap::iterator it = mCacheMap.find(data.key);

            assert(!resource->mCompleteState);
            if (it != mCacheMap.end()) {
                if (data.mode & CacheMode_MemoryCache) {
                    resource->mExpiration = data.expiration;
                    resource->mCompleteState = Resource::PendingComplete;
                    const int cost = resource->cost();
                    if (cost > mCapacity) {
                        remove(it);
                    } else {
                        resource->mCompleteState |= Resource::AddToCache;
                    }
                } else if (it->second->resource == resource) {
                    remove(it);
                }
            }
            NTRACE(TRACE_RESOURCEMANAGER, "Finished %s for %zu requests", resource->url().c_str(), resource->mRequests.size());
        }
    }

    if (mDiskCache) {
        if (!(data.mode & CacheMode_DiskCache)) {
            mDiskCache->remove(data.key);
        } else if (state == Resource::State_Network || state == Resource::State_Network_Validated) {
            mDiskCache->addResource(data.key, httpStatusCode, resource->url(),
                                    responseHeaders, responseData, data.priority, data.expiration, data.maxAge);
        }
    }

    if (redirect) {
        requestRedirected(redirect, resource);
    } else {
        resourceFinished(resource, state);
    }
}

void ResourceManager::requestRedirected(const ResourceRequest::SharedPtr &redirect, const Resource::SharedPtr &)
{
     INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("gibbon.request", redirect.get()));
     startRequest(redirect);
}

void ResourceManager::resourceFinished(const Resource::SharedPtr &resource, Resource::State state)
{
    while (true) {
        shared_ptr<ResourceRequest> request;
        {
            ScopedMutex lock(sMutex);
            if (resource->mRequests.empty()) {
                if (resource->mCompleteState & Resource::AddToCache) {
                    CacheMap::iterator it = mCacheMap.find(resource->mCacheKey);
                    if (it != mCacheMap.end()) {
                        mSize += resource->cost();
                        touch(it->second);
                        purge_internal();
                    }
                }
                resource->mCompleteState = Resource::Complete;
                sCondition.broadcast();
                break;
            }
            request = *resource->mRequests.begin();
            resource->mRequests.erase(resource->mRequests.begin());
            if (EventLoop::Timer::SharedPtr timer = request->mTimeoutTimer.lock()) {
                timer->stop();
                request->mTimeoutTimer.reset();
            }
        }
        NTRACE(TRACE_RESOURCEMANAGER, "Calling requestFinished for %s %p resource: %p", resource->url().c_str(), request.get(), resource.get());
        instrument(request, resource, resource->url(), state);
        requestFinished(request, resource, state);
        request->onComplete(resource, state);
    }
}

void ResourceManager::report(Result result, const AseUrl &/*url*/) // lock always held
{
    const char *res = "";
    switch (result) {
    case Result_Hit: ++mHits; res = "hits"; break;
    case Result_Miss: ++mMisses; res = "misses"; break;
    case Result_Expired: ++mExpired; res = "expired"; break;    }
    INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("resourcemanager", res);
}

bool ResourceManager::handleFileRequest(const std::string &fileName, const shared_ptr<ResourceRequest> &request)
{
    Stopwatch sw;
    Resource::SharedPtr resource = createResource(request);

    struct stat statBuf;
    if (!::stat(fileName.c_str(), &statBuf) && !S_ISDIR(statBuf.st_mode)) {
        const Time mtime = Time::fromSeconds(statBuf.st_mtime);
        if (mtime.ms()) {
            const std::string if_modified_since = request->requestHeader("If-Modified-Since");
            if (!if_modified_since.empty() && Time::fromRFC1123(if_modified_since) >= mtime) {
                finish(resource, AS_NO_ERROR, 304, Resource::State_File,
                       NetworkMetrics(0, 0, static_cast<uint32_t>(sw.elapsed())));
                return true;
            }
        }

        bool ok;
        const DataBuffer buffer = DataBuffer::fromFile(fileName.c_str(), -1, &ok);
        if (ok) {
            Log::info(TRACE_RESOURCEMANAGER, "allowing filesystem access for %s (%s)", request->url().c_str(), fileName.c_str());
            HttpResponseHeaders headers;
            if (mtime.ms())
                headers.push_back(std::make_pair("Last-Modified", mtime.rfc1123()));
            const std::string mimetype = mimeType(request->url());
            if (!mimetype.empty())
                headers.push_back(std::make_pair("Content-Type", mimetype));
            finish(resource, AS_NO_ERROR, 200, Resource::State_File,
                   NetworkMetrics(0, 0, static_cast<uint32_t>(sw.elapsed())),
                   headers, buffer);
            return true;
        }
    }
    finish(resource, AS_NO_ERROR, 404, Resource::State_File);
    return false;
}

bool ResourceManager::handleDataRequest(const shared_ptr<ResourceRequest> &request)
{
    Stopwatch sw;
    Resource::SharedPtr resource = createResource(request);
    const std::string uri = request->url().str();
    assert(startsWith(uri, "data:"));
    const std::string::size_type dataOffset = uri.find(',');
    if (dataOffset != std::string::npos) {
        std::string mimeType, charset;
        bool base64 = false;

        // We have a data part, now iterate through all semicolon separated parts
        std::string::size_type semi = 4, lastSemi;
        bool first = true;
        while (true) {
            if (semi == dataOffset)
                break;

            lastSemi = semi + 1;
            semi = std::min(uri.find(';', lastSemi), dataOffset);
            std::string part = uri.substr(lastSemi, semi - lastSemi);
            if (first) {
                // the first part is always the mime type
                mimeType = part;
                first = false;
            } else if (part == "base64") {
                base64 = true;
            } else if (startsWith(part, "charset=")) {
                charset = part;
            }
        }

        if (mimeType.empty()) {
            // Use RFC2397 default values
            mimeType = "text/plain";
            if (charset.empty())
                charset = "charset=US-ASCII";
        }

        HttpResponseHeaders headers;
        if (!charset.empty())
            mimeType += "; " + charset;
        headers.push_back(std::make_pair("Content-Type", mimeType));
        const DataBuffer src = DataBuffer::fromRawData(reinterpret_cast<const unsigned char*>(uri.c_str()) + dataOffset + 1, uri.size() - dataOffset - 1);
        const DataBuffer buffer = src.decode(base64 ? DataBuffer::Encoding_Base64 : DataBuffer::Encoding_Url);
        finish(resource, AS_NO_ERROR, 200, Resource::State_Data,
               NetworkMetrics(0, 0, static_cast<uint32_t>(sw.elapsed())),
               headers, buffer);
        return true;
    }
    finish(resource, AS_NO_ERROR, 404, Resource::State_Data);
    return true;
}

void ResourceManager::removeNetworkRequest(const shared_ptr<ResourceManagerHttpServiceRequest> &request)
{
    Resource::SharedPtr resource; // resource should die without holding sMutex
    {
        ScopedMutex lock(sMutex);
        mNetworkRequests.erase(request);
        std::swap(request->mResource, resource);
    }
}

struct Control {
    enum Type {
        Priority,
        MaxAge,
        Refresh,
        NoCache,
        NoDiskCache,
        NoMemoryCache,
        Fresh,
        OnlyIfCached,
        Key
    } const type;

    const std::string string;

    enum Flag {
        NoFlag = 0x0,
        Value = 0x1,
        NoServer = 0x2
    };
    const unsigned int flags;
} static const sControls[] = {
    { Control::Priority, "priority=", Control::Value },
    { Control::MaxAge, "max-age=", Control::Value },
    { Control::Refresh, "refresh=", Control::Value },
    { Control::Refresh, "refresh", Control::NoFlag },
    { Control::NoCache, "no-cache", Control::NoFlag },
    { Control::NoDiskCache, "no-disk-cache", Control::NoFlag },
    { Control::NoMemoryCache, "no-memory-cache", Control::NoFlag },
    { Control::Fresh, "fresh", Control::NoFlag },
    { Control::OnlyIfCached, "only-if-cached", Control::NoFlag },
    { Control::Key, "key=", Control::Value|Control::NoServer }
};

static inline void processCacheControl(const std::string &controlString,
                                       std::map<Control::Type, std::string> &map,
                                       const HttpResponseHeaders *responseHeaders)
{
    std::vector<std::string> controls;
    if (!controlString.empty()) {
        StringTokenizer::tokenize(controlString, controls, ",");
    } else if (responseHeaders) {
        controls.push_back("server");
    }

    const bool fromServer = (responseHeaders == 0);
    for (std::vector<std::string>::iterator it = controls.begin(); it != controls.end(); ++it) {
        std::string str = *it;
        trim(str);
        int found = -1;
        for (size_t i=0; i<sizeof(sControls) / sizeof(sControls[0]); ++i) {
            if (fromServer && (sControls[i].flags & Control::NoServer))
                continue;
            if (sControls[i].flags & Control::Value) {
                if (startsWith(str, sControls[i].string)) {
                    std::string &val = map[sControls[i].type];
                    if (val.empty()) {
                        val = str.substr(sControls[i].string.length());
                        trim(val);
                    }
                    found = i;
                    break;
                }
            } else if (str == sControls[i].string) {
                map.insert(std::make_pair(sControls[i].type, std::string()));
                found = i;
                break;
            }
        }
        if (found == -1 && responseHeaders && str == "server") {
            const std::string cacheControl = ResourceManager::findResponseHeader("Cache-Control", *responseHeaders);
            if (!cacheControl.empty()) {
                processCacheControl(cacheControl, map, 0);
                responseHeaders = 0;
            }
        }
    }
}

void ResourceManager::parseCacheData(CacheData &data, const AseUrl &url, Resource::Method op,
                                     const HttpHeaders &requestHeaders, const DataBuffer &postData,
                                     const HttpResponseHeaders &responseHeaders)
{
    std::map<Control::Type, std::string> controls;
    const HttpHeaders::const_iterator cacheControl = requestHeaders.find("X-Gibbon-Cache-Control");
    ::processCacheControl(cacheControl == requestHeaders.end() ? std::string() : cacheControl->second,
                          controls, &responseHeaders);
    if (op == Resource::Method_Delete) {
        data.mode = CacheMode_NoCache;
    } else {
        data.mode = CacheMode_DiskCache;
        if (postData.empty() || responseHeaders.empty())
            data.mode |= CacheMode_MemoryCache;

        if (const ullong maxAge = strtoul(mapValue(controls, Control::MaxAge).c_str(), 0, 10)) {
            data.maxAge = maxAge;
            data.mode |= CacheMode_MemoryCache;
        } else {
            const std::string expires = ResourceManager::findResponseHeader("Expires", responseHeaders);
            if (!expires.empty()) {
                data.expiration = Time::fromRFC1123(expires);
                if (data.expiration.val())
                    data.mode |= CacheMode_MemoryCache;
            }
        }

        if (!controls.empty()) {
            if (controls.find(Control::NoCache) != controls.end()) {
                data.mode = CacheMode_NoCache;
            } else {
                if (controls.find(Control::NoDiskCache) != controls.end())
                    data.mode &= ~CacheMode_DiskCache;
                if (controls.find(Control::NoMemoryCache) != controls.end())
                    data.mode &= ~CacheMode_MemoryCache;
                if (data.mode) {
                    if (controls.find(Control::OnlyIfCached) != controls.end()) {
                        data.mode |= CacheMode_OnlyIfCached;
                        // only-if-cached and fresh are mutually exclusive, give
                        // only-if-cached precedence
                    } else if (controls.find(Control::Fresh) != controls.end()) {
                        data.mode |= CacheMode_Fresh;
                    }
                }
            }
        }

        const std::map<Control::Type, std::string>::const_iterator refresh = controls.find(Control::Refresh);
        if (refresh != controls.end())
            data.refresh = std::max(-1, atoi(refresh->second.c_str()));

        const std::map<Control::Type, std::string>::const_iterator priority = controls.find(Control::Priority);
        if (priority != controls.end())
            data.priority = std::max(0, atoi(priority->second.c_str()));
    }
    if (data.key.isEmpty()) {
        data.key = mapValue(controls, Control::Key);
        if (data.key.isEmpty()) {
            const HttpHeaders::const_iterator hash = requestHeaders.find("X-Gibbon-Hash");
            if (hash != requestHeaders.end()) {
                const size_t equals = hash->second.find('=');
                if (equals != std::string::npos)
                    data.key = hash->second.substr(equals + 1);;
            }
            if (data.key.isEmpty() && !generateCacheKey(data.key, url, op, requestHeaders, postData)) {
                data.key.clear();
                data.mode = CacheMode_NoCache;
            }
        }
    }
}

bool ResourceManager::generateCacheKey(DataBuffer &cacheKey, const AseUrl &url, Resource::Method op,
                                       const HttpHeaders &requestHeaders, const DataBuffer &postData)
{
    Stopwatch sw;
    SHA256_CTX context;
    if (!SHA256_Init(&context)) {
        Log::error(TRACE_RESOURCEMANAGER, "Error initializing SHA256_Init %d", errno);
        return false;
    }

    NTRACE(TRACE_RESOURCEMANAGER, "Generating cache key for %s", url.c_str());

    bool ok = false;
    if (SHA256_Update(&context, &op, sizeof(Resource::Method))
        && SHA256_Update(&context, url.c_str(), url.str().size())) {
        ok = true;
        for (HttpHeaders::const_iterator it = requestHeaders.begin(); it != requestHeaders.end(); ++it) {
            if (!strcasecmp("If-None-Match", it->first.c_str())
                || !strcasecmp("If-Modified-Since", it->first.c_str())
                || !strcasecmp("Referer", it->first.c_str())
                || !strcasecmp("X-Gibbon-Cache-Control", it->first.c_str())) {
                NTRACE(TRACE_RESOURCEMANAGER, "Skipped header [%s: %s]", it->first.c_str(), it->second.c_str());
                continue;
            }

            NTRACE(TRACE_RESOURCEMANAGER, "Added header [%s: %s]", it->first.c_str(), it->second.c_str());

            if (!SHA256_Update(&context, it->first.c_str(), it->first.size())
                || !SHA256_Update(&context, it->second.c_str(), it->second.size())) {
                Log::error(TRACE_RESOURCEMANAGER, "Failed to has header [%s: %s]", it->first.c_str(), it->second.c_str());
                ok = false;
                break;
            }
        }
        if (ok && !postData.isEmpty()) {
            NTRACE(TRACE_RESOURCEMANAGER, "Added postData [%s]", postData.c_str());
            ok = SHA256_Update(&context, postData.data(), postData.size());
        }
    }
    cacheKey.reserve(SHA256_DIGEST_LENGTH);
    cacheKey.setUsed(SHA256_DIGEST_LENGTH);
    if (!ok) {
        Log::error(TRACE_RESOURCEMANAGER, "SHA256 update error %d", errno);
        return false;
    }
    if (!SHA256_Final(cacheKey.data(), &context)) {
        Log::error(TRACE_RESOURCEMANAGER, "SHA256 final error %d", errno);
        return false;
    } else {
        NTRACE(TRACE_RESOURCEMANAGER, "Generated cache key for %s: %s", url.c_str(), cacheKey.toBase64().c_str());
    }

    NRDP_OBJECTCOUNT_DESCRIBE(cacheKey, "RESOURCEMANAGER_CACHEKEY:"+url.str());
    return true;
}

void ResourceManager::abortDiskCacheRequest(const shared_ptr<ResourceManagerDiskCacheRequest> &request)
{
    assert(request);
    request->abort();
}

void ResourceManager::waitForDiskCacheRequest(const shared_ptr<ResourceManagerDiskCacheRequest> &request)
{
    assert(request);
    request->wait();
}

// ========== ResourceManagerHttpServiceRequest ==========

ResourceManagerHttpServiceRequest::ResourceManagerHttpServiceRequest(const Resource::SharedPtr &resource,
                                                                     const ResourceRequest::SharedPtr &request,
                                                                     Resource::State state)
    : mRedirectCount(0), mStarted(Time::mono()), mResource(resource), mState(state),
      mCachedHttpStatusCode(-1), mRequestFlags(request->flags())
{
    switch (request->method()) {
    case Resource::Method_None:
        assert(0);
        // fall through
    case Resource::Method_Get:
        mRequestMethod = IAsyncHttpRequest::GET_METHOD;
        break;
    case Resource::Method_Post:
        mRequestMethod = IAsyncHttpRequest::POST_METHOD;
        mRequestBody = request->requestData();
        break;
    case Resource::Method_Head:
        mRequestMethod = IAsyncHttpRequest::HEAD_METHOD;
        break;
    case Resource::Method_Delete:
        mRequestMethod = IAsyncHttpRequest::DELETE_METHOD;
        break;
    }

    mRequestUrl = request->url();
    mRequestHeaders = request->requestHeaders();
    mTimeout = request->timeout().val() ?  AseTimeVal::fromMS(request->timeout().ms()) : AseTimeVal::INFINITE;
    mConnectTimeout = request->connectTimeout().val() ? AseTimeVal::fromMS(request->connectTimeout().ms()) : AseTimeVal::INFINITE;
    mStallTimeout = request->stallTimeout().val() ? AseTimeVal::fromMS(request->stallTimeout().ms()) : AseTimeVal::INFINITE;
    if (mRequestHeaders.find("Connection") != mRequestHeaders.end())
        mRequestHeaders["Connection"] = "Keep-Alive";
}

void ResourceManagerHttpServiceRequest::provideResponseHeaders(const HttpResponseHeaders &responseHeaders)
{
    mResponseHeaders = responseHeaders;
}

void ResourceManagerHttpServiceRequest::provideResponseBody(const DataBuffer& responseData)
{
    mResponseData = responseData;
}

void ResourceManagerHttpServiceRequest::finish(int nativeErrorCode, long httpStatusCode, HttpServiceMetrics *pHttpServiceMetrics)
{
    Resource::SharedPtr r = resource();
    NTRACE(TRACE_RESOURCEMANAGER, "ResourceManagerHttpServiceRequest::finish %s %d/%ld (%p)",
           mRequestUrl.c_str(), nativeErrorCode, httpStatusCode, r.get());
    if (!r) {
        mResourceManager->removeNetworkRequest(shared_from_this());
        return;
    }

    NetworkMetrics networkMetrics;
    if (pHttpServiceMetrics) {
        networkMetrics = NetworkMetrics(pHttpServiceMetrics->mDnsLookupTime,
                                        pHttpServiceMetrics->mHttpConnectTime,
                                        pHttpServiceMetrics->mHttpTransactionTime);
        networkMetrics.setServerIpAddress(pHttpServiceMetrics->mServerIpAddress);
        networkMetrics.setCName(pHttpServiceMetrics->mCName);
    }

    if (nativeErrorCode != AS_NO_ERROR) {
        httpStatusCode = 400;
    } else if (httpStatusCode == 304) {
        if (mState != Resource::State_DiskCacheRefresh)
            mState = Resource::State_Network_Validated;
        HttpResponseHeaders merged = mCachedResponseHeaders;
        const int originalMergedSize = merged.size();
        bool headersChanged = false;
        for (HttpResponseHeaders::const_iterator it = mResponseHeaders.begin(); it != mResponseHeaders.end(); ++it) {
            enum {
                NotFound,
                Same,
                Different
            } state = NotFound;
            for (int i=0; i<originalMergedSize; ++i) {
                HttpResponseHeaders::Header &old = merged[i];
                if (old.header == it->header) {
                    state = (old.value == it->value ? Same : Different);
                    if (state == Different) {
                        old.value = it->value;
                        headersChanged = true;
                    }
                    break;
                }
            }
            if (state == NotFound) {
                headersChanged = true;
                merged.push_back(*it);
            }
        }
        NTRACE(TRACE_RESOURCEMANAGER, "Got 304 for %s, different %d", mRequestUrl.c_str(), headersChanged);
        mResponseData = mCachedResponseData;
        if (headersChanged)
            mResponseHeaders = merged;
        httpStatusCode = mCachedHttpStatusCode;
    } else {
        NTRACE(TRACE_RESOURCEMANAGER, "Request %s got response headers:\n%s", mRequestUrl.c_str(), mResponseHeaders.toString().c_str());
        if (!mRequestUrl.IsHttps() && !mResponseData.isEmpty()) {
            // Inflate the resource data if it is gzipped.
            // Try to find "Content-Encoding" header.

            std::string encoding = ResourceManager::findResponseHeader("Content-Encoding", mResponseHeaders);
            if (!encoding.empty()) {
                lowerCase(encoding);
                if (contains(encoding, std::string("gzip"))) {
                    DataBuffer newDataBuffer;
                    bool ok = false;

                    DataBuffer uncompressed = mResponseData.uncompress(DataBuffer::Compression_GZip, &ok);
                    NRDP_OBJECTCOUNT_DESCRIBE(uncompressed, "RESOURCEMANAGER_UNCOMPRESS:" + mRequestUrl.str());

                    if (ok) {
                        NTRACE(TRACE_RESOURCEMANAGER, "ResourceManagerHttpServiceRequest::finish %s uncompress %d -> %d",
                               mRequestUrl.c_str(), mResponseData.size(), uncompressed.size());
                        mResponseData = uncompressed;
                    } else {
                        httpStatusCode = 400;
                        nativeErrorCode = AS_COMPRESSION_ERROR;
                    }
                }
            }
        }
    }

    mResourceManager->finish(r, nativeErrorCode, httpStatusCode, mState, networkMetrics,
                             nativeErrorCode == AS_NO_ERROR ? mResponseHeaders : HttpResponseHeaders(),
                             nativeErrorCode == AS_NO_ERROR ? mResponseData : DataBuffer());
    mResourceManager->removeNetworkRequest(shared_from_this());
}

int ResourceManagerHttpServiceRequest::sslVerifyCallback(int status, void *ctx, HttpServiceRequest *request, const NrdIpAddr *ipAddress)
{
    if (ResourceManagerHttpServiceRequest *hstRequest = static_cast<ResourceManagerHttpServiceRequest*>(request)) {
        Resource::SharedPtr resource = hstRequest->resource();
        if (resource) {
            shared_ptr<ResourceManager> resourceManager = resource->resourceManager();
            if (resourceManager)
                return resourceManager->sslVerify(status, ctx, resource, ipAddress ? *ipAddress : NrdIpAddr());
        }
    }
    return status;
}

// ========== ResourceManagerDiskCacheRequest ==========

ResourceManagerDiskCacheRequest::ResourceManagerDiskCacheRequest(const AseUrl &url,
                                                                 DiskCache::Response::Type type,
                                                                 const CacheData &cacheData,
                                                                 const Resource::SharedPtr &resource,
                                                                 const ResourceRequest::SharedPtr &request)
    : DiskCache::Request(url, cacheData.key, type), mResource(resource), mRequest(request),
      mCacheData(cacheData), mStarted(Time::mono())
{}

void ResourceManagerDiskCacheRequest::onComplete(const DiskCache::Response::SharedPtr &cached)
{
    shared_ptr<ResourceManagerHttpServiceRequest> hstRequest;
    ResourceManager::SharedPtr manager = mResource->resourceManager();
    Resource::State state = Resource::State_Network;
    if (!cached) {
        manager->mDiskCache->report(DiskCache::Miss, mRequest->url());
        if (mCacheData.mode & ResourceManager::CacheMode_OnlyIfCached) {
            manager->finish(mResource, AS_NO_ERROR, 404, Resource::State_DiskCache,
                            NetworkMetrics(0, 0, (Time::mono() - mStarted).ms()));
            return;
        }
    } else if (!cached->isExpired()) {
        manager->mDiskCache->report(DiskCache::Hit, mRequest->url());
        mResource->mExpiration = cached->expiration();
        mResource->mUrls[mResource->mUrls.size() - 1] = cached->url();
        manager->finish(mResource, AS_NO_ERROR, cached->httpStatusCode(),
                        mCacheData.refresh == -1 ? Resource::State_DiskCache : Resource::State_DiskCacheRefreshPending,
                        NetworkMetrics(0, 0, (Time::mono() - mStarted).ms()),
                        cached->responseHeaders(), cached->responseData());
        if (mCacheData.refresh == -1)
            return;
        mResource = mResource->resourceManager()->createResource(mRequest);
        state = Resource::State_DiskCacheRefresh;
    } else {
        manager->mDiskCache->report(DiskCache::Expired, mRequest->url());
        if (mCacheData.mode & ResourceManager::CacheMode_OnlyIfCached) {
            manager->finish(mResource, AS_NO_ERROR, 404, Resource::State_DiskCache,
                            NetworkMetrics(0, 0, (Time::mono() - mStarted).ms()));
            return;
        }
    }

    hstRequest.reset(new ResourceManagerHttpServiceRequest(mResource, mRequest, state));
    hstRequest->mResourceManager = manager;
    if (cached) {
        const std::string etag = cached->responseHeader("ETag");
        if (!etag.empty())
            hstRequest->mRequestHeaders["If-None-Match"] = etag;
        const std::string modified = cached->responseHeader("Last-Modified");
        if (!modified.empty())
            hstRequest->mRequestHeaders["If-Modified-Since"] = modified;
        if (!modified.empty() || !etag.empty()) {
            hstRequest->mCachedResponseData = cached->responseData();
            hstRequest->mCachedResponseHeaders = cached->responseHeaders();
            hstRequest->mCachedHttpStatusCode = cached->httpStatusCode();
        }
    }
    if (mCacheData.refresh > 0) {
        const Application::Timer::SharedPtr timer(new ResourceManagerRefreshTimer(mCacheData.refresh, hstRequest));
        app()->startTimer(timer);
    } else {
        manager->sendNetworkRequest(hstRequest);
    }
}

bool ResourceManager::remove(const DataBuffer &cacheKey)
{
    ScopedMutex lock(sMutex);
    const CacheMap::iterator it = mCacheMap.find(cacheKey);
    NTRACE(TRACE_RESOURCEMANAGER, "Removing resource with cachekey: %s (%d)",
           ResourceManager::prettyCacheKey(cacheKey).c_str(), it != mCacheMap.end());
    if (it != mCacheMap.end()) {
        remove(it);
        return true;
    }
    return false;
}

std::string ResourceManager::mimeType(const AseUrl &url)
{
    struct {
        const char *extension;
        const char *mimeType;
    } const mimeTypes[] = {
        // Images
        { ".bmp", "image/bmp" },
        { ".gif", "image/gif" },
        { ".ico", "image/x-icon" },
        { ".jpeg", "image/jpeg" },
        { ".jpg", "image/jpeg" },
        { ".png", "image/png" },
        { ".tif", "image/tiff" },
        { ".tiff", "image/tiff" },
        { ".xbm", "image/x-xbitmap" },

        // Ogg
        { ".ogg", "audio/ogg" },
        { ".oga", "audio/ogg" },
        { ".ogv", "video/ogg" },

        // MPEG
        { ".mpa", "audio/mpeg" },
        { ".mpg", "video/mpeg" },
        { ".mpeg", "video/mpeg" },
        { ".mpv", "video/mpeg" },

        // MPEG-4
        { ".m4v", "video/x-m4v" },
        { ".m4a", "audio/x-m4a" },
        { ".m4b", "audio/x-m4b" },
        { ".m4p", "audio/x-m4p" },

        // MP3
        { ".mp3", "audio/mp3" },

        // AAC
        { ".aac", "audio/aac" },
        { ".m4r", "audio/x-aac" },

        // WAV / AIFF
        { ".wav", "audio/x-wav" },
        { ".aiff", "audio/x-aiff" },

        // Downloadable fonts
        { ".psf", "application/x-font-linux-psf" },
        { ".otf", "application/x-font-otf" },
        { ".pcf", "application/x-font-pcf" },
        { ".snf", "application/x-font-snf" },
        { ".ttc", "application/x-font-ttf" },
        { ".ttf", "application/x-font-ttf" },
        { ".pfa", "application/x-font-type1" },
        { ".pfb", "application/x-font-type1" },
        { ".pfm", "application/x-font-type1" },
        { ".afm", "application/x-font-type1" },

        // Subtitles
        { ".dfxp", "text/xml" },

        // Text Resources
        { ".xml", "text/xml" },
        { ".html", "text/html" },
        { ".htm", "text/html" },
        { ".js", "text/javascript" },
        { ".css", "text/css" },
        { 0, 0 }
    };

    const std::string path = url.GetPath();

    for (int i=0; mimeTypes[i].mimeType; ++i) {
        if (endsWith(path, mimeTypes[i].extension)) {
            return mimeTypes[i].mimeType;
        }
    }
    return std::string();
}


void ResourceManager::instrument(const ResourceRequest::SharedPtr &request,
                                 const Resource::SharedPtr &resource,
                                 const AseUrl &url,
                                 Resource::State state)
{
    (void)request;
    (void)resource;
    (void)url;
    (void)state;
#ifdef NRDP_HAS_SWITCHED_INSTRUMENTATION
    Variant data;
    data["url"] = url.str();
    const HttpHeaders headers = request->requestHeaders();
    int requestSize = request->requestData().size();
    for (HttpHeaders::const_iterator it = headers.begin(); it != headers.end(); ++it)
        requestSize += it->first.size() + it->second.size();
    data["requestSize"] = requestSize;
    data["status"] = resource->httpStatusCode();
    data["errorCode"] = resource->errorCode();
    data["type"] = resource->responseHeader("Content-Type");
    int bytes = resource->responseData().size();
    const HttpResponseHeaders &responseHeaders = resource->responseHeaders();
    for (HttpResponseHeaders::const_iterator it = responseHeaders.begin(); it != responseHeaders.end(); ++it)
        bytes += it->header.size() + it->value.size();
    data["bytes"] = bytes;
    if (state != Resource::State_None)
        data["source"] = Resource::stateToString(state);

    const NetworkMetrics metrics = resource->networkMetrics();
    data["dnsTime"] = metrics.dnsTime();
    data["connectTime"] = metrics.connectTime();
    data["transactionTime"] = metrics.transactionTime();
    data["duration"] = metrics.duration();
    if (metrics.serverIpAddress().valid())
        data["serverIp"] = metrics.serverIpAddress().getStrAddr();

    INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("gibbon.request", request.get()), data);
#endif
}
}


