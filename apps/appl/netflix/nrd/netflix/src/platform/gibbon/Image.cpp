
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Image.h>
#include <Debug.h>
#include <Screen.h>
#include <Widget.h>
#include <Animation.h>
#if GIBBON_BENCHMARKS
#include <Benchmarks.h>
#endif
#include <SurfaceCache.h>
#include <SurfaceCache.h>
#include <SurfaceDecoder.h>
#include <GibbonConfiguration.h>

#include "Image.h"

#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/NFErrorCode.h>
#include <nrdbase/ThreadPool.h>
#include <nrdbase/Semaphore.h>
#include <nrd/AppThread.h>
#include <nrdbase/Log.h>

#include <iostream>
#include <fstream>
#include <nrdbase/tr1.h>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonImage);

static int sID = 0xFF000000;

namespace netflix {
namespace gibbon {
class ImageSurfaceChangedEvent : public GibbonApplication::Event
{
public:
    inline ImageSurfaceChangedEvent(const Image::SharedPtr &image, const Rect &rect) : mImage(image), mRect(rect) {}
    virtual void eventFired()
    {
        if(Image::SharedPtr image = mImage.lock())
            image->onSurfaceChanged(Surface::SharedPtr(), mRect);
    }
    const Image::WeakPtr mImage;
    const Rect mRect;
};

class ImageSurfaceDecodedEvent : public GibbonApplication::Event
{
public:
    inline ImageSurfaceDecodedEvent(const Image::SharedPtr &image) : mImage(image) {}
    inline void eventFired()
    {
        ScopedMutex lock(Widget::lock());
        if(Image::SharedPtr image = getImage()) {
            image->surfaceLoaded(mSurface, mFlags);
            if(Widget::SharedPtr widget = image->getWidget())
                widget->needsRender();
        }
    }
    inline Image::SharedPtr getImage() const {
        ScopedMutex lock(Widget::lock());
        if(SurfaceLoader::SharedPtr loader = mLoader.lock()) {
            if(Image::SharedPtr image = mImage.lock()) {
                if(image->isActiveLoader(loader))
                    return image;
            }
        }
        return Image::SharedPtr();
    }

    const Image::WeakPtr mImage;
    SurfaceLoader::WeakPtr mLoader;
    Surface::SharedPtr mSurface;
    uint8_t mFlags;
};
class ImageSurfaceDownloadedEvent : public GibbonApplication::Event
{
public:
    enum SizeInfo { SizeIgnore, SizeSet, SizeLoad };
    inline ImageSurfaceDownloadedEvent(const shared_ptr<ImageSurfaceDecodedEvent> &decodeEvent) : mDecodedEvent(decodeEvent), mSizeInfo(SizeIgnore) {}
    inline void eventFired() {
        ScopedMutex lock(Widget::lock());
        if(Image::SharedPtr image = mDecodedEvent->getImage()) {
            image->mState = mState;
            image->mNetworkInfo = mNetworkInfo;
            DEBUG_SURFACE_LOAD("Image::onSurfaceDownloaded(%p) %s (%s:%d) [%p]", image.get(),
                               mDecodedEvent->mLoader.lock()->getSource()->url().str().c_str(),
                               mSize.toString().c_str(), mSizeInfo, mDecodedEvent->mLoader.lock().get());
            if(!mDecodedEvent->mSurface && mSizeInfo != SizeIgnore) {
                image->mSize = mSize;
                if(mSize.empty()) {
                    image->mFlags |= Image::Failed;
                }
                image->sendImageLoaded(mSizeInfo == SizeLoad);
            }
        }
    }
    shared_ptr<ImageSurfaceDecodedEvent> mDecodedEvent;
    Image::NetworkInfo mNetworkInfo;
    Resource::State mState;
    SizeInfo mSizeInfo;
    Size mSize;
};
class ImageSurfaceLoader : public SurfaceLoader, public enable_shared_from_this<ImageSurfaceLoader>
{
public:
    inline ImageSurfaceLoader(const shared_ptr<SurfaceLoader::Source> &source) : SurfaceLoader(source) { }
    inline void init(const Image::SharedPtr &image)
    {
        assert(!mDecodedEvent);
        mDecodedEvent.reset(new ImageSurfaceDecodedEvent(image));
        mDecodedEvent->mLoader = shared_from_this();
    }
    virtual bool isReady() const {
        if(!SurfaceLoader::isReady())
            return false;
        ScopedMutex lock(Widget::lock());
        if(Image::SharedPtr image = mDecodedEvent->getImage()) {
            if(Widget::SharedPtr widget = image->getWidget())
                return widget->isRendered();
            return true;
        }
        return false;
    }
    virtual void onDownloaded(const shared_ptr<GibbonResource> &resource, Resource::State state)
    {
        assert(resource);
        assert(mDecodedEvent);
        if(Image::SharedPtr image = mDecodedEvent->getImage()) {
            shared_ptr<ImageSurfaceDownloadedEvent> event(new ImageSurfaceDownloadedEvent(mDecodedEvent));
            event->mState = state;
            event->mNetworkInfo.httpStatusCode = resource->httpStatusCode();
            event->mNetworkInfo.errorCode = resource->errorCode();
            event->mNetworkInfo.metrics = resource->networkMetrics();
            event->mNetworkInfo.encodedSize = resource->responseData().size();

            if((getSource()->loadFlags() & SurfaceLoader::Load_LazyDecode)) {
                event->mSizeInfo = ImageSurfaceDownloadedEvent::SizeLoad;
            } else if(Widget::SharedPtr widget = image->getWidget()) {
                if(widget->isFlexible())
                    event->mSizeInfo = ImageSurfaceDownloadedEvent::SizeSet;
            }
            if(event->mSizeInfo != ImageSurfaceDownloadedEvent::SizeIgnore)
                event->mSize = resource->surfaceInfo().size;

#if !defined(GIBBON_NO_THREADANIMATION)
            if(!Animation::isAnimationThread()) {
#if GIBBON_BENCHMARKS
                assert(!Benchmarks::isRunning()); //you likely wanted synchronous :)
#endif
                Animation::postEvent(event);
                return;
            }
#endif
            event->eventFired();
        }
    }
    virtual void onSurfaceDecoded(const Surface::SharedPtr &surface, bool loaded)
    {
        assert(mDecodedEvent);
        if(Image::SharedPtr image = mDecodedEvent->getImage()) {
            DEBUG_SURFACE_LOAD("Image::onSurfaceDecoded(%p) %s (%s) [%p]", image.get(), getSource()->url().str().c_str(),
                               surface ? surface->describe().c_str() : "N/A", this);
            uint8_t flags = 0;
            if(loaded) {
                flags |= Image::Loaded_SendLoadedEvent;
                if(!surface && GibbonApplication::instance()->getScreen()->isRunning())
                    flags |= Image::Loaded_Failure;
            }
            mDecodedEvent->mSurface = surface;
            mDecodedEvent->mFlags = flags;
#if !defined(GIBBON_NO_THREADANIMATION)
            if(!Animation::isAnimationThread()) {
#if GIBBON_BENCHMARKS
                assert(!Benchmarks::isRunning()); //you likely wanted synchronous :)
#endif
                Animation::postEvent(mDecodedEvent);
                return;
            }
#endif
            mDecodedEvent->eventFired();
        }
    }
private:
    shared_ptr<ImageSurfaceDecodedEvent> mDecodedEvent;
};
}}

