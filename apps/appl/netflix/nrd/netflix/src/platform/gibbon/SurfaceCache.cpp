/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SurfaceCache.h"

#include <Debug.h>
#include <Screen.h>
#include <GibbonEvent.h>
#include <SurfaceDecoder.h>
#include <GibbonApplication.h>
#include <GibbonConfiguration.h>

#include <nrd/AppThread.h>

#include <nrdbase/Dumper.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>

#include <openssl/md5.h>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonLoadJob);
NRDP_DEFINE_OBJECTCOUNT(GibbonDecodeJob);

namespace netflix {
namespace gibbon {

class LoadJobRequest : public GibbonResourceRequest
{
public:
    inline LoadJobRequest(const SurfaceLoader::SharedPtr &loader) :
        GibbonResourceRequest(getFlags(loader->getSource()), loader->getSource()->url(), Resource::Method_Get),
        mLoader(loader)
    {
        SurfaceLoader::Source::SharedPtr source = loader->getSource();
        if(ullong t = source->timeout().ms())
            setTimeout(t);
        if(ullong t = source->connectTimeout().ms())
            setConnectTimeout(t);
        if(ullong t = source->stallTimeout().ms())
            setStallTimeout(t);
        setRequestHeaders(source->headers());
        setCacheKey(source->cacheKey());
    }
    virtual void finish()
    {
        if(SurfaceLoader::SharedPtr loader = mLoader.lock()) {
            GibbonResourceRequest::SharedPtr that = static_pointer_cast<GibbonResourceRequest>(shared_from_this());
            GibbonApplication::instance()->getSurfaceCache()->processLoadResponse(loader, that);
        }
    }

private:
    inline static unsigned int getFlags(const SurfaceLoader::Source::SharedPtr &source)
    {
        unsigned int result = GibbonResourceManager::Surface|GibbonResourceManager::Trusted;
        if(source->loadFlags() & SurfaceLoader::Load_Synchronous)
            result |= ResourceRequest::Synchronous;
        return result;
    }

    SurfaceLoader::WeakPtr mLoader;
};

class DecodeJobIdleTimer : public GibbonApplication::Timer
{
public:
    DecodeJobIdleTimer() : GibbonApplication::Timer(GibbonConfiguration::surfaceLoadIdle()) { }

    virtual std::string describe() const { return "DecodeJobIdleTimer"; }
    virtual void timerFired() {
        PERFORMANCE_MARKER_SCOPED("decodejobidletimer.fired");
        if(SurfaceCache::SharedPtr cache = GibbonApplication::instance()->getSurfaceCache())
            cache->processIdleTime();
    }
};

}};

Mutex SurfaceCache::sMutex(GIBBON_SURFACECACHE_MUTEX, "GibbonSurfaceCache");
Mutex SurfaceCache::sDecodeMutex(GIBBON_SURFACECACHE_DECODE_MUTEX, "GibbonSurfaceCache::Decode");

SurfaceCache::SurfaceCache()
    : mCacheSize(0), mCacheSizeLimit(0), mThreadPoolMutex(GIBBON_SURFACECACHE_THREADPOOL_MUTEX, "SurfaceCache::ThreadPoolMutex")
{
    mDecodeSuspended = false;
    mDecodeSlots = GibbonConfiguration::surfaceLoadThreads();
}

void SurfaceCache::init(unsigned capacity)
{
    ScopedMutex lock(sMutex);
    mCacheSizeLimit = capacity;
    mAtlas.setSurfaceCache(shared_from_this());
    initThreadPool();
}

void SurfaceCache::shutdown()
{
    shared_ptr<ThreadPool> threadPool;
    {
        ScopedMutex lock(mThreadPoolMutex);
        std::swap(threadPool, mThreadPool);
    }
    if(threadPool)
        threadPool->shutdown();
    clear(Shutdown);
}

inline SurfaceCache::CacheEntry *SurfaceCache::erase(CacheEntry *entry)
{
    assert(MutexStack::locked(SurfaceCache::lock().name()));
    if(entry->surface->getFlags() & Surface::Flags_Master) {
        NTRACE(TRACE_GIBBON_SURFACE, "Erasing master %p", entry->surface.get());
        mAtlas.purge(entry->surface);
    }

    if(!entry->cacheKey.isEmpty()) {
        assert(mByCacheKey.find(entry->cacheKey) != mByCacheKey.end());
        mByCacheKey.erase(entry->cacheKey);
    }
    const int size = entry->surface->calculateSize();
    mCacheSize -= size;
    CacheEntry *next = entry->next;
    mSurfaceList.remove(entry);
    NTRACE(TRACE_GIBBON_SURFACE, "Removed %s", entry->surface->describe().c_str());
    delete entry;
    return next;
}

SurfaceCache::CacheEntry *SurfaceCache::remove(CacheEntry *entry)
{
    assert(MutexStack::locked(SurfaceCache::lock().name()));
    Surface::SharedPtr surface = entry->surface;
    assert(mBySurface.find(surface.get()) != mBySurface.end());
    mBySurface.erase(surface.get());
    SurfaceCache::CacheEntry *result = erase(entry);
    releasedSurface(surface);
    return result;
}

void SurfaceCache::remove(const Surface::SharedPtr &surface)
{
    ScopedMutex lock(sMutex);
    std::map<Surface*, CacheEntry*>::iterator entry = mBySurface.find(surface.get());
    assert(entry != mBySurface.end());
    CacheEntry *e = entry->second;
    mBySurface.erase(entry);
    erase(e);
    releasedSurface(surface);
}

static inline bool match(Coordinate wanted, Coordinate current, bool exact)
{
    if(exact)
        return wanted == current;
    const Coordinate diff = current - wanted;
    return diff >= 0 && diff <= (wanted * .10);
}

bool SurfaceCache::matches(const Surface::SharedPtr& surface, int width, int height, uint8_t flags, bool exactSize, int group)
{
    return (surface->isAccelerated() && surface->matches(flags) &&
            ::match(width, surface->getWidth(), exactSize) &&
            ::match(height, surface->getHeight(), exactSize) &&
            (group == surface->mGroup || (group == -1 && (surface->getFlags() & Surface::Flags_Master))));
}

Surface::SharedPtr SurfaceCache::requestSurface(const Surface::WeakPtr &surface)
{
    ScopedMutex lock(sMutex);
    if(surface.use_count() == 1) {
        if(Surface::SharedPtr result = surface.lock()) {
            result->modify(false);
            return result;
        }
    }
    return Surface::SharedPtr();
}

