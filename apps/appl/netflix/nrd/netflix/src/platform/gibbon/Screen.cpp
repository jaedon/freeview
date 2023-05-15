/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Screen.h"
#include "Debug.h"
#include "Animation.h"
#include "SurfaceCache.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"

#include <nrd/NrdConfiguration.h>
#include <nrd/AppThread.h>
#include <nrd/AppLog.h>

#include <nrdbase/Instrumentation.h>
#include <nrdbase/Log.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>

#include <algorithm>
#include <queue>

#ifdef DEBUG_SCREEN_RENDER
# undef DEBUG_SCREEN_RENDER
# define DEBUG_SCREEN_RENDER(fmt, ...) if(Debug::DebugScreenRender) Log::error(TRACE_GIBBON_GRAPHICS, fmt, __VA_ARGS__)
#else
# define DEBUG_SCREEN_RENDER(fmt, ...)
#endif

using namespace netflix;
using namespace netflix::gibbon;

Mutex Screen::sLock(GIBBON_SCREEN_MUTEX, "GibbonScreen");
static int sScreenRenders = 0;

#if defined(GIBBON_SCREEN_FLIPTHREAD)
class FlipThread : public Thread
{
    struct FlipRequest {
        FlipRequest(Surface::SharedPtr _surface, const Rect &_rect) : surface(_surface), rect(_rect) { }
        Surface::SharedPtr surface;
        Rect rect;
    };

public:
    FlipThread(Surface::SharedPtr screenSurface) : Thread(&THREAD_GIBBON_FLIP), mDone(false),
                                                   mMutex(GIBBON_FLIP_MUTEX, "GibbonFlipThread"),
                                                   mScreenSurface(screenSurface)
    {
        for(int i = 0; i < 2; ++i) {
            Surface::SharedPtr surface(GibbonApplication::instance()->getSurfaceCache()->requestSurface(screenSurface->getWidth(),
                                                                                                        screenSurface->getHeight(),
                                                                                                        Surface::Flags_Alpha|Surface::Flags_RenderTarget));
            char buffer[2048];
            sprintf(buffer, "FlipSurface_%d", i);
            surface->setDescription(buffer);
            mSurfaces.push(surface);
        }
    }
    ~FlipThread() { }

    void finish() {
        mDone = true;
        mWakeup.signal();
    }

    Surface::SharedPtr surface() {
        ScopedMutex _lock(mMutex);
        if(mSurfaces.empty()) //wait for it...
            mFlip.wait(&mMutex);
        Surface::SharedPtr surface = mSurfaces.front();
        mSurfaces.pop();
        return surface;
    }
    void flip(Surface::SharedPtr surface, const Rect &rect) {
        ScopedMutex _lock(mMutex);
        mPendingFlip.push(FlipRequest(surface, rect));
        mWakeup.signal();
    }

protected:
    void Run() {
        {
            ScopedMutex _lock(mMutex);
            mDone = false;
        }
        while(1) {
            ScopedMutex _lock(mMutex);
            while(!mPendingFlip.empty() && !mDone) {
                FlipRequest request = mPendingFlip.front();
                mPendingFlip.pop();
                {
                    GraphicsEngine engine(mScreenSurface);
                    engine.clear(request.rect);
                    engine.blit(request.surface, request.rect, request.rect);
                    engine.end();
                    GibbonApplication::instance()->getScreen()->flip(request.rect);
                }
                mSurfaces.push(request.surface);
                mFlip.signal();
            }
            if(mDone)
                break;
            mWakeup.wait(&mMutex);
        }
        mScreenSurface.reset();
        while(!mPendingFlip.empty())
            mPendingFlip.pop();
        while(!mSurfaces.empty())
            mSurfaces.pop();
    }

    bool mDone;
    Mutex mMutex;
    ConditionVariable mFlip, mWakeup;
    Surface::SharedPtr mScreenSurface;
    std::queue<FlipRequest> mPendingFlip;
    std::queue<Surface::SharedPtr> mSurfaces;
};
static FlipThread *sFlipThread = 0;
#endif

namespace netflix {
namespace gibbon {

class FrameGraph
{
public:
    inline FrameGraph() : mLastFrameTime(0), mLastFlipTime(0), mLastAnimLoopTime(0), mAnimationTime(0), mLastAnimationTime(0), mOffset(0) { }

