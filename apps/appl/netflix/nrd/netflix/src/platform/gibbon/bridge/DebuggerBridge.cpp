/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for ny
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DebuggerBridge.h"
#include "GibbonApplication.h"
#include "GibbonResourceManager.h"
#include "SurfaceCache.h"
#include "WidgetBridge.h"
#include "Screen.h"
#include "Debug.h"

#include <nrdbase/Version.h>
#include <nrdbase/Base64.h>
#include <nrdnet/IAsyncHttpLib.h>

#include <sstream>

using namespace netflix::gibbon;
using namespace netflix;

#define DebuggerBridgePropertyList(Property)    \
    Property(breakpoints, NfObject::ReadOnly)   \
    Property(frame, NfObject::ReadWrite)        \
    Property(identifiers, NfObject::ReadOnly)   \
    Property(scopeChain, NfObject::ReadOnly)    \
    Property(sources, NfObject::ReadOnly)       \
    Property(stack, NfObject::ReadOnly)         \
    Property(state, NfObject::ReadOnly)

PropertyTable(DebuggerBridge, DebuggerBridgePropertyList, properties)

#define DebuggerBridgeMethodList(Method)        \
    Method(addBreakpoint)                       \
    Method(addDOMBreakpoint)                    \
    Method(clearNetworkCache)                   \
    Method(clearNetworkCookies)                 \
    Method(continueFromBreakpoint)              \
    Method(disableDebug)                        \
    Method(disableNetwork)                      \
    Method(disableProfile)                      \
    Method(enableDebug)                         \
    Method(enableNetwork)                       \
    Method(enableProfile)                       \
    Method(enableRender)                        \
    Method(evaluate)                            \
    Method(evaluateOnCallFrame)                 \
    Method(finish)                              \
    Method(getProperties)                       \
    Method(ignoreExceptions)                    \
    Method(overlay)                             \
    Method(pause)                               \
    Method(reload)                              \
    Method(removeBreakpoint)                    \
    Method(removeDOMBreakpoint)                 \
    Method(resource)                            \
    Method(scene)                               \
    Method(setSkipAllPauses)                    \
    Method(startCPUProfiler)                    \
    Method(stepInto)                            \
    Method(stepOver)                            \
    Method(stopCPUProfiler)

MethodTable(DebuggerBridge, DebuggerBridgeMethodList);

namespace netflix {
namespace gibbon {

static inline Variant jsMemoryStats()
{
    Variant result;
    assert(GibbonApplication::instance()->getScriptEngine());
    ScriptEngine::Stats stats = GibbonApplication::instance()->getScriptEngine()->stats();
    result["heap"] = static_cast<int>(stats.heapSize);
    result["heapCapacity"] = static_cast<int>(stats.heapCapacity);
    result["globalObjectCount"] = static_cast<int>(stats.globalObjectCount);
    result["protectedObjectCount"] = static_cast<int>(stats.protectedObjectCount);
    result["protectedGlobalObjectCount"] = static_cast<int>(stats.protectedGlobalObjectCount);
    result["objectCount"] = static_cast<int>(stats.objectCount);
    result["fastMalloc"] = static_cast<int>(stats.fastMallocSystem);
    result["fastMalloc_free"] = static_cast<int>(stats.fastMallocFreebytes);
    result["osAllocatorCommitted"] = static_cast<int>(stats.osAllocatorCommitted);
    return result;
}

#ifdef INSTRUMENTATION_ENABLED
class DebuggerInstrumentation : public instrumentation::Listener
{
public:
    DebuggerInstrumentation(DebuggerBridge *debugger) : mDebugger(debugger) { }

    virtual void receive(const Variant &inst) {
        mDebugger->processInstrumentation(inst);
    }
protected:
    DebuggerBridge *mDebugger;
};
#endif

class DebuggerLogEvent : public GibbonApplication::Event
{
public:
    typedef shared_ptr<DebuggerLogEvent> SharedPtr;

    DebuggerLogEvent(const shared_ptr<NfObject> &bridge, const Log::Message &message)
        : mBridge(bridge), mMessage(message)
    {}

    std::string describe() const
    {
        return "DebuggerLogEvent";
    }

