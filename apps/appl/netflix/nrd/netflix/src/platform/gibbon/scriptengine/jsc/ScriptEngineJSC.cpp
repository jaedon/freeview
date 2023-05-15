/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include "ScriptEngine.h"
#include "WidgetBridge.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"
#include "GibbonEventConnectionJSC.h"
#include "Debug.h"
#include "JSONParserJSC.h"
#include "XMLParserJSC.h"
#include "ArrayBufferJSC.h"

#include <gibbon/config.h>

#include <nrd/Request.h>
#include <nrd/EventConnection.h>
#include <nrd/NrdConfiguration.h>
#include <nrd/ISystem.h>

#include <nrdbase/config.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/JSONParser.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/XMLParser.h>
#include <nrdbase/Random.h>
#include <nrdbase/Stopwatch.h>
#include <nrd/AppThread.h>
#include <nrd/AppLog.h>

#include <queue>
#include <assert.h>
#include <iterator>
#include <algorithm>

#include GIBBON_JSC_API(JSContextRef.h)
#include GIBBON_JSC_API(JSStringRef.h)
#include GIBBON_JSC_API(JSValueRef.h)
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
# include GIBBON_JSC_API(JSBasePrivate.h)
#endif

#ifdef HAVE_MALLOC_TRIM
#include <malloc.h>
#endif

using namespace netflix;
using namespace netflix::gibbon;
using namespace netflix::gibbon::JSC;

struct GibbonSyncObjectCount : public netflix::ObjectCount::Record
{
public:
    inline GibbonSyncObjectCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    virtual std::string describe(void *object) const;
};

NRDP_DEFINE_OBJECTCOUNT_RECORD(GibbonSyncObjectCount, GibbonSyncObjectEffect);
NRDP_DEFINE_OBJECTCOUNT_RECORD(GibbonSyncObjectCount, GibbonSyncObjectImage);
NRDP_DEFINE_OBJECTCOUNT_RECORD(GibbonSyncObjectCount, GibbonSyncObjectText);
NRDP_DEFINE_OBJECTCOUNT_RECORD(GibbonSyncObjectCount, GibbonSyncObjectWidget);

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
NRDP_DEFINE_OBJECTCOUNT_RECORD(ScriptEngineStringCount, ScriptEngineString);

std::string ScriptEngineStringCount::describe(void *object) const
{
    return reinterpret_cast<ScriptEngine::StringImplData*>(object)->toString();
}

size_t ScriptEngineStringCount::cost(void *object) const
{
    return reinterpret_cast<ScriptEngine::StringImplData*>(object)->cost();
}
#endif

#ifdef NRDP_HAS_BREAKPAD
# include <nrdbase/guid_creator.h>
#endif

static const JSClassDefinition kJSClassDefinitionEmptyLocal = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

