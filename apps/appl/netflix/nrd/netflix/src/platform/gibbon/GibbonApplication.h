/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GIBBONAPPLICATION_H
#define GIBBONAPPLICATION_H

#include <string>
#include <vector>
#include <nrdbase/StdInt.h>

#include <ScriptEngine.h>
#include <GibbonBridge.h>
#include <GibbonConsole.h>
#include <GibbonConfiguration.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/Time.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/ConsoleSink.h>
#include <nrdbase/ThreadLocalStore.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/ScopedMutex.h>
#include <nrd/NrdApplication.h>
#include <nrdnet/IAsyncHttpLib.h>

#ifdef GIBBON_CORE_POSIX
class PosixEventLoop;
#endif
#if !defined(NRDP_PLATFORM_NOVA) && defined(BUILD_DEBUG)
# define GIBBON_CHECK_RENDER_THREAD
#endif

enum GibbonOEMEvent {
    GibbonOEM_Init                = 1, //Sent early in main before nrd is initialized
    GibbonOEM_Start               = 5, //Sent just before nrd is started
    GibbonOEM_Started             = 2, //Sent just after nrd has started
    GibbonOEM_Cleanup             = 3, //Sent just before nrd exits
    GibbonOEM_Suspended           = 4, //Sent when nrd would has completed suspending
    GibbonOEM_WatchdogSigChild    = 6, //Sent when watchdog receives a SigChild
    GibbonOEM_WatchdogNoHeartbeat = 7, //Sent when watchdog does not receive a heartbeat for a period of time
    GibbonOEM_Rendered            = 8  //Sent when screen has been rendered and flipped
};
extern void gibbon_oem_event(GibbonOEMEvent event, void *data=0);
extern void gibbon_oem_set_splash(bool);

enum GibbonWatchdogEvent {
    GibbonWatchdog_Relaunch        = 1,
    GibbonWatchdog_Shutdown        = 2,
    GibbonWatchdog_Instrumentation = 3,
    GibbonWatchdog_SigChild        = 4,
};
extern void gibbon_watchdog_event(GibbonWatchdogEvent event, const netflix::DataBuffer &data=netflix::DataBuffer());

namespace netflix {
namespace gibbon {

class Widget;
class Surface;
class SurfaceCache;
class RemoteScreenClient;
class RemoteScreenServer;
class GibbonResourceManager;
class GibbonConsoleSink;

class Screen;
class InputEvent;
class InputManager;

class GibbonSetLocationEvent;
class FPSTimer;
class GibbonApplication : public NrdApplication
{
public:
    GibbonApplication();
    virtual ~GibbonApplication();

    static GibbonApplication *instance() { return static_cast<GibbonApplication*>(NrdApplication::instance()); }

    static inline int getArgc() { return sArgc; }
    static inline char **getArgv() { return sArgv; }
    static inline void setArgs(int argc, char **argv)
    {
        sArgc = argc;
        sArgv = argv;
    }

    static bool isRenderThread();
    static inline void checkRenderThread() {
#ifdef GIBBON_CHECK_RENDER_THREAD
        assert(isRenderThread());
#endif
    }

    void startAnimation();
    void endAnimation();

    inline shared_ptr<RemoteScreenServer> getRemoteScreenServer() const {
        return mServer;
    }
    inline shared_ptr<RemoteScreenClient> getRemoteScreenClient() const {
        return mClient;
    }

    inline shared_ptr<SurfaceCache> getSurfaceCache() const {
        ScopedMutex _lock(mMutex);
        return mSurfaceCache;
    }
    inline shared_ptr<GibbonBridge> getBridge() const {
        ScopedMutex _lock(mMutex);
        return mGibbonBridge;
    }
    inline shared_ptr<DebuggerBridge> getDebugger() const {
        ScopedMutex _lock(mMutex);
        return mGibbonBridge ? mGibbonBridge->getDebugger() : shared_ptr<DebuggerBridge>();
    }
    inline shared_ptr<InputManager> getInputManager() const {
        ScopedMutex _lock(mInputManagerMutex);
        return mInputManager;
    }
    shared_ptr<GibbonResourceManager> resourceManager() const;
    inline shared_ptr<Screen> getScreen() const { return mScreen; } //only on construction
    inline shared_ptr<ScriptEngine> getScriptEngine() const { return mScriptEngine; } //only on construction

    virtual Variant versionInfo() const;

    void playbackExitedEvent();
    void playbackEnteredEvent();

    inline void inputEvent(InputEvent *event) { handleEvent((GibbonEvent*)event); } //compatibility
    void handleEvent(GibbonEvent *event);

    void initGraphics();
    void cleanupGraphics();
    void initScriptEngine(const std::string &spec);
    void initSurfaceCache(const std::string &spec);

    inline int surfaceCacheCapacity() const {
        netflix::ScopedMutex lock(mMutex);
        return mCacheCapacity.surface;
    }
    inline int surfaceCachePlaybackCapacity() const {
        netflix::ScopedMutex lock(mMutex);
        return mCacheCapacity.surfacePlayback;
    }
    void setSurfaceCacheCapacity(int cap);
    void setSurfaceCachePlaybackCapacity(int cap);