Image::Image(int id, Widget::SharedPtr widget, Type type) :
    mID(id == -1 ? sID++ : id), mType(type), mFlags(CanPurge | Visible), mGroup(-1),
    mScale(1.0f), mWidget(widget), mHAlign(Align_Normal), mVAlign(Align_Normal),
    mSourceRect(0, 0, CoordinateMax, CoordinateMax)
{
    NRDP_OBJECTCOUNT_REF(GibbonImage, this);
}

Image::~Image()
{
    ScopedMutex lock(Widget::lock());
    stop();
    NRDP_OBJECTCOUNT_DEREF(GibbonImage, this);
}

SurfaceLoader::Source::SharedPtr Image::createSource(const Variant &value)
{
    unsigned int loadFlags = SurfaceLoader::Load_Required;
    SurfaceLoader::Source::SharedPtr result(new SurfaceLoader::Source);
    if(value.isStringMap()) {
        if(value.mapValue<bool>("highPriority", 0, false))
            loadFlags |= SurfaceLoader::Load_HighPriority;
        if(!value.mapValue<bool>("async", 0, true))
            loadFlags |= SurfaceLoader::Load_Synchronous;
        if(value.contains("lazy"))
            loadFlags |= (value.mapValue<unsigned int>("lazy", 0, 0) & (SurfaceLoader::Load_LazyDownload|SurfaceLoader::Load_LazyDecode));
        if(value.mapValue<bool>("fast", 0, false))
            loadFlags |= SurfaceLoader::Load_AcceleratedOnly;
        if(!value.mapValue<bool>("required", 0, true))
            loadFlags &= ~SurfaceLoader::Load_Required;

        const Variant::StringMapIterator end = value.stringMapEnd();
        const Variant::StringMapIterator h = value.find("headers");
        if(h != end && h->second.isStringMap()) {
            const Variant &hdrs = h->second;
            HttpHeaders headers = result->headers();
            const Variant::StringMapIterator hend = hdrs.stringMapEnd();
            for(Variant::StringMapIterator it = hdrs.stringMapBegin(); it != hend; ++it)
                headers[it->first] = it->second.value<std::string>();
            result->setHeaders(headers);
        }
        result->setUrl(value.mapValue<std::string>("url"));
        result->setData(value.mapValue<DataBuffer>("data"));
        if(Debug::TestScreenCoordinateScale)
            result->setScale(value.mapValue("scale", 0, GibbonApplication::instance()->getScreen()->getCoordinateScale()));
        int val = value.mapValue("timeout", 0, -1);
        if(val >= 0)
            result->setTimeout(Time::fromMS(val));
        val = value.mapValue("connectTimeout", 0, -1);
        if(val >= 0)
            result->setConnectTimeout(Time::fromMS(val));
        val = value.mapValue("stallTimeout", 0, -1);
        if(val >= 0)
            result->setStallTimeout(Time::fromMS(val));
    } else if(value.isString()) {
        result->setUrl(value.string());
        result->setScale(GibbonApplication::instance()->getScreen()->getCoordinateScale());
    }
    result->setLoadFlags(loadFlags);
    return result;
}