    inline int border() const { return 20; }
    inline Rect rect() const {
        if(mSurface) {
            const int b = border();
            return Rect(b, GibbonConfiguration::uiHeight() - mSurface->getHeight() - b, mSurface->getSize());
        }
        return Rect();
    }
    inline Surface::SharedPtr surface() const { return mSurface; }
    inline void flip(Stopwatch::ms t) { mLastFlipTime = t; }
    inline void animationLoop(Stopwatch::ms t) { mLastAnimLoopTime = t; }
    inline void addAnimationTime(Stopwatch::ms t) { mAnimationTime = t; }
    inline Surface::SharedPtr update(Stopwatch::ms frameTime) {
        const unsigned upperBound = 50;
        const Coordinate height = GibbonConfiguration::uiHeight()*.25f;
        if(!mSurface) {
            SurfaceCache::SharedPtr cache = GibbonApplication::instance()->getSurfaceCache();
            mSurface = cache->requestSurface(GibbonConfiguration::uiWidth()-(border()*2), height,
                                             Surface::Flags_Accelerated|Surface::Flags_RenderTarget);
            mSurface->setDescription("Screen::FrameGraph");
            {
                GraphicsEngine engine(mSurface);
                engine.clear(mSurface->getRect());
            }
        }
        {
            GraphicsEngine engine(mSurface);
            const unsigned lastOffset = mOffset;
            if(++mOffset >= unsigned(mSurface->getWidth()))
                mOffset = 0;
            static float desiredFrameTime_height = (((1.0f / gibbonApp()->getScreen()->getTargetFPS()) * 1000.0f) / upperBound) * height;
            if(mLastFlipTime) {
                const Coordinate lastFlipTime_height = (mLastFlipTime / upperBound) * height;
                const Coordinate lastFrameTime_height = ((mLastFrameTime + mLastAnimationTime) / upperBound) * height;
                engine.fill(Rect(lastOffset, mSurface->getHeight()-lastFrameTime_height-lastFlipTime_height,
                                 1, lastFlipTime_height), Color(255, 20, 255));
                engine.fill(Rect(lastOffset, mSurface->getHeight()-desiredFrameTime_height, 1, 1), Color(0, 0, 255));
            }
            if(mLastAnimLoopTime) {
                const Coordinate lastAnimLoopTime_height = (mLastAnimLoopTime / upperBound) * height;
                engine.fill(Rect(lastOffset, mSurface->getHeight()-lastAnimLoopTime_height,
                                 1, 1), Color(0, 0, 255));
            }
            engine.fill(Rect(lastOffset, 0, 1, mSurface->getHeight()), Color(0, 0, 0, 15));
            const Coordinate frameTime_height = (frameTime / upperBound) * height;
            const Coordinate animTime_height = (mAnimationTime / upperBound) * height;
            engine.fill(Rect(mOffset, 0, 1, mSurface->getHeight()-frameTime_height - animTime_height), Color(192, 192, 192));
            engine.fill(Rect(mOffset, mSurface->getHeight()-frameTime_height, 1, frameTime_height), Color(20, 255, 20));
            engine.fill(Rect(mOffset, mSurface->getHeight()-frameTime_height - animTime_height, 1, animTime_height), Color(255, 255, 255));
            engine.fill(Rect(mOffset, mSurface->getHeight()-desiredFrameTime_height, 1, 1), Color(0, 0, 255));
        }
        mLastAnimLoopTime = mLastFlipTime = 0;
        mLastFrameTime = frameTime;
        mLastAnimationTime = mAnimationTime;
        mAnimationTime = 0;
        return mSurface;
    }

protected:
    Stopwatch::ms mLastFrameTime, mLastFlipTime, mLastAnimLoopTime, mAnimationTime, mLastAnimationTime;
    unsigned mOffset;
    Surface::SharedPtr mSurface;
};

}} // netflix::applicaton::gibbon

class ScreenRenderEvent;
static shared_ptr<ScreenRenderEvent> sRenderEvent;
class ScreenRenderEvent : public GibbonApplication::Event
{
public:
    std::string describe() const { return std::string("ScreenRenderEvent"); }
    void eventFired()
    {
        PERFORMANCE_MARKER_SCOPED("screenrenderevent.fired");
        GibbonApplication::instance()->getScreen()->render();
    }
    void eventCanceled()
    {
        ScopedMutex _lock(Screen::lock());
        sRenderEvent.reset();
    }
};

Screen::Screen() : mRunningMutex(ZERO_MUTEX, "GibbonScreen::Running")
{
    mFlipSleep = 0;
    mRunning = false;
    mSplashScreen = false;
    mNeedsRestart = true;
    mCoordinateScale = 1.0f;
    mVSyncLast = Stopwatch::mono();
    mEndian = Screen::Endian_Default;
    for(int i = 0; i < 3; ++i)
        mFormats[i] = Surface::Format_None;
    setFormat(SurfaceFormat_Opaque, GibbonConfiguration::opaqueSurfaceFormat());
    setFormat(SurfaceFormat_Alpha, GibbonConfiguration::alphaSurfaceFormat());
    setFormat(SurfaceFormat_Text, GibbonConfiguration::textSurfaceFormat());
    setTargetFPS(GibbonConfiguration::fpsTarget());
}

Screen::~Screen()
{
    {
        ScopedMutex _lock(Screen::lock());
        sRenderEvent.reset();
        assert(!mOverlay);
        assert(!mRoot);
    }
    setShowSplashScreen(false);
}

