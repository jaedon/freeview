/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonApplication.h"

#include <Debug.h>
#include <Screen.h>
#include <Animation.h>
#include <TextEngine.h>
#include <FontManager.h>
#include <DisplayList.h>
#include <RemoteScreen.h>
#include <WidgetBridge.h>
#include <SurfaceCache.h>
#include <InputManager.h>
#include <SurfaceDecoder.h>
#include <GraphicsEngine.h>
#include <GibbonConfiguration.h>
#include <nrd/SystemDataKey.h>

#include <gibbon/config.h>
#include <nrdbase/config.h>

#ifdef HAVE_MALLINFO
#  include <malloc.h>
#endif

#include <GibbonFreetype.h>

#include <nrd/AppLog.h>

#include <nrdnet/AseUrl.h>
#include <nrdbase/Thread.h>
#include <nrdbase/DumpInfo.h>
#include <nrdbase/Version.h>
#include <nrdbase/Configuration.h>

#ifdef HAVE_GIBBON_TCMALLOC
# include <gperftools/tcmalloc.h>
# include <gperftools/malloc_extension.h>
#endif

#include <unicode/putil.h>
#include <unicode/uclean.h>
#include <unicode/utypes.h>

#ifdef NRDP_HAS_SOCKETIZER
# include <nrdsocketizer/Socketizer.h>
using namespace netflix::socketizer;
#endif

#if !defined(GIBBON_NO_THREADANIMATION)
# define GIBBON_BRIDGE_ENQUEUE
#endif

using namespace netflix;
using namespace netflix::gibbon;

namespace netflix {
namespace gibbon {

extern std::string gibbon_indent(int indent);

class FPSTimer : public GibbonApplication::Timer
{
public:
    FPSTimer(int interval);

    virtual std::string describe() const { return "FPSTimer"; }
    virtual void timerFired();

private:
    ullong m_started;
    ullong m_frames;
};

FPSTimer::FPSTimer(int interval)
    : Application::Timer(interval), m_frames(0)
{
    m_started = Time::serverTime().ms();
}

void FPSTimer::timerFired()
{
    PERFORMANCE_MARKER_SCOPED("fpstimer.fired");

    ++m_frames;

    const ullong now = Time::serverTime().ms();
    const ullong diff = now - m_started;
    if(diff >= 1000) {
        const double div = static_cast<double>(diff) / 1000.;
        assert(GibbonApplication::instance());
        GibbonApplication::instance()->mFps = static_cast<int>(static_cast<double>(m_frames) / div);
        Log::warn(TRACE_GIBBON_GRAPHICS, "fps: %d\n", GibbonApplication::instance()->mFps);
        m_frames = 0;
        m_started = now;
    }
}

#if defined(GIBBON_BRIDGE_ENQUEUE)

class QueuedRequestEvent : public Application::Event
{
public:
    inline QueuedRequestEvent(const Request &request)
        : mRequest(request)
    {}
    std::string describe() const { return std::string("GibbonBaseBridgeSendEvent::AnimationSync"); }
    virtual void eventFired()
    {
        PERFORMANCE_MARKER_SCOPED("animationsync.fired");
        ScopedMutex _lock(Widget::lock());
        if(shared_ptr<NfObject> bridge = mRequest.object()) {
            bridge->send(mRequest);
        }
    }

private:
    const Request mRequest;
};
#endif

class GibbonEventLoop : public EventLoop
{
public:
    GibbonEventLoop() : EventLoop() { }
    ~GibbonEventLoop()  { }
    virtual void wakeup()
    {
        if(shared_ptr<InputManager> input = GibbonApplication::instance()->getInputManager())
            input->wakeup();
        EventLoop::wakeup();
    }
protected:
    virtual void wait(llong waitMS)
    {
        if(shared_ptr<InputManager> input = GibbonApplication::instance()->getInputManager()) {
            input->waitForInput(waitMS);
            return;
        }
        return EventLoop::wait(waitMS);
    }
    virtual bool compress(const Application::Event::SharedPtr &event, EventQueue &queue)
    {
        if(event->type() == InputEvent::Type_MouseMove) { //compression
            int i = queue.size();
            while (i-- > 0) {
                const Application::Event::SharedPtr &ev = queue.at(i);
                assert(ev);
                if(ev->type() == InputEvent::Type_MousePress || ev->type() == InputEvent::Type_MouseRelease)
                    break;
                if(ev->type() == InputEvent::Type_MouseMove) {
                    shared_ptr<MouseEvent> newEvent = static_pointer_cast<MouseEvent>(event);
                    shared_ptr<MouseEvent> oldEvent = static_pointer_cast<MouseEvent>(ev);
                    if(newEvent->mButtons == oldEvent->mButtons &&
                       newEvent->mShowCursor == oldEvent->mShowCursor &&
                       newEvent->mModifiers == oldEvent->mModifiers) {
                        oldEvent->mPos = newEvent->mPos;
                        return true;
                    }
                    break;
                }
            }
        } else if(event->type() == InputEvent::Type_KeyPress) {
            int i = queue.size();
            while (i-- > 0) {
                const Application::Event::SharedPtr &ev = queue.at(i);
                assert(ev);
                if(ev->type() == InputEvent::Type_KeyRelease)
                    break;
                if(ev->type() == InputEvent::Type_KeyPress) {
                    shared_ptr<KeyEvent> newEvent = static_pointer_cast<KeyEvent>(event);
                    shared_ptr<KeyEvent> oldEvent = static_pointer_cast<KeyEvent>(ev);
                    if(oldEvent->mKey == newEvent->mKey &&
                       oldEvent->mText == newEvent->mText &&
                       oldEvent->mModifiers == newEvent->mModifiers) {
                        oldEvent->mRepeat = true;
                        ++oldEvent->mCount;
                        return true;
                    }
                    break;
                }
            }
        }
        return EventLoop::compress(event, queue);
    }
};

}}

int GibbonApplication::sArgc = 0;
char **GibbonApplication::sArgv = 0;

GibbonApplication::GibbonApplication()
    : NrdApplication(), mMutex(GIBBON_APPLICATION_MUTEX, "GibbonApplication", Mutex::NonRecursive),
      mInputManagerMutex(GIBBON_APPLICATION_INPUT_MANAGER_MUTEX, "GibbonApplication::InputManager"), mFps(0)
{
    mSuspend = !Configuration::suspendModeOnStart().empty();

#ifdef NRDP_HAS_SOCKETIZER
    const std::string socketizerFile = Configuration::socketizerConditionFile();
    if(!socketizerFile.empty()) {
        const DataBuffer socketizerData = DataBuffer::fromFile(socketizerFile.c_str());
        const Variant socketizerCondition = Variant::fromJSON(socketizerData);
        if(socketizerCondition.isArray() || socketizerCondition.isStringMap()) {
            addSocketizerConditions(socketizerCondition);
        } else {
            Log::error(TRACE_UI_ENGINE, "Socketizer file didn't parse as JSON: %s", socketizerFile.c_str());
        }
    }
#endif

    Debug::init();
    GraphicsEngine::initCommands();
    mScreen.reset(new Screen());
    mScriptEngine.reset(new ScriptEngine);
}

GibbonApplication::~GibbonApplication()
{
    stopFpsTimer();

    if(mServer) {
        mServer->quit();
        mServer->Wait();
        mServer.reset();
    }
    if(mClient) {
        mClient->disconnect();
        mClient.reset();
    }
    Application::cleanupConsole();
    cleanupInputManager();
    mScriptEngine.reset();
    clearEventConnections();
    Animation::shutdown();
    GibbonFreetype::destroyCache();
    FontManager::clear();
#if defined(GIBBON_NO_THREADANIMATION)
    cleanupGraphics();
#endif
}

EventLoop::SharedPtr GibbonApplication::createEventLoop() const
{
    return EventLoop::SharedPtr(new GibbonEventLoop);
}