void Image::load(const SurfaceLoader::Source::SharedPtr &source)
{
    assert(MutexStack::locked(Widget::lock().name()));

    assert(source);
    if(mSource && source->compare(mSource) && (hasFailed() || mSurface || mLoader)) {
        DEBUG_SURFACE_LOAD("Image::load(%p) not loading, hasFailed: %d mSurface: %p mLoader %p",
                         this, hasFailed(), mSurface.get(), mLoader.get());
        return;
    }

    stop();
    mSource.reset();
    purge();

    mSource = source;
    DEBUG_SURFACE_LOAD("Image::load(%p) %s", this, mSource->describe(true).c_str());

    mFlags &= ~Failed;
    mSize = Size(0, 0);
    mScale = source->scale();
    mNetworkInfo = NetworkInfo();
    if(hasUrl())
        sendRequest();
}


bool Image::isLoading() const
{
    ScopedMutex lock(Widget::lock());
    return mLoader.get();
}

bool Image::shouldDownload() const
{
    if(loadFlags() & SurfaceLoader::Load_LazyDownload)
        return false;
    for(Widget::SharedPtr widget = getWidget(); widget; widget = widget->getParent()) {
        if(!widget->shouldLoadImages())
            return false;
    }
    return true;
}

void Image::sendRequest()
{
    PERFORMANCE_MARKER_SCOPED("image.sendrequest");

    assert(mSource);
    assert(!mLoader);
    assert(MutexStack::locked(Widget::lock().name()));

    mSource->setGroup(mGroup);

    SurfaceCache::SharedPtr surfaceCache = GibbonApplication::instance()->getSurfaceCache();
    if(Surface::SharedPtr surface = surfaceCache->quickFind(mSource->cacheKey())) {
        bool useSurface = true;
        if(Widget::SharedPtr widget = getWidget()) {
            if(getType() == Type_Foreground && widget->isFlexible() && widget->isRendering()) {
                const Rect widgetRect(widget->getPaddedRect()),
                           renderRect(calculateRenderRect(surface->getWidth(), surface->getHeight()));
                if(mHAlign == Align_Normal && mVAlign == Align_Normal) {
                    if(widgetRect.width < renderRect.width || widgetRect.height < renderRect.height)
                        useSurface = false;
                } else {
                    useSurface = widgetRect.width != renderRect.width || widgetRect.height != renderRect.height;
                }
                if(!useSurface) {
                    DEBUG_SURFACE_LOAD("Image::sendRequest(%p) UNUSABLE %s %s (%s)", this, mSource->describe().c_str(),
                                     renderRect.toString().c_str(), widget->describe(true).c_str());
                }
            }
        }
        if(useSurface) {
            DEBUG_SURFACE_LOAD("Image::sendRequest(%p) FOUND %s", this, mSource->describe().c_str());
            mState = Resource::State_SurfaceCache;
            mNetworkInfo.metrics.clear();
            surfaceLoaded(surface, Loaded_SendLoadedEvent);
            return;
        }
    }

    if(!shouldDownload()) {
        DEBUG_SURFACE_LOAD("Image::sendRequest(%p) AVOID %s", this, mSource->describe().c_str());
        sendImageLoaded(false);
        return;
    }

    mLoader.reset(new ImageSurfaceLoader(getSource()));
    mLoader->init(shared_from_this());
    DEBUG_SURFACE_LOAD("Image::sendRequest(%p) LOAD %s [%p]", this, mSource->describe().c_str(), mLoader.get());
    surfaceCache->load(mLoader);
}

Surface::SharedPtr Image::reload()
{
    PERFORMANCE_MARKER_SCOPED("image.reload");
    assert(MutexStack::locked(Widget::lock().name()));
    if(mSource && !hasFailed() && !mSurface && hasUrl() && !mLoader) {
        DEBUG_SURFACE_LOAD("** Image::reload(%p) %s", this, mSource->describe().c_str());
        mNetworkInfo = NetworkInfo();
        if(unsigned int lazy = (mSource->loadFlags() & SurfaceLoader::Load_LazyMask))
            mSource->setLoadFlags(mSource->loadFlags() & ~lazy);
        sendRequest();
    }
    GibbonApplication::instance()->getSurfaceCache()->touch(cacheKey());
    if(mSurface) {
        if(mSurface->getController())
            start();
        return mSurface;
    }
    return Surface::SharedPtr();
}