extern "C" {
#if defined(NRDP_BUILD_XB1) || defined(NRDP_BUILD_PSP2)
# define JS_EXPORT_PRIVATE __declspec(dllexport)
#else
# define JS_EXPORT_PRIVATE
#endif
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
typedef void* Netflix_JSPropertyNameArrayRef;
JS_EXPORT_PRIVATE void Netflix_JSCInitializeThreading();
JS_EXPORT_PRIVATE void Netflix_WTFInitializeMainThread();
JS_EXPORT_PRIVATE bool Netflix_HeapInfo(JSContextRef ctx, size_t *size, size_t *capacity, size_t *objectCount,
                                        size_t *globalObjectCount, size_t *protectedObjectCount,
                                        size_t *protectedGlobalObjectCount);
JS_EXPORT_PRIVATE void Netflix_ReleaseExecutableMemory(JSContextRef ctx);
typedef void (*TimeoutCallback)(void *userData, const char *line);
JS_EXPORT_PRIVATE void Netflix_SetJSTimeoutInterval(JSContextRef ctx, unsigned int interval, TimeoutCallback callback, void *userData);
JS_EXPORT_PRIVATE void Netflix_TerminateScavengerThread();
JS_EXPORT_PRIVATE void Netflix_QueryMalloc(ullong* system, ullong* metadata, ullong* freebytes, size_t* returned);
typedef void (*MallocCallback)(ullong systemBytes);
JS_EXPORT_PRIVATE void Netflix_SetFastMallocCallback(MallocCallback callback);
typedef void (*DumpCallback)(void *userData, const char *fmt, ...);
JS_EXPORT_PRIVATE void Netflix_DumpJSHeap(JSContextRef ctx, void *userData, DumpCallback cb);
JS_EXPORT_PRIVATE void Netflix_DumpJSHeapCounts(JSContextRef ctx, void *userData, DumpCallback cb);
JS_EXPORT_PRIVATE void Netflix_Global_Stacktrace(JSContextRef ctx, void *userData, DumpCallback cb);
#else
typedef JSPropertyNameArrayRef Netflix_JSPropertyNameArrayRef;
#endif

JS_EXPORT_PRIVATE JSStringRef Netflix_JSStringCreate(const char* string, int length);
JS_EXPORT_PRIVATE Netflix_JSPropertyNameArrayRef Netflix_JSObjectPropertyNames(JSContextRef ctx, JSObjectRef object);
JS_EXPORT_PRIVATE Netflix_JSPropertyNameArrayRef Netflix_JSPropertyNameArrayRetain(Netflix_JSPropertyNameArrayRef array);
JS_EXPORT_PRIVATE void Netflix_JSPropertyNameArrayRelease(JSContextRef ctx, Netflix_JSPropertyNameArrayRef array);
JS_EXPORT_PRIVATE size_t Netflix_JSPropertyNameArrayGetCount(Netflix_JSPropertyNameArrayRef array);
JS_EXPORT_PRIVATE const JSChar* Netflix_JSPropertyNameArrayGetNameAtIndex(JSContextRef ctx, Netflix_JSPropertyNameArrayRef array, size_t index, size_t* size);
JS_EXPORT_PRIVATE JSValueRef Netflix_JSObjectGetProperty(JSContextRef ctx, JSObjectRef object, Netflix_JSPropertyNameArrayRef array, size_t index, JSValueRef* exception);
JS_EXPORT_PRIVATE size_t Netflix_GarbageCollect(JSContextRef ctx);
JS_EXPORT_PRIVATE JSContextGroupRef Netflix_JSContextGroupCreate();
JS_EXPORT_PRIVATE JSGlobalContextRef Netflix_JSGlobalContextCreateInGroup(JSContextGroupRef group);
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
JS_EXPORT_PRIVATE void Netflix_SetGCMaxBytesPerCycle(size_t size);
JS_EXPORT_PRIVATE void Netflix_UseBackdoorGarbageCollect(bool use);
#endif

#if defined(NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS) && defined(GIBBON_SCRIPT_JSC_BACKDOOR)
typedef void (*OSAllocatorCallback)(void *, size_t);
typedef void (*OSAllocatorStartCallback)();
typedef void (*StringImplRefCallback)(void *, const char *, const int16_t *, unsigned);
typedef void (*StringImplDerefCallback)(void *);
JS_EXPORT_PRIVATE void Netflix_SetAllocationCallbacks(OSAllocatorCallback allocate, OSAllocatorCallback release,
                                                      OSAllocatorCallback commit, OSAllocatorCallback decommit,
                                                      OSAllocatorStartCallback start,
                                                      StringImplRefCallback stringImplRefCallback,
                                                      StringImplDerefCallback stringImplDerefCallback);
#endif

#if defined(NRDP_JSC_ENABLE_GC_CALLBACKS) && defined(GIBBON_SCRIPT_JSC_BACKDOOR)
typedef void (*GCPhaseCallback)(int);
enum { GCPhase_StartMark, GCPhase_EndMark, GCPhase_StartSweep, GCPhase_EndSweep };
JS_EXPORT_PRIVATE void Netflix_SetGCCallbacks(GCPhaseCallback);
#endif

#ifdef GIBBON_SCRIPT_JSC_DEBUGGER
struct Netflix_JSBreakpoint
{
    JSStringRef url, condition;
    int line;
};
struct Netflix_JSStackFrame
{
    JSStringRef url, function;
    int line, column;
};
JS_EXPORT_PRIVATE void Netflix_AttachDebugger(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_DetachDebugger(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_StartCPUProfiler(JSContextRef ctx, int profileId);
JS_EXPORT_PRIVATE void Netflix_StopCPUProfiler(JSContextRef ctx, int profileId, void *userData, DumpCallback cb);
JS_EXPORT_PRIVATE void Netflix_ResolveSourceID(intptr_t sourceID, JSStringRef* file);
typedef void (*BreakpointCallback)(intptr_t sourceID, int line, int column, int reason, void* userData);
JS_EXPORT_PRIVATE void Netflix_SetBreakpointCallback(JSContextRef ctx, BreakpointCallback callback, void* userData);
JS_EXPORT_PRIVATE void Netflix_SetBreakpoint(JSContextRef ctx, JSStringRef file, int line, unsigned int* position);
JS_EXPORT_PRIVATE void Netflix_SetConditionalBreakpoint(JSContextRef ctx, unsigned int id, JSStringRef condition);
JS_EXPORT_PRIVATE void Netflix_RemoveBreakpoint(JSContextRef ctx, unsigned int id);
JS_EXPORT_PRIVATE void Netflix_StepOver(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_StepInto(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_Continue(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_Finish(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_Pause(JSContextRef ctx);
JS_EXPORT_PRIVATE bool Netflix_EvaluateAtBreakpoint(JSContextRef ctx, JSStringRef code, JSValueRef* ret, JSValueRef* exception, int frame);
JS_EXPORT_PRIVATE int Netflix_IgnoreExceptions(JSContextRef ctx, int ignore);
JS_EXPORT_PRIVATE int Netflix_StackTrace(JSContextRef ctx, Netflix_JSStackFrame *stack, int max);
JS_EXPORT_PRIVATE JSValueRef Netflix_Interpreter_StackTrace(JSContextRef ctx, JSObjectRef object);
JS_EXPORT_PRIVATE int Netflix_ListBreakpoints(JSContextRef ctx, Netflix_JSBreakpoint *breakpoints, int max);
JS_EXPORT_PRIVATE void Netflix_SetCurrentFrame(JSContextRef ctx, size_t frameNo);
JS_EXPORT_PRIVATE size_t Netflix_GetCurrentFrame(JSContextRef ctx);
JS_EXPORT_PRIVATE bool Netflix_Identifiers(JSContextRef ctx, JSStringRef** identifiers, size_t* count, int mode, int frame);
JS_EXPORT_PRIVATE void Netflix_ResetDebugger(JSContextRef ctx);
JS_EXPORT_PRIVATE int Netflix_SaveDebuggerState(JSContextRef ctx);
JS_EXPORT_PRIVATE void Netflix_RestoreDebuggerState(JSContextRef ctx, int id);
#endif
#ifdef NRDP_USE_JSC_FILE_LINE_TRACKING
JS_EXPORT_PRIVATE void JSFileLineSetLogFilename(const char *filename);
#endif
} // extern "C"

#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
static void stringDumpCallback(void *str, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    *reinterpret_cast<std::string*>(str) += StringFormatter::vsformat(fmt, args);
    va_end(args);
}
typedef void (*JSDumpCallbackCallback)(JSContextRef ctx, void *userData, DumpCallback cb);
static std::string jsCallbackToString(JSContextRef ctx, JSDumpCallbackCallback callbackCallback)
{
    std::string ret;
    callbackCallback(ctx, &ret, stringDumpCallback);
    return ret;
}
#if defined(BUILD_DEBUG)
static void jsTimeoutCallback(void *, const char *line)
{
    Log::Message msg(TRACE_GIBBON_SCRIPT, Log::Warn);
    msg.m_message = line;
    msg.send();
}
#endif
#endif

#if defined(NRDP_JSC_ENABLE_GC_CALLBACKS) && defined(GIBBON_SCRIPT_JSC_BACKDOOR)
static void GCPhase(int phase)
{
    switch(phase) {
    case GCPhase_StartMark:
        INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.jsc.mark");
        break;
    case GCPhase_EndMark:
        INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.jsc.mark");
        break;
    case GCPhase_StartSweep:
        INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.jsc.sweep");
        break;
    case GCPhase_EndSweep:
        INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.jsc.sweep");
        break;
    default:
        Log::error(TRACE_LOG, "Unhandled GCPhase[%d]!", phase);
        break;
    }
}
#endif

ScriptEngine::ContextScope::ContextScope(JSContextRef ctx)
    : mPrevious(ScriptEngine::sCurrentContext), mBridge(gibbonApp()->getBridge())
{
    assert(mPrevious);
    assert(Application::isAppThread());
    assert(ctx);
    ScriptEngine::sCurrentContext = ctx;
    assert(mBridge);
    mBridge->beginPendingSync();
}

ScriptEngine::ContextScope::~ContextScope()
{
    assert(Application::isAppThread());
    assert(mPrevious);
    ScriptEngine::sCurrentContext = mPrevious;
    assert(mBridge);
    mBridge->endPendingSync();
}

JSC::ProtectedValue ScriptEngine::sUndefined;
JSContextRef ScriptEngine::sCurrentContext = 0;
void ScriptEngine::init()
{
    mReleasedExecutableMemory = false;
    mGroup = 0;
    assert(!currentContext());
    mGlobal = 0;
    mGCTimeout  = GibbonConfiguration::garbageCollectTimeout();
    mBreakCount = 0;
    mFinalizeClass = 0;
    memset(&mArrayBufferClass, 0, sizeof(mArrayBufferClass));
}

JSCGCTimer::JSCGCTimer(int ms, bool bgc)
    : Application::Timer(ms), backdoorGC(bgc)
{
    setSingleShot(true);
}

void JSCGCTimer::timerFired()
{
    PERFORMANCE_MARKER_SCOPED("jsgctimer.fired");
    JSContextRef ctx = ScriptEngine::currentContext();
    if (ctx) {
        GibbonBridge::PendingSyncBlock scope;
        shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
        assert(engine);
        if (engine->isInBreakpoint())
            return;
        if (backdoorGC) {
            if (!engine->mReleasedExecutableMemory) {
                engine->mReleasedExecutableMemory = true;
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
                Netflix_ReleaseExecutableMemory(ctx);
#endif
#ifdef HAVE_MALLOC_TRIM
                malloc_trim(0);
#endif
                // restart the timer with the real timeout,
                // ReleaseExecutableMemory will already have collected garbage
            } else {
                Netflix_GarbageCollect(ctx);
#ifdef HAVE_MALLOC_TRIM
                malloc_trim(0);
#endif
            }
        } else {
            JSGarbageCollect(ctx);
        }
    }
    NTRACE(TRACE_UI_SCRIPT, "JS GC timer fired %p", ctx);
}

static inline std::string formatException(const JSValueRef& e, bool json)
{
    const Variant v = JSValueToVariant(e);

    const int line = v.mapValue<int>("line");
    const int column = v.mapValue<int>("column", 0, -1);
    const std::string url = v.mapValue<std::string>("sourceURL");

    std::ostringstream stream;

    if (json) {
        stream << "{\"url\":\"" << url << "\",\"line\":" << (line - 1) << ",\"column\":" << column << ",\"stack\":[";
    } else {
        stream << "  at " << url << ":" << (line - 1) << "\n"; // the lines that the debugger expects are 0-indexed
    }

    std::vector<std::string> stack;
    StringTokenizer::tokenize(v.mapValue<std::string>("stack"), stack, "\n");
    if (!stack.empty()) {
        if (!json)
            stream << "Stack:\n";

        const size_t sz = stack.size();
        for (size_t i = 0; i < sz; ++i) {
            const size_t cl = stack[i].rfind(':');
            if (cl != std::string::npos) {
                const int ln = atoi(stack[i].c_str() + cl + 1);
                if (json) {
                    stream << "{\"url\":\"" << stack[i].substr(0, cl) << "\",\"line\":" << (ln - 1) << "}";
                } else {
                    stream << "    " << stack[i].substr(0, cl);
                    stream << ":" << (ln - 1);
                }
            } else {
                if (json) {
                    stream << "{\"url\":\"" << stack[i] << "\",\"line\":-1}";
                } else {
                    stream << "    " << stack[i];
                }
            }
            if (i + 1 < sz) {
                if (json)
                    stream << ",";
                else
                    stream << "\n";
            }
        }
    }

    if (json) {
        stream << "]}";
    }

    return stream.str();
}

inline void LogException(const JSValueRef& e, const std::string file) {
    if (file == "CONSOLE") // Do not log console exceptions
        return;

    if (Configuration::crashReportEnabled()) {
        Variant crashReport;
        crashReport["reporter"] = "jsc";

#ifdef NRDP_HAS_BREAKPAD
        crashReport["breakpad"] = true;

        GUID guid;
        char guid_str[kGUIDStringLength + 1];
        if (!CreateGUID(&guid) || !GUIDToString(&guid, guid_str, sizeof(guid_str))) {
            crashReport["guid"] = "";
        } else {
            crashReport["guid"] = guid_str;
        }
#else
        crashReport["breakpad"] = false;
        crashReport["guid"] = "";
#endif
        crashReport["log"].stringStream() << "Uncaught exception in file " << file;
        crashReport["reason"] = "exception";
        crashReport["crashCode"] = JSValueToString(e);
        crashReport["dump"] = formatException(e, true);

        Log::Message m(TRACE_SYSTEM, Log::Error, "crashreport");
        m.m_message = crashReport.toJSON();
        m.m_flags |= (Log::Message::Critical|Log::Message::SendToAppBoot);

        m.send();
    }
}


JSValueRef ScriptEngine::jscBridgeEnabled(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef [], JSValueRef *)
{
    return JSValueMakeBoolean(ctx, true);
}

JSValueRef ScriptEngine::jscBridgeInit(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    ContextScope scope(ctx);
    shared_ptr<GibbonEventConnection> connection;
    {
        shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
        assert(engine);
        if (engine->mConnection)
            return ScriptEngine::sUndefined.value();
        NRDP_UNUSED(argumentCount);
        assert(argumentCount == 1);
        assert(JSValueIsObject(ctx, arguments[0]));
        ProtectedValue eventCallback = JSValueToObject(ctx, arguments[0], exception);
        engine->mConnection.reset(new GibbonEventConnection(eventCallback));
        connection = engine->mConnection;
    }

    nrdApp()->addEventConnection(connection);
    return JSValueMakeBoolean(ctx, true);
}

static inline void setupRequest(Request &request, JSContextRef ctx, JSObjectRef thisObject)
{
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    assert(ctx == ScriptEngine::currentContext());
    if (request.flags() & Request::JSBacktrace) {
        const std::string backtrace = JSValueToString(Netflix_Interpreter_StackTrace(ctx, thisObject));
        if (!backtrace.empty()) {
            request.setValue("jsDebuggerStack", backtrace);
        }
    }
#endif
}

JSValueRef ScriptEngine::jscBridgeSetProperty(JSContextRef ctx, JSObjectRef /*function*/,
                                              JSObjectRef thisObject, size_t argumentCount,
                                              const JSValueRef arguments[], JSValueRef* exception)
{
    ContextScope scope(ctx);
    assert(argumentCount >= 3);
    (void)argumentCount;
    Variant variantMap;
    assert(JSValueIsString(ctx, arguments[0]));
    assert(JSValueIsString(ctx, arguments[1]));
    variantMap["object"] = JSValueToString(arguments[0]);
    variantMap["property"] = JSValueToString(arguments[1]);
    variantMap["value"] = JSValueToVariant(arguments[2], exception);
    Request nbpRequest(variantMap, Request::FromTrustedSource);
    setupRequest(nbpRequest, ctx, thisObject);
    int bytes;
    return VariantToJSValue(nrdApp()->post(nbpRequest), bytes, exception);
}

class GibbonSyncObject : public PrivateBase
{
public:
    inline GibbonSyncObject()
        : PrivateBase(Type_SyncObject), mType(GibbonBaseBridge::UnknownType), mWidgetID(0)
    {}
    inline GibbonSyncObject(const shared_ptr<GibbonBridge> &bridge, GibbonBaseBridge::ClassType type, int widgetID)
        : PrivateBase(Type_SyncObject), mBridge(bridge), mType(type), mWidgetID(widgetID)
    {
#ifdef NRDP_HAS_OBJECTCOUNT
        switch(mType) {
        case GibbonBaseBridge::TextType:
            NRDP_OBJECTCOUNT_REF(GibbonSyncObjectText, this);
            break;
        case GibbonBaseBridge::WidgetType:
            NRDP_OBJECTCOUNT_REF(GibbonSyncObjectWidget, this);
            break;
        case GibbonBaseBridge::ImageType:
            NRDP_OBJECTCOUNT_REF(GibbonSyncObjectImage, this);
            break;
        case GibbonBaseBridge::EffectType:
            NRDP_OBJECTCOUNT_REF(GibbonSyncObjectEffect, this);
            break;
        case GibbonBaseBridge::UnknownType:
            break;
        }
#endif
    }
    ~GibbonSyncObject() {
        switch(mType) {
        case GibbonBaseBridge::TextType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectText, this);
            sObjects.erase(mapID(mType, mWidgetID));
            mBridge->pendingSync_destroyText(mWidgetID);
            break;
        case GibbonBaseBridge::WidgetType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectWidget, this);
            sObjects.erase(mapID(mType, mWidgetID));
            mBridge->pendingSync_destroyWidget(mWidgetID);
            break;
        case GibbonBaseBridge::ImageType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectImage, this);
            sObjects.erase(mapID(mType, mID));
            mBridge->pendingSync_destroyImage(mWidgetID, mID);
            break;
        case GibbonBaseBridge::EffectType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectEffect, this);
            sObjects.erase(mapID(mType, mID));
            mBridge->pendingSync_destroyEffect(mWidgetID, mID);
            break;
        case GibbonBaseBridge::UnknownType:
            break;
        }
    }
    inline int id() const { return mWidgetID; }
    inline void setValue(JSValueRef value) { mValue = value; }
    inline void setProperty(int property, const Variant &value, bool append)
    {
        switch(mType) {
        case GibbonBaseBridge::TextType:
            mBridge->pendingSync_addTextChange(mWidgetID, property, value, append);
            break;
        case GibbonBaseBridge::WidgetType:
            mBridge->pendingSync_addWidgetChange(mWidgetID, property, value, append);
            break;
        case GibbonBaseBridge::ImageType:
            mBridge->pendingSync_addImageChange(mWidgetID, mID, property, value, append);
            break;
        case GibbonBaseBridge::EffectType:
            mBridge->pendingSync_addEffectChange(mWidgetID, mID, property, value, append);
            break;
        default:
            assert(false);
        }
    }

    inline static ullong mapID(GibbonBaseBridge::ClassType type, int id) {
        return (((ullong)type) << 32) | id;
    }

    inline void init(int id, const Variant &params)
    {
        mID = id;
        switch(mType) {
        case GibbonBaseBridge::ImageType:
            sObjects[mapID(mType, mID)] = this;
            mBridge->pendingSync_createImage(mWidgetID, mID, params);
            break;
        case GibbonBaseBridge::EffectType:
            sObjects[mapID(mType, mID)] = this;
            mBridge->pendingSync_createEffect(mWidgetID, mID, params);
            break;
        default:
            assert(false);
        }
    }

    inline void init()
    {
        switch(mType) {
        case GibbonBaseBridge::TextType:
            sObjects[mapID(mType, mWidgetID)] = this;
            mBridge->pendingSync_createText(mWidgetID);
            break;
        case GibbonBaseBridge::WidgetType:
            sObjects[mapID(mType, mWidgetID)] = this;
            mBridge->pendingSync_createWidget(mWidgetID);
            break;
        default:
            assert(false);
        }
    }

    class Custom : public ValueCustom
    {
    public:
        inline Custom(JSValueRef value, const shared_ptr<GibbonBaseBridge> &bridge)
            : ValueCustom(ValueCustom::Protect, value), mBridge(bridge)
        {}
        virtual Variant toVariant() const { return mBridge->path(); }
        shared_ptr<GibbonBaseBridge> mBridge;
    };

    inline static bool isEmpty() { return sObjects.empty(); }
    inline static Variant find(const shared_ptr<GibbonBaseBridge> &bridge) {
        if(Application::isAppThread()) {
            if(const int id = bridge->classID()) {
                std::map<ullong, GibbonSyncObject*>::const_iterator it = sObjects.find(mapID(bridge->classType(), id));
                if(it != sObjects.end())
                    return Variant(shared_ptr<Variant::CustomData>(new Custom(it->second->mValue, bridge)));
            }
        }
        return Variant();
    }


    static inline JSObjectRef constructor(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
    {
        ScriptEngine::ContextScope scope(ctx);
        (void)argumentCount;
        assert(argumentCount >= 2);
        shared_ptr<ScriptEngine> scriptEngine = GibbonApplication::instance()->getScriptEngine();
        const int id = (int)JSValueToNumber(ctx, arguments[1], exception);
        GibbonBaseBridge::ClassType type = (GibbonBaseBridge::ClassType)(int)JSValueToNumber(ctx, arguments[2], exception);
        GibbonSyncObject *object = new GibbonSyncObject(gibbonApp()->getBridge(), type, id);
        switch(type) {
        case GibbonBaseBridge::TextType:
        case GibbonBaseBridge::WidgetType:
            object->init();
            break;
        case GibbonBaseBridge::ImageType:
        case GibbonBaseBridge::EffectType:
            assert(argumentCount >= 4);
            object->init(static_cast<int>(JSValueToNumber(ctx, arguments[3], exception)), JSValueToVariant(ctx, arguments[4]));
            break;
        default:
            assert(false);
        }

        ProtectedValue result(JSObjectMake(ctx, scriptEngine->syncObjectClass(), object));
        object->setValue(arguments[0]);
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
        JSReportExtraMemoryCost(ctx, 8*1024);
#endif
        return result.object();
    }

    static inline JSValueRef setSyncProperty(JSContextRef ctx, JSObjectRef /*function*/,
                                             JSObjectRef thiz, size_t argumentCount,
                                             const JSValueRef arguments[], JSValueRef* exception)
    {
        ScriptEngine::ContextScope scope(ctx);
        assert(argumentCount >= 2);
        GibbonSyncObject *object = static_cast<GibbonSyncObject*>(JSObjectGetPrivate(thiz));
        assert(object);
        bool append = false;
        if (argumentCount >= 3)
            append = JSValueToBoolean(ctx, arguments[2]);
        object->setProperty(static_cast<int>(JSValueToNumber(ctx, arguments[0], exception)),
                            JSValueToVariant(ctx, arguments[1], exception), append);
        return ScriptEngine::undefined().value();
    }

    static inline bool hasInstance(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
    {
        ScriptEngine::ContextScope scope(ctx);
        if (JSValueIsObject(ctx, possibleInstance)) {
            const PrivateBase *priv = reinterpret_cast<const PrivateBase*>(JSObjectGetPrivate(JSValueToObject(ctx, possibleInstance, exception)));
            return priv && priv->type == PrivateBase::Type_SyncObject;
        }
        return false;
    }

    static inline void finalize(JSObjectRef object)
    {
        delete static_cast<GibbonSyncObject*>(JSObjectGetPrivate(object));
    }
protected:
    shared_ptr<GibbonBridge> mBridge;
    GibbonBaseBridge::ClassType mType;
    int mWidgetID, mID;
    JSValueRef mValue;
    static std::map<ullong, GibbonSyncObject*> sObjects;
};
std::map<ullong, GibbonSyncObject*> GibbonSyncObject::sObjects;

std::string GibbonSyncObjectCount::describe(void *object) const
{
    const int id = static_cast<GibbonSyncObject*>(object)->id();
    {
        ScopedMutex _lock(Widget::lock());
        if(Widget::SharedPtr widget = gibbonApp()->getWidget(id))
            return widget->describe();
    }
    return StringFormatter::sformat("UNKNOWN:%d", id);
}

Variant ScriptEngine::objectId(const shared_ptr<GibbonBaseBridge> &bridge)
{
    return GibbonSyncObject::find(bridge);
}

JSValueRef ScriptEngine::jscBridgeInvoke(JSContextRef ctx, JSObjectRef /*function*/,
                                         JSObjectRef thisObject, size_t argumentCount,
                                         const JSValueRef arguments[], JSValueRef* exception)
{
    ContextScope scope(ctx);
    assert(argumentCount >= 3);
    (void)argumentCount;
    assert(JSValueIsString(ctx, arguments[0]));
    assert(JSValueIsString(ctx, arguments[1]));
    Variant variantMap = JSValueToVariant(arguments[2], exception);
    if (!variantMap.isStringMap() && !variantMap.isNull()) {
        Log::error(TRACE_GIBBON_SCRIPT, "Invalid arguments passed to jscBridgeInvoke");
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments jsc._invoke "));
        return ScriptEngine::sUndefined.value();
    }
    variantMap["object"] = JSValueToString(arguments[0]);
    variantMap["method"] = JSValueToString(arguments[1]);

    Request nbpRequest(variantMap, Request::FromTrustedSource);
    setupRequest(nbpRequest, ctx, thisObject);
    int bytes;
    return VariantToJSValue(nrdApp()->post(nbpRequest), bytes, exception);
}

JSValueRef ScriptEngine::jscLocation(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef[], JSValueRef *)
{
    int bytes = 0;
    ScriptEngine::ContextScope scope(ctx);
    Variant location;
    location["url"] = GibbonApplication::instance()->getLocation().str();
    location["count"] = GibbonApplication::instance()->getLocationCount();
    return VariantToJSValue(location, bytes, 0);
}


JSValueRef ScriptEngine::jscLog(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *)
{
#if defined(BUILD_DEBUG)
    ScriptEngine::ContextScope scope(ctx);
    assert(argumentCount >= 1);
    (void)argumentCount;
    const std::string msg = JSValueToString(arguments[0]);
    NTRACE(TRACE_UI_SCRIPT, "%s", msg.c_str());
#else
    NRDP_UNUSED(ctx);
    NRDP_UNUSED(argumentCount);
    NRDP_UNUSED(arguments);
#endif
    return ScriptEngine::sUndefined.value();
}

JSValueRef ScriptEngine::jscParseJSON(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount != 1) {
        Log::error(TRACE_UI_SCRIPT, "jscParseJSON takes exactly one argument. Not %zu", argumentCount);
        return ScriptEngine::sUndefined.value();
    }

    DecodedData data;
    if (!data.decode(ctx, arguments[0])) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments parseJson "));
        return ScriptEngine::sUndefined.value();
    }

    size_t pos = 0;
    std::vector<JSValueRef> ret;
    const char *ch = data.data();
    while (pos < data.length) {
        JSONParserJSC parser(ctx);
        JSValueRef val = parser.parse(ch + pos, data.length - pos);
        if (JSValueIsUndefined(ctx, val)) {
            for (std::vector<JSValueRef>::const_iterator it = ret.begin(); it != ret.end(); ++it)
                JSValueUnprotect(ctx, *it);
            return ScriptEngine::sUndefined.value();
        }

        ret.push_back(val);
        pos = parser.current() - ch;
    }
    JSObjectRef array = JSObjectMakeArray(ctx, ret.size(), &ret[0], exception);
    for (std::vector<JSValueRef>::const_iterator it = ret.begin(); it != ret.end(); ++it)
        JSValueUnprotect(ctx, *it);
    return array;
}

JSValueRef ScriptEngine::jscParseXML(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount != 1) {
        Log::error(TRACE_UI_SCRIPT, "jscParseXML takes exactly 1 argument. Not %zu", argumentCount);
        return ScriptEngine::sUndefined.value();
    }

    DecodedData data;
    if (!data.decode(ctx, arguments[0])) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments parseXML "));
        return ScriptEngine::sUndefined.value();
    }

    XMLParserJSC parser(ctx);
    shared_ptr<XMLParser::Value> value = parser.parse(data.data(), data.length);
    if (value) {
        JSObjectRef ret = reinterpret_cast<XMLParserJSC::JSCObject*>(value.get())->object;
        JSValueUnprotect(ctx, ret);
        return ret;
    }
    return ScriptEngine::sUndefined.value();
}

