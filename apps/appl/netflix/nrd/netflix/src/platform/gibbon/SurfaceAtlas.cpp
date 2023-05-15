/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SurfaceAtlas.h"
#include "SurfaceCache.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"
#include "Screen.h"
#include "Widget.h"
#include <nrd/AppLog.h>

using namespace netflix;
using namespace netflix::gibbon;

SurfaceAtlas::SurfaceAtlas()
    : mPadWidth(0), mPadHeight(0), mAtlasMutex(GIBBON_SURFACEATLAS_MUTEX, "SurfaceAtlasMutex")
{
}

SurfaceAtlas::~SurfaceAtlas()
{
}

void SurfaceAtlas::setSurfaceCache(const SurfaceCache::SharedPtr& cache)
{
    mCache = cache;
}

void SurfaceAtlas::setAtlasSize(int group, Coordinate width, Coordinate height)
{
    AtlasData& atlas = mData[group];
    atlas.width = static_cast<int>(width);
    atlas.height = static_cast<int>(height);
}

void SurfaceAtlas::notifyDestroyed(rectpacker::RectPacker::Node* node)
{
    ScopedMutex atlasLock(mAtlasMutex);
    rectpacker::RectPacker::clear(node);
}

namespace netflix {
namespace gibbon {

class SurfaceAtlasCompare
{
public:
    bool operator()(SurfaceCache::CacheEntry* a, SurfaceCache::CacheEntry* b) const
    {
        if (a->accessed < b->accessed)
            return true;
        if (a->accessed > b->accessed)
            return false;
        return a < b;
    }
};

class SurfaceAtlasMaster
{
public:
    SurfaceAtlasMaster() : nonpurgeable(0), group(-1) { }
    int nonpurgeable;
    int group;
    std::vector<SurfaceCache::CacheEntry*> subs;
};

}}

float SurfaceAtlas::masterUtilization(const Surface::SharedPtr& surface) const
{
    ScopedMutex lock(SurfaceCache::lock());
    const std::map<Surface::WeakPtr, SurfaceData*>::const_iterator it = mBySurface.find(surface);
    if (it == mBySurface.end())
        return 1.;
    unsigned int used = 0;
    const unsigned int size = static_cast<unsigned int>(surface->getWidth() * surface->getHeight());
    const SurfaceData* data = it->second;
    std::vector<Surface::WeakPtr>::const_iterator dit = data->subs.begin();
    const std::vector<Surface::WeakPtr>::const_iterator dend = data->subs.end();
    while (dit != dend) {
        if (dit->use_count() > 1) {
            const Surface::SharedPtr sub = dit->lock();
            if (sub) {
                used += static_cast<unsigned int>(sub->getWidth() * sub->getHeight());
            }
        }
        ++dit;
    }
    assert(used <= size);
    return static_cast<float>(used) / size;
}

// Need to hold the surfacecache mutex when calling this!
bool SurfaceAtlas::canPurgeData(const SurfaceData* data) const
{
    NTRACE(TRACE_GIBBON_SURFACE, "can I purge master %p (cur %p size %zu)?", data->master.lock().get(), data, data->subs.size());
    assert(MutexStack::locked(SurfaceCache::lock().name()));
    std::vector<Surface::WeakPtr>::const_iterator it = data->subs.begin();
    const std::vector<Surface::WeakPtr>::const_iterator end = data->subs.end();
    while (it != end) {
        {
            Surface::SharedPtr subsurface = it->lock();
            NTRACE(TRACE_GIBBON_SURFACE, "sub %p (master %p) use %ld", subsurface.get(), data->master.lock().get(), it->use_count() - 1);
        }
        if (it->use_count() > 1)
            return false;
        ++it;
    }
    return true;
}

unsigned int SurfaceAtlas::use(SurfaceData* data, SurfaceCache* cache)
{
    assert(MutexStack::locked(SurfaceCache::lock().name()));
    unsigned int count = 0;
    std::vector<Surface::WeakPtr>::iterator sit = data->subs.begin();
    while (sit != data->subs.end()) {
        if (sit->use_count() <= 1) {
            Surface::SharedPtr es = sit->lock();
            NTRACE(TRACE_GIBBON_SURFACE, "Removed surface from master %p cur %p (%p %ld)", data->master.lock().get(), data, es.get(), sit->use_count());
            if (es) {
                es->mMaster.reset();
                assert(es->mNode);
                notifyDestroyed(es->mNode);
                es->mNode = 0;
                ++count;
                std::map<Surface*, SurfaceCache::CacheEntry*>::const_iterator eit = cache->mBySurface.find(es.get());
                if (eit != cache->mBySurface.end()) {
                    cache->remove(eit->second);
                }
            }
            sit = data->subs.erase(sit);
        } else {
            ++sit;
        }
    }
    return count;
}