bool Image::hasAlpha() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(Surface::SharedPtr surface = getSurface())
        return surface->hasAlpha();
    return true;
}

Coordinate Image::getWidth() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    return mSize.width;
}

Coordinate Image::getHeight() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    return mSize.height;
}

Rect Image::calculateRenderRect(Coordinate width, Coordinate height) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    Rect result = mSourceRect;
    bool intersect = false;
    if(result.width == CoordinateMax)
        result.width = width - result.x;
    else
        intersect = true;
    if(result.height == CoordinateMax)
        result.height = height - result.y;
    else
        intersect = true;
    if(intersect)
        result.intersect(Rect(0, 0, width, height));
    return result;
}

void Image::setSourceRect(const Rect &rect)
{
    assert(MutexStack::locked(Widget::lock().name()));
    const Rect oldSourceRect = mSourceRect;
    Widget::SharedPtr widget;
    if(oldSourceRect != rect) {
        mSourceRect = rect;
        if(Widget::SharedPtr widget = getWidget()) {
            if(mType == Type_Foreground && rect.size() != oldSourceRect.size()) {
                widget->parentNeedsRender();
                widget->needsRect();
            }
            widget->needsRender();
        }
    }
}

Rect Image::getDestinationRect(const Rect &dest) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    const AspectFitMode aspect = aspectFitMode();
    unsigned halign = mHAlign;
    unsigned valign = mVAlign;
    if(aspect == Aspect_None && (halign & (Align_Stretch|Align_Tile)) && (valign & (Align_Stretch|Align_Tile)))
        return dest;

    Rect rect;
    {
        const Rect src = getRenderRect();
        switch (aspect) {
        case Image::Aspect_None:
            rect = Rect(dest.x, dest.y, src.width, src.height);
            break;
        case Image::Aspect_Fit:
        case Image::Aspect_FitHorizontal:
        case Image::Aspect_FitVertical: {
            const float hratio = static_cast<float>(dest.width) / static_cast<float>(src.width);
            const float vratio = static_cast<float>(dest.height) / static_cast<float>(src.height);
            const bool horizontal = (aspect == Image::Aspect_FitHorizontal || (aspect == Image::Aspect_Fit && hratio <= vratio));
            const float ratio = (horizontal ? hratio : vratio);
            rect.x = dest.x;
            rect.y = dest.y;
            rect.width = GIBBON_ROUND(src.width * ratio);
            rect.height = GIBBON_ROUND(src.height * ratio);
            halign &= ~(Image::Align_Stretch|Image::Align_MaintainAspectRatio);
            valign &= ~(Image::Align_Stretch|Image::Align_MaintainAspectRatio);
            if(horizontal) {
                // since we're stretched to the full width center/right is meaningless
                halign &= ~(Image::Align_Center|Image::Align_Right);
            } else {
                // since we're stretched to the full height center/bottom is meaningless
                valign &= ~(Image::Align_Center|Image::Align_Bottom);
            }
            break; }
        }
    }

    struct {
        Coordinate &pos, &size;
        const Coordinate destSize;
        const unsigned alignment;
    } dimensions[] = {
        { rect.x, rect.width, dest.width, halign },
        { rect.y, rect.height, dest.height, valign }
    };
    for(int i=0; i<2; ++i) {
        if(dimensions[i].alignment & (Align_Tile|Align_Stretch)) {
            dimensions[i].size = dimensions[i].destSize;
        } else if(halign & Align_Center) {
            const Coordinate diff = dimensions[i].destSize - dimensions[i].size;
            if(diff > 0)
                rect.x += ((diff + 1) / 2);// round up
        } else if(halign & Align_Right) {
            const Coordinate diff = dimensions[i].destSize - dimensions[i].size;
            if(diff > 0)
                dimensions[i].pos += diff;
        }
    }
    return rect;
}

void Image::setHorizontalAlignment(uint8_t a)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mHAlign != a) {
        mHAlign = a;
        if(Widget::SharedPtr widget = getWidget()) {
            if(mType == Type_Foreground) {
                widget->parentNeedsRender();
                widget->needsRect();
            }
            widget->needsRender();
        }
    }
}

void Image::setVisible(bool v)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if (v != static_cast<bool>(mFlags & Visible)) {
        if (v) {
            mFlags |= Visible;
        } else {
            mFlags &= ~Visible;
        }
        if(Widget::SharedPtr widget = getWidget()) {
            if(mType == Type_Foreground) {
                widget->parentNeedsRender();
                widget->needsRect();
            }
            widget->needsRender();
        }
    }
}

void Image::setVerticalAlignment(uint8_t a)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(mVAlign != a) {
        mVAlign = a;
        if(Widget::SharedPtr widget = getWidget()) {
            if(mType == Type_Foreground) {
                widget->parentNeedsRender();
                widget->needsRect();
            }
            widget->needsRender();
        }
    }
}