Surface::SharedPtr SurfaceCache::requestSurface(Coordinate width, Coordinate height, uint8_t flags, bool exactSize, int group)
{
    assert(width > 0 && height > 0);
    if(!GibbonApplication::instance()->getScreen()->isRunning())
        return Surface::SharedPtr(new Surface);

    assert(width > 0 && height > 0);
    int requestSurface;
    (void)requestSurface;
    const unsigned int surfaceSize = Surface::estimateSize(width, height, flags);

    processTouches();
    //dump();

    bool purgeWidgets = true, purgableSurfaces = false;
    const Time now(Time::mono());
    std::vector<Surface::WeakPtr> candidates;


    ScopedMutex lock(sMutex);

    if(group != -1 && GibbonConfiguration::surfaceAtlasEnabled()) {
        Surface::SharedPtr surface(new Surface(width, height, flags, group));
        if(addSurface(surface, DataBuffer(), false))
            return surface;
    }

    static int requestSurfaceCount = 0;
    requestSurface = ++requestSurfaceCount;
    NTRACES(TRACE_GIBBON_SURFACE, "Request(" << requestSurface << "): "
            << width << 'x' << height << ' '
            << exactSize
            << " (0x" << std::hex << flags << std::dec << ") [" << prettyBytes(surfaceSize) << ']');

    if(surfaceSize + SurfaceCache::size() <= mCacheSizeLimit) {
        Surface::SharedPtr surface(new Surface(width, height, flags));
        NTRACE(TRACE_GIBBON_SURFACE, "Created(%d): %s [%d]", requestSurface, surface->describe().c_str(), __LINE__);
        if(addSurface(surface))
            return surface;
    }

    std::vector<CacheEntry*> purgable;
    for(CacheEntry *e = mSurfaceList.first(); e; ) {
        const bool match = matches(e->surface, (int)width, (int)height, flags, exactSize, group);
        if(canPurge(e)) {
            if(!e->surface->isAccelerated()) {
                e = remove(e);
                continue;
            }
            if(match) {
                if(e->cacheKey.isEmpty()) {
                    touch(e);
                    e->surface->reuse(flags);
                    NTRACE(TRACE_GIBBON_SURFACE, "Reused(%d): %s [%d]", requestSurface, e->surface->describe().c_str(), __LINE__);
                    return e->surface;
                }
                purgeWidgets = false;
                candidates.push_back(e->surface);
            } else if((now-e->accessed) > Time::fromSeconds(10) || e->cacheKey.isEmpty()) {
                purgable.push_back(e);
            } else {
                purgableSurfaces = true;
            }
        } else if(match && e->surface->isAccelerated()) {
            candidates.push_back(e->surface);
        }
        e = e->next;
    }

    unsigned int purgableIndex = 0;
    while(surfaceSize + SurfaceCache::size() > mCacheSizeLimit && purgableIndex < purgable.size())
        remove(purgable.at(purgableIndex++));
    if(surfaceSize + SurfaceCache::size() <= mCacheSizeLimit) {
        Surface::SharedPtr surface(new Surface(width, height, flags));
        NTRACE(TRACE_GIBBON_SURFACE, "Created(%d): %s [%d]", requestSurface, surface->describe().c_str(), __LINE__);
        if(addSurface(surface))
            return surface;
    }

    if(purgeWidgets) {
        lock.unlock();
        if(unsigned int purged = Widget::purgeAll(Widget::Purge_Offscreen)) {
            purgableSurfaces = true;
            NTRACE(TRACE_GIBBON_SURFACE, "Purged(%d): %s [%d]", requestSurface, prettyBytes(purged).c_str(), __LINE__);
            NRDP_UNUSED(purged);
        }
        lock.relock();
    }

    for(std::vector<Surface::WeakPtr>::const_iterator it = candidates.begin(); it != candidates.end(); ++it) {
        const Surface::WeakPtr &weakSurface = (*it);
        if(weakSurface.use_count() != 1)
            continue;

        CacheEntry *entry = 0;
        if(Surface::SharedPtr surface = weakSurface.lock())
            entry = mapValue(mBySurface, surface.get());
        if(entry) {
            if(canPurge(entry)) {
                entry->surface->reuse(flags);
                if(entry->surface->getFlags() & Surface::Flags_Master) // Purge all subsurfaces if we're a master
                    mAtlas.purge(entry->surface);
                assert(matches(entry->surface, (int)width, (int)height, flags, exactSize, group));
                touch(entry);
                if(!entry->cacheKey.isEmpty()) {
                    assert(mByCacheKey.find(entry->cacheKey) != mByCacheKey.end() && mByCacheKey.find(entry->cacheKey)->second == entry);
                    mByCacheKey.erase(entry->cacheKey);
                    entry->cacheKey.clear();
                }
                NTRACE(TRACE_GIBBON_SURFACE, "Reused(%d): %s [%d]", requestSurface, entry->surface->describe().c_str(), __LINE__);
                return entry->surface;
            }
        }
    }

    if(purgableSurfaces) {
        for(CacheEntry *entry = mSurfaceList.first(); entry && SurfaceCache::size() + surfaceSize > mCacheSizeLimit; ) {
            if(canPurge(entry)) {
                if(matches(entry->surface, (int)width, (int)height, flags, exactSize, group)) {
                    entry->surface->reuse(flags);
                    touch(entry);
                    if(!entry->cacheKey.isEmpty()) {
                        assert(mByCacheKey.find(entry->cacheKey) != mByCacheKey.end() && mByCacheKey.find(entry->cacheKey)->second == entry);
                        mByCacheKey.erase(entry->cacheKey);
                        entry->cacheKey.clear();
                    }
                    NTRACE(TRACE_GIBBON_SURFACE, "Reused(%d): %s [%d]", requestSurface, entry->surface->describe().c_str(), __LINE__);
                    return entry->surface;
                } else {
                    entry = remove(entry);
                }
            } else {
                entry = entry->next;
            }
        }
    }

    Surface::SharedPtr surface(new Surface(width, height, flags));
    NTRACE(TRACE_GIBBON_SURFACE, "Created(%d)%s: %s [%d]", requestSurface,
           ((SurfaceCache::size() + surfaceSize) > mCacheSizeLimit) ? ":OVERBUDGET" : "", surface->describe().c_str(), __LINE__);
    if(addSurface(surface)) {
        if(mCacheSizeLimit > 0 && (SurfaceCache::size()) > mCacheSizeLimit) {
            Log::error(TRACE_GIBBON_SURFACE, "SurfaceCache: over budget! %s::%s (%d%%)",
                       prettyBytes(SurfaceCache::size()).c_str(), prettyBytes(mCacheSizeLimit).c_str(), int((SurfaceCache::size() / float(mCacheSizeLimit)) * 100));
        }
        return surface;
    }
    return Surface::SharedPtr();
}

void SurfaceCache::clear(ClearMode mode)
{
    if(mode != RemoteClient)
        abortDecodeJobs();

    LinkedList<CacheEntry*> surfaceList;
    {
        ScopedMutex lock(sMutex);
        mByCacheKey.clear();
        if(mode == Reload) {
            for(CacheEntry *e = mSurfaceList.first(); e; e = e->next)
                e->cacheKey.clear();
        } else {
            std::swap(surfaceList, mSurfaceList);
            NTRACE(TRACE_GIBBON_SURFACE, "clearing SurfaceCache");
            mBySurface.clear();
            mCacheSize = 0;
            if(mode == Shutdown) {
                shared_ptr<ThreadPool> threadPool;
                {
                    ScopedMutex l(mThreadPoolMutex);
                    std::swap(threadPool, mThreadPool);
                }
            }
            for(CacheEntry *e = surfaceList.first(); e; e = e->next) {
                if(!canPurge(e))
                    Log::info(TRACE_GIBBON_SURFACE, "SurfaceCache: clear with non-purgable: %s (%ld)",
                              e->surface->describe().c_str(), e->surface.use_count());
                releasedSurface(e->surface);
            }
        }
    }
    surfaceList.deleteAll();
}

#if !defined(GIBBON_NO_THREADANIMATION)
class AnimationGCWaitEvent : public GibbonApplication::Event
{
public:
    inline AnimationGCWaitEvent() : mFired(false), mMutex(ZERO_MUTEX, "AnimationGCWaitLock") { }

    void eventFired() { done(); }
    void eventCanceled() { done(); }
    void wait() {
        ScopedMutex _lock(mMutex);
        while(!mFired)
            mCondition.wait(&mMutex);
    }
private:
    void done()
    {
        ScopedMutex _lock(mMutex);
        mFired = true;
        mCondition.signal();
    }
    bool mFired;
    Mutex mMutex;
    ConditionVariable mCondition;
};
#endif

