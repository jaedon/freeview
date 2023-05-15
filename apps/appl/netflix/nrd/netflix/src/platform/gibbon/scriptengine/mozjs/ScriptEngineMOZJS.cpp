
#include <js/jsxdrapi.h>

#include <GibbonApplication.h>

#include <nrd/EventConnection.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Variant.h>

#include <iostream>

#define MOZJS_USE_JSIDARRAY

using namespace netflix;
using namespace netflix::gibbon;

#if 0
# define GIBBON_PROFILE_MOZJS
#endif

#ifdef GIBBON_PROFILE_MOZJS
struct ScriptEngineMOZJSProfiler
{
    ScriptEngineMOZJSProfiler(const std::string name) : mName(name)
    {
        mStarted = sinceStart();
        printf("JAVASCRIPT(MOZJS)! ((%lld)) [%s]\n", (mStarted-sLast).ms(), mName.c_str());
    }
    ~ScriptEngineMOZJSProfiler()
    {
        sLast = sinceStart();
        const long long time = (sLast-mStarted).ms();
        sTotalTime += time;
        printf("~JAVASCRIPT(MOZJS)! %lld [%lld] [%lld]\n", time, sTotalTime, sLast.ms());
    }
    std::string mName;
    Time mStarted;
    static Time sLast;
    static long long sTotalTime;
};
Time ScriptEngineMOZJSProfiler::sLast;
long long ScriptEngineMOZJSProfiler::sTotalTime = 0;
#endif

static std::string convertToString(JSContext *context, JSString *str) {
    JS::Anchor<JSString *> a_str(str);
    return std::string(JS_EncodeString(context, str), JS_GetStringEncodingLength(context, str));
}
static std::string convertToString(JSContext *context, jsval value)
{
    std::string result;
    if(JSVAL_IS_STRING(value)) {
        if(JSString *str = JSVAL_TO_STRING(value))
            result = convertToString(context, str);
    }
    return result;
}

static Variant convertToVariant(JSContext *context, jsval value);
static Variant convertToVariant(JSContext *context, JSObject *object)
{
    JS::Anchor<JSObject *> a_obj(object);
    Variant result;
    const bool isArray = JS_IsArrayObject(context, object);
    if(JSIdArray *properties = JS_Enumerate(context, object)) {
        jsval e;
        jsid property;
#ifdef MOZJS_USE_JSIDARRAY
        const jsint length = JS_IdArrayLength(context, properties);
#else
        const jsint length = properties->length;
#endif
        for(jsint i = 0; i < length; ++i) {
#ifdef MOZJS_USE_JSIDARRAY
            property = JS_IdArrayGet(context, properties, i);
#else
            property = properties->vector[i];
#endif
            if(JS_GetPropertyById(context, object, property, &e)) {
                const Variant v = convertToVariant(context, e);
                if(isArray) {
                    result.push_back(v);
                } else if(JSID_IS_STRING(property)) {
                    if(JSString *jsname = JSID_TO_STRING(property)) {
                        const std::string name(convertToString(context, jsname));
                        result[name] = v;
                    }
                } else {
                    assert(false);
                }
            }
        }
        JS_DestroyIdArray(context, properties);
    }
    return result;
}

static Variant convertToVariant(JSContext *context, jsval value)
{
    Variant result;
    if(JSVAL_IS_VOID(value)) {
        //nothing
    } else if(JSVAL_IS_STRING(value)) {
        result = convertToString(context, value);
    } else if(JSVAL_IS_OBJECT(value)) {
        if(JSObject *object = JSVAL_TO_OBJECT(value))
            result = convertToVariant(context, object);
    } else if(JSVAL_IS_INT(value)) {
        result = JSVAL_TO_INT(value);
    } else if(JSVAL_IS_DOUBLE(value)) {
        result = JSVAL_TO_DOUBLE(value);
    } else if(JSVAL_IS_BOOLEAN(value)) {
        result = (value == JSVAL_TRUE);
    }
    return result;
}