void Image::onSurfaceChanged(const Surface::SharedPtr &, const Rect &rect)
{
#ifdef GIBBON_NO_THREADANIMATION
    if(!Application::isAppThread()) {
        GibbonApplication::instance()->postEvent(new ImageSurfaceChangedEvent(shared_from_this(), rect));
        return;
    }
#else
    if(!Animation::isAnimationThread()) {
        Animation::postEvent(new ImageSurfaceChangedEvent(shared_from_this(), rect));
        return;
    }
#endif
    ScopedMutex _lock(Widget::lock());
    if (mFlags & Running) {
        if(Widget::SharedPtr widget = getWidget()) {
            if(widget->isOnScreen()) {
                upload();
                widget->needsRender();
            } else {
                stop();
            }
        }
    }
    mFlags &= ~Empty;
}

unsigned int Image::surfaceLoaded(const Surface::SharedPtr &surface, uint8_t loadedFlags)
{
    unsigned int released = 0;
    mLoader.reset();
    PERFORMANCE_MARKER_SCOPED("image.loaded");
    assert(MutexStack::locked(Widget::lock().name()));
    bool sendToWidget = false;
    if(surface != mSurface) {
        PERFORMANCE_MARKER_SCOPED("image.loaded.changed");
        if(mSurface && mSurface->getController()) {
            stop();
            mSurface->getController()->removeListener(shared_from_this());
        }

        const int useCount = mSurface ? mSurface.use_count() : 0;
        if(useCount == 1) { // Yuck, NRDP-3329
            ScopedMutex locker(SurfaceCache::lock());
            released = mSurface->calculateSize();
            mSurface = surface;
        } else {
            if(useCount == 2)
                released = mSurface->calculateSize();
            mSurface = surface;
        }
        if(mSurface) {
            GibbonApplication::instance()->getSurfaceCache()->touch(cacheKey());
            if(mSurface->getController())
                mSurface->getController()->addListener(shared_from_this());
            mSize = mSurface->getSize();
        }
        sendToWidget = true;
    } else if(loadedFlags & Loaded_Failure) {
        sendToWidget = true;
    }

    if(loadedFlags & Loaded_Failure)
        mFlags |= Failed;
    if(sendToWidget)
        sendImageLoaded(loadedFlags & Loaded_SendLoadedEvent);

    upload();
    return released;
}

void Image::upload()
{
    if(mSurface && GibbonApplication::instance()->getScreen()->isRunning())
        mSurface->upload();
}

bool Image::isAnimated() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    return mSurface && mSurface->getController();
}

void Image::start()
{
    assert(MutexStack::locked(Widget::lock().name()));
    if ((mFlags & Running) || !mSurface)
        return;

    mFlags |= (Running | Empty);
    mSurface->start();
}

void Image::stop()
{
    assert(MutexStack::locked(Widget::lock().name()));
    if((mFlags & Running) && mSurface) {
        mFlags &= ~Running;
        mSurface->stop();
    }

    if(mLoader) {
        DEBUG_SURFACE_LOAD("Image::abortRequest(%p) %s [%p]", this, mSource->describe().c_str(), mLoader.get());
        GibbonApplication::instance()->getSurfaceCache()->abort(mLoader);
        mLoader.reset();
    }
}

bool Image::isActiveLoader(const SurfaceLoader::SharedPtr &loader) const
{
    assert(loader);
    assert(MutexStack::locked(Widget::lock().name()));
    return loader == mLoader;
}

Surface::SharedPtr Image::getSurface() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    return mSurface;
}

bool Image::isRunning() const
{
    assert(MutexStack::locked(Widget::lock().name()));
    return (mFlags & Running);
}