int GibbonApplication::exec()
{
    setResourceManager(ResourceManager::SharedPtr(new GibbonResourceManager));
    Animation::init();

    NTRACE(TRACE_PERFORMANCE, "Bookmark: Exec");
    if (!start())
        return 1;

    if (Configuration::consoleEnabled()) {
        assert(console());
        static_pointer_cast<GibbonConsole>(console())->Start();
    }

    loadInjectJS();

    Variant location;
    Variant prefetchUrls;
    if (const Variant *conf = GibbonConfiguration::prefetchUrls()) {
        prefetchUrls = *conf;
    } else {
        const std::string prefetchStr = readSystemConfiguration(SystemKeys::PrefetchUrls);
        if(!prefetchStr.empty()) {
            writeSystemConfiguration(SystemKeys::PrefetchUrls, std::string(), NrdApplication::WriteMode_Flush);
            bool ok;
            prefetchUrls = Variant::fromJSON(prefetchStr, &ok);
            if(!ok || !prefetchUrls.isStringMap()) {
                Log::error(TRACE_UI_ENGINE, "Unable to parse prefetch urls: %s\n", prefetchStr.c_str());
                prefetchUrls.clear();
            }
        }
    }
    const std::string prefetchKey = GibbonConfiguration::prefetchKey();
    loadPrefetchUrls(prefetchKey, prefetchUrls, &location);
    mGibbonBridge->setPrefetchUrls(prefetchKey, prefetchUrls);

    if (location.isNull()) {
        // Load the initial page
        NTRACE(TRACE_PERFORMANCE, "Bookmark: LoadUrl: %s", NrdConfiguration::bootRequest().mapValue<std::string>("url").c_str());
        reload(Location_Base);
    } else {
        NTRACE(TRACE_PREFETCH, "Loading prefetch ui: %s", location.toJSON().c_str());
        location["bootfallback"] = true;
        setLocation(location);
    }

    if(GibbonConfiguration::showFps()) // set up the performance timer
        startFpsTimer();

#ifdef NRDP_WEBSERVER_ENABLED
    if(NrdConfiguration::nbpdEnabled())
        startMongoose();
#endif
    if(const int ret = runEventLoop())
        return ret;
    return quitCode();
}

void GibbonApplication::resetAppTrustStore()
{
    DataBuffer pem = GibbonApplication::instance()->appTrustStore();

    if(pem.isEmpty()) {
        pem = Configuration::resourceContent(GibbonConfiguration::uiCAFile());
        if(pem.isEmpty())
            pem = DataBuffer::fromFile(GibbonConfiguration::uiCAFile().c_str());
    }

    setAppTrustStore(pem.toString());
}

void GibbonApplication::setSurfaceCacheCapacity(int cap)
{
    CacheCapacity::Mode mode;
    shared_ptr<SurfaceCache> surfaceCache;
    {
        ScopedMutex _lock(mMutex);
        mCacheCapacity.surface = cap;
        mode = mCacheCapacity.mode;
        surfaceCache = mSurfaceCache;
    }
    // there's a race here but we don't care. the ui should avoid
    // setting the capacity while they're starting or stopping playback
    if (mode == CacheCapacity::Browse && surfaceCache) {
        surfaceCache->setCapacity(cap);
        garbageCollect(Collect_Surfaces);
    }
}

void GibbonApplication::setSurfaceCachePlaybackCapacity(int cap)
{
    CacheCapacity::Mode mode;
    shared_ptr<SurfaceCache> surfaceCache;
    {
        ScopedMutex _lock(mMutex);
        mCacheCapacity.surfacePlayback = cap;
        mode = mCacheCapacity.mode;
        surfaceCache = mSurfaceCache;
    }
    // there's a race here but we don't care. the ui should avoid
    // setting the capacity while they're starting or stopping playback
    if (mode == CacheCapacity::Playback && surfaceCache) {
        surfaceCache->setCapacity(cap);
        garbageCollect(Collect_Surfaces);
    }
}

void GibbonApplication::event(ApplicationEvent::Type event, const Variant &data)
{
    switch (event) {
    case ApplicationEvent::Event_ServerTimeChanged:
        break;
    case ApplicationEvent::Event_SuspendComplete:
        NTRACE(TRACE_SUSPEND, "calling gibbon_oem_event(GibbonOEM_Suspended)");
        gibbon_oem_event(GibbonOEM_Suspended);
        break;
    case ApplicationEvent::Event_Started: {
        {
            ScopedMutex _lock(mMutex);
            mGibbonBridge.reset(new GibbonBridge);
            mGibbonBridge->init();
            findObject("nrdp")->addChild(mGibbonBridge);
        }

        initScriptEngine(readSystemConfiguration(SystemKeys::ScriptEngine));
        initSurfaceCache(readSystemConfiguration(SystemKeys::SurfaceCache));

#if defined(GIBBON_NO_THREADANIMATION)
        initGraphics();
#endif
        resetAppTrustStore();
        resourceManager()->cookieManager()->load();

        addLibraryInfo(GibbonFreetype::getLibraryInfo());
        addLibraryInfo(GibbonHarfbuzz::getLibraryInfo());
        addLibraryInfo(GibbonHarfbuzz::getGraphiteInfo());
        addLibraryInfo(GibbonHarfbuzz::getICUInfo());

        checkLibraryVersions();
        break; }
    case ApplicationEvent::Event_Shutdown: {
        cleanupInputManager();
        mScriptEngine->stop();
        Animation::shutdown();
        Application::cleanupEventLoop();
        clearScene();
        {
            ScopedMutex _lock(mMutex);
            if(mGibbonBridge) {
                mGibbonBridge->cleanup();
                mGibbonBridge.reset();
            }
        }
        break; }
    case ApplicationEvent::Event_FactoryReset:
        // Clear the cookie jar
        resourceManager()->cookieManager()->clear();

        // reset the trust store
        resetAppTrustStore();

        // remove downloaded fonts from the font manager
        FontManager::clearDownloadedFonts();
        break;
    case ApplicationEvent::Event_PlaybackExited:
    case ApplicationEvent::Event_PlaybackEntered: {
        int capacity = 0;
        shared_ptr<SurfaceCache> surfaceCache;
        {
            ScopedMutex _lock(mMutex);
            mCacheCapacity.mode = (event == ApplicationEvent::Event_PlaybackEntered) ? CacheCapacity::Playback : CacheCapacity::Browse;
            if(mCacheCapacity.surfacePlayback != mCacheCapacity.surface) {
                if(event == ApplicationEvent::Event_PlaybackEntered)
                    capacity = mCacheCapacity.surfacePlayback;
                else
                    capacity = mCacheCapacity.surface;
                surfaceCache = mSurfaceCache;
            }
        }
        if(surfaceCache) {
            surfaceCache->setCapacity(capacity);
            garbageCollect(Collect_Surfaces);
        }
        break; }
    case ApplicationEvent::Event_UrlChanged:
        getBridge()->locationChanged();
        Log::warn(TRACE_UI_ENGINE, "UrlChanged: %s", getLocation().c_str());
        break;
    case ApplicationEvent::Event_Suspend:
    case ApplicationEvent::Event_Resume: {
        if(!hasSuspendHandler())
            GibbonApplication::setSuspended(event == ApplicationEvent::Event_Suspend);
        if(mClient)
            mClient->handleSuspendChanged(event == ApplicationEvent::Event_Suspend, data);
        break; }
    case ApplicationEvent::Event_SystemDataDatumChanged:
        switch (data.value<int>()) {
        case SystemDataKey::SYSTEM_FILE: {
            const std::map<std::string, std::string> networking_params = serviceParameters("networking");
            const std::map<std::string, std::string> streaming_params = serviceParameters("streaming");
            if(networking_params.find("surface_cache") != networking_params.end()) //try networking params first
                writeSystemConfiguration(SystemKeys::SurfaceCache, mapValue(networking_params, std::string("surface_cache")));
            else //fallback to streaming params
                writeSystemConfiguration(SystemKeys::SurfaceCache, mapValue(streaming_params, std::string("gibbon_surface_cache")));
            break; }
        case SystemDataKey::UI_LANGUAGE: {
            const std::string language = nrdApp()->uiLanguages();
            resourceManager()->setLanguage(language);
            break; }
        default:
            break;
        }
        break;
    case ApplicationEvent::Event_UrlAboutToChange:
        break;
    }
    NrdApplication::event(event, data);
}

void GibbonApplication::addWidget(const Widget::SharedPtr &widget)
{
    assert(MutexStack::locked(Widget::lock().name()));
    mWidgets[widget->getID()] = widget;
}