void Screen::cleanupScene()
{
    ScopedMutex _widgetLock(Widget::lock());
    Widget::SharedPtr root, overlay;
    {
        ScopedMutex _screenLock(Screen::lock());
        setShowSplashScreen(false);
        std::swap(root, mRoot);
        std::swap(overlay, mOverlay);
        mRootDisplayList.reset();
    }
}

static struct {
    const char *name;
    Surface::Format format;
} sFormatMap[] = {
    { "RGBA32", Surface::Format_RGBA32, },
    { "RGBA32_PM", Surface::Format_RGBA32_Premultiplied, },
    { "BGRA32", Surface::Format_BGRA32, },
    { "BGRA32_PM", Surface::Format_BGRA32_Premultiplied, },
    { "ABGR32", Surface::Format_ABGR32 },
    { "ABGR32_PM", Surface::Format_ABGR32_Premultiplied },
    { "RGB32", Surface::Format_RGB32, },
    { "BGR32", Surface::Format_BGR32, },
    { "RGB24", Surface::Format_RGB24, },
    { "BGR24", Surface::Format_BGR24, },
    { "A8", Surface::Format_AlphaOnly, },
    { 0, Surface::Format_None },
};

static struct {
    const char *name;
    Screen::EndianType type;
} sEndianMap[] = {
    { "default", Screen::Endian_Default },
    { "little", Screen::Endian_Little },
    { "big", Screen::Endian_Big },
    { 0, Screen::Endian_Default }
};

#ifdef DEBUG_SCREEN_IDLE
class ScreenIdleTimer : public GibbonApplication::Timer
{
public:
    ScreenIdleTimer() : GibbonApplication::Timer(10000) {}
    ~ScreenIdleTimer() {}

    static void poke() {
        if(!sTimer)
            sTimer.reset(new ScreenIdleTimer);
        sTimer->mPending = true;
        sTimer->mTime = GibbonApplication::instance()->timeSinceLocationChanged();
        sTimer->restart();
    }

    virtual std::string describe() const { return "ScreenIdleTimer"; }
    virtual void timerFired() { if(sTimer.get() == this) dump_internal(); }

    static void dump() { if(sTimer) sTimer->dump_internal(); }
protected:
    void dump_internal()
    {
        if(mPending) {
            mPending = false;
            stop();
            Log::error(TRACE_GIBBON_GRAPHICS, "SCREEN_IDLE: %lld", mTime.ms());
        }
    }

    Time mTime;
    bool mPending;
    static shared_ptr<ScreenIdleTimer> sTimer;
};
shared_ptr<ScreenIdleTimer> ScreenIdleTimer::sTimer;
void Screen::dumpScreenIdleTimer() { ScreenIdleTimer::dump(); }
#endif

std::string Screen::describeFormat(Surface::Format format)
{
    for(int i = 0; sFormatMap[i].name; ++i) {
        if(sFormatMap[i].format == format)
            return sFormatMap[i].name;
    }
    return "None";
}

std::vector<std::string> Screen::formats()
{
    std::vector<std::string> names;
    for (int i = 0; sFormatMap[i].name; ++i)
        names.push_back(sFormatMap[i].name);
    return names;
}

Surface::Format Screen::getFormat(const std::string &format)
{
    for(int i = 0; sFormatMap[i].name; ++i) {
        if(format == sFormatMap[i].name)
            return sFormatMap[i].format;
    }
    return Surface::Format_None;
}

void Screen::setFormat(SurfaceFormatType type, const std::string &format)
{
    if(const Surface::Format found = getFormat(format))
        setFormat(type, found);
    else
        Log::error(TRACE_GIBBON_GRAPHICS, "Invalid %s format %s", type == SurfaceFormat_Alpha ? "alpha" : "opaque", format.c_str());
}

std::string Screen::describeEndian(EndianType type)
{
    for (int i = 0; sEndianMap[i].name; ++i) {
        if(sEndianMap[i].type == type)
            return sEndianMap[i].name;
    }
    assert("Unknown endian format" && 0);
    return "unknown";
}

void Screen::setEndian(const std::string& type)
{
    EndianType found = Endian_Default;
    for (int i = 0; sEndianMap[i].name; ++i) {
        if(type == sEndianMap[i].name) {
            found = sEndianMap[i].type;
            break;
        }
    }
    setEndian(found);
}

void Screen::update(const Rect &_rect, Widget::SharedPtr widget)
{
    if(GibbonApplication::instance()->isSuspended())
        return;

    Rect rect = _rect;
    if(Debug::TestScreenCoordinateScale) {
        rect.x *= mCoordinateScale;
        rect.y *= mCoordinateScale;
        rect.width *= mCoordinateScale;
        rect.height *= mCoordinateScale;
    }
    {
        ScopedMutex _lock(Screen::lock());

        mDirty.unite(rect);
#if defined(GIBBON_NO_THREADANIMATION)
        handleUpdate(rect);
#endif
    }
    DEBUG_SCREEN_RENDER("[%s:%d]: UPDATE(%d) %s %s", __func__, __LINE__, sScreenRenders,
                        rect.toString().c_str(), widget ? widget->describe().c_str() : "(N/A)");
#if !defined(GIBBON_NO_THREADANIMATION)
    Animation::update(rect);
#endif
    NRDP_UNUSED(widget);
}