    virtual void eventFired()
    {
        PERFORMANCE_MARKER_SCOPED("debuggerlogevent.fired");
        if(shared_ptr<NfObject> bridge = mBridge.lock()) {
            Variant data;
            //data["jsc"] = jsMemoryStats();
            data["traceArea"] = mMessage.m_traceArea->getName();
            if(mMessage.m_logLevel == Log::Trace)
                data["level"] = "trace";
            else if(mMessage.m_logLevel == Log::Debug)
                data["level"] = "debug";
            else if(mMessage.m_logLevel == Log::Info)
                data["level"] = "info";
            else if(mMessage.m_logLevel == Log::Warn)
                data["level"] = "warn";
            else if(mMessage.m_logLevel == Log::Error)
                data["level"] = "error";
            else if(mMessage.m_logLevel == Log::Fatal)
                data["level"] = "fatal";
            data["tags"] = mMessage.m_tags;
            data["callstack"] = mMessage.m_jsStack;
            data["message"] = mMessage.m_message;
            bridge->sendEvent("LogEvent", data);
        }
    }
protected:
    weak_ptr<NfObject> mBridge;
    Log::Message mMessage;
};

class DebuggerInstrumentationEvent : public GibbonApplication::Event
{
public:
    typedef shared_ptr<DebuggerInstrumentationEvent> SharedPtr;

    DebuggerInstrumentationEvent(shared_ptr<NfObject> bridge, const Variant &instrumentation) : mBridge(bridge), mInstrumentation(instrumentation) {}

    std::string describe() const
    {
        return "DebuggerInstrumentationEvent";
    }

    virtual void eventFired()
    {
        PERFORMANCE_MARKER_SCOPED("debuggerinstrumentationevent.fired");
        if(shared_ptr<NfObject> bridge = mBridge.lock()) {
            Variant data;
            if(GibbonApplication *app = GibbonApplication::instance()) {
                Variant counters;
                counters["jsc"] = jsMemoryStats();
                counters["surfaceCacheUsed"] = app->getSurfaceCache()->size();
                counters["views"] = app->getWidgetCount();
                data["counters"] = counters;
            }
            data["instrumentation"] = mInstrumentation;
            bridge->sendEvent("InstrumentationEvent", data);
        }
    }
protected:
    weak_ptr<NfObject> mBridge;
    Variant mInstrumentation;
};

static inline std::string fromDOMType(const std::string& type)
{
    if(type == "attribute-modified")
        return "1";
    else if(type == "subtree-modified")
        return "2";
    else if(type == "node-removed")
        return "3";
    Log::error(TRACE_UI_ENGINE, "Invalid DOM breakpoint type: '%s'", type.c_str());
    return std::string();
}

static inline Variant variantToObject(const std::string& name, const Variant& data, int frame)
{
    Variant result, value;
    result["name"] = name;
    switch (data.type()) {
    case Variant::Type_String:
    case Variant::Type_DataBuffer:
        value["type"] = "string";
        value["description"] = data.value<std::string>();
        break;
    case Variant::Type_StringMap: {
        value["type"] = "object";
        std::ostringstream objectId;
        objectId << name << ":" << frame;
        value["objectId"] = objectId.str();
        value["description"] = "Object";
        break; }
    case Variant::Type_Array: {
        value["type"] = "object";
        value["subtype"] = "array";
        std::ostringstream objectId;
        objectId << name << ":" << frame;
        value["objectId"] = objectId.str();
        std::ostringstream descr;
        descr << "Array[" << data.size() << "]";
        value["description"] = descr.str();
        break; }
    case Variant::Type_Double: {
        value["type"] = "number";
        std::ostringstream descr;
        descr << data.value<double>();
        value["description"] = descr.str();
        break; }
    case Variant::Type_Boolean:
        value["type"] = "boolean";
        value["description"] = (data.value<bool>() ? "true" : "false");
        break;
    case Variant::Type_Null:
        value["type"] = "undefined";
        break;
    default:
        Log::error(TRACE_UI_ENGINE, "Invalid variant type: %d", data.type());
        assert(false);
        break;
    }
    result["value"] = value;
    return result;
}

class DebuggerEvent : public GibbonApplication::Event
{
public:
    typedef shared_ptr<DebuggerEvent> SharedPtr;

    enum ObjectId { Id_Unknown, Id_Completion, Id_Console, Id_Watch };

