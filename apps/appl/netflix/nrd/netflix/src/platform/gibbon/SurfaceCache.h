/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SURFACECACHE_H
#define SURFACECACHE_H

#include <Surface.h>
#include <RectPacker.h>
#include <SurfaceAtlas.h>
#include <GibbonConsole.h>
#include <GibbonConfiguration.h>
#include <GibbonResourceManager.h>

#include <nrd/AppLog.h>

#include <nrdbase/tr1.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/Resource.h>
#include <nrdbase/DumpInfo.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/ThreadPool.h>

#include <nrdnet/Resource.h>

#include <list>

#define DEBUG_SURFACE_LOAD(fmt, ...)                                    \
    do {                                                                \
        if(Debug::DebugSurfaceLoad) {                                   \
            Log::warn(TRACE_GIBBON_SURFACE, "SURFACE_LOAD:" fmt, __VA_ARGS__); \
        } else {                                                        \
            NTRACE(TRACE_GIBBON_SURFACE, "SURFACE_LOAD:" fmt, __VA_ARGS__); \
        }                                                               \
    } while (0)

NRDP_DECLARE_OBJECTCOUNT(GibbonLoadJob);
NRDP_DECLARE_OBJECTCOUNT(GibbonDecodeJob);

namespace netflix {
namespace gibbon {
class SurfaceCacheDumper;
class DecodeJobIdleTimer;

class SurfaceLoader
{
public:
    typedef shared_ptr<SurfaceLoader> SharedPtr;
    typedef weak_ptr<SurfaceLoader> WeakPtr;

    enum LoadFlag {
        Load_LazyDecode      = 0x01,
        Load_LazyDownload    = 0x02,
        Load_AcceleratedOnly = 0x04,
        Load_HighPriority    = 0x08,
        Load_Synchronous     = 0x10,
        Load_Required        = 0x20,

        Load_LazyMask        = Load_LazyDecode | Load_LazyDownload
    };

    class Source
    {
    public:
        typedef shared_ptr<Source> SharedPtr;

        inline Source(const AseUrl &u, unsigned int f=0) : mGroup(-1), mScale(1.0f),
                                                           mTimeout(GibbonConfiguration::defaultNetworkTimeout()),
                                                           mConnectTimeout(GibbonConfiguration::defaultNetworkConnectTimeout()),
                                                           mStallTimeout(GibbonConfiguration::defaultNetworkStallTimeout()),
            mLoadFlags(f)
        {
            setUrl(u);
            std::string &accept = mHeaders["Accept"];
            if (accept.empty())
                accept = "image/*";
        }
        inline Source(unsigned int f=0) : mGroup(-1), mScale(1.0f), mTimeout(GibbonConfiguration::defaultNetworkTimeout()),
                                          mConnectTimeout(GibbonConfiguration::defaultNetworkConnectTimeout()),
                                          mStallTimeout(GibbonConfiguration::defaultNetworkStallTimeout()),
                                          mLoadFlags(f)
        {
            std::string &accept = mHeaders["Accept"];
            if (accept.empty())
                accept = "image/*";
        }

        inline bool compare(const Source::SharedPtr &rhs) const
        {
            if(!rhs)
                return false;
            if(rhs.get() == this)
                return true;
            return compare(*rhs.get());
        }
        inline bool compare(const Source &rhs) const
        {
            const Type type = this->type();
            if(type != rhs.type())
                return false;
            switch(type) {
            case Source::Type_Url:
                if(mUrl != rhs.mUrl)
                    return false;
                break;
            case Source::Type_Data:
                if(mData != rhs.mData)
                    return false;
                break;
            case Source::Type_None:
                break;
            }
            if(mScale != rhs.mScale)
                return false;
            if(mLoadFlags != rhs.mLoadFlags)
                return false;
            if(mHeaders != rhs.mHeaders)
                return false;
            if(mTimeout != rhs.mTimeout)
                return false;
            if(mConnectTimeout != rhs.mConnectTimeout)
                return false;
            if(mStallTimeout != rhs.mStallTimeout)
                return false;
            return true;
        }

