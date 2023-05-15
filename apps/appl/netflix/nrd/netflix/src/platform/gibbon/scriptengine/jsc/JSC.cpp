/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include <GibbonApplication.h>
#include GIBBON_JSC_API(JSContextRef.h)
#include GIBBON_JSC_API(JSValueRef.h)
#include <nrd/Response.h>
#include <nrd/AppLog.h>
#include "ArrayBufferJSC.h"

using namespace netflix;
using namespace netflix::gibbon;
using namespace netflix::gibbon::JSC;

NRDP_DEFINE_OBJECTCOUNT(gctag);

extern "C" {
typedef void* Netflix_JSPropertyNameArrayRef;
bool Netflix_JSValueIsArray(JSContextRef ctx, JSValueRef value);
JSStringRef Netflix_JSStringCreate(const char* string, int length);
    void Netflix_JSFunctionCode(JSContextRef ctx, JSObjectRef object, JSStringRef* str);
const JSChar* Netflix_JSValueGetString(JSContextRef ctx, JSValueRef value, size_t* size);
size_t Netflix_JSValueGetUTF8CString(const JSChar* chars, size_t size, char* buffer, size_t bufferSize);
Netflix_JSPropertyNameArrayRef Netflix_JSObjectPropertyNames(JSContextRef ctx, JSObjectRef object);
void Netflix_JSPropertyNameArrayRelease(JSContextRef ctx, Netflix_JSPropertyNameArrayRef array);
size_t Netflix_JSPropertyNameArrayGetCount(Netflix_JSPropertyNameArrayRef array);
const JSChar* Netflix_JSPropertyNameArrayGetNameAtIndex(JSContextRef ctx, Netflix_JSPropertyNameArrayRef array,
                                                        size_t index, size_t* size);
JSValueRef Netflix_JSObjectGetProperty(JSContextRef ctx, JSObjectRef object, Netflix_JSPropertyNameArrayRef array,
                                       size_t index, JSValueRef* exception);
size_t Netflix_GarbageCollect(JSContextRef ctx);
};

// ==================== ValueWrapper ====================

ProtectedValue::ProtectedValue()
    : data(0)
{
}

ProtectedValue::ProtectedValue(JSValueRef value, bool protect)
    : data(new Data)
{
    data->ref = 1;
    data->value = value;
    assert(ScriptEngine::currentContext());
    if (value && protect)
        JSValueProtect(ScriptEngine::currentContext(), value);
}

ProtectedValue::ProtectedValue(const ProtectedValue &other)
    : data(other.data)
{
    if (data)
        ++data->ref;
}

ProtectedValue &ProtectedValue::operator=(const ProtectedValue &other)
{
    clear();
    data = other.data;
    if (data)
        ++data->ref;
    return *this;
}

ProtectedValue::~ProtectedValue()
{
    clear();
}

void ProtectedValue::clear()
{
    if (data) {
        if (!--data->ref) {
            if (data->value) {
                JSContextRef ctx = ScriptEngine::currentContext();
                if (ctx)
                    JSValueUnprotect(ctx, data->value);
            }
            delete data;
        }
        data = 0;
    }
}

bool ProtectedValue::isNull() const
{
    return !data;
}

JSValueRef ProtectedValue::value() const
{
    assert(data);
    return data->value;
}

JSObjectRef ProtectedValue::object() const
{
    assert(data);
    return const_cast<JSObjectRef>(data->value);
}

// ==================== ScopedString ====================

ScopedString::ScopedString(const char *str, int len) : mString(Netflix_JSStringCreate(str, len == -1 ? strlen(str) : len))
{
}

// ==================== ValueCustom ====================

ValueCustom::ValueCustom(JSValueRef val)
    : Variant::CustomData(Type), value(val)
{
    assert(val);
}

ValueCustom::ValueCustom(CreationMode, JSValueRef val)
    : Variant::CustomData(Type), value(val)
{
    assert(value);
    assert(Application::isAppThread());
    assert(ScriptEngine::currentContext());
    JSValueProtect(ScriptEngine::currentContext(), value);
}

