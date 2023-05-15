/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ScriptEngine.h"
#include "GibbonApplication.h"
#include "GibbonResourceManager.h"
#include "GibbonConfiguration.h"

#include <nrdnet/AseUrl.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/UrlEncoder.h>

#include <nrd/AppLog.h>
#include <nrdnet/NetError.h>

#include <gibbon/config.h>

using namespace netflix;
using namespace netflix::gibbon;

namespace netflix {
namespace gibbon {
class ScriptEngineLoadRequest : public GibbonResourceRequest
{
public:
    ScriptEngineLoadRequest(unsigned int flags,
                            const AseUrl &url,
                            unsigned int locationFlags = 0,
                            Resource::Method method = Resource::Method_Get)
        : GibbonResourceRequest(flags, url, method), mLocationFlags(locationFlags)
    {
        if (shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine()) {
            ScopedMutex lock(engine->mMutex);
            engine->mRequests.insert(this);
        }
    }
    virtual void finish()
    {
        shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
        if (!engine || state() == Resource::State_DiskCacheRefresh)
            return;
        assert(engine->isStarted());

        {
            ScopedMutex lock(engine->mMutex);
            std::set<ScriptEngineLoadRequest*>::iterator it = engine->mRequests.find(this);
            if (it == engine->mRequests.end())
                return;
            engine->mRequests.erase(it);
        }

        const Resource::SharedPtr r = resource();
        assert(r);

        const bool included = mLocationFlags & ScriptEngine::Location_Included;
        if (!included) {
            if (r->urls().size() > 1) {
                const std::vector<AseUrl> urls = r->urls();
                for(size_t i = 0; i < urls.size()-1; ++i)
                    Log::warn(TRACE_UI_ENGINE, "UrlChanged(%zu): %s", i, urls[i].c_str());
            }
            ++engine->mLocationCount;
            engine->mLocation = r->url();
            nrdApp()->sendEvent(ApplicationEvent::Event_UrlChanged, r->url().str());
            app()->flushEvents();
        }

        const int statusCode = r->httpStatusCode();
        Variant errorPageParameters;
        Log::Message errorLogParameters(TRACE_UI_ENGINE, Log::Error, "networkerror");
        if (statusCode >= 200 && statusCode < 300) {
            Variant e;
            engine->evaluate(r->responseData(), r->url(), &e);
            if (!e.isNull()) {
                if (!included && mLocationFlags & ScriptEngine::Location_BootFallback) {
                    Log::error(TRACE_UI_ENGINE, "Evaluation failed for prefetch ui url %s (%s). Falling back to BIB",
                               r->url().c_str(), e.toJSON().c_str());
                    gibbonApp()->reload(GibbonApplication::Location_Base);
                    return;
                }
                errorPageParameters["ERROR_CATEGORY"] = "Script";
                errorPageParameters["ERROR_STATUS_CODE"] = -1;
                errorPageParameters["ERROR_DESCRIPTION"] = "Exception was thrown while evaluating JS";
                errorLogParameters.setTag("exception", e.toJSON()); // not pretty-printed
                if (included)
                    Log::error(TRACE_UI_ENGINE, "Included javascript error %s failed: %s\n", r->url().c_str(), e.toJSON().c_str());
            }
        } else {
            if (!included && mLocationFlags & ScriptEngine::Location_BootFallback) {
                Log::error(TRACE_UI_ENGINE, "Network request failed for prefetch ui url %s (%d). Falling back to BIB",
                           r->url().c_str(), statusCode);
                gibbonApp()->reload(GibbonApplication::Location_Base);
                return;
            }

            errorPageParameters["ERROR_CATEGORY"] = "Network";
            errorPageParameters["ERROR_STATUS_CODE"] = r->httpStatusCode();
            errorLogParameters.m_flags |= Log::Message::SendToAppBoot;
            if (included)
                Log::error(TRACE_UI_ENGINE, "Included javascript %s failed, status: %d\n", r->url().c_str(), statusCode);
        }

        if (!errorPageParameters.isNull()) {
            errorPageParameters["ERROR_CODE"] = r->errorCode();
            errorPageParameters["ERROR_GROUP"] = NetError::mapErrorGroup(static_cast<NetError::ErrorCode>(r->errorCode()));
            errorPageParameters["NATIVE_ERROR_CODE"] = r->nativeErrorCode();
            errorPageParameters["ERROR_DESCRIPTION"] = NetError::errorCodeToString(r->errorCode());
            errorPageParameters["ERROR_URL"] = r->url().str();
            errorPageParameters["ERROR_FRAME_URL"] = GibbonApplication::instance()->getLocation().str();
            if (state() == Resource::State_DiskCache)
                errorPageParameters["ERROR_DISK_CACHE"] = true;
            const NetworkMetrics metrics = r->networkMetrics();
            if (metrics.serverIpAddress().valid())
                errorPageParameters["SERVERIP"] = metrics.serverIpAddress().getStrAddr();
            const std::string cname = metrics.cname();
            if (!cname.empty())
                errorPageParameters["CNAME"] = cname;

            errorLogParameters.m_flags |= Log::Message::Critical;
            errorLogParameters.m_message = "Critical Network request failed";
            errorLogParameters.setTag("url", r->url().str());
            errorLogParameters.setTag("errorCode", r->errorCode());
            errorLogParameters.setTag("error", errorPageParameters.toJSON());
            errorLogParameters.send();
            engine->showErrorPage(errorPageParameters);
        } else if(!included) {
            if (shared_ptr<GibbonBridge> bridge = GibbonApplication::instance()->getBridge())
                bridge->cookiesChanged();
        }
    }
private:
    const unsigned int mLocationFlags;
};

class GibbonSetLocationEvent : public GibbonApplication::Event
{
public:
    GibbonSetLocationEvent(const AseUrl &url, unsigned int locationFlags, const DataBuffer &source, const HttpHeaders &headers,
                           const Time &timeout, const Time &connectTimeout, const Time &stallTimeout, const DataBuffer &body,
                           Resource::Method method)
        : mUrl(url), mLocationFlags(locationFlags), mSource(source), mHeaders(headers), mTimeout(timeout),
          mConnectTimeout(connectTimeout), mStallTimeout(stallTimeout), mBody(body), mMethod(method)
    {
    }