unsigned SurfaceCache::purge()
{
    PERFORMANCE_MARKER_SCOPED("surfacecache.purge");

    processTouches();

    unsigned result = 0;
    {
        ScopedMutex lock(sMutex);

        const int old = SurfaceCache::size();
        NRDP_UNUSED(old);
        NTRACE(TRACE_GIBBON_SURFACE, "PURGING %s", prettyBytes(old).c_str());

        int totalPurged = 0;
        for(CacheEntry *e = mSurfaceList.first(); e; ) {
            if(canPurge(e)) {
                NTRACE(TRACE_GIBBON_SURFACE, "    PURGED %s", e->surface->describe().c_str());
                e = remove(e);
                ++totalPurged;
            } else {
                NTRACE(TRACE_GIBBON_SURFACE, "    LEFT %s", e->surface->describe().c_str());
                e = e->next;
            }
        }
        if(totalPurged) {
            const unsigned bytesPurged = old - SurfaceCache::size();
            NTRACES(TRACE_GIBBON_SURFACE, "Surface:purged " << totalPurged << "/"
                    << mBySurface.size() + totalPurged << " surfaces "
                    << prettyBytes(bytesPurged) << "/" <<  prettyBytes(old));
            result = bytesPurged;
        }
    }
#if !defined(GIBBON_NO_THREADANIMATION)
    if(result && !Animation::isAnimationThread()) {
        shared_ptr<AnimationGCWaitEvent> event(new AnimationGCWaitEvent);
        if(Animation::postEvent(event))
            event->wait();
    }
#endif
    return result;
}

SurfaceCache::Touch::Touch() : mJob(0), mSurfaces(0), mMutex(GIBBON_SURFACECACHE_TOUCH_MUTEX, "GibbonSurfaceCache::Touch")
{
}

void SurfaceCache::Touch::Job::execute()
{
    if(SurfaceCache::SharedPtr surfaceCache = GibbonApplication::instance()->getSurfaceCache()) {
        {
            ScopedMutex lock(surfaceCache->mTouch.mMutex);
            assert(surfaceCache->mTouch.mJob == this);
            surfaceCache->mTouch.mJob = 0;
        }
        surfaceCache->processTouches();
    }
}

void SurfaceCache::dumpAtlas(const DumpInfo &) const
{
    Log::warn(TRACE_LOG, "Surface atlas: %s (default: %lldx%lld)", GibbonConfiguration::surfaceAtlasEnabled() ? "enabled" : "disabled",
              GibbonConfiguration::surfaceAtlasWidth(), GibbonConfiguration::surfaceAtlasHeight());
}

void SurfaceCache::dumpTouch(const DumpInfo &) const
{
    ScopedMutex lock(mTouch.mMutex);
    int count = 0;
    for(Touch::Item *i = mTouch.mItemList.first(); i; i = i->next) {
        if(!i->cacheKey.empty())
            Log::warn(TRACE_LOG, " [%d] (KEY:%s) [0x%x]", count++, i->cacheKey.c_str(), i->flags);
        else if(Surface::SharedPtr surface = i->surface.lock())
            Log::warn(TRACE_LOG, " [%d] (SURFACE:%s) [0x%x]", count++, surface->describe().c_str(), i->flags);
    }
    Log::warn(TRACE_LOG, "TOUCH(%p): %d -- %d(%zu)", mTouch.mJob, mTouch.mSurfaces, mTouch.mItemList.size(), mTouch.mByCacheKey.size());
}

void SurfaceCache::touch()
{
    PERFORMANCE_MARKER_SCOPED("surfacecache.touch");
    ScopedMutex lock(mTouch.mMutex);
    if(!mTouch.mJob && !mTouch.mItemList.isEmpty()) {
        ScopedMutex lock(mThreadPoolMutex);
        if(mThreadPool)
            mThreadPool->post(mTouch.mJob = new Touch::Job);
    }
}

void SurfaceCache::processTouches()
{
    PERFORMANCE_MARKER_SCOPED("surfacecache.processtouches");
#if defined(GIBBON_NO_THREADANIMATION)
    const bool touchResources = true;
#else
    const bool touchResources = !Animation::isAnimationThread();
#endif
    std::vector<DataBuffer> resources;
    {
        ScopedMutex lock(mTouch.mMutex);
        if(mTouch.mSurfaces || touchResources) {
            for(Touch::Item *i = mTouch.mItemList.first(); i; ) {
                Touch::Item *next = i->next;
                if(i->flags & Touch::Item::SurfaceCache_ItemFlag) {
                    if(!i->cacheKey.empty()) {
                        bool found = false;
                        {
                            ScopedMutex lock(sMutex);
                            const std::map<DataBuffer, CacheEntry*>::const_iterator it = mByCacheKey.find(i->cacheKey);
                            if(it != mByCacheKey.end()) {
                                touch(it->second);
                                found = true;
                            }
                        }
                        if(!found) {
                            ScopedMutex lock(sDecodeMutex);
                            const std::map<DataBuffer, DecodeJob::SharedPtr>::const_iterator it = mDecodeJobs.find(i->cacheKey);
                            if(it != mDecodeJobs.end())
                                touch(it->second);
                        }
                    } else {
                        ScopedMutex lock(sMutex);
                        if(Surface::SharedPtr surface = i->surface.lock()) {
                            if(surface->isValid()) {
                                std::map<Surface*, CacheEntry*>::const_iterator it = mBySurface.find(surface.get());
                                if(it != mBySurface.end())
                                    touch(it->second);
                            }
                        }
                    }
                    --mTouch.mSurfaces;
                    i->flags &= ~Touch::Item::SurfaceCache_ItemFlag;
                }
                if(touchResources && (i->flags & Touch::Item::ResourceCache_ItemFlag)) {
                    assert(!i->cacheKey.empty());
                    resources.push_back(i->cacheKey);
                    i->flags &= ~Touch::Item::ResourceCache_ItemFlag;
                }
                if(!(i->flags & (Touch::Item::ResourceCache_ItemFlag|Touch::Item::SurfaceCache_ItemFlag))) {
                    if(!i->cacheKey.empty())
                        mTouch.mByCacheKey.erase(i->cacheKey);
                    mTouch.mItemList.remove(i);
                    delete i;
                }
                i = next;
            }
        }
    }
    if(touchResources && !resources.empty())
        GibbonApplication::instance()->resourceManager()->touch(resources);
}

void SurfaceCache::touch(const DataBuffer &cacheKey)
{
    if(cacheKey.empty())
        return;

    PERFORMANCE_MARKER_SCOPED("surfacecache.touchdb");
    ScopedMutex lock(mTouch.mMutex);
    Touch::Item *item = 0;
    {
        const std::map<DataBuffer, Touch::Item*>::const_iterator it = mTouch.mByCacheKey.find(cacheKey);
        if(it != mTouch.mByCacheKey.end())
            item = it->second;
    }
    if(!item) {
        item = new Touch::Item(cacheKey);
        mTouch.mItemList.push_back(item);
        mTouch.mByCacheKey[cacheKey] = item;
    } else {
        mTouch.mItemList.moveToEnd(item);
    }
    if(!(item->flags & Touch::Item::SurfaceCache_ItemFlag)) {
        item->flags |= Touch::Item::SurfaceCache_ItemFlag;
        ++mTouch.mSurfaces;
    }
    item->flags |= Touch::Item::ResourceCache_ItemFlag;
}

void SurfaceCache::touch(const Surface::SharedPtr &surface)
{
    PERFORMANCE_MARKER_SCOPED("surfacecache.touchsurf");
    ScopedMutex lock(mTouch.mMutex);
    Touch::Item *item = new Touch::Item(surface);
    item->flags |= Touch::Item::SurfaceCache_ItemFlag;
    mTouch.mItemList.push_back(item);
    ++mTouch.mSurfaces;
}

bool SurfaceCache::addSurface(const Surface::SharedPtr &surface, const DataBuffer &cacheKey, bool account)
{
    assert(surface);
    assert(MutexStack::locked(SurfaceCache::lock().name()));
    if(!surface->isValid())
        return false;
    if(!surface->isAccelerated())
        Log::info(TRACE_GIBBON_SURFACE, "Created non-accelerated surface: %s", surface->describe().c_str());

    CacheEntry *entry = new CacheEntry(surface);
    mSurfaceList.append(entry);
    assert(mBySurface.find(surface.get()) == mBySurface.end());
    NTRACE(TRACE_GIBBON_SURFACE, "inserting %p", surface.get());
    mBySurface[surface.get()] = entry;
    if(!cacheKey.isEmpty() && !Debug::TestSurfaceCacheKeysDisabled) {
        entry->cacheKey = cacheKey;
        assert(mByCacheKey.find(cacheKey) == mByCacheKey.end());
        mByCacheKey[cacheKey] = entry;
    }

    if(account)
        mCacheSize += surface->calculateSize();
    return true;
}