void GibbonApplication::removeWidget(const Widget::SharedPtr &widget)
{
    removeWidget(widget->getID());
}

void GibbonApplication::removeWidget(int id)
{
    mWidgets.erase(id);
}

void GibbonApplication::update(int waitMS)
{
    PERFORMANCE_MARKER_SCOPED("gibbonapplication.update");
    eventLoop()->spin(waitMS);
}

int GibbonApplication::runEventLoop()
{
    eventLoop()->run();
    return 0;
}

std::list<Widget::SharedPtr> GibbonApplication::getWidgets() const
{
    std::list<Widget::SharedPtr> result;
    assert(MutexStack::locked(Widget::lock().name()));
    for(std::map<int, Widget::WeakPtr>::const_iterator it = mWidgets.begin(); it != mWidgets.end(); ++it) {
        if(Widget::SharedPtr w = it->second.lock())
            result.push_back(w);
    }
    return result;
}

std::list<shared_ptr<Widget> > GibbonApplication::getRootWidgets() const
{
    std::list<Widget::SharedPtr> result;
    assert(MutexStack::locked(Widget::lock().name()));
    for(std::map<int, Widget::WeakPtr>::const_iterator it = mWidgets.begin(); it != mWidgets.end(); ++it) {
        if(Widget::SharedPtr w = it->second.lock()) {
            if(!w->getParent())
                result.push_back(w);
        }
    }
    return result;
}

int GibbonApplication::getWidgetCount() const
{
    ScopedMutex _lock(Widget::lock());
    return mWidgets.size();
}

shared_ptr<Widget> GibbonApplication::findWidget(const DumpInfo &dump) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(!dump.empty()) {
        for(std::map<int, Widget::WeakPtr>::const_iterator it = mWidgets.begin(); it != mWidgets.end(); ++it) {
            if(Widget::SharedPtr w = it->second.lock()) {
                if(w->getID() == dump.idx || w.get() == dump.ptr ||
                   (dump.match.length() && w->getName() == dump.match))
                    return w;
            }
        }
    }
    return Widget::SharedPtr();
}

Widget::SharedPtr GibbonApplication::getWidget(int id) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(id == -1)
        return getScreen()->root();
    std::map<int, Widget::WeakPtr>::const_iterator it = mWidgets.find(id);
    if(it != mWidgets.end())
        return it->second.lock();
    return Widget::SharedPtr();
}

void GibbonApplication::initGraphics()
{
    int surfaceCacheCapacity = GibbonConfiguration::surfaceCacheCapacity();
    {
        ScopedMutex _lock(mMutex);
        if(mCacheCapacity.surface != -1)
            surfaceCacheCapacity = mCacheCapacity.surface;
        mSurfaceCache.reset(new SurfaceCache);
    }
    mSurfaceCache->init(surfaceCacheCapacity);

    {
        ScopedMutex _screenLock(Screen::lock());
        if(!mSuspend) {
            mScreen->start();
            initInputManager();
        }

        UErrorCode status = U_ZERO_ERROR;
        NTRACE(TRACE_UI_ENGINE, "Using ICU data library from: %s", (Configuration::dataReadPath() + "/icu").c_str());
        u_setDataDirectory((Configuration::dataReadPath() + "/icu").c_str());
        u_init(&status);
        if(U_FAILURE(status))
            Log::error(TRACE_UI_ENGINE, "Error initializing ICU: %s", u_errorName(status));

        FontManager::addFontsFromDirectory(GibbonConfiguration::textFontPath());
        TextEngine::init();
    }

    if(SurfaceDecoders::SharedPtr decoders = SurfaceDecoders::instance()) {
        Variant info = decoders->libraryInfo();
        const int count = info.size();
        for(int i=0; i<count; ++i) {
            addLibraryInfo(info.at(i));
        }
    }

#ifdef HAVE_GIBBON_TCMALLOC
    {
        Variant library;
        library["name"] = "tcmalloc";
        library["compiled"] = TC_VERSION_STRING;
        library["runtime"] = "";
        addLibraryInfo(library);
    }
#endif

    if(GibbonConfiguration::remoteScreenEnabled()) {
        mServer.reset(new RemoteScreenServer(GibbonConfiguration::remoteScreenPort()));
        mServer->Start();
    }
    mScreen->setShowSplashScreen(true);
}

void GibbonApplication::cleanupGraphics()
{
    clearScene();
    u_cleanup();

    if(mScreen)
        mScreen->cleanupScene();
    SurfaceDecoders::cleanup();
    GibbonFreetype::cleanup();
    if(mSurfaceCache) {
        mSurfaceCache->shutdown();
        mSurfaceCache.reset();
    }
    TextEngine::cleanup();
    DisplayList::cleanupFactories();
    if(mScreen) {
        mScreen->stop();
        mScreen.reset();
    }
}

void GibbonApplication::initRemoteClient(const AseUrl &url)
{
    if(mClient) {
        mClient->disconnect();
        mClient->Wait();
        mClient.reset();
    }
    mClient.reset(new RemoteScreenClient);
    mClient->connect(url);
}

void GibbonApplication::initScriptEngine(const std::string &spec)
{
    const Variant var = Variant::fromJSON(spec);
    if(var.contains("gcMaxBytesPerCycle"))
        mScriptEngine->setGarbageCollectMaxBytesPerCycle(var.mapValue<int>("gcMaxBytesPerCycle"));
}

void GibbonApplication::initSurfaceCache(const std::string &spec)
{
    CacheCapacity capacity;
    const Variant var = Variant::fromJSON(spec);
    const llong graphicsMemoryCapacity = getScreen()->getGraphicsMemoryCapacity();
    capacity.surface = var.mapValue<int>("capacity", 0, graphicsMemoryCapacity);
    if(capacity.surface > graphicsMemoryCapacity) {
        Log::error(TRACE_UI_ENGINE, "Error initializing stored surface cache capacity: %s [%d]", spec.c_str(), capacity.surface);
        capacity.surface = graphicsMemoryCapacity;
    }

    capacity.surfacePlayback = var.mapValue<int>("playbackCapacity", 0, GibbonConfiguration::surfaceCachePlaybackCapacity());
    if(capacity.surfacePlayback == -1)
        capacity.surfacePlayback = capacity.surface;
    const int confPBSurfaceCache = GibbonConfiguration::surfaceCachePlaybackCapacity() == -1 ? capacity.surface : GibbonConfiguration::surfaceCachePlaybackCapacity();
    if(capacity.surfacePlayback > confPBSurfaceCache) {
        Log::error(TRACE_UI_ENGINE, "Error initializing stored surface cache capacity: %s [%d]", spec.c_str(), capacity.surfacePlayback);
        capacity.surfacePlayback = confPBSurfaceCache;
    }

    shared_ptr<SurfaceCache> surfaceCache;
    {
        ScopedMutex _lock(mMutex);
        surfaceCache = mSurfaceCache;
        mCacheCapacity = capacity;
    }
    if(surfaceCache)
        mSurfaceCache->setCapacity(capacity.surface);
}

void GibbonApplication::clearScene()
{
    NrdApplication::setHasSuspendHandler(false);
    {
        ScopedMutex _lock(Widget::lock());
        if(mScreen)
            mScreen->setRoot(Widget::SharedPtr());
        if(shared_ptr<GibbonBridge> bridge = getBridge())
            bridge->clearScene();
    }
    FontManager::clearDownloadedFonts(FontManager::NonPersisted);
}

#if !defined(GIBBON_NO_THREADANIMATION)
class RestartEvent : public Application::Event
{
public:
    RestartEvent(const Screen::SharedPtr& s)
        : screen(s), mutex(ZERO_MUTEX, "RestartEvent::Mutex"), finished(false)
    {
    }

    virtual void eventFired()
    {
        if(Screen::SharedPtr s = screen.lock()) {
            ScopedMutex _lock(Widget::lock());
            if(s->isRunning())
                s->restart();
        }
        ScopedMutex lock(mutex);
        finished = true;
        cond.signal();
    }

    void wait()
    {
        ScopedMutex lock(mutex);
        while(!finished) {
            cond.wait(&mutex);
        }
    }

private:
    Screen::WeakPtr screen;
    Mutex mutex;
    bool finished;
    ConditionVariable cond;
};
#endif