    DebuggerEvent()
        : type(-1)
    {}
    std::string describe() const
    {
        return "DebuggerEvent";
    }
    virtual void eventFired()
    {
        PERFORMANCE_MARKER_SCOPED("debuggerevent.fired");
        ScriptEngine::SharedPtr engine = scriptEngine.lock();
        if(!engine)
            return;
        switch (type) {
        case Methods::DebuggerBridge::addBreakpoint: {
            const int bid = engine->setBreakpoint(get("url").string(), get("line").integer());
            const std::string condition = get("condition").string();
            if(!condition.empty())
                engine->setCondition(bid, condition);
            break; }
        case Methods::DebuggerBridge::continueFromBreakpoint:
            engine->continueFromBreakpoint();
            break;
        case Methods::DebuggerBridge::addDOMBreakpoint: {
            const std::string widget = get("widget").value<std::string>();
            const std::string type = get("type").string();
            engine->attachDebugger();
            engine->evaluate("nrdp.gibbon._addDOMBreakpoint(" + widget + ", " + fromDOMType(type) + ")", std::string());
            break; }
        case Methods::DebuggerBridge::removeDOMBreakpoint: {
            const std::string widget = get("widget").value<std::string>();
            const std::string type = get("type").string();
            engine->evaluate("nrdp.gibbon._removeDOMBreakpoint(" + widget + ", " + fromDOMType(type) + ")", std::string());
            engine->detachDebugger();
            break; }
        case Methods::DebuggerBridge::pause:
            engine->pauseOnNextStatement();
            break;
        case Methods::DebuggerBridge::evaluate: {
            const std::string expression = get("expression").string();

            Variant event;
            event["expression"] = expression;
            const int id = get("id").integer();
            switch (id) {
            case Id_Console:
                if(expression[0] == '/') {
                    shared_ptr<Console> console = app()->console();
                    if (console)
                        console->handleCommand(expression, Console::Console_None);
                    break;
                }
                // fall through
            case Id_Watch:
            case Id_Completion: {
                Variant exception, result;
                const int wasIgnored = engine->ignoreExceptions(0);
                engine->evaluate(expression, std::string(), &exception, &result, 1);
                engine->ignoreExceptions(wasIgnored);
                event["result"] = result;
                if(id == Id_Completion && !exception.isNull())
                    event["error"] = exception;
                break; }
            default:
                event["error"] = "Unknown object group";
                break;
            }
            if(shared_ptr<NfObject> b = bridge.lock())
                b->sendEvent("Evaluate", event);
            break; }
        case Methods::DebuggerBridge::evaluateOnCallFrame: {
            const int frame = get("frame").integer();
            const std::string expression = get("expression").string();

            const int current = engine->currentFrame();
            engine->setCurrentFrame(frame);
            Variant exception, evaluated;
            engine->evaluate(expression, std::string(), &exception, &evaluated, 1);
            engine->setCurrentFrame(current);
            Variant result;
            result["wasThrown"] = !exception.isNull();
            result["result"] = evaluated;
            Variant event;
            event["error"] = exception;
            event["result"] = result;
            Variant ident;
            ident["frame"] = frame;
            ident["expression"] = expression;
            event["ident"] = ident;
            if(shared_ptr<NfObject> b = bridge.lock())
                b->sendEvent("EvaluateOnCallFrame", event);
            break; }
        case Methods::DebuggerBridge::getProperties: {
            std::string objectId = get("objectId").string();

            Variant result;
            result["objectId"] = objectId;

            size_t c = objectId.rfind(':');
            int frameNo = 1;
            std::string frameStr;
            if(c != std::string::npos) {
                frameStr = objectId.substr(c + 1);
                frameNo = std::max(atoi(frameStr.c_str()), 1);
                objectId = objectId.substr(0, c);
            }
            //const bool ownProperties = get("ownProperties").boolean();

            Variant evaluated;
            if(objectId.substr(0, 6) == "scope+") {
                const int scopeNo = atoi(objectId.substr(6).c_str());
                if(shared_ptr<DebuggerBridge> b = static_pointer_cast<DebuggerBridge>(bridge.lock())) {
                    --frameNo;
                    if(frameNo >= 0 && frameNo < static_cast<int>(b->mScopeChain.size())
                       && scopeNo >= 0 && scopeNo < static_cast<int>(b->mScopeChain[frameNo].size())) {
                        b->resolveChain(frameNo, scopeNo);
                        const Variant& chain = b->mScopeChain[frameNo][scopeNo];
                        Variant::StringMapIterator key = chain.stringMapBegin();
                        const Variant::StringMapIterator end = chain.stringMapEnd();
                        while(key != end) {
                            evaluated.push_back(variantToObject(key->first, key->second, frameNo + 1));
                            ++key;
                        }
                    }
                }
            } else {
                const int current = engine->currentFrame();
                if(current != frameNo)
                    engine->setCurrentFrame(frameNo);

                Variant exception;
                std::ostringstream expr;
                expr << "nrdp.gibbon._debugObject(" << objectId << ", \"" << objectId << "\", ";
                if(frameStr.empty()) {
                    expr << current << ")";
                } else {
                    expr << frameStr << ")";
                }
                engine->evaluate(expr.str(), std::string(), &exception, &evaluated, 3);

                if(current != frameNo)
                    engine->setCurrentFrame(current);
            }

            result["data"] = evaluated;

            //printf("result %s\n", evaluated.toJSON().c_str());
            //printf("exception %s\n", exception.toJSON().c_str());

            if(shared_ptr<NfObject> b = bridge.lock())
                b->sendEvent("GetProperties", result);

            break; }
        case Methods::DebuggerBridge::ignoreExceptions:
            engine->ignoreExceptions(get("ignore").boolean());
            break;
        case Methods::DebuggerBridge::finish:
            engine->finish();
            break;
        case Methods::DebuggerBridge::removeBreakpoint:
            engine->removeBreakpoint(get("id").integer());
            break;
        case Methods::DebuggerBridge::stepInto:
            engine->stepInto();
            break;
        case Methods::DebuggerBridge::stepOver:
            engine->stepOver();
            break;
        case Methods::DebuggerBridge::startCPUProfiler:
            engine->startCPUProfiler(get("profileId").integer());
            break;
        case Methods::DebuggerBridge::stopCPUProfiler: {
            int id = get("profileId").integer();
            Variant result;
            result["data"]["profileId"] = id;
            result["data"]["json"] = engine->stopCPUProfiler(id);
            if(shared_ptr<NfObject> b = bridge.lock())
                b->sendEvent("CPUProfile", result);
            break; }
        case Methods::DebuggerBridge::setSkipAllPauses:
            engine->setSkipAllPauses(get("skipped").boolean(), get("untilReload").boolean());
            break;
        default:
            Log::error(TRACE_UI_ENGINE, "Invalid DebuggerEvent: %d", type);
            return;
        }
    }