ValueCustom::~ValueCustom()
{
    if (value) {
        assert(Application::isAppThread());
        if (JSContextRef ctx = ScriptEngine::currentContext()) {
            JSValueUnprotect(ctx, value);
        } else {
            Log::error(TRACE_GIBBON_SCRIPT, "Destroying ValueCustom without script engine");
        }
    }
}

Variant ValueCustom::toVariant() const
{
    GibbonBridge::PendingSyncBlock scope;
    assert(Application::isAppThread());
    if (JSContextRef ctx = ScriptEngine::currentContext()) {
        return JSValueToVariant(ctx, value);
    }
    return Variant(Variant::Type_StringMap);
}

// ==================== TypedArrayCustom ====================

TypedArrayCustom::TypedArrayCustom(ScriptEngine::TypedArrayType t, const DataBuffer &buffer)
    : CustomData(Type), arrayType(t), dataBuffer(buffer)
{
}

Variant TypedArrayCustom::toVariant() const
{
    return dataBuffer;
}

// ==================== StringCustom ====================

StringCustom::StringCustom(const DataBuffer &buffer)
    : CustomData(Type), dataBuffer(buffer)
{
}

Variant StringCustom::toVariant() const
{
    return dataBuffer;
}

// ==================== FinalizeObject ====================

FinalizeObject *FinalizeObject::getFinalize(JSObjectRef object)
{
    PrivateBase *base = static_cast<PrivateBase*>(JSObjectGetPrivate(object));
    assert(base->type == PrivateBase::Type_Finalize);
    return static_cast<FinalizeObject*>(base);
}

void FinalizeObject::finalize(JSObjectRef object)
{
    if (FinalizeObject *final = getFinalize(object)) {
        final->finalize();
        delete final;
    }
}

// ==================== GCTagPrivate ====================

GCTagPrivate::GCTagPrivate(const std::string &s) : name(s), time(Time::monoMS())
{
    NRDP_OBJECTCOUNT_REF(gctag, this);
    static uint64_t next = 1;
    number = next++;
}

GCTagPrivate::~GCTagPrivate()
{
    NRDP_OBJECTCOUNT_DEREF(gctag, this);
}

void GCTagPrivate::finalize()
{
    Log::warn(TRACE_GIBBON_SCRIPT)
        << "FINALIZING GC TAG " << number
        << " CREATED " << (Time::monoMS() - time) / 1000.0 << " s AGO"
        << " NAMED '" << name << "'";
}

// ==================== Global functions ====================

