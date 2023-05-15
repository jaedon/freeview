/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <ScriptEngine.h>
#include <GibbonApplication.h>
#include "JSONParserV8.h"

#include <nrd/AppLog.h>
#include <nrd/EventConnection.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Random.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Variant.h>

#include <v8.h>
#include <iostream>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonSyncObjectEffect);
NRDP_DEFINE_OBJECTCOUNT(GibbonSyncObjectImage);
NRDP_DEFINE_OBJECTCOUNT(GibbonSyncObjectText);
NRDP_DEFINE_OBJECTCOUNT(GibbonSyncObjectWidget);

class ContextHolder
{
public:
    ContextHolder();
    ~ContextHolder();

    v8::HandleScope& handleScope() { return hs; }
    v8::Context::Scope& contextScope() { return cs; }
    v8::Handle<v8::Context>& context() { return ctx; }

private:
    v8::HandleScope hs;
    v8::Handle<v8::Context> ctx;
    v8::Context::Scope cs;
};

ContextHolder::ContextHolder()
    : hs(v8::Isolate::GetCurrent()),
      ctx(v8::Local<v8::Context>::New(v8::Isolate::GetCurrent(), GibbonApplication::instance()->getScriptEngine()->getV8Context())),
      cs(ctx)
{
    GibbonApplication::instance()->getBridge()->beginPendingSync();
}

ContextHolder::~ContextHolder()
{
    GibbonApplication::instance()->getBridge()->endPendingSync();
}

class EscapableContextHolder
{
public:
    EscapableContextHolder();
    ~EscapableContextHolder();

    v8::EscapableHandleScope& handleScope() { return hs; }
    v8::Context::Scope& contextScope() { return cs; }
    v8::Handle<v8::Context>& context() { return ctx; }

private:
    v8::EscapableHandleScope hs;
    v8::Handle<v8::Context> ctx;
    v8::Context::Scope cs;
};

EscapableContextHolder::EscapableContextHolder()
    : hs(v8::Isolate::GetCurrent()),
      ctx(v8::Local<v8::Context>::New(v8::Isolate::GetCurrent(), GibbonApplication::instance()->getScriptEngine()->getV8Context())),
      cs(ctx)
{
    GibbonApplication::instance()->getBridge()->beginPendingSync();
}

EscapableContextHolder::~EscapableContextHolder()
{
    GibbonApplication::instance()->getBridge()->endPendingSync();
}

#if 0
# define GIBBON_PROFILE_V8
#endif

#ifdef GIBBON_PROFILE_V8
struct ScriptEngineV8Profiler
{
    ScriptEngineV8Profiler(const std::string name) : mName(name)
    {
        mStarted = sinceStart();
        printf("JAVASCRIPT(V8)! (%lld/%lld) [%s]\n", mStarted.ms(), (mStarted-sLast).ms(), mName.c_str());
    }
    ~ScriptEngineV8Profiler()
    {
        sLast = sinceStart();
        const long long time = (sLast-mStarted).ms();
        sTotalTime += time;
        printf("~JAVASCRIPT(V8)! %lld [%lld] [%lld]\n", time, sTotalTime, sLast.ms());
    }
    std::string mName;
    Time mStarted;
    static Time sLast;
    static long long sTotalTime;
};
Time ScriptEngineV8Profiler::sLast;
long long ScriptEngineV8Profiler::sTotalTime = 0;
#endif

struct V8DecodedData
{
    enum Type {
        Type_None,
        Type_ArrayBuffer,
        Type_String,
        Type_TypedArray
    };

    V8DecodedData()
        : type(Type_None)
    {}

    bool decode(const v8::Handle<v8::Value>& value);

    Type type;
    DataBuffer dataBuffer;

private:
    void decode(const char* chars, int len);
    void decode(const v8::Handle<v8::ArrayBuffer>& buffer, size_t offset, size_t length);
};

bool V8DecodedData::decode(const v8::Handle<v8::Value>& value)
{
    v8::HandleScope hs(v8::Isolate::GetCurrent());
    bool ok = false;
    if (value->IsString()) {
        v8::String::Utf8Value str(value);
        decode(*str, str.length());
        type = Type_String;
        ok = true;
    } else if (value->IsArrayBufferView()) {
        v8::Handle<v8::ArrayBufferView> view = v8::Handle<v8::ArrayBufferView>::Cast(value);
        decode(view->Buffer(), view->ByteOffset(), view->ByteLength());
        type = Type_TypedArray;
        ok = true;
    } else if (value->IsArrayBuffer()) {
        v8::Handle<v8::ArrayBuffer> buffer = v8::Handle<v8::ArrayBuffer>::Cast(value);
        decode(buffer, 0, buffer->ByteLength());
        type = Type_ArrayBuffer;
        ok = true;
    }
    return ok;
}

void V8DecodedData::decode(const v8::Handle<v8::ArrayBuffer>& buffer, size_t offset, size_t length)
{
    if (length) {
        dataBuffer.resize(length);
        memcpy(dataBuffer.data(), static_cast<char*>(buffer->BaseAddress()) + offset, length);
    }
}

void V8DecodedData::decode(const char* chars, int len)
{
    assert(type == Type_None);
    if (chars && len) {
        dataBuffer.resize(len);
        memcpy(dataBuffer.data(), chars, len);
    }
}

static std::string convertToString(v8::Handle<v8::Value> value)
{
    if(value->IsString()) {
        v8::String::Utf8Value strValue(value);
        return std::string(*strValue);
    }
    return std::string();
}