JSValueRef ScriptEngine::jscCompress(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    if (argumentCount < 2 || !JSValueIsString(ctx, arguments[1]))
        return ScriptEngine::sUndefined.value();

    ScopedString type(JSValueToStringCopy(ctx, arguments[1], exception));
    if (!type)
        return ScriptEngine::sUndefined.value();
    const bool gzip = JSStringIsEqualToUTF8CString(type, "gzip");
    if (!gzip)
        Log::error(TRACE_UI_SCRIPT, "Invalid compression type: '%s'", toStdString(type).c_str());
    if (!gzip)
        return ScriptEngine::sUndefined.value();

    ScriptEngine::ContextScope scope(ctx);
    DecodedData data;
    if (!data.decode(ctx, arguments[0])) {
        if (JSValueGetType(ctx, arguments[0]) == kJSTypeObject) {
            JSStringRef json = JSValueCreateJSONString(ctx, arguments[0], 0, exception);
            if (json)
                data.decode(json);
            JSStringRelease(json);
        }
        if (!data.length) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments parseXML "));
            return ScriptEngine::sUndefined.value();
        }
    }

    const bool base64 = (argumentCount < 3 || !JSValueToBoolean(ctx, arguments[2]));

    if (!data.length)
        return ScriptEngine::sUndefined.value();

    bool ok;
    const DataBuffer::CompressionMode mode = DataBuffer::Compression_GZip;
    DataBuffer compressed = data.view().compress(mode, &ok);
    if (!ok)
        return ScriptEngine::sUndefined.value();
    if (base64) {
        compressed = compressed.encode(DataBuffer::Encoding_Base64);
        NTRACE(TRACE_GIBBON_SCRIPT, "jscCompress: %zu -> %d bytes (string)", data.length, compressed.size());
        return stringToJSValue(ctx, compressed);
    }
    NRDP_OBJECTCOUNT_DESCRIBE(compressed, "JSCCOMPRESS");
    NTRACE(TRACE_GIBBON_SCRIPT, "jscCompress: %zu -> %d bytes (typed array)", data.length, compressed.size());
    return TypedArrayJSC::construct(ScriptEngine::Type_Uint8, ctx, compressed);
}

JSValueRef ScriptEngine::jscUncompress(JSContextRef ctx, JSObjectRef, JSObjectRef,
                                       size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount < 2)
        return ScriptEngine::sUndefined.value();

    ScopedString type(JSValueToStringCopy(ctx, arguments[1], exception));
    if (!type)
        return ScriptEngine::sUndefined.value();
    const bool gzip = JSStringIsEqualToUTF8CString(type, "gzip");
    if (!gzip)
        Log::error(TRACE_UI_SCRIPT, "Invalid compression type: '%s'", toStdString(type).c_str());
    if (!gzip)
        return ScriptEngine::sUndefined.value();

    DecodedData data;
    data.decode(ctx, arguments[0]);

    if (!data.length)
        return ScriptEngine::sUndefined.value();

    bool ok;
    const DataBuffer::CompressionMode mode = DataBuffer::Compression_GZip;
    DataBuffer raw = data.view();
    if (data.type == DecodedData::Type_String)
        raw = raw.decode(DataBuffer::Encoding_Base64);
    DataBuffer uncompressed = raw.uncompress(mode, &ok);
    if (!ok)
        return ScriptEngine::sUndefined.value();

    if (argumentCount < 3 || !JSValueToBoolean(ctx, arguments[2])) {
        NTRACE(TRACE_GIBBON_SCRIPT, "jscUncompress: %zu -> %d bytes (string)", data.length, uncompressed.size());
        return stringToJSValue(ctx, uncompressed);
    }
    NRDP_OBJECTCOUNT_DESCRIBE(uncompressed, "JSCUNCOMPRESS");
    NTRACE(TRACE_GIBBON_SCRIPT, "jscUncompress: %zu -> %d bytes (typed array)", data.length, uncompressed.size());
    return TypedArrayJSC::construct(Type_Uint8, ctx, uncompressed);
}

// convert a typed array with utf8 to a string