static jsval convertFromVariant(JSContext *context, const Variant &variant)
{
    jsval result;
    switch(variant.type()) {
    case Variant::String: {
        result = STRING_TO_JSVAL(JS_NewStringCopyZ(context, variant.string().c_str()));
        break; }
    case Variant::Array: {
        std::vector<jsval> v(variant.size());
        for(int i = 0; i < variant.size(); ++i)
            v[i] = convertFromVariant(context, variant[i]);
        result = OBJECT_TO_JSVAL(JS_NewArrayObject(context, v.size(), &v[0]));
        break; }
    case Variant::Map: {
        JSObject *o = JS_NewObject(context, 0, 0, 0);
        for(Variant::StringMapIterator it = variant.stringMapBegin(); it != variant.stringMapEnd(); ++it) {
            jsval v = convertFromVariant(context, it->second);
            JS_SetProperty(context, o, it->first.c_str(), &v);
        }
        result = OBJECT_TO_JSVAL(o);
        break; }
    case Variant::Integer:
        result = INT_TO_JSVAL(variant.integer());
        break;
    case Variant::Double:
        result = DOUBLE_TO_JSVAL(variant.dbl());
        break;
    case Variant::Boolean:
        result = variant.boolean() ? JSVAL_TRUE : JSVAL_FALSE;
        break;
    case Variant::Null:
    default:
        result = JSVAL_VOID;
        break;
    }
    return result;
}

void ScriptEngine::init()
{
    mRuntime = 0;
    mContext = 0;
    mEventConnection = 0;
}

#if !defined(GIBBON_SCRIPTENGINE_NO_COMPILE)
Variant ScriptEngine::run(ScriptEngine::Program::SharedPtr program)
{
#ifdef GIBBON_PROFILE_MOZJS
    ScriptEngineV8Profiler profile("Run: " + program->getUrl());
#endif
    if(program->mScript) {
        jsval result;
        JSObject *global = JS_GetGlobalObject(mContext);
        JS::Anchor<JSObject *> a_obj(global);
        if(JS_ExecuteScript(mContext, global, program->mScript, &result))
            return convertToVariant(mContext, result);
        Log::error(TRACE_UI_ENGINE, "SpiderMonkey Run Error!");
        {
            jsval exception;
            if(JS_GetPendingException(mContext, &exception))
                Log::error(TRACE_UI_ENGINE, " Exception: %s", convertToVariant(mContext, exception).toJSON(true).c_str());
        }
    }
    return Variant();
}

void ScriptEngine::Program::init(const std::string &data, bool code)
{
    JSContext *context = GibbonApplication::instance()->getScriptEngine()->getMOZJSContext();
    if(code) {
#ifdef GIBBON_PROFILE_MOZJS
        ScriptEngineMOZJSProfiler profile("Compile: " + getUrl());
#endif
        JS_ClearPendingException(context);
        JSObject *global = JS_GetGlobalObject(context);
        JS::Anchor<JSObject *> a_obj(global);
        if(!(mScript = JS_CompileScript(context, global, data.c_str(), data.length(), getUrl().c_str(), 0)))
            Log::error(TRACE_UI_ENGINE, "SpiderMonkey Compile Error! %s", getUrl().c_str());
    } else {
        JSXDRState *xdr = JS_XDRNewMem(context, JSXDR_DECODE);
        JS_XDRMemSetData(xdr, (char*)data.c_str(), data.length());
        if(!JS_XDRScript(xdr, &mScript))
            Log::error(TRACE_UI_ENGINE, "SpiderMonkey Load Error! %s", getUrl().c_str());
        JS_XDRMemSetData(xdr, NULL, 0);    // this avoid the memory to being freed by JS
        JS_XDRDestroy(xdr);
    }
}

std::string ScriptEngine::Program::getData() const
{
    std::string result;
    if(mScript) {
        JSContext *context = GibbonApplication::instance()->getScriptEngine()->getMOZJSContext();
        JSXDRState*    xdr = JS_XDRNewMem(context, JSXDR_ENCODE);
        if(JS_XDRScript(xdr, &mScript)) {
            uint32_t len = 0;
            char *bytes = (char*)JS_XDRMemGetData(xdr, &len);
            result = std::string(bytes, len);
        }
        JS_XDRDestroy(xdr);
    }
    return result;
}

