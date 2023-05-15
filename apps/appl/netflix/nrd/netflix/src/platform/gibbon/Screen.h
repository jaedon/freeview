/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Rect.h>
#include <Surface.h>
#include <Widget.h>
#include <nrdbase/tr1.h>
#include <gibbon/config.h>
#include <GibbonConfiguration.h>

#if defined(GIBBON_GRAPHICSENGINE_GLES2)
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
namespace Graphics { struct Context; }
namespace nova { class Mutex; }
class Texture;
typedef struct RenderableInternal RenderableType;
#endif

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
#ifndef GIBBON_DFBSCREEN_NO_LAYER
# define GIBBON_DFBSCREEN_LAYER
#endif
#if !defined(GIBBON_DFBSCREEN_ID)
# define GIBBON_DFBSCREEN_ID DLID_PRIMARY
#endif
#define DIRECTFB_CHECK(statement) \
    { const DFBResult err = statement;           \
    if (err != DFB_OK) \
        Log::error(TRACE_GIBBON_GRAPHICS, "DFB:%d: Failed(%d): %s", __LINE__, err, #statement); }
#define DIRECTFB_OPERATOR_FLAGS(F)                                                        \
    inline F operator~(F f) { return F(~static_cast<int>(f)); }                           \
    inline F operator&(F l, F r) { return F(static_cast<int>(l) & static_cast<int>(r)); } \
    inline F operator|(F l, F r) { return F(static_cast<int>(l) | static_cast<int>(r)); } \
    inline F &operator|=(F &l, F r) { l = (l | r); return l; }                            \
    inline F &operator&=(F &l, F r) { l = (l & r); return l; }
DIRECTFB_OPERATOR_FLAGS(DFBInputDeviceCapabilities)
DIRECTFB_OPERATOR_FLAGS(DFBWindowDescriptionFlags)
DIRECTFB_OPERATOR_FLAGS(DFBWindowCapabilities)
DIRECTFB_OPERATOR_FLAGS(DFBWindowOptions)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceDescriptionFlags)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceCapabilities)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceLockFlags)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceBlittingFlags)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceDrawingFlags)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceFlipFlags)
DIRECTFB_OPERATOR_FLAGS(DFBSurfaceRenderOptions)
DIRECTFB_OPERATOR_FLAGS(DFBFontDescriptionFlags)
DIRECTFB_OPERATOR_FLAGS(DFBDisplayLayerConfigFlags)
#endif

#if defined(BUILD_DEBUG) || 1
# define DEBUG_SCREEN_RENDER
# define DEBUG_SCREEN_IDLE
#endif

namespace netflix {
namespace gibbon {

class FrameGraph;

class Screen
{
public:
    Screen();
    ~Screen();

    void cleanupScene();

    class Listener
    {
    public:
        typedef shared_ptr<Listener> SharedPtr;
        typedef weak_ptr<Listener> WeakPtr;
        Listener() {}
        virtual ~Listener() {}

        virtual void onScreenRendered(const GraphicsEngine */*engine*/, const Rect &/*rect*/) { }
        virtual void onScreenShowSplash(bool /*show*/) { }
    };
    void addListener(const Listener::SharedPtr &listener);
    void removeListener(const Listener::SharedPtr &listener);
    inline std::vector<Listener::SharedPtr> listeners() const {
        ScopedMutex _lock(lock());
        std::vector<Listener::SharedPtr> result;
        mListeners.lock(result);
        return result;
    }

    void restart();
    void start();
    void stop();
    inline bool needsRestart() const { return mNeedsRestart; }
    Variant graphicsInfo() const;

    typedef shared_ptr<Screen> SharedPtr;
    typedef weak_ptr<Screen> WeakPtr;

    inline static Mutex &lock() { return sLock; }