static Variant convertToVariant(v8::Handle<v8::Value> value)
{
    Variant result;
    if (value->IsString()) {
        // why can't I return a DataBuffer here?
        result = convertToString(value);
    } else if (value->IsTypedArray() || value->IsArrayBuffer()) {
        V8DecodedData data;
        data.decode(value);
        result = data.dataBuffer;
    } else if (value->IsArray()) {
        v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(value);
        result.resizeArray(array->Length());
        for(size_t i = 0; i < array->Length(); ++i)
            result[i] = convertToVariant(array->Get(i));
    } else if (value->IsObject()) {
        v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(value);
        v8::Local<v8::Array> properties = object->GetOwnPropertyNames();
        for(size_t i = 0; i < properties->Length(); ++i) {
            const v8::Handle<v8::Value> key = properties->Get(i);
            result[convertToString(key)] = convertToVariant(object->Get(key));
        }
    } else if (value->IsInt32()) {
        result = value->IntegerValue();
    } else if (value->IsNumber()) {
        result = value->NumberValue();
    } else if (value->IsBoolean()) {
        result = value->BooleanValue();
    } else if (value->IsNull() || value->IsUndefined()) {
    } else {
        Log::warn(TRACE_UI_SCRIPT, "Unknown value type in convertToVariant");
    }
    return result;
}

struct V8External
{
    V8External(const DataBuffer& b) : buf(b) { }

    DataBuffer buf;
    v8::Persistent<v8::Value> persistent;
};

static void v8finalize(const v8::WeakCallbackData<v8::Value, V8External>& data)
{
    v8::HandleScope scope(data.GetIsolate());
    V8External* external = data.GetParameter();
    assert(external->persistent.IsNearDeath());
    external->persistent.Reset();
    delete external;
}

struct V8ArrayBuffer
{
    V8ArrayBuffer(const DataBuffer& buf)
    {
        V8External* external = new V8External(buf);
        buffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), external->buf.data(), external->buf.size());
        external->persistent.Reset(v8::Isolate::GetCurrent(), buffer);
        external->persistent.SetWeak(external, v8finalize);
        external->persistent.MarkIndependent();
    }
    v8::Handle<v8::ArrayBuffer> buffer;
};

class ValueCustom : public Variant::CustomData
{
public:
    enum { Type = 101 };
    ValueCustom(const v8::Handle<v8::Value>& val)
        : CustomData(Type), value(v8::Isolate::GetCurrent(), val)
    {
    }
    virtual std::string toJSON(bool pretty) const
    {
        ContextHolder holder;
        v8::Local<v8::Value> val = v8::Local<v8::Value>::New(v8::Isolate::GetCurrent(), value);
        const Variant variant = convertToVariant(val);
        return variant.toJSON(pretty);
    }

    v8::Persistent<v8::Value> value;
};

class StringCustom : public Variant::CustomData
{
public:
    enum { Type = 102 };
    StringCustom(const DataBuffer& b)
        : CustomData(Type), buffer(b)
    {
    }

    virtual std::string toJSON(bool /*pretty*/) const
    {
        return buffer.toString();
    }

    v8::Handle<v8::Value> v8Value() const
    {
        v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
        v8::Local<v8::String> str = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                                            buffer.data<const char>(),
                                                            v8::String::kNormalString,
                                                            buffer.size());
        return hs.Escape(str);
    }

    DataBuffer buffer;
};

class TypedArrayCustom : public Variant::CustomData
{
public:
    enum { Type = 103 };
    TypedArrayCustom(ScriptEngine::TypedArrayType t, const DataBuffer& b)
        : CustomData(Type), type(t), buffer(b)
    {
    }

    v8::Handle<v8::Value> v8Value() const
    {
        EscapableContextHolder holder;

        v8::Local<v8::Value> array;
        V8ArrayBuffer buf(buffer);
        switch (type) {
        case ScriptEngine::Type_Uint8: array = v8::Uint8Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Int8: array = v8::Int8Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Uint8Clamped: array = v8::Uint8ClampedArray::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Uint16: array = v8::Uint16Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Int16: array = v8::Int16Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Uint32: array = v8::Uint32Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Int32: array = v8::Int32Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Float32: array = v8::Float32Array::New(buf.buffer, 0, buffer.size()); break;
        case ScriptEngine::Type_Float64: array = v8::Float64Array::New(buf.buffer, 0, buffer.size()); break;
        default: assert(0); break;
        }

        return holder.handleScope().Escape(array);
    }

    ScriptEngine::TypedArrayType type;
    DataBuffer buffer;
};

static inline v8::Local<v8::Value> convertFromVariant_helper(const Variant &variant)
{
    v8::Local<v8::Value> result;
    switch(variant.type()) {
    case Variant::Type_Custom: {
        assert(variant.custom());
        const shared_ptr<const Variant::CustomData>& custom = variant.custom();
        switch (custom->type) {
        case ValueCustom::Type:
            result = v8::Local<v8::Value>::New(v8::Isolate::GetCurrent(),
                                               static_pointer_cast<const ValueCustom>(custom)->value);
            break;
        case StringCustom::Type:
            result = static_pointer_cast<const StringCustom>(custom)->v8Value();
            break;
        case TypedArrayCustom::Type:
            result = static_pointer_cast<const TypedArrayCustom>(custom)->v8Value();
            break;
        }
        break; }
    case Variant::Type_String:
        result = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), variant.string().c_str());
        break;
    case Variant::Type_DataBuffer: {
        const DataBuffer& databuf = variant.dataBuffer();
        V8ArrayBuffer buf(databuf);
        result = buf.buffer;
        break; }
    case Variant::Type_Array: {
        v8::Handle<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), variant.size());
        const int sz = variant.size();
        for (int i = 0; i < sz; ++i)
            array->Set(i, convertFromVariant_helper(variant[i]));
        result = array;
        break; }
    case Variant::Type_StringMap: {
        v8::Handle<v8::Object> object = v8::Object::New(v8::Isolate::GetCurrent());
        const Variant::StringMapIterator end = variant.stringMapEnd();
        for (Variant::StringMapIterator it = variant.stringMapBegin(); it != end; ++it)
            object->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), it->first.c_str()), convertFromVariant_helper(it->second));
        result = object;
        break; }
    case Variant::Type_Integer: {
        const llong number = variant.integer();
        if(number > INT_MAX || number < INT_MIN)
            result = v8::Number::New(v8::Isolate::GetCurrent(), number);
        else
            result = v8::Integer::New(v8::Isolate::GetCurrent(), number);
        break; }
    case Variant::Type_Double:
        result = v8::Number::New(v8::Isolate::GetCurrent(), variant.dbl());
        break;
    case Variant::Type_Boolean:
        result = v8::Boolean::New(v8::Isolate::GetCurrent(), variant.boolean());
        break;
    case Variant::Type_IntegerMap:
        assert(false);
        break;
    case Variant::Type_Null:
        result = v8::Undefined(v8::Isolate::GetCurrent());
        break;
    }
    return result;
}

