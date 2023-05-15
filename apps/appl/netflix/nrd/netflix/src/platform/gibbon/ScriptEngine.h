/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <set>

#include <nrdbase/tr1.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Application.h>
#include <gibbon/config.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/Resource.h>
#include <set>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/WeakPtrSet.h>
#include <nrdnet/IAsyncHttpLib.h>

#if defined(GIBBON_SCRIPTENGINE_JSC)
typedef const struct OpaqueJSContext* JSContextRef;
typedef const struct OpaqueJSContextGroup* JSContextGroupRef;
typedef struct OpaqueJSContext* JSGlobalContextRef;
typedef struct OpaqueJSValue* JSObjectRef;
typedef const struct OpaqueJSValue* JSValueRef;
typedef struct OpaqueJSClass* JSClassRef;
typedef struct OpaqueJSString *JSStringRef;
#elif defined(GIBBON_SCRIPTENGINE_V8)
# include <v8.h>
#elif defined(GIBBON_SCRIPTENGINE_MOZJS)
# include <js/js-config.h>
# include <js/jsapi.h>
#endif

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
struct ScriptEngineStringCount : public netflix::ObjectCount::Record
{
public:
    inline ScriptEngineStringCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~ScriptEngineStringCount() { }
    virtual std::string describe(void *object) const;
    virtual size_t cost(void *) const;
};

NRDP_DECLARE_OBJECTCOUNT_RECORD(ScriptEngineStringCount, ScriptEngineString);
#endif

namespace netflix {
namespace gibbon {
class GibbonBridge;
class GibbonBaseBridge;
#if defined(GIBBON_SCRIPTENGINE_JSC)
namespace JSC {
class GibbonEventConnection;
class ProtectedValue;
class ArrayBufferViewJSC;
}
class JSCGCTimer : public Application::Timer
{
public:
    JSCGCTimer(int ms, bool bgc);
    virtual std::string describe() const { return "GarbageCollectTimer"; }
    virtual void timerFired();

private:
    bool backdoorGC;
};

#elif defined(GIBBON_SCRIPTENGINE_V8)
class GibbonEventConnection;
class GibbonArrayBufferAllocator;
#elif defined(GIBBON_SCRIPTENGINE_MOZJS)
class GibbonEventConnection;
#endif
class ScriptEngineLoadRequest;

class ScriptEngine
{
public:
    typedef shared_ptr<ScriptEngine> SharedPtr;
    typedef weak_ptr<ScriptEngine> WeakPtr;

    ScriptEngine();
    virtual ~ScriptEngine();

    bool isStarted() const { return mStarted; }
    void start();
    void stop();

    inline void reload() { setLocation(mLocation, 0, DataBuffer(), mHeaders, mTimeout, mConnectTimeout, mStallTimeout); }
    inline int getLocationCount() const { return mLocationCount; }
    inline AseUrl getLocation() const { return mLocation; }
    enum LocationFlag {
        Location_Now = 0x1,
        Location_ReuseEngine = 0x2,
        Location_Included = 0x4,
        Location_BootFallback = 0x8
    };

    void setLocation(const AseUrl &url,
                     unsigned int flags,
                     const DataBuffer &source,
                     const HttpHeaders &headers = HttpHeaders(),
                     const Time &timeout = Time(),
                     const Time &connectTimeout = Time(),
                     const Time &stallTimeout = Time(),
                     const DataBuffer &body = DataBuffer(),
                     Resource::Method method = Resource::Method_Get);

    struct Stats {
        Stats()
            : heapSize(0), heapCapacity(0), objectCount(0), globalObjectCount(0), protectedObjectCount(0),
              protectedGlobalObjectCount(0), fastMallocSystem(0), fastMallocMetadata(0), fastMallocFreebytes(0),
              fastMallocReturned(0), jsStringCount(0), jsStringsSize(0), osAllocatorCommitted(0), osAllocatorDecommitted(0)
        {}

        size_t heapSize, heapCapacity;
        size_t objectCount, globalObjectCount, protectedObjectCount, protectedGlobalObjectCount;
        ullong fastMallocSystem, fastMallocMetadata, fastMallocFreebytes;
        size_t fastMallocReturned;
        size_t jsStringCount, jsStringsSize;
        size_t osAllocatorCommitted, osAllocatorDecommitted;
    };