JSValueRef ScriptEngine::jscutf8toa(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef*)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount >= 1) {
        DecodedData data;
        if (data.decode(ctx, arguments[0])) {
            NTRACES(TRACE_GIBBON_SCRIPT, "jscutf8toa: " << data.length << " bytes (string)");
            if (data.length)
                return stringToJSValue(ctx, data.data(), data.length);
            return stringToJSValue(ctx, std::string());
        }
    }
    return ScriptEngine::sUndefined.value();
}

// convert a string to a utf8 typed array

JSValueRef ScriptEngine::jscatoutf8(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef*)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount >= 1) {
        DecodedData data;
        if (data.decode(ctx, arguments[0]) && data.type == DecodedData::Type_String) {
            NTRACES(TRACE_GIBBON_SCRIPT, "jscatoutf8: " << data.length << " bytes (typed array)");
            if (data.length) {
                data.dataBuffer.detach();
                NRDP_OBJECTCOUNT_DESCRIBE(data.dataBuffer, "JSCATOUTF8");
                return TypedArrayJSC::construct(Type_Uint8, ctx, data.dataBuffer);
            }
            return TypedArrayJSC::construct(Type_Uint8, ctx, DataBuffer());
        }
    }
    return ScriptEngine::sUndefined.value();
}

class BackInserterWrapper
{
public:
    BackInserterWrapper(uint8_t *data, int size)
        : mData(data), mSize(size), mPos(0)
    {
        NRDP_UNUSED(mSize);
    }

    typedef const unsigned char& const_reference;
    typedef unsigned char& reference;
    typedef unsigned char value_type;

    inline void push_back(unsigned char ch)
    {
        assert(mPos < mSize);
        mData[mPos++] = ch;
    }
private:
    uint8_t *mData;
    int mSize, mPos;
};

// base64 to string

JSValueRef ScriptEngine::jscatob(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* /* exception */)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount >= 1) {
        const bool returnTypedArray = argumentCount >= 2 ? JSValueToBoolean(ctx, arguments[1]) : false;
        DecodedData data;
        data.decode(ctx, arguments[0]);
        if (data.length) {
            const char *ch = data.data();
            int size = Base64::decode_reserve(data.length);
            const char *end = ch + (data.length - 1);
            while (end >= ch && *end == '=') {
                --end;
                --size;
            }
            ++end;
            if (returnTypedArray) {
                DataBuffer ret = DataBuffer::fromBase64(reinterpret_cast<const unsigned char*>(ch), end - ch);
                NRDP_OBJECTCOUNT_DESCRIBE(ret, "JSCATOB");
                NTRACES(TRACE_GIBBON_SCRIPT, "jscatob: " << data.length << " -> " << ret.size() << " bytes (typed array)");
                return TypedArrayJSC::construct(ScriptEngine::Type_Uint8, ctx, ret);
            }

            std::string result;
            result.reserve(size);
            Base64::decode(ch, end, std::back_inserter(result));

            NTRACES(TRACE_GIBBON_SCRIPT, "jscatob: " << data.length << " -> " << result.size() << " bytes (string)");
            // NTRACE(TRACE_GIBBON_SCRIPT, "base64Decode(2): %s => %s", data.data, result.c_str());
            return stringToJSValue(ctx, result);
        } else {
            if (returnTypedArray)
                return TypedArrayJSC::construct(ScriptEngine::Type_Uint8, ctx, DataBuffer());
            return stringToJSValue(ctx, std::string());
        }
    }
    return ScriptEngine::sUndefined.value();
}

// string to base64

JSValueRef ScriptEngine::jscbtoa(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* /* exception */)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount >= 1) {
        const bool urlSafe = argumentCount >= 2 ? JSValueToBoolean(ctx, arguments[1]) : false;
        const bool returnTypedArray = argumentCount >= 3 ? JSValueToBoolean(ctx, arguments[2]) : false;
        DecodedData data;
        data.decode(ctx, arguments[0]);
        if (data.length) {

            const char *ch = data.data();
            if (returnTypedArray) {
                const DataBuffer buf = DataBuffer::fromRawData(ch, data.length);
                DataBuffer ret = buf.encode(urlSafe ? DataBuffer::Encoding_Base64_UrlSafe : DataBuffer::Encoding_Base64);
                NRDP_OBJECTCOUNT_DESCRIBE(ret, "JSCBTOA");
                NTRACES(TRACE_GIBBON_SCRIPT, "jscbtoa: " << data.length << " -> " << ret.size() << " bytes (typed array)");
                return TypedArrayJSC::construct(ScriptEngine::Type_Uint8, ctx, ret);
                // // NTRACE(TRACE_GIBBON_SCRIPT, "base64Encode(1): %s => %s", data.data, reinterpret_cast<const char*>(outData));
            }

            const int size = Base64::encode_reserve(data.length);
            std::vector<JSChar> result;
            result.reserve(size);
            Base64::encode(ch, ch + data.length, std::back_inserter(result), urlSafe);
            // NTRACE(TRACE_GIBBON_SCRIPT, "base64Encode(2): %s => %s", data.data, eatString(JSStringCreateWithCharacters(&result[0], result.size())).c_str());

            NTRACES(TRACE_GIBBON_SCRIPT, "jscbtoa: " << data.length << " -> " << result.size() << " bytes (string)");
            return stringToJSValue(ctx, result);
        } else {
            if (returnTypedArray)
                return TypedArrayJSC::construct(ScriptEngine::Type_Uint8, ctx, DataBuffer());
            return stringToJSValue(ctx, std::string());
        }
    }
    return ScriptEngine::sUndefined.value();
}

JSValueRef ScriptEngine::jscrandom(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount >= 1 && JSValueIsObject(ctx, arguments[0])) {
        JSObjectRef object = JSValueToObject(ctx, arguments[0], exception);
        assert(object);
        PrivateBase *data = static_cast<PrivateBase*>(JSObjectGetPrivate(object));
        if (data) {
            int offset = 0;
            int length = 0;
            DataBuffer *buffer = 0;
            switch (data->type) {
            case PrivateBase::Type_ArrayBuffer:
                buffer = &static_cast<ArrayBufferJSC*>(data)->mDataBuffer;
                length = buffer->size();
                break;
            case PrivateBase::Type_DataView:
            case PrivateBase::Type_TypedArray: {
                ArrayBufferViewJSC *view = static_cast<ArrayBufferViewJSC*>(data);
                if (ArrayBufferJSC *buf = view->buffer()) {
                    offset = view->mByteOffset;
                    length = view->mByteLength;
                    buffer = &buf->mDataBuffer;
                }
                break; }
            default:
                break;
            }
            if (buffer && length) {
                Random::next(buffer->data() + offset, length);
                return JSValueMakeBoolean(ctx, true);
            }
        }
    }
    return ScriptEngine::sUndefined.value();
}

JSValueRef ScriptEngine::jscMakeGCTag(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* /*exception*/)
{
    ScriptEngine::ContextScope scope(ctx);
    const std::string name(argumentCount >= 1 ? JSValueToString(arguments[0]) : "<unnamed>");
    return JSObjectMake(ctx, GibbonApplication::instance()->getScriptEngine()->mFinalizeClass, new GCTagPrivate(name));
}

JSValueRef ScriptEngine::jscMono(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef[], JSValueRef* /*exception*/)
{
    ScriptEngine::ContextScope scope(ctx);
    return JSValueMakeNumber(ctx, Time::monoMS());
}

JSValueRef ScriptEngine::jscPMono(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef[], JSValueRef* /*exception*/)
{
    ScriptEngine::ContextScope scope(ctx);
    return JSValueMakeNumber(ctx, Stopwatch::mono());
}

JSValueRef ScriptEngine::jscGlobal(JSContextRef ctx, JSObjectRef, JSStringRef, JSValueRef*)
{
    ScriptEngine::ContextScope scope(ctx);
    ScriptEngine::SharedPtr engine = GibbonApplication::instance()->getScriptEngine();
    assert(engine);
    return engine->mGlobal;
}