static v8::Local<v8::Value> convertFromVariant(const Variant& variant)
{
    EscapableContextHolder holder;
    return holder.handleScope().Escape(convertFromVariant_helper(variant));
}

namespace netflix {
namespace gibbon {
class GibbonArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
    virtual void* Allocate(size_t length) { return calloc(length, 1); }
    virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
    virtual void Free(void* data, size_t /*length*/) { free(data); }
};

class GibbonEventConnection : public netflix::EventConnection, public enable_shared_from_this<GibbonEventConnection>
{
public:
    GibbonEventConnection(const v8::Handle<v8::Function>& callback)
        : mCallback(v8::Isolate::GetCurrent(), callback)
    {
    }

    ~GibbonEventConnection()
    {
        Finish();
    }

    virtual void Finish() {
        mCallback.Reset();
    }

protected:
    friend class GibbonBridgeObject;
    friend class GibbonJSEvent;
    class GibbonJSEvent : public GibbonApplication::Event
    {
    public:
        typedef shared_ptr<GibbonJSEvent> SharedPtr;
        GibbonJSEvent(const shared_ptr<GibbonEventConnection>& c, const Variant& e) : mConnection(c), mEvent(e) { }
        std::string describe() const { return std::string("GibbonJSEvent"); }
        void eventFired() {
            shared_ptr<GibbonEventConnection> conn = mConnection.lock();
            if (!conn)
                return;
            conn->sendEvent(mEvent);
        }

        weak_ptr<GibbonEventConnection> mConnection;
        Variant mEvent;
    };

    virtual bool writeResponse(const Response &response)
    {
        const Variant variant = response.toVariant();

        if (Application::isAppThread() && response.flags() & Response::Flag_Synchronous) {
            sendEvent(variant);
            return true;
        }

        shared_ptr<GibbonJSEvent> event(new GibbonJSEvent(shared_from_this(), variant));
        GibbonApplication::instance()->postEvent(event);
        return true;
    }

    void sendEvent(const Variant& event)
    {
        if(!mCallback.IsEmpty()) {
            ContextHolder holder;
#ifdef GIBBON_PROFILE_V8
            char buffer[256];
            sprintf(buffer, "%s/%d", mSend[0].contains("name") ? mSend[0]["name"].string().c_str() : "NA", mSend.size());
            ScriptEngineV8Profiler profile(buffer);
#endif
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), mCallback);
            const int argc = 1;
            v8::Handle<v8::Value> argv = convertFromVariant(event);
            callback->Call(holder.context()->Global(), argc, &argv);
        }
    }

private:
    v8::Persistent<v8::Function> mCallback;
};
}
}

void ScriptEngine::init()
{
    mArrayBufferAllocator = new GibbonArrayBufferAllocator;
    static bool first = true;
    if (first) {
        v8::V8::SetArrayBufferAllocator(mArrayBufferAllocator);
        first = false;
    }
}

void ScriptEngine::shutdown()
{
}

void ScriptEngine::evaluate(const DataBuffer &src, const AseUrl &url,
                            Variant *exception, Variant *result, int /*recurseDepth*/)
{
    ContextHolder holder;
    v8::TryCatch tryCatch;
#ifdef GIBBON_PROFILE_V8
    ScriptEngineV8Profiler profile("Evaluate: " + filename);
#endif
    const std::string& filename = url.str();
    v8::Handle<v8::Script> compiled_script = v8::Script::Compile(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), src.c_str()),
                                                                 v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), filename.c_str()));

    if (compiled_script.IsEmpty()) {
        v8::String::Utf8Value error(tryCatch.Exception());
        Log::error(TRACE_UI_SCRIPT, "V8 COMPILE error: %s:%d [%s]", filename.c_str(), tryCatch.Message()->GetLineNumber(),
               *error);
        if (exception)
            *exception = *error;
        return;
    }

    v8::Handle<v8::Value> ret = compiled_script->Run();
    if (ret.IsEmpty()) {
        v8::String::Utf8Value error(tryCatch.Exception());
        Log::error(TRACE_UI_SCRIPT, "V8 RUN error: %s:%d [%s]", filename.c_str(), tryCatch.Message()->GetLineNumber(),
               *error);
        if (exception)
            *exception = *error;
        return;
    }
    if (result)
        *result = convertToVariant(ret);
}