    inline Rect getRootRect() const {
        Rect result = getRect();
        if(shared_ptr<Widget> widget = root())
            result |= widget->getContentsRect();
        return result;
    }
    std::list<Widget::SharedPtr> widgetsAt(const Point &pos) const;
    Widget::SharedPtr root() const;
    void setRoot(Widget::SharedPtr root);
    void setRootDisplayList(const DisplayList::SharedPtr &displayList, const Rect &updateRect=Rect());
    DisplayList::SharedPtr rootDisplayList() const;

    void addOverlay(Widget::SharedPtr overlay);
    Widget::SharedPtr createOverlay(const Rect &rect, const Color &color=Color(0, 0, 0, 0));
    Widget::SharedPtr overlay() const;
    void setOverlay(Widget::SharedPtr overlay);

    inline bool hasFrameGraph() const { return static_cast<bool>(mFrameGraph); }

    void addAnimationTime(Stopwatch::ms t);

    Surface::SharedPtr grab();
    void setShowSplashScreen(bool show);

    llong getGraphicsMemoryCapacity() const;

    static Effect::SharedPtr createEffect(Effect::Type type);
#if defined(GIBBON_SURFACE_OPTIMIZED_RENDERERS)
    static EffectRenderers createEffectRenderers(const Effects &effects);
#else
    static EffectRenderers createEffectRenderers(const Effects &effects) { return createEffectRenderers_internal(effects); }
#endif

    inline float getCoordinateScale() const { return mCoordinateScale; }

    inline Rect getRect() const { return Rect(0, 0, GibbonConfiguration::uiWidth() * mCoordinateScale, GibbonConfiguration::uiHeight() * mCoordinateScale); }
    inline Size getSize() const { return Size(GibbonConfiguration::screenWidth(), GibbonConfiguration::screenHeight()); }
    inline Size getUISize() const { return Size(GibbonConfiguration::uiWidth(), GibbonConfiguration::uiHeight()); }

    inline void update(Widget::SharedPtr widget=Widget::SharedPtr()) { update(getRect(), widget); }
    void update(const Rect &rect, Widget::SharedPtr widget=Widget::SharedPtr());
    bool render();

    inline bool isRunning() const {
        ScopedMutex _lock(mRunningMutex);
        return mRunning;
    }
    inline Surface::SharedPtr getSurface() const { return mSurface; }

    enum SurfaceFormatType {
        SurfaceFormat_Opaque,
        SurfaceFormat_Alpha,
        SurfaceFormat_Text
    };
    static std::vector<std::string> formats();
    static std::string describeFormat(Surface::Format format);
    static Surface::Format getFormat(const std::string &format);
    inline Surface::Format getFormat(SurfaceFormatType type) const { return mFormats[type]; }
    inline void setFormat(SurfaceFormatType type, Surface::Format format) { mNeedsRestart = true; mFormats[type] = format; }
#ifdef DEBUG_SCREEN_IDLE
    static void dumpScreenIdleTimer();
#endif
    void setFormat(SurfaceFormatType type, const std::string &format);

    enum EndianType {
        Endian_Default,
        Endian_Little,
        Endian_Big
    };
    static std::string describeEndian(EndianType type);
    inline EndianType getEndian() const { return mEndian;  }
    inline void setEndian(EndianType type) { mNeedsRestart = true; mEndian = type; }
    void setEndian(const std::string& type);

    inline int flipSleep() const { return mFlipSleep; }
    inline void setFlipSleep(int ms) { mFlipSleep = ms; }

    // Tell the screen that this thread will be the current rendering
    // thread (when current is true) or that this thread will no longer
    // render to it (when current is false). This is mostly to ensure that
    // the current context is valid for GL but it could have other uses.

    void makeCurrent(bool current);

    void flip(const Rect &rect);
    void flip(const DisplayList::SharedPtr &displayList, const Rect &rect);

    void vsync();
    inline Stopwatch::ms getVsyncInterval() const {
        ScopedMutex _lock(Screen::lock());
        return mVsyncInterval;
    }

