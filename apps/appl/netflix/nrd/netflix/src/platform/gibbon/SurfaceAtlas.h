/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SURFACEATLAS_H
#define SURFACEATLAS_H

#include "RectPacker.h"
#include "Surface.h"
#include <nrdbase/LinkedList.h>
#include <nrdbase/WeakPtrMap.h>
#include <map>
#include <vector>
#include <assert.h>

namespace netflix {
namespace gibbon {

class SurfaceCache;

class SurfaceAtlas
{
public:
    SurfaceAtlas();
    ~SurfaceAtlas();

    void setPadding(int width, int height) { mPadWidth = width; mPadHeight = height; }

    void setSurfaceCache(const shared_ptr<SurfaceCache>& cache);
    void setAtlasSize(int group, Coordinate width, Coordinate height);

    Surface::SharedPtr requestMaster(const Surface::SharedPtr& sub, int group, int width, int height, rectpacker::RectPacker::Node*& node);
    void updateSize(int group, const Surface::SharedPtr& master, int width, int height);

    void notifyDestroyed(rectpacker::RectPacker::Node* node);

    bool canPurge(const Surface::SharedPtr& surface) const;
    void purge(const Surface::SharedPtr& surface);

    float masterUtilization(const Surface::SharedPtr& surface) const;

    void removeFromMaster(const Surface::SharedPtr& surface);

private:
    struct SurfaceData
    {
        SurfaceData() : prev(0), next(0) { }

        rectpacker::RectPacker packer;
        Surface::WeakPtr master;
        std::vector<Surface::WeakPtr> subs;
        int group;

        SurfaceData *prev, *next;
    };

    bool canPurgeData(const SurfaceData* data) const;
    unsigned int use(SurfaceData* data, SurfaceCache* cache);

    struct AtlasData
    {
        AtlasData() : width(0), height(0) { }
        ~AtlasData() { data.deleteAll(); }

        LinkedList<SurfaceData*> data;
        int width, height;
    };

    weak_ptr<SurfaceCache> mCache;
    std::map<int, AtlasData> mData;
    std::map<Surface::WeakPtr, SurfaceData*> mBySurface;
    int mPadWidth, mPadHeight;
    Mutex mAtlasMutex;
};

inline bool SurfaceAtlas::canPurge(const Surface::SharedPtr& surface) const
{
    assert(surface->getFlags() & Surface::Flags_Master);
    const std::map<Surface::WeakPtr, SurfaceData*>::const_iterator it = mBySurface.find(surface);
    assert(it != mBySurface.end());
    return canPurgeData(it->second);
}

}} // netflix::gibbon

#endif