namespace netflix {
namespace gibbon {
namespace JSC {

Variant JSValueToVariant_internal(JSContextRef ctx, JSValueRef value, JSValueRef* exception, std::set<JSObjectRef> &seen, int recurseDepth)
{
    assert(ctx);
    if (value) {
        switch (JSValueGetType(ctx, value)) {
        case kJSTypeUndefined:
        case kJSTypeNull:
            return Variant();
        case kJSTypeBoolean:
            return JSValueToBoolean(ctx, value);
        case kJSTypeNumber:
            return JSValueToNumber(ctx, value, exception);
        case kJSTypeString:
            return JSValueToString(value);
        case kJSTypeObject: {
            JSObjectRef object = JSValueToObject(ctx, value, 0);
            const std::pair<std::set<JSObjectRef>::iterator, bool> insert = seen.insert(object);
            if (!insert.second)
                return "(Recursive Object reference)";
            if (JSObjectIsFunction(ctx, object)) {
                ScopedString str;
                Netflix_JSFunctionCode(ctx, object, &str);
                if (str)
                    return Variant(toStdString(str));
                return Variant("(function)");
            }
            const PrivateBase *priv = reinterpret_cast<const PrivateBase*>(JSObjectGetPrivate(object));
            if (priv) {
                switch (priv->type) {
                case PrivateBase::Type_ArrayBuffer: {
                    const ArrayBufferJSC *buf = reinterpret_cast<const ArrayBufferJSC*>(priv);
                    return buf->mDataBuffer; }
                case PrivateBase::Type_TypedArray:
                case PrivateBase::Type_DataView:
                    return reinterpret_cast<const ArrayBufferViewJSC*>(priv)->dataBuffer();
                default:
                    break;
                }
            }

            Variant ret;
            const bool array = Netflix_JSValueIsArray(ctx, value);
            Netflix_JSPropertyNameArrayRef properties = Netflix_JSObjectPropertyNames(ctx, object);
            const size_t count = Netflix_JSPropertyNameArrayGetCount(properties);
            bool recurse = true;
            if (!recurseDepth) {
                if (array) {
                    for (int i=count - 1; i>=0; --i) {
                        if (JSValueGetType(ctx, JSObjectGetPropertyAtIndex(ctx, object, i, 0)) == kJSTypeObject) {
                            recurse = false;
                            break;
                        }
                    }
                } else {
                    for (size_t i=0; i<count; ++i) {
                        if (JSValueGetType(ctx, Netflix_JSObjectGetProperty(ctx, object, properties, i, 0)) == kJSTypeObject) {
                            recurse = false;
                            break;
                        }
                    }
                }
            }

            if (recurse) {
                if (array) {
                    if (!count) {
                        ret = std::vector<Variant>();
                    } else {
                        for (int i=count - 1; i>=0; --i) {
                            JSValueRef value = JSObjectGetPropertyAtIndex(ctx, object, i, 0);
                            ret[i] = JSValueToVariant_internal(ctx, value, exception, seen, recurseDepth == -1 ? -1 : recurseDepth - 1);
                        }
                    }
                } else {
                    if (!count) {
                        ret = std::map<std::string, Variant>();
                    } else {
                        for (size_t i=0; i<count; ++i) {
                            size_t size;
                            const JSChar* name = Netflix_JSPropertyNameArrayGetNameAtIndex(ctx, properties, i, &size);
                            JSValueRef value = Netflix_JSObjectGetProperty(ctx, object, properties, i, 0);
                            ret[toStdString(name, size)] = JSValueToVariant_internal(ctx, value, exception, seen, recurseDepth == -1 ? -1 : recurseDepth - 1);
                        }
                    }
                }
            } else {
                char buf[32];
                snprintf(buf, sizeof(buf), "(%s %zu)", array ? "Array" : "Object", count);
                ret = buf;
            }
            Netflix_JSPropertyNameArrayRelease(ctx, properties);
            seen.erase(object);
            return ret; }
        }
    }
    return Variant();
}

ProtectedValue VariantToJSValue(JSContextRef ctx, const Variant &variant, int &bytes, JSValueRef* exception)
{
    assert(ctx);
    bytes += sizeof(variant);
    switch (variant.type()) {
    case Variant::Type_Null:
        break;
    case Variant::Type_Pointer: {
        const std::string str = StringFormatter::sformat("ptr:%p", variant.pointer());
        bytes += str.length();
        return stringToJSValue(ctx, str); }
    case Variant::Type_Custom:
        assert(variant.custom());
        switch (variant.custom()->type) {
        case ValueCustom::Type: {
            JSValueRef ref = static_pointer_cast<const ValueCustom>(variant.custom())->takeValue();
            return ProtectedValue(ref, false); } // already protected
        case TypedArrayCustom::Type: {
            const shared_ptr<const TypedArrayCustom> custom = static_pointer_cast<const TypedArrayCustom>(variant.custom());
            return TypedArrayJSC::construct(custom->arrayType, ctx, custom->dataBuffer); }
        case StringCustom::Type: {
            const shared_ptr<const StringCustom> custom = static_pointer_cast<const StringCustom>(variant.custom());
            return stringToJSValue(ctx, custom->dataBuffer); }
        default:
            assert(0);
            break;
        }
        break;
    case Variant::Type_String: {
        const std::string str = variant.string();
        bytes += str.length();
        return stringToJSValue(ctx, str); }
    case Variant::Type_DataBuffer: {
        const DataBuffer buf = variant.dataBuffer();
        bytes += buf.size();
        return ArrayBufferJSC::construct(ctx, buf); }
    case Variant::Type_Integer: {
        JSValueRef ref = JSValueMakeNumber(ctx, variant.integer());
        return ref; }
    case Variant::Type_Double: {
        JSValueRef ref = JSValueMakeNumber(ctx, variant.dbl());
        return ref; }
    case Variant::Type_Boolean: {
        JSValueRef ref = JSValueMakeBoolean(ctx, variant.boolean());
        return ref; }
    case Variant::Type_IntegerMap: {
        assert(false);
        break; }
    case Variant::Type_Array: {
        const int size = variant.size();
        JSValueRef stack_args[64];
        JSValueRef *args = stack_args;
        if (size > 64)
            args = new JSValueRef[size];
        for (int i=0; i<size; ++i) {
            args[i] = VariantToJSValue(ctx, variant[i], bytes, exception);
            JSValueProtect(ctx, args[i]);
        }

        ProtectedValue array = JSObjectMakeArray(ctx, size, args, exception);
        for (int i=0; i<size; ++i) {
            JSValueUnprotect(ctx, args[i]);
        }

        if (args != stack_args)
            delete[] args;
        return array; }
    case Variant::Type_StringMap: {
        const Variant::StringMapIterator end = variant.stringMapEnd();
        ProtectedValue obj = JSObjectMake(ctx, 0, exception);
        for (Variant::StringMapIterator it = variant.stringMapBegin(); it != end; ++it) {
            const std::string &str = it->first;
            bytes += str.length();
            ScopedString name(Netflix_JSStringCreate(str.c_str(), str.size()));
            JSObjectSetProperty(ctx, obj, name, VariantToJSValue(ctx, it->second, bytes, exception),
                                kJSPropertyAttributeNone, 0);
        }
        return obj; }
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef stringToJSValue(JSContextRef ctx, const JSChar *str, int length)
{
    ScopedString string(JSStringCreateWithCharacters(str, length));
    return JSValueMakeString(ctx, string);
}

JSValueRef stringToJSValue(JSContextRef ctx, const std::vector<JSChar> &str)
{
    return stringToJSValue(ctx, &str[0], str.size());
}

JSValueRef stringToJSValue(JSContextRef ctx, const char *str, int length)
{
    ScopedString string(Netflix_JSStringCreate(str, length));
    return JSValueMakeString(ctx, string);
}

JSValueRef stringToJSValue(JSContextRef ctx, const std::string &str)
{
    return stringToJSValue(ctx, str.c_str(), str.length());
}

JSValueRef stringToJSValue(JSContextRef ctx, const DataBuffer &buffer)
{
    return stringToJSValue(ctx, buffer.c_str(), buffer.size());
}

static JSStringRef sGeneratedStrings[JSC::_GeneratedStringCount];

JSStringRef generatedString(GeneratedString string)
{
    return sGeneratedStrings[string];
}

void initGeneratedStrings()
{
#define MAKE_STRING(str) sGeneratedStrings[String_## str] = Netflix_JSStringCreate(#str, strlen(#str));
    MAKE_STRING(ClassSync);
    MAKE_STRING(Error);
    MAKE_STRING(Event);
    MAKE_STRING(GetProperty);
    MAKE_STRING(Method);
    MAKE_STRING(PropertyUpdate);
    MAKE_STRING(SetProperty);
    MAKE_STRING(data);
    MAKE_STRING(method);
    MAKE_STRING(name);
    MAKE_STRING(properties);
    MAKE_STRING(property);
    MAKE_STRING(returnValue);
    MAKE_STRING(time);
    MAKE_STRING(type);
    MAKE_STRING(value);
#undef MAKE_STRING
}

void cleanupGeneratedStrings()
{
    for(int i = 0; i < JSC::_GeneratedStringCount; ++i) {
        JSStringRelease(sGeneratedStrings[i]);
        sGeneratedStrings[i] = 0;
    }
}

std::string eatString(JSStringRef ref)
{
    assert(ref);
    size_t size = JSStringGetMaximumUTF8CStringSize(ref);
    std::string ret(size, '\0');
    if (size) {
        size = JSStringGetUTF8CString(ref, &ret[0], size);
        if (size)
            ret.resize(size - 1);
    }
    JSStringRelease(ref);
    return ret;
}

std::string toStdString(JSStringRef string)
{
    std::string str(JSStringGetMaximumUTF8CStringSize(string), '\0');
    const int s = JSStringGetUTF8CString(string, &str[0], str.size());
    str.resize(s - 1);
    return str;
}

std::string toStdString(const JSChar* chars, size_t size)
{
    std::string str(size * 3 + 1, '\0');
    size = Netflix_JSValueGetUTF8CString(chars, size, &str[0], str.size());
    if(size) {
        str.resize(size - 1);
        return str;
    }
    return std::string();
}

std::string toStdString(JSContextRef ctx, JSValueRef value)
{
    size_t size;
    const JSChar* chars = Netflix_JSValueGetString(ctx, value, &size);
    return toStdString(chars, size);
}

std::string JSValueToString(JSValueRef value)
{
    return toStdString(ScriptEngine::currentContext(), value);
}

bool DecodedData::decode(JSContextRef ctx, JSValueRef value, JSValueRef *exception)
{
    assert(type == Type_None);
    if (JSValueIsString(ctx, value)) {
        const JSChar *chars = Netflix_JSValueGetString(ctx, value, &length);
        decode(chars, length);
        type = Type_String;
    } else if (JSValueIsObject(ctx, value)) {
        JSObjectRef object = JSValueToObject(ctx, value, exception);
        assert(object);
        if (PrivateBase *priv = static_cast<PrivateBase*>(JSObjectGetPrivate(object))) {
            switch (priv->type) {
            case PrivateBase::Type_ArrayBuffer: {
                ArrayBufferJSC *buf = static_cast<ArrayBufferJSC*>(priv);
                assert(buf);
                dataBuffer = buf->mDataBuffer;
                length = buf->mDataBuffer.size();
                type = Type_ArrayBuffer;
                break; }
            case PrivateBase::Type_TypedArray: {
                TypedArrayJSC *array = static_cast<TypedArrayJSC*>(priv);
                assert(array);
                if (ArrayBufferJSC *buf = array->buffer()) {
                    dataBuffer = buf->mDataBuffer;
                    offset = array->mByteOffset;
                    length = array->mByteLength;
                    type = Type_TypedArray;
                }
                break; }
            default:
                break;
            }
        }
    }
    return type != Type_None;
}

void DecodedData::decode(JSStringRef string)
{
    decode(JSStringGetCharactersPtr(string), JSStringGetLength(string));
}

void DecodedData::decode(const JSChar *chars, size_t len)
{
    assert(type == Type_None);
    if (chars && len) {
        const size_t needed = (len * 3) + 1;
        char *ch;
        if (needed <= sizeof(staticBuf)) {
            ch = staticBuf;
        } else {
            dataBuffer.resize(needed);
            ch = dataBuffer.data<char>();
        }
        length = Netflix_JSValueGetUTF8CString(chars, len, ch, needed);
        if (!length) {
            dataBuffer.clear();
            type = Type_None;
            return;
        }
        type = Type_String;
        // length includes 0-termination
        ch[--length] = '\0';
        if (ch == staticBuf) {
            dataBuffer = DataBuffer::fromRawData(ch, length);
        } else {
            dataBuffer.resize(length);
        }
    }
}

DataBuffer DecodedData::view()
{
    if (!offset && static_cast<size_t>(dataBuffer.length()) == length)
        return dataBuffer;
    return DataBuffer::fromRawData(dataBuffer.data() + offset, length);
}

}
}
}