    inline int getTargetFPS() const {
        ScopedMutex _lock(Screen::lock());
        return mVsyncInterval ? (int)(1000.0 / mVsyncInterval) : 0;
    }
    inline void setTargetFPS(int fps) {
        ScopedMutex _lock(Screen::lock());
        mVsyncInterval = fps ? (1000.0f / fps) : 0;
    }

#ifndef GIBBON_GRAPHICSENGINE_GLES2
    int glyphPadding() const { return 1; }
#else
    int glyphPadding() const;
#endif

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    IDirectFB *getDirectFB() const { return mDirectFB; }
    inline void setDirectFBWindow(IDirectFBWindow *window) { mDirectFBWindow = window; }
    struct Configuration {
        inline Configuration(int _layer, bool _window) : layer(_layer), window(_window) { }
        int layer;
        bool window;
    };
    static inline const Configuration &getConfiguration() { return sConfiguration; }
    static void setConfiguration(const Configuration &config);
    IDirectFBWindow *getDirectFBWindow() const { return mDirectFBWindow; }
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
    struct Configuration {
        inline Configuration(int _samples) : targetSamples(_samples) { }
        int targetSamples;
    };
    static inline const Configuration &getConfiguration() { return sConfiguration; }
    static void setConfiguration(const Configuration &config);
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
    inline Graphics::Context * &getContext() { return mContext; }
    inline StrongPtr<Texture> getTexture() { return mTexture; }
    void renderBegin();
    void renderEnd();
    void renderGibbon();
    void renderNova();
    void queueForFinalize(StrongPtr<Texture> texture);
    void addToLockedGlyphs(const std::vector<ftcache::Glyph::SharedPtr>& glyphs);
#endif


protected:
    void vsync_sys();
    void makeCurrent_sys(bool current);
    void flip_sys(const Rect &rect);
    void flip_sys(const DisplayList::SharedPtr &displayList, const Rect &rect);

    void init();
    void init(unsigned int width, unsigned int height, unsigned int depth, unsigned int flags);
    void cleanup();
    void handleUpdate(const Rect &rect);
    bool canScaleDirectly() const;
    void sendRenderEvent();

    static EffectRenderers createEffectRenderers_internal(const Effects &effects);
    void flip_internal(const DisplayList::SharedPtr &displayList, const Rect &rect);
    void vsync_internal();

    void initSurface();
    void cleanupSurface();

    Rect mDirty;
    int mFlipSleep;
    bool mSplashScreen;
    int mRenderSceneDisabled;
    bool mNeedsRestart;
    Widget::SharedPtr mRoot, mOverlay;
    Stopwatch::ms mVSyncLast, mVsyncInterval;
    DisplayList::SharedPtr mRootDisplayList;
    static Mutex sLock;
    shared_ptr<FrameGraph> mFrameGraph;
    Surface::SharedPtr mSurface;
    Surface::SharedPtr mScaledSurface;
    float mCoordinateScale;
    Size mVirtualSize;
    Surface::Format mFormats[3];
    EndianType mEndian;
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    IDirectFB* mDirectFB;
    IDirectFBWindow *mDirectFBWindow;
    static Configuration sConfiguration;
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
    static Configuration sConfiguration;
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
    Graphics::Context * mContext;
    StrongPtr<Texture> mTexture;
    RenderableType * mGibbonRenderable;
    RenderableType * mNovaRenderable;
    unsigned int mRenderBeginCount;
    nova::Mutex * mNeedsFinalizeLock;
    std::set< StrongPtr<Texture> > mNeedsFinalize;
    std::vector< std::vector<ftcache::Glyph::SharedPtr> > mLockedGlyphs;
#endif

    mutable Mutex mRunningMutex;
    bool mRunning;

    typedef WeakPtrSet<Listener> ListenerContainer;
    mutable ListenerContainer mListeners;

    class PlatformData;
    PlatformData *mPlatform;
};

}} // namespace netflix::gibbon

#endif // SCREEN_H