void GibbonApplication::restart()
{
    clearScene();
    if(mScreen->isRunning() && mScreen->needsRestart()) {
#if !defined(GIBBON_NO_THREADANIMATION)
        cleanupInputManager();
        shared_ptr<RestartEvent> event(new RestartEvent(mScreen));
        if(Animation::postEvent(event))
            event->wait();
        initInputManager();
#else
        mScreen->restart();
#endif
    }
    gibbon_oem_event(GibbonOEM_Started);
}

void GibbonApplication::handleEvent(GibbonEvent *event)
{
    if(mClient)
        mClient->handleEvent(event);
    if(shared_ptr<GibbonBridge> bridge = getBridge())
        bridge->handleEvent(event);
}

void GibbonApplication::reloadCaches()
{
    getSurfaceCache()->clear(SurfaceCache::Reload);
    resourceManager()->clear();
    if(DiskCache::SharedPtr disk = resourceManager()->diskCache())
        disk->clearStats();
}

void GibbonApplication::reload(Location location)
{
    switch (location) {
    case Location_Base:
        setLocation(NrdConfiguration::bootRequest());
        break;
    case Location_Current:
        getScriptEngine()->reload();
        break;
    }
}

void GibbonApplication::setLocation(const Variant &value)
{
    std::string url;
    if (value.isString()) {
        url = value.string();
    } else {
        url = value.mapValue<std::string>("url");
    }
    const Variant headers = value.value("headers");
    assert(headers.isNull() || headers.isStringMap());
    HttpHeaders httpHeaders;
    if(headers.isStringMap()) {
        const Variant::StringMapIterator end = headers.stringMapEnd();
        for(Variant::StringMapIterator it = headers.stringMapBegin(); it != end; ++it)
            httpHeaders[it->first] = it->second.value<std::string>();
    }

    const Time timeout = Time::fromMS(value.mapValue<int>("timeout", 0,
                                                          GibbonConfiguration::defaultNetworkTimeout()));
    const Time connectTimeout = Time::fromMS(value.mapValue<int>("connectTimeout", 0,
                                                                 GibbonConfiguration::defaultNetworkConnectTimeout()));
    const Time stallTimeout = Time::fromMS(value.mapValue<int>("stallTimeout", 0,
                                                               GibbonConfiguration::defaultNetworkStallTimeout()));
    Resource::Method method = GibbonResourceManager::methodFromString(value.mapValue<std::string>("requestMethod"));
    unsigned int flags = 0;
    if(value.mapValue<bool>("reuse"))
        flags |= ScriptEngine::Location_ReuseEngine;
    if(value.mapValue<bool>("bootfallback"))
        flags |= ScriptEngine::Location_BootFallback;

    mScriptEngine->setLocation(url, flags, value.mapValue<DataBuffer>("source", 0, 0),
                               httpHeaders, timeout, connectTimeout, stallTimeout,
                               value.mapValue<DataBuffer>("body"), method);
}

bool GibbonApplication::isIdle() const
{
    if(Animation::isAnimating())
        return false;
    return true;
}

class GibbonDataUriEvent : public GibbonApplication::Event
{
public:
    GibbonDataUriEvent() : mutex(UNTRACKED_MUTEX, "GibbonDataUriEvent::Mutex")
    {
    }

    virtual DataBuffer getData() const;

    virtual void eventFired()
    {
        const DataBuffer b = getData();
        ScopedMutex scope(mutex);
        buffer = b;
        cond.signal();
    }

    virtual std::string describe() const
    {
        return "GibbonDataUriEvent";
    }

    DataBuffer wait()
    {
        ScopedMutex scope(mutex);
        while(buffer.isEmpty())
            cond.wait(&mutex);
        return buffer;
    }

private:
    Surface::SharedPtr surface;
    DataBuffer buffer;
    Mutex mutex;
    ConditionVariable cond;
};

static int recurseViewsEntries(const Widget::SharedPtr &w, std::vector<NrdApplication::DataListEntry> *entries, int indent)
{
    int result = 1;
    {
        NrdApplication::DataListEntry entry;
        std::string id_str;
        {
            std::ostringstream id_stream;
            id_stream << w->getID();
            id_str = id_stream.str();
        }
        entry.href = "/gibbon/widget/" + id_str;
        {
            std::string name = "<table><tr><td>";
            for(int i = 0; i < indent*4; ++i)
                name += "&nbsp;";
            name += "</td><td><a href=\"/gibbon/widget/js/" + id_str + "\">*</a><a href=\"" + entry.href.str() + "\">" + w->describe(true) + "</a></td></tr></table>";
            entry.name = name;
        }
        entry.size = 0;
        entries->push_back(entry);
    }
    const Widget::ChildContainer &children = w->getChildren();
    for(Widget::ChildContainer::const_iterator it = children.begin(); it != children.end(); ++it)
        result += recurseViewsEntries((*it), entries, indent+1);
    return result;

}

Variant GibbonApplication::stats(uint8_t flags) const
{
    Variant result = NrdApplication::stats(flags);
    if(flags & Stat_JS)
        result["js"] = mScriptEngine->getScriptStats();
    if(flags & Stat_Memory) {
        Variant &memory = result["memory"];
        if(SurfaceCache::SharedPtr surfaceCache = getSurfaceCache())
            memory["video"] = surfaceCache->size();
        if(mScriptEngine && mScriptEngine->isStarted()) {
            Variant jsc;
            const ScriptEngine::Stats stats = mScriptEngine->stats();
            jsc["heap"] = stats.heapSize;
            jsc["heap_capacity"] = stats.heapCapacity;
            jsc["fastmalloc"] = stats.fastMallocSystem;
            jsc["fastmalloc_free"] = stats.fastMallocFreebytes;
            jsc["fastmalloc_returned"] = stats.fastMallocReturned;
            jsc["fastmalloc_committed"] = stats.fastMallocFreebytes - stats.fastMallocReturned;
            jsc["objectCount"] = stats.objectCount;
            jsc["globalObjectCount"] = stats.globalObjectCount;
            jsc["protectedObjectCount"] = stats.protectedObjectCount;
            jsc["protectedGlobalObjectCount"] = stats.protectedGlobalObjectCount;
            jsc["osAllocatorCommitted"] = stats.osAllocatorCommitted;
            jsc["osAllocatorDecommitted"] = stats.osAllocatorDecommitted;
            jsc["jsStringCount"] = stats.jsStringCount;
            jsc["jsStringsSize"] = stats.jsStringsSize;
            memory["jsc"] = jsc;
        }
#ifdef HAVE_GIBBON_TCMALLOC
        MallocExtension* ext = MallocExtension::instance();
        assert(ext);
        size_t allocated = 0, heap = 0, unmapped = 0;
        ext->GetNumericProperty("generic.current_allocated_bytes", &allocated);
        ext->GetNumericProperty("generic.heap_size", &heap);
        ext->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes", &unmapped);
        Variant tcmalloc;
        tcmalloc["systemused"] = allocated;
        tcmalloc["systemallocated"] = heap - unmapped;
        memory["tcmalloc"] = tcmalloc;
#endif
    }
    return result;
}

namespace netflix {
namespace gibbon {
class DataURIEvent : public Application::Event
{
public:
    DataURIEvent(const std::string& p="") : path(p), mutex(ZERO_MUTEX, "DataURIEvent::Mutex"), finished(false) { }

    virtual DataBuffer getData() const
    {
        assert(!path.empty());
        return GibbonApplication::instance()->dataURI_internal(path);
    }

    virtual void eventFired()
    {
        const DataBuffer d = getData();
        ScopedMutex lock(mutex);
        data = d;
        finished = true;
        cond.signal();
    }

    virtual void eventCanceled()
    {
        ScopedMutex lock(mutex);
        finished = true;
        cond.signal();
    }

    virtual std::string describe() const { return "DataURIEvent"; };

    DataBuffer wait()
    {
        ScopedMutex lock(mutex);
        while(!finished)
            cond.wait(&mutex);
        return data;
    }

private:
    std::string path;
    DataBuffer data;
    Mutex mutex;
    bool finished;
    ConditionVariable cond;
};
}}