bool SurfaceCache::replace(Surface::SharedPtr& src, const Surface::SharedPtr& dst)
{
    assert(src != dst);

    ScopedMutex lock(sMutex);

    std::map<Surface*, CacheEntry*>::iterator srcit = mBySurface.find(src.get());
    if(srcit == mBySurface.end())
        return false;
    std::map<Surface*, CacheEntry*>::iterator dstit = mBySurface.find(dst.get());
    if(dstit == mBySurface.end())
        return false;

    if(src->mMaster) {
        // remove it from the sub list
        mAtlas.removeFromMaster(src);
    }

    CacheEntry* dstentry = dstit->second;
    mSurfaceList.remove(dstentry);
    mBySurface.erase(dstit);
    if(!dstentry->cacheKey.isEmpty()) {
        mByCacheKey.erase(dstentry->cacheKey);
    }
    delete dstentry;

    CacheEntry* entry = srcit->second;
    mBySurface.erase(srcit);

    entry->surface = dst;
    mBySurface[dst.get()] = entry;

    touch(entry);

    src = dst;

    return true;
}

std::list<Surface::SharedPtr> SurfaceCache::find(const DumpInfo &dump) const
{
    std::list<Surface::SharedPtr> result;
    ScopedMutex lock(sMutex);
    int surface_count = 0;
    for(CacheEntry *e = mSurfaceList.first(); e; e = e->next) {
        if(dump.empty() || (int)surface_count == dump.idx || e->surface.get() == dump.ptr ||
           (dump.match.length() && contains(e->surface->describe(), dump.match)))
            result.push_back(e->surface);
        ++surface_count;
    }
    return result;
}

void SurfaceCache::dumpPending(const DumpInfo &) const
{
    ScopedMutex _widget_lock(Widget::lock());
    ScopedMutex _decode_lock(sDecodeMutex);
    int count = 0;
    for(std::map<DataBuffer, DecodeJob::SharedPtr>::const_reverse_iterator it = mDecodeJobs.rbegin(); it != mDecodeJobs.rend(); ++it) {
        DecodeJob::SharedPtr job = (*it).second;
        if(!mDecodePendingList.contains(job))
            Log::warn(TRACE_LOG, " [%d] (%lld) %s (QUEUED) %s", count++, (Time::mono()-job->decodeInfo.accessed).ms(),
                      job->description().c_str(), job->decodeInfo.isReady() ? "*" : "");
    }
    for(DecodeJob::SharedPtr job = mDecodePendingList.last(); job; job = job->prev)
        Log::warn(TRACE_LOG, " [%d] (%lld) %s (PENDING%s) %s", count++, (Time::mono()-job->decodeInfo.accessed).ms(),
                  job->description().c_str(), job->decodeInfo.isQueued() ? "*" : "", job->decodeInfo.isReady() ? "*" : "");
    Log::warn(TRACE_LOG, "Surface Cache Pending: %lld/%lld [%d] (%lld:%p)", GibbonConfiguration::surfaceLoadThreads(),
              GibbonConfiguration::animationSurfaceLoadThreads(), mDecodeSlots, InputEvent::getIdleTime().ms(), mDecodeIdleTimer.get());
}

std::list<SurfaceCache::DecodeInfo> SurfaceCache::findPending(const DumpInfo &) const
{
    ScopedMutex lock(sDecodeMutex);
    std::list<SurfaceCache::DecodeInfo> result;
    for(std::map<DataBuffer, DecodeJob::SharedPtr>::const_iterator it = mDecodeJobs.begin(); it != mDecodeJobs.end(); ++it)
        result.push_back((*it).second->decodeInfo);
    return result;
}

namespace netflix {
namespace gibbon {
class SurfaceCacheDumper : public Dumper
{
public:
    SurfaceCacheDumper() { }

    Surface::SharedPtr surface() const { return mSurface; }
    void setSurface(const Surface::SharedPtr& surface) { mSurface = surface; }

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value);
    virtual std::vector<std::string> consolePostProcess(const Variant& values);

private:
    Surface::SharedPtr mSurface;
};

std::vector<std::string> SurfaceCacheDumper::consoleEntry(const unsigned int /*idx*/, const std::string& /*section*/, const Variant& value)
{
    return one(StringFormatter::sformat(" [%lld:%s%s] %lldms [%s]",
                                        value["surfaceCount"].integer(),
                                        prettyBytes(value["size"].integer()).c_str(),
                                        value["canPurge"].boolean() ? "*" : "",
                                        value["accessed"].integer(),
                                        value["description"].string().c_str()));
}

std::vector<std::string> SurfaceCacheDumper::consolePostProcess(const Variant& values)
{
    return one(StringFormatter::sformat("Surface Cache(%s): (%lld/%s*)::(%lld/%s)==(%lld/%s)(%lld/%s) [%s] [%lld%%]",
                                        prettyBytes(values["size"].integer()).c_str(),
                                        values["purgable"].integer(),
                                        prettyBytes(values["purgableSize"].integer()).c_str(),
                                        values["unpurgable"].integer(),
                                        prettyBytes(values["unpurgableSize"].integer()).c_str(),
                                        values["surfaceListSize"].integer(),
                                        prettyBytes(values["cacheSize"].integer()).c_str(),
                                        values["matches"].integer(),
                                        prettyBytes(values["matchesSize"].integer()).c_str(),
                                        prettyBytes(values["cacheSizeLimit"].integer()).c_str(),
                                        values["cacheUsed"].integer()));
}
}} // namespace netflix::gibbon

shared_ptr<SurfaceCacheDumper> SurfaceCache::dumper(const DumpInfo& dump) const
{
    shared_ptr<SurfaceCacheDumper> dumper(new SurfaceCacheDumper);
    dumper->keys() << "surfaceCount" << "size" << "canPurge" << "accessed" << "description"
                   << "width" << "height" << "cacheKey" << "group" << "subsurface" << "ptr" << "alpha";

    ScopedMutex screenLock(Screen::lock());
    ScopedMutex lock(sMutex);
    size_t surface_count = 0;
    unsigned int purgable = 0, purgable_size = 0;
    unsigned int unpurgable = 0, unpurgable_size = 0;
    unsigned matches = 0, matches_size = 0;
    for(CacheEntry *e = mSurfaceList.first(); e; e = e->next) {
        const int size = e->surface ? e->surface->calculateSize() : 0;
        if(canPurge(e)) {
            ++purgable;
            purgable_size += size;
        } else {
            ++unpurgable;
            unpurgable_size += size;
        }
        if(!dump.match.length() || (e->surface && contains(e->surface->describe(), dump.match))) {
            bool doDump = true;
            if((int)surface_count == dump.idx || e->surface.get() == dump.ptr)
                dumper->setSurface(e->surface);
            else if(dump.ptr || dump.idx != -1)
                doDump = false;
            if(doDump) {
                std::string description;
                void* ptr = 0;
                int group = -1;
                bool subsurface = false;
                bool alpha = false;
                Coordinate width = 0;
                Coordinate height = 0;
                DataBuffer cacheKey;
                if(e->surface) {
                    ptr = e->surface.get();
                    group = e->surface->group();
                    description = e->surface->describe();
                    alpha = e->surface->hasAlpha();
                    if(e->surface->mode() == Surface::Subsurface) {
                        description += " subsurface";
                        subsurface = true;
                    }
                    width = e->surface->getWidth();
                    height = e->surface->getHeight();
                    cacheKey = e->surface->cacheKey();
                } else {
                    description = StringFormatter::sformat("pending:%s",
                                                           e->cacheKey.empty() ? "*unknown*" : ResourceManager::prettyCacheKey(e->cacheKey).c_str());
                }
                dumper->values() << surface_count << size << canPurge(e) << (Time::mono()-e->accessed).ms()
                                 << description << width << height << cacheKey << group << subsurface << ptr << alpha;
                ++matches;
                matches_size += e->surface ? e->surface->calculateSize() : 0;
            }
        }
        ++surface_count;
    }
    Dumper& d = *dumper;
    d["size"] = SurfaceCache::size();
    d["purgable"] = purgable;
    d["purgableSize"] = purgable_size;
    d["unpurgable"] = unpurgable;
    d["unpurgableSize"] = unpurgable_size;
    d["surfaceListSize"] = mSurfaceList.size();
    d["cacheSize"] = mCacheSize;
    d["matches"] = matches;
    d["matchesSize"] = matches_size;
    d["cacheSizeLimit"] = mCacheSizeLimit;
    if (mCacheSizeLimit)
        d["cacheUsed"] = int((SurfaceCache::size() / float(mCacheSizeLimit)) * 100);
    else
        d["cacheUsed"] = 100;

    return dumper;
}