void ScriptEngine::Program::cleanup()
{
    if(mScript) {
#if 0
        JSContext *context = GibbonApplication::instance()->getScriptEngine()->getMOZJSContext();
        JS_DestroyScript(context, mScript);
#endif
        mScript = 0;
    }
}
#endif

Variant ScriptEngine::evaluate(const std::string &data, const std::string &filename)
{
#ifdef GIBBON_PROFILE_MOZJS
    ScriptEngineMOZJSProfiler profile("Evaluate: " + filename);
#endif
    JS_ClearPendingException(mContext);
    JSObject *global = JS_GetGlobalObject(mContext);
    JS::Anchor<JSObject *> a_obj(global);
    jsval result;
    if(JS_EvaluateScript(mContext, global, data.c_str(), data.length(), filename.c_str(), 0, &result))
        return convertToVariant(mContext, result);
    Log::error(TRACE_UI_ENGINE, "SpiderMonkey Evaluate Error!");
    {
        jsval exception;
        if(JS_GetPendingException(mContext, &exception))
            Log::error(TRACE_UI_ENGINE, " Exception: %s", convertToVariant(mContext, exception).toJSON(true).c_str());
    }
    return Variant();
}

static void mozjsErrorReport(JSContext *, const char *message, JSErrorReport *report)
{
    Log::error(TRACE_UI_ENGINE, "SpiderMonkey Error: %s (%s:%d)", message, report->filename, report->lineno);
}

void ScriptEngine::startEngine()
{
    JS_SetCStringsAreUTF8();
    mRuntime = JS_NewRuntime(160L * 1024 * 1024);
    mContext = JS_NewContext(mRuntime, 8192);
    JS_SetErrorReporter(mContext, mozjsErrorReport);
    uint32_t options = JS_GetOptions(mContext) | JSOPTION_VAROBJFIX;
#if 1
    options |= JSOPTION_METHODJIT;
#endif
    JS_SetOptions(mContext, options);
    JS_SetVersion(mContext, JSVERSION_LATEST);
    JS_SetGCParameterForThread(mContext, JSGC_MAX_CODE_CACHE_BYTES, 16 * 1024 * 1024);

    static JSClass global_class = { "global", JSCLASS_GLOBAL_FLAGS,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS };

    JSObject *globalObject = JS_NewCompartmentAndGlobalObject(mContext, &global_class, 0);
    JS_InitStandardClasses(mContext, globalObject);
    bridgeInit(globalObject);
}

void ScriptEngine::stopEngine()
{
    bridgeCleanup();
    JS_DestroyContext(mContext);
    JS_DestroyRuntime(mRuntime);
    JS_ShutDown();
}

size_t ScriptEngine::garbageCollect()
{
    JS_GC(mContext);
    return 0;
}

namespace netflix {
namespace gibbon {
class GibbonEventConnection : public Thread, public EventConnection
{
public:
    GibbonEventConnection(jsval callback) : Thread("GibbonEventConnectionMOZJS"), mCallback(callback), mContext(0),
                                            mMutex(GIBBON_EVENTCONNECTIONMOZJS_MUTEX, "GibbonEventConnectionMOZJS")
    {
        Start();
    }

    ~GibbonEventConnection()
    {
        Finish();
        Wait();
    }

    virtual void Finish() {
        ScopedMutex _lock(mMutex);
        mCallback = JSVAL_VOID;
        mConditionVariable.signal();
    }

    class Context
    {
    public:
        Context(GibbonEventConnection *connection, bool add=true) : mConnection(connection) {
            if(add)
                mConnection->addContext(this);
        }
        virtual ~Context() { mConnection->removeContext(this); }

        inline void enqueue(const Variant &v) { mSend.push_back(v); }
        inline Variant send() {
            ScopedMutex _lock(mConnection->mMutex);
            mConnection->removeContext(this);
            return mSend;
        }
    protected:
        friend class GibbonEventConnection;
        GibbonEventConnection *mConnection;
        Variant mSend;
    };

protected:
    virtual void Run() {
        NBP::instance()->addEventConnection(this);
        {
            ScopedMutex _lock(mMutex);
            if(!JSVAL_IS_VOID(mCallback))
                mConditionVariable.wait(&mMutex);
        }
        NBP::instance()->removeEventConnection(this);
    }