#if !defined(GIBBON_NO_THREADANIMATION)
class SurfaceDataWriteEvent : public DataURIEvent
{
public:
    SurfaceDataWriteEvent(Surface::SharedPtr s) : surface(s) { }
    virtual DataBuffer getData() const
    {
        PERFORMANCE_MARKER_SCOPED("surfacedatawriteevent.fired");
        return SurfaceDecoders::instance()->encode(surface);
    }
    virtual std::string describe() const { return DataURIEvent::describe() + "::Surface"; };

private:
    Surface::SharedPtr surface;
};
#endif

static std::string describe(const DisplayList::SharedPtr &dl)
{
    int indent = 0;
    std::ostringstream result;
    const std::list<DisplayList::Command::SharedPtr> &commands = dl->getCommands();
    for(std::list<DisplayList::Command::SharedPtr>::const_iterator it = commands.begin(); it != commands.end(); ++it) {
        switch((*it)->type()) {
        case DisplayList::Command::Type_Push:
            ++indent;
            break;
        case DisplayList::Command::Type_Pop:
            --indent;
            break;
        default:
            result << gibbon_indent(indent) << (*it)->describe() << std::endl;
            break;
        }
    }
    return result.str();
}

DataBuffer GibbonApplication::dataURI(const std::string &path) const
{
    {
        const DataBuffer base = NrdApplication::dataURI(path);
        if(!base.empty())
            return base;
    }
    if(startsWith(path, "/gibbon/resource/") || startsWith(path, "/gibbon/disk/") || !startsWith(path, "/gibbon/"))
        return dataURI_internal(path);
#if !defined(GIBBON_NO_THREADANIMATION)
    if(!Animation::isAnimationThread()) {
        shared_ptr<DataURIEvent> dataURIEvent(new DataURIEvent(path));
        if(!Animation::sendEvent(dataURIEvent))
            return DataBuffer();
        return dataURIEvent->wait();
    }
#else
    if(!isAppThread()) {
        shared_ptr<DataURIEvent> dataURIEvent(new DataURIEvent(path));
        if(!const_cast<GibbonApplication*>(this)->sendEvent(dataURIEvent))
            return DataBuffer();
        return dataURIEvent->wait();
    }
#endif
    return dataURI_internal(path);
}

DataBuffer GibbonApplication::dataURI_internal(const std::string &path) const
{
    if(path == "/gibbon/" || path == "/gibbon") {
        std::vector<DataListEntry> entries;
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "Stats";
            entry.href = "/html/stats.html";
            entries.push_back(entry);
        }
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "Screen";
            entry.href = "/img/screenshot";
            entries.push_back(entry);
        }
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "Render";
            entry.href = "/gibbon/widget/draw/";
            entries.push_back(entry);
        }
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "Widgets";
            entry.href = "/gibbon/widget/";
            entries.push_back(entry);
        }
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "ResourceCache";
            entry.href = "/gibbon/resource/";
            entries.push_back(entry);
        }
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "DiskCache";
            entry.href = "/gibbon/disk/";
            entries.push_back(entry);
        }

        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "SurfaceCache";
            entry.href = "/gibbon/surface/";
            entries.push_back(entry);
        }
        {
            DataListEntry entry;
            entry.size = 0;
            entry.name = "SurfaceCache (pending)";
            entry.href = "/gibbon/surface/pending/";
            entries.push_back(entry);
        }
        return dataList(entries);
    } else if(path == "/img/screenshot") {
        ScopedMutex _widgetLock(Widget::lock());
        if(Surface::SharedPtr surface = GibbonApplication::instance()->getScreen()->grab())
            return SurfaceDecoders::instance()->encode(surface);
    } else if(startsWith(path, "/gibbon/widget/js/")) {
        const std::string wdump = path.substr(18);
        ScopedMutex _widgetLock(Widget::lock());
        if(shared_ptr<WidgetBridge> widget = GibbonApplication::instance()->getBridge()->getWidget(wdump.empty() ? -1 : atoi(wdump.c_str()))) {
            const Variant result = widget->dump(WidgetBridge::Dump_Recursive);
            std::ostringstream href_stream;
            href_stream << "// " << widget->getWidget()->describe() << std::endl
                        << "var widget = " << result.toJSON(true) << std::endl
                        << "function createWidget(obj) {" << std::endl
                        << "     nrdp.gibbon.beginPendingSync();" << std::endl
                        << "     var widget = nrdp.gibbon.makeWidget(obj);" << std::endl
                        << "     nrdp.gibbon.endPendingSync();" << std::endl
                        << "     return widget;" << std::endl
                        << "}" << std::endl
                        << "function makeScene() { nrdp.gibbon.scene.widget = createWidget(widget); }" << std::endl
                        << "nrdp.gibbon._baseUrl = \"" << GibbonApplication::instance()->getLocation().str().c_str() << "\";" << std::endl
                        << "nrdp.addEventListener(\"init\", makeScene);" << std::endl
                        << "nrdp.isReady ? makeScene() : nrdp.init();" << std::endl;
            return href_stream.str();
        }
    } else if(startsWith(path, "/gibbon/widget/draw/")) {
        const std::string wdump = path.substr(20);
        ScopedMutex _widgetLock(Widget::lock());
        if(wdump.empty()) {
            std::ostringstream href_stream;
            if(DisplayList::SharedPtr grab = gibbonApp()->getScreen()->rootDisplayList())
                href_stream << "ROOTDISPLAYLIST" << std::endl << describe(grab) << std::endl;
            if(Widget::SharedPtr widget = gibbonApp()->getScreen()->root()) {
                if(DisplayList::SharedPtr grab = widget->grab(Widget::Render_Comment))
                    href_stream << "ROOT" << std::endl << describe(grab) << std::endl;
            }
            if(Widget::SharedPtr widget = gibbonApp()->getScreen()->overlay()) {
                if(DisplayList::SharedPtr grab = widget->grab(Widget::Render_Comment))
                    href_stream << "OVERLAY" << std::endl << describe(grab) << std::endl;
            }
            return href_stream.str();
        } else if(shared_ptr<WidgetBridge> widget = GibbonApplication::instance()->getBridge()->getWidget(atoi(wdump.c_str()))) {
            getScreen()->setOverlay(Widget::SharedPtr());
            getScreen()->createOverlay(widget->getWidget()->getGlobalRect());
            if(DisplayList::SharedPtr grab = widget->getWidget()->grab(Widget::Render_Comment)) {
                std::ostringstream href_stream;
                href_stream <<  widget->getWidget()->describe() << std::endl << describe(grab) << std::endl;
                return href_stream.str();
            }
            return DataBuffer();
        }
    } else if(startsWith(path, "/gibbon/widget/")) {
        const std::string wdump = path.substr(15);
        if(!wdump.empty()) {
            Surface::SharedPtr surface;
            {
                ScopedMutex _widgetLock(Widget::lock());
                if(shared_ptr<WidgetBridge> widget = GibbonApplication::instance()->getBridge()->getWidget(atoi(wdump.c_str()))) {
                    getScreen()->setOverlay(Widget::SharedPtr());
                    getScreen()->createOverlay(widget->getWidget()->getGlobalRect());
                    if(DisplayList::SharedPtr grab = widget->getWidget()->grab())
                        surface = grab->flatten();
                }
            }
            if(surface) {
#if defined(GIBBON_NO_THREADANIMATION)
                return SurfaceDecoders::instance()->write(surface);
#else
                shared_ptr<SurfaceDataWriteEvent> writeEvent(new SurfaceDataWriteEvent(surface));
                if(Animation::sendEvent(writeEvent))
                    return writeEvent->wait();
#endif
            }
            return DataBuffer();
        }
        ScopedMutex _widgetLock(Widget::lock());
        if(Widget::SharedPtr root = getScreen()->root()) {
            std::vector<DataListEntry> entries;
            recurseViewsEntries(root, &entries, 0);
            return dataList(entries);
        }
        return DataBuffer();
    } else if(startsWith(path, "/gibbon/surface/pending/")) {
        DumpInfo dump(path.substr(24));
        const std::list<SurfaceCache::DecodeInfo> decodes = mSurfaceCache->findPending(dump);

        std::vector<DataListEntry> entries;
        for(std::list<SurfaceCache::DecodeInfo>::const_iterator it = decodes.begin(); it != decodes.end(); ++it) {
            const SurfaceCache::DecodeInfo &decode = (*it);
            DataListEntry entry;
            entry.name = decode.description();
            entry.size = decode.resource->responseData().size();
            entries.push_back(entry);
        }
        return dataList(entries);
    } else if(startsWith(path, "/gibbon/surface/")) {
        DumpInfo dump(path.substr(16));
        const std::list<Surface::SharedPtr> surfaces = mSurfaceCache->find(dump);
        if(surfaces.size() == 1) {
#if defined(GIBBON_NO_THREADANIMATION)
            return SurfaceDecoders::instance()->write(surfaces.front());
#else
            shared_ptr<SurfaceDataWriteEvent> writeEvent(new SurfaceDataWriteEvent(surfaces.front()));
            if(Animation::sendEvent(writeEvent))
                return writeEvent->wait();
            return DataBuffer();
#endif
        }

        std::vector<DataListEntry> entries;
        for(std::list<Surface::SharedPtr>::const_iterator it = surfaces.begin(); it != surfaces.end(); ++it) {
            Surface::SharedPtr surface = (*it);
            DataListEntry entry;
            entry.name = surface->describe();
            {
                std::ostringstream href_stream;
                href_stream << std::hex << surface.get();
                std::string href = href_stream.str();
                if(!startsWith(href, "0x"))
                    href = "0x" + href;
                href = "/gibbon/surface/" + href;
                entry.href = href;
            }
            entry.size = surface->calculateSize();
            entries.push_back(entry);
        }
        return dataList(entries);
    } else if(startsWith(path, "/gibbon/resource/")) {
        const std::list<Resource::SharedPtr> resources = resourceManager()->find(DumpInfo(path.substr(17)));
        if(resources.size() == 1) {
            return resources.front()->responseData();
        }

        std::vector<DataListEntry> entries;
        entries.reserve(resources.size());
        for(std::list<Resource::SharedPtr>::const_iterator it = resources.begin(); it != resources.end(); ++it) {
            Resource::SharedPtr response = (*it);
            DataListEntry entry;
            entry.name = response->url().str();
            {
                std::ostringstream href_stream;
                href_stream << std::hex << response.get();
                std::string href = href_stream.str();
                if(!startsWith(href, "0x"))
                    href = "0x" + href;
                href = "/gibbon/resource/" + href;
                entry.href = href;
            }
            entry.size = response->cost();
            entries.push_back(entry);
        }
        return dataList(entries);
    } else if(startsWith(path, "/gibbon/disk/")) {
        if(DiskCache::SharedPtr diskCache = resourceManager()->diskCache()) {
            DumpInfo dump(path.substr(13));
            const std::list<DiskCache::Response::SharedPtr> resources = diskCache->find(dump);
            if(resources.size() == 1)
                return resources.front()->responseData();

            std::vector<DataListEntry> entries;
            entries.reserve(resources.size());
            for(std::list<DiskCache::Response::SharedPtr>::const_iterator it = resources.begin(); it != resources.end(); ++it) {
                DiskCache::Response::SharedPtr response = (*it);
                DataListEntry entry;
                entry.name = response->url();
                {
                    std::ostringstream href;
                    href << "/gibbon/disk/" << response->cacheKey().toBase64().c_str();
                    entry.href = href.str();
                }
                entry.size = response->size();
                entries.push_back(entry);
            }
            return dataList(entries);
        }
    } else if(startsWith(path, "/gibbon/mem/js/") || path == "/gibbon/mem/js") {
        if(Application::isAppThread()) {
            std::string result;
            gibbonApp()->getScriptEngine()->dump(result);
#if 0
            static int memory = 0;
            const std::string memory_name(StringFormatter::sformat("/tmp/memory%d.js", memory++));
            if(FILE *f = fopen(memory_name.c_str(), "w")) {
                Log::error(TRACE_LOG, "[%s:%d]: %s", __FILE__, __LINE__, memory_name.c_str());
                fwrite(result.c_str(), result.length(), 1, f);
                fclose(f);
            }
#endif
            return result;
        }
        shared_ptr<DataURIEvent> dataURIEvent(new DataURIEvent(path));
        GibbonApplication::instance()->sendEvent(dataURIEvent);
        return dataURIEvent->wait();
    }
    return NrdApplication::dataURI(path);
}