bool Screen::render()
{
    if(!isRunning()) {
        vsync();
        return false;
    }

    PERFORMANCE_MARKER_SCOPED("screen.render");
    INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.render");

    const Stopwatch renderTime;
    Rect dirtyRect, updateRect;
    bool scale = false, hideSplash = false;
    GraphicsEngine graphics;
    Surface::SharedPtr surface;
    DisplayList::SharedPtr displayList;
    if(GibbonConfiguration::screenRenderDisplayList())
        displayList.reset(new DisplayList(getSize()));
    {
        PERFORMANCE_MARKER_SCOPED("screen.render.prep");

        static const Variant rootV(Variant::StringMap("root",true));
        static const Variant overlayV(Variant::StringMap("overlay",true));

        ScopedMutex _widgetLock(Widget::lock());
        Rect prerender;
        if(mRoot) {
            PERFORMANCE_MARKER_SCOPED("screen.render.root.prerender");
            const Rect p = mRoot->prerender();
            if(!p.empty()) {
                prerender |= p;
                INSTRUMENTATION_INTERVAL_SWITCHED_TAG("gibbon.render", rootV);
            }
        }
        if(mOverlay) {
            PERFORMANCE_MARKER_SCOPED("screen.render.overlay.prerender");
            const Rect p = mOverlay->prerender();
            if(!p.empty()) {
                prerender |= p;
                INSTRUMENTATION_INTERVAL_SWITCHED_TAG("gibbon.render", overlayV);
            }
        }
        dirtyRect |= prerender;

        INSTRUMENTATION_INTERVAL_SWITCHED_MARK("gibbon.render" , "pre");

        Widget::SharedPtr root, overlay;
        DisplayList::SharedPtr rootDisplayList;
        {
            PERFORMANCE_MARKER_SCOPED("screen.render.root.prep");
            const Rect rootRect = getRootRect();
            ScopedMutex _lock(Screen::lock());

#if defined(GIBBON_NO_THREADANIMATION)
            sRenderEvent.reset();
#endif

            if(Debug::TestAnimationNoIdle || Debug::TestScreenFullRender)
                dirtyRect = rootRect;
            else
                dirtyRect |= mDirty & rootRect;
            mDirty.clear();
            if(dirtyRect.empty()) {
                INSTRUMENTATION_INTERVAL_SWITCHED_CANCEL("gibbon.render");
                DEBUG_SCREEN_RENDER("[%s:%d]: RENDER(%d) NO-OP %s %s", __func__, __LINE__, sScreenRenders, dirtyRect.toString().c_str(), prerender.toString().c_str());
                ++sScreenRenders;
                vsync();
                return true; // Nothing changed this frame, but it still counts as a render (for performance tracking)
            }
#if defined(GIBBON_SCREEN_FULLRENDER)
            updateRect = rootRect;
#else
            if(Debug::TestScreenFullRender || displayList)
                updateRect = rootRect;
            else
                updateRect = dirtyRect.intersected(rootRect);
#endif
            if(Debug::DrawScreenFrameGraph) {
                if(!mFrameGraph)
                    mFrameGraph.reset(new FrameGraph);
                updateRect |= mFrameGraph->rect();
            } else {
                mFrameGraph.reset();
            }

            INSTRUMENTATION_INTERVAL_SWITCHED_TAG("gibbon.render", Variant::StringMap("dirtyRect",Variant::Array(dirtyRect.x)(dirtyRect.y)(dirtyRect.width)(dirtyRect.height))
                                                  ("updateRect",Variant::Array(updateRect.x)(updateRect.y)(updateRect.width)(updateRect.height)));

#if (defined(DEBUG_ANIMATION_FPS) && (DEBUG_ANIMATION_FPS > 1))
            Log::info(TRACE_GIBBON_ANIMATION) << "********** Render:screen: "
                                              << updateRect.x << " " << updateRect.y << " " << updateRect.width << " " << updateRect.height
                                              << " [" <<  sScreenRenders << "]";
#endif
            if(mRootDisplayList)
                rootDisplayList = mRootDisplayList;
            else
                root = mRoot;
            hideSplash = (rootDisplayList || (root && root->getVisible() && root->hasContent())) && mSplashScreen;
            overlay = mOverlay;
            if(mScaledSurface)
                scale = true;
#if defined(GIBBON_SCREEN_FLIPTHREAD)
            surface = sFlipThread->surface();
#else
            surface = mSurface;
#endif
        }

        if(displayList)
            graphics.begin(displayList);
        else if(scale)
            graphics.begin(mScaledSurface);
        else
            graphics.begin(surface);
        graphics.setClip(updateRect);

        bool opaque = (rootDisplayList || (root && root->hasOpaqueContent()));
        if(!opaque && overlay) {
            if(overlay->hasOpaqueContent()) {
                opaque = true;
            } else {
                const Widget::ChildContainer &overlays = overlay->getChildren();
                for(Widget::ChildContainer::const_iterator it = overlays.begin(); it != overlays.end(); ++it) {
                    if((*it)->hasOpaqueContent()) {
                        opaque = true;
                        break;
                    }
                }
            }
        }
        if(!opaque)
            graphics.clear(updateRect);

        const bool rootHasContent = root ? root->hasContent() : false,
                overlayHasContent = overlay ? overlay->hasContent() : false;
        if(rootDisplayList || rootHasContent || overlayHasContent) {
            uint8_t renderFlags = 0;
#if 0
            renderFlags |= Widget::Render_Comment;
#endif
            if(Debug::InstrumentWidgetRender) {
                graphics.flush();
                renderFlags |= Widget::Render_Comment;
                graphics.setInstrument(true);
            }
            if(rootDisplayList) {
                PERFORMANCE_MARKER_SCOPED("screen.render.displayList.apply");
                graphics.apply(rootDisplayList, rootDisplayList->getRect(), rootDisplayList->getRect());
            }
            if(rootHasContent) {
                PERFORMANCE_MARKER_SCOPED("screen.render.root.render");
                root->render(&graphics, Point(0, 0), updateRect, renderFlags);
            }
            if(overlayHasContent) {
                PERFORMANCE_MARKER_SCOPED("screen.render.overlay.render");
                overlay->render(&graphics, Point(0, 0), updateRect, renderFlags);
            }
            if(Debug::InstrumentWidgetRender)
                graphics.setInstrument(false);
        }

        GibbonApplication::instance()->getSurfaceCache()->startDecodeJobs();
#ifdef DEBUG_SCREEN_IDLE
        if(Debug::DebugScreenIdle && (rootHasContent || overlayHasContent))
            ScreenIdleTimer::poke();
#endif
        DEBUG_SCREEN_RENDER("[%s:%d]: RENDER(%d) %s (%s) [%d]", __func__, __LINE__, sScreenRenders, updateRect.toString().c_str(), prerender.toString().c_str(), opaque);
        if(Debug::DrawScreenRectangles) {
            graphics.setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
            static const char *colors[] = { "maroon", "red", "orange", "yellow", "olive", "purple", "fuchsia", "white", "lime", 0 };
            static int color = 0;
            ++color;
            if(!colors[color])
                color = 0;
            Color c(colors[color]);
            c.a = 100;
            graphics.fill(updateRect, c);
            c.a = 255;
            graphics.drawRect(updateRect, c);
            {
                Style::SharedPtr style(new Style);
                style->setFontFamily("Arial_for_Netflix");
                style->setFontPixelSize(18);
                style->setColor(Color(255, 255, 255, 255));
                char buffer[1024];
                sprintf(buffer, "r:%d:%s", sScreenRenders,updateRect.toString().c_str());
                graphics.drawText(buffer, updateRect, updateRect, style);
            }
        }
        if(mFrameGraph) {
            if(Debug::TestScreenFlush)
                graphics.flush();
            if(Surface::SharedPtr surface = mFrameGraph->update(renderTime.elapsed())) {
                const Rect graphArea(mFrameGraph->rect());
                graphics.setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);
                graphics.setOpacity(0.75);
                graphics.blit(mFrameGraph->surface(), surface->getRect(), graphArea);
            }
        }
        ++sScreenRenders;
    }
    if(!updateRect.empty()) {
        if(GibbonConfiguration::showFps()) {
            Style::SharedPtr style(new Style);
            style->setFontFamily("Arial_for_Netflix");
            style->setFontPixelSize(18);
            style->setColor(Color(255, 255, 255, 255));
            char buffer[1024];
            sprintf(buffer, "FPS:%d", GibbonApplication::instance()->getFps());
            const Rect textRect(0, 0, CoordinateMax, CoordinateMax);
            graphics.drawText(buffer, textRect, textRect, style);
        }
        {
            ScopedMutex _lock(Screen::lock());
            std::vector<Listener::SharedPtr> listeners;
            mListeners.lock(listeners);
            for(std::vector<Listener::SharedPtr>::const_iterator it = listeners.begin(); it != listeners.end(); ++it)
                (*it)->onScreenRendered(&graphics, dirtyRect);
        }
    }
    graphics.end();

    if(scale) {
        graphics.end();
        ScopedMutex _lock(Screen::lock());
        assert(mScaledSurface);
        graphics.begin(surface);
        graphics.setCompositionMode(GraphicsEngine::CompositionMode_Source);
        graphics.blit(mScaledSurface, mScaledSurface->getRect(), mSurface->getRect());
        updateRect = mSurface->getRect();
    }

    if(!updateRect.empty()) {
        const Stopwatch flipTime;
        PERFORMANCE_MARKER_START("screen.render.flip");
        if(displayList) {
            flip(displayList, updateRect);
        } else {
#if defined(GIBBON_SCREEN_FLIPTHREAD)
            sFlipThread->flip(surface, updateRect);
#else
            flip(updateRect);
#endif
        }
        PERFORMANCE_MARKER_END();
        if(mFrameGraph)
            mFrameGraph->flip(flipTime.elapsed());
#if defined(DEBUG_ANIMATION_FPS) && (DEBUG_ANIMATION_FPS > 1)
        Log::info(TRACE_GIBBON_ANIMATION, "********** ~Render:screen: %f", renderTime.elapsed());
#endif
        if(mFlipSleep)
            usleep(mFlipSleep*1000);
        if(hideSplash)
            setShowSplashScreen(false);
        GibbonApplication::instance()->getSurfaceCache()->touch();
        if(mFrameGraph) {
            static Stopwatch::ms before = Stopwatch::mono();
            Stopwatch::ms now = Stopwatch::mono();
            mFrameGraph->animationLoop(now-before);
            before = now;
        }
    }

    INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.render");
    return true; // count this frame in performance tracking
}