    static ObjectId toObjectId(const std::string& name)
    {
        // ### make this a map?
        if(name == "completion")
            return Id_Completion;
        if(name == "console")
            return Id_Console;
        if(name == "watch-group")
            return Id_Watch;
        return Id_Unknown;
    }

    void set(const std::string& key, Variant value)
    {
        data[key] = value;
    }

    Variant get(const std::string& key) const
    {
        Variant::StringMapIterator it = data.find(key);
        if(it == data.end())
            return Variant();
        return it->second;
    }

    int type;
    ScriptEngine::WeakPtr scriptEngine;
    weak_ptr<NfObject> bridge;
    std::map<std::string, Variant> data;
};

}}

DebuggerBridge::DebuggerBridge()
    : NfObject("debugger",
               Properties::DebuggerBridge::properties, Properties::DebuggerBridge::PropertyCount,
               Methods::DebuggerBridge::methods, Methods::DebuggerBridge::MethodCount),
      mFrame(0)
{
    mEnableDebug = mSendProfile = mSendNetwork = 0;
    mDebuggerState = scriptEngine()->debuggerState();
}

void DebuggerBridge::init()
{
    if(!mSink) {
        mSink.reset(new LogSinkForwarder<DebuggerBridge>(static_pointer_cast<DebuggerBridge>(shared_from_this())));
        LogSink::add(mSink);
        mSink->welcome();
    }
    if(!mListener && scriptEngine()) {
        mListener.reset(new Listener(*this));
        scriptEngine()->addDebuggerListener(mListener);
    }
}

void DebuggerBridge::cleanup()
{
    if(mListener && scriptEngine()) {
        scriptEngine()->removeDebuggerListener(mListener);
        mListener.reset();
    }
    if(mSink) {
        LogSink::remove(mSink);
        mSink.reset();
    }
}

void DebuggerBridge::resolveChain(int frameNo, int scopeNo)
{
    Variant& chain = mScopeChain[frameNo][scopeNo];
    Variant::StringMapIterator key = chain.stringMapBegin();
    const Variant::StringMapIterator end = chain.stringMapEnd();
    while(key != end) {
        chain[key->first] = scriptEngine()->evaluateIdentifier(key->first, frameNo + 1);
        ++key;
    }
}