void GibbonApplication::changedScene()
{
    if(shared_ptr<GibbonBridge> bridge = getBridge())
        bridge->getDebugger()->onSceneChanged();
}

size_t GibbonApplication::garbageCollect(uint8_t collect)
{
    size_t result = 0;
    if(collect & Collect_JS) //  Just like any ScriptEngineJSC call, the the below garbageCollect() MUST be called from the main thread
        result += getScriptEngine()->garbageCollect();
    if(collect & Collect_Surfaces) {
        result += Widget::purgeAll(Widget::Purge_Offscreen);
        getSurfaceCache()->purge();
    }
    if(collect & Collect_TcMalloc) {
#ifdef HAVE_GIBBON_TCMALLOC
        MallocExtension* ext = MallocExtension::instance();
        assert(ext);
        ext->ReleaseFreeMemory();
#else
        Log::error(TRACE_UI_ENGINE, "Gibbon build without tcmalloc support");
#endif
    }
    return result;
}

void GibbonApplication::setSuspended(bool suspend, const std::string &reason)
{
    {
        Log::Message msg(TRACE_UI_ENGINE, Log::Info, suspend ? "suspend" : "resume");
        msg.m_message = StringFormatter::sformat("Gibbon %s queued", suspend ? "Suspend" : "Resume");
        msg.m_tags["reason"] = reason;
        msg.m_flags |= (Log::Message::Critical | Log::Message::SendToAppBoot);
        msg.send();
    }
#ifdef GIBBON_NO_THREADANIMATION
    setSuspendedNow(suspend, reason);
#else
    shared_ptr<GibbonSetSuspendedEvent> suspendEvent(new GibbonSetSuspendedEvent(suspend, reason));
    Animation::sendEvent(suspendEvent);
#endif
}

void GibbonApplication::setSuspendedNow(bool suspend, const std::string &reason)
{
    NTRACE(TRACE_SUSPEND, "GibbonApplication::setSuspendedNow(%d, \"%s\", %d) ",
           suspend, reason.c_str(), mScreen->isRunning());

    if(suspend == mScreen->isRunning()) {
        Widget::SharedPtr root, overlay;
        {
            ScopedMutex _screen_lock(Screen::lock());
            overlay = mScreen->overlay();
            root = mScreen->root();
            mSuspend = suspend;
        }
        Widget::purgeAll(Widget::Purge_Force);
        if(suspend) {
            cleanupInputManager();
            mScreen->stop();
        } else {
            if(SurfaceCache::SharedPtr surfaceCache = getSurfaceCache())
                surfaceCache->clear();
            mScreen->start();
            initInputManager();
        }
        if(shared_ptr<GibbonBridge> bridge = getBridge())
            bridge->suspendChanged(suspend, reason);
    }
    if(!suspend) {
        ScopedMutex lock(Widget::lock());
        const std::list<Widget::SharedPtr> widgets = getRootWidgets();
        for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it)
            (*it)->unsuspend();
        mScreen->update();
    }
    {
        Log::Message msg(TRACE_UI_ENGINE, Log::Info, suspend ? "suspend" : "resume");
        msg.m_message = StringFormatter::sformat("Gibbon %s complete", suspend ? "Suspend" : "Resume");
        msg.m_tags["reason"] = reason;
        msg.m_flags |= (Log::Message::Critical | Log::Message::SendToAppBoot);
        msg.send();
    }
    if(suspend && !GibbonApplication::instance()->hasSuspendHandler())
        GibbonApplication::instance()->suspendComplete();
}

void GibbonApplication::initInputManager()
{
#ifndef GIBBON_INPUT_NONE
    ScopedMutex _lock(mInputManagerMutex);
    if(mInputManager)
        mInputManager->wakeup();
    mInputManager.reset(new InputManager);
#endif
}

void GibbonApplication::cleanupInputManager()
{
#ifndef GIBBON_INPUT_NONE
    shared_ptr<InputManager> manager;
    {
        ScopedMutex _lock(mInputManagerMutex);
        std::swap(manager, mInputManager);
    }
    if (manager)
        manager->wakeup();
#endif
}