Surface::SharedPtr Screen::grab()
{
    assert(MutexStack::locked(Widget::lock().name()));

    Rect screenRect;
    Widget::SharedPtr root, overlay;
    {
        ScopedMutex _lock(Screen::lock());
        if(!isRunning())
            return Surface::SharedPtr();
        screenRect = getRect();
        root = mRoot;
        overlay = mOverlay;
    }
    Surface::SharedPtr surface;
    if(root || overlay) {
        surface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(screenRect.width, screenRect.height,
                                                                                   Surface::Flags_Alpha|Surface::Flags_RenderTarget);
        if(surface) {
            if(overlay) {
                overlay->prerender();
                GraphicsEngine engine(surface);
                engine.clear(Rect(0, 0, screenRect.width, screenRect.height));
                overlay->render(&engine, Point(0, 0), Rect(0, 0, screenRect.width, screenRect.height));
            }
            if(root) {
                root->prerender();
                GraphicsEngine engine(surface);
                engine.clear(Rect(0, 0, screenRect.width, screenRect.height));
                root->render(&engine, Point(0, 0), Rect(0, 0, screenRect.width, screenRect.height));
            }
        }
    }
    return surface;
}

std::list<Widget::SharedPtr> Screen::widgetsAt(const Point &pos) const
{
    Widget::SharedPtr root, overlay;
    {
        ScopedMutex _screenLock(Screen::lock());
        overlay = mOverlay;
        root = mRoot;
    }

    ScopedMutex _widgetLock(Widget::lock());
    std::list<Widget::SharedPtr> result;
#if 0
    if(overlay)
        result += overlay->widgetsAt(pos);
#endif
    if(root)
        result = root->widgetsAt(pos);
    return result;
}