    std::string describe() const
    {
        return "GibbonSetLocationEvent " + mUrl.str();
    }
    virtual void eventFired()
    {
        PERFORMANCE_MARKER_SCOPED("setlocationevent.fired");
        assert(mLocationFlags & ScriptEngine::Location_Now);
        GibbonApplication::instance()->getScriptEngine()->setLocation(mUrl, mLocationFlags, mSource, mHeaders, mTimeout,
                                                                      mConnectTimeout, mStallTimeout, mBody, mMethod);
    }
private:
    const AseUrl mUrl;
    const unsigned int mLocationFlags;
    const DataBuffer mSource;
    const HttpHeaders mHeaders;
    const Time mTimeout;
    const Time mConnectTimeout;
    const Time mStallTimeout;
    const DataBuffer mBody;
    const Resource::Method mMethod;
};


}}

ScriptEngine::ScriptEngine()
    : mStarted(false), mSavedState(-1),
#if defined(GIBBON_SCRIPTENGINE_JSC) && defined(NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS)
#ifdef NRDP_OSALLOCATOR_LOGS_ENABLED
      mOSAllocatorChunksMutex(GIBBON_SCRIPTENGINE_OSALLOCATOR_MUTEX, "sOSAllocatorChunksMutex"),
#else
      mOSAllocatorChunksMutex(ZERO_MUTEX, "sOSAllocatorChunksMutex"),
#endif
#endif
      mDebuggerState(State_Running), mMutex(GIBBON_SCRIPTENGINE_MUTEX, "GibbonDebuggerListenersMutex"),
      mLocationCount(0), mSkipMode(0)
{
    init();
    setGarbageCollectMaxBytesPerCycle(GibbonConfiguration::garbageCollectMaxBytesPerCycle());
}

ScriptEngine::~ScriptEngine()
{
    stop();
    shutdown();
}