    void addContext(Context *context)
    {
        ScopedMutex _lock(mMutex);
        if(!mContext) {
            mContext = context;
            if(mEvent) {
                mContext->mSend = mEvent->mSend;
                mEvent->mSend.clear();
            }
        }
    }

    void removeContext(Context *context)
    {
        ScopedMutex _lock(mMutex);
        if(mContext == context)
            mContext = 0;
    }

    friend class GibbonBridgeObject;
    class GibbonJSEvent : public GibbonApplication::Event, public Context
    {
    public:
        typedef shared_ptr<GibbonJSEvent> SharedPtr;
        GibbonJSEvent(GibbonEventConnection *c) : Context(c, false) { }
        std::string describe() const { return std::string("GibbonJSEvent"); }
        void eventFired() {
            {
                ScopedMutex _lock(mConnection->mMutex);
                assert(mConnection->mEvent.get() == this);
                mConnection->mEvent.reset();
            }
            if(!mSend.isNull() && !JSVAL_IS_VOID(mConnection->mCallback)) {
#ifdef GIBBON_PROFILE_MOZJS
                char buffer[256];
                sprintf(buffer, "%s/%d", mSend[0].contains("name") ? mSend[0]["name"].string().c_str() : "NA", mSend.size());
                ScriptEngineMOZJSProfiler profile(buffer);
#endif
                JSContext *context = GibbonApplication::instance()->getScriptEngine()->getMOZJSContext();
                jsval events = convertFromVariant(context, mSend), result;
                JS_CallFunctionValue(context, 0, mConnection->mCallback, 1, &events, &result);
            }
        }
    };

    virtual bool writeResponse(const Response &response) {
        ScopedMutex _lock(mMutex);
        if(mContext) {
            mContext->enqueue(response.toVariantMap());
        } else {
            if(!mEvent) {
                mEvent = GibbonJSEvent::SharedPtr(new GibbonJSEvent(this));
                GibbonApplication::instance()->postEvent(mEvent);
            }
            mEvent->enqueue(response.toVariantMap());
        }
        return true;
    }

private:
    jsval mCallback;
    ConditionVariable mConditionVariable;

    Context *mContext;
    GibbonJSEvent::SharedPtr mEvent;
    Mutex mMutex;
};
}
}

JSBool ScriptEngine::mozjsBridgeInit(JSContext *cx, uintN, jsval *vp)
{
    assert(!GibbonApplication::instance()->getScriptEngine()->mEventConnection);
    jsval callback = JS_ARGV(cx, vp)[0];
    if(!GibbonApplication::instance()->getScriptEngine()->mEventConnection) {
        GibbonApplication::instance()->getScriptEngine()->mEventConnection = new GibbonEventConnection(callback);
        JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    } else {
        JS_SET_RVAL(cx, vp, JSVAL_FALSE);
    }
    return JS_TRUE;
}

static JSBool mozjsBridgeEnabled(JSContext *cx, uintN, jsval *vp)
{
    JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    return JS_TRUE;
}

static JSBool mozjsBridgeConstruct(JSContext *cx, uintN, jsval *vp)
{
    std::map<std::string, Variant> variantMap;
    variantMap["object"] = convertToString(cx, JS_ARGV(cx, vp)[0]);
    variantMap["construct"] = convertToString(cx, JS_ARGV(cx, vp)[1]);

    {
        if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[2])) {
            Variant variant = convertToVariant(cx, JS_ARGV(cx, vp)[2]);
            assert(variant.isMap());
            for(Variant::StringMapIterator it = variant.begin(); it != variant.end(); ++it)
                variantMap[it->first] = it->second;
        } else if(JSVAL_IS_VOID(JS_ARGV(cx, vp)[2])) {
        } else {
            assert(false);
        }
    }

    const Request nbpRequest(Variant(variantMap), Request::FromTrustedSource);
    JS_SET_RVAL(cx, vp, convertFromVariant(cx, NBP::instance()->send(nbpRequest)));
    return JS_TRUE;
}