    virtual Variant send(const Request &request);
    virtual bool debuggerAttached() const;

    int getWidgetCount() const;
    shared_ptr<Widget> findWidget(const DumpInfo &dump) const;
    void addWidget(const shared_ptr<Widget> &widget);
    void removeWidget(int id);
    void removeWidget(const shared_ptr<Widget> &widget);
    shared_ptr<Widget> getWidget(int id) const;
    std::list<shared_ptr<Widget> > getWidgets() const;
    std::list<shared_ptr<Widget> > getRootWidgets() const;

    bool isIdle() const;
    enum {
        Collect_Surfaces = 0x01,
        Collect_JS       = 0x02,
        Collect_TcMalloc = 0x04,
        Collect_All      = 0xFF
    };
    size_t garbageCollect(uint8_t collect=Collect_All);

    bool isSuspended() const;
    void setSuspended(bool m, const std::string &reason=std::string());
    void setSuspendedNow(bool m, const std::string &reason=std::string());

    virtual DataBuffer dataURI(const std::string &path) const;

    virtual bool initConfiguration(int argc, char **argv) { return GibbonConfiguration::init(argc, argv); }
    virtual ConfigurationOptions defaultConfigurationOptions() const { return GibbonConfiguration::defaultOptions(); }

    int exec();
    void update(int wait = -1);

    inline void reenter() { eventLoop()->run(); }
    inline void leave() { eventLoop()->exit(); }
    void changedScene();

    void restart();
    void clearScene();
    void reloadCaches();

    enum Location {
        Location_Base,
        Location_Current
    };
    void reload(Location location);
    void setLocation(const Variant &value);
    inline int getLocationCount() const { return mScriptEngine ? mScriptEngine->getLocationCount() : 0; }
    inline AseUrl getLocation() const { return mScriptEngine ? mScriptEngine->getLocation() : AseUrl(); }

    bool runningFpsTimer() const;
    void startFpsTimer();
    void stopFpsTimer();
    int getFps() { return mFps; }

    void initInputManager();
    void initRemoteClient(const AseUrl &url);

    virtual Variant stats(uint8_t flags) const;

    enum { MallocSystem = 0x1, MallocJavascript = 0x2 };
    void dumpMalloc(int mode) const;

    bool addSocketizerCondition(const Variant& condition);
    bool addSocketizerConditions(const Variant& condition);

#ifndef GIBBON_NO_CONSOLE
    inline shared_ptr<GibbonConsole> console() const { return static_pointer_cast<GibbonConsole>(Application::console()); }
#endif
protected:
    virtual EventLoop::SharedPtr createEventLoop() const;
#ifndef GIBBON_NO_CONSOLE
    Console::SharedPtr createConsole() const { return shared_ptr<GibbonConsole>(new GibbonConsole); }
    virtual ConsoleSink::SharedPtr createConsoleSink() const { return ConsoleSink::SharedPtr(new GibbonConsoleSink); }
#endif

    int runEventLoop();
    void initEventLoop();
    void cleanupInputManager();

    void resetAppTrustStore();

#ifdef INSTRUMENTATION_ENABLED
    virtual void instrumentation(const instrumentation::Event::SharedPtr &inst);
#endif
    virtual void event(ApplicationEvent::Type event, const Variant &data);
private:
    void loadPrefetchUrls(const std::string &prefetchKey,
                          const Variant &prefetchUrls,
                          Variant *locationSet);
    void loadInjectJS();
    DataBuffer dataURI_internal(const std::string &path) const;

    bool mSuspend;
    shared_ptr<Screen> mScreen;
    mutable Mutex mMutex, mInputManagerMutex;
    shared_ptr<ScriptEngine> mScriptEngine;
    shared_ptr<InputManager> mInputManager;
    shared_ptr<GibbonBridge> mGibbonBridge;
    shared_ptr<SurfaceCache> mSurfaceCache;
    std::map<int, weak_ptr<Widget> > mWidgets;
    struct CacheCapacity {
        CacheCapacity()
            : surface(-1), surfacePlayback(-1), mode(Browse)
        {
        }
        int surface;
        int surfacePlayback;
        enum Mode { Playback, Browse } mode;
    } mCacheCapacity;
    shared_ptr<RemoteScreenServer> mServer;
    int mFps;
    friend class FPSTimer;
    shared_ptr<FPSTimer> mFpsTimer;
    shared_ptr<RemoteScreenClient> mClient;
    friend class DataURIEvent;

    static int sArgc;
    static char **sArgv;
};

static inline GibbonApplication *gibbonApp() { return GibbonApplication::instance(); }

class GibbonSetSuspendedEvent : public GibbonApplication::Event
{
public:
    GibbonSetSuspendedEvent(bool suspend, const std::string &reason) : mSuspend(suspend), mReason(reason) { }
    std::string describe() const
    {
        return "GibbonSetSuspended:" + mReason;
    }
    virtual void eventFired() {
        PERFORMANCE_MARKER_SCOPED("setsuspendedevent.fired");
        GibbonApplication::instance()->setSuspendedNow(mSuspend, mReason);
    }
private:
    bool mSuspend;
    std::string mReason;
};
}} // namespace netflix::gibbon

#endif // GIBBONAPPLICATION_H