bool GibbonApplication::isSuspended() const
{
    ScopedMutex _screenLock(Screen::lock());
    return mSuspend;
}

bool GibbonApplication::runningFpsTimer() const
{
    ScopedMutex _lock(mMutex);
   return (bool)mFpsTimer;
}

void GibbonApplication::startFpsTimer()
{
    shared_ptr<FPSTimer> timer;
    {
        ScopedMutex _lock(mMutex);
        if(!mFpsTimer) {
            mFpsTimer.reset(new FPSTimer((int)mScreen->getVsyncInterval()));
            timer = mFpsTimer;
        }
    }
    if(timer) {
#if defined(GIBBON_NO_THREADANIMATION)
        startTimer(timer);
#else
        Animation::startTimer(timer);
#endif
    }
}

void GibbonApplication::stopFpsTimer()
{
    shared_ptr<FPSTimer> timer;
    {
        ScopedMutex _lock(mMutex);
        std::swap(timer, mFpsTimer);
    }
    if(timer) {
#if defined(GIBBON_NO_THREADANIMATION)
        stopTimer(timer);
#else
        Animation::stopTimer(timer);
#endif
    }
}

static inline std::string megaChange(double now, double prev)
{
    std::string ret;
    if(now > prev)
        ret += '+';
    ret += prettyMegaBytes(now - prev);
    return ret;
}

void GibbonApplication::dumpMalloc(int mode) const
{
    if(mode & MallocSystem) {
#ifdef HAVE_MALLINFO
        static struct mallinfo prevInfo = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        struct mallinfo info = mallinfo();
        Log::warn(TRACE_UI_ENGINE, "mallinfo:\narena: %s (%s)\nordblks: %s (%s)\nsmblks: %s (%s)\n"
                  "hblks: %s (%s)\nhblkhd: %s (%s)\nusmblks: %s (%s)\nfsmblks: %s (%s)\n"
                  "uordblks: %s (%s)\nfordblks: %s (%s)\nkeepcost: %s (%s)",
                  prettyMegaBytes(info.arena).c_str(), prettyMegaBytes(info.arena - prevInfo.arena).c_str(),
                  prettyMegaBytes(info.ordblks).c_str(), prettyMegaBytes(info.ordblks - prevInfo.ordblks).c_str(),
                  prettyMegaBytes(info.smblks).c_str(), prettyMegaBytes(info.smblks - prevInfo.smblks).c_str(),
                  prettyMegaBytes(info.hblks).c_str(), prettyMegaBytes(info.hblks - prevInfo.hblks).c_str(),
                  prettyMegaBytes(info.hblkhd).c_str(), prettyMegaBytes(info.hblkhd - prevInfo.hblkhd).c_str(),
                  prettyMegaBytes(info.usmblks).c_str(), prettyMegaBytes(info.usmblks - prevInfo.usmblks).c_str(),
                  prettyMegaBytes(info.fsmblks).c_str(), prettyMegaBytes(info.fsmblks - prevInfo.fsmblks).c_str(),
                  prettyMegaBytes(info.uordblks).c_str(), prettyMegaBytes(info.uordblks - prevInfo.uordblks).c_str(),
                  prettyMegaBytes(info.fordblks).c_str(), prettyMegaBytes(info.fordblks - prevInfo.fordblks).c_str(),
                  prettyMegaBytes(info.keepcost).c_str(), prettyMegaBytes(info.keepcost - prevInfo.keepcost).c_str());
        prevInfo = info;
#else
        Log::error(TRACE_UI_ENGINE, "No mallinfo on this system");
#endif
    }
    if(mode & MallocJavascript) {
        static ullong prevSystem = 0;
        static ullong prevMetadata = 0;
        static ullong prevFreebytes = 0;
        static size_t prevReturned = 0;
        static size_t prevStringCount = 0;
        static size_t prevStringsSize = 0;
        static size_t prevCommitted = 0;
        static size_t prevDecommitted = 0;

        ullong system, metadata, freebytes;
        size_t stringCount, jsStringsSize, returned, committed, decommitted;
        GibbonApplication::instance()->getScriptEngine()->queryMalloc(&system, &metadata, &freebytes, &returned,
                                                                      &stringCount, &jsStringsSize,
                                                                      &committed, &decommitted);
        const ullong fastMallocCommitted = system - returned;
        const ullong prevFastMallocCommitted = prevSystem - prevReturned;

        enum { Pad = 12 };
        Log::warn(TRACE_UI_ENGINE, "Fastmalloc: committed %s/%s decommitted %s/%s metadata %s/%s free %s/%s strings: %zu/%s%zu %s/%s | OSAllocator: committed %s/%s decommitted %s/%s | Total: committed %s/%s decommitted %s/%s",
                  prettyMegaBytes(fastMallocCommitted).c_str(), megaChange(fastMallocCommitted, prevFastMallocCommitted).c_str(),
                  prettyMegaBytes(returned).c_str(), megaChange(returned, prevReturned).c_str(),
                  prettyMegaBytes(metadata).c_str(), megaChange(metadata, prevMetadata).c_str(),
                  prettyMegaBytes(freebytes).c_str(), megaChange(freebytes, prevFreebytes).c_str(),
                  stringCount, stringCount > prevStringCount ? "+" : "", stringCount - prevStringCount,
                  prettyMegaBytes(jsStringsSize).c_str(), megaChange(jsStringsSize, prevStringsSize).c_str(),
                  prettyMegaBytes(committed).c_str(), megaChange(committed, prevCommitted).c_str(),
                  prettyMegaBytes(decommitted).c_str(), megaChange(decommitted, prevDecommitted).c_str(),
                  prettyMegaBytes(fastMallocCommitted + committed).c_str(),
                  megaChange(fastMallocCommitted + committed, prevFastMallocCommitted + prevCommitted).c_str(),
                  prettyMegaBytes(returned + decommitted).c_str(), megaChange(returned + decommitted, prevReturned + prevDecommitted).c_str());

        prevSystem = system;
        prevMetadata = metadata;
        prevFreebytes = freebytes;
        prevReturned = returned;
        prevStringCount = stringCount;
        prevStringsSize = jsStringsSize;
        prevCommitted = committed;
        prevDecommitted = decommitted;
    }
}

bool GibbonApplication::isRenderThread()
{
#if !defined(GIBBON_NO_THREADANIMATION)
    return Animation::isAnimationThread();
#else
    return Application::isAppThread();
#endif
}

Variant GibbonApplication::send(const Request &request)
{
    if(mServer && mServer->handleNBPSend(request))
        return Variant();

#if defined(GIBBON_BRIDGE_ENQUEUE)
    if(request.flags() & Request::Queued) {
        Animation::postEvent(new QueuedRequestEvent(request));
        return Variant();
    }
#endif

    return NrdApplication::send(request);
}

void GibbonApplication::startAnimation()
{
    if(shared_ptr<SurfaceCache> cache = getSurfaceCache())
        cache->startAnimation();
    if(mServer)
        mServer->startAnimation();
}

void GibbonApplication::endAnimation()
{
    if(shared_ptr<SurfaceCache> cache = getSurfaceCache())
        cache->endAnimation();
    if(mServer)
        mServer->endAnimation();
}

