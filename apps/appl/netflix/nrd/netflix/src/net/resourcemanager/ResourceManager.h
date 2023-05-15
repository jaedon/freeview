/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <nrdbase/tr1.h>

#include <nrdbase/DumpInfo.h>
#include <nrdbase/Mutex.h>
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
#include <nrdnet/ResourceRequest.h>

#include <vector>
#include <map>
#include <set>

namespace netflix {

class HttpServiceThread;
class ResourceManagerHttpServiceRequest;
class ResourceManagerDumper;
class DiskCache;
class NrdIpAddr;
struct CacheData;

class ResourceManager : public enable_shared_from_this<ResourceManager>
{
public:
    typedef weak_ptr<ResourceManager> WeakPtr;
    typedef shared_ptr<ResourceManager> SharedPtr;
    ResourceManager();
    virtual ~ResourceManager();

    void init();
    void shutdown();

    static std::string findResponseHeader(const std::string &name, const HttpResponseHeaders &headers);
    static std::string findRequestHeader(const std::string &name, const HttpHeaders &headers);
    static AseUrl fixRelativeUrl(const AseUrl &base, const AseUrl &url_parser);

    void dump(const DumpInfo &dump=DumpInfo()) const;
    enum DumpFlag {
        Dump_Requests = 0x1,
        Dump_HST = 0x2
    };
    void dumpNetwork(const DumpInfo &dump);
    std::list<shared_ptr<Resource> > find(const DumpInfo &dump) const;
    Variant resourceInfo() const;

    void setCapacity(int capacity);
    inline int capacity() const { ScopedMutex lock(sMutex); return mCapacity; }
    void clear();

    int size() const { ScopedMutex lock(sMutex); return mSize; }
    int purge();

    static bool generateCacheKey(DataBuffer &cacheKey, const AseUrl &url, Resource::Method op=Resource::Method_Get,
                                 const HttpHeaders &requestHeaders=HttpHeaders(), const DataBuffer &postData=DataBuffer());
    inline static DataBuffer prettyCacheKey(const DataBuffer &cacheKey) {
        return cacheKey.isBinary() ? cacheKey.toBase64() : cacheKey;
    }

    shared_ptr<Resource> find(const DataBuffer &cacheKey);
    void touch(const DataBuffer &cacheKey);
    void touch(const std::vector<DataBuffer> &cacheKeys);

    void setTrustStoreData(const DataBuffer &trustStore);
    shared_ptr<TrustStore> trustStore() const { ScopedMutex lock(sMutex); return mTrustStore; }

    enum CacheMode {
        CacheMode_NoCache = 0x00,
        CacheMode_Fresh = 0x01,
        CacheMode_OnlyIfCached = 0x02,
        CacheMode_MemoryCache = 0x04,
        CacheMode_DiskCache = 0x08
    };

    void finish(const Resource::SharedPtr &resource, int32_t nativeErrorCode,
                int httpStatusCode, Resource::State state = Resource::State_Network,
                const NetworkMetrics &metrics = NetworkMetrics(),
                const HttpResponseHeaders &responseHeaders = HttpResponseHeaders(),
                const DataBuffer &responseData = DataBuffer());

    bool remove(const DataBuffer &cacheKey);

    void sendRequest(const shared_ptr<ResourceRequest> &req);
    inline void sendRequest(ResourceRequest *req) { sendRequest(shared_ptr<ResourceRequest>(req)); }
    virtual Resource::SharedPtr createResource(const ResourceRequest::SharedPtr &request)
    {
        shared_ptr<Resource> resource(new Resource);
        registerResource(request, resource);
        return resource;
    }
    shared_ptr<DiskCache> diskCache() const;
    void configureDiskCache(int capacity, int maxPending, int catalogTimer);