bool DebuggerBridge::getProperty(int index, Variant *result) const
{
    assert(result);
    switch (index) {
    case Properties::DebuggerBridge::breakpoints: {
        Variant &breakpoints = *result;
        breakpoints.clear();
        for(std::vector<ScriptEngine::Breakpoint>::const_iterator it = mBreakpoints.begin(); it != mBreakpoints.end(); ++it) {
            Variant b;
            b["url"] = it->url.str();
            if(!it->condition.empty())
                b["condition"] = it->condition;
            b["line"] = it->line;
            breakpoints.push_back(b);
        }
        break; }
    case Properties::DebuggerBridge::frame:
        *result = mFrame;
        break;
    case Properties::DebuggerBridge::identifiers:
        *result = mIdentifiers;
        break;
    case Properties::DebuggerBridge::sources: {
        Variant &res = *result;
        res.clear();
        std::map<AseUrl, std::string> sources = scriptEngine()->getResourceUrls();
        std::map<AseUrl, std::string>::const_iterator it = sources.begin();
        const std::map<AseUrl, std::string>::const_iterator end = sources.end();
        while(it != end) {
            res[it->first.str()] = it->second;
            ++it;
        }
        break; }
    case Properties::DebuggerBridge::scopeChain: {
        Variant& scopeCounts = *result;
        scopeCounts.clear();
        for(unsigned int i = 0; i < mScopeChain.size(); ++i) {
            scopeCounts.push_back(mScopeChain[i].size());
        }
        break; }
    case Properties::DebuggerBridge::stack: {
        Variant &stack = *result;
        stack.clear();
        for(std::vector<ScriptEngine::StackFrame>::const_iterator it = mFrames.begin(); it != mFrames.end(); ++it) {
            Variant b;
            b["url"] = it->url.str();
            if(!it->function.empty())
                b["function"] = it->function;
            b["line"] = it->line;
            b["column"] = it->column;
            stack.push_back(b);
        }
        break; }
    case Properties::DebuggerBridge::state:
        switch (mDebuggerState) {
        case ScriptEngine::State_Running:
            *result = "running";
            break;
        case ScriptEngine::State_Stopped:
            *result = "stopped";
            break;
        case ScriptEngine::State_Exception:
            *result = "exception";
            break;
        }
        break;
    default:
        return false;
    }
    return true;
}

bool DebuggerBridge::setProperty(int index, const Variant &value)
{
    switch (index) {
    case Properties::DebuggerBridge::frame: {
        const unsigned int frame = value.value<unsigned int>();
        if(frame <= 0 || frame > mFrames.size()) {
            Log::error(TRACE_UI_SCRIPT, "Invalid stack frame %d", frame);
            return false;
        }
        mFrame = frame;
        scriptEngine()->setCurrentFrame(frame);
        propertyUpdated(Properties::DebuggerBridge::frame);
        break; }
    default:
        return false;
    }
    return true;
}