void SurfaceCache::dump(const DumpInfo& dump) const
{
    shared_ptr<SurfaceCacheDumper> d = dumper(dump);
    const std::vector<std::string>& data = d->toConsole();
    for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
        Log::warn(TRACE_GIBBON_SURFACE, "%s", it->c_str());
    }
    const Surface::SharedPtr& surface = d->surface();
    if(surface) {
        static int serial = 0;
        char filename[2048];
        sprintf(filename, "/tmp/surface_%d", serial++);
        if(surface && SurfaceDecoders::instance()->encode(surface, filename)) {
            Log::warn(TRACE_GIBBON_SURFACE, " [%s] [%s] DUMPED %s", prettyBytes(surface->calculateSize()).c_str(),
                      surface->describe().c_str(), filename);
        }
    }
}

Variant SurfaceCache::surfaceInfo() const
{
    shared_ptr<SurfaceCacheDumper> d = dumper();
    return d->variant();
}

void SurfaceCache::setCapacity(unsigned s)
{
    {
        ScopedMutex lock(sMutex);
        mCacheSizeLimit = s;
    }
    purge();
}

void SurfaceCache::abort(const SurfaceLoader::SharedPtr &loader)
{
    shared_ptr<GibbonResourceRequest> abort;
    {
        ScopedMutex lock(loader->mMutex);
        if(!(loader->getSource()->loadFlags() & SurfaceLoader::Load_Required))
            abort = loader->mRequest;
        loader->mRequest.reset();
    }
    if(abort) {
        abort->abort();
        abort.reset();
    }
    {
        ScopedMutex lock(sDecodeMutex);
        const std::map<DataBuffer, DecodeJob::SharedPtr>::const_iterator it = mDecodeJobs.find(loader->getSource()->cacheKey());
        if(it != mDecodeJobs.end()) {
            const DecodeJob::SharedPtr job = it->second;
            if(loader->getSource()->loadFlags() & SurfaceLoader::Load_AcceleratedOnly)
                --job->decodeInfo.acceleratedOnly;
            if(loader->getSource()->loadFlags() & SurfaceLoader::Load_Required)
                --job->decodeInfo.required;
            if(job->decodeInfo.queued) {
                job->decodeInfo.queued = false;
                ++mDecodeSlots;
            }
            for(std::list<SurfaceLoader::SharedPtr>::iterator it = job->decodeInfo.loaders.begin(); it != job->decodeInfo.loaders.end(); ++it) {
                if((*it) == loader) {
                    job->decodeInfo.loaders.erase(it);
                    break;
                }
            }
        }
    }
}

bool SurfaceCache::isDecoding()
{
    ScopedMutex lock(sDecodeMutex);
    return !mDecodeJobs.empty();
}

void SurfaceLoader::Source::setUrl(const AseUrl &url)
{
    if(url.IsEmpty())
        mUrl = url;
    else
        mUrl = ResourceManager::fixRelativeUrl(GibbonApplication::instance()->getLocation(), url);
    mCacheKey.clear();
}

void SurfaceLoader::Source::prepareCacheKey() const
{
    if(!mCacheKey.empty())
        return;
    mCacheKey = SurfaceCache::prepareCacheKey(mUrl, mHeaders, mData);
    NRDP_OBJECTCOUNT_DESCRIBE(mCacheKey, "SURFACECACHE_CACHEKEY:"+mUrl.str());
}

std::string SurfaceLoader::Source::describe(bool full) const
{
    std::ostringstream stream;
    if(!mUrl.IsEmpty())
        stream << mUrl;
    else if(!mData.isEmpty())
        stream << "data:;base64," << mData.toBase64String();
    stream << " flags: " << std::hex << mLoadFlags;
    stream << " scale: " << mScale;
    if(full) {
        if(!mHeaders.empty())
            stream << " headers: " << httpHeadersToVariant(mHeaders).toJSON();
        if(mTimeout.ms())
            stream << " timeout: " << mTimeout.ms();
        if(mConnectTimeout.ms())
            stream << " connectTimeout: " << mConnectTimeout.ms();
        if(mStallTimeout.ms())
            stream << " stallTimeout: " << mStallTimeout.ms();
    }
    return stream.str();
}

void SurfaceCache::processLoadResponse(const SurfaceLoader::SharedPtr &loader, const shared_ptr<GibbonResource> &resource, Resource::State state)
{
    SurfaceLoader::Source::SharedPtr source = loader->getSource();
    if(!source)
        return;

    PERFORMANCE_MARKER_SCOPED("surfacecache.loaded.response");
    {
        const std::vector<Listener::SharedPtr> l = listeners();
        for(std::vector<Listener::SharedPtr>::const_iterator it = l.begin(); it != l.end(); ++it)
            (*it)->onSurfaceDownloaded(source, resource, state);
    }
    loader->onDownloaded(resource, state);
    INSTRUMENTATION_INTERVAL_SWITCHED_MARK(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()), "download");
    if(Surface::SharedPtr surface = find(source->cacheKey())) {
        DEBUG_SURFACE_LOAD("SurfaceCache::processLoadResponse(%p) CACHED to %s", loader.get(), source->describe().c_str());
        processSurfaceDecoded(loader, surface, true);
    } else if(!GibbonConfiguration::surfaceLoadThreads() || (source->loadFlags() & SurfaceLoader::Load_Synchronous)) {
        uint8_t flags = 0;
        if(source->loadFlags() & SurfaceLoader::Load_AcceleratedOnly)
            flags |= Surface::Flags_Accelerated;
        Surface::SharedPtr surface = processSurfaceDecode(source, resource, flags);
        if(surface)
            setCacheKey(surface, source->cacheKey());
        DEBUG_SURFACE_LOAD("SurfaceCache::processLoadResponse(%p) DECODE to %s", loader.get(), source->describe().c_str());
        processSurfaceDecoded(loader, surface, true);
    } else if((source->loadFlags() & SurfaceLoader::Load_LazyDecode) && !loader->isReady()) {
        DEBUG_SURFACE_LOAD("SurfaceCache::processLoadResponse(%p) DECODELAZY to %s", loader.get(), source->describe().c_str());
        processSurfaceDecoded(loader, Surface::SharedPtr(), false);
    } else {
        DEBUG_SURFACE_LOAD("SurfaceCache::processLoadResponse(%p) DECODEJOB to %s", loader.get(), source->describe().c_str());
        decode(resource, loader);
    }
}