void ScriptEngine::startEngine()
{
    assert(mStarted);
#ifdef NRDP_OSALLOCATOR_LOGS_ENABLED
    checkOSAllocatorLog();
#endif

#ifdef NRDP_USE_JSC_FILE_LINE_TRACKING
    JSFileLineSetLogFilename(GibbonConfiguration::jsAllocLogPath().c_str());
#endif

#if defined(NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS) && defined(GIBBON_SCRIPT_JSC_BACKDOOR)
    Netflix_SetAllocationCallbacks(OSAllocatorAllocate, OSAllocatorRelease, OSAllocatorCommit,
                                   OSAllocatorDecommit, OSAllocatorStart, StringImplRef, StringImplDeref);
#endif
#if defined(NRDP_JSC_ENABLE_GC_CALLBACKS) && defined(GIBBON_SCRIPT_JSC_BACKDOOR)
    Netflix_SetGCCallbacks(GCPhase);
#endif
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    Netflix_SetFastMallocCallback(mallocCallback);
    Netflix_JSCInitializeThreading();
    Netflix_WTFInitializeMainThread();
    Netflix_UseBackdoorGarbageCollect(GibbonConfiguration::backdoorGarbageCollect());
#endif

    mGroup = Netflix_JSContextGroupCreate();
    assert(mGroup);
    mBridge = GibbonApplication::instance()->getBridge();
    assert(!sCurrentContext);
    sCurrentContext = Netflix_JSGlobalContextCreateInGroup(mGroup);
    assert(sCurrentContext);
#ifdef GIBBON_SCRIPT_JSC_DEBUGGER
    if (mSavedState != -1) {
        Netflix_RestoreDebuggerState(sCurrentContext, mSavedState);
        mSavedState = -1;
    }
    Netflix_SetBreakpointCallback(sCurrentContext, ScriptEngine::breakpoint, this);
#endif
    mGlobal = JSContextGetGlobalObject(sCurrentContext);
    sUndefined = JSValueMakeUndefined(sCurrentContext);
    JSC::initGeneratedStrings();
    {
        JSStaticFunction functions[] = {
            { "jscBridgeEnabled", jscBridgeEnabled, kJSPropertyAttributeNone },
            { "jscBridgeInit", jscBridgeInit, kJSPropertyAttributeNone },
            { "jscBridgeSetProperty", jscBridgeSetProperty, kJSPropertyAttributeNone },
            { "jscBridgeInvoke", jscBridgeInvoke, kJSPropertyAttributeNone },
            { "location", jscLocation, kJSPropertyAttributeNone },
            { "log", jscLog, kJSPropertyAttributeNone },
            { "parseJSON", jscParseJSON, kJSPropertyAttributeNone },
            { "parseXML", jscParseXML, kJSPropertyAttributeNone },
            { "atob", jscatob, kJSPropertyAttributeNone },
            { "btoa", jscbtoa, kJSPropertyAttributeNone },
            { "atoutf8", jscatoutf8, kJSPropertyAttributeNone },
            { "utf8toa", jscutf8toa, kJSPropertyAttributeNone },
            { "compress", jscCompress, kJSPropertyAttributeNone },
            { "uncompress", jscUncompress, kJSPropertyAttributeNone },
            { "random", jscrandom, kJSPropertyAttributeNone },
            { "gctag", jscMakeGCTag, kJSPropertyAttributeNone },
            { "mono", jscMono, kJSPropertyAttributeNone },
            { "pmono", jscPMono, kJSPropertyAttributeNone },
            { 0, 0, kJSPropertyAttributeNone }
        };

        JSStaticValue values[] = {
            { "global", jscGlobal, 0, kJSPropertyAttributeNone },
            { 0, 0, 0, kJSPropertyAttributeNone }
        };

        JSClassDefinition definition = kJSClassDefinitionEmptyLocal;
        definition.staticFunctions = functions;
        definition.staticValues = values;

        JSClassRef classRef = JSClassCreate(&definition);
        mNrdpPlatform.reset(new ProtectedValue(JSObjectMake(sCurrentContext, classRef, 0)));
        JSClassRelease(classRef);
        ScopedString name(JSStringCreateWithUTF8CString("nrdp_platform"));
        JSObjectSetProperty(sCurrentContext, mGlobal, name,
                            mNrdpPlatform->object(), kJSPropertyAttributeNone, 0);
    }
    {
        JSClassDefinition finalizeDef = kJSClassDefinitionEmptyLocal;
        finalizeDef.finalize = FinalizeObject::finalize;
        mFinalizeClass = JSClassCreate(&finalizeDef);
    }

    {
        JSClassDefinition syncObjectClassDef = kJSClassDefinitionEmptyLocal;
        syncObjectClassDef.attributes = kJSClassAttributeNoAutomaticPrototype;
        syncObjectClassDef.callAsConstructor = GibbonSyncObject::constructor;
        syncObjectClassDef.hasInstance = GibbonSyncObject::hasInstance;
        syncObjectClassDef.className = "GibbonSyncObject";
        syncObjectClassDef.finalize = GibbonSyncObject::finalize;
        JSStaticFunction functions[] = {
            { "setSyncProperty", GibbonSyncObject::setSyncProperty, kJSPropertyAttributeNone },
            { 0, 0, kJSPropertyAttributeNone }
        };
        syncObjectClassDef.staticFunctions = functions;
        mSyncObjectClass.classRef = JSClassCreate(&syncObjectClassDef);
        mSyncObjectClass.prototype = JSObjectMake(sCurrentContext, mSyncObjectClass.classRef, new GibbonSyncObject);
        ScopedString name(JSStringCreateWithUTF8CString(syncObjectClassDef.className));
        JSObjectSetProperty(sCurrentContext, mGlobal, name, mSyncObjectClass.prototype, kJSPropertyAttributeReadOnly, 0);
    }

    {
        JSClassDefinition arrayBufferClassDef = kJSClassDefinitionEmptyLocal;
        arrayBufferClassDef.attributes = kJSClassAttributeNoAutomaticPrototype;
        // arrayBufferClassDef.attributes = kJSClassAttributeNone;
        arrayBufferClassDef.callAsConstructor = ArrayBufferJSC::construct;
        arrayBufferClassDef.className = "ArrayBuffer";
        arrayBufferClassDef.finalize = ArrayBufferJSC::finalize;
        arrayBufferClassDef.hasInstance = ArrayBufferJSC::hasInstance;
        arrayBufferClassDef.getPropertyNames = ArrayBufferJSC::getPropertyNames;
        JSStaticFunction functions[] = {
            { "slice", ArrayBufferJSC::slice, kJSPropertyAttributeDontEnum },
            { 0, 0, kJSPropertyAttributeNone }
        };
        arrayBufferClassDef.staticFunctions = functions;
        JSStaticValue values[] = {
            { "byteLength", ArrayBufferJSC::byteLength, 0, kJSPropertyAttributeReadOnly },
            { "data", ArrayBufferJSC::data, 0, kJSPropertyAttributeDontEnum },
            { "base64", ArrayBufferJSC::base64, 0, kJSPropertyAttributeDontEnum },
            { "printable", ArrayBufferJSC::printable, 0, kJSPropertyAttributeDontEnum },
            { "constructor", ArrayBufferJSC::constructor, 0, kJSPropertyAttributeDontEnum },
            { 0, 0, 0, 0 }
        };
        arrayBufferClassDef.staticValues = values;
        mArrayBufferClass.classRef = JSClassCreate(&arrayBufferClassDef);
        mArrayBufferClass.prototype = JSObjectMake(sCurrentContext, mArrayBufferClass.classRef, new ArrayBufferJSC);
        ScopedString name(JSStringCreateWithUTF8CString(arrayBufferClassDef.className));
        JSObjectSetProperty(sCurrentContext, mGlobal, name, mArrayBufferClass.prototype, kJSPropertyAttributeReadOnly, 0);
    }

    {
        JSClassDefinition typedArrayClassDef = kJSClassDefinitionEmptyLocal;
        typedArrayClassDef.finalize = TypedArrayJSC::finalize;
        // typedArrayClassDef.attributes = kJSClassAttributeNoAutomaticPrototype;
        typedArrayClassDef.attributes = kJSClassAttributeNone;
        typedArrayClassDef.getProperty = TypedArrayJSC::getProperty;
        typedArrayClassDef.setProperty = TypedArrayJSC::setProperty;
        typedArrayClassDef.hasProperty = TypedArrayJSC::hasProperty;
        typedArrayClassDef.getPropertyNames = TypedArrayJSC::getPropertyNames;

        JSStaticFunction functions[] = {
            { "set", TypedArrayJSC::set, kJSPropertyAttributeDontEnum },
            { "subarray", TypedArrayJSC::subarray, kJSPropertyAttributeDontEnum },
            { 0, 0, kJSPropertyAttributeNone }
        };

        typedArrayClassDef.staticFunctions = functions;
        struct {
            const char *className;
            JSObjectCallAsConstructorCallback constructor;
            JSObjectHasInstanceCallback hasInstance;
            const TypedArrayType type;
        } types[] = {
            { "Int8Array", TypedArrayJSC::constructInt8, TypedArrayJSC::hasInstanceInt8, Type_Int8 },
            { "Uint8Array", TypedArrayJSC::constructUint8, TypedArrayJSC::hasInstanceUint8, Type_Uint8 },
            { "Uint8ClampedArray", TypedArrayJSC::constructUint8Clamped, TypedArrayJSC::hasInstanceUint8Clamped, Type_Uint8Clamped },
            { "Int16Array", TypedArrayJSC::constructInt16, TypedArrayJSC::hasInstanceInt16, Type_Int16 },
            { "Uint16Array", TypedArrayJSC::constructUint16, TypedArrayJSC::hasInstanceUint16, Type_Uint16 },
            { "Int32Array", TypedArrayJSC::constructInt32, TypedArrayJSC::hasInstanceInt32, Type_Int32 },
            { "Uint32Array", TypedArrayJSC::constructUint32, TypedArrayJSC::hasInstanceUint32, Type_Uint32 },
            { "Float32Array", TypedArrayJSC::constructFloat32, TypedArrayJSC::hasInstanceFloat32, Type_Float32 },
            { "Float64Array", TypedArrayJSC::constructFloat64, TypedArrayJSC::hasInstanceFloat64, Type_Float64 },
            { 0, 0, 0, Type_None }
        };
        for (int i=0; types[i].className; ++i) {
            typedArrayClassDef.className = types[i].className;
            typedArrayClassDef.hasInstance = types[i].hasInstance;
            typedArrayClassDef.callAsConstructor = types[i].constructor;
            JSClassRef classRef = JSClassCreate(&typedArrayClassDef);
            JSObjectRef prototypeObject = JSObjectMake(sCurrentContext, classRef, new TypedArrayJSC(types[i].type));
            const Class c = { classRef, prototypeObject };
            mTypedArrayClasses[types[i].type] = c;
            ScopedString name(JSStringCreateWithUTF8CString(types[i].className));
            JSObjectSetProperty(sCurrentContext, mGlobal, name, prototypeObject, kJSPropertyAttributeReadOnly, 0);
        }
    }

    {
        JSClassDefinition dataViewClassDef = kJSClassDefinitionEmptyLocal;
        dataViewClassDef.attributes = kJSClassAttributeNoAutomaticPrototype;
        // dataViewClassDef.attributes = kJSClassAttributeNone;
        dataViewClassDef.callAsConstructor = DataViewJSC::construct;
        dataViewClassDef.className = "DataView";
        dataViewClassDef.finalize = DataViewJSC::finalize;
        dataViewClassDef.hasInstance = DataViewJSC::hasInstance;
        JSStaticFunction functions[] = {
            { "getInt8", DataViewJSC::getInt8, kJSPropertyAttributeDontEnum },
            { "getUInt8", DataViewJSC::getUInt8, kJSPropertyAttributeDontEnum },
            { "getUint8", DataViewJSC::getUInt8, kJSPropertyAttributeDontEnum },
            { "getInt16", DataViewJSC::getInt16, kJSPropertyAttributeDontEnum },
            { "getUInt16", DataViewJSC::getUInt16, kJSPropertyAttributeDontEnum },
            { "getUint16", DataViewJSC::getUInt16, kJSPropertyAttributeDontEnum },
            { "getInt32", DataViewJSC::getInt32, kJSPropertyAttributeDontEnum },
            { "getUInt32", DataViewJSC::getUInt32, kJSPropertyAttributeDontEnum },
            { "getUint32", DataViewJSC::getUInt32, kJSPropertyAttributeDontEnum },
            { "getFloat32", DataViewJSC::getFloat32, kJSPropertyAttributeDontEnum },
            { "getFloat64", DataViewJSC::getFloat64, kJSPropertyAttributeDontEnum },
            { "setInt8", DataViewJSC::setInt8, kJSPropertyAttributeDontEnum },
            { "setUInt8", DataViewJSC::setUInt8, kJSPropertyAttributeDontEnum },
            { "setUint8", DataViewJSC::setUInt8, kJSPropertyAttributeDontEnum },
            { "setInt16", DataViewJSC::setInt16, kJSPropertyAttributeDontEnum },
            { "setUInt16", DataViewJSC::setUInt16, kJSPropertyAttributeDontEnum },
            { "setUint16", DataViewJSC::setUInt16, kJSPropertyAttributeDontEnum },
            { "setInt32", DataViewJSC::setInt32, kJSPropertyAttributeDontEnum },
            { "setUInt32", DataViewJSC::setUInt32, kJSPropertyAttributeDontEnum },
            { "setUint32", DataViewJSC::setUInt32, kJSPropertyAttributeDontEnum },
            { "setFloat32", DataViewJSC::setFloat32, kJSPropertyAttributeDontEnum },
            { "setFloat64", DataViewJSC::setFloat64, kJSPropertyAttributeDontEnum },
            { 0, 0, kJSPropertyAttributeNone }
        };
        dataViewClassDef.staticFunctions = functions;
        JSStaticValue values[] = {
            { "byteLength", ArrayBufferViewJSC::byteLength, 0, kJSPropertyAttributeReadOnly },
            { "byteOffset", ArrayBufferViewJSC::byteOffset, 0, kJSPropertyAttributeDontEnum },
            { "buffer", DataViewJSC::buffer, 0, kJSPropertyAttributeDontEnum },
            { 0, 0, 0, 0 }
        };
        dataViewClassDef.staticValues = values;
        mDataViewClass.classRef = JSClassCreate(&dataViewClassDef);
        mDataViewClass.prototype = JSObjectMake(sCurrentContext, mDataViewClass.classRef, new DataViewJSC);
        ScopedString name(JSStringCreateWithUTF8CString(dataViewClassDef.className));
        JSObjectSetProperty(sCurrentContext, mGlobal, name, mDataViewClass.prototype, kJSPropertyAttributeReadOnly, 0);
    }

#if defined(GIBBON_SCRIPT_JSC_BACKDOOR) && defined(BUILD_DEBUG)
    enum { JSTimeout = 60000 };
    Netflix_SetJSTimeoutInterval(sCurrentContext, JSTimeout, jsTimeoutCallback, 0);
#endif

    mGCTimeout = GibbonConfiguration::garbageCollectTimeout();
    const llong initial = GibbonConfiguration::garbageCollectInitialTimeout();
    if (mGCTimeout != -1 && initial != -1) {
        // hardcode the first GC timer to be 30 seconds, the timer will be restarted when it times out
        NTRACE(TRACE_UI_SCRIPT, "Initializing GC timer at 30000 ms");
        mGCTimer.reset(new JSCGCTimer(initial, GibbonConfiguration::backdoorGarbageCollect()));
        mGCTimer->start();
    }
}

int ScriptEngine::garbageCollectTimeout() const
{
    return mGCTimeout;
}

void ScriptEngine::setGarbageCollectMaxBytesPerCycle(size_t size)
{
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    Netflix_SetGCMaxBytesPerCycle(size);
#endif
}

void ScriptEngine::setGarbageCollectTimeout(int timeout)
{
    assert(Application::isAppThread());
    if (mGCTimer) {
        NTRACE(TRACE_UI_SCRIPT, "Stopping GC timer");
        GibbonApplication::instance()->stopTimer(mGCTimer);
    }
    mGCTimeout = timeout;
    if (timeout == -1)
        return;
}