void ScriptEngine::setLocation(const AseUrl &url,
                               unsigned int locationFlags,
                               const DataBuffer &source,
                               const HttpHeaders &headers,
                               const Time &timeout,
                               const Time &connectTimeout,
                               const Time &stallTimeout,
                               const DataBuffer &body,
                               Resource::Method method)
{
#ifdef DEBUG_SCREEN_IDLE
    Screen::dumpScreenIdleTimer();
#endif
    if (!(locationFlags & Location_Now) || isInBreakpoint()) {
        mSkipMode |= SkipEngine;
        continueFromBreakpoint();
        shared_ptr<Application::Event> event;
        event.reset(new GibbonSetLocationEvent(url, locationFlags|Location_Now, source, headers,
                                               timeout, connectTimeout, stallTimeout, body, method));
        GibbonApplication::instance()->postEvent(event);
        return;
    }
    if ((mSkipMode & (SkipDebugger|SkipDebuggerUntilReload)) == (SkipDebugger|SkipDebuggerUntilReload)) {
        mSkipMode &= ~SkipDebugger;
    }

    const AseUrl location = ResourceManager::fixRelativeUrl(mLocation, url);

    bool error = false;
    if (location.IsEmpty()) {
        error = true;
    } else if (!GibbonResourceManager::isNBP(location)) {
        const std::string path = location.GetPath();
        if (endsWith(path, ".htm", 4) || endsWith(path, ".html", 5))
            error = true;
    }

    if ((locationFlags & Location_ReuseEngine) != Location_ReuseEngine) {
        GibbonApplication::instance()->restart();
        stop();
    }
    nrdApp()->sendEvent(ApplicationEvent::Event_UrlAboutToChange, location.str());
    mLocation = location;
    mHeaders = headers;
    mTimeout = (timeout.val() ? timeout : Time::fromMS(GibbonConfiguration::defaultNetworkTimeout()));
    mStallTimeout = (stallTimeout.val() ? stallTimeout : Time::fromMS(GibbonConfiguration::defaultNetworkStallTimeout()));
    mConnectTimeout = (connectTimeout.val()
                       ? connectTimeout
                       : Time::fromMS(GibbonConfiguration::defaultNetworkConnectTimeout()));

    if(url.GetScheme() == "gibbon") {
        GibbonApplication::instance()->initRemoteClient(url);
        return;
    }

    start();
    if (error) {
        if (locationFlags & Location_BootFallback) {
            Log::error(TRACE_UI_ENGINE, "Invalid prefetch ui set: %s. Falling back to BIB",
                       location.c_str());
            gibbonApp()->reload(GibbonApplication::Location_Base);
            return;
        } else {
            Variant params;
            params["ERROR_CATEGORY"] = "URL";
            params["ERROR_CODE"] = -1;
            params["ERROR_DESCRIPTION"] = "Requesting an HTML page in a JS-only environment";
            params["ERROR_URL"] = url.str();
            params["ERROR_FRAME_URL"] = GibbonApplication::instance()->getLocation().str();
            showErrorPage(params);
        }
    } else {
        injectJS();
        unsigned flags = GibbonResourceManager::Script;
        if (!source.isEmpty())
            flags |= ResourceRequest::Synchronous;

        shared_ptr<ScriptEngineLoadRequest> request(new ScriptEngineLoadRequest(flags, location, locationFlags, method));
        request->setRequestData(body);
        request->setRequestHeaders(headers);
        request->setTimeout(mTimeout);
        request->setConnectTimeout(mConnectTimeout);
        request->setStallTimeout(mStallTimeout);
        if (source.isEmpty()) {
            nrdApp()->resourceManager()->sendRequest(request);
        } else {
            Resource::SharedPtr response = nrdApp()->resourceManager()->createResource(request);
            response->setHttpStatusCode(200);
            response->setResponseData(source);
            request->onComplete(response, Resource::State_MemoryCache);
            request->complete();
            request->clearResponse(); //yuck
        }
    }
}

void ScriptEngine::loadLocalScript(const AseUrl &url)
{
    const unsigned int flags = GibbonResourceManager::Script|GibbonResourceManager::Trusted|ResourceRequest::Synchronous;
    shared_ptr<ScriptEngineLoadRequest> request(new ScriptEngineLoadRequest(flags, url, Location_Included));
    nrdApp()->resourceManager()->sendRequest(request);
    request->complete();
}

void ScriptEngine::start()
{
    if (!mStarted) {
        mStats.clear();
        mStarted = true;
        startEngine();
        loadLocalScript(AseUrl("http://localcontrol.netflix.com/js/NetflixBridge.js"));
        loadLocalScript(AseUrl("http://localcontrol.netflix.com/js/PartnerBridge.js"));
        evaluate("nrdp.setupBackchannel()", AseUrl("ScriptEngine::start"));
    }
    mSkipMode &= ~SkipEngine;
}