void ScriptEngine::startEngine()
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope hs(isolate);
    v8::Handle<v8::ObjectTemplate> globalObject = v8::ObjectTemplate::New(isolate);
    bridgeInit(globalObject);
    v8::Handle<v8::Context> context = v8::Context::New(isolate, NULL, globalObject);
    mContext.Reset(isolate, context);
    mIsolate = isolate;
    mBridge = GibbonApplication::instance()->getBridge();
}

void ScriptEngine::stopEngine()
{
    bridgeCleanup();
    mContext.Reset();
}

size_t ScriptEngine::garbageCollect()
{
    while (!v8::V8::IdleNotification()) {}
    return 0;
}

void ScriptEngine::v8BridgeInit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    assert(!GibbonApplication::instance()->getScriptEngine()->mEventConnection);
    v8::HandleScope hs(v8::Isolate::GetCurrent());
    v8::Handle<v8::Value> callback = args[0];
    if(!GibbonApplication::instance()->getScriptEngine()->mEventConnection && callback->IsFunction()) {
        shared_ptr<GibbonEventConnection>& connection = GibbonApplication::instance()->getScriptEngine()->getEventConnection();
        connection.reset(new GibbonEventConnection(v8::Handle<v8::Function>::Cast(callback)));
        nrdApp()->addEventConnection(connection);
        args.GetReturnValue().Set(v8::True(v8::Isolate::GetCurrent()));
        return;
    }
    args.GetReturnValue().Set(v8::False(v8::Isolate::GetCurrent()));
}

static void v8BridgeEnabled(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    args.GetReturnValue().Set(v8::True(v8::Isolate::GetCurrent()));
}

static void v8BridgeConstruct(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope hs(v8::Isolate::GetCurrent());
    Variant variantMap;
    if (args.Length() < 3) {
        Log::error(TRACE_UI_SCRIPT, "v8BridgeInvoke called with %d arguments", args.Length());
        return;
    }
    variantMap["object"] = convertToString(args[0]);
    variantMap["construct"] = convertToString(args[1]);
    {
        if(args[2]->IsObject()) {
            Variant variant = convertToVariant(args[2]);
            for(Variant::StringMapIterator it = variant.stringMapBegin(); it != variant.stringMapEnd(); ++it)
                variantMap[it->first] = it->second;
        } else if(args[2]->IsNull()) {
        } else {
            assert(false);
        }
    }

    //printf("constructed request '%s'\n", variantMap.toJSON(true).c_str());
    const Request nbpRequest(variantMap, Request::FromTrustedSource);
    args.GetReturnValue().Set(convertFromVariant(nrdApp()->send(nbpRequest)));
}

static void v8BridgeSetProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope hs(v8::Isolate::GetCurrent());
    Variant variantMap;
    variantMap["object"] = convertToString(args[0]);
    variantMap["property"] = convertToString(args[1]);
    variantMap["value"] = convertToVariant(args[2]);

    const Request nbpRequest(variantMap, Request::FromTrustedSource);
    nrdApp()->post(nbpRequest);
}

static void v8BridgeInvoke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope hs(v8::Isolate::GetCurrent());
    Variant variantMap;
    if (args.Length() < 3) {
        Log::error(TRACE_UI_SCRIPT, "v8BridgeInvoke called with %d arguments", args.Length());
        return;
    }
    variantMap["object"] = convertToString(args[0]);
    variantMap["method"] = convertToString(args[1]);
    {
        if(args[2]->IsObject()) {
            Variant variant = convertToVariant(args[2]);
            for(Variant::StringMapIterator it = variant.stringMapBegin(); it != variant.stringMapEnd(); ++it)
                variantMap[it->first] = it->second;
        } else if(args[2]->IsNull() || args[2]->IsUndefined()) {
        } else {
            assert(false);
        }
    }

    //printf("invoked %s\n", variantMap.toJSON(true).c_str());

    //printf("invoked request '%s'\n", variantMap.toJSON(true).c_str());
    const Request nbpRequest(variantMap, Request::FromTrustedSource);
    args.GetReturnValue().Set(convertFromVariant(nrdApp()->post(nbpRequest)));
}

static void v8Eval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    const std::string script = convertToString(args[0]);
    const std::string file = convertToString(args[1]);
    Variant result;
    GibbonApplication::instance()->getScriptEngine()->evaluate(script, file, 0, &result);
    args.GetReturnValue().Set(convertFromVariant(result));
}

static void v8Log(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    const std::string msg = convertToString(args[0]);
    if(!startsWith(msg, "NETFLIX:")) {
#if 1
        NTRACE(TRACE_UI_SCRIPT, "%s", msg.c_str());
#else
        printf("Foo: %s\n", msg.c_str());
#endif
    }
}

static void v8Console(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    const std::string msg = convertToString(args[0]);
    fprintf(stderr, "Console[%lld]: %s\n", NrdApplication::timeSinceLocationChanged().ms(), msg.c_str());
}

static void v8Mono(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> mono = v8::Number::New(v8::Isolate::GetCurrent(), Time::monoMS());
    args.GetReturnValue().Set(hs.Escape(mono));
}

static void v8Now(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> now = v8::Number::New(v8::Isolate::GetCurrent(), Time::serverTime().ms());
    args.GetReturnValue().Set(hs.Escape(now));
}

static void v8Location(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
    const std::string& loc = GibbonApplication::instance()->getLocation().str();
    v8::Local<v8::Value> v8loc = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), loc.c_str());
    args.GetReturnValue().Set(hs.Escape(v8loc));
}