static JSBool mozjsBridgeSetProperty(JSContext *cx, uintN, jsval *vp)
{
    std::map<std::string, Variant> variantMap;
    variantMap["object"] = convertToString(cx, JS_ARGV(cx, vp)[0]);
    variantMap["property"] = convertToString(cx, JS_ARGV(cx, vp)[1]);
    variantMap["value"] = convertToVariant(cx, JS_ARGV(cx, vp)[2]);

    jsval result;
    {
        const Request nbpRequest(Variant(variantMap), Request::FromTrustedSource);
        GibbonEventConnection::Context context(GibbonApplication::instance()->getScriptEngine()->getEventConnection());
        NBP::instance()->post(nbpRequest);
        result = convertFromVariant(cx, context.send());
    }
    JS_SET_RVAL(cx, vp, result);
    return JS_TRUE;
}

static JSBool mozjsBridgeInvoke(JSContext *cx, uintN, jsval *vp)
{
    std::map<std::string, Variant> variantMap;
    variantMap["object"] = convertToString(cx, JS_ARGV(cx, vp)[0]);
    variantMap["method"] = convertToString(cx, JS_ARGV(cx, vp)[1]);
    {
        //js_Stringify(cx, Jsvalify(JS_ARGV(cx, vp)+2), 0, Int32Value(1),
        if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[2])) {
            Variant variant = convertToVariant(cx, JS_ARGV(cx, vp)[2]);
            assert(variant.isMap());
            for(Variant::StringMapIterator it = variant.begin(); it != variant.end(); ++it)
                variantMap[it->first] = it->second;
        } else if(JSVAL_IS_VOID(JS_ARGV(cx, vp)[2])) {
        } else {
            assert(false);
        }
    }

    jsval result;
    {
        const Request nbpRequest(Variant(variantMap), Request::FromTrustedSource);
        GibbonEventConnection::Context context(GibbonApplication::instance()->getScriptEngine()->getEventConnection());
        NBP::instance()->post(nbpRequest);
        result = convertFromVariant(cx, context.send());
    }
    JS_SET_RVAL(cx, vp, result);
    return JS_TRUE;
}

static JSBool mozjsEval(JSContext *cx, uintN argc, jsval *vp)
{
    const std::string script = convertToString(cx, JS_ARGV(cx, vp)[0]);
    const std::string file = argc >= 2 ? convertToString(cx, JS_ARGV(cx, vp)[1]) : std::string();
    JS_SET_RVAL(cx, vp, convertFromVariant(cx, GibbonApplication::instance()->getScriptEngine()->evaluate(script, file)));
    return JS_TRUE;
}

static JSBool mozjsLog(JSContext *cx, uintN, jsval *vp)
{
    const std::string msg = convertToString(cx, JS_ARGV(cx, vp)[0]);
    if(!startsWith(msg, "NETFLIX:"))
        NTRACE(TRACE_UI_SCRIPT, "%s", msg.c_str());
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

static JSBool mozjsConsole(JSContext *cx, uintN, jsval *vp)
{
    const std::string msg = convertToString(cx, JS_ARGV(cx, vp)[0]);
    fprintf(stderr, "Console[%lld]: %s\n", NrdApplication::timeSinceStart().ms(), msg.c_str());
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

void ScriptEngine::bridgeCleanup()
{
    delete mEventConnection;
    mEventConnection = 0;
}

void ScriptEngine::bridgeInit(JSObject *o)
{
    mEventConnection = 0;
    JSObject *nrdp_platform = JS_NewObject(mContext, 0, 0, 0);
    JS_DefineFunction(mContext, nrdp_platform, "mozjsBridgeInit", mozjsBridgeInit, 1, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "mozjsBridgeEnabled", mozjsBridgeEnabled, 0, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "mozjsBridgeConstruct", mozjsBridgeConstruct, 3, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "mozjsBridgeSetProperty", mozjsBridgeSetProperty, 3, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "mozjsBridgeInvoke", mozjsBridgeInvoke, 3, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "console", mozjsConsole, 1, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "eval", mozjsEval, 2, JSPROP_ENUMERATE);
    JS_DefineFunction(mContext, nrdp_platform, "log", mozjsLog, 1, JSPROP_ENUMERATE);
    jsval nrdp_platform_value = OBJECT_TO_JSVAL(nrdp_platform);
    JS_SetProperty(mContext, o, "nrdp_platform", &nrdp_platform_value);
}