void ScriptEngine::injectJS()
{
    const Variant injectUrls = GibbonConfiguration::injectJS();
    const bool isNBP = GibbonResourceManager::isNBP(GibbonApplication::instance()->getLocation());
    for (Variant::ArrayIterator it = injectUrls.arrayBegin(); it != injectUrls.arrayEnd(); ++it) {
        const Variant& inject = *it;
        if (isNBP && inject.mapValue<bool>("ignoreLocal"))
            continue;
        std::string url = inject.mapValue<std::string>("url");
        if (startsWith(url, "javascript:")) {
            url = "data:," + UrlEncoder::encode(url.substr(11));
        } else if (url == "nrdjs") {
            url = "http://localcontrol.netflix.com/js/nrdjs.js";
        }
        loadLocalScript(url);
    }
}

void ScriptEngine::stop()
{
    {
        ScopedMutex lock(mMutex);
        mRequests.clear();
    }
    if (mStarted) {
        GibbonBridge::PendingSyncBlock _pendingSync;
        evaluate("nrdp.shutdownBackchannel()", AseUrl("ScriptEngine::stop"));
        stopEngine();
        mStarted = false;
        mStats.clear();
    }
}

void ScriptEngine::breakpointCallback(const std::string& file, int line, int column, BreakpointReason reason)
{
    const char *reasonName = "";
    switch (reason) {
    case Reason_Breakpoint:
        reasonName = "breakpoint";
        mDebuggerState = State_Stopped;
        break;
    case Reason_Exception:
        reasonName = "exception";
        mDebuggerState = State_Exception;
        break;
    case Reason_Step:
        reasonName = "step";
        mDebuggerState = State_Stopped;
        break;
    case Reason_CallstackSize:
        Log::error(TRACE_UI_ENGINE, "Maximum call stack size exceeded");
        reasonName = "callstack exceeded";
        mDebuggerState = State_Exception;
        break;
    }
    Log::warn(TRACE_UI_ENGINE, "Got %s at %s %d:%d", reasonName, file.c_str(), line, column);
    if (mSkipMode & (SkipDebugger|SkipEngine)) {
        Log::warn(TRACE_UI_ENGINE, "Skipping");
        return;
    }
    sendDebugEvent(DebuggerListener::State);
    GibbonApplication::instance()->getBridge()->stopAllTimers();
    GibbonApplication::instance()->reenter();
    mDebuggerState = State_Running;
    sendDebugEvent(DebuggerListener::State);
}

void ScriptEngine::addDebuggerListener(const shared_ptr<DebuggerListener> &listener)
{
    assert(listener);
    ScopedMutex lock(mMutex);
    mDebuggerListeners.insert(listener);
}

void ScriptEngine::removeDebuggerListener(const shared_ptr<DebuggerListener> &listener)
{
    assert(listener);
    ScopedMutex lock(mMutex);
    mDebuggerListeners.erase(listener);
}

void ScriptEngine::sendDebugEvent(DebuggerListener::Type type)
{
    std::vector<DebuggerListener::SharedPtr> locked;
    {
        ScopedMutex lock(mMutex);
        mDebuggerListeners.lock(locked);
    }
    for (std::vector<DebuggerListener::SharedPtr>::const_iterator it = locked.begin(); it != locked.end(); ++it) {
        (*it)->onChanged(type);
    }
}

ScriptEngine::DebuggerState ScriptEngine::debuggerState() const
{
    return mDebuggerState;
}

void ScriptEngine::showErrorPage(const Variant &params)
{
    if(GibbonApplication::instance()->getLocation() == "http://localcontrol.netflix.com/js/error.js") {
        Log::error(TRACE_UI_ENGINE, "Already on error page!");
        return;
    }
    if (!GibbonApplication::instance()->getDebugger()->isEnabled()) {
        const AseUrl errorUrl("http://localcontrol.netflix.com/js/error.js");
        const DataBuffer errorPageContent = NrdApplication::instance()->resourceContent("js/error.js", params);
        setLocation(errorUrl, 0, errorPageContent);
    } else {
        Log::error(TRACE_UI_ENGINE, "Debugger: no error page");
    }
}

void ScriptEngine::setSkipAllPauses(bool skip, bool untilReload)
{
    if (skip) {
        mSkipMode |= SkipDebugger;
    } else {
        mSkipMode &= ~SkipDebugger;
    }
    if (untilReload) {
        mSkipMode |= SkipDebuggerUntilReload;
    } else {
        mSkipMode &= ~SkipDebuggerUntilReload;
    }
}