static void v8btoa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());

    if (args.Length() >= 1) {
        const bool urlSafe = args.Length() >= 2 ? args[1]->ToBoolean()->Value() : false;
        const bool returnTypedArray = args.Length() >= 3 ? args[2]->ToBoolean()->Value() : false;
        V8DecodedData data;
        data.decode(args[0]);
        const DataBuffer& buffer = data.dataBuffer;
        if (!buffer.isEmpty()) {
            const DataBuffer encoded = buffer.encode(urlSafe ? DataBuffer::Encoding_Base64_UrlSafe : DataBuffer::Encoding_Base64);
            if (returnTypedArray) {
                V8ArrayBuffer buf(encoded);
                v8::Local<v8::Uint8Array> array = v8::Uint8Array::New(buf.buffer, 0, encoded.size());
                args.GetReturnValue().Set(hs.Escape(array));
            } else {
                v8::Local<v8::String> str = v8::String::NewFromOneByte(v8::Isolate::GetCurrent(),
                                                                       encoded.data(),
                                                                       v8::String::kNormalString,
                                                                       encoded.size());
                args.GetReturnValue().Set(hs.Escape(str));
            }
        }
    }
}

static void v8atob(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
    if (args.Length() >= 1) {
        const bool returnTypedArray = args.Length() >= 2 ? args[1]->ToBoolean()->Value() : false;
        V8DecodedData data;
        data.decode(args[0]);
        const DataBuffer& buffer = data.dataBuffer;
        if (!buffer.isEmpty()) {
            const char *ch = buffer.data<const char>();
            int size = Base64::decode_reserve(buffer.size());
            const char *end = ch + (buffer.size() - 1);
            while (end >= ch && *end == '=') {
                --end;
                --size;
            }
            ++end;
            if (returnTypedArray) {
                const DataBuffer decoded = DataBuffer::fromBase64(reinterpret_cast<const unsigned char*>(ch), end - ch);
                V8ArrayBuffer buf(decoded);
                v8::Local<v8::Uint8Array> array = v8::Uint8Array::New(buf.buffer, 0, decoded.size());
                args.GetReturnValue().Set(hs.Escape(array));
            } else {
                std::string result;
                result.reserve(size);
                Base64::decode(ch, end, std::back_inserter(result));
                v8::Local<v8::String> ret = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                                                     result.c_str(),
                                                                     v8::String::kNormalString,
                                                                     result.size());
                args.GetReturnValue().Set(hs.Escape(ret));
            }
        }
    }
}

static void v8atoutf8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
    if (args.Length() >= 1) {
        V8DecodedData data;
        if (data.decode(args[0]) && data.type == V8DecodedData::Type_String) {
            if (data.dataBuffer.isEmpty()) {
                args.GetReturnValue().Set(hs.Escape(v8::Local<v8::Uint8Array>()));
                return;
            }
            V8ArrayBuffer buf(data.dataBuffer);
            v8::Local<v8::Uint8Array> array = v8::Uint8Array::New(buf.buffer, 0, data.dataBuffer.size());
            args.GetReturnValue().Set(hs.Escape(array));
        }
    }
}

static void v8utf8toa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::EscapableHandleScope hs(v8::Isolate::GetCurrent());
    if (args.Length() >= 1) {
        V8DecodedData data;
        if (data.decode(args[0])) {
            if (data.dataBuffer.isEmpty()) {
                args.GetReturnValue().Set(hs.Escape(v8::Local<v8::String>()));
                return;
            }
            args.GetReturnValue().Set(hs.Escape(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
                                                                        data.dataBuffer.c_str(),
                                                                        v8::String::kNormalString,
                                                                        data.dataBuffer.size())));
        }
    }
}

static void v8Random(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope hs(v8::Isolate::GetCurrent());
    if (args.Length() >= 1) {
        v8::Handle<v8::Value> value = args[0];
        void* buf = 0;
        size_t off = 0, sz = 0;
        if (value->IsArrayBufferView()) {
            v8::Handle<v8::ArrayBufferView> view = v8::Handle<v8::ArrayBufferView>::Cast(value);
            buf = view->Buffer()->BaseAddress();
            off = view->ByteOffset();
            sz = view->ByteLength();
        } else if (value->IsArrayBuffer()) {
            v8::Handle<v8::ArrayBuffer> buffer = v8::Handle<v8::ArrayBuffer>::Cast(value);
            buf = buffer->BaseAddress();
            sz = buffer->ByteLength();
        }
        if (buf && sz) {
            Random::next(reinterpret_cast<unsigned char*>(buf) + off, sz);
            args.GetReturnValue().Set(v8::True(v8::Isolate::GetCurrent()));
            return;
        }
    }
    args.GetReturnValue().Set(v8::False(v8::Isolate::GetCurrent()));
}

static void v8ParseJSON(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    EscapableContextHolder holder;
    const int argCount = args.Length();

    if (argCount != 1) {
        Log::error(TRACE_UI_SCRIPT, "parseJSON takes exactly one argument. Not %zu", argCount);
        return;
    }

    V8DecodedData data;
    if (!data.decode(args[0])) {
        v8::Isolate* isolate = args.GetIsolate();
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments to parseJSON",
                                                        v8::String::kNormalString));
        return;
    }

    int pos = 0, dataSize = data.dataBuffer.size();
    std::vector<v8::Handle<v8::Value> > ret;
    const char *ch = data.dataBuffer.4data<const char>();
    while (pos < dataSize) {
        JSONParserV8 parser;
        const v8::Handle<v8::Value>& val = parser.parse(ch + pos, dataSize - pos);
        if (val->IsUndefined()) {
            return;
        }

        ret.push_back(val);
        pos = parser.current() - ch;
    }
    const size_t retSize = ret.size();
    v8::Local<v8::Array> array = v8::Array::New(args.GetIsolate(), retSize);
    for (size_t i = 0; i < retSize; ++i) {
        array->Set(i, ret.at(i));
    }
    args.GetReturnValue().Set(holder.handleScope().Escape(array));
}