void ScriptEngine::stopEngine()
{
    assert(Application::isAppThread());
    if (mStarted && mGlobal) {
        assert(sCurrentContext);
        if (mGCTimer) {
            NTRACE(TRACE_UI_SCRIPT, "Stopping GC timer");
            GibbonApplication::instance()->stopTimer(mGCTimer);
        }

        if (mConnection) {
            GibbonBridge::PendingSyncBlock scope;
            nrdApp()->removeEventConnection(mConnection);
            GibbonApplication::instance()->flushEvents();
            mConnection->shutdown();
            mConnection.reset();
        }

        for (std::set<ArrayBufferViewJSC*>::const_iterator it = mArrayBufferViews.begin(); it != mArrayBufferViews.end(); ++it)
            (*it)->clear();
        mArrayBufferViews.clear();
#ifdef GIBBON_SCRIPT_JSC_DEBUGGER
        mSavedState = Netflix_SaveDebuggerState(sCurrentContext);
        Netflix_ResetDebugger(sCurrentContext);
#endif
        mNrdpPlatform.reset();

        sUndefined.clear();
        JSC::cleanupGeneratedStrings();
        JSContextGroupRelease(mGroup);
        assert(sCurrentContext);
        JSGlobalContextRelease(const_cast<JSGlobalContextRef>(sCurrentContext));
        sCurrentContext = 0;
        JSClassRelease(mFinalizeClass);
        JSClassRelease(mArrayBufferClass.classRef);
        JSClassRelease(mDataViewClass.classRef);
        JSClassRelease(mSyncObjectClass.classRef);
        for (std::map<TypedArrayType, Class>::const_iterator it = mTypedArrayClasses.begin(); it != mTypedArrayClasses.end(); ++it)
            JSClassRelease(it->second.classRef);
        mTypedArrayClasses.clear();

        mGroup = 0;
        mGlobal = 0;
        mBridge.reset();
        mFinalizeClass = 0;
        mArrayBufferClass.classRef = 0;
        mArrayBufferClass.prototype = 0;
        mReleasedExecutableMemory = false;
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
        //assert(mStrings.empty());
        dumpOSAllocator(OSAllocator_Full);
        {
            ScopedMutex lock(mOSAllocatorChunksMutex);
            assert(mOSAllocatorChunks.empty());
        }
#endif
        assert(GibbonSyncObject::isEmpty());
    }
}

#ifdef GIBBON_SCRIPT_JSC_DEBUGGER

void ScriptEngine::attachDebugger()
{
    Netflix_AttachDebugger(currentContext());
}

void ScriptEngine::detachDebugger()
{
    Netflix_DetachDebugger(currentContext());
}

void ScriptEngine::startCPUProfiler(int profileId)
{
    if (JSContextRef ctx = currentContext()) {
        GibbonBridge::PendingSyncBlock scope;
        Netflix_StartCPUProfiler(ctx, profileId);
    }
}

static void dumpCPUCallback(void *userData, const char *fmt, ...)
{
    std::string& str = *static_cast<std::string*>(userData);
    va_list args;
    va_start(args, fmt);
    str += StringFormatter::vsformat(fmt, args);
    va_end(args);
}

std::string ScriptEngine::stopCPUProfiler(int profileId)
{
    std::string data;
    if (JSContextRef ctx = currentContext()) {
        GibbonBridge::PendingSyncBlock scope;
        Netflix_StopCPUProfiler(ctx, profileId, &data, dumpCPUCallback);
    }
    return data;
}

void ScriptEngine::setCurrentFrame(unsigned int frame)
{
    if (!mStarted || !mBreakCount)
        return;
    Netflix_SetCurrentFrame(currentContext(), frame);
}

unsigned int ScriptEngine::currentFrame() const
{
    if (!mStarted || !mBreakCount)
        return 0;
    return Netflix_GetCurrentFrame(currentContext());
}

void ScriptEngine::breakpoint(intptr_t sourceID, int line, int column, int reason, void* userData)
{
    ScopedString string;
    Netflix_ResolveSourceID(sourceID, &string);
    char filename[1024];
    JSStringGetUTF8CString(string, filename, 1024);
    /*
      printf("got %s at %s %d\n",
      (reason == ScriptEngine::Breakpoint) ? "breakpoint" : (reason == ScriptEngine::Exception) ? "exception" : "step",
      filename, line);
    */

    ScriptEngine* engine = static_cast<ScriptEngine*>(userData);
    if (engine->mGCTimer) {
        engine->mGCTimer->stop();
        engine->mGCTimer.reset();
    }

    if (!(engine->mSkipMode & (SkipDebugger|SkipEngine)))
        ++engine->mBreakCount;
    engine->breakpointCallback(filename, line, column, static_cast<ScriptEngine::BreakpointReason>(reason));
}

int ScriptEngine::ignoreExceptions(int ignore)
{
    return Netflix_IgnoreExceptions(currentContext(), ignore);
}

unsigned int ScriptEngine::setBreakpoint(const std::string &file, int line)
{
    ScopedString str(JSStringCreateWithUTF8CString(file.c_str()));
    unsigned int ret;
    Netflix_SetBreakpoint(currentContext(), str, line, &ret);
    sendDebugEvent(DebuggerListener::Breakpoints);
    return ret;
}

void ScriptEngine::setCondition(unsigned int breakpointId, const std::string &condition)
{
    ScopedString c(JSStringCreateWithUTF8CString(condition.c_str()));
    Netflix_SetConditionalBreakpoint(currentContext(), breakpointId, c);
    sendDebugEvent(DebuggerListener::Breakpoints);
}

void ScriptEngine::removeBreakpoint(unsigned int id)
{
    if (!mStarted)
        return;

    Netflix_RemoveBreakpoint(currentContext(), id);
    sendDebugEvent(DebuggerListener::Breakpoints);
}

void ScriptEngine::listBreakpoints(std::vector<ScriptEngine::Breakpoint> &breakpoints)
{
    if (!mStarted)
        return;

    Netflix_JSBreakpoint b[128];
    const int count = Netflix_ListBreakpoints(currentContext(), b, sizeof(b) / sizeof(b[0]));
    breakpoints.resize(count);
    for (int i=0; i<count; ++i) {
        breakpoints[i].url = eatString(b[i].url);
        breakpoints[i].condition = eatString(b[i].condition);
        breakpoints[i].line = b[i].line;
    }
}

void ScriptEngine::pauseOnNextStatement()
{
    if (!mStarted)
        return;
    Netflix_Pause(currentContext());
}

bool ScriptEngine::isInBreakpoint() const
{
    return mBreakCount;
}

void ScriptEngine::continueFromBreakpoint()
{
    if (!mStarted || !mBreakCount)
        return;
    --mBreakCount;
    Netflix_Continue(currentContext());
    GibbonApplication::instance()->leave();
    GibbonApplication::instance()->getBridge()->startAllTimers();
    if (!mGCTimer)
        mGCTimer.reset(new JSCGCTimer(mGCTimeout, GibbonConfiguration::backdoorGarbageCollect()));
    if (!mBreakCount)
        mConnection->continueFromBreakpoint();
}

void ScriptEngine::stepOver()
{
    if (!mStarted || !mBreakCount)
        return;
    continueFromBreakpoint();
    Netflix_StepOver(currentContext());
}

void ScriptEngine::stepInto()
{
    if (!mStarted || !mBreakCount)
        return;
    continueFromBreakpoint();
    Netflix_StepInto(currentContext());
}

void ScriptEngine::finish()
{
    if (!mStarted || !mBreakCount)
        return;
    continueFromBreakpoint();
    Netflix_Finish(currentContext());
}

void ScriptEngine::stackTrace(std::vector<StackFrame>& frames)
{
    if (!mStarted || !mBreakCount) {
        frames.clear();
        return;
    }
    Netflix_JSStackFrame f[128];
    const int count = Netflix_StackTrace(currentContext(), f, sizeof(f) / sizeof(f[0]));
    frames.resize(count);

    for (int i=0; i<count; ++i) {
        frames[i].url = eatString(f[i].url);
        frames[i].function = eatString(f[i].function);
        frames[i].line = f[i].line;
        frames[i].column = f[i].column;
    }
}

bool ScriptEngine::listIdentifiers(Variant& identifiers, int mode, int frame, bool resolve)
{
    JSStringRef* list;
    size_t count;
    identifiers.clear();
    if (!Netflix_Identifiers(currentContext(), &list, &count, mode, frame))
        return false;
    if (!count)
        return true;

    for (size_t i = 0; i < count; ++i) {
        if (resolve) {
            JSValueRef val;
            Netflix_EvaluateAtBreakpoint(currentContext(), list[i], &val, 0, frame);
            identifiers[eatString(list[i])] = JSValueToVariant(val, 0, 1);
        } else {
            identifiers[eatString(list[i])] = Variant();
        }
    }
    free(list);
    return true;
}

Variant ScriptEngine::evaluateIdentifier(const std::string& identifier, int frame)
{
    JSValueRef val;
    ScopedString id(Netflix_JSStringCreate(identifier.c_str(), identifier.size()));
    Netflix_EvaluateAtBreakpoint(currentContext(), id, &val, 0, frame);
    return JSValueToVariant(val, 0, 1);
}
#else

void ScriptEngine::attachDebugger()
{
}

void ScriptEngine::detachDebugger()
{
}

void ScriptEngine::startCPUProfiler(int)
{
}

std::string ScriptEngine::stopCPUProfiler(int)
{
    return std::string();
}

void ScriptEngine::setCurrentFrame(unsigned int)
{
}

unsigned int ScriptEngine::currentFrame() const
{
    return 0;
}

int ScriptEngine::ignoreExceptions(int)
{
    return 0;
}

unsigned int ScriptEngine::setBreakpoint(const std::string &, int)
{
    return 0;
}

void ScriptEngine::setCondition(unsigned int, const std::string &)
{
}

void ScriptEngine::removeBreakpoint(unsigned int)
{
}

void ScriptEngine::listBreakpoints(std::vector<ScriptEngine::Breakpoint> &)
{
}

void ScriptEngine::pauseOnNextStatement()
{
}

bool ScriptEngine::isInBreakpoint() const
{
    return false;
}

void ScriptEngine::continueFromBreakpoint()
{
}

void ScriptEngine::stepOver()
{
}

void ScriptEngine::stepInto()
{
}

void ScriptEngine::finish()
{
}

void ScriptEngine::stackTrace(std::vector<StackFrame> &frames)
{
    frames.clear();
}

bool ScriptEngine::listIdentifiers(Variant&, int, int, bool)
{
    return false;
}

Variant ScriptEngine::evaluateIdentifier(const std::string&, int)
{
    return Variant();
}
#endif

std::vector<std::string> ScriptEngine::completions(const std::string &commandLine)
{
    GibbonBridge::PendingSyncBlock scope;
    std::vector<std::string> alternatives;
    JSObjectRef obj = mGlobal;
    const char *ch = commandLine.c_str();
    std::string cmd = commandLine;
    const char *start = 0;
    while (*ch && obj) {
        while (std::isspace((unsigned char)*ch))
            ++ch;
        if (!start)
            start = ch;
        const char *last = ch;
        while (*ch && *ch != '.')
            ++ch;
        const bool fullWord = (*ch == '.');
        int len = ch - last;
        if (*ch)
            ++ch;
        const bool atEnd = !*ch;

        while (len > 0 && std::isspace((unsigned char)*(last + len - 1)))
            --len;
        const std::string word(last, len);
        Netflix_JSPropertyNameArrayRef names = Netflix_JSObjectPropertyNames(currentContext(), obj);
        const size_t count = Netflix_JSPropertyNameArrayGetCount(names);
        bool found = false;
        for (size_t i=0; i<count; ++i) {
            size_t size;
            const JSChar* ref = Netflix_JSPropertyNameArrayGetNameAtIndex(currentContext(), names, i, &size);
            std::string prop = toStdString(ref, size);
            if (fullWord) {
                if (prop == word) {
                    found = true;
                    JSValueRef val = Netflix_JSObjectGetProperty(currentContext(), obj, names, i, 0);
                    if (!JSValueIsObject(currentContext(), val)) {
                        obj = 0;
                    } else {
                        obj = JSValueToObject(currentContext(), val, 0);
                        assert(obj);
                        if (fullWord && atEnd) {
                            Netflix_JSPropertyNameArrayRef n = Netflix_JSObjectPropertyNames(currentContext(), obj);
                            const size_t c = Netflix_JSPropertyNameArrayGetCount(n);
                            for (size_t j=0; j<c; ++j) {
                                ref = Netflix_JSPropertyNameArrayGetNameAtIndex(currentContext(), n, j, &size);
                                const std::string prop = commandLine + toStdString(ref, size);
                                alternatives.push_back(prop);
                            }
                            Netflix_JSPropertyNameArrayRelease(currentContext(), n);
                        }
                    }
                    break;
                }
            } else if (startsWith(prop, word)) {
                prop.insert(0, start, last - start);
                alternatives.push_back(prop);
                found = true;
            }
        }

        Netflix_JSPropertyNameArrayRelease(currentContext(), names);
        if (!found) {
            break;
        }
    }

    return alternatives;
}

