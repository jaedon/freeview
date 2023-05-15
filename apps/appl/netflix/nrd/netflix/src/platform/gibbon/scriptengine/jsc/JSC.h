/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JSC_h
#define JSC_h

#include <nrdbase/Variant.h>
#include <nrdbase/Noncopyable.h>
#include <vector>
#include "ScriptEngine.h"
#include "GibbonApplication.h"

#include <API/JSValueRef.h>

typedef const struct OpaqueJSValue *JSValueRef;
typedef struct OpaqueJSValue *JSObjectRef;
typedef const struct OpaqueJSContext *JSContextRef;
typedef struct OpaqueJSString *JSStringRef;

# define GIBBON_JSC_API(x) <API/x>

// copied from JSStringRef.h
#if !defined(WIN32) && !defined(_WIN32)                                 \
    && !((defined(__CC_ARM) || defined(__ARMCC__)) && !defined(__linux__)) /* RVCT */
typedef unsigned short JSChar;
#else
typedef wchar_t JSChar;
#endif

#include GIBBON_JSC_API(JSStringRef.h)

namespace netflix {
namespace gibbon {
namespace JSC {

class PrivateBase
{
public:
    enum Type {
        Type_None = 0,
        Type_Finalize,
        Type_ArrayBuffer,
        Type_TypedArray,
        Type_DataView,
        Type_SyncObject
    };

    inline PrivateBase(unsigned int t) : type(t) { assert(t <= Type_SyncObject); }
    virtual ~PrivateBase() {}

    const unsigned int type;
};
class ProtectedValue
{
public:
    ProtectedValue();
    ProtectedValue(JSValueRef value, bool protect = true);
    ProtectedValue(const ProtectedValue &other);
    ~ProtectedValue();

    ProtectedValue &operator=(const ProtectedValue &other);
    void clear();
    bool isNull() const;
    JSValueRef value() const;
    JSObjectRef object() const;
    operator JSValueRef() { return value(); }
    operator JSObjectRef() { return object(); }
private:
    struct Data {
        JSValueRef value;
        int ref;
    } *data;
};

class ScopedString : public Noncopyable
{
public:
    ScopedString(const char *str, int length = -1);
    inline ScopedString(JSStringRef ref=0) : mString(ref) { }
    inline ~ScopedString() {
        if (mString)
            JSStringRelease(mString);
    }
    inline void assign(JSStringRef ref) {
        assert(!mString);
        mString = ref;
    }
    inline JSStringRef str() const { return mString; }
    operator JSStringRef() { return mString; }
    JSStringRef *operator&() { return &mString; }
private:
    JSStringRef mString;
};

class ValueCustom : public Variant::CustomData
{
public:
    enum { Type = 101 };
    enum CreationMode {
        Protect
    };
    ValueCustom(JSValueRef val);
    ValueCustom(CreationMode, JSValueRef val);
    virtual ~ValueCustom();
    virtual Variant toVariant() const;
    JSValueRef takeValue() const
    {
        JSValueRef ret = value;
        value = 0;
        return ret;
    }
private:
    mutable JSValueRef value;
};

class TypedArrayCustom : public Variant::CustomData
{
public:
    enum { Type = ValueCustom::Type + 1 };
    TypedArrayCustom(ScriptEngine::TypedArrayType type, const DataBuffer &buffer);
    virtual Variant toVariant() const;

    ScriptEngine::TypedArrayType arrayType;
    DataBuffer dataBuffer;
};

class StringCustom : public Variant::CustomData
{
public:
    enum { Type = TypedArrayCustom::Type + 1 };
    StringCustom(const DataBuffer &buffer);
    virtual Variant toVariant() const;

    DataBuffer dataBuffer;
};

class FinalizeObject : public PrivateBase
{
public:
    inline FinalizeObject() : PrivateBase(Type_Finalize) { }
    virtual ~FinalizeObject() { }
    virtual void finalize() = 0;
    static FinalizeObject *getFinalize(JSObjectRef object);
    static void finalize(JSObjectRef object);
};

class GCTagPrivate : public FinalizeObject
{
public:
    GCTagPrivate(const std::string &s);
    ~GCTagPrivate();
    virtual void finalize();

    uint64_t number;
    const std::string name;
    const uint64_t time;
};

std::string JSValueToString(JSValueRef value);
Variant JSValueToVariant_internal(JSContextRef ctx, JSValueRef value, JSValueRef* exception, std::set<JSObjectRef> &seen, int recurseDepth);
inline Variant JSValueToVariant(JSContextRef ctx, JSValueRef value, JSValueRef* exception = 0, int recurseDepth = -1)
{
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
        std::set<JSObjectRef> seen;
        return JSValueToVariant_internal(ctx, value, exception, seen, recurseDepth);  }
    }
    return Variant();
}
inline Variant JSValueToVariant(JSValueRef value, JSValueRef *exception = 0, int recurseDepth = -1)
{
    return JSValueToVariant(ScriptEngine::currentContext(), value, exception, recurseDepth);
}
ProtectedValue VariantToJSValue(JSContextRef ctx, const Variant &variant, int &bytes, JSValueRef *exception);
inline ProtectedValue VariantToJSValue(const Variant &variant, int &bytes, JSValueRef *exception)
{
    return VariantToJSValue(ScriptEngine::currentContext(), variant, bytes, exception);
}
inline ProtectedValue VariantToJSValue(const Variant &variant, JSValueRef* exception)
{
    int bytes;
    return VariantToJSValue(variant, bytes, exception);
}
inline ProtectedValue VariantToJSValue(JSContextRef ctx, const Variant &variant, JSValueRef* exception)
{
    int bytes;
    return VariantToJSValue(ctx, variant, bytes, exception);
}

JSValueRef stringToJSValue(JSContextRef ctx, const JSChar *str, int length);
JSValueRef stringToJSValue(JSContextRef ctx, const std::vector<JSChar> &str);
JSValueRef stringToJSValue(JSContextRef ctx, const char *str, int length);
JSValueRef stringToJSValue(JSContextRef ctx, const std::string &str);
JSValueRef stringToJSValue(JSContextRef ctx, const DataBuffer &buffer);
std::string toStdString(JSStringRef string);
std::string toStdString(const JSChar *chars, size_t size);
std::string toStdString(JSContextRef ctx, JSValueRef value);
std::string eatString(JSStringRef ref);

enum GeneratedString {
    String_ClassSync,
    String_Error,
    String_Event,
    String_GetProperty,
    String_Method,
    String_PropertyUpdate,
    String_SetProperty,
    String_data,
    String_method,
    String_name,
    String_properties,
    String_property,
    String_returnValue,
    String_time,
    String_type,
    String_value,
    _GeneratedStringCount
};
JSStringRef generatedString(GeneratedString string);
void cleanupGeneratedStrings();
void initGeneratedStrings();

struct DecodedData
{
    enum Type {
        Type_None,
        Type_ArrayBuffer,
        Type_String,
        Type_TypedArray
    };

    DecodedData()
        : type(Type_None), offset(0), length(0)
    {}

    bool decode(JSContextRef ctx, JSValueRef value, JSValueRef *exception = 0);
    void decode(JSStringRef string);

    Type type;
    int offset;
    size_t length;
    DataBuffer dataBuffer;

    DataBuffer view();
    char *data() { return dataBuffer.data<char>() + offset; }
private:
    void decode(const JSChar *chars, size_t length);
    char staticBuf[1024];
};

}}}

#endif