DisplayList::SharedPtr Screen::rootDisplayList() const
{
    ScopedMutex _lock(Screen::lock());
    return mRootDisplayList;
}

void Screen::setRootDisplayList(const DisplayList::SharedPtr &displayList, const Rect &updateRect)
{
    {
        ScopedMutex _widgetLock(Widget::lock());
        ScopedMutex _screenLock(Screen::lock());
        if(mRootDisplayList == displayList)
            return;
        mRootDisplayList = displayList;
        if(!mRootDisplayList && GibbonApplication::instance()->status() != GibbonApplication::Status_Stopping)
            setShowSplashScreen(true);
    }
    update(updateRect.empty() ? getRect() : updateRect);
}

void Screen::setRoot(Widget::SharedPtr root)
{
    Widget::SharedPtr oldRoot;
    {
        ScopedMutex _widgetLock(Widget::lock());
        ScopedMutex _screenLock(Screen::lock());
        if(mRoot == root)
            return;
        oldRoot = mRoot;
        mRoot = root;
        if(!mRoot && GibbonApplication::instance()->status() != GibbonApplication::Status_Stopping)
            setShowSplashScreen(true);
    }
    if(root) {
        root->setRoot(true);
        root->needsRender();
    }
    if(oldRoot) {
        oldRoot->setRoot(false);
        oldRoot->purge(Widget::Purge_Force);
    }
    GibbonApplication::instance()->changedScene();
    update();
}

Widget::SharedPtr Screen::createOverlay(const Rect &rect, const Color &color)
{
    Widget::SharedPtr result;
    if(rect.empty()) {
        setOverlay(Widget::SharedPtr());
    } else {
        result.reset(new Widget);
        result->setName("Overlay:" + rect.toString());
        result->setRect(rect);
        result->setBackgroundColor(color.empty() ? Color(100, 100, 220, 100) : color);
        addOverlay(result);
    }
    return result;
}