void Image::render(GraphicsEngine *graphics, const Rect &globalRect)
{
    assert(MutexStack::locked(Widget::lock().name()));

    PERFORMANCE_MARKER_SCOPED("image.render");

    Surface::SharedPtr imageSurface = reload();
    if(!imageSurface || !imageSurface->isValid() || (mFlags & Empty))
        return;

    if(mGroup != imageSurface->group()) {
        NTRACE(TRACE_GIBBON_SURFACE, "Image group != surface group, util %f", imageSurface->masterUtilization());
        if(imageSurface->masterUtilization() <= 0.2) { // Less or equal than 20%
            NTRACE(TRACE_GIBBON_SURFACE, "Making copy");
            const Rect dstRect(0, 0, imageSurface->getWidth(), imageSurface->getHeight());
            Surface::SharedPtr copy = GibbonApplication::instance()->getSurfaceCache()->requestSurface(dstRect.width,
                                                                                                       dstRect.height,
                                                                                                       imageSurface->getFlags()
                                                                                                       | Surface::Flags_RenderTarget);
            if(copy) {
                {
                    GraphicsEngine engine(copy);
                    engine.setCompositionMode(GraphicsEngine::CompositionMode_Source);
                    engine.blit(imageSurface, dstRect, dstRect);
                }

                SurfaceCache::SharedPtr surfaceCache = GibbonApplication::instance()->getSurfaceCache();
                if(surfaceCache) {
                    imageSurface = copy;
                    const std::string descr = mSurface->getDescription();
                    surfaceCache->replace(mSurface, copy);
                    assert(mSurface == copy);
                    copy->setDescription(descr + ":COPY");
                }
            } else {
                Log::error(TRACE_GIBBON_SURFACE) << "Failed to get surface for group separation copy, " << dstRect.size();
            }
        }
    }

    Rect src = getRenderRect(), dest(0, 0, globalRect.width, globalRect.height);
    if(dest.width == CoordinateMax)
        dest.width = src.width;
    if(dest.height == CoordinateMax)
        dest.height = src.height;

    if(dest.width > 0 && dest.height > 0 && !src.empty()) {
#if 1
        const bool debugRender = false;
#else
        const bool debugRender = true;
#endif
        if(debugRender) {
            std::string description = StringFormatter::sformat("%p:%p", this, mSurface.get());
            if(Widget::SharedPtr widget = getWidget())
                description = widget->describe() + ":" + description;
            Log::error(TRACE_LOG, "RENDER(%s) %s (%s->%s) :: (%s) [%f]", description.c_str(), globalRect.toString().c_str(),
                       src.toString().c_str(), dest.toString().c_str(), imageSurface->describe().c_str(), mScale);
        }

        bool widgetClipped = true;
        unsigned int halign = getHorizontalAlignment(), valign = getVerticalAlignment();

        if(Widget::SharedPtr widget = getWidget())
            widgetClipped = widget->getClipEnabled();

        Rect rect;
        struct {
            /* pos and size are references to rect and indicate the position
             * and size of each blit. E.g. if we're rendering a 10x10 image
             * into destRect(20, 20, 100, 100) rect will be 20, 20, 10, 10.
             */
            Coordinate &pos, &size;

            /* srcPos and srcSize are in source coordinates. */
            Coordinate &srcPos, &srcSize;

            const Coordinate destPos;
            const Coordinate destSize;
            const unsigned int &alignment;

            /* tileCount is the number of full blits per dimension,
             * e.g. It's always 1 unless tiling is enabled. If we blit an
             * image of 10x10 into area of 11x11 tileCount will be 1 and
             * tileExtra 1. Note that when we use tileExtra to modify sizes
             * and offsets into the src rects we have to divide by ratio in
             * case we're scaled and tiling (only possible when using
             * stretch aspect */
            int16_t tileCount, tileExtra;
        } dimensions[] = {
            { rect.x, rect.width, src.x, src.width, dest.x, dest.width, halign, 1, 0 },
            { rect.y, rect.height, src.y, src.height, dest.y, dest.height, valign, 1, 0 }
        };
        const Image::AspectFitMode aspectFitMode = this->aspectFitMode();
        float ratio = 1.0;
        switch (aspectFitMode) {
        case Image::Aspect_None:
            rect = Rect(dest.x, dest.y, src.width, src.height);
            break;
        case Image::Aspect_Fit:
        case Image::Aspect_FitHorizontal:
        case Image::Aspect_FitVertical: {
            const float hratio = static_cast<float>(dest.width / mScale) / static_cast<float>(src.width);
            const float vratio = static_cast<float>(dest.height / mScale) / static_cast<float>(src.height);
            const bool horizontal = (aspectFitMode == Image::Aspect_FitHorizontal || (aspectFitMode == Image::Aspect_Fit && hratio <= vratio));
            ratio = (horizontal ? hratio : vratio);
            rect.x = dest.x;
            rect.y = dest.y;
            rect.width = GIBBON_ROUND(src.width * ratio);
            rect.height = GIBBON_ROUND(src.height * ratio);
            halign &= ~(Image::Align_Stretch|Image::Align_MaintainAspectRatio);
            valign &= ~(Image::Align_Stretch|Image::Align_MaintainAspectRatio);
            if(horizontal) {
                // since we're stretched to the full width center/right is meaningless
                halign &= ~(Image::Align_Center|Image::Align_Right);
            } else {
                // since we're stretched to the full height center/bottom is meaningless
                valign &= ~(Image::Align_Center|Image::Align_Bottom);
            }
            break; }
        }

        if(!gibbon_float_compare(mScale, 1.0f)) {
            rect.width = roundf(float(rect.width) * mScale);
            rect.height = roundf(float(rect.height) * mScale);
        }

        enum { Horizontal = 0, Vertical = 1 };

        for(int i=0; i<2; ++i) {
            unsigned int align = dimensions[i].alignment;
            const bool destBigger = dimensions[i].destSize > dimensions[i].size;
            if(align & Image::Align_Tile) {
                align &= ~Image::Align_Stretch;
                dimensions[i].tileCount = static_cast<int16_t>(static_cast<float>(dimensions[i].destSize) / static_cast<float>(dimensions[i].size));
                dimensions[i].tileExtra = int(dimensions[i].destSize - (dimensions[i].tileCount * dimensions[i].size));
            } else if(align & Image::Align_Stretch) {
                dimensions[i].size = dimensions[i].destSize;
            } else if(align & Image::Align_Center) {
                if(destBigger) {
                    dimensions[i].pos = roundf(fabsf(dimensions[i].destSize - dimensions[i].size) / 2);
                } else {
                    dimensions[i].srcPos += roundf(fabsf(dimensions[i].destSize - dimensions[i].size) / ratio / 2 / mScale);
                }
            } else if(align & Image::Align_Right) {
                if(destBigger) {
                    dimensions[i].pos = dimensions[i].destSize - dimensions[i].size;
                    if(dimensions[i].pos < 0)
                        dimensions[i].pos = -dimensions[i].pos;
                } else {
                    dimensions[i].srcPos += roundf(fabsf(dimensions[i].destSize - dimensions[i].size) / ratio / mScale);
                }
            }

            if(!(align & Image::Align_Stretch) && aspectFitMode == Image::Aspect_None) {
                dimensions[i].size = std::min(dimensions[i].destSize, Coordinate(roundf(float(dimensions[i].srcSize) * mScale)));
                dimensions[i].srcSize = std::min(Coordinate(roundf(dimensions[i].destSize / mScale)), Coordinate(dimensions[i].srcSize));
            }
        }
        assert(!src.empty());
        // printf("[%s] %s:%d: tile %d/%d rect %s src %s dest %s\n", __func__, __FILE__, __LINE__,
        //        dimensions[Horizontal].tileCount,
        //        dimensions[Vertical].tileCount,
        //        rect.toString().c_str(),
        //        src.toString().c_str(),
        //        dest.toString().c_str());
        Coordinate y = rect.y;
        if((valign & (Image::Align_Right|Image::Align_Tile)) == (Image::Align_Right|Image::Align_Tile))
            y += dimensions[Vertical].tileExtra;

        Rect oldClip;
        bool clip = false;
        if(aspectFitMode != Image::Aspect_None && !widgetClipped && !globalRect.contains(rect)) {
            oldClip = graphics->clip();
            graphics->setClip(globalRect);
            clip = true;
        }
        for(int v=0; v<dimensions[Vertical].tileCount; ++v) {
            Coordinate x = rect.x;
            if((halign & (Image::Align_Right|Image::Align_Tile)) == (Image::Align_Right|Image::Align_Tile))
                x += dimensions[Horizontal].tileExtra;
            for(int h=0; h<dimensions[Horizontal].tileCount; ++h) {
                const Rect target(globalRect.x + x, globalRect.y + y, rect.width, rect.height);
                if(debugRender)
                    Log::error(TRACE_LOG, "+ %s -> %s [%d] (%d)", src.toString().c_str(), target.toString().c_str(), __LINE__, h);
                graphics->blit(imageSurface, src, target);
                x += rect.width;
            }
            if(dimensions[Horizontal].tileExtra) {
                Rect target(globalRect.x + x, globalRect.y + y, dimensions[Horizontal].tileExtra, rect.height);
                Rect source(src.x, src.y, roundf(float(dimensions[Horizontal].tileExtra) / ratio / mScale), src.height);
                if(halign & Image::Align_Right) {
                    source.x += src.width - source.width;
                    target.x = dest.x + globalRect.x;
                }
                if(debugRender)
                    Log::error(TRACE_LOG, "+ %s -> %s [%d]", source.toString().c_str(), target.toString().c_str(), __LINE__);
                graphics->blit(imageSurface, source, target);
            }
            y += rect.height;
        }
        if(dimensions[Vertical].tileExtra) {
            {
                Rect target(globalRect.x + rect.x, globalRect.y + rect.y, rect.width, dimensions[Vertical].tileExtra);
                if(!(valign & Image::Align_Bottom))
                    target.y += (rect.height * dimensions[Vertical].tileCount);
                if((halign & (Image::Align_Right|Image::Align_Tile)) == (Image::Align_Right|Image::Align_Tile))
                    target.x += dimensions[Horizontal].tileExtra;

                Rect source(src.x, src.y, src.width, roundf(float(dimensions[Vertical].tileExtra) / ratio / mScale));
                if(valign & Image::Align_Bottom) {
                    source.y += src.height - source.height;
                    target.y = dest.y + globalRect.y;
                }

                for(int h=0; h<dimensions[Horizontal].tileCount; ++h) {
                    if(debugRender)
                        Log::error(TRACE_LOG, "+ %s -> %s [%d] (%d)", source.toString().c_str(), target.toString().c_str(), __LINE__, h);
                    graphics->blit(imageSurface, source, target);
                    target.x += rect.width;
                }
            }
            if(dimensions[Horizontal].tileExtra) { // corner tile
                Rect source(src.x, src.y, roundf(float(dimensions[Horizontal].tileExtra / mScale)),
                            roundf(float(dimensions[Vertical].tileExtra / mScale)));
                Rect target(rect.x + globalRect.x, rect.y + globalRect.y, source.width, source.height);
                if(halign & Image::Align_Right) {
                    source.x += ((src.width) - target.width);
                    target.width *= mScale;
                } else {
                    target.width *= mScale;
                    target.x += ((dimensions[Horizontal].destSize) - target.width);
                }
                if(valign & Image::Align_Bottom) {
                    source.y += ((src.height) - target.height);
                    target.height *= mScale;
                } else {
                    target.height *= mScale;
                    target.y += ((dimensions[Vertical].destSize) - target.height);
                }
                if(debugRender)
                    Log::error(TRACE_LOG, "+ %s -> %s [%d]", source.toString().c_str(), target.toString().c_str(), __LINE__);
                graphics->blit(imageSurface, source, target);
            }
        }
        if(clip)
            graphics->setClip(oldClip);
    }
}