void SurfaceCache::processLoadRequest(const SurfaceLoader::SharedPtr &loader)
{
    SurfaceLoader::Source::SharedPtr source = loader->getSource();
    if(source->type() == SurfaceLoader::Source::Type_Data) {
        INSTRUMENTATION_INTERVAL_SWITCHED_MARK(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()), "dataload_start");
        PERFORMANCE_MARKER_SCOPED("surfacecache.sendrequest.loaddata");
        DEBUG_SURFACE_LOAD("SurfaceCache::processLoadRequest(%p) DATA %s", loader.get(), source->describe().c_str());
        shared_ptr<GibbonResource> resource(new GibbonResource);
        resource->setCacheKey(source->cacheKey());
        resource->setResponseData(source->data());
        processLoadResponse(loader, resource, Resource::State_Data);
        INSTRUMENTATION_INTERVAL_SWITCHED_MARK(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()), "dataload_end");
    } else {
        INSTRUMENTATION_INTERVAL_SWITCHED_MARK(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()), "download_start");
        PERFORMANCE_MARKER_SCOPED("surfacecache.sendrequest.download");
        DEBUG_SURFACE_LOAD("SurfaceCache::processLoadRequest(%p) DOWNLOAD %s", loader.get(), source->describe().c_str());
        GibbonResourceRequest::SharedPtr request(new LoadJobRequest(loader));
        {
            ScopedMutex lock(loader->mMutex);
            loader->mRequest = request;
        }
        nrdApp()->resourceManager()->sendRequest(request);
        if(source->loadFlags() & SurfaceLoader::Load_Synchronous)
            request->complete();
    }
}

void SurfaceCache::processLoadResponse(const SurfaceLoader::SharedPtr &loader, const GibbonResourceRequest::SharedPtr &request)
{
    {
        ScopedMutex lock(loader->mMutex);
        if(loader->mRequest != request)
            return;
        loader->mRequest.reset();
    }
    processLoadResponse(loader, static_pointer_cast<GibbonResource>(request->resource()), request->state());
}

void SurfaceCache::load(const SurfaceLoader::SharedPtr &loader)
{
    PERFORMANCE_MARKER_SCOPED("surfacecache.load");
    abort(loader);
    INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()), Variant::StringMap("url", loader->getSource()->url().str()));
    if(loader->getSource()->loadFlags() & SurfaceLoader::Load_Synchronous) {
        processLoadRequest(loader);
    } else {
        ScopedMutex lock(mThreadPoolMutex);
        if(mThreadPool)
            mThreadPool->post(LoadJob::SharedPtr(new LoadJob(loader)));
    }
}

void SurfaceCache::initThreadPool()
{
    ScopedMutex lock(mThreadPoolMutex);
    if(!mThreadPool) {
        mThreadPool.reset(new ThreadPool);
        mThreadPool->init(GibbonConfiguration::surfaceLoadThreads(), &THREAD_GIBBON_SURFACE_LOADER, 1, 60000, 50000);
    }
}

Surface::SharedPtr SurfaceCache::quickFind(const DataBuffer &cacheKey)
{
    if(Debug::TestSurfaceCacheKeysDisabled)
        return Surface::SharedPtr();

    Surface::SharedPtr result;
    if(sMutex.tryLock() == NFErr_OK) {
        const std::map<DataBuffer, CacheEntry*>::const_iterator it = mByCacheKey.find(cacheKey);
        if(it != mByCacheKey.end()) {
            touch(it->second);
            result = it->second->surface;
        }
        sMutex.unlock();
    }
    return result;
}

Surface::SharedPtr SurfaceCache::find(const DataBuffer &cacheKey)
{
    if(Debug::TestSurfaceCacheKeysDisabled)
        return Surface::SharedPtr();
    assert(!cacheKey.isEmpty());
    ScopedMutex lock(sMutex);
    const std::map<DataBuffer, CacheEntry*>::const_iterator it = mByCacheKey.find(cacheKey);
    if(it == mByCacheKey.end())
        return Surface::SharedPtr();
    touch(it->second);
    return it->second->surface;
}

void SurfaceCache::touch(const DecodeJob::SharedPtr &job)
{
    assert(job);
    assert(MutexStack::locked(sDecodeMutex.name()));
    if(mDecodePendingList.contains(job))
        mDecodePendingList.moveToEnd(job);
    job->decodeInfo.accessed = Time::mono();
    // NTRACE(TRACE_GIBBON_SURFACE, "Touched: %s %s", job->describe());
}

void SurfaceCache::touch(CacheEntry *entry)
{
    assert(entry);
    assert(MutexStack::locked(SurfaceCache::lock().name()));
    mSurfaceList.moveToEnd(entry);
    entry->accessed = Time::mono();
    if(Surface* master = entry->surface->master().get()) {
        CacheEntry* masterEntry = mBySurface[master];
        assert(masterEntry != 0);
        masterEntry->accessed = entry->accessed;
        mSurfaceList.moveToEnd(masterEntry);
    }
    // NTRACE(TRACE_GIBBON_SURFACE, "Touched: %s %s",
    //        entry->surface ? entry->surface->describe().c_str() : "No surface",
    //        entry->cacheKey.c_str());
}

void SurfaceCache::setCacheKey(const Surface::SharedPtr &surface, const DataBuffer &cacheKey)
{
    ScopedMutex lock(sMutex);
    surface->setCacheKey(cacheKey);
    CacheEntry *entry = mapValue(mBySurface, surface.get());
    if(entry && entry->cacheKey != cacheKey) {
        if(!entry->cacheKey.isEmpty())
            mByCacheKey.erase(entry->cacheKey);
        if(!Debug::TestSurfaceCacheKeysDisabled) {
            entry->cacheKey = cacheKey;
            if(!cacheKey.isEmpty()) {
                assert(mByCacheKey.find(cacheKey) == mByCacheKey.end());
                mByCacheKey[cacheKey] = entry;
            }
        }
    } else if(!entry) {
        Log::error(TRACE_LOG, "Didn't find: %s", surface->describe().c_str());
    }
}

void SurfaceCache::startAnimation()
{
    if(Debug::TestAnimationNoDecode)
        setDecodeSuspended(true);
    if(const int loadSlots = GibbonConfiguration::surfaceLoadThreads()) {
        const int animationLoadSlots = GibbonConfiguration::animationSurfaceLoadThreads();
        if(animationLoadSlots != loadSlots && animationLoadSlots != -1) {
            {
                ScopedMutex _lock(sDecodeMutex);
                mDecodeSlots -= (loadSlots - animationLoadSlots);
            }
            startDecodeJobs();
        }
    }
}

void SurfaceCache::endAnimation()
{
    if(Debug::TestAnimationNoDecode)
        setDecodeSuspended(false);
    if(const int loadSlots = GibbonConfiguration::surfaceLoadThreads()) {
        const int animationLoadSlots = GibbonConfiguration::animationSurfaceLoadThreads();
        if(animationLoadSlots != loadSlots && animationLoadSlots != -1) {
            {
                ScopedMutex _lock(sDecodeMutex);
                mDecodeSlots += (loadSlots - animationLoadSlots);
            }
            startDecodeJobs();
        }
    }
}

void SurfaceCache::releasedSurface(const Surface::SharedPtr &surface)
{
    assert(MutexStack::locked(sMutex.name()));
    notifySurfaceModified(surface, Listener::Modified_Released);
    if(rectpacker::RectPacker::Node *node = surface->node()) {
        assert(surface->master());
        mAtlas.notifyDestroyed(node);
    }
    surface->invalidate();
}

void SurfaceCache::addListener(const SurfaceCache::Listener::SharedPtr &listener)
{
    ScopedMutex _lock(sMutex);
    mListeners.insert(listener);
}

void SurfaceCache::removeListener(const SurfaceCache::Listener::SharedPtr &listener)
{
    ScopedMutex _lock(sMutex);
    mListeners.erase(listener);
}