static void v8ParseXML(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    EscapableContextHolder holder;
    const int argCount = args.Length();

    if (argCount != 1) {
        Log::error(TRACE_UI_SCRIPT, "v8ParseXML takes exactly 1 argument. Not %zu", argCount);
        return;
    }

    V8DecodedData data;
    if (!data.decode(args[0])) {
        v8::Isolate* isolate = args.GetIsolate();
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments parseXML",
                                                        v8::String::kNormalString));
        return;
    }

    bool ok = false;
    const Variant var = Variant::fromXML(data.dataBuffer, &ok);
    if (!ok)
        return;
    args.GetReturnValue().Set(holder.handleScope().Escape(convertFromVariant(var)));
}

static void v8Compress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    const int argCount = args.Length();
    if (argCount < 2 || !args[1]->IsString())
        return;

    EscapableContextHolder holder;

    v8::Handle<v8::String> type = v8::Handle<v8::String>::Cast(args[1]);
    v8::String::Utf8Value typeValue(type);
    if (strcmp(*typeValue, "gzip")) {
        Log::error(TRACE_UI_SCRIPT, "Invalid compression type: '%s'", *typeValue);
        return;
    }

    V8DecodedData data;
    if (!data.decode(args[0])) {
        if (args[0]->IsObject()) {
            Variant var = convertToVariant(args[0]);
            data.type = V8DecodedData::Type_String;
            data.dataBuffer = var.toJSON();
        }
        if (data.dataBuffer.isEmpty()) {
            v8::Isolate* isolate = args.GetIsolate();
            isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments compress",
                                                            v8::String::kNormalString));
            return;
        }
    }

    const bool base64 = (argCount < 3 || !args[2]->BooleanValue());

    if (data.dataBuffer.isEmpty())
        return;

    bool ok;
    const DataBuffer::CompressionMode mode = DataBuffer::Compression_GZip;
    DataBuffer compressed = data.dataBuffer.compress(mode, &ok);
    NTRACE(TRACE_UI_SCRIPT, "jscCompress: %d bytes", compressed.size());
    if (!ok)
        return;
    if (base64) {
        compressed = compressed.encode(DataBuffer::Encoding_Base64);
        v8::Local<v8::String> out = v8::String::NewFromOneByte(v8::Isolate::GetCurrent(),
                                                               compressed.data(),
                                                               v8::String::kNormalString,
                                                               compressed.size());
        args.GetReturnValue().Set(holder.handleScope().Escape(out));
        return;
    }

    V8ArrayBuffer buffer(compressed);
    v8::Local<v8::Uint8Array> out = v8::Uint8Array::New(buffer.buffer, 0, compressed.size());
    args.GetReturnValue().Set(holder.handleScope().Escape(out));
}

static void v8Uncompress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    const int argCount = args.Length();
    if (argCount < 2)
        return;

    EscapableContextHolder holder;

    v8::Handle<v8::String> type = v8::Handle<v8::String>::Cast(args[1]);
    v8::String::Utf8Value typeValue(type);
    if (strcmp(*typeValue, "gzip")) {
        Log::error(TRACE_UI_SCRIPT, "Invalid compression type: '%s'", *typeValue);
        return;
    }

    V8DecodedData data;
    data.decode(args[0]);

    if (data.dataBuffer.isEmpty())
        return;

    bool ok;
    const DataBuffer::CompressionMode mode = DataBuffer::Compression_GZip;
    DataBuffer raw = data.dataBuffer;
    if (data.type == V8DecodedData::Type_String)
        raw = raw.decode(DataBuffer::Encoding_Base64);
    const DataBuffer uncompressed = raw.uncompress(mode, &ok);
    NTRACE(TRACE_UI_SCRIPT, "jscUncompress: %zu -> %d bytes", raw.size(), uncompressed.size());
    if (!ok)
        return;

    if (argCount < 3 || !args[2]->BooleanValue()) {
        v8::Local<v8::String> out = v8::String::NewFromOneByte(v8::Isolate::GetCurrent(),
                                                               uncompressed.data(),
                                                               v8::String::kNormalString,
                                                               uncompressed.size());
        args.GetReturnValue().Set(holder.handleScope().Escape(out));
        return;
    }

    V8ArrayBuffer buffer(uncompressed);
    v8::Local<v8::Uint8Array> out = v8::Uint8Array::New(buffer.buffer, 0, uncompressed.size());
    args.GetReturnValue().Set(holder.handleScope().Escape(out));
}

class SyncObject
{
public:
    inline SyncObject(const shared_ptr<GibbonBridge> &bridge, GibbonBaseBridge::ClassType type, int widgetID)
        : mBridge(bridge), mType(type), mWidgetID(widgetID)
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
        default:
            assert(false);
        }
#endif
    }
    ~SyncObject() {
#ifdef NRDP_HAS_OBJECTCOUNT
        switch(mType) {
        case GibbonBaseBridge::TextType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectText, this);
            break;
        case GibbonBaseBridge::WidgetType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectWidget, this);
            break;
        case GibbonBaseBridge::ImageType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectImage, this);
            break;
        case GibbonBaseBridge::EffectType:
            NRDP_OBJECTCOUNT_DEREF(GibbonSyncObjectEffect, this);
            break;
        default:
            assert(false);
        }