Size Image::sizeContent() const
{
    assert(mType != Type_Background);
    Size result(getRenderRect().size());
    if(Widget::SharedPtr widget = getWidget()) {
        if(result.width != 0 && result.height != 0) {
            if((widget->getWidth() != CoordinateMax && widget->getHeight() == CoordinateMax) || (widget->getWidth() == CoordinateMax && widget->getHeight() != CoordinateMax)) {
                const Image::AspectFitMode aspectFitMode = this->aspectFitMode();
                if(aspectFitMode != Image::Aspect_None) {
                    if(widget->getWidth() == CoordinateMax && (aspectFitMode & Image::Aspect_FitVertical)) {
                        const float ratio = static_cast<float>(widget->getHeight()-widget->getTopPadding() - widget->getBottomPadding()) / static_cast<float>(result.height);
                        result.width = (Coordinate)round(result.width * ratio);
                    } else if(widget->getHeight() == CoordinateMax && (aspectFitMode & Image::Aspect_FitHorizontal)) {
                        const float ratio = static_cast<float>(widget->getWidth()-widget->getTopPadding() - widget->getBottomPadding()) / static_cast<float>(result.width);
                        result.height = (Coordinate)round(result.height * ratio);
                    }
                }
            }
        }
    }
    result.width *= mScale;
    result.height *= mScale;
    return result;
}