    static std::string mimeType(const AseUrl &url);

protected:
    inline void registerResource(const ResourceRequest::SharedPtr &request, const Resource::SharedPtr &resource) {
        assert(request && resource);
        if (request) {
            request->setResource(resource);
            resource->addRequest(request);
            resource->setCacheKey(request->cacheKey());
            resource->init(request->url(), request->method(), request->flags());
        }
        resource->setResourceManager(shared_from_this());
    }

    virtual void startRequest(const ResourceRequest::SharedPtr &request);
    virtual shared_ptr<DiskCache> createDiskCache() { return shared_ptr<DiskCache>(); }
    virtual int sslVerify(int status, void */*ctx*/, const Resource::SharedPtr &/*resource*/, const NrdIpAddr &/*ipAddr*/) { return status; }
    virtual void resourceFinished(const Resource::SharedPtr &resource, Resource::State state);
    virtual void requestRedirected(const shared_ptr<ResourceRequest> &resource, const shared_ptr<Resource> &redirect);
    virtual void requestFinished(const shared_ptr<ResourceRequest> &, const shared_ptr<Resource> &, Resource::State) {}
    virtual void processHeaders(const shared_ptr<Resource> &/* resource */, const HttpResponseHeaders &/* headers */) {}

    bool handleFileRequest(const std::string &fileName, const shared_ptr<ResourceRequest> &request);
private:
    int purge_internal();
    static void parseCacheData(CacheData &data,
                               const AseUrl &url,
                               Resource::Method method,
                               const HttpHeaders &requestHeaders,
                               const DataBuffer &requestData,
                               const HttpResponseHeaders &responseHeaders = HttpResponseHeaders());
    bool sendDiskCacheRequest(const DiskCache::SharedPtr &diskCache,
                              const ResourceRequest::SharedPtr &request,
                              const Resource::SharedPtr &resource,
                              const CacheData &cacheData);
    Resource::SharedPtr findResource(const AseUrl &url, const DataBuffer &cacheKey);
    bool handleDataRequest(const shared_ptr<ResourceRequest> &request);
    bool sendNetworkRequest(const shared_ptr<ResourceManagerHttpServiceRequest> &request);
    void removeNetworkRequest(const shared_ptr<ResourceManagerHttpServiceRequest> &request);
    void instrument(const ResourceRequest::SharedPtr &request, const Resource::SharedPtr &resource,
                    const AseUrl &url, Resource::State state);

    static void abortDiskCacheRequest(const shared_ptr<ResourceManagerDiskCacheRequest> &request);
    static void waitForDiskCacheRequest(const shared_ptr<ResourceManagerDiskCacheRequest> &request);
    enum Result {
        Result_Hit,
        Result_Miss,
        Result_Expired
    };
    void report(Result result, const AseUrl &url);
    shared_ptr<ResourceManagerDumper> dumper(const DumpInfo& dump = DumpInfo()) const;

    int mCapacity, mSize;

    shared_ptr<TrustStore> mTrustStore;
    shared_ptr<HttpServiceThread> mHttpServiceThread;

    static Mutex sMutex;
    static ConditionVariable sCondition;

    int mHits, mMisses, mExpired, mNoCache;

    enum State {
        Initial,
        Running,
        ShuttingDown,
        Finished
    } mState;

    struct CacheEntry {
        shared_ptr<Resource> resource;
        CacheEntry *prev, *next;
        DataBuffer cacheKey;
    };
    LinkedList<CacheEntry*> mCacheList;
    typedef std::map<DataBuffer, CacheEntry*> CacheMap;
    CacheMap mCacheMap;

    void touch(CacheEntry *entry);
    CacheEntry *remove(CacheMap::iterator it);

    shared_ptr<DiskCache> mDiskCache;
    std::set<shared_ptr<ResourceManagerHttpServiceRequest> > mNetworkRequests;

    friend class ResourceRequest;
    friend class Resource;
    friend class ResourceManagerHttpServiceRequest;
    friend class ResourceManagerDiskCacheRequest;
    friend class ResourceManagerRefreshTimer;
};

} // namespace netflix::net

#endif