void ScriptEngine::shutdown()
{
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    Netflix_TerminateScavengerThread();
#endif
}

void ScriptEngine::evaluate(const DataBuffer &script, const AseUrl &url,
                            Variant *exception, Variant *result, int recurseDepth)
{
    GibbonBridge::PendingSyncBlock scope;

    INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("gibbon.jsc.evaluate", script.data()), Variant::StringMap("url", url.str())("size", script.size()));
    if (!mStarted || !ScriptEngine::currentContext() || script.isEmpty()) {
        INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("gibbon.jsc.evaluate", script.data()));
        return;
    }

    const char* scriptCode = script.data<const char>();
    // find source maps
    {
        const char* maps = strstr(scriptCode, "//# sourceMappingURL=");
        if (!maps) {
            addResourceUrl(url);
        } else {
            // find either eof or newline
            maps += 21;
            const char* nl = strstr(maps, "\n");
            std::string mapsFile;
            if (!nl)
                mapsFile = maps;
            else
                mapsFile = std::string(maps, nl - maps);
            addResourceUrl(url, mapsFile);
        }
    }

    if (shared_ptr<DebuggerBridge> debugger = GibbonApplication::instance()->getDebugger())
        debugger->updateLocation(url);

    ScopedString code(Netflix_JSStringCreate(scriptCode, script.length()));
    ScopedString file(Netflix_JSStringCreate(url.str().c_str(), url.str().length()));

    JSValueRef e = 0, r = 0;
#ifdef GIBBON_SCRIPT_JSC_DEBUGGER
    if (mBreakCount)
        Netflix_EvaluateAtBreakpoint(ScriptEngine::currentContext(), code, &r, 0, 0);
    else
#endif
        r = JSEvaluateScript(ScriptEngine::currentContext(), code, mGlobal, file, 0, &e);

    if (result && r) {
        if (recurseDepth != -1) {
            *result = JSValueToVariant(r, 0, recurseDepth);
        } else {
            *result = shared_ptr<Variant::CustomData>(new JSC::ValueCustom(JSC::ValueCustom::Protect, r));
        }
    }

    if (e) {
        Log::error(TRACE_GIBBON_SCRIPT, "gibbon: uncaught exception in file '%s'\n%s\nError: '%s'\n", url.str().c_str(),
                   formatException(e, false).c_str(),
                   JSValueToString(e).c_str());
        if (exception)
            *exception = JSValueToVariant(e);

        LogException(e, url.str());
    }
    dirtyGCTimer();
    INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("gibbon.jsc.evaluate", script.data()));
}

#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
class GibbonJSHeapInfoEvent : public GibbonApplication::Event
{
public:
    GibbonJSHeapInfoEvent() : mMutex(netflix::ZERO_MUTEX, "GibbonJSHeapInfoEvent"), mDone(false) {}
    ~GibbonJSHeapInfoEvent()
    {
        ScopedMutex lock(mMutex);
        mDone = true;
        mCond.broadcast();
    }

    std::string describe() const { return "GibbonJSHeapInfoEvent";  }

    virtual void eventFired()
    {
        mStats = gibbonApp()->getScriptEngine()->stats();
        done();
    }
    virtual void eventCanceled() { done(); }

    ScriptEngine::Stats wait()
    {
        ScopedMutex lock(mMutex);
        while(!mDone)
            mCond.wait(&mMutex);
        return mStats;
    }
protected:
    void done()
    {
        ScopedMutex lock(mMutex);
        mDone = true;
        mCond.broadcast();
    }

    ScriptEngine::Stats mStats;
    Mutex mMutex;
    ConditionVariable mCond;
    bool mDone;
};

ScriptEngine::Stats ScriptEngine::stats() const
{
    Stats ret;
    if(Application::isAppThread() && currentContext()) {
        GibbonBridge::PendingSyncBlock scope;
        Netflix_HeapInfo(currentContext(), &ret.heapSize, &ret.heapCapacity, &ret.objectCount,
                         &ret.globalObjectCount, &ret.protectedObjectCount, &ret.protectedGlobalObjectCount);
        queryMalloc(&ret.fastMallocSystem, &ret.fastMallocMetadata,
                    &ret.fastMallocFreebytes, &ret.fastMallocReturned,
                    &ret.jsStringCount, &ret.jsStringsSize,
                    &ret.osAllocatorCommitted, &ret.osAllocatorDecommitted);
    } else {
        shared_ptr<GibbonJSHeapInfoEvent> event(new GibbonJSHeapInfoEvent);
        if(gibbonApp()->postEvent(event))
            ret = event->wait();
    }
    return ret;
}

static void dumpHeapCallback(void *userData, const char *fmt, ...)
{
    FILE **f = reinterpret_cast<FILE**>(userData);
    if (*f) {
        va_list args;
        va_start(args, fmt);
        const std::string str = StringFormatter::vsformat(fmt, args);
        va_end(args);
        if (!fwrite(str.c_str(), str.size(), 1, *f)) {
            fclose(*f);
            *f = 0;
        }
    }
}

void ScriptEngine::heap(FILE *f) const
{
    if (currentContext()) {
        GibbonBridge::PendingSyncBlock scope;
        if(!Debug::TestScriptDumpHeapNoGC)
            Netflix_GarbageCollect(currentContext());
        Netflix_DumpJSHeap(currentContext(), &f, dumpHeapCallback);
    }
}

void ScriptEngine::dump(std::string& heap)
{
    if (currentContext()) {
        GibbonBridge::PendingSyncBlock scope;
        if(!Debug::TestScriptDumpHeapNoGC)
            Netflix_GarbageCollect(currentContext());
        heap = jsCallbackToString(currentContext(), Netflix_DumpJSHeap);
#if 0
        {
            static int serial = 0;
            const std::string file = StringFormatter::sformat("/tmp/jsheap_%d.js", serial++);
            if(FILE *f = fopen(file.c_str(), "w")) {
                Log::error(TRACE_LOG, "[%s:%d]: %s", __FILE__, __LINE__, file.c_str());
                fwrite(heap.c_str(), heap.length(), 1, f);
                fclose(f);
            }
        }
#endif
    }
}

void ScriptEngine::dumpCounts(std::string & result)
{
    result = jsCallbackToString(currentContext(), Netflix_DumpJSHeapCounts);
}

#else
ScriptEngine::Stats ScriptEngine::stats() const
{
    return Stats();
}

void ScriptEngine::heap(FILE *) const
{
}

void ScriptEngine::dump(std::string&)
{
}

void ScriptEngine::dumpCounts(std::string &)
{
}

#endif

size_t ScriptEngine::garbageCollect()
{
    GibbonBridge::PendingSyncBlock scope;
    assert(Application::isAppThread());
    if (mStarted && currentContext()) {
        JSContextRef ctx = ScriptEngine::currentContext();
        assert(ctx);
        const size_t ret = Netflix_GarbageCollect(ctx);
        return ret;
    }
    return 0;
}

Variant ScriptEngine::createTypedArray(TypedArrayType type, const DataBuffer &buffer)
{
    return shared_ptr<Variant::CustomData>(new TypedArrayCustom(type, buffer));
}

Variant ScriptEngine::createString(const DataBuffer &buffer)
{
    return shared_ptr<Variant::CustomData>(new StringCustom(buffer));
}

Variant ScriptEngine::parseJSON(const char *data, int length, bool *ok, int *used) const
{
    GibbonBridge::PendingSyncBlock scope;
    if (ok)
        *ok = false;
    if (used)
        *used = 0;
    assert(Application::isAppThread());
    if (!length || !data || !currentContext())
        return Variant();
    JSONParserJSC parser(currentContext());
    JSValueRef val = parser.parse(data, length);
    if (JSValueIsUndefined(currentContext(), val)) {
        return Variant();
    }

    shared_ptr<Variant::CustomData> custom(new ValueCustom(val)); // already protected
    if (used)
        *used = parser.current() - data;
    if (ok)
        *ok = true;
    return custom;
}

Variant ScriptEngine::parseXML(const char *data, int length, bool *ok, int *used) const
{
    GibbonBridge::PendingSyncBlock scope;
    if (ok)
        *ok = false;
    if (used)
        *used = 0;
    assert(Application::isAppThread());
    if (!length || !data || !currentContext())
        return Variant();
    XMLParserJSC parser(ScriptEngine::currentContext());
    shared_ptr<XMLParser::Value> ret = parser.parse(data, length);
    if (!ret) {
        return Variant();
    }
    shared_ptr<Variant::CustomData> custom(new ValueCustom(reinterpret_cast<XMLParserJSC::JSCObject*>(ret.get())->object)); // already protected

    if (ok)
        *ok = true;
    if (used)
        *used = parser.current() - data;

    return custom;
}

#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
void ScriptEngine::mallocCallback(ullong systemBytes)
{
    if (systemBytes >= static_cast<unsigned int>(GibbonConfiguration::jsHighWaterMark())) {
        Log::error(TRACE_GIBBON_SCRIPT, "JS capacity reached: %llu (threshold %lld)",
                   systemBytes, GibbonConfiguration::jsHighWaterMark());
        GibbonApplication::instance()->getBridge()->reportHighWatermark(systemBytes);
    }

    if (!GibbonConfiguration::jsReportStack())
        return;

    if (!GibbonApplication::instance()->isAppThread()) {
        Log::error(TRACE_GIBBON_SCRIPT, "FastMalloc callback from non-appthread");
        return;
    }

    ScriptEngine::SharedPtr engine = GibbonApplication::instance()->getScriptEngine();
    if (!engine || !engine->currentContext())
        return;

    const std::string stack = jsCallbackToString(engine->currentContext(), Netflix_Global_Stacktrace);
    if (!stack.empty()) {
        Log::error(TRACE_GIBBON_SCRIPT, "FastMalloc: %llu\n%s", systemBytes, stack.c_str());
    } else {
        Log::error(TRACE_GIBBON_SCRIPT, "FastMalloc: %llu (no stack trace)", systemBytes);
    }
}
#endif

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS

void ScriptEngine::OSAllocatorJoin(std::map<char*, OSAllocatorChunk>::iterator it)
{
    assert(MutexStack::locked(mOSAllocatorChunksMutex.name()));
    assert(it == mOSAllocatorChunks.find(it->first));
    if (it != mOSAllocatorChunks.begin()) {
        std::map<char*, OSAllocatorChunk>::iterator prev = it;
        --prev;
        if (prev->first + prev->second.size == it->first
            && it->second.committed == prev->second.committed) { // contiguous
            prev->second.size += it->second.size;
            mOSAllocatorChunks.erase(it--);
        }
    }
    std::map<char*, OSAllocatorChunk>::iterator next = it;
    ++next;
    if (next != mOSAllocatorChunks.end()
        && it->first + it->second.size == next->first
        && it->second.committed == next->second.committed) { // contiguous
        it->second.size += next->second.size;
        mOSAllocatorChunks.erase(next);
    }
}