void Screen::addOverlay(Widget::SharedPtr overlay)
{
    assert(MutexStack::locked(Widget::lock().name()));
    ScopedMutex _animation(Animation::lock());
    ScopedMutex _lock(Screen::lock());
    if(!mOverlay) {
        Widget::SharedPtr o(new Widget);
        o->init();
        o->setRect(getRect());
        setOverlay(o);
    }
    if(overlay)
        mOverlay->addChild(overlay);
    update();
}

void Screen::setOverlay(Widget::SharedPtr overlay)
{
    Widget::SharedPtr oldOverlay;
    {
        assert(MutexStack::locked(Widget::lock().name()));
        ScopedMutex _lock(Screen::lock());
        if(mOverlay == overlay)
            return;
        oldOverlay = mOverlay;
        mOverlay = overlay;
        if(mOverlay) {
            mOverlay->setRoot(true);
        }
    }

    if(overlay)
        overlay->needsRender();
    if(oldOverlay)
        oldOverlay->purge(Widget::Purge_Force);
    update(getRootRect());
}

void Screen::addAnimationTime(Stopwatch::ms t)
{
    assert(mFrameGraph);
    mFrameGraph->addAnimationTime(t);
}

void Screen::makeCurrent( bool current )
{
    ScopedMutex _lock(Screen::lock());
    makeCurrent_sys(current);
}

void Screen::stop()
{
    const bool wasRunning = isRunning();
    {
        ScopedMutex widgetLock(Widget::lock());
        ScopedMutex screenLock(Screen::lock());
#if defined(GIBBON_SCREEN_FLIPTHREAD)
        if(sFlipThread) {
            sFlipThread->finish();
            sFlipThread->Wait();
            delete sFlipThread;
            sFlipThread = 0;
        }
#endif
        cleanupSurface();
        GibbonFreetype::clearCache();
    }

    if(shared_ptr<SurfaceCache> cache = GibbonApplication::instance()->getSurfaceCache()) {
        cache->clear();
    }

    if(wasRunning) {
        ScopedMutex screenLock(Screen::lock());
        cleanup();
    }
}

void Screen::start()
{
    ScopedMutex screenLock(Screen::lock());
    init();
    mNeedsRestart = false;
    initSurface();
#if defined(GIBBON_SCREEN_FLIPTHREAD)
    if(!sFlipThread) {
        sFlipThread = new FlipThread(mSurface);
        sFlipThread->Start();
    }
#endif

    static bool first = true;
    if(first) {
        first = false;
        Variant graphicsReport = graphicsInfo();
        graphicsReport["coordinateScale"] = mCoordinateScale;
#if defined(GIBBON_SCREEN_FLIPTHREAD)
        graphicsReport["flipThread"] = true;
#endif
#if defined(GIBBON_SCREEN_FLIPVSYNC)
        graphicsReport["vsync"] = true;
#endif
#if defined(GIBBON_SCREEN_FULLRENDER)
        graphicsReport["fullScreen"] = true;
#endif
        graphicsReport["endian"] = Screen::describeEndian(getEndian());
        graphicsReport["ui"]["width"] = GibbonConfiguration::uiWidth();
        graphicsReport["ui"]["height"] = GibbonConfiguration::uiHeight();
        graphicsReport["screen"]["width"] = GibbonConfiguration::screenWidth();
        graphicsReport["screen"]["height"] = GibbonConfiguration::screenHeight();
        graphicsReport["memory"]["capacity"] = gibbonApp()->surfaceCacheCapacity();
        graphicsReport["memory"]["playbackCapacity"] = gibbonApp()->surfaceCachePlaybackCapacity();
        {
            Log::Message m(TRACE_GIBBON_GRAPHICS, Log::Info, "graphics");
            m.m_message = graphicsReport.toJSON(true);
            m.send();
        }
        gibbonApp()->addLibraryInfo(graphicsReport["libraries"]);
    }
}

void Screen::restart()
{
    assert(MutexStack::locked(Widget::lock().name()));
    {
        ScopedMutex screenLock(Screen::lock());
        stop();
        start();
    }
    setRoot(Widget::SharedPtr());
    update();
}