#endif
    }

    static void finalizeSync(const v8::WeakCallbackData<v8::Object, SyncObject>& data)
    {
        SyncObject* sync = data.GetParameter();
        sync->finalize();
        delete sync;
    }
    inline void setWeak(v8::Handle<v8::Object> obj)
    {
        mPersistent.Reset(v8::Isolate::GetCurrent(), obj);
        mPersistent.SetWeak(this, finalizeSync);
        mPersistent.MarkIndependent();
    }
    inline void setProperty(int property, const Variant &value, bool append) {
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

    inline void init(int id, const Variant &params) {
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
    inline void init() {
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
    void finalize() {
        switch(mType) {
        case GibbonBaseBridge::TextType:
            sObjects.erase(mapID(mType, mWidgetID));
            mBridge->pendingSync_destroyText(mWidgetID);
            break;
        case GibbonBaseBridge::WidgetType:
            sObjects.erase(mapID(mType, mWidgetID));
            mBridge->pendingSync_destroyWidget(mWidgetID);
            break;
        case GibbonBaseBridge::ImageType:
            sObjects.erase(mapID(mType, mID));
            mBridge->pendingSync_destroyImage(mWidgetID, mID);
            break;
        case GibbonBaseBridge::EffectType:
            sObjects.erase(mapID(mType, mID));
            mBridge->pendingSync_destroyEffect(mWidgetID, mID);
            break;
        default:
            assert(false);
        }
        assert(mPersistent.IsNearDeath());
        mPersistent.Reset();
    }

    class Custom : public ValueCustom
    {
    public:
        inline Custom(v8::Handle<v8::Object> value, const shared_ptr<GibbonBaseBridge> &bridge) : ValueCustom(value), mBridge(bridge) { }
        virtual Variant toVariant() const { return mBridge->path(); }
        shared_ptr<GibbonBaseBridge> mBridge;
    };

    inline static bool isEmpty() { return sObjects.empty(); }

    inline static Variant find(const shared_ptr<GibbonBaseBridge> &bridge) {
        if (!Application::isAppThread())
            return Variant();
        if(const int id = bridge->classID()) {
            std::map<ullong, SyncObject*>::const_iterator it = sObjects.find(mapID(bridge->classType(), id));
            if(it != sObjects.end()) {
                v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), it->second->mPersistent);
                return Variant(shared_ptr<Variant::CustomData>(new Custom(obj, bridge)));
            }
        }
        return Variant();
    }

protected:
    shared_ptr<GibbonBridge> mBridge;
    GibbonBaseBridge::ClassType mType;
    int mWidgetID, mID;
    v8::Persistent<v8::Object> mPersistent;
    static std::map<ullong, SyncObject*> sObjects;
};
std::map<ullong, SyncObject*> SyncObject::sObjects;

Variant ScriptEngine::objectId(const shared_ptr<GibbonBaseBridge> &bridge)
{
    return SyncObject::find(bridge);
}

static void v8GibbonCreateSyncObject(const v8::FunctionCallbackInfo<v8::Value>& arguments)
{
    ContextHolder holder;
    assert(arguments.Length() >= 2);
    shared_ptr<ScriptEngine> scriptEngine = GibbonApplication::instance()->getScriptEngine();
    const int widgetID = v8::Handle<v8::Integer>::Cast(arguments[0])->Value();
    GibbonBaseBridge::ClassType type = (GibbonBaseBridge::ClassType)v8::Handle<v8::Integer>::Cast(arguments[1])->Value();
    SyncObject *object = new SyncObject(scriptEngine->getBridge(), type, widgetID);
    switch(type) {
    case GibbonBaseBridge::TextType:
    case GibbonBaseBridge::WidgetType:
        object->init();
        break;
    case GibbonBaseBridge::ImageType:
    case GibbonBaseBridge::EffectType:
        assert(arguments.Length() >= 4);
        object->init(v8::Handle<v8::Integer>::Cast(arguments[2])->Value(), convertToVariant(arguments[3]));
        break;
    default:
        assert(false);
    }

    v8::Handle<v8::ObjectTemplate> syncTempl = v8::ObjectTemplate::New(v8::Isolate::GetCurrent());
    syncTempl->SetInternalFieldCount(1);
    v8::Handle<v8::Object> obj = syncTempl->NewInstance();
    obj->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), object));
    object->setWeak(obj);
    arguments.GetReturnValue().Set(obj);
}

static void v8GibbonSetSyncProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ContextHolder holder;
    assert(args.Length() >= 3);
    if (args[0]->IsObject()) {
        v8::Handle<v8::Object> obj = v8::Handle<v8::Object>::Cast(args[0]);
        v8::Handle<v8::External> ext = v8::Handle<v8::External>::Cast(obj->GetInternalField(0));
        SyncObject* object = static_cast<SyncObject*>(ext->Value());
        bool append = false;
        if(args.Length() == 4)
            append = args[3]->BooleanValue();
        object->setProperty(v8::Handle<v8::Integer>::Cast(args[1])->Value(), convertToVariant(args[2]), append);
    } else {
        assert(false);
    }
}

ScriptEngine::Stats ScriptEngine::stats() const
{
    return Stats();
}

void ScriptEngine::bridgeCleanup()
{
    if (mEventConnection) {
        nrdApp()->removeEventConnection(mEventConnection);
        mEventConnection->Finish();
        mEventConnection.reset();
    }
}