Variant DebuggerBridge::invoke(int method, const Variant &data)
{
    bool ok;
    DebuggerEvent::SharedPtr event;
    switch (method) {
    case Methods::DebuggerBridge::addBreakpoint: {
        const std::string url = data.mapValue<std::string>("url");
        if(url.empty()) {
            invalidArgumentError(method, "url");
            return false;
        }
        const std::string condition = data.mapValue<std::string>("condition");
        const int line = data.mapValue<int>("line", &ok);
        if(!ok) {
            invalidArgumentError(method, "line");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("line", line);
        event->set("url", url);
        event->set("condition", condition);
        break; }
    case Methods::DebuggerBridge::addDOMBreakpoint:
    case Methods::DebuggerBridge::removeDOMBreakpoint:
        event.reset(new DebuggerEvent);
        event->set("widget", data["widget"].value<int>());
        event->set("type", data["type"].value<std::string>());
        break;
    case Methods::DebuggerBridge::pause:
        event.reset(new DebuggerEvent);
        event->bridge = shared_from_this();
        break;
    case Methods::DebuggerBridge::overlay: {
        ScopedMutex locker(Widget::lock());
        Rect rect;
        Color color(0, 0, 0, 0);
        if(data.contains("replace"))
            GibbonApplication::instance()->getScreen()->Screen::createOverlay(Rect());
        if(data.contains("widget")) {
            if(Widget::SharedPtr widget = GibbonApplication::instance()->getWidget(data["widget"].value<int>()))
                rect = widget->getGlobalRect();
        }
        if(data.contains("x"))
            rect.x = data["x"].value<int>();
        if(data.contains("y"))
            rect.y = data["y"].value<int>();
        if(data.contains("width"))
            rect.width = data["width"].value<int>();
        if(data.contains("height"))
            rect.height = data["height"].value<int>();
        if(data.contains("color")) {
            color.r = data["color"]["r"].value<int>(0, 0);
            color.g = data["color"]["g"].value<int>(0, 0);
            color.b = data["color"]["b"].value<int>(0, 0);
            color.a = data["color"]["a"].value<int>(0, 255);
        }
        GibbonApplication::instance()->getScreen()->Screen::createOverlay(rect);
        break; }
    case Methods::DebuggerBridge::removeBreakpoint: {
        const unsigned id = data.mapValue<unsigned>("id");
        if(!id || id > mBreakpoints.size()) {
            invalidArgumentError(method, "id");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("id", id);
        break; }
    case Methods::DebuggerBridge::resource: {
        const std::string url = data.mapValue<std::string>("url");
        if(url.empty()) {
            invalidArgumentError(method, "url");
            return false;
        }
        shared_ptr<GibbonResourceRequest> req(new GibbonResourceRequest(ResourceRequest::Synchronous,
                                                                        url, Resource::Method_Get));
        nrdApp()->resourceManager()->sendRequest(req);
        Resource::SharedPtr resource = req->resource();
        Variant data;
        data["url"] = url;
        if(!resource) {
            data["error"] = true;
        } else {
            data["contentType"] = ResourceManager::findResponseHeader("content-type", resource->responseHeaders());
            if(!resource->responseData().isEmpty())
                data["data"] = resource->responseData().toBase64();
        }
        sendEvent("ResourceResponse", data);
        break; }
    case Methods::DebuggerBridge::evaluate: {
        const std::string expression = data.mapValue<std::string>("expression");
        if(expression.empty()) {
            invalidArgumentError(method, "expression");
            return false;
        }
        const std::string group = data.mapValue<std::string>("group");
        if(group.empty()) {
            invalidArgumentError(method, "group");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("expression", expression);
        event->set("id", DebuggerEvent::toObjectId(group));
        event->bridge = shared_from_this();
        break; }
    case Methods::DebuggerBridge::evaluateOnCallFrame: {
        if(mDebuggerState == ScriptEngine::State_Running) {
            Log::error(TRACE_UI_ENGINE, "Methods::DebuggerBridge::evaluateOnCallFrame called when not sitting in a breakpoint");
            return false;
        }
        const int frameNo = data.mapValue<int>("frame", &ok);
        if(!ok) {
            invalidArgumentError(method, "frame");
            return false;
        }
        const std::string expression = data.mapValue<std::string>("expression");
        if(expression.empty()) {
            invalidArgumentError(method, "expression");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("frame", frameNo);
        event->set("expression", expression);
        event->bridge = shared_from_this();
        break; }
    case Methods::DebuggerBridge::startCPUProfiler:
    case Methods::DebuggerBridge::stopCPUProfiler: {
        const int profileId = data.mapValue<int>("profileId", &ok);
        if(!ok) {
            invalidArgumentError(method, "profileId");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("profileId", profileId);
        event->bridge = shared_from_this();
        break; }
    case Methods::DebuggerBridge::setSkipAllPauses: {
        const bool skipped = data.mapValue<bool>("skipped", &ok);
        if(!ok) {
            invalidArgumentError(method, "skipped");
            return false;
        }
        const bool untilReload = data.mapValue<bool>("untilReload", &ok);
        if(!ok) {
            invalidArgumentError(method, "untilReload");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("skipped", skipped);
        event->set("untilReload", untilReload);
        event->bridge = shared_from_this();
        break; }
    case Methods::DebuggerBridge::enableRender: {
        const Variant types = data["types"];
        for(int i = 0; i < types.size(); ++i) {
            if(types[i].isString())
                Debug::setProperty(types[i].string());
            else if(types[i].isStringMap())
                Debug::setProperty(types[i]["name"].string(), types[i]["value"]);
        }
        break; }
    case Methods::DebuggerBridge::enableProfile:
        if(!mSendProfile++) {
#ifdef INSTRUMENTATION_ENABLED
            assert(!mInstrumenation);
            mInstrumenation.reset(new DebuggerInstrumentation(this));
            instrumentation::switched(true);
            instrumentation::enable();
            instrumentation::addListener(mInstrumenation);
#endif
        }
        break;
    case Methods::DebuggerBridge::enableDebug:
        if(!mEnableDebug++)
            init();
        sendEvent("EnableDebugResponse", NRDP_VERSION_STRING);
        break;
    case Methods::DebuggerBridge::disableDebug:
        if(!--mEnableDebug)
            cleanup();
        break;
    case Methods::DebuggerBridge::disableProfile:
        if(!--mSendProfile) {
#ifdef INSTRUMENTATION_ENABLED
            assert(mInstrumenation);
            instrumentation::removeListener(mInstrumenation);
            mInstrumenation.reset();
#endif
        }
        break;
    case Methods::DebuggerBridge::enableNetwork:
        ++mSendNetwork;
        break;
    case Methods::DebuggerBridge::disableNetwork:
        --mSendNetwork;
        break;
    case Methods::DebuggerBridge::clearNetworkCookies:
        GibbonApplication::instance()->resourceManager()->cookieManager()->clear();
        break;
    case Methods::DebuggerBridge::clearNetworkCache:
        GibbonApplication::instance()->getSurfaceCache()->clear();
        nrdApp()->resourceManager()->clear();
        break;
    case Methods::DebuggerBridge::reload:
        GibbonApplication::instance()->reload(GibbonApplication::Location_Current);
        break;
    case Methods::DebuggerBridge::scene: {
        ScopedMutex locker(Widget::lock());
        Variant scene;
        const int id = data.mapValue<int>("id", &ok);
        if(ok)
            scene["id"] = id;
        if(Widget::SharedPtr root = GibbonApplication::instance()->getScreen()->root()) {
            shared_ptr<WidgetBridge> bridge = GibbonApplication::instance()->getBridge()->getWidget(root->getID());
            if(bridge)
                scene["root"] = bridge->dump(WidgetBridge::Dump_Recursive|WidgetBridge::Dump_All);
        }
        sendEvent("SceneResponse", scene);
        break; }
    case Methods::DebuggerBridge::ignoreExceptions:
        event.reset(new DebuggerEvent);
        event->set("ignore", data.mapValue<int>("ignore", 0, 0) ? true : false);
        break;
    case Methods::DebuggerBridge::getProperties: {
        const std::string objectId = data.mapValue<std::string>("objectId", &ok);
        if(!ok) {
            invalidArgumentError(method, "objectId");
            return false;
        }
        const bool ownProperties = data.mapValue<bool>("ownProperties", &ok);
        if(!ok) {
            invalidArgumentError(method, "ownProperties");
            return false;
        }
        event.reset(new DebuggerEvent);
        event->set("objectId", objectId);
        event->set("ownProperties", ownProperties);
        event->bridge = shared_from_this();
        break; }
    case Methods::DebuggerBridge::continueFromBreakpoint:
    case Methods::DebuggerBridge::finish:
    case Methods::DebuggerBridge::stepInto:
    case Methods::DebuggerBridge::stepOver:
        event.reset(new DebuggerEvent);
        break;
    }
    if(!event)
        return false;

    event->scriptEngine = scriptEngine();
    event->type = method;
    GibbonApplication::instance()->postEvent(event);
    return true;
}

ScriptEngine::SharedPtr DebuggerBridge::scriptEngine() const
{
    return GibbonApplication::instance()->getScriptEngine();
}

void DebuggerBridge::onChanged(ScriptEngine::DebuggerListener::Type type)
{
    if(!mEnableDebug)
        return;

    ScriptEngine::SharedPtr engine = scriptEngine();
    assert(engine);
    switch (type) {
    case ScriptEngine::DebuggerListener::Breakpoints:
        engine->listBreakpoints(mBreakpoints);
        propertyUpdated(Properties::DebuggerBridge::breakpoints);
        break;
    case ScriptEngine::DebuggerListener::State:
        switch ((mDebuggerState = engine->debuggerState())) {
        case ScriptEngine::State_Running:
            mIdentifiers.clear();
            mFrame = 0;
            mFrames.clear();
            mScopeChain.clear();
            break;
        case ScriptEngine::State_Stopped:
        case ScriptEngine::State_Exception:
            mFrame = engine->currentFrame();
            engine->stackTrace(mFrames);
            mScopeChain.clear();
            mScopeChain.resize(mFrames.size());
            for(unsigned int i = 0; i < mFrames.size(); ++i) {
                mScopeChain[i].resize(2);
                engine->listIdentifiers(mScopeChain[i][0], ScriptEngine::Global, i + 1, /*resolve*/false);
                engine->listIdentifiers(mScopeChain[i][1], ScriptEngine::Local, i + 1, /*resolve*/false);
                int closures = 0;
                bool ok;
                do {
                    Variant closure;
                    ok = engine->listIdentifiers(closure, ++closures, i + 1, /*resolve*/false);
                    if(ok)
                        mScopeChain[i].push_back(closure);
                } while(ok);
            }
            engine->listIdentifiers(mIdentifiers);
            break;
        }
        const int properties[] = {
            Properties::DebuggerBridge::frame,
            Properties::DebuggerBridge::scopeChain,
            Properties::DebuggerBridge::identifiers,
            Properties::DebuggerBridge::stack,
            Properties::DebuggerBridge::state
        };
        propertiesUpdated(properties, sizeof(properties) / sizeof(properties[0]));
        break;
    }
}

void DebuggerBridge::updateLocation(const AseUrl &url)
{
    if(!mEnableDebug)
        return;
    NRDP_UNUSED(url);
    propertyUpdated(Properties::DebuggerBridge::sources);
}

void DebuggerBridge::beginRequest(const ResourceRequest::SharedPtr &request)
{
    if(!mEnableDebug)
        return;

    if(mSendNetwork) {
        int id = -1;
        Variant var = request->extraData();
        if(var.isStringMap() || var.isNull()) {
            {
                static int sId = 0;
                static Mutex sMutex(ZERO_MUTEX, "DebuggerRequestIdMutex");
                id = ++sId;
            }

            var["id"] = id;
            request->setExtraData(var);
        }

        Variant data;
        data["request"] = id;
        data["now"] = Time::mono().ms() - Application::MONO_STARTED.ms();
        data["type"] = "begin";
        data["url"] = request->url().str();
        if(request->flags() & ResourceRequest::Joined)
            data["joined"] = true;
        switch (request->method()) {
        case Resource::Method_None:
        case Resource::Method_Get:
            data["method"] = "GET";
            break;
        case Resource::Method_Post:
            data["method"] = "POST";
            break;
        case Resource::Method_Head:
            data["method"] = "HEAD";
            break;
        case Resource::Method_Delete:
            data["method"] = "DELETE";
            break;
        }
        data["data"] = request->requestData().encode(DataBuffer::Encoding_Base64);
        data["headers"] = httpHeadersToVariant(request->requestHeaders());
        data["location"] = GibbonApplication::instance()->getLocation().str();
        sendEvent("NetworkEvent", data);
    }
}

void DebuggerBridge::endRequest(const ResourceRequest::SharedPtr &request,
                                const Resource::SharedPtr &resource,
                                Resource::State state)
{
    if(!mEnableDebug)
        return;

    if(mSendNetwork) {
        Variant data;
        const Variant requestData = request->extraData();
        data["request"] = requestData.mapValue<int>("id");
        if(request->flags() & ResourceRequest::Joined)
            data["joined"] = true;

        data["now"] = Time::mono().ms() - Application::MONO_STARTED.ms();
        data["type"] = "end";
        data["url"] = request->url().str();
        data["statusCode"] = resource->httpStatusCode();
        data["downloaded"] = resource->downloaded().ms();
        if(resource->errorCode() != NetErrorCode::SUCCESS) {
            data["error"]["code"] = resource->errorCode();
            data["error"]["string"] = NetError::errorCodeToString(resource->errorCode());
        }
        data["state"] = Resource::stateToString(state);
        data["location"] = GibbonApplication::instance()->getLocation().str();
        const std::string contentType = resource->responseHeader("Content-Type");
        if(startsWith(contentType, "image/")) {
            data["kind"] = "surface";
        } else if(contains(contentType, "javascript") || contains(contentType, "json")) {
            data["kind"] = "script";
        } else {
            data["kind"] = "data";
        }
        data["data"] = resource->responseData().toBase64();
        {
            Variant responseHeaders;
            const HttpResponseHeaders& headers = resource->responseHeaders();
            for(HttpResponseHeaders::const_iterator it = headers.begin(); it != headers.end(); ++it) {
                Variant& value = responseHeaders[it->header];
                if(value.isNull())
                    value = it->value;
                else
                    value = value.string() + "," + it->value;
            }
            data["headers"] = responseHeaders;
        }
        sendEvent("NetworkEvent", data);
    }
}

void DebuggerBridge::notifyClicked(const std::list<Widget::SharedPtr>& widgets)
{
    if(!mEnableDebug)
        return;

    Variant data;
    for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it)
        data["widgets"].push_back((*it)->getID());
    sendEvent("WidgetClicked", data);
}

void DebuggerBridge::receive(const Log::Message &msg)
{
    if(!mEnableDebug)
        return;

    bool send = true;
    if(msg.m_message.empty() || ((msg.m_traceArea != &TRACE_UI_SCRIPT) && msg.m_logLevel < Log::Warn))
        send = false;
    if(send) {
        DebuggerLogEvent::SharedPtr event(new DebuggerLogEvent(shared_from_this(), msg));
        GibbonApplication::instance()->postEvent(event);
    }
}

void DebuggerBridge::processInstrumentation(const Variant &inst)
{
    if(!mEnableDebug)
        return;

    if(mSendProfile) {
        DebuggerInstrumentationEvent::SharedPtr event(new DebuggerInstrumentationEvent(shared_from_this(), inst));
        GibbonApplication::instance()->postEvent(event);
    }
}

void DebuggerBridge::onLocationChanged()
{
    if(!mEnableDebug)
        return;
    sendEvent("LocationChangedEvent", GibbonApplication::instance()->getLocation().str());
}

void DebuggerBridge::onSceneChanged()
{
    if(!mEnableDebug)
        return;
    sendEvent("SceneChangedEvent");
}

void DebuggerBridge::updateWidget(const Variant& properties)
{
    sendEvent("WidgetChangedEvent", properties);
}

void DebuggerBridge::destroyWidget(int id)
{
    sendEvent("WidgetDestroyedEvent", id);
}

unsigned DebuggerBridge::requestFlags(const Request &request) const
{
    unsigned ret = Request::None;
    switch (request.method()) {
    case Methods::DebuggerBridge::disableDebug:
    case Methods::DebuggerBridge::enableDebug:
    case Methods::DebuggerBridge::disableNetwork:
    case Methods::DebuggerBridge::enableNetwork:
    case Methods::DebuggerBridge::disableProfile:
    case Methods::DebuggerBridge::enableProfile:
        ret |= Request::Synchronous;
        break;
    default:
        break;
    }
    return ret;
}