bool GibbonApplication::addSocketizerCondition(const Variant& variant)
{
#ifdef NRDP_HAS_SOCKETIZER
    if(!variant.isStringMap()) {
        Log::error(TRACE_UI_ENGINE, "Variant is not a string map");
        return false;
    }

    const Variant& flags = variant["flags"];
    const Variant& conds = variant["conditions"];
    const std::string host = variant["host"].string();

    bool hasCall;
    const Socketizer::Call call = Socketizer::stringToCall(variant["call"].string(), &hasCall);

    Socketizer::Condition condition;
    if(flags.isArray()) {
        Variant::ArrayIterator it = flags.arrayBegin();
        const Variant::ArrayIterator end = flags.arrayEnd();
        while(it != end) {
            const std::string flag = it->string();
            if(flag == "countprobability") {
                condition.flags |= Socketizer::Condition::CountProbability;
            } else {
                Log::error(TRACE_UI_ENGINE, "Unrecognized flag: %s", flag.c_str());
            }
            ++it;
        }
    }
    if(conds.isStringMap()) {
        Variant::StringMapIterator it = conds.stringMapBegin();
        const Variant::StringMapIterator end = conds.stringMapEnd();
        while(it != end) {
            const std::string key = it->first;
            const Variant val = it->second;
            if(key == "probability") {
                condition.flags |= Socketizer::Condition::Probability;
                condition.probability = val.value<float>();
            } else if(key == "block") {
                condition.flags |= Socketizer::Condition::Block;
                condition.block = val.value<uint32_t>();
            } else if(key == "timeout") {
                condition.flags |= Socketizer::Condition::Timeout;
                condition.timeout = val.value<uint32_t>();
            } else if(key == "errno") {
                condition.flags |= Socketizer::Condition::Errno;
                condition.err = val.value<int32_t>();
            } else if(key == "count") {
                condition.flags |= Socketizer::Condition::Count;
                condition.count = val.value<uint32_t>();
            } else if(key == "filedescriptor") {
                condition.flags |= Socketizer::Condition::FileDescriptor;
                condition.fd = val.value<int32_t>();
            } else if(key == "domain") {
                condition.flags |= Socketizer::Condition::Domain;
                condition.domain = val.value<int32_t>();
            } else if(key == "type") {
                condition.flags |= Socketizer::Condition::Type;
                condition.type = val.value<int32_t>();
            } else if(key == "protocol") {
                condition.flags |= Socketizer::Condition::Protocol;
                condition.protocol = val.value<int32_t>();
            } else if(key == "pattern") {
                condition.flags |= Socketizer::Condition::Pattern;
                condition.pattern.repeat = val["repeat"].value<uint32_t>(0, -1);
                const std::string match = val["match"].string();
                condition.pattern.match.resize(match.size());
                for(size_t i = 0; i < match.size(); ++i) {
                    if(match[i] == '1')
                        condition.pattern.match[i] = true;
                }
            } else {
                Log::error(TRACE_UI_ENGINE, "Unrecognized condition: %s", key.c_str());
            }
            ++it;
        }
    }

    if(!condition.flags) {
        Log::error(TRACE_UI_ENGINE, "No conditions passed to GibbonApplication::addSocketizerCondition");
        return false;
    }

    if(hasCall) {
        if(!host.empty())
            Socketizer::instance()->addCondition(host, call, condition);
        else
            Socketizer::instance()->addCondition(call, condition);
    } else {
        if(!host.empty())
            Socketizer::instance()->addCondition(host, condition);
        else
            Socketizer::instance()->addCondition(condition);
    }
    return true;
#else
    NRDP_UNUSED(variant);
    Log::error(TRACE_UI_ENGINE, "Socketizer support not enabled");
    return false;
#endif
}

bool GibbonApplication::addSocketizerConditions(const Variant& variant)
{
    if(variant.isStringMap())
        return addSocketizerCondition(variant);
    assert(variant.isArray());
    Variant::ArrayIterator it = variant.arrayBegin();
    const Variant::ArrayIterator end = variant.arrayEnd();
    while(it != end) {
        if(!addSocketizerCondition(*it))
            return false;
        ++it;
    }
    return true;
}

class PrefetchRequest : public GibbonResourceRequest
{
public:
    PrefetchRequest(unsigned int flags, const AseUrl &url, Resource::Method method)
        : GibbonResourceRequest(flags, url, method)
    {}
protected:
    virtual void finish()
    {
        NTRACE(TRACE_PREFETCH, "Prefetch request fininshed %s from %s (%d)",
               url().c_str(), Resource::stateToString(state()), resource()->httpStatusCode());
    }
};

void GibbonApplication::loadInjectJS()
{
    const std::string injectStr = readSystemConfiguration(SystemKeys::InjectJS);
    if(!injectStr.empty()) {
        writeSystemConfiguration(SystemKeys::InjectJS, std::string(), NrdApplication::WriteMode_Flush);
        bool ok;
        const Variant injectJS = Variant::fromJSON(injectStr, &ok);
        if(!ok || !injectJS.isArray()) {
            Log::error(TRACE_UI_ENGINE, "Unable to parse persistent inject js: '%s'\n", injectStr.c_str());
            return;
        }

        Variant::ArrayIterator it = injectJS.arrayBegin();
        const Variant::ArrayIterator end = injectJS.arrayEnd();
        while (it != end) {
            if (it->isStringMap()) {
                GibbonConfiguration::addInjectJS(*it);
            } else {
                Log::error(TRACE_UI_ENGINE, "Inject JS is not an object: '%s'", it->value<std::string>().c_str());
            }
            ++it;
        }
    }
}

void GibbonApplication::loadPrefetchUrls(const std::string &prefetchKey,
                                         const Variant &object,
                                         Variant *location)
{
    assert(location);
    const Variant::StringMapIterator list = object.find(prefetchKey);
    if(list == object.stringMapEnd()) {
        NTRACE(TRACE_PREFETCH, "No prefetch data with key: %s", prefetchKey.c_str());
        return;
    }

    const Variant &prefetchList = list->second;

    const ConfigurationOption::Source source = Configuration::source("boot-url");

    for(Variant::ArrayIterator it = prefetchList.arrayBegin(); it != prefetchList.arrayEnd(); ++it) {
        const Variant &data = *it;
        NTRACE(TRACE_PREFETCH, "Processing prefetch: %s", data.toJSON().c_str());
        if(source == ConfigurationOption::Source_Default && data.mapValue<bool>("bootUI")) {
            if(location->isNull()) {
                *location = data;
            } else {
                Log::error(TRACE_PREFETCH, "Multiple bootUI entries: %s and %s",
                           data.toJSON().c_str(), location->toJSON().c_str());
            }
            continue;
        }

        bool ok = false;
        const std::string url = data.mapValue<std::string>("url", &ok);
        if(!ok) {
            Log::error(TRACE_PREFETCH, "Invalid prefetch: %s", data.toJSON().c_str());
            continue;
        }

        Resource::Method method = GibbonResourceManager::methodFromString(data.mapValue<std::string>("requestMethod"));

        unsigned int flags = 0;
        if(data.mapValue<bool>("eval"))
            flags |= GibbonResourceManager::ScriptDownload;
        else if(data.mapValue<bool>("surface"))
            flags |= GibbonResourceManager::Surface;
        if(data.mapValue<bool>("secure"))
            flags |= GibbonResourceManager::Trusted;

        shared_ptr<PrefetchRequest> request(new PrefetchRequest(flags, url, method));

        GibbonResourceManager::setupRequest(request, data);

        if(flags & GibbonResourceManager::Surface) {
            DataBuffer cacheKey = SurfaceCache::prepareCacheKey(request->url(), request->requestHeaders(), DataBuffer());
            NRDP_OBJECTCOUNT_DESCRIBE(cacheKey, "PREFETCH_CACHEKEY:"+request->url().str());
            request->setCacheKey(cacheKey);
        }

        nrdApp()->resourceManager()->sendRequest(request);
    }
}

shared_ptr<GibbonResourceManager> GibbonApplication::resourceManager() const
{
    return static_pointer_cast<GibbonResourceManager>(NrdApplication::resourceManager());
}

#ifdef INSTRUMENTATION_ENABLED
void GibbonApplication::instrumentation(const instrumentation::Event::SharedPtr &inst)
{
    if(GibbonConfiguration::watchdogEnabled()) {
        Serializer serializer;
        serializer << inst->type;
        serializer << inst->name;
        serializer << inst->value;
        serializer << inst->thread;
        gibbon_watchdog_event(GibbonWatchdog_Instrumentation, serializer.data());
    }
    NrdApplication::instrumentation(inst);
}
#endif

bool GibbonApplication::debuggerAttached() const
{
    return getDebugger()->isEnabled();
}

Variant GibbonApplication::versionInfo() const
{
    Variant map = NrdApplication::versionInfo();
    map["ui"]["width"] = GibbonConfiguration::uiWidth();
    map["ui"]["height"] = GibbonConfiguration::uiHeight();
    map["screen"]["width"] = GibbonConfiguration::screenWidth();
    map["screen"]["height"] = GibbonConfiguration::screenHeight();
    return map;
}