        inline const AseUrl &url() const { return mUrl; }
        void setUrl(const AseUrl &url);

        inline const DataBuffer &data() const { return mData; }
        inline void setData(const DataBuffer &data) { mData = data; }

        inline const HttpHeaders &headers() const { return mHeaders; }
        inline void setHeaders(const HttpHeaders &headers) { mHeaders = headers; mCacheKey.clear(); }

        inline unsigned int loadFlags() const { return mLoadFlags; }
        inline void setLoadFlags(unsigned int flags) { mLoadFlags = flags; }

        inline void setCacheKey(const DataBuffer &cacheKey) const { mCacheKey = cacheKey; }
        inline const DataBuffer cacheKey() const { prepareCacheKey(); return mCacheKey; }

        inline int group() const { return mGroup; }
        inline void setGroup(int group) { mGroup = group; }

        inline float scale() const { return mScale; }
        inline void setScale(float scale) { mScale = scale; }

        inline Time timeout() const { return mTimeout; }
        inline void setTimeout(const Time &t) { mTimeout = t; }

        inline Time connectTimeout() const { return mConnectTimeout; }
        inline void setConnectTimeout(const Time &t) { mConnectTimeout = t; }

        inline Time stallTimeout() const { return mStallTimeout; }
        inline void setStallTimeout(const Time &t) { mStallTimeout = t; }

        enum Type {
            Type_None,
            Type_Url,
            Type_Data
        };
        inline Type type() const
        {
            if(!mData.isEmpty())
                return Type_Data;
            return mUrl.IsEmpty() ? Type_None : Type_Url;
        }

        std::string describe(bool full=false) const;
    private:
        void prepareCacheKey() const;

        int mGroup;
        float mScale;
        AseUrl mUrl;
        DataBuffer mData;
        HttpHeaders mHeaders;
        mutable DataBuffer mCacheKey;
        Time mTimeout, mConnectTimeout, mStallTimeout;
        unsigned int mLoadFlags;
    };

    inline SurfaceLoader(const Source::SharedPtr &source) : mSource(source), mMutex(ZERO_MUTEX, "SurfaceLoader::mMutex") { }
    virtual ~SurfaceLoader() {  }

    inline Source::SharedPtr getSource() const
    {
        ScopedMutex lock(mMutex);
        return mSource;
    }

    virtual bool isReady() const { return true; }
    virtual void onDownloaded(const shared_ptr<GibbonResource> &/*resource*/, Resource::State /*state*/) { }
    virtual void onSurfaceDecode(const shared_ptr<GibbonResource> &/*resource*/) { }
    virtual void onSurfaceDecoded(const Surface::SharedPtr &/*surface*/, bool /*loaded*/) { }

protected:
    friend class SurfaceCache;
    friend class LoadJobRequest;

    Source::SharedPtr mSource;
    GibbonResourceRequest::SharedPtr mRequest;
    mutable Mutex mMutex;
};

class SurfaceCache : public enable_shared_from_this<SurfaceCache>
{
public:
    typedef shared_ptr<SurfaceCache> SharedPtr;
    typedef weak_ptr<SurfaceCache> WeakPtr;

    SurfaceCache();

    void init(unsigned capacity);
    void shutdown();

    class DecodeInfo
    {
    public:
        inline DecodeInfo(const SurfaceLoader::Source::SharedPtr &_source, const shared_ptr<GibbonResource> &_resource) : queued(false), resource(_resource), source(_source)
        {
            required = 0;
            acceleratedOnly = 0;
            accessed = Time::mono();
        }

        inline bool isReady() const {
            for(std::list<SurfaceLoader::SharedPtr>::const_iterator it = loaders.begin(); it != loaders.end(); ++it) {
                if((*it)->isReady())
                    return true;
            }
            return false;
        }
        inline bool isQueued() const { return queued; }
        inline bool isOrphaned() const { return loaders.empty(); }

