/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Surface.h"
#include "SurfaceCache.h"
#include "Screen.h"

#include <nrdbase/Log.h>
#include <nrdbase/Backtrace.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>

#include <sstream>

using namespace netflix;
using namespace netflix::gibbon;

static AtomicCount sSerial;
Mutex Surface::sMutex(GIBBON_SURFACE_MUTEX, "Gibbon::Surface");

struct SurfaceObjectCount : public netflix::ObjectCount::Record
{
public:
    inline SurfaceObjectCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    virtual size_t cost(void *object) const { return static_cast<Surface*>(object)->dataSize(); }
    virtual std::string describe(void *object) const { return static_cast<Surface*>(object)->describe(); }
};

NRDP_DEFINE_OBJECTCOUNT_RECORD(SurfaceObjectCount, GibbonSurface);
NRDP_DEFINE_OBJECTCOUNT(GibbonSurfaceData);
NRDP_DEFINE_OBJECTCOUNT_RECORD(SurfaceObjectCount, GibbonSubsurface);
NRDP_DEFINE_OBJECTCOUNT(GibbonSurfacePixels);

Surface::Surface(Coordinate width, Coordinate height, uint8_t flags, int group)
    : mRect(0, 0, width, height), mData(0), mFlags(flags), mGroup(group), mNode(0), mPixels(0)
{
    mSerial = sSerial.ref();
    if(mode() == Master) {
        NRDP_OBJECTCOUNT_REF(GibbonSurface, this);
    } else {
        NRDP_OBJECTCOUNT_REF(GibbonSubsurface, this);
    }
    init();
}

Surface::~Surface()
{
    unsigned char* pixels = 0;
    {
        ScopedMutex locker(sMutex);
        std::swap(mPixels, pixels);
    }
    if(pixels) {
        free(pixels);
        NRDP_OBJECTCOUNT_COUNT(GibbonSurfacePixels, -static_cast<int>(dataSize()));
    }

    if(mode() == Master) {
        NRDP_OBJECTCOUNT_DEREF(GibbonSurface, this);
    } else {
        NRDP_OBJECTCOUNT_DEREF(GibbonSubsurface, this);
    }
    cleanup();
}

void Surface::modify(bool apply)
{
    if(shared_ptr<SurfaceCache> surfaceCache = GibbonApplication::instance()->getSurfaceCache())
        surfaceCache->modifiedSurface(shared_from_this());
    mSerial = sSerial.ref();
    if(apply)
        applyDisplayList();
}

void Surface::reuse(uint8_t flags)
{
    assert(matches(flags));
    shared_ptr<DisplayList> displayList;
    if(shared_ptr<SurfaceCache> surfaceCache = GibbonApplication::instance()->getSurfaceCache())
        surfaceCache->reusedSurface(shared_from_this());
    {
        ScopedMutex _lock(sMutex);
        std::swap(mDisplayList, displayList);
        mDescription.clear();
        mCacheKey.clear();
        mFlags = flags;
    }
    mSerial = sSerial.ref();
    mReuseCount.ref();
}

void Surface::applyDisplayList()
{
    shared_ptr<DisplayList> displayList;
    {
        ScopedMutex _lock(sMutex);
        std::swap(mDisplayList, displayList);
    }
    if(displayList) {
        GraphicsEngine engine;
        engine.begin(shared_from_this(), false);
        engine.apply(displayList, displayList->getRect(), getRect());
    }
}

std::string Surface::describe() const
{
    std::ostringstream s;
    s << "Surface(" << this << ":" << reuseCount() << ":" << mSerial;
    {
        const std::string cacheKey = ResourceManager::prettyCacheKey(this->cacheKey()).toString();
        if(!contains(mDescription, cacheKey))
            s << ":" << cacheKey;
    }
    s << ")";
    if(!isValid()) {
        s << " INVALID";
    } else {
        s << " a=" << hasAlpha() << " " << (!isAccelerated() ? "slow " : "") << "size=" << getWidth() << "x" << getHeight();
        if(mController)
            s << " f=" << mController->getCurrentFrame();
    }
    if(mFlags & Flags_Master)
        s << " master=" << mGroup;
    else if(mGroup != -1)
        s << " sub=" << mGroup << " pos=" << mRect.x << "," << mRect.y;
    s << ": " << mDescription;
    return s.str();
}

Surface::Controller::Controller(const Surface::SharedPtr &surface) : mSurface(surface), mMutex(GIBBON_SURFACECONTROLLER_MUTEX, "SurfaceControllerMutex")
{
}

Surface::SharedPtr Surface::Controller::getSurface() const
{
    ScopedMutex lock(SurfaceCache::lock());
    return mSurface.lock();
}

void Surface::Controller::onSurfaceChanged(const Rect &rect)
{
    std::vector<Listener::SharedPtr> listeners;
    {
        ScopedMutex _lock(mMutex);
        mListeners.lock(listeners);
    }
    if(Surface::SharedPtr surface = getSurface()) {
        for(std::vector<Listener::SharedPtr>::const_iterator it = listeners.begin(); it != listeners.end(); ++it)
            (*it)->onSurfaceChanged(surface, rect);
    }
}