#ifdef NRDP_OSALLOCATOR_LOGS_ENABLED
void ScriptEngine::checkOSAllocatorLog()
{
    FILE *f = fopen("/tmp/osallocator.logs", "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            char *str = strstr(line, "GIBBON_SCRIPT(warn): OSALLOCATOR: ");

            if (str) {
                str += 34;
                char *endPtr = 0;
                void *ptr = 0;
                size_t size = 0;
                void (*func)(void *, size_t) = 0;
                if (!strncmp("allocate ", str, 9)) {
                    ptr = reinterpret_cast<void*>(strtoul(str + 9, &endPtr, 0));
                    func = &OSAllocatorAllocate;
                } else if (!strncmp("commit ", str, 7)) {
                    ptr = reinterpret_cast<void*>(strtoul(str + 7, &endPtr, 0));
                    func = &OSAllocatorCommit;
                } else if (!strncmp("decommit ", str, 9)) {
                    ptr = reinterpret_cast<void*>(strtoul(str + 9, &endPtr, 0));
                    func = &OSAllocatorDecommit;
                } else if (!strncmp("release ", str, 8)) {
                    ptr = reinterpret_cast<void*>(strtoul(str + 8, &endPtr, 0));
                    func = &OSAllocatorRelease;
                } else {
                    fprintf(stderr, "Unhandled OSALLOCATOR line: %s", line);
                    continue;
                }
                if (*endPtr != ' ') {
                    fprintf(stderr, "Couldn't parse line: %s", line);
                    continue;
                }
                size = strtoul(endPtr + 1, 0, 10);
                if (!size) {
                    fprintf(stderr, "Couldn't parse line 2: %s", line);
                    continue;
                }
                if (ptr == reinterpret_cast<void*>(0xa7eaa000) && size == 126976) {
                    int idx = 0;
                    char *last = 0;
                    for (std::map<char*, OSAllocatorChunk>::const_iterator it = mOSAllocatorChunks.begin(); it != mOSAllocatorChunks.end(); ++it) {
                        fprintf(stderr, "%d/%zu: %p-%p (%zu-%zu) (%zu bytes) %scommitted (distance %d)\n", ++idx, mOSAllocatorChunks.size(),
                                it->first, it->first + it->second.size,
                                reinterpret_cast<uintptr_t>(it->first), reinterpret_cast<uintptr_t>(it->first + it->second.size),
                                it->second.size, it->second.committed ? "" : "de",
                                it->first - last);
                        last = it->first;
                    }
                }
                (*func)(ptr, size);
            }
        }
        int idx = 0;
        char *last = 0;
        for (std::map<char*, OSAllocatorChunk>::const_iterator it = mOSAllocatorChunks.begin(); it != mOSAllocatorChunks.end(); ++it) {
            fprintf(stderr, "%d/%zu: %p-%p (%zu-%zu) (%zu bytes) %scommitted (distance %d)\n", ++idx, mOSAllocatorChunks.size(),
                    it->first, it->first + it->second.size,
                    reinterpret_cast<uintptr_t>(it->first), reinterpret_cast<uintptr_t>(it->first + it->second.size),
                    it->second.size, it->second.committed ? "" : "de",
                    it->first - last);
            last = it->first;
        }

        mOSAllocatorChunks.clear();
        fclose(f);
    }
}
#endif

void ScriptEngine::OSAllocatorAllocate(void *p, size_t size)
{
#ifdef NRDP_OSALLOCATOR_LOGS_ENABLED
    Log::trace(TRACE_GIBBON_SCRIPT, "OSALLOCATOR: allocate %p %zu", p, size);
#endif
    shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
    if (p) {
        char *ptr = static_cast<char*>(p);
        std::pair<std::map<char*, OSAllocatorChunk>::iterator, bool> it = engine->mOSAllocatorChunks.insert(std::make_pair(ptr, OSAllocatorChunk(size, true)));
#ifndef NDEBUG
        if (it.first != engine->mOSAllocatorChunks.begin()) {
            std::map<char*, OSAllocatorChunk>::iterator before = it.first;
            --before;
            assert(before->first + before->second.size <= it.first->first);
        }
#endif
        assert(it.second);
        engine->OSAllocatorJoin(it.first);
    }
    engine->mOSAllocatorChunksMutex.unlock();
}

void ScriptEngine::OSAllocatorStart()
{
    gibbonApp()->getScriptEngine()->mOSAllocatorChunksMutex.lock();
}

void ScriptEngine::OSAllocatorModify(char *ptr, size_t size, OSAllocatorModificationType type)
{
    if (type != ExtraRelease) {
#ifdef NRDP_OSALLOCATOR_LOGS_ENABLED
        const char *names[] = { "release", "commit", "decommit" };
        Log::trace(TRACE_GIBBON_SCRIPT, "OSALLOCATOR: %s %p %zu", names[type], ptr, size);
#endif
    } else {
        type = Release;
    }
    assert(!mOSAllocatorChunks.empty());

    std::map<char*, OSAllocatorChunk>::iterator it = mOSAllocatorChunks.lower_bound(ptr);
    size_t extraRelease = 0;
    if (it != mOSAllocatorChunks.end() && it->first == ptr) {
        const int remains = (it->second.size - size);
        switch (type) {
        case ExtraRelease:
            assert(0 && "Impossible impossibility");
            break;
        case Release:
            if (remains > 0) {
                // We're releasing the front of this chunk
                mOSAllocatorChunks[ptr + size] = OSAllocatorChunk(remains, it->second.committed);
            } else if (remains < 0) {
                extraRelease = -remains;
                // We're releasing more than one chunk
            }
            mOSAllocatorChunks.erase(it);
            break;
        case Commit:
        case Decommit:
            assert(size <= it->second.size);
            if (remains) {
                mOSAllocatorChunks[ptr + size] = OSAllocatorChunk(remains, it->second.committed);

                it->second.size = size;
                it->second.committed = (type == Commit);
            } else {
                it->second.committed = (type == Commit);
                //std::map<char*, OSAllocatorChunk>::iterator bit = mOSAllocatorChunks.lower_bound(ptr);
            }
            break;
        }
    } else {
        assert(it != mOSAllocatorChunks.begin());
        --it;
        const size_t offset = (ptr - it->first);
        assert(offset < it->second.size);
        assert(it->first < ptr);
        if (offset + size < it->second.size) { // new chunk after this one
            mOSAllocatorChunks[ptr + size] = OSAllocatorChunk(it->second.size - (offset + size), it->second.committed);
        } else if (offset + size > it->second.size) {
            assert(type == Release);
            extraRelease = (offset + size) - it->second.size;
            // We're releasing more than one chunk
        }
        it->second.size = offset;
        assert(it->second.size > 0);
        if (type != Release) {
            mOSAllocatorChunks[ptr] = OSAllocatorChunk(size, (type == Commit));
        }
    }
    if (type != Release) {
        OSAllocatorJoin(it);
    } else if (extraRelease) {
        OSAllocatorModify(ptr + size - extraRelease, extraRelease, ExtraRelease);
    }
}

void ScriptEngine::OSAllocatorRelease(void *ptr, size_t size)
{
    shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
    engine->OSAllocatorModify(static_cast<char*>(ptr), size, Release);
    engine->mOSAllocatorChunksMutex.unlock();
}

void ScriptEngine::OSAllocatorCommit(void *ptr, size_t size)
{
    shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
    engine->OSAllocatorModify(static_cast<char*>(ptr), size, Commit);
    engine->mOSAllocatorChunksMutex.unlock();
}

void ScriptEngine::OSAllocatorDecommit(void *ptr, size_t size)
{
    shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
    if (ptr) {
        engine->OSAllocatorModify(static_cast<char*>(ptr), size, Decommit);
    }
    engine->mOSAllocatorChunksMutex.unlock();
}

void ScriptEngine::dumpOSAllocator(DumpOSAllocatorMode mode)
{
    std::map<char*, OSAllocatorChunk> chunks;
    {
        ScopedMutex lock(mOSAllocatorChunksMutex);
        chunks = mOSAllocatorChunks;
    }

    int idx = 0;
    char *last = 0;
    size_t committed = 0, decommitted = 0;
    for (std::map<char*, OSAllocatorChunk>::const_iterator it = chunks.begin(); it != chunks.end(); ++it) {
        if (mode == OSAllocator_Full) {
            Log::warn(TRACE_LOG, "%d/%zu: %p-%p (%zu-%zu) (%s) %scommitted (distance %zd)", ++idx, chunks.size(),
                      it->first, it->first + it->second.size,
                      reinterpret_cast<uintptr_t>(it->first), reinterpret_cast<uintptr_t>(it->first + it->second.size),
                      prettyBytes(it->second.size).c_str(), it->second.committed ? "" : "de",
                      last ? it->first - last : 0);
            last = it->first + it->second.size;
        }
        (it->second.committed ? committed : decommitted) += it->second.size;
    }
    Log::warn(TRACE_LOG, "Totals: committed: %s decommitted: %s",
              prettyBytes(committed).c_str(), prettyBytes(decommitted).c_str());
}

void ScriptEngine::StringImplRef(void *string, const char *data8bit, const int16_t *data16bit, unsigned length)
{
    assert(Application::isAppThread());
    if(length) {
        shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
        StringImplData *data = new StringImplData;
        data->data8bit = data8bit;
        data->data16bit = data16bit;
        data->length = length;
        engine->mStrings[string] = data;
    }
}

void ScriptEngine::StringImplDeref(void *string)
{
    shared_ptr<ScriptEngine> engine = gibbonApp()->getScriptEngine();
    std::map<void*, StringImplData*>::iterator it = engine->mStrings.find(string);
    assert(it != engine->mStrings.end());
    delete it->second;
    engine->mStrings.erase(it);
}

std::string ScriptEngine::StringImplData::toString(DumpStringsMode mode) const
{
    std::string result;
    if (length) {
        if (data16bit) {
            if (mode == Raw) 
                result = std::string(reinterpret_cast<const char *>(data16bit), length * 2);
            else
                result = JSC::toStdString(reinterpret_cast<const JSChar*>(data16bit), length);
        } else {
            result = std::string(data8bit, length);
        }
        if(DataBuffer::fromRawData(result.c_str(), result.length()).isBinary())
            result = Log::hexDump(result);
    }
    return result;
}

bool ScriptEngine::dumpStrings(FILE *f, DumpStringsMode mode)
{
    assert(Application::isAppThread());
    for (std::map<void*, StringImplData*>::const_iterator it = mStrings.begin(); it != mStrings.end(); ++it) {
        const std::string str = it->second->toString(mode);
        if (str.length() && (!fwrite(str.c_str(), str.length(), 1, f) || !fwrite("\n", 1, 1, f)))
            return false;
    }
    return true;
}

ScriptEngine::StringsData ScriptEngine::stringsData() const
{
    StringsData ret;
    ret.bytes = 0;
    ret.count = mStrings.size();
    assert(Application::isAppThread());
    for (std::map<void*, StringImplData*>::const_iterator it = mStrings.begin(); it != mStrings.end(); ++it)
        ret.bytes += it->second->cost();
    return ret;
}

#endif

void ScriptEngine::queryMalloc(ullong* system, ullong* metadata, ullong* freebytes,
                               size_t* returned, size_t *stringCount, size_t *stringBytes,
                               size_t *committed, size_t *decommitted) const
{
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    Netflix_QueryMalloc(system, metadata, freebytes, returned);
    *committed = 0;
    *decommitted = 0;
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    {
        const StringsData data =  stringsData();
        *stringCount = data.count;
        *stringBytes = data.bytes;
        ScopedMutex lock(mOSAllocatorChunksMutex);
        for (std::map<char*, OSAllocatorChunk>::const_iterator it = mOSAllocatorChunks.begin(); it != mOSAllocatorChunks.end(); ++it) {
            if (it->second.committed) {
                *committed += it->second.size;
            } else {
                *decommitted += it->second.size;
            }
        }
    }
#else
    *stringCount = 0;
    *stringBytes = 0;
#endif
#else
    *system = 0;
    *metadata = 0;
    *freebytes = 0;
    *returned = 0;
    *stringCount = 0;
    *stringBytes = 0;
    *committed = 0;
    *decommitted = 0;
#endif
}

void ScriptEngine::dirtyGCTimer()
{
    if (mGCTimer && !mGCTimer->isRunning()) {
        mGCTimer->restart(mGCTimeout);
    }
}