        std::string description() const {
            std::ostringstream stream;
            stream << "SurfaceDecoder(" << this;
            for(std::list<SurfaceLoader::SharedPtr>::const_iterator it = loaders.begin(); it != loaders.end(); ++it)
                stream << ":" << it->get();
            if(resource) {
                const std::string resource_desc = resource->decodeDescription();
                if(!resource_desc.empty())
                    stream << ":" << resource_desc;
            }
            stream << "): " << source->url().str();
            if(isOrphaned())
                stream << " (ORPHANED)";
            return stream.str();
        }

        bool queued;
        int required;
        int acceleratedOnly;
        Time accessed;
        shared_ptr<GibbonResource> resource;
        SurfaceLoader::Source::SharedPtr source;
        std::list<SurfaceLoader::SharedPtr> loaders;
    };

    class Listener
    {
    public:
        typedef shared_ptr<Listener> SharedPtr;
        typedef weak_ptr<Listener> WeakPtr;
        Listener() {}
        virtual ~Listener() {}

        virtual void onSurfaceDownloaded(const SurfaceLoader::Source::SharedPtr &/*source*/,
                                         const shared_ptr<GibbonResource> &/*resource*/, Resource::State /*state*/) { }
        virtual void onSurfaceDecode(const SurfaceLoader::Source::SharedPtr &/*source*/,
                                     const shared_ptr<GibbonResource> &/*resource*/) { }
        virtual void onSurfaceDecoded(const SurfaceCache::DecodeInfo &/*info*/, const Surface::SharedPtr &/*surface*/) { }

        enum Modification {
            Modified_Released,
            Modified_Reused,
            Modified_Changed
        };
        virtual void onSurfaceModified(const Surface::SharedPtr &/*surface*/, Modification /*modification*/) { }
    };
    void addListener(const Listener::SharedPtr &listener);
    void removeListener(const Listener::SharedPtr &listener);
    inline std::vector<Listener::SharedPtr> listeners() const {
        ScopedMutex _lock(sMutex);
        std::vector<Listener::SharedPtr> result;
        mListeners.lock(result);
        return result;
    }

    inline void modifiedSurface(const Surface::SharedPtr &surface) { notifySurfaceModified(surface, Listener::Modified_Changed); }
    inline void reusedSurface(const Surface::SharedPtr &surface) { notifySurfaceModified(surface, Listener::Modified_Reused); }

    std::list<Surface::SharedPtr> find(const DumpInfo &dump) const;
    void dump(const DumpInfo &dump = DumpInfo()) const;
    std::list<DecodeInfo> findPending(const DumpInfo &dump) const;
    void dumpPending(const DumpInfo &dump = DumpInfo()) const;
    void dumpTouch(const DumpInfo &dump = DumpInfo()) const;
    void dumpAtlas(const DumpInfo &dump = DumpInfo()) const;
    unsigned purge();
    Variant surfaceInfo() const;

    enum ClearMode {
        RemoteClient,
        Shutdown,
        Reload,
        Clear
    };
    void clear(ClearMode mode=Clear);

    void remove(const Surface::SharedPtr& surface);

    void setCapacity(unsigned s);
    inline unsigned capacity() const {
        ScopedMutex lock(sMutex);
        return mCacheSizeLimit;
    }
    inline unsigned size() const {
        ScopedMutex lock(sMutex);
        const int graphicsMemoryUsed = Surface::getGraphicsMemoryUsed();
        if(graphicsMemoryUsed != -1)
            return std::max((unsigned)graphicsMemoryUsed, mCacheSize);
        return mCacheSize;
    }

    void touch();
    void touch(const Surface::SharedPtr &surface);
    void touch(const DataBuffer &cacheKey);

    void setCacheKey(const Surface::SharedPtr &surface, const DataBuffer &cacheKey);

    Surface::SharedPtr requestSurface(const Surface::WeakPtr &surface);
    Surface::SharedPtr requestSurface(Coordinate width, Coordinate height, uint8_t flags, bool exactSize=true, int group = -1);
    Surface::SharedPtr find(const DataBuffer &cacheKey);
    Surface::SharedPtr quickFind(const DataBuffer &cacheKey);
    Surface::SharedPtr find(const Resource::SharedPtr &resource) { return find(resource->cacheKey()); }
    bool replace(Surface::SharedPtr& src, const Surface::SharedPtr& dst);
    void load(const SurfaceLoader::SharedPtr &loader);
    void abort(const SurfaceLoader::SharedPtr &loader);
    bool isDecoding();
    void startDecodeJobs();
    void abortDecodeJobs();