void Screen::initSurface()
{
    assert(MutexStack::locked(Screen::lock().name()));
    const int width = GibbonConfiguration::screenWidth(), height = GibbonConfiguration::screenHeight();
    if(!mSurface || width != mSurface->getWidth() || height != mSurface->getHeight()) {
        mSurface.reset(new Surface(width, height, Surface::Flags_Screen|Surface::Flags_Alpha|Surface::Flags_RenderTarget));
        mSurface->setDescription("Screen");
    }
    mCoordinateScale = 1.0f;
    GraphicsEngine::setDefaultSmoothScale(false);
    const int ui_width = GibbonConfiguration::uiWidth(), ui_height = GibbonConfiguration::uiHeight();
    if(mVirtualSize.width != ui_width || mVirtualSize.height != ui_height) {
        mVirtualSize.set(ui_width, ui_height);
        if(mVirtualSize.width != width || mVirtualSize.height != height) { //scale
            if(Debug::TestScreenCoordinateScale && gibbon_float_compare(float(width) / ui_width, float(height) / ui_height)) {
                GraphicsEngine::setDefaultSmoothScale(true);
                mCoordinateScale = float(width) / ui_width;
            } else if(!canScaleDirectly()) {
                mScaledSurface.reset(new Surface(mVirtualSize.width, mVirtualSize.height, Surface::Flags_Alpha|Surface::Flags_RenderTarget));
                mScaledSurface->setDescription("ScaledScreen");
            }
        }
    }
    {
        ScopedMutex _lock(mRunningMutex);
        mRunning = true;
    }
}

void Screen::cleanupSurface()
{
    assert(MutexStack::locked(Screen::lock().name()));
    {
        ScopedMutex _lock(mRunningMutex);
        mRunning = false;
    }
    if(mSurface) {
        const Rect r = mSurface->getRect();
        for(int i = 0; i < 2; ++i) { //clear front and back buffer
            GraphicsEngine engine(mSurface);
            engine.clear(r);
            engine.end();
            flip(r);
        }
    }
    mSurface.reset();
    mFrameGraph.reset();
    mVirtualSize.clear();
    mScaledSurface.reset();
}

void Screen::setShowSplashScreen(bool show)
{
    {
        ScopedMutex _lock(Screen::lock());
        if(mSplashScreen == show)
            return;
        mSplashScreen = show;
    }
    const std::vector<Listener::SharedPtr> l = listeners();
    for(std::vector<Listener::SharedPtr>::const_iterator it = l.begin(); it != l.end(); ++it)
        (*it)->onScreenShowSplash(show);
    gibbon_oem_set_splash(show);
}

void Screen::sendRenderEvent()
{
    shared_ptr<ScreenRenderEvent> event;
    {
        ScopedMutex _lock(Screen::lock());
        if(!sRenderEvent) {
            sRenderEvent.reset(new ScreenRenderEvent);
            event = sRenderEvent;
        }
    }
    if(event)
        GibbonApplication::instance()->postEvent(event);
}

Widget::SharedPtr Screen::root() const
{
    ScopedMutex _lock(Screen::lock());
    return mRoot;
}

Widget::SharedPtr Screen::overlay() const
{
    ScopedMutex _lock(Screen::lock());
    return mOverlay;
}

llong Screen::getGraphicsMemoryCapacity() const
{
    llong result = GibbonConfiguration::surfaceCacheCapacity();
    if(result == -1)
        result = Surface::getGraphicsMemoryCapacity();
    if(result == -1) //hardcoded default
        result = 28*1024*1024;
    return result;
}

void Screen::addListener(const Listener::SharedPtr &listener)
{
    ScopedMutex _lock(Screen::lock());
    mListeners.insert(listener);
}

void Screen::removeListener(const Listener::SharedPtr &listener)
{
    ScopedMutex _lock(Screen::lock());
    mListeners.erase(listener);
}

EffectRenderers Screen::createEffectRenderers_internal(const Effects &effects)
{
    EffectRenderers result;
    for(Effects::const_iterator it = effects.begin(); it != effects.end(); ++it) {
        Effect::SharedPtr effect = (*it);
        if(effect->getVisible())
            result.push_back(effect->getDefaultRenderer());
    }
    return result;
}

void Screen::vsync()
{
    vsync_sys();
    mVSyncLast = Stopwatch::mono();
}

void Screen::vsync_internal()
{
    const Stopwatch::ms remainingTime = Stopwatch::mono()-mVSyncLast;
    if(remainingTime < mVsyncInterval) {
        const int usleepTime = (int)floor((mVsyncInterval-remainingTime) * 1000);
        usleep(usleepTime);
    }
}

void Screen::flip(const Rect &rect)
{
    flip_sys(rect);
#if defined(GIBBON_SCREEN_FLIPVSYNC)
    mVSyncLast = Stopwatch::mono();
#else
    vsync();
#endif
    gibbon_oem_event(GibbonOEM_Rendered);
}

void Screen::flip(const DisplayList::SharedPtr &displayList, const Rect &rect)
{
    flip_sys(displayList, rect);
#if defined(GIBBON_SCREEN_FLIPVSYNC)
    mVSyncLast = Stopwatch::mono();
#else
    vsync();
#endif
    gibbon_oem_event(GibbonOEM_Rendered);
}

void Screen::flip_internal(const DisplayList::SharedPtr &displayList, const Rect &rect)
{
    Surface::SharedPtr surface;
    {
        ScopedMutex _lock(Screen::lock());
        surface = mSurface;
    }
    GraphicsEngine engine(surface);
    engine.apply(displayList, rect, rect);
    engine.end();
    flip(rect);
}