    Stats stats() const;
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    enum DumpStringsMode {
        Raw,
        Encoded
    };
    struct StringImplData {
        inline StringImplData() : data8bit(0), data16bit(0), length(0)
        {
            NRDP_OBJECTCOUNT_REF(ScriptEngineString, this);
        }
        inline ~StringImplData()
        {
            NRDP_OBJECTCOUNT_DEREF(ScriptEngineString, this);
        }
        inline size_t cost() const {
            if(data16bit)
                return length*2;
            return length;
        }
        std::string toString(DumpStringsMode mode=Encoded) const;
        const char *data8bit;
        const int16_t *data16bit;
        unsigned length;
    };
    bool dumpStrings(FILE *f, DumpStringsMode mode);
    struct StringsData {
        unsigned int count, bytes;
    };
    StringsData stringsData() const;
#endif
    void heap(FILE *f) const;
    size_t garbageCollect();
    int garbageCollectTimeout() const;
    void setGarbageCollectTimeout(int timeout);
    void setGarbageCollectMaxBytesPerCycle(size_t maxBytesPerCycle);

    std::vector<std::string> completions(const std::string &commandLine);

    void evaluate(const DataBuffer &src, const AseUrl &url=AseUrl(),
                  Variant *exception = 0,
                  Variant *result = 0, int recurseDepth = -1);
    void dump(std::string& heap);
    void dumpCounts(std::string& result);
    int ignoreExceptions(int ignore);
    unsigned int setBreakpoint(const std::string &file, int line);
    class DebuggerListener
    {
    public:
        typedef shared_ptr<DebuggerListener> SharedPtr;
        typedef weak_ptr<DebuggerListener> WeakPtr;
        enum Type {
            Breakpoints,
            State
        };
        DebuggerListener() {}
        virtual ~DebuggerListener() {}
        virtual void onChanged(Type /*type*/) {}
    };
    void addDebuggerListener(const DebuggerListener::SharedPtr &listener);
    void removeDebuggerListener(const DebuggerListener::SharedPtr &listener);
    void setCondition(unsigned int breakPointId, const std::string &condition);
    void removeBreakpoint(unsigned int id);
    void continueFromBreakpoint();
    void pauseOnNextStatement();
    bool isInBreakpoint() const;
    enum CustomDataType
    {
        Custom_JSValue = 101,
        Custom_DataBufferSlice
    };
    struct Breakpoint
    {
        AseUrl url;
        std::string condition;
        int line;
    };
    struct StackFrame
    {
        AseUrl url;
        std::string function;
        int line, column;
    };
    enum DebuggerState {
        State_Stopped,
        State_Running,
        State_Exception
    };
    DebuggerState debuggerState() const;
    void listBreakpoints(std::vector<Breakpoint>& breakpoints);
    void stepOver();
    void stepInto();
    void finish();
    enum IdentifierMode { Global = -1, Local = 0 };
    bool listIdentifiers(Variant &identifiers, int mode = Global, int frame = 0, bool resolve = true);
    Variant evaluateIdentifier(const std::string& identifier, int frame = 0);
    void stackTrace(std::vector<StackFrame>& frames);
    void setCurrentFrame(unsigned int frame);
    void attachDebugger();
    void detachDebugger();
    unsigned int currentFrame() const;
    std::map<AseUrl, std::string> getResourceUrls() const { return mResourceUrls; }

    void queryMalloc(ullong* system, ullong* metadata, ullong* freebytes, size_t* returned,
                     size_t *stringCount, size_t *stringBytes,
                     size_t *committed, size_t *decommitted) const;

    void startCPUProfiler(int profileId);
    std::string stopCPUProfiler(int profileId);

    void setSkipAllPauses(bool skip, bool untilReload);

    inline Variant getScriptStats() const { return mStats; }
    inline void setScriptStat(const std::string &name, const Variant &value) { mStats[name] = value; }

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    enum DumpOSAllocatorMode {
        OSAllocator_Summary,
        OSAllocator_Full
    };
    void dumpOSAllocator(DumpOSAllocatorMode mode);
#endif

    Variant parseJSON(const char *data, int length, bool *ok = 0, int *used = 0) const;
    Variant parseXML(const char *data, int length, bool *ok = 0, int *used = 0) const;