    void setDecodeSuspended(bool suspend);
    bool isDecodeSuspended() const;

    void startAnimation();
    void endAnimation();

    inline Surface::SharedPtr requestMaster(const Surface::SharedPtr &surface, int group, int width, int height,
                                            rectpacker::RectPacker::Node* &node)
    {
        return mAtlas.requestMaster(surface, group, width, height, node);
    }
    void updateSizes(int group, const Surface::SharedPtr& surface, Coordinate width, Coordinate height);
    void setAtlasSize(int group, Coordinate width, Coordinate height);
    void setAtlasPad(int width, int height) { mAtlas.setPadding(width, height); }
    float masterUtilization(const Surface::SharedPtr& surface) const { return mAtlas.masterUtilization(surface); }

    static Mutex &lock() { return sMutex; }
    static Mutex &decodeLock() { return sDecodeMutex; }

    static DataBuffer prepareCacheKey(const AseUrl &url, const HttpHeaders &headers, const DataBuffer &data);
private:
    void initThreadPool();

    inline void notifySurfaceModified(const Surface::SharedPtr &surface, Listener::Modification modification) {
        NTRACE(TRACE_GIBBON_SURFACE, "Modified: (%d): %s", modification, surface->describe().c_str());
        const std::vector<Listener::SharedPtr> l = listeners();
        for(std::vector<Listener::SharedPtr>::const_iterator it = l.begin(); it != l.end(); ++it)
            (*it)->onSurfaceModified(surface, modification);
    }
    void releasedSurface(const Surface::SharedPtr &surface);
    shared_ptr<SurfaceCacheDumper> dumper(const DumpInfo& dump = DumpInfo()) const;

    struct CacheEntry
    {
        CacheEntry(const Surface::SharedPtr &s = Surface::SharedPtr())
            : surface(s), accessed(Time::mono()), prev(0), next(0)
        {}

        Surface::SharedPtr surface;
        Time accessed;
        DataBuffer cacheKey;
        CacheEntry *prev, *next;
    };

    struct Touch {
        Touch();
        ~Touch() { mItemList.deleteAll(); }
        class Job : public ThreadPoolJob
        {
        public:
            inline Job() { }
            virtual std::string description() const { return "TouchJob"; }
            void execute();
        };
        struct Item {
            inline Item(const Surface::SharedPtr &_surface) : flags(0), surface(_surface), next(0), prev(0) { }
            inline Item(const DataBuffer &_cacheKey) : flags(0), cacheKey(_cacheKey), next(0), prev(0) { }
            enum { SurfaceCache_ItemFlag = 0x01, ResourceCache_ItemFlag = 0x02 };
            int flags;
            Surface::WeakPtr surface;
            DataBuffer cacheKey;
            Item *next, *prev;
        };
        Job *mJob;
        int mSurfaces;
        mutable Mutex mMutex;
        LinkedList<Item*> mItemList;
        std::map<DataBuffer, Item*> mByCacheKey;
    } mTouch;
    unsigned int mCacheSize, mCacheSizeLimit;
    static Mutex sMutex;
    Mutex mThreadPoolMutex;
    void processTouches();

    bool canPurge(CacheEntry* entry) const {
        if (!(entry->surface->getFlags() & Surface::Flags_Master))
            return entry->surface->mode() == Surface::Master && entry->surface.use_count() == 1;
        return mAtlas.canPurge(entry->surface);
    }

    class LoadJob : public ThreadPoolJob
    {
    public:
        typedef shared_ptr<LoadJob> SharedPtr;

        inline LoadJob(const SurfaceLoader::SharedPtr &loader) : mLoader(loader) { NRDP_OBJECTCOUNT_REF(GibbonLoadJob, this); }
        ~LoadJob() { NRDP_OBJECTCOUNT_DEREF(GibbonLoadJob, this); }

    protected:
        void execute();