void SurfaceAtlas::removeFromMaster(const Surface::SharedPtr& surface)
{
    assert(MutexStack::locked(SurfaceCache::lock().name()));

    assert(surface->mMaster);
    Surface::SharedPtr master = surface->mMaster;
    assert(master->getFlags() & Surface::Flags_Master);

    const std::map<Surface::WeakPtr, SurfaceData*>::iterator it = mBySurface.find(master);
    assert(it != mBySurface.end());
    // loop through subs and find surface
    std::vector<Surface::WeakPtr>::iterator sub = it->second->subs.begin();
    const std::vector<Surface::WeakPtr>::const_iterator end = it->second->subs.end();
    while (sub != end) {
        Surface::SharedPtr subsurface = sub->lock();
        if (subsurface && subsurface == surface) {
            it->second->subs.erase(sub);
            return;
        }
        ++sub;
    }
    assert(false && "removeFromMaster, surface not found");
}

void SurfaceAtlas::purge(const Surface::SharedPtr& surface)
{
    assert(surface->getFlags() & Surface::Flags_Master);
    SurfaceCache::SharedPtr cache = mCache.lock();
    if (!cache)
        return;
    const std::map<Surface::WeakPtr, SurfaceData*>::iterator it = mBySurface.find(surface);
    assert(it != mBySurface.end());
    const size_t sz = it->second->subs.size();
    const size_t removed = use(it->second, cache.get());
    assert(sz == removed);
    (void)sz;
    (void)removed;
    mBySurface.erase(it);
}