    enum TypedArrayType {
        Type_None = 0x000,
        Type_8 = 0x008,
        Type_16 = 0x010,
        Type_32 = 0x020,
        Type_64 = 0x040,
        Type_Uint = 0x100,
        Type_Int = 0x200,
        Type_Float = 0x400,
        Type_Clamped = 0x800,
        Type_Int8 = Type_8|Type_Int,
        Type_Uint8 = Type_8|Type_Uint,
        Type_Uint8Clamped = Type_Uint8|Type_Clamped,
        Type_Int16 = Type_16|Type_Int,
        Type_Uint16 = Type_16|Type_Uint,
        Type_Int32 = Type_32|Type_Int,
        Type_Uint32 = Type_32|Type_Uint,
        Type_Float32 = Type_32|Type_Float,
        Type_Float64 = Type_64|Type_Float,
        Size_Mask = Type_8|Type_16|Type_32|Type_64
    };

    static Variant objectId(const shared_ptr<GibbonBaseBridge> &bridge);
    static Variant createTypedArray(TypedArrayType type, const DataBuffer &buffer);
    static Variant createString(const DataBuffer &buffer);

#if defined(GIBBON_SCRIPTENGINE_JSC)
    class ContextScope
    {
    public:
        ContextScope(JSContextRef ctx);
        ~ContextScope();
    private:
        JSContextRef mPrevious;
        shared_ptr<GibbonBridge> mBridge;
    };

    static JSContextRef currentContext() { return sCurrentContext; }
    void dirtyGCTimer();
    JSClassRef arrayBufferClass() const { return mArrayBufferClass.classRef; }
    JSObjectRef arrayBufferPrototype() const { return mArrayBufferClass.prototype; }
    JSClassRef typedArrayClass(TypedArrayType type) const { return mapValue(mTypedArrayClasses, type).classRef; }
    JSObjectRef typedArrayPrototype(TypedArrayType type) const { return mapValue(mTypedArrayClasses, type).prototype; }
    JSClassRef dataViewClass() const { return mDataViewClass.classRef; }
    JSObjectRef dataViewPrototype() const { return mDataViewClass.prototype; }
    JSClassRef syncObjectClass() const { return mSyncObjectClass.classRef; }
    JSObjectRef syncObjectPrototype() const { return mSyncObjectClass.prototype; }

    void addArrayBufferView(JSC::ArrayBufferViewJSC *view) { mArrayBufferViews.insert(view); }
    void removeArrayBufferView(JSC::ArrayBufferViewJSC *view) { mArrayBufferViews.erase(view); }
    int arrayBufferViewCount() const { return mArrayBufferViews.size(); }
    static const JSC::ProtectedValue &undefined() { return sUndefined; }
#elif defined(GIBBON_SCRIPTENGINE_V8)
    int scriptAllocated() const;
    inline shared_ptr<GibbonEventConnection>& getEventConnection() { return mEventConnection; }
    inline const shared_ptr<GibbonEventConnection>& getEventConnection() const { return mEventConnection; }
    inline const v8::Persistent<v8::Context>& getV8Context() const { return mContext; }
    shared_ptr<GibbonBridge>& getBridge() { return mBridge; }
    v8::Isolate* getIsolate() const { return mIsolate; }
#elif defined( GIBBON_SCRIPTENGINE_MOZJS)
    inline GibbonEventConnection *getEventConnection() const { return mEventConnection; }
    inline JSContext *getMOZJSContext() const { return mContext; }
#endif

private:
    void init();
    void showErrorPage(const Variant &params);

    void loadLocalScript(const AseUrl &url);
    void injectJS();
    friend class ScriptEngineLoadRequest;
    void startEngine();
    void stopEngine();
    void shutdown();
    inline void addResourceUrl(const AseUrl &url, const std::string &sourceMap = std::string())
    {
        if (url.HasScheme())
            mResourceUrls[url] = sourceMap;
    }

    bool mStarted;
    Variant mStats;
    int mSavedState;
    std::set<ScriptEngineLoadRequest*> mRequests;
#if defined(GIBBON_SCRIPTENGINE_JSC)
    bool mReleasedExecutableMemory;
    JSContextGroupRef mGroup;
    static JSContextRef sCurrentContext;
    friend class ContextScope;
    JSObjectRef mGlobal;
    static JSC::ProtectedValue sUndefined;
    shared_ptr<GibbonBridge> mBridge;
    shared_ptr<JSC::ProtectedValue> mNrdpPlatform;
    shared_ptr<JSC::GibbonEventConnection> mConnection;
    shared_ptr<JSCGCTimer> mGCTimer;
    int mGCTimeout, mBreakCount;
    std::set<JSC::ArrayBufferViewJSC*> mArrayBufferViews;
    struct Class {
        JSClassRef classRef;
        JSObjectRef prototype;
    };
    JSClassRef mFinalizeClass;
    Class mArrayBufferClass, mDataViewClass, mSyncObjectClass;
    std::map<TypedArrayType, Class> mTypedArrayClasses;

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    static void OSAllocatorAllocate(void *ptr, size_t size);
    static void OSAllocatorRelease(void *ptr, size_t size);
    static void OSAllocatorCommit(void *ptr, size_t size);
    static void OSAllocatorDecommit(void *ptr, size_t size);
    static void OSAllocatorStart();
    enum OSAllocatorModificationType {
        Release,
        Commit,
        Decommit,
        ExtraRelease
    };
    void OSAllocatorModify(char *ptr, size_t size, OSAllocatorModificationType type);