        virtual std::string description() const {
            std::ostringstream stream;
            stream << "SurfaceLoader(" << this << ":" << mLoader.get() << "): " << mLoader->getSource()->url().str();
            return stream.str();
        }

        SurfaceLoader::SharedPtr mLoader;
    };
    void processLoadRequest(const SurfaceLoader::SharedPtr &loader);
    void processLoadResponse(const SurfaceLoader::SharedPtr &loader, const GibbonResourceRequest::SharedPtr &request);
    void processLoadResponse(const SurfaceLoader::SharedPtr &loader, const shared_ptr<GibbonResource> &resource, Resource::State state);

    class DecodeJob : public ThreadPoolJob, public enable_shared_from_this<DecodeJob>
    {
    public:
        typedef shared_ptr<DecodeJob> SharedPtr;
        typedef weak_ptr<DecodeJob> WeakPtr;

        inline DecodeJob(const SurfaceLoader::Source::SharedPtr &source, const shared_ptr<GibbonResource> &resource) : state(State_Idle), mutex(ZERO_MUTEX, "DecodeJob"),
                                                                                                                       decodeInfo(source, resource)
        {
            NRDP_OBJECTCOUNT_REF(GibbonDecodeJob, this);
        }
        ~DecodeJob() {
            NRDP_OBJECTCOUNT_DEREF(GibbonDecodeJob, this);
        }

        void abort();
        void decoded();
        void execute();

        virtual std::string description() const;

        enum {
            State_Idle,
            State_Decoding,
            State_Aborted,
            State_Decoded
        } state;
        Mutex mutex;
        ConditionVariable stateCondition;

        DecodeInfo decodeInfo;
        Surface::SharedPtr surface;
        shared_ptr<DecodeJob> next, prev;
    };
    bool processIdleTime();
    DecodeJob::SharedPtr nextDecodeJob();
    void startDecodeJob(const DecodeJob::SharedPtr &task);
    void finishDecodeJob(const DecodeJob::SharedPtr &task, bool loaded);
    void decode(const shared_ptr<GibbonResource> &resource, const SurfaceLoader::SharedPtr &loader);
    Surface::SharedPtr processSurfaceDecode(const SurfaceLoader::Source::SharedPtr &source,
                                            const shared_ptr<GibbonResource> &resource, uint8_t flags);
    void processSurfaceDecoded(const SurfaceLoader::SharedPtr &loader, const Surface::SharedPtr &surface, bool loaded);

    std::map<Surface*, CacheEntry*> mBySurface;
    LinkedList<CacheEntry*> mSurfaceList;
    std::map<DataBuffer, CacheEntry*> mByCacheKey;

    SurfaceAtlas mAtlas;

    int mDecodeSlots;
    bool mDecodeSuspended;
    static Mutex sDecodeMutex;
    shared_ptr<DecodeJobIdleTimer> mDecodeIdleTimer;
    std::map<DataBuffer, DecodeJob::SharedPtr> mDecodeJobs;
    LinkedList<DecodeJob::SharedPtr, DecodeJob> mDecodePendingList;

    bool addSurface(const Surface::SharedPtr &surface,
                    const DataBuffer &cacheKey = DataBuffer(),
                    bool account = true);
    static bool matches(const Surface::SharedPtr& surface, int width, int height, uint8_t flags, bool exactSize = true, int group = -1);
    CacheEntry *erase(CacheEntry *entry);
    CacheEntry *remove(CacheEntry *entry);
    void touch(CacheEntry *entry);
    void touch(const DecodeJob::SharedPtr &task);

    typedef WeakPtrSet<Listener> ListenerContainer;
    mutable ListenerContainer mListeners;

    shared_ptr<ThreadPool> mThreadPool;
    friend class DecodeJobIdleTimer;
    friend class DecodeJob;
    friend class Touch::Job;
    friend class LoadJobRequest;
    friend class LoadJob;
    friend class SurfaceAtlas;
    friend class SurfaceAtlasCompare;
    friend class SurfaceAtlasMaster;
};

}} // namespace netflix::gibbon

#endif // SURFACECACHE_H