void Surface::Controller::onSurfaceStarted(bool started)
{
    std::vector<Listener::SharedPtr> listeners;
    {
        ScopedMutex _lock(mMutex);
        mListeners.lock(listeners);
    }
    if(Surface::SharedPtr surface = getSurface()) {
        for(std::vector<Listener::SharedPtr>::const_iterator it = listeners.begin(); it != listeners.end(); ++it)
            (*it)->onSurfaceStarted(surface, started);
    }
}

void Surface::Controller::addListener(const Listener::SharedPtr &listener)
{
    ScopedMutex _lock(mMutex);
    assert(listener);
    mListeners.insert(listener);
}

void Surface::Controller::removeListener(const Listener::SharedPtr &listener)
{
    ScopedMutex _lock(mMutex);
    mListeners.erase(listener);
}

bool Surface::requestMaster()
{
    assert(!mNode);
    assert(mode() == Subsurface);
    NTRACE(TRACE_GIBBON_SURFACE, "Setting master for %p", this);
    mMaster = GibbonApplication::instance()->getSurfaceCache()->requestMaster(shared_from_this(), mGroup, (int)mRect.width, (int)mRect.height, mNode);
    if(mNode) {
        assert(mMaster);
        updateSubsurface();
        updateSubsurface_sys();
        return true;
    }
    mGroup = -1;
    NRDP_OBJECTCOUNT_DEREF(GibbonSubsurface, this);
    NRDP_OBJECTCOUNT_REF(GibbonSurface, this);
    assert(!mMaster);
    assert(!mNode);
    return false;
}

Surface::Data::Data() : mData(0), mFormat(Format_None), mBytesPerLine(0), mWidth(CoordinateMax), mHeight(CoordinateMax), mLockType(NoLock)
{
    NRDP_OBJECTCOUNT_REF(GibbonSurfaceData, this);
}

Surface::Data::Data(unsigned char *data, Coordinate width, Coordinate height, Format format, LockType type)
    : mData(0), mFormat(Format_None), mBytesPerLine(0), mWidth(0), mHeight(0), mLockType(NoLock)
{
    NRDP_OBJECTCOUNT_REF(GibbonSurfaceData, this);
    mBytesPerLine = int(mWidth * bytesPerPixel());
    lock(data, width, height, format, type);
}

Surface::Data::Data(const Surface::SharedPtr &surface, LockType type) : mData(0), mBytesPerLine(0), mLockType(type)
{
    NRDP_OBJECTCOUNT_REF(GibbonSurfaceData, this);
    lock(surface, type);
}

Surface::Data::~Data()
{
    NRDP_OBJECTCOUNT_DEREF(GibbonSurfaceData, this);
    unlock();
}

void Surface::Data::lock_internal()
{
    if(!mData && mWidth && mHeight) {
        NRDP_OBJECTCOUNT_COUNT(GibbonSurfacePixels, static_cast<int>(dataSize()));
        mData = static_cast<unsigned char*>(malloc(dataSize()));
    }
}

void Surface::Data::unlock_internal()
{
    assert(mData);
    NRDP_OBJECTCOUNT_COUNT(GibbonSurfacePixels, -static_cast<int>(dataSize()));
    free(mData);
    mData = 0;
}

void
Surface::Data::unlock()
{
    if(mSurface) {
        assert(mSurface->mData == this);
        unlockSurface();
        if(mData) {
            ScopedMutex locker(sMutex);
            std::swap(mData, mSurface->mPixels);
            if(mData)
                unlock_internal();
        }
        mSurface->mData = 0;
    }
    mWidth = 0;
    mHeight = 0;
    mBytesPerLine = 0;
    mLockType = NoLock;
    mFormat = Format_None;
}

void Surface::Data::lock(unsigned char* data, Coordinate width, Coordinate height, Format format, LockType type)
{
    mData = data;
    mWidth = width;
    mHeight = height;
    mFormat = format;
    mLockType = type;
    mBytesPerLine = int(mWidth * bytesPerPixel());
}

void Surface::Data::lock(const Surface::SharedPtr &surface, LockType type)
{
    if(type & WriteLock)
        surface->modify();
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    mOwnsData = false;
#endif
    mSurface = surface;
    assert(surface.get());
    assert(!surface->mData);
    assert(surface->isValid());
    mSurface->mData = this;
    mWidth = mSurface->getWidth();
    mHeight = mSurface->getHeight();
    mLockType = type;

    {
        ScopedMutex locker(sMutex);
        std::swap(mSurface->mPixels, mData);
    }

    if (!lockSurface(type))
        return;
    assert(mBytesPerLine != 0);
    assert(mData != 0);
}

void Surface::offset(Coordinate x, Coordinate y)
{
    const Rect srcRect = getRect(), dstRect = Rect(srcRect.x+x, srcRect.y+y, srcRect.size());
    copy(srcRect, dstRect);
}