Surface::SharedPtr SurfaceAtlas::requestMaster(const Surface::SharedPtr& subsurface, int group, int width, int height, rectpacker::RectPacker::Node*& node)
{
    assert(group != -1);

    SurfaceCache::SharedPtr cache = mCache.lock();
    if (!cache) {
        Log::error(TRACE_GIBBON_SURFACE, "No SurfaceCache in SurfaceAtlas");
        return Surface::SharedPtr();
    }

    AtlasData& atlas = mData[group];
    LinkedList<SurfaceData*>& lst = atlas.data;
    if (lst.isEmpty())
        lst.append(new SurfaceData);

    // we need to potentially check all the previous rectpackers for the group since surfaces
    // contained within might have been purged, leaving room for new ones
    SurfaceData* cur = lst.last();
    while (cur) {
        ScopedMutex lock(SurfaceCache::lock());
        if (Surface::SharedPtr master = cur->master.lock()) {
            ScopedMutex atlasLock(mAtlasMutex);
            node = cur->packer.insert(width + mPadWidth, height + mPadHeight);
            if (!node) {
                // try the previous packer
                cur = cur->prev;
                continue;
            }
            NTRACE(TRACE_GIBBON_SURFACE, "Adding surface to atlas %p at %d,%d\n", master.get(), node->rect.x, node->rect.y);
            mBySurface[master] = cur;
            cur->subs.push_back(subsurface);
            //NTRACE(TRACE_GIBBON_SURFACE, "Putting surface of size %dx%d in group %d at %d,%d",
            //           width, height, group, node->rect.x, node->rect.y);
            NTRACE(TRACE_GIBBON_SURFACE, "Returning %d %p (cur %p sub %p)", __LINE__, master.get(), cur, subsurface.get());
            master->mGroup = group;
            return master;
        } else {
            // Couldn't lock the master, it's probably been fully purged
            NTRACE(TRACE_GIBBON_SURFACE, "Couldn't lock master for group %d (sub %p)", group, subsurface.get());
            mBySurface.erase(cur->master);
            SurfaceData* tmp = cur;
            cur = cur->prev;
            lst.remove(tmp);
            delete tmp;
        }
    }

    NTRACE(TRACE_GIBBON_SURFACE, "Couldn't fit %dx%d for group %d in an existing atlas", width, height, group);

    const int masterWidth = atlas.width ? atlas.width : GibbonConfiguration::surfaceAtlasWidth();
    const int masterHeight = atlas.height ? atlas.height : GibbonConfiguration::surfaceAtlasHeight();
    const int surfaceSize = Surface::estimateSize(masterWidth, masterHeight, Surface::Flags_Alpha);

    if (masterWidth < width || masterHeight < height) {
        // we'll never be able to fit the subsurface, don't even try to create the master
        NTRACE(TRACE_GIBBON_SURFACE, "Returning empty %d", __LINE__);
        return Surface::SharedPtr();
    }

    {
        ScopedMutex lock(SurfaceCache::lock());

        // We couldn't find room in an existing master for this group. Check if we can create a new master outright
        if (surfaceSize + cache->mCacheSize <= cache->mCacheSizeLimit) {
            // yes, create the master and retry
            Surface::SharedPtr master(new Surface(masterWidth, masterHeight, Surface::Flags_Alpha | Surface::Flags_Master));
            master->setDescription("SurfaceAtlas");
            NTRACE(TRACE_GIBBON_SURFACE, "Created master: %s [%d]", master->describe().c_str(), __LINE__);
            if (cache->addSurface(master)) {
                ScopedMutex atlasLock(mAtlasMutex);
                SurfaceData* sd = new SurfaceData;
                lst.append(sd);
                sd->master = master;
                sd->packer.init(masterWidth, masterHeight);
                sd->subs.push_back(subsurface);
                sd->group = group;
                node = sd->packer.insert(width + mPadWidth, height + mPadHeight);
                if (!node) {
                    // a reasonable application would never do this
                    NTRACE(TRACE_GIBBON_SURFACE, "Subsurface of %dx%d didn't fit in master of %dx%d",
                               width, height, masterWidth, masterHeight);
                    return Surface::SharedPtr();
                }
                mBySurface[master] = sd;
                master->mGroup = group;
                NTRACE(TRACE_GIBBON_SURFACE, "Returning %d %p (cur %p sub %p)", __LINE__, master.get(), sd, subsurface.get());
                return master;
            } else {
                // this is bad
                NTRACE(TRACE_GIBBON_SURFACE, "Couldn't add surface in SurfaceAtlas");
                return Surface::SharedPtr();
            }
        }
    }

    {
        ScopedMutex lock(SurfaceCache::lock());

        // Check if we can purge any existing master surfaces, we prioritize surfaces with other groups than ours
        std::vector<SurfaceData*> candidates;
        std::map<Surface::WeakPtr, SurfaceData*>::iterator it = mBySurface.begin();
        const std::map<Surface::WeakPtr, SurfaceData*>::const_iterator end = mBySurface.end();
        while (it != end) {
            SurfaceData* sd = it->second;
            Surface::SharedPtr surface = it->first.lock();
            NTRACE(TRACE_GIBBON_SURFACE, "Looking at %p %p %d %d", sd, surface.get(), sd->group, group);
            if (surface && SurfaceCache::matches(surface, masterWidth, masterHeight, Surface::Flags_Alpha) && canPurgeData(sd)) {
                if (sd->group == group) {
                    candidates.push_back(sd);
                } else {
                    //NTRACE(TRACE_GIBBON_SURFACE, "Purging other atlas");
                    // purge all the subsurfaces
                    use(sd, cache.get());
                    ScopedMutex atlasLock(mAtlasMutex);
                    assert(sd->subs.empty());
                    AtlasData& oldAtlas = mData[sd->group];
                    oldAtlas.data.remove(sd);
                    sd->packer.init(masterWidth, masterHeight);
                    node = sd->packer.insert(width + mPadWidth, height + mPadHeight);
                    if (!node) {
                        NTRACE(TRACE_GIBBON_SURFACE, "Subsurface of %dx%d didn't fit in master of %dx%d",
                                   width, height, masterWidth, masterHeight);
                        return Surface::SharedPtr();
                    }
                    sd->subs.push_back(subsurface);
                    sd->group = group;
                    atlas.data.append(sd);
                    surface->mGroup = group;
                    NTRACE(TRACE_GIBBON_SURFACE, "Returning %d %p (cur %p sub %p)", __LINE__, surface.get(), sd, subsurface.get());
                    return surface;
                }
            }
            ++it;
        }

        if (!candidates.empty()) {
            NTRACE(TRACE_GIBBON_SURFACE, "Purging own atlas");
            ScopedMutex atlasLock(mAtlasMutex);
            SurfaceData* sd = candidates.front();
            Surface::SharedPtr surface = sd->master.lock();
            use(sd, cache.get());
            assert(sd->subs.empty());
            AtlasData& oldAtlas = mData[sd->group];
            oldAtlas.data.remove(sd);
            sd->packer.init(masterWidth, masterHeight);
            node = sd->packer.insert(width + mPadWidth, height + mPadHeight);
            if (!node) {
                NTRACE(TRACE_GIBBON_SURFACE, "Subsurface of %dx%d didn't fit in master of %dx%d",
                           width, height, masterWidth, masterHeight);
                return Surface::SharedPtr();
            }
            sd->subs.push_back(subsurface);
            sd->group = group;
            atlas.data.append(sd);
            surface->mGroup = group;
            NTRACE(TRACE_GIBBON_SURFACE, "Returning %d %p (cur %p sub %p)", __LINE__, surface.get(), sd, subsurface.get());
            return surface;
        }
    }

    // purge widgets
    if(unsigned int purged = Widget::purgeAll(Widget::Purge_Offscreen)) {
        NTRACE(TRACE_GIBBON_SURFACE, "Purged: %s [%d]", prettyBytes(purged).c_str(), __LINE__);
        NRDP_UNUSED(purged);
    }

    {
        ScopedMutex lock(SurfaceCache::lock());

        // No room for a full master surface and no fully purgeable masters
        // Try to do a partial purge of a master in the same group

        std::map<Surface::WeakPtr, SurfaceData*>::const_iterator it = mBySurface.begin();
        const std::map<Surface::WeakPtr, SurfaceData*>::const_iterator end = mBySurface.end();
        while (it != end) {
            if (it->second->group == group) {
                // do we match?
                SurfaceData* data = it->second;
                Surface::SharedPtr surface = data->master.lock();
                if (SurfaceCache::matches(surface, masterWidth, masterHeight, Surface::Flags_Alpha)) {
                    // do we have any purgeable subsurfaces?
                    if (use(data, cache.get())) {
                        // yes, try again
                        ScopedMutex atlasLock(mAtlasMutex);
                        node = data->packer.insert(width + mPadWidth, height + mPadHeight);
                        if (node) {
                            // success, return
                            NTRACE(TRACE_GIBBON_SURFACE, "Reused node in atlas %p at %d,%d\n",
                                   surface.get(), node->rect.x, node->rect.y);
                            data->subs.push_back(subsurface);
                            surface->mGroup = group;
                            NTRACE(TRACE_GIBBON_SURFACE, "Returning %d %p (cur %p sub %p)", __LINE__, surface.get(), data, subsurface.get());
                            return surface;
                        }
                    }
                }
            }
            ++it;
        }

        // at this point we need to purge any existing non-atlased surfaces until we have room for the atlas
        if (surfaceSize + cache->mCacheSize > cache->mCacheSizeLimit) {
            for (SurfaceCache::CacheEntry *e = cache->mSurfaceList.first(); e; ) {
                if (e->surface->mode() == Surface::Master && !(e->surface->getFlags() & Surface::Flags_Master)) {
                    if (cache->canPurge(e)) {
                        e = cache->remove(e);
                        if (surfaceSize + cache->mCacheSize <= cache->mCacheSizeLimit)
                            break;
                        continue;
                    }
                }
                e = e->next;
            }
        }

        // at this point we just need to create the surface, regardless of if we're going over budget or not
        Surface::SharedPtr master(new Surface(masterWidth, masterHeight, Surface::Flags_Alpha | Surface::Flags_Master));
        master->setDescription("SurfaceAtlas");
        NTRACE(TRACE_GIBBON_SURFACE, "Created master%s: %s [%d]",
               ((cache->mCacheSize + surfaceSize) > cache->mCacheSizeLimit) ? ":OVERBUDGET" : "",
               master->describe().c_str(), __LINE__);
        if (cache->addSurface(master)) {
            ScopedMutex atlasLock(mAtlasMutex);
            SurfaceData* sd = new SurfaceData;
            lst.append(sd);
            sd->master = master;
            sd->packer.init(masterWidth, masterHeight);
            sd->subs.push_back(subsurface);
            sd->group = group;
            node = sd->packer.insert(width + mPadWidth, height + mPadHeight);
            if (!node) {
                // a reasonable application would never do this
                NTRACE(TRACE_GIBBON_SURFACE, "Subsurface of %dx%d didn't fit in master of %dx%d",
                           width, height, masterWidth, masterHeight);
                return Surface::SharedPtr();
            }
            mBySurface[master] = sd;
            master->mGroup = group;
            NTRACE(TRACE_GIBBON_SURFACE, "Returning %d %p (cur %p sub %p)", __LINE__, master.get(), sd, subsurface.get());
            return master;
        }
    }
    NTRACE(TRACE_GIBBON_SURFACE, "Returning empty %d", __LINE__);
    return Surface::SharedPtr();
}

void SurfaceAtlas::updateSize(int /*group*/, const Surface::SharedPtr& master, int width, int height)
{
    ScopedMutex atlasLock(mAtlasMutex);
    std::map<Surface::WeakPtr, SurfaceData*>::const_iterator sd = mBySurface.find(master);
    if (sd != mBySurface.end()) {
        assert(sd->second);
        sd->second->packer.resize(width, height);
    } else {
        assert(false);
    }
}