void SurfaceCache::updateSizes(int group, const Surface::SharedPtr& surface, Coordinate width, Coordinate height)
{
    ScopedMutex _lock(sMutex);

    const int oldSurfaceSize = surface->calculateSize();
    surface->setRect(Rect(0, 0, width, height));
    const int newSurfaceSize = surface->calculateSize();

    assert(newSurfaceSize < oldSurfaceSize);

    mCacheSize += (newSurfaceSize - oldSurfaceSize);

    // update the rect packer for this
    mAtlas.updateSize(group, surface, static_cast<int>(width), static_cast<int>(height));
}

void SurfaceCache::setAtlasSize(int group, Coordinate width, Coordinate height)
{
    mAtlas.setAtlasSize(group, width, height);
}

void SurfaceCache::decode(const shared_ptr<GibbonResource> &resource, const SurfaceLoader::SharedPtr &loader)
{
    assert(loader);

    const DataBuffer cacheKey = resource->cacheKey();
    assert(!cacheKey.isEmpty());

    Surface::SharedPtr surface;
    const SurfaceLoader::Source::SharedPtr source = loader->getSource();
    {
        ScopedMutex lock(sDecodeMutex);
        if(!Debug::TestSurfaceCacheKeysDisabled) {
            ScopedMutex lock(sMutex);
            const std::map<DataBuffer, CacheEntry*>::const_iterator it = mByCacheKey.find(cacheKey);
            if(it != mByCacheKey.end() && (it->second->surface->isAccelerated() || !(source->loadFlags() & SurfaceLoader::Load_AcceleratedOnly))) {
                surface = it->second->surface;
                touch(it->second);
            }
        }
        if(!surface) {
            DecodeJob::SharedPtr &job = mDecodeJobs[cacheKey];
            if(!job) {
                job.reset(new DecodeJob(loader->getSource(), resource));
                if(source->loadFlags() & SurfaceLoader::Load_AcceleratedOnly)
                    ++job->decodeInfo.acceleratedOnly;
                if(source->loadFlags() & SurfaceLoader::Load_Required)
                    ++job->decodeInfo.required;
                job->decodeInfo.loaders.push_back(loader);
                if((source->loadFlags() & SurfaceLoader::Load_HighPriority) ||
                   (mDecodeSlots > 0 /*&& job->decodeInfo.isReady()*/))
                    startDecodeJob(job);
                else
                    mDecodePendingList.append(job);
                return;
            } else if(!job->surface) {
                if(source->loadFlags() & SurfaceLoader::Load_AcceleratedOnly)
                    ++job->decodeInfo.acceleratedOnly;
                if(source->loadFlags() & SurfaceLoader::Load_Required)
                    ++job->decodeInfo.required;
                job->decodeInfo.loaders.push_back(loader);
                if((source->loadFlags() & SurfaceLoader::Load_HighPriority) &&
                   mDecodePendingList.contains(job)) {
                    mDecodePendingList.remove(job);
                    startDecodeJob(job);
                }
                return;
            }
            surface = job->surface;
        }
    }
    NTRACE(TRACE_GIBBON_SURFACE, "Found in cache %s", resource->url().str().c_str());
    processSurfaceDecoded(loader, surface, true);
}

static Time decodeIdleTime()
{
    if(nrdApp()->isPlayerOpen())
        return Time(0);
   return InputEvent::getIdleTime();
}

bool SurfaceCache::processIdleTime()
{
    if(GibbonConfiguration::surfaceLoadIdle()) {
        {
            ScopedMutex _lock(sDecodeMutex);
            if(!mDecodeIdleTimer) //already handled
                return false;

            if(mDecodePendingList.isEmpty()) {
                mDecodeIdleTimer->stop();
                mDecodeIdleTimer.reset();
                return false;
            }

            const Time idleTime = decodeIdleTime();
            if(idleTime < GibbonConfiguration::surfaceLoadIdle()) {
                NTRACE(TRACE_GIBBON_SURFACE, "PushDecodeJobIdleTimer(%p) %lld (%lld)", mDecodeIdleTimer.get(),
                       idleTime.ms(), (GibbonConfiguration::surfaceLoadIdle() - idleTime).ms());
                mDecodeIdleTimer->restart(GibbonConfiguration::surfaceLoadIdle() - idleTime.seconds());
                return false;
            }

            NTRACE(TRACE_GIBBON_SURFACE, "ProcessDecodeJobIdleTimer(%p) %lld", mDecodeIdleTimer.get(), idleTime.ms());
        }
        startDecodeJobs();
    }
    return true;
}

SurfaceCache::DecodeJob::SharedPtr SurfaceCache::nextDecodeJob()
{
    assert(MutexStack::locked(sDecodeMutex.name()));
    if(mDecodeSlots <= 0 || mDecodePendingList.isEmpty())
        return SurfaceCache::DecodeJob::SharedPtr();

    for(DecodeJob::SharedPtr job = mDecodePendingList.last(); job; job = job->prev) {
        if(job->decodeInfo.isReady()) {
            mDecodePendingList.remove(job);
            return job;
        }
    }

    if(GibbonConfiguration::surfaceLoadIdle()) {
        const Time idleTime = decodeIdleTime();
        if(InputEvent::getIdleTime() < GibbonConfiguration::surfaceLoadIdle()) {
            if(!mDecodeIdleTimer) {
                mDecodeIdleTimer.reset(new DecodeJobIdleTimer());
                NTRACE(TRACE_GIBBON_SURFACE, "StartDecodeJobIdleTimer(%p) %lld (%lld)", mDecodeIdleTimer.get(),
                       idleTime.ms(), (GibbonConfiguration::surfaceLoadIdle() - idleTime).ms());
                mDecodeIdleTimer->restart((GibbonConfiguration::surfaceLoadIdle() - idleTime).ms());
            }
            return SurfaceCache::DecodeJob::SharedPtr();
        }
        if(mDecodeIdleTimer) {
            mDecodeIdleTimer->stop();
            mDecodeIdleTimer.reset();
        }
    }

    return mDecodePendingList.pop_back(); //oh well just take the last
}

void SurfaceCache::abortDecodeJobs()
{
    std::map<DataBuffer, DecodeJob::SharedPtr> jobs;
    {
        ScopedMutex lock(sDecodeMutex);
        std::swap(jobs, mDecodeJobs);
        mDecodePendingList.clear();
    }
    for(std::map<DataBuffer, DecodeJob::SharedPtr>::const_iterator it = jobs.begin(); it != jobs.end(); ++it)
        it->second->abort();
}

void SurfaceCache::setDecodeSuspended(bool suspend)
{
    ScopedMutex _decode_lock(sDecodeMutex);
    mDecodeSuspended = suspend;
    for(std::map<DataBuffer, DecodeJob::SharedPtr>::const_iterator it = mDecodeJobs.begin(); it != mDecodeJobs.end(); ++it) {
        DecodeJob::SharedPtr job = (*it).second;
        if(job->decodeInfo.resource)
            job->decodeInfo.resource->setSuspended(suspend);
    }
}

bool SurfaceCache::isDecodeSuspended() const
{
    ScopedMutex _decode_lock(sDecodeMutex);
    return mDecodeSuspended;
}

void SurfaceCache::startDecodeJobs()
{
    ScopedMutex _widget_lock(Widget::lock());
    ScopedMutex _decode_lock(sDecodeMutex);
    while(DecodeJob::SharedPtr job = nextDecodeJob())
        startDecodeJob(job);
}

void SurfaceCache::startDecodeJob(const DecodeJob::SharedPtr &job)
{
    assert(MutexStack::locked(sDecodeMutex.name()));
    NTRACE(TRACE_GIBBON_SURFACE, "StartDecodeJob(%p:%zu) %s [%d]", job.get(), job->decodeInfo.loaders.size(), job->description().c_str(), mDecodeSlots);
    job->decodeInfo.queued = true;
    --mDecodeSlots;
    {
        ScopedMutex lock(mThreadPoolMutex);
        if(mThreadPool)
            mThreadPool->post(job);
    }
}