void Image::sendImageLoaded(bool loaded)
{
    assert(MutexStack::locked(Widget::lock().name()));
    PERFORMANCE_MARKER_SCOPED("image.sendimageloaded");
    for(Image::ListenerContainer::iterator it = mListeners.begin(); it != mListeners.end(); ) {
        if(Listener::SharedPtr listener = it->lock()) {
            listener->onImageLoaded(shared_from_this(), loaded);
            ++it;
        } else {
            mListeners.erase(it++);
        }
    }
}

void Image::addListener(const Image::Listener::SharedPtr &listener)
{
    assert(MutexStack::locked(Widget::lock().name()));
    mListeners.insert(listener);
}

void Image::removeListener(const Image::Listener::SharedPtr &listener)
{
    assert(MutexStack::locked(Widget::lock().name()));
    mListeners.erase(listener);
}


std::string Image::describe() const
{
    std::ostringstream stream;
    if(getType() == Image::Type_Background)
        stream << "bgImg";
    else
        stream << "img";
    stream << ":" << this << ":";
    if(loadFlags() & SurfaceLoader::Load_LazyDownload)
        stream << "lazydownload:";
    if(loadFlags() & SurfaceLoader::Load_LazyDecode)
        stream << "lazydecode:";
    if(!(loadFlags() & SurfaceLoader::Load_Required))
        stream << "notrequired:";
    if(loadFlags() & SurfaceLoader::Load_AcceleratedOnly)
        stream << "fast:";
    if(!gibbon_float_compare(scale(), gibbonApp()->getScreen()->getCoordinateScale()))
        stream << "scale[" << scale() << "]:";
    if(isLoading())
        stream << "loading:";
    if(isRunning())
        stream << "running:";
    if(!getVisible())
        stream << "hidden:";
    if(hasFailed())
        stream << "failed:";
    if(!canPurge())
        stream << "nopurge:";
    if(uint32_t h = getHorizontalAlignment())
        stream << "h[0x" << std::hex << h << "]:" << std::dec;
    if(uint32_t v = getVerticalAlignment())
        stream << "v[0x" << std::hex << v << "]:" << std::dec;
    if(getSourceRect().x || getSourceRect().y ||
       getSourceRect().width != CoordinateMax || getSourceRect().height != CoordinateMax)
        stream << prettyRect(getSourceRect()) << ":";
    if(mSurface)
        stream << "(" << mSurface->describe() << ") ";
    else
        stream << "nosurface" << "(" << getUrl() << ") ";
    return stream.str();
}