void ScriptEngine::bridgeInit(v8::Handle<v8::ObjectTemplate> o)
{
    v8::Isolate* iso = v8::Isolate::GetCurrent();
    v8::Handle<v8::ObjectTemplate> nrdp_platform = v8::ObjectTemplate::New(iso);
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "v8BridgeInit"), v8::FunctionTemplate::New(iso, v8BridgeInit));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "v8BridgeEnabled"), v8::FunctionTemplate::New(iso, v8BridgeEnabled));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "v8BridgeConstruct"), v8::FunctionTemplate::New(iso, v8BridgeConstruct));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "v8BridgeSetProperty"), v8::FunctionTemplate::New(iso, v8BridgeSetProperty));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "v8BridgeInvoke"), v8::FunctionTemplate::New(iso, v8BridgeInvoke));

    /*
      { "parseXML", jscParseXML, kJSPropertyAttributeNone },
      { "gctag", jscMakeGCTag, kJSPropertyAttributeNone },*/
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "gibbonCreateSyncObject"), v8::FunctionTemplate::New(iso, v8GibbonCreateSyncObject));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "gibbonSetSyncProperty"), v8::FunctionTemplate::New(iso, v8GibbonSetSyncProperty));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "console"), v8::FunctionTemplate::New(iso, v8Console));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "eval"), v8::FunctionTemplate::New(iso, v8Eval));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "log"), v8::FunctionTemplate::New(iso, v8Log));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "location"), v8::FunctionTemplate::New(iso, v8Location));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "mono"), v8::FunctionTemplate::New(iso, v8Mono));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "now"), v8::FunctionTemplate::New(iso, v8Now));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "btoa"), v8::FunctionTemplate::New(iso, v8btoa));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "atob"), v8::FunctionTemplate::New(iso, v8atob));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "atoutf8"), v8::FunctionTemplate::New(iso, v8atoutf8));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "utf8toa"), v8::FunctionTemplate::New(iso, v8utf8toa));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "random"), v8::FunctionTemplate::New(iso, v8Random));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "parseJSON"), v8::FunctionTemplate::New(iso, v8ParseJSON));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "parseXML"), v8::FunctionTemplate::New(iso, v8ParseXML));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "compress"), v8::FunctionTemplate::New(iso, v8Compress));
    nrdp_platform->Set(v8::String::NewFromUtf8(iso, "uncompress"), v8::FunctionTemplate::New(iso, v8Uncompress));
    o->Set(v8::String::NewFromUtf8(iso, "nrdp_platform"), nrdp_platform);
}

#warning all of the functions below need to be implemented

Variant ScriptEngine::parseJSON(const char *data, int length, bool *ok, int *used) const
{
    if (ok)
        *ok = false;
    if (used)
        *used = 0;
    assert(Application::isAppThread());
    if (!length || !data)
        return Variant();
    ContextHolder holder;

    JSONParserV8 parser;
    v8::Handle<v8::Value> val = parser.parse(data, length);
    if (val->IsUndefined())
        return Variant();

    shared_ptr<Variant::CustomData> custom(new ValueCustom(val));
    if (used)
        *used = parser.current() - data;
    if (ok)
        *ok = true;
    return custom;
}

Variant ScriptEngine::parseXML(const char *data, int length, bool *ok, int *used) const
{
    return Variant::fromXML(data, length, ok, used);
}

Variant ScriptEngine::createTypedArray(TypedArrayType type, const DataBuffer &buffer)
{
    shared_ptr<Variant::CustomData> custom(new TypedArrayCustom(type, buffer));
    return custom;
}

Variant ScriptEngine::createString(const DataBuffer &buffer)
{
#warning need to do utf8 or binary based on the content
    shared_ptr<Variant::CustomData> custom(new StringCustom(buffer));
    return custom;
}

int ScriptEngine::scriptAllocated() const
{
    return 0;
}

int ScriptEngine::garbageCollectTimeout() const
{
    return 30;
}

void ScriptEngine::setGarbageCollectTimeout(int timeout)
{
}

void ScriptEngine::queryMalloc(ullong* system, ullong* metadata, ullong* freebytes,
                               size_t* returned, size_t* commited, size_t* decommitted) const
{
    if (system)
        *system = 0;
    if (metadata)
        *metadata = 0;
    if (freebytes)
        *freebytes = 0;
    if (returned)
        *returned = 0;
    if (commited)
        *commited = 0;
    if (decommitted)
        *decommitted = 0;
}

void ScriptEngine::dumpCounts(std::string& result)
{
    result = std::string();
}

void ScriptEngine::dump(std::string& heap)
{
    heap = std::string();
}

void ScriptEngine::heap(FILE*) const
{
}

int ScriptEngine::ignoreExceptions(int ignore)
{
    return 0;
}

void ScriptEngine::pauseOnNextStatement()
{
}

bool ScriptEngine::isInBreakpoint() const
{
    return false;
}

void ScriptEngine::startCPUProfiler(int profileId)
{
}

std::string ScriptEngine::stopCPUProfiler(int profileId)
{
    return std::string();
}

unsigned int ScriptEngine::setBreakpoint(const std::string &file, int line)
{
    return 0;
}

void ScriptEngine::setCondition(unsigned int breakPointId, const std::string &condition)
{
}

void ScriptEngine::removeBreakpoint(unsigned int id)
{
}

void ScriptEngine::continueFromBreakpoint()
{
}

void ScriptEngine::listBreakpoints(std::vector<Breakpoint>& breakpoints)
{
    breakpoints.clear();
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

bool ScriptEngine::listIdentifiers(Variant &identifiers, int mode, int frame, bool resolve)
{
    identifiers.clear();
    return true;
}

Variant ScriptEngine::evaluateIdentifier(const std::string& identifier, int frame)
{
    return Variant();
}

void ScriptEngine::stackTrace(std::vector<StackFrame>& frames)
{
    frames.clear();
}

void ScriptEngine::setCurrentFrame(unsigned int frame)
{
}

void ScriptEngine::attachDebugger()
{
}

void ScriptEngine::detachDebugger()
{
}

unsigned int ScriptEngine::currentFrame() const
{
    return 0;
}

std::vector<std::string> ScriptEngine::completions(const std::string &commandLine)
{
    return std::vector<std::string>();
}