Surface::SharedPtr SurfaceCache::processSurfaceDecode(const SurfaceLoader::Source::SharedPtr &source, const shared_ptr<GibbonResource> &resource, uint8_t flags)
{
    {
        const std::vector<Listener::SharedPtr> l = gibbonApp()->getSurfaceCache()->listeners();
        for(std::vector<Listener::SharedPtr>::const_iterator it = l.begin(); it != l.end(); ++it)
            (*it)->onSurfaceDecode(source, resource);
    }
    if(Surface::SharedPtr surface = resource->surfaceDecode(source->group(), flags)) {
        surface->setDescription(source->url().str());
        return surface;
    }
    if(GibbonApplication::instance()->getScreen()->isRunning())
        Log::warn(TRACE_GIBBON_SURFACE, "Failed to decode surface: %s (%d bytes)", source->url().c_str(), resource->responseData().size());
    return Surface::SharedPtr();
}

void SurfaceCache::processSurfaceDecoded(const SurfaceLoader::SharedPtr &loader, const Surface::SharedPtr &surface, bool loaded)
{
    DEBUG_SURFACE_LOAD("SurfaceCache::processSurfaceDecoded(%p) DECODEJOBFINISHED to %s (%s) [0x%x]", loader.get(), loader->getSource()->url().str().c_str(),
                       surface ? surface->describe().c_str() : "N/A", loader->getSource()->loadFlags());
    {
        ScopedMutex lock(loader->mMutex);
        loader->mRequest.reset();
    }
    loader->onSurfaceDecoded(surface, loaded);
    INSTRUMENTATION_INTERVAL_SWITCHED_MARK(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()), "decode");
    INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("gibbon.surfaceLoad", loader.get()));
}

void SurfaceCache::finishDecodeJob(const DecodeJob::SharedPtr &job, bool loaded)
{
    if(job->surface) {
        const std::vector<Listener::SharedPtr> l = listeners();
        for(std::vector<Listener::SharedPtr>::const_iterator it = l.begin(); it != l.end(); ++it)
            (*it)->onSurfaceDecoded(job->decodeInfo, job->surface);
    }

    std::list<SurfaceLoader::SharedPtr> loaders;
    {
        ScopedMutex _lock(sDecodeMutex);
        if(job->decodeInfo.isQueued()) {
            NTRACE(TRACE_GIBBON_SURFACE, "FinishDecodeJob(%p:%zu) %s [%d]", job.get(), job->decodeInfo.loaders.size(), job->description().c_str(), mDecodeSlots);
            job->decodeInfo.queued = false;
            ++mDecodeSlots;
        }
        if(mDecodeJobs.erase(job->decodeInfo.source->cacheKey()) && job->surface) {
            ScopedMutex _lock(sMutex);
            SurfaceCache::CacheEntry *entry = mapValue(mBySurface, job->surface.get());
            assert(!job->decodeInfo.source->cacheKey().isEmpty());
            if(entry && !Debug::TestSurfaceCacheKeysDisabled) {
                assert(entry->cacheKey.isEmpty());
                entry->cacheKey = job->decodeInfo.source->cacheKey();
                assert(mByCacheKey.find(entry->cacheKey) == mByCacheKey.end());
                mByCacheKey[entry->cacheKey] = entry;
            }
        }
        std::swap(loaders, job->decodeInfo.loaders);
    }
    startDecodeJobs();
    while(!loaders.empty()) {
        const shared_ptr<SurfaceLoader> loader = loaders.front();
        loaders.pop_front();
        if(!job->surface || job->surface->isAccelerated() || !(loader->getSource()->loadFlags() & SurfaceLoader::Load_AcceleratedOnly))
            processSurfaceDecoded(loader, job->surface, loaded);
        else
            processSurfaceDecoded(loader, Surface::SharedPtr(), loaded);
    }
    job->decoded();
}

DataBuffer SurfaceCache::prepareCacheKey(const AseUrl &url, const HttpHeaders &headers, const DataBuffer &data)
{
    HttpHeaders::const_iterator it = headers.find("X-Gibbon-Cache-Control");
    if(it != headers.end()) {
        std::vector<std::string> controls;
        StringTokenizer::tokenize(it->second, controls, ",");
        for(std::vector<std::string>::iterator it = controls.begin(); it != controls.end(); ++it) {
            std::string &c = *it;
            trim(c);
            if (startsWith(c, "key=") && c.size() > 4)
                return c.substr(4);
        }
    }
    if(data.isEmpty())
        return url.str();
    return data.hash(DataBuffer::Hash_SHA256);
}

void SurfaceCache::LoadJob::execute()
{
    GibbonApplication::instance()->getSurfaceCache()->processLoadRequest(mLoader);
}

void SurfaceCache::DecodeJob::execute()
{
    {
        ScopedMutex _lock(mutex);
        if(state == State_Aborted)
            return;
        state = State_Decoding;
    }

    if(Debug::TestSurfaceLoadDelay)
        Thread::Sleep(Time(Debug::TestSurfaceLoadDelay));

    bool loaded = false;
    shared_ptr<SurfaceCache> surfaceCache = GibbonApplication::instance()->getSurfaceCache();
    if(decodeInfo.isOrphaned() && !decodeInfo.required) {
        NTRACE(TRACE_GIBBON_SURFACE, "Orphaned non-required surface: %s (%d bytes) [%lld]", decodeInfo.source->url().c_str(), decodeInfo.resource->responseData().size(),
               (Time::mono()-decodeInfo.accessed).ms());
    } else if(GibbonConfiguration::surfaceLoadIdle() && decodeInfo.isOrphaned() && Time::mono()-decodeInfo.accessed >= GibbonConfiguration::surfaceLoadIdle()*2) {
        NTRACE(TRACE_GIBBON_SURFACE, "Timeout to decode surface: %s (%d bytes) [%lld]", decodeInfo.source->url().c_str(), decodeInfo.resource->responseData().size(),
               (Time::mono()-decodeInfo.accessed).ms());
    } else {
        uint8_t flags = 0;
        if(decodeInfo.acceleratedOnly)
            flags |= Surface::Flags_Accelerated;
        NTRACE(TRACE_GIBBON_SURFACE, "Decode: %s [%s]", decodeInfo.source->url().str().c_str(), decodeInfo.source->cacheKey().c_str());
        surface = surfaceCache->processSurfaceDecode(decodeInfo.source, decodeInfo.resource, flags);
        if(surface)
            surface->setCacheKey(decodeInfo.source->cacheKey());
        NTRACE(TRACE_GIBBON_SURFACE, " + %s", surface ? surface->describe().c_str() : "N/A");
        loaded = true;
    }
    surfaceCache->finishDecodeJob(shared_from_this(), loaded);
}

void SurfaceCache::DecodeJob::decoded()
{
    ScopedMutex _lock(mutex);
    if(state == State_Decoding) {
        state = State_Decoded;
        stateCondition.broadcast();
    }
}

void SurfaceCache::DecodeJob::abort()
{
    bool aborted = false;
    {
        ScopedMutex _lock(mutex);
        if(state == State_Aborted) {
            //nothing to do...
        } else if(state == State_Idle) {
            state = State_Aborted;
            aborted = true;
        } else {
            while(state != State_Decoded)
                stateCondition.wait(&mutex);
        }
    }
    if(aborted) {
        NTRACE(TRACE_GIBBON_SURFACE, "AbortDecodeJob(%p:%zu) %s", this, decodeInfo.loaders.size(), description().c_str());
        GibbonApplication::instance()->getSurfaceCache()->finishDecodeJob(shared_from_this(), false);
    }
}

std::string SurfaceCache::DecodeJob::description() const
{
    ScopedMutex _lock(GibbonApplication::instance()->getSurfaceCache()->decodeLock());
    return decodeInfo.description();
}