Surface::SharedPtr Surface::duplicate(const Rect &rect) const
{
    uint8_t flags = Surface::Flags_None|Surface::Flags_RenderTarget;
    if(hasAlpha())
        flags |= Surface::Flags_Alpha;
    Surface::SharedPtr result = GibbonApplication::instance()->getSurfaceCache()->requestSurface(rect.width, rect.height, flags, true);
    result->setDescription(getDescription() + ":DUPLICATE");
    {
        GraphicsEngine graphics(result);
        graphics.setSmoothScale(false);
        graphics.setCompositionMode(GraphicsEngine::CompositionMode_Source);
        graphics.clear(result->getRect());
        graphics.blit(const_cast<Surface*>(this)->shared_from_this(), rect, result->getRect());
    }
    return result;
}

void Surface::copy(const Rect &srcRect, const Rect &dstRect)
{
    Surface::SharedPtr that = shared_from_this();
#if defined(GIBBON_SURFACE_COPY_PINGPONG) //"PingPong technique"
    static Surface::WeakPtr tmpWeakSurface;
    Surface::SharedPtr tmpSurface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(tmpWeakSurface);
    Coordinate w = tmpSurface ? tmpSurface->getWidth() : 0;
    Coordinate h = tmpSurface ? tmpSurface->getHeight() : 0;
    if(!tmpSurface || w < dstRect.width || h < dstRect.height) {
        if(dstRect.width > w)
            w = dstRect.width;
        if(dstRect.height > h)
            h = dstRect.height;
        tmpSurface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(w, h, Flags_RenderTarget);
        tmpSurface->setDescription("Surface Temporary Copy");
        tmpWeakSurface = tmpSurface;
    }
    const Rect tmpRect(0, 0, dstRect.width, dstRect.height);
    {
        GraphicsEngine graphics(tmpSurface);
        graphics.setSmoothScale(false);
        graphics.setCompositionMode(GraphicsEngine::CompositionMode_Source);
        graphics.blit(that, srcRect, tmpRect);
    }
    {
        GraphicsEngine graphics(that);
        graphics.setSmoothScale(false);
        graphics.setCompositionMode(GraphicsEngine::CompositionMode_Source);
        graphics.blit(tmpSurface, tmpRect, dstRect);
    }
#else
    GraphicsEngine graphics(that);
    graphics.setSmoothScale(false);
    graphics.setCompositionMode(GraphicsEngine::CompositionMode_Source);
    graphics.blit(that, srcRect, dstRect);
#endif
}

float Surface::masterUtilization() const
{
    if(!mMaster)
        return 1.;
    return GibbonApplication::instance()->getSurfaceCache()->masterUtilization(mMaster);
}

void Surface::updateSubsurface()
{
    assert(mNode);
    assert(mMaster);
    const rectpacker::Rect& r = mNode->rect;
    assert(mRect.width <= r.width());
    assert(mRect.height <= r.height());
    mRect.x = r.x;
    mRect.y = r.y;
}

class SurfaceUploadEvent : public GibbonApplication::Event
{
public:
    SurfaceUploadEvent(const Surface::SharedPtr &surface) : mSurface(surface) { }

    virtual void eventFired()
    {
        Surface::SharedPtr surface;
        {
            ScopedMutex lock(SurfaceCache::lock());
            surface = mSurface.lock();
        }
        if(surface)
            surface->upload();
    }
private:
    const Surface::WeakPtr mSurface;
};

void Surface::upload()
{
    unsigned char* pixels = 0;
    {
#if !defined(GIBBON_NO_THREADANIMATION)
        const bool isAnimationThread = Animation::isAnimationThread();
        ScopedMutex locker(sMutex);
        if(mPixels && !isAnimationThread) {
            locker.unlock();
            Animation::postEvent(new SurfaceUploadEvent(shared_from_this()));
            return;
        }
#else
        ScopedMutex locker(sMutex);
#endif
        std::swap(mPixels, pixels);
    }
    if(!pixels)
        return;

    NRDP_OBJECTCOUNT_COUNT(GibbonSurfacePixels, -static_cast<int>(dataSize()));
    if(mode() == Surface::Subsurface) {
        if(!mMaster && !requestMaster()) {
            assert(mode() == Surface::Master);

            upload(getRect(), pixels);
            free(pixels);
            return;
        }
        mMaster->upload(getRect(), pixels, atlasPadding());
    } else {
        upload(getRect(), pixels);
    }

    free(pixels);
}

#ifndef NDEBUG
bool Surface::WeakPtr::isSurfaceCacheLocked() const
{
    return MutexStack::locked(SurfaceCache::lock().name());
}
#endif

int Surface::bytesPerPixel(int flags)
{
    const shared_ptr<Screen> screen = GibbonApplication::instance()->getScreen();
    if(!screen)
        return 0;
    if((flags & Flags_Text))
        return bytesPerPixel(screen->getFormat(Screen::SurfaceFormat_Text));
    if(flags & Flags_Alpha)
        return bytesPerPixel(screen->getFormat(Screen::SurfaceFormat_Alpha));
    return bytesPerPixel(screen->getFormat(Screen::SurfaceFormat_Opaque));
}