    struct OSAllocatorChunk {
        OSAllocatorChunk(size_t s = 0, bool c = false)
            : size(s), committed(c)
        {}
        size_t size;
        bool committed;
    };

    void OSAllocatorJoin(std::map<char*, OSAllocatorChunk>::iterator it);

    std::map<char*, OSAllocatorChunk> mOSAllocatorChunks;
    mutable Mutex mOSAllocatorChunksMutex;
#ifdef NRDP_OSALLOCATOR_LOGS_ENABLED
    void checkOSAllocatorLog();
#endif

    std::map<void*, StringImplData*> mStrings;
    static void StringImplRef(void *string, const char *data8bit, const int16_t *data16bit, unsigned length);
    static void StringImplDeref(void *);
#endif
    friend class JSCGCTimer;
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    static void mallocCallback(ullong systemBytes);
#endif
#ifdef GIBBON_SCRIPT_JSC_DEBUGGER
    static void breakpoint(intptr_t sourceID, int line, int column, int reason, void* userData);
#endif
    static JSValueRef jscBridgeEnabled(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef [], JSValueRef *);
    static JSValueRef jscBridgeInit(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscBridgeSetProperty(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscBridgeInvoke(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscLocation(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef [], JSValueRef *);
    static JSValueRef jscLog(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *);
    static JSValueRef jscParseJSON(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argCount, const JSValueRef args[], JSValueRef* exception);
    static JSValueRef jscParseXML(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argCount, const JSValueRef args[], JSValueRef* exception);
    static JSValueRef jscCompress(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argCount, const JSValueRef args[], JSValueRef* exception);
    static JSValueRef jscUncompress(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argCount, const JSValueRef args[], JSValueRef* exception);
    static JSValueRef jscutf8toa(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef*);
    static JSValueRef jscatoutf8(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef*);
    static JSValueRef jscatob(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscbtoa(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscrandom(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscMakeGCTag(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscMono(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscPMono(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef jscGlobal(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception);
#elif defined(GIBBON_SCRIPTENGINE_V8)
    v8::Persistent<v8::Context> mContext;
    v8::Isolate* mIsolate;
    shared_ptr<GibbonBridge> mBridge;
    shared_ptr<GibbonEventConnection> mEventConnection;
    GibbonArrayBufferAllocator *mArrayBufferAllocator;
    static void v8BridgeInit(const v8::FunctionCallbackInfo<v8::Value>& args);
    void bridgeInit(v8::Handle<v8::ObjectTemplate> o);
    void bridgeCleanup();
#elif defined(GIBBON_SCRIPTENGINE_MOZJS)
    JSRuntime *mRuntime;
    JSContext *mContext;
    GibbonEventConnection *mEventConnection;
    static JSBool mozjsBridgeInit(JSContext *cx, uintN argc, jsval *vp);
    void bridgeInit(JSObject *o);
    void bridgeCleanup();
#endif

    enum BreakpointReason { Reason_Breakpoint, Reason_Exception, Reason_Step, Reason_CallstackSize };
    void breakpointCallback(const std::string& file, int line, int column, BreakpointReason reason);

    void sendDebugEvent(DebuggerListener::Type);

    DebuggerState mDebuggerState;
    mutable Mutex mMutex;
    WeakPtrSet<DebuggerListener> mDebuggerListeners;
    std::map<AseUrl, std::string> mResourceUrls;

    AseUrl mLocation;
    int mLocationCount;
    HttpHeaders mHeaders;
    Time mTimeout, mConnectTimeout, mStallTimeout;

    enum SkipMode { SkipDebugger = 0x1, SkipEngine = 0x2, SkipDebuggerUntilReload = 0x4 };
    unsigned int mSkipMode;
};

}} // namespace netflix::gibbon

#endif // SCRIPTENGINE_H
